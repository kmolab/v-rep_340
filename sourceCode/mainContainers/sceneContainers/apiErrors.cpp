// This file is part of V-REP, the Virtual Robot Experimentation Platform.
// 
// Copyright 2006-2017 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// V-REP is dual-licensed, under the terms of EITHER (at your option):
//   1. V-REP commercial license (contact us for details)
//   2. GNU GPL (see below)
// 
// GNU GPL license:
// -------------------------------------------------------------------
// V-REP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// V-REP IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with V-REP.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.4.0 rev. 1 on April 5th 2017

#include "vrepMainHeader.h"
#include "funcDebug.h"
#include "apiErrors.h"
#include "app.h"
#include "v_rep_internal.h"
#include "luaScriptFunctions.h"
#include "threadPool.h"
#include "v_repStrings.h"
#include <iostream>

std::vector<VTHREAD_ID_TYPE> CApiErrors::_controllerLocation_threadIds;
std::vector<std::vector<int> > CApiErrors::_controllerLocation_locationStack;

int CApiErrors::_c_gui_errorReportMode=sim_api_error_report;
int CApiErrors::_c_nonGui_errorReportMode=sim_api_error_report;

std::string CApiErrors::_c_gui_lastError=SIM_API_CALL_NO_ERROR;
std::string CApiErrors::_c_nonGui_lastError=SIM_API_CALL_NO_ERROR;

std::string CApiErrors::_cSideGeneratedLuaError;

CApiErrors::CApiErrors()
{
}

CApiErrors::~CApiErrors()
{
}

int CApiErrors::_getIndexFromCurrentThread()
{
    VTHREAD_ID_TYPE currentThreadId=VThread::getCurrentThreadId();
    for (int i=0;i<int(_controllerLocation_threadIds.size());i++)
    {
        if (VThread::areThreadIDsSame(currentThreadId,_controllerLocation_threadIds[i]))
            return(i);
    }
    return(-1); // Error, should never happen
}

bool CApiErrors::addNewThreadForErrorReporting(int scriptId_or_0ForCGui_or_1ForCNonGui)
{
    if (_getIndexFromCurrentThread()==-1)
    {
        _controllerLocation_threadIds.push_back(VThread::getCurrentThreadId());
        std::vector<int> tmp;
        _controllerLocation_locationStack.push_back(tmp);
        _controllerLocation_locationStack[_controllerLocation_locationStack.size()-1].push_back(scriptId_or_0ForCGui_or_1ForCNonGui);
        return(true);
    }
    return(false);
}

bool CApiErrors::removeThreadFromErrorReporting()
{
    int index=_getIndexFromCurrentThread();
    if (index!=-1)
    {
        _controllerLocation_threadIds.erase(_controllerLocation_threadIds.begin()+index);
        _controllerLocation_locationStack.erase(_controllerLocation_locationStack.begin()+index);
        return(true);
    }
    return(false);
}

void CApiErrors::pushLocation(int scriptId_or_0IfNoScript)
{
    int index=_getIndexFromCurrentThread();
    if (index!=-1)
    {
        if (scriptId_or_0IfNoScript==0)
        {
            if (VThread::isCurrentThreadTheUiThread())
                scriptId_or_0IfNoScript=0;
            else
                scriptId_or_0IfNoScript=1;
        }
        _controllerLocation_locationStack[index].push_back(scriptId_or_0IfNoScript);
    }
}

void CApiErrors::popLocation()
{
    int index=_getIndexFromCurrentThread();
    if (index!=-1)
        _controllerLocation_locationStack[index].pop_back();
}

int CApiErrors::_getCurrentLocation(bool onlyLuaLocation/*=false*/)
{
    int index=_getIndexFromCurrentThread();
    if (index>=0)
    {
        if (!onlyLuaLocation)
            return(_controllerLocation_locationStack[index][_controllerLocation_locationStack[index].size()-1]);
        // We arrive here if simSetLastError was called from C/C++: we basically want to hand the error to the last Lua script that called (i.e. called-back) the plugin
        for (int i=_controllerLocation_locationStack[index].size()-1;i>=0;i--)
        {
            int loc=_controllerLocation_locationStack[index][i];
            if (loc>1)
                return(loc); // ok, this is a Lua script!
        }
    }
    return(-1);
}

void CApiErrors::setApiCallErrorMessage(const char* functionName,const char* errMsg)
{
    std::string funcName(functionName);
    if (funcName.size()>9)
    {
        if (funcName.compare(funcName.size()-9,9,"_internal")==0)
            funcName.assign(funcName.begin(),funcName.end()-9);
    }

    std::string msg(errMsg);
    msg+=" ("+funcName+")";

    int loc=_getCurrentLocation(false);

    if (loc!=-1)
    {
        if (loc==0)
        { // C API, GUI thread
            if ((_c_gui_errorReportMode&(sim_api_error_report|sim_api_error_output))!=0)
                _c_gui_lastError=msg;
            if ((_c_gui_errorReportMode&sim_api_error_output)!=0)
            {
                std::string tmp=IDSNOTR_FUNCTION_CALL_ERROR_C_API_GUI+msg;
                std::cout << tmp.c_str() << std::endl;
            }
        }
        else if (loc==1)
        { // C API, Simulation thread
            if ((_c_nonGui_errorReportMode&(sim_api_error_report|sim_api_error_output))!=0)
                _c_nonGui_lastError=msg;
            if ((_c_nonGui_errorReportMode&sim_api_error_output)!=0)
            {
                std::string tmp=IDSNOTR_FUNCTION_CALL_ERROR_C_API_NONGUI+msg;
                std::cout << tmp.c_str() << std::endl;
            }
        }
        else if ((App::ct!=NULL)&&(App::ct->luaScriptContainer!=NULL))
        { // Lua API (or C API called-back from Lua, which calls 'simSetLastError' (typically a plugin that was called-back via a custom Lua function, and that needs to output an error to the GUI console))
            CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(loc);
            if (it!=NULL)
                _cSideGeneratedLuaError=errMsg;
        }
    }
}

void CApiErrors::clearCSideGeneratedLuaError()
{
    _cSideGeneratedLuaError.clear();
}

std::string CApiErrors::getCSideGeneratedLuaError()
{
    return(_cSideGeneratedLuaError);
}

void CApiErrors::decorateLuaErrorMessage(const char* functionName,std::string& errMsg)
{
    std::string funcName(functionName);
    errMsg+=" ("+funcName+")";
    errMsg=getLocationString()+errMsg;
}

void CApiErrors::setLuaCallErrorMessage(const char* functionName,const char* errMsg)
{ // call only directly from Lua
    std::string funcName(functionName);

    std::string msg(errMsg);
    msg+=" ("+funcName+")";

    int loc=_getCurrentLocation(true);
    if (loc>1)
    {
        if ((App::ct!=NULL)&&(App::ct->luaScriptContainer!=NULL))
        { // Lua API (or C API called-back from Lua, which calls 'simSetLastError' (typically a plugin that was called-back via a custom Lua function, and that needs to output an error to the GUI console))
            CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(loc);
            if (it!=NULL)
            {
                // Here we can react to errors:
                App::ct->simulation->pauseOnErrorRequested();

                msg=getLocationString()+msg;

                if ((it->getErrorReportMode()&(sim_api_error_report|sim_api_error_output))!=0)
                    it->setLastErrorString(msg.c_str());
                if ((it->getErrorReportMode()&sim_api_error_output)!=0)
                {
                    std::string tmp=IDSNOTR_FUNCTION_CALL_ERROR_LUA_API+msg;
                    App::addStatusbarMessage(tmp.c_str());
                    SUIThreadCommand cmdIn;
                    SUIThreadCommand cmdOut;
                    cmdIn.cmdId=FLASH_STATUSBAR_UITHREADCMD;
                    App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
                }
            }
        }
    }
}

void CApiErrors::setLuaCallErrorMessage_fromPlugin(const char* functionName,const char* errMsg)
{ // call only indirectly from Lua via a plugin callback
    int loc=_getCurrentLocation(true);
    if (loc<2)
    { // C API, GUI thread or simulation thread
        if (loc==0)
        { // C API, GUI thread
            if ((_c_gui_errorReportMode&(sim_api_error_report|sim_api_error_output))!=0)
                _c_gui_lastError=errMsg;
            if ((_c_gui_errorReportMode&sim_api_error_output)!=0)
            {
                std::string tmp=IDSNOTR_FUNCTION_CALL_ERROR_C_API_GUI;
                tmp+=errMsg;
                std::cout << tmp.c_str() << std::endl;
            }
        }
        else if (loc==1)
        { // C API, Simulation thread
            if ((_c_nonGui_errorReportMode&(sim_api_error_report|sim_api_error_output))!=0)
                _c_nonGui_lastError=errMsg;
            if ((_c_nonGui_errorReportMode&sim_api_error_output)!=0)
            {
                std::string tmp=IDSNOTR_FUNCTION_CALL_ERROR_C_API_NONGUI;
                tmp+=errMsg;
                std::cout << tmp.c_str() << std::endl;
            }
        }

        std::string funcName(functionName);
        std::string msg(errMsg);
        msg+=" ("+funcName+")";
        App::ct->simulation->pauseOnErrorRequested();
        msg=getLocationString()+msg;
        std::string tmp="Error: "+msg;
        App::addStatusbarMessage(tmp.c_str());
        SUIThreadCommand cmdIn;
        SUIThreadCommand cmdOut;
        cmdIn.cmdId=FLASH_STATUSBAR_UITHREADCMD;
        App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
    }
    else if ((App::ct!=NULL)&&(App::ct->luaScriptContainer!=NULL))
    { // Lua API (or C API called-back from Lua, which calls 'simSetLastError' (typically a plugin that was called-back via a custom Lua function, and that needs to output an error to the GUI console))
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(loc);
        if (it!=NULL)
            _cSideGeneratedLuaError=errMsg;
    }
}

void CApiErrors::clearApiCallErrorMessage()
{
    int loc=_getCurrentLocation();
    if (loc==0)
        _c_gui_lastError=SIM_API_CALL_NO_ERROR;
    else if (loc==1)
        _c_nonGui_lastError=SIM_API_CALL_NO_ERROR;
    else if ((App::ct!=NULL)&&(App::ct->luaScriptContainer!=NULL))
    {
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(loc);
        if (it!=NULL)
            it->setLastErrorString(SIM_API_CALL_NO_ERROR);
    }
}

std::string CApiErrors::getApiCallErrorMessage()
{
    int loc=_getCurrentLocation();
    if (loc==0)
        return(_c_gui_lastError);
    if (loc==1)
        return(_c_nonGui_lastError);
    if ((App::ct!=NULL)&&(App::ct->luaScriptContainer!=NULL))
    {
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(loc);
        if (it!=NULL)
            return(it->getLastErrorString());
    }
    return(SIM_API_CALL_NO_ERROR);
}

void CApiErrors::setApiCallErrorReportMode(int mode)
{
    int loc=_getCurrentLocation();
    if (loc==0)
        _c_gui_errorReportMode=mode;
    else if (loc==1)
        _c_nonGui_errorReportMode=mode;
    else if ((App::ct!=NULL)&&(App::ct->luaScriptContainer!=NULL))
    {
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(loc);
        if (it!=NULL)
            it->setErrorReportMode(mode);
    }
}

int CApiErrors::getApiCallErrorReportMode()
{
    int loc=_getCurrentLocation();
    if (loc==0)
        return(_c_gui_errorReportMode);
    if (loc==1)
        return(_c_nonGui_errorReportMode);
    if ((App::ct!=NULL)&&(App::ct->luaScriptContainer!=NULL))
    {
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(loc);
        if (it!=NULL)
            return(it->getErrorReportMode());
    }
    return(0);
}



void CApiErrors::setApiCallWarningMessage(const char* functionName,const char* warnMsg)
{
    std::string funcName(functionName);
    if (funcName.size()>9)
    {
        if (funcName.compare(funcName.size()-9,9,"_internal")==0)
            funcName.assign(funcName.begin(),funcName.end()-9);
    }

    std::string msg(warnMsg);
    msg+=" ("+funcName+")";

    int loc=_getCurrentLocation();
    if (loc==0)
    {
        if ((_c_gui_errorReportMode&sim_api_warning_output)!=0)
        {
// Not yet supported!
//          std::string tmp=IDSNOTR_FUNCTION_CALL_ERROR_C_API_GUI+_c_gui_lastError;
//          std::cout << tmp.c_str() << std::endl;
        }
    }
    else if (loc==1)
    {
        if ((_c_nonGui_errorReportMode&sim_api_warning_output)!=0)
        {
            // Not yet supported!
//          std::string tmp=IDSNOTR_FUNCTION_CALL_ERROR_C_API_NONGUI+_c_nonGui_lastError;
//          std::cout << tmp.c_str() << std::endl;
        }
    }
    else if ((App::ct!=NULL)&&(App::ct->luaScriptContainer!=NULL))
    {
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(loc);
        if (it!=NULL)
        {
            if ((it->getErrorReportMode()&sim_api_warning_output)!=0)
            {
                std::string msg=getLocationString()+warnMsg;
                std::string tmp=std::string(IDSNOTR_FUNCTION_CALL_WARNING_LUA_API)+msg;
                App::addStatusbarMessage(tmp.c_str());
            }
        }
    }
}

void CApiErrors::clearApiCallWarningMessage()
{
    // not yet supported
}

std::string CApiErrors::getApiCallWarningMessage()
{
    // not yet supported
    return("");
}









