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
#include "v_rep_internal.h"
#include "luaScriptFunctions.h"
#include "tt.h"
#include "threadPool.h"
#include "linMotionRoutines.h"
#include "pluginContainer.h"
#include <boost/lexical_cast.hpp>
#include "vVarious.h"
#include "vDateTime.h"
#include "app.h"
#include "apiErrors.h"
#include "interfaceStack.h"

#define LUA_START(funcName) \
    CApiErrors::clearCSideGeneratedLuaError(); \
    memorizeLocation(L); \
    std::string functionName(funcName); \
    std::string errorString;

#define LUA_END(p) \
    if (true) \
    { \
        forgetLocation(); \
        return(p); \
    } \
    else

#define LUA_SET_OR_RAISE_ERROR() \
if (_hasErrors(errorString)) \
{ \
    if (App::userSettings->raiseErrorWithApiScriptFunctions) \
    { \
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L)); \
        if ((it!=NULL)&&((it->getErrorReportMode()&(sim_api_error_report|sim_api_error_output))!=0)) \
        { \
            CApiErrors::decorateLuaErrorMessage(functionName.c_str(),errorString); \
            forgetLocation(); \
            luaWrap_lua_pushstring(L,errorString.c_str()); \
            luaWrap_lua_error(L); \
        } \
    } \
    else \
    { \
        CApiErrors::setLuaCallErrorMessage(functionName.c_str(),errorString.c_str()); \
    } \
}

#define SIM_SCRIPT_NAME_SUFFIX "sim_script_name_suffix"

std::vector<std::string> locations;
std::vector<VTHREAD_ID_TYPE> threadIDs;
VMutex memorizeLocationMutex;

std::vector<int> serialPortHandles;
std::vector<std::string> serialPortLeftOverData;

void insertSerialPortLeftOver(int portHandle,std::string leftOver)
{
    serialPortHandles.push_back(portHandle);
    serialPortLeftOverData.push_back(leftOver);
}

std::string retrieveSerialPortLeftOver(int portHandle)
{
    std::string leftOver;
    for (int i=0;i<int(serialPortHandles.size());i++)
    {
        if (serialPortHandles[i]==portHandle)
        {
            serialPortHandles.erase(serialPortHandles.begin()+i);
            leftOver=serialPortLeftOverData[i];
            serialPortLeftOverData.erase(serialPortLeftOverData.begin()+i);
            break;
        }
    }
    return(leftOver);
}

void clearSerialPortLeftOver(int portHandle)
{
    for (int i=0;i<int(serialPortHandles.size());i++)
    {
        if (serialPortHandles[i]==portHandle)
        {
            serialPortHandles.erase(serialPortHandles.begin()+i);
            serialPortLeftOverData.erase(serialPortLeftOverData.begin()+i);
            break;
        }
    }
}

bool isObjectAssociatedWithThisThreadedChildScriptValid(luaWrap_lua_State* L)
{
    int id=getCurrentScriptID(L);
    CLuaScriptObject* script=App::ct->luaScriptContainer->getScriptFromID(id);
    if (script==NULL)
        return(false);
    int h=script->getObjectIDThatScriptIsAttachedTo_child();
    return(h!=-1);
}

void memorizeLocation(luaWrap_lua_State* L)
{
    memorizeLocationMutex.lock();
    VTHREAD_ID_TYPE threadID=VThread::getCurrentThreadId();
    int index=getLocationIndex(threadID);
    if (index==-1)
    {
        locations.push_back("");
        threadIDs.push_back(threadID);
        index=getLocationIndex(threadID);
    }

    std::string scriptName("");
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
    if (it!=NULL)
        scriptName=it->getShortDescriptiveName();
    int lineNumber=-1;
    lineNumber=luaWrap_getCurrentCodeLine(L);
    std::string msg("[string \"");
    msg+=scriptName+"\"]:";
    msg+=boost::lexical_cast<std::string>(lineNumber);
    msg+=": ";
    locations[index]=msg;
    memorizeLocationMutex.unlock();
}

std::string getLocationString()
{
    VTHREAD_ID_TYPE threadID=VThread::getCurrentThreadId();
    int index=getLocationIndex(threadID);
    if (index==-1)
        return("[string -unknown location]:?: "); // Should normally not happen!
    return(locations[index]);
}

int getLocationIndex(VTHREAD_ID_TYPE threadID)
{
    for (int i=0;i<int(threadIDs.size());i++)
    {
        if (VThread::areThreadIDsSame(threadID,threadIDs[i]))
            return(i);
    }
    return(-1);
}

void forgetLocation()
{
    memorizeLocationMutex.lock();
    VTHREAD_ID_TYPE threadID=VThread::getCurrentThreadId();
    int index=getLocationIndex(threadID);
    if (index!=-1)
    { // should always be the case!
        locations.erase(locations.begin()+index);
        threadIDs.erase(threadIDs.begin()+index);
    }
    memorizeLocationMutex.unlock();
}

void pushCorrectTypeOntoLuaStack(luaWrap_lua_State* L,const std::string& txt)
{ // Pushes nil, false, true, number or string (in that order!!) onto the stack depending on the txt content!
    int t=getCorrectType(txt);
    if (t==0)
        luaWrap_lua_pushnil(L);
    if (t==1)
        luaWrap_lua_pushboolean(L,0);
    if (t==2)
        luaWrap_lua_pushboolean(L,1);
    if (t==3)
    {
        float floatVal;
        tt::getValidFloat(txt,floatVal);
        luaWrap_lua_pushnumber(L,floatVal);
    }
    if (t==4)
        luaWrap_lua_pushlstring(L,txt.c_str(),txt.length());
}

int getCorrectType(const std::string& txt)
{ // returns 0=nil, 1=boolean false, 2=boolean true, 3=number or 4=string (in that order!!) depending on the txt content!
    if (txt.length()!=0)
    {
        if (txt.length()!=strlen(txt.c_str()))
            return(4); // We have embedded zeros, this has definitively to be a string:
    }
    if (strcmp(txt.c_str(),"nil")==0)
        return(0);
    if (strcmp(txt.c_str(),"false")==0)
        return(1);
    if (strcmp(txt.c_str(),"true")==0)
        return(2);
    float floatVal;
    if (tt::getValidFloat(txt,floatVal))
        return(3);
    return(4);
}

void getScriptTree(luaWrap_lua_State* L,bool selfIncluded,std::vector<int>& scriptHandles)
{ // Returns all scripts that are built under the current one
    scriptHandles.clear();
    int currentScriptID=getCurrentScriptID(L);

    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);

    if (it->getScriptType()==sim_scripttype_mainscript)
    { // we have a main script here
        if (selfIncluded)
            scriptHandles.push_back(currentScriptID);
        for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
        {
            C3DObject* q=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
            CLuaScriptObject* lso=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(q->getID());
            if (lso!=NULL)
                scriptHandles.push_back(lso->getScriptID());
        }
    }

    if (it->getScriptType()==sim_scripttype_childscript)
    { // we have a child script
        C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo_child());
        if (obj!=NULL)
        { // should always pass
            if (selfIncluded)
                scriptHandles.push_back(currentScriptID);

            std::vector<C3DObject*> objList;
            obj->getAllObjectsRecursive(&objList,false);
            for (int i=0;i<int(objList.size());i++)
            {
                CLuaScriptObject* lso=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(objList[i]->getID());
                if (lso!=NULL)
                    scriptHandles.push_back(lso->getScriptID());
            }
        }
    }

    if (it->getScriptType()==sim_scripttype_jointctrlcallback)
    { // we have a joint ctrl callback script
        C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo_callback());
        if (obj!=NULL)
        { // should always pass
            if (selfIncluded)
            {
                CLuaScriptObject* aScript=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(obj->getID());
                if (aScript!=NULL)
                    scriptHandles.push_back(aScript->getScriptID());
            }

            std::vector<C3DObject*> objList;
            obj->getAllObjectsRecursive(&objList,false);
            for (int i=0;i<int(objList.size());i++)
            {
                CLuaScriptObject* lso=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(objList[i]->getID());
                if (lso!=NULL)
                    scriptHandles.push_back(lso->getScriptID());
            }
        }
    }

    if (it->getScriptType()==sim_scripttype_customizationscript)
    { // we have a customization script
        C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo_customization());
        if (obj!=NULL)
        { // should always pass
            if (selfIncluded)
            {
                CLuaScriptObject* aScript=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(obj->getID());
                if (aScript!=NULL)
                    scriptHandles.push_back(aScript->getScriptID());
            }

            std::vector<C3DObject*> objList;
            obj->getAllObjectsRecursive(&objList,false);
            for (int i=0;i<int(objList.size());i++)
            {
                CLuaScriptObject* lso=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(objList[i]->getID());
                if (lso!=NULL)
                {
                    scriptHandles.push_back(lso->getScriptID());
                }
            }
        }
    }

}

void getScriptChain(luaWrap_lua_State* L,bool selfIncluded,bool mainIncluded,std::vector<int>& scriptHandles)
{ // Returns all script IDs that are parents (or grand-parents,grand-grand-parents, etc.) of the current one
    scriptHandles.clear();
    int currentScriptID=getCurrentScriptID(L);

    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);

    if (it->getScriptType()==sim_scripttype_mainscript)
    { // we have a main script here
        if (selfIncluded&&mainIncluded)
            scriptHandles.push_back(currentScriptID);
    }

    if (it->getScriptType()==sim_scripttype_childscript)
    { // we have a child script here
        C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo_child());
        if (obj!=NULL)
        {
            if (selfIncluded)
                scriptHandles.push_back(currentScriptID);
            while (obj->getParent()!=NULL)
            {
                obj=obj->getParent();
                CLuaScriptObject* lso=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(obj->getID());
                if (lso!=NULL)
                    scriptHandles.push_back(lso->getScriptID());
            }
            if (mainIncluded)
            {
                CLuaScriptObject* lso=App::ct->luaScriptContainer->getMainScript();
                if (lso!=NULL)
                    scriptHandles.push_back(lso->getScriptID());
            }
        }
    }

    if (it->getScriptType()==sim_scripttype_jointctrlcallback)
    { // we have a joint callback script here
        C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo_callback());
        if (obj!=NULL)
        {
            if (selfIncluded)
            {
                CLuaScriptObject* aScript=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(obj->getID());
                if (aScript!=NULL)
                    scriptHandles.push_back(aScript->getScriptID());
            }
            while (obj->getParent()!=NULL)
            {
                obj=obj->getParent();
                CLuaScriptObject* lso=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(obj->getID());
                if (lso!=NULL)
                    scriptHandles.push_back(lso->getScriptID());
            }
            if (mainIncluded)
            {
                CLuaScriptObject* lso=App::ct->luaScriptContainer->getMainScript();
                if (lso!=NULL)
                    scriptHandles.push_back(lso->getScriptID());
            }
        }
    }

    if (it->getScriptType()==sim_scripttype_customizationscript)
    { // we have a customization script here
        C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo_customization());
        if (obj!=NULL)
        {
            if (selfIncluded)
            {
                CLuaScriptObject* aScript=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(obj->getID());
                if (aScript!=NULL)
                    scriptHandles.push_back(aScript->getScriptID());
            }
            while (obj->getParent()!=NULL)
            {
                obj=obj->getParent();
                CLuaScriptObject* lso=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(obj->getID());
                if (lso!=NULL)
                    scriptHandles.push_back(lso->getScriptID());
            }
            if (mainIncluded)
            {
                CLuaScriptObject* lso=App::ct->luaScriptContainer->getMainScript();
                if (lso!=NULL)
                    scriptHandles.push_back(lso->getScriptID());
            }
        }
    }

}

luaWrap_lua_State* initializeNewLuaState(const char* scriptSuffixNumberString)
{
    luaWrap_lua_State* L=luaWrap_luaL_newstate();
    luaWrap_luaL_openlibs(L);
    registerNewLuaFunctions(L);
    prepareNewLuaVariables(L,scriptSuffixNumberString);
    luaWrap_lua_sethook(L,luaHookFunction,luaWrapGet_LUA_MASKCOUNT(),100); // This instruction gets also called in luaHookFunction!!!!

    // --------------------------------------------
    // append some paths to the Lua path variable:
    luaWrap_lua_getglobal(L,"package");
    luaWrap_lua_getfield(L,-1,"path");
    std::string cur_path=luaWrap_lua_tostring(L,-1);
    cur_path+=";";
    cur_path+=App::directories->executableDirectory;
#ifdef MAC_VREP
    // We are inside of the package!!!
    cur_path+="/../../../lua/?.lua";
#else
    cur_path+="/lua/?.lua";
#endif
    cur_path+=";";
    cur_path+=App::directories->executableDirectory;
#ifdef MAC_VREP
    // We are inside of the package!!!
    cur_path+="/../../../BlueWorkforce/?.lua";
#else
    cur_path+="/BlueWorkforce/?.lua";
#endif
    if (App::ct->mainSettings->getScenePathAndName().compare("")!=0)
    {
        cur_path+=";";
        cur_path+=App::ct->mainSettings->getScenePath();
        cur_path+="/?.lua";
    }
    if (App::userSettings->additionalLuaPath.length()>0)
    {
        cur_path+=";";
        cur_path+=App::userSettings->additionalLuaPath;
        cur_path+="/?.lua";
    }
    luaWrap_lua_pop(L,1);
    luaWrap_lua_pushstring(L,cur_path.c_str());
    luaWrap_lua_setfield(L,-2,"path");
    luaWrap_lua_pop(L,1);
    // --------------------------------------------

    return(L);
}

void registerNewLuaFunctions(luaWrap_lua_State* L)
{
    for (int i=0;simLuaCommands[i].name!="";i++)
        luaWrap_lua_register(L,simLuaCommands[i].name.c_str(),simLuaCommands[i].func);
    App::ct->luaCustomFuncAndVarContainer->registerCustomLuaFunctions(L,_simGenericFunctionHandler);
}

void luaHookFunction(luaWrap_lua_State* L,luaWrap_lua_Debug* ar)
{
    FUNCTION_DEBUG;
    // Following 2 instructions are important: it can happen that the user locks/unlocks automatic thread switch in a loop,
    // and that the hook function by malchance only gets called when the thread switches are not allowed (due to the loop
    // timing and hook call timing overlap) --> this thread doesn't switch and stays in a lua loop forever.
    // To avoid this we add some random component to the hook timing:
    int randComponent=rand()/(RAND_MAX/10);
    luaWrap_lua_sethook(L,luaHookFunction,luaWrapGet_LUA_MASKCOUNT(),95+randComponent);
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
    int scriptType=it->getScriptType();
    if ( (scriptType==sim_scripttype_mainscript)||(scriptType==sim_scripttype_childscript)||(scriptType==sim_scripttype_jointctrlcallback)||(scriptType==sim_scripttype_contactcallback) ) //||(scriptType==sim_scripttype_generalcallback) )
    {
#ifdef SIM_WITH_GUI
        if (App::userSettings->abortScriptExecutionButton!=0)
        {
            bool doIt=( (App::ct->luaScriptContainer->getMainScriptExecTimeInMs()>(App::userSettings->abortScriptExecutionButton*1000))&&App::ct->luaScriptContainer->getInMainScriptNow() );
            App::ct->simulation->showAndHandleEmergencyStopButton(doIt,it->getShortDescriptiveName().c_str());
        }

        if ( CThreadPool::getSimulationEmergencyStop() ) // No automatic yield when flagged for destruction!! ||it->getFlaggedForDestruction() )
        { // This is the only way a non-threaded script can yield. But threaded child scripts can also yield here
            luaWrap_lua_yield(L,0);
            return;
        }
        else
        {
            if (CThreadPool::getSimulationStopRequestedAndActivated())
            { // returns true only after 1-2 seconds after the request arrived
                if (!VThread::isCurrentThreadTheMainSimulationThread())
                { // Here only threaded scripts can yield!
                    luaWrap_lua_yield(L,0);
                    return;
                }
            }
        }
#endif
        CThreadPool::switchBackToPreviousThreadIfNeeded();
    }
    else
    { // non-simulation scripts (i.e. add-ons and customization scripts)
#ifdef SIM_WITH_GUI
        if (App::userSettings->abortScriptExecutionButton!=0)
        {
            if ( it->getScriptExecutionTimeInMs()>(App::userSettings->abortScriptExecutionButton*1000) )
            {
                App::ct->simulation->showAndHandleEmergencyStopButton(true,it->getShortDescriptiveName().c_str());
                if (CLuaScriptObject::emergencyStopButtonPressed)
                {
                    CLuaScriptObject::emergencyStopButtonPressed=false;
                    if (it->getScriptType()==sim_scripttype_customizationscript)
                        it->setCustomizationScriptIsTemporarilyDisabled(true); // stop it
                    if (it->getScriptType()==sim_scripttype_addonscript)
                        it->setAddOnDesiredExecutionState(0); // stop it
                    luaWrap_lua_yield(L,0);
                }
            }
            else
                App::ct->simulation->showAndHandleEmergencyStopButton(false,"");
        }
#endif
    }
}

void prepareNewLuaVariables(luaWrap_lua_State* L,const char* scriptSuffixNumberString)
{
    for (int i=0;simLuaVariables[i].name!="";i++)
    {
        setNewLuaVariable(L,simLuaVariables[i].name.c_str(),simLuaVariables[i].val);
    }
    App::ct->luaCustomFuncAndVarContainer->assignCustomVariables(L);

    // Here we have the name prefix thing:
    std::string tmp(SIM_SCRIPT_NAME_SUFFIX);
    tmp+="='";
    tmp+=scriptSuffixNumberString;
    tmp+="'";
    luaWrap_luaL_dostring(L,tmp.c_str());
}

void setNewLuaVariable(luaWrap_lua_State* L,const char* name,int identifier)
{
    std::string tmp(name);
    tmp+="="+boost::lexical_cast<std::string>(identifier);
    luaWrap_luaL_dostring(L,tmp.c_str());
}

int getCurrentScriptID(luaWrap_lua_State* L)
{
    luaWrap_lua_getglobal(L,"sim_current_script_id");
    int retVal=-1;
    if (luaWrap_lua_isnumber(L,-1))
        retVal=luaWrap_lua_tointeger(L,-1);
    luaWrap_lua_pop(L,1);
    return(retVal);
}

bool readCustomFunctionDataFromStack(luaWrap_lua_State* L,int ind,int dataType,
                                     std::vector<char>& inBoolVector,
                                     std::vector<int>& inIntVector,
                                     std::vector<float>& inFloatVector,
                                     std::vector<double>& inDoubleVector,
                                     std::vector<std::string>& inStringVector,
                                     std::vector<std::string>& inCharVector,
                                     std::vector<int>& inInfoVector)
{ // return value false means there is no more data on the stack
    if (luaWrap_lua_gettop(L)<ind)
        return(false); // not enough data on the stack
    inInfoVector.push_back(sim_script_arg_invalid); // Dummy value for type
    inInfoVector.push_back(0);                      // dummy value for size
    if (luaWrap_lua_isnil(L,ind))
    { // Special case: nil should not generate a sim_script_arg_invalid type!
        inInfoVector[inInfoVector.size()-2]=sim_script_arg_null;
        return(true);
    }
    if (dataType&sim_script_arg_table)
    { // we have to read a table:
        dataType^=sim_script_arg_table;
        if (!luaWrap_lua_istable(L,ind))
            return(true); // this is not a table
        int dataSize=int(luaWrap_lua_objlen(L,ind));
        std::vector<char> boolV;
        std::vector<int> intV;
        std::vector<float> floatV;
        std::vector<double> doubleV;
        std::vector<std::string> stringV;
        for (int i=0;i<dataSize;i++)
        {
            luaWrap_lua_rawgeti(L,ind,i+1);
            if (dataType==sim_script_arg_bool)
            {
                if (!luaWrap_lua_isboolean(L,-1))
                {
                    luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                    return(true); // we don't have the correct data type
                }
                boolV.push_back(luaToBool(L,-1));
            }
            else if (dataType==sim_script_arg_int32)
            {
                if (!luaWrap_lua_isnumber(L,-1))
                {
                    luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                    return(true); // we don't have the correct data type
                }
                intV.push_back(luaToInt(L,-1));
            }
            else if (dataType==sim_script_arg_float)
            {
                if (!luaWrap_lua_isnumber(L,-1))
                {
                    luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                    return(true); // we don't have the correct data type
                }
                floatV.push_back(luaToFloat(L,-1));
            }
            else if (dataType==sim_script_arg_double)
            {
                if (!luaWrap_lua_isnumber(L,-1))
                {
                    luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                    return(true); // we don't have the correct data type
                }
                doubleV.push_back(luaToDouble(L,-1));
            }
            else if (dataType==sim_script_arg_string)
            {
                if (!luaWrap_lua_isstring(L,-1))
                {
                    luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                    return(true); // we don't have the correct data type
                }
                stringV.push_back(std::string(luaWrap_lua_tostring(L,-1)));
            }
            else
            {
                luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                return(true); // data type not recognized!
            }
            luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
        }
        // All values in the tables passed the check!
        for (int i=0;i<int(boolV.size());i++)
            inBoolVector.push_back(boolV[i]);
        for (int i=0;i<int(intV.size());i++)
            inIntVector.push_back(intV[i]);
        for (int i=0;i<int(floatV.size());i++)
            inFloatVector.push_back(floatV[i]);
        for (int i=0;i<int(doubleV.size());i++)
            inDoubleVector.push_back(doubleV[i]);
        for (int i=0;i<int(stringV.size());i++)
            inStringVector.push_back(stringV[i]);
        inInfoVector[inInfoVector.size()-1]=dataSize; // Size of the table
        inInfoVector[inInfoVector.size()-2]=dataType|sim_script_arg_table; // Type
        return(true);
    }
    else
    { // we have simple data here (not a table)
        if (dataType==sim_script_arg_bool)
        {
            if (!luaWrap_lua_isboolean(L,ind))
                return(true); // we don't have the correct data type
            inBoolVector.push_back(luaToBool(L,ind));
            inInfoVector[inInfoVector.size()-2]=dataType;
        }
        else if (dataType==sim_script_arg_int32)
        {
            if (!luaWrap_lua_isnumber(L,ind))
                return(true); // we don't have the correct data type
            inIntVector.push_back(luaToInt(L,ind));
            inInfoVector[inInfoVector.size()-2]=dataType;
        }
        else if (dataType==sim_script_arg_float)
        {
            if (!luaWrap_lua_isnumber(L,ind))
                return(true); // we don't have the correct data type
            inFloatVector.push_back(luaToFloat(L,ind));
            inInfoVector[inInfoVector.size()-2]=dataType;
        }
        else if (dataType==sim_script_arg_double)
        {
            if (!luaWrap_lua_isnumber(L,ind))
                return(true); // we don't have the correct data type
            inDoubleVector.push_back(luaToDouble(L,ind));
            inInfoVector[inInfoVector.size()-2]=dataType;
        }
        else if (dataType==sim_script_arg_string)
        {
            if (!luaWrap_lua_isstring(L,ind))
                return(true); // we don't have the correct data type
            inStringVector.push_back(std::string(luaWrap_lua_tostring(L,ind)));
            inInfoVector[inInfoVector.size()-2]=dataType;
        }
        else if (dataType==sim_script_arg_charbuff)
        {
            if (!luaWrap_lua_isstring(L,ind))
                return(true); // we don't have the correct data type
            size_t dataLength;
            char* data=(char*)luaWrap_lua_tolstring(L,ind,&dataLength);
            inCharVector.push_back(std::string(data,dataLength));
            inInfoVector[inInfoVector.size()-2]=dataType;
            inInfoVector[inInfoVector.size()-1]=int(dataLength);
        }
        return(true); // data type not recognized!
    }
    return(true);
}

void writeCustomFunctionDataOntoStack(luaWrap_lua_State* L,int dataType,int dataSize,
                                      unsigned char* boolData,int& boolDataPos,
                                      int* intData,int& intDataPos,
                                      float* floatData,int& floatDataPos,
                                      double* doubleData,int& doubleDataPos,
                                      char* stringData,int& stringDataPos,
                                      char* charData,int& charDataPos)
{
    if (((dataType|sim_script_arg_table)-sim_script_arg_table)==sim_script_arg_charbuff)
    { // special handling here
        luaWrap_lua_pushlstring(L,charData+charDataPos,dataSize);
        charDataPos+=dataSize;
    }
    else
    {
        int newTablePos=0;
        bool weHaveATable=false;
        if (dataType&sim_script_arg_table)
        { // we have a table
            luaWrap_lua_newtable(L);
            newTablePos=luaWrap_lua_gettop(L);
            dataType^=sim_script_arg_table;
            weHaveATable=true;
        }
        else
            dataSize=1;
        for (int i=0;i<dataSize;i++)
        {
            if (dataType==sim_script_arg_bool)
                luaWrap_lua_pushboolean(L,boolData[boolDataPos++]);
            else if (dataType==sim_script_arg_int32)
                luaWrap_lua_pushinteger(L,intData[intDataPos++]);
            else if (dataType==sim_script_arg_float)
                luaWrap_lua_pushnumber(L,floatData[floatDataPos++]);
            else if (dataType==sim_script_arg_double)
                luaWrap_lua_pushnumber(L,doubleData[doubleDataPos++]);
            else if (dataType==sim_script_arg_string)
            {
                luaWrap_lua_pushstring(L,stringData+stringDataPos);
                stringDataPos+=strlen(stringData+stringDataPos)+1; // Thanks to Ulrich Schwesinger for noticing a bug here!
            }
            else
                luaWrap_lua_pushnil(L); // that is an error!

            if (weHaveATable) // that's when we have a table
                luaWrap_lua_rawseti(L,newTablePos,i+1);
        }
    }
}

void getFloatsFromTable(luaWrap_lua_State* L,int tablePos,int floatCount,float* arrayField)
{
    for (int i=0;i<floatCount;i++)
    {
        luaWrap_lua_rawgeti(L,tablePos,i+1);
        arrayField[i]=luaToFloat(L,-1);
        luaWrap_lua_pop(L,1); // we pop one element from the stack;
    }
}

void getDoublesFromTable(luaWrap_lua_State* L,int tablePos,int doubleCount,double* arrayField)
{
    for (int i=0;i<doubleCount;i++)
    {
        luaWrap_lua_rawgeti(L,tablePos,i+1);
        arrayField[i]=luaWrap_lua_tonumber(L,-1);
        luaWrap_lua_pop(L,1); // we pop one element from the stack;
    }
}

bool getIntsFromTable(luaWrap_lua_State* L,int tablePos,int intCount,int* arrayField)
{
    for (int i=0;i<intCount;i++)
    {
        luaWrap_lua_rawgeti(L,tablePos,i+1);
        if (!luaWrap_lua_isnumber(L,-1))
        {
            luaWrap_lua_pop(L,1); // we pop one element from the stack;
            return(false); // Not a number!!
        }
        arrayField[i]=luaToInt(L,-1);
        luaWrap_lua_pop(L,1); // we pop one element from the stack;
    }
    return(true);
}

bool getUIntsFromTable(luaWrap_lua_State* L,int tablePos,int intCount,unsigned int* arrayField)
{
    for (int i=0;i<intCount;i++)
    {
        luaWrap_lua_rawgeti(L,tablePos,i+1);
        if (!luaWrap_lua_isnumber(L,-1))
        {
            luaWrap_lua_pop(L,1); // we pop one element from the stack;
            return(false); // Not a number!!
        }
        luaWrap_lua_Number na=luaWrap_lua_tonumber(L,-1);
        if (na<0.0)
            na=0.0;
        arrayField[i]=(unsigned int)na;
        luaWrap_lua_pop(L,1); // we pop one element from the stack;
    }
    return(true);
}

bool getUCharsFromTable(luaWrap_lua_State* L,int tablePos,int intCount,unsigned char* arrayField)
{
    for (int i=0;i<intCount;i++)
    {
        luaWrap_lua_rawgeti(L,tablePos,i+1);
        if (!luaWrap_lua_isnumber(L,-1))
        {
            luaWrap_lua_pop(L,1); // we pop one element from the stack;
            return(false); // Not a number!!
        }
        luaWrap_lua_Number na=luaWrap_lua_tonumber(L,-1);
        if (na<0.0)
            na=0.0;
        arrayField[i]=(unsigned char)na;
        luaWrap_lua_pop(L,1); // we pop one element from the stack;
    }
    return(true);
}

void getBoolsFromTable(luaWrap_lua_State* L,int tablePos,int boolCount,char* arrayField)
{
    for (int i=0;i<boolCount;i++)
    {
        luaWrap_lua_rawgeti(L,tablePos,i+1);
        arrayField[i]=(char)luaWrap_lua_toboolean(L,-1);
        luaWrap_lua_pop(L,1); // we pop one element from the stack;
    }
}

void pushFloatTableOntoStack(luaWrap_lua_State* L,int floatCount,const float* arrayField)
{
    luaWrap_lua_newtable(L);
    int newTablePos=luaWrap_lua_gettop(L);
    for (int i=0;i<floatCount;i++)
    {
        luaWrap_lua_pushnumber(L,arrayField[i]);
        luaWrap_lua_rawseti(L,newTablePos,i+1);
    }
}

void pushDoubleTableOntoStack(luaWrap_lua_State* L,int doubleCount,const double* arrayField)
{
    luaWrap_lua_newtable(L);
    int newTablePos=luaWrap_lua_gettop(L);
    for (int i=0;i<doubleCount;i++)
    {
        luaWrap_lua_pushnumber(L,arrayField[i]);
        luaWrap_lua_rawseti(L,newTablePos,i+1);
    }
}

void pushIntTableOntoStack(luaWrap_lua_State* L,int intCount,const int* arrayField)
{
    luaWrap_lua_newtable(L);
    int newTablePos=luaWrap_lua_gettop(L);
    for (int i=0;i<intCount;i++)
    {
        luaWrap_lua_pushnumber(L,arrayField[i]);
        luaWrap_lua_rawseti(L,newTablePos,i+1);
    }
}

void pushUIntTableOntoStack(luaWrap_lua_State* L,int intCount,const unsigned int* arrayField)
{
    luaWrap_lua_newtable(L);
    int newTablePos=luaWrap_lua_gettop(L);
    for (int i=0;i<intCount;i++)
    {
        luaWrap_lua_pushnumber(L,arrayField[i]);
        luaWrap_lua_rawseti(L,newTablePos,i+1);
    }
}

void pushUCharTableOntoStack(luaWrap_lua_State* L,int intCount,const unsigned char* arrayField)
{
    luaWrap_lua_newtable(L);
    int newTablePos=luaWrap_lua_gettop(L);
    for (int i=0;i<intCount;i++)
    {
        luaWrap_lua_pushnumber(L,arrayField[i]);
        luaWrap_lua_rawseti(L,newTablePos,i+1);
    }
}

void pushStringTableOntoStack(luaWrap_lua_State* L,const std::vector<std::string>& stringTable)
{
    luaWrap_lua_newtable(L);
    int newTablePos=luaWrap_lua_gettop(L);
    for (size_t i=0;i<stringTable.size();i++)
    {
        luaWrap_lua_pushstring(L,stringTable[i].c_str());
        luaWrap_lua_rawseti(L,newTablePos,i+1);
    }
}

void insertFloatsIntoTableAlreadyOnStack(luaWrap_lua_State* L,int tablePos,int floatCount,const float* arrayField)
{
    for (int i=0;i<floatCount;i++)
    {
        luaWrap_lua_pushnumber(L,arrayField[i]);
        luaWrap_lua_rawseti(L,tablePos,i+1);
    }
}

int luaToInt(luaWrap_lua_State* L,int pos)
{
    return((int)luaWrap_lua_tointeger(L,pos));
}

float luaToFloat(luaWrap_lua_State* L,int pos)
{
    return((float)luaWrap_lua_tonumber(L,pos));
}

double luaToDouble(luaWrap_lua_State* L,int pos)
{
    return(luaWrap_lua_tonumber(L,pos));
}

bool luaToBool(luaWrap_lua_State* L,int pos)
{
    return(luaWrap_lua_toboolean(L,pos)!=0);
}

bool suffixAdjustStringIfNeeded(const std::string& functionName,bool outputError,luaWrap_lua_State* L,std::string& name)
{
    std::string suffTxt;
    luaWrap_lua_getglobal(L,SIM_SCRIPT_NAME_SUFFIX);
    if (luaWrap_lua_isstring(L,-1))
        suffTxt=luaWrap_lua_tostring(L,-1);
    luaWrap_lua_pop(L,1);

    if (suffTxt=="")
    { // we don't want adjustment (the script name doesn't have any '#')
        // The special cases like "marci#" are handled on the C-API side!
        return(true); // means no error
    }

    // We want suffix adjustment!
    if (!isDashFree(functionName,name))
    { // We have a dash in the name of the object we want the handle of
        // We explicitely access the object (no name adjustment)
        // The special cases like "marci#" are handled on the C-API side!
        return(true); // no error
    }

    name+="#";
    name+=suffTxt;
    return(true);// no error
}

bool isDashFree(const std::string& functionName,const std::string& name)
{
    if (name.find('#')==std::string::npos)
        return(true);
    return(false);
}

int checkOneGeneralInputArgument(luaWrap_lua_State* L,int index,
                           int type,int cnt_orZeroIfNotTable,bool optional,bool nilInsteadOfTypeAndCountAllowed,std::string* errStr)
{ // return -1 means error, 0 means data is missing, 1 means data is nil, 2 means data is ok
    // if cnt_orZeroIfNotTable is -1, we are expecting a table, which could also be empty
    // 1. We check if there is something on the stack at that position:
    if (luaWrap_lua_gettop(L)<index)
    { // That data is missing:
        if (optional)
            return(0);
        if (errStr!=NULL)
            errStr->assign(SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS);
        return(-1);
    }
    // 2. We check if we have nil on the stack:
    if (luaWrap_lua_isnil(L,index))
    { // We have nil.
        // Did we expect a boolean? If yes, it is ok
        if ( (type==lua_arg_bool)&&(cnt_orZeroIfNotTable==0) )
            return(2);
        if ( (type==lua_arg_nil)&&(cnt_orZeroIfNotTable==0) )
            return(2);
        if (nilInsteadOfTypeAndCountAllowed)
            return(1);
        if (errStr!=NULL)
            errStr->assign(SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG);
        return(-1);
    }
    // 3. we check if we expect a table:
    if (cnt_orZeroIfNotTable!=0) // was >=1 until 18/2/2016
    { 
        // We check if we really have a table at that position:
        if (!luaWrap_lua_istable(L,index))
        {
            if (errStr!=NULL)
                errStr->assign(SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG);
            return(-1);
        }
        // we check the table size:
        if (int(luaWrap_lua_objlen(L,index))<cnt_orZeroIfNotTable)
        { // the size is not correct
            if (errStr!=NULL)
                errStr->assign(SIM_ERROR_ONE_TABLE_SIZE_IS_WRONG);
            return(-1);
        }
        else
        { // we have the correct size
            // Now we need to check each element individually for the correct type:
            for (int i=0;i<cnt_orZeroIfNotTable;i++)
            {
                luaWrap_lua_rawgeti(L,index,i+1);
                if (!checkOneInputArgument(L,-1,type,errStr))
                    return(-1);
                luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
            }
            // Everything went fine:
            return(2);
        }
    }
    else
    { // we expect a non-table type
        if (checkOneInputArgument(L,index,type,errStr))
            return(2);
        return(-1);
    }
}

bool _hasErrors(std::string& funcErrorString)
{
    if (funcErrorString.size()>0)
        return(true);
    funcErrorString=CApiErrors::getCSideGeneratedLuaError();
    CApiErrors::clearCSideGeneratedLuaError();
    return(funcErrorString.size()>0);
}

void luaApiCallWarning(const char* functionName,const char* message)
{   // Those messages can be suppressed, since they are not a formatting (i.e. bad code) problem.
    // Those messages are NOT saved (i.e. cannot be retrieved with simGetLastError since they are warnings, not errors)
    CApiErrors::setApiCallWarningMessage(functionName,message);
}

bool checkOneInputArgument(luaWrap_lua_State* L,int index,int type,std::string* errStr)
{
    // 1. We check if there is something on the stack at that position:
    if (luaWrap_lua_gettop(L)<index)
    { // That data is missing:
        if (errStr!=NULL)
            errStr->assign(SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS);
        return(false);
    }
    if (type==lua_arg_number)
    {
        if (!luaWrap_lua_isnumber(L,index))
        {
            if (errStr!=NULL)
                errStr->assign(SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG);
            return(false); // error
        }
        return(true);
    }
    if (type==lua_arg_bool)
    { // since anything can be a bool value, we don't generate any error!
        return(true);
    }
    if (type==lua_arg_nil)
    { // Here we expect a nil value:
        if (!luaWrap_lua_isnil(L,index))
        {
            if (errStr!=NULL)
                errStr->assign(SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG);
            return(false);
        }
        return(true);
    }
    if (type==lua_arg_string)
    {
        if (!luaWrap_lua_isstring(L,index))
        {
            if (errStr!=NULL)
                errStr->assign(SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG);
            return(false); // error
        }
        return(true);
    }
    if (type==lua_arg_table)
    {
        if (!luaWrap_lua_istable(L,index))
        {
            if (errStr!=NULL)
                errStr->assign(SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG);
            return(false); // error
        }
        return(true);
    }
    if (type==lua_arg_function)
    {
        if (!luaWrap_lua_isfunction(L,index))
        {
            if (errStr!=NULL)
                errStr->assign(SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG);
            return(false); // error
        }
        return(true);
    }
    if (type==lua_arg_userdata)
    {
        if (!luaWrap_lua_isuserdata(L,index))
        {
            if (errStr!=NULL)
                errStr->assign(SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG);
            return(false); // error
        }
        return(true);
    }
    // Here we have a table
    if (errStr!=NULL)
        errStr->assign(SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG);
    return(false);
}

bool checkInputArguments(luaWrap_lua_State* L,std::string* errStr,
                         int type1,int type1Cnt_zeroIfNotTable,
                         int type2,int type2Cnt_zeroIfNotTable,
                         int type3,int type3Cnt_zeroIfNotTable,
                         int type4,int type4Cnt_zeroIfNotTable,
                         int type5,int type5Cnt_zeroIfNotTable,
                         int type6,int type6Cnt_zeroIfNotTable,
                         int type7,int type7Cnt_zeroIfNotTable,
                         int type8,int type8Cnt_zeroIfNotTable,
                         int type9,int type9Cnt_zeroIfNotTable,
                         int type10,int type10Cnt_zeroIfNotTable,
                         int type11,int type11Cnt_zeroIfNotTable)
{ // all arguments, except L and the functionName have default values
    // CAREFUL!!! if typeXCnt_zeroIfNotTable is 0, it means it is a simple value (not table)
    // if typeXCnt_zeroIfNotTable is >=1 then we are expecting a table with at least typeXCnt_zeroIfNotTable elements!!
    // if typeXCnt_zeroIfNotTable is =-1 then we are expecting a table that can have any number of elements
    int inArgs[11]={type1,type2,type3,type4,type5,type6,type7,type8,type9,type10,type11};
    int inArgsCnt_zeroIfNotTable[11]={type1Cnt_zeroIfNotTable,type2Cnt_zeroIfNotTable,type3Cnt_zeroIfNotTable,type4Cnt_zeroIfNotTable,type5Cnt_zeroIfNotTable,type6Cnt_zeroIfNotTable,type7Cnt_zeroIfNotTable,type8Cnt_zeroIfNotTable,type9Cnt_zeroIfNotTable,type10Cnt_zeroIfNotTable,type11Cnt_zeroIfNotTable};
    int totArgs=0;
    for (int i=0;i<11;i++)
    {
        if (inArgs[i]==lua_arg_empty)
            break;
        if (inArgsCnt_zeroIfNotTable[i]<-1)
        {
            printf("Error in call to 'checkInputArguments' routine\n");
            App::beep();
            while (true);
        }
        totArgs++;
    }
    int numberOfArguments=luaWrap_lua_gettop(L);
    if (numberOfArguments<totArgs)
    { // we don't have enough arguments!
        if (errStr!=NULL)
            errStr->assign(SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS);
        return(false); // error
    }
    for (int i=0;i<totArgs;i++)
    {
//        if (checkOneGeneralInputArgument(L,i+1,inArgs[i],inArgsCnt_zeroIfNotTable[i],false,false,errStr)==-1)
        if (checkOneGeneralInputArgument(L,i+1,inArgs[i],inArgsCnt_zeroIfNotTable[i],false,false,errStr)!=2)
            return(false); // error
    }
    return(true);
}

int _genericFunctionHandler_new(luaWrap_lua_State* L,CLuaCustomFunction* func,std::string& raiseErrorWithMsg)
{
    LUA_API_FUNCTION_DEBUG;
    CInterfaceStack* stack=new CInterfaceStack();
    int stackId=App::ct->interfaceStackContainer->addStack(stack);
    stack->buildFromLuaStack(L);

    // We retrieve the suffix:
    std::string suffix("");
    luaWrap_lua_getglobal(L,SIM_SCRIPT_NAME_SUFFIX);
    if (luaWrap_lua_isstring(L,-1))
        suffix=luaWrap_lua_tostring(L,-1);
    luaWrap_lua_pop(L,1); // we correct the stack
    // Now we retrieve the object ID this script might be attached to:
    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* itObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    int linkedObject=-1;
    if (itObj->getScriptType()==sim_scripttype_childscript)
    {
        C3DObject* obj=App::ct->objCont->getObject(itObj->getObjectIDThatScriptIsAttachedTo_child());
        if (obj!=NULL)
            linkedObject=obj->getID();
    }
    if (itObj->getScriptType()==sim_scripttype_jointctrlcallback)
    {
        C3DObject* obj=App::ct->objCont->getObject(itObj->getObjectIDThatScriptIsAttachedTo_callback());
        if (obj!=NULL)
            linkedObject=obj->getID();
    }
    if (itObj->getScriptType()==sim_scripttype_customizationscript)
    {
        C3DObject* obj=App::ct->objCont->getObject(itObj->getObjectIDThatScriptIsAttachedTo_customization());
        if (obj!=NULL)
            linkedObject=obj->getID();
    }

    // We prepare the callback structure:
    char raiseErrorMsg[258];
    raiseErrorMsg[0]='\0';
    SScriptCallBack* cb=new SScriptCallBack;
    cb->objectID=linkedObject;
    cb->scriptID=currentScriptID;
    cb->stackID=stackId;
    cb->waitUntilZero=0;
    cb->raiseErrorWithMessage=(char*)&raiseErrorMsg;

    // Now we can call the callback:
    CApiErrors::pushLocation(1); // for correct error handling (i.e. assignement to the correct script and output)
    itObj->setInsideCustomLuaFunction(true);
    func->callBackFunction_new(cb);
    itObj->setInsideCustomLuaFunction(false);
    CApiErrors::popLocation(); // for correct error handling (i.e. assignement to the correct script and output)

    bool dontDeleteStructureYet=false;
    while (cb->waitUntilZero!=0)
    { // todo: What happens to a main script caught in here?! check (and fix) later
        if (!CThreadPool::switchBackToPreviousThread())
            break;
        if (CThreadPool::getSimulationStopRequestedAndActivated())
        { // give a chance to the c app to set the waitUntilZero to zero! (above turns true only 1-2 secs after the stop request arrived)
            // Following: the extension module might still write 0 into that position to signal "no more waiting" in
            // case this while loop got interrupted by a stop request.
            dontDeleteStructureYet=true;
            break;
        }
    }

    // Now we have to build the returned data onto the stack:
    stack->buildOntoLuaStack(L,false);

    if (strlen(cb->raiseErrorWithMessage)!=0)
        raiseErrorWithMsg+=cb->raiseErrorWithMessage;

    // And we return the number of arguments:
    int outputArgCount=stack->getStackSize();
    if (dontDeleteStructureYet)
    {   // We cannot yet delete the structure because an extension module might still write '0' into
        // p->waitUntilZero!! We delete the structure at the end of the simulation.
        App::ct->luaScriptContainer->addCallbackStructureObjectToDestroyAtTheEndOfSimulation_new(cb);
    }
    else
        delete cb;
    App::ct->interfaceStackContainer->destroyStack(stackId);
    return(outputArgCount);
}

int _genericFunctionHandler_old(luaWrap_lua_State* L,CLuaCustomFunction* func)
{
    LUA_API_FUNCTION_DEBUG;
    // We first read all arguments from the stack
    std::vector<char> inBoolVector;
    std::vector<int> inIntVector;
    std::vector<float> inFloatVector;
    std::vector<double> inDoubleVector;
    std::vector<std::string> inStringVector;
    std::vector<std::string> inCharVector;
    std::vector<int> inInfoVector;
    for (int i=0;i<int(func->inputArgTypes.size());i++)
    {
        if (!readCustomFunctionDataFromStack(L,i+1,func->inputArgTypes[i],inBoolVector,inIntVector,inFloatVector,inDoubleVector,inStringVector,inCharVector,inInfoVector))
            break;
    }

    // We retrieve the suffix:
    std::string suffix("");
    luaWrap_lua_getglobal(L,SIM_SCRIPT_NAME_SUFFIX);
    if (luaWrap_lua_isstring(L,-1))
        suffix=luaWrap_lua_tostring(L,-1);
    luaWrap_lua_pop(L,1); // we correct the stack
    // Now we retrieve the object ID this script might be attached to:
    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* itObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    int linkedObject=-1;
    if (itObj->getScriptType()==sim_scripttype_childscript)
    {
        C3DObject* obj=App::ct->objCont->getObject(itObj->getObjectIDThatScriptIsAttachedTo_child());
        if (obj!=NULL)
            linkedObject=obj->getID();
    }
    if (itObj->getScriptType()==sim_scripttype_jointctrlcallback)
    {
        C3DObject* obj=App::ct->objCont->getObject(itObj->getObjectIDThatScriptIsAttachedTo_callback());
        if (obj!=NULL)
            linkedObject=obj->getID();
    }
    if (itObj->getScriptType()==sim_scripttype_customizationscript)
    {
        C3DObject* obj=App::ct->objCont->getObject(itObj->getObjectIDThatScriptIsAttachedTo_customization());
        if (obj!=NULL)
            linkedObject=obj->getID();
    }
    // We prepare the callback structure:
    SLuaCallBack* p=new SLuaCallBack;
    p->objectID=linkedObject;
    p->scriptID=currentScriptID;
    p->inputBool=NULL;
    p->inputInt=NULL;
    p->inputFloat=NULL;
    p->inputDouble=NULL;
    p->inputChar=NULL;
    p->inputCharBuff=NULL;
    p->inputArgCount=0;
    p->inputArgTypeAndSize=NULL;
    p->outputBool=NULL;
    p->outputInt=NULL;
    p->outputFloat=NULL;
    p->outputDouble=NULL;
    p->outputChar=NULL;
    p->outputCharBuff=NULL;
    p->outputArgCount=0;
    p->outputArgTypeAndSize=NULL;
    p->waitUntilZero=0;
    // Now we prepare the input buffers:
    p->inputBool=new unsigned char[inBoolVector.size()];
    p->inputInt=new int[inIntVector.size()];
    p->inputFloat=new float[inFloatVector.size()];
    p->inputDouble=new double[inDoubleVector.size()];
    int charCnt=0;
    for (int k=0;k<int(inStringVector.size());k++)
        charCnt+=inStringVector[k].length()+1; // terminal 0
    p->inputChar=new char[charCnt];

    int charBuffCnt=0;
    for (int k=0;k<int(inCharVector.size());k++)
        charBuffCnt+=inCharVector[k].length();
    p->inputCharBuff=new char[charBuffCnt];

    p->inputArgCount=inInfoVector.size()/2;
    p->inputArgTypeAndSize=new int[inInfoVector.size()];
    // We fill the input buffers:
    for (int k=0;k<int(inBoolVector.size());k++)
        p->inputBool[k]=inBoolVector[k];
    for (int k=0;k<int(inIntVector.size());k++)
        p->inputInt[k]=inIntVector[k];
    for (int k=0;k<int(inFloatVector.size());k++)
        p->inputFloat[k]=inFloatVector[k];
    for (int k=0;k<int(inDoubleVector.size());k++)
        p->inputDouble[k]=inDoubleVector[k];
    charCnt=0;
    for (int k=0;k<int(inStringVector.size());k++)
    {
        for (int l=0;l<int(inStringVector[k].length());l++)
            p->inputChar[charCnt+l]=inStringVector[k][l];
        charCnt+=inStringVector[k].length();
        // terminal 0:
        p->inputChar[charCnt]=0;
        charCnt++;
    }

    charBuffCnt=0;
    for (int k=0;k<int(inCharVector.size());k++)
    {
        for (int l=0;l<int(inCharVector[k].length());l++)
            p->inputCharBuff[charBuffCnt+l]=inCharVector[k][l];
        charBuffCnt+=inCharVector[k].length();
    }

    for (int k=0;k<int(inInfoVector.size());k++)
        p->inputArgTypeAndSize[k]=inInfoVector[k];

    // Now we can call the callback:


    CApiErrors::pushLocation(1); // for correct error handling (i.e. assignement to the correct script and output)
    itObj->setInsideCustomLuaFunction(true);
    func->callBackFunction_old(p);
    itObj->setInsideCustomLuaFunction(false);
    CApiErrors::popLocation(); // for correct error handling (i.e. assignement to the correct script and output)

    bool dontDeleteStructureYet=false;
    while (p->waitUntilZero!=0)
    { // todo: What happens to a main script caught in here?! check (and fix) later
        if (!CThreadPool::switchBackToPreviousThread())
            break;
        if (CThreadPool::getSimulationStopRequestedAndActivated())
        { // give a chance to the c app to set the waitUntilZero to zero! (above turns true only 1-2 secs after the stop request arrived)
            // Following: the extension module might still write 0 into that position to signal "no more waiting" in
            // case this while loop got interrupted by a stop request.
            dontDeleteStructureYet=true;
            break;
        }
    }

    // We first delete the input buffers:
    delete[] p->inputBool;
    delete[] p->inputInt;
    delete[] p->inputFloat;
    delete[] p->inputDouble;
    delete[] p->inputChar;
    delete[] p->inputCharBuff;
    delete[] p->inputArgTypeAndSize;
    // Now we have to build the returned data onto the stack:
    if (p->outputArgCount!=0)
    {
        int boolPt=0;
        int intPt=0;
        int floatPt=0;
        int doublePt=0;
        int stringPt=0;
        int stringBuffPt=0;
        for (int i=0;i<p->outputArgCount;i++)
        {
            writeCustomFunctionDataOntoStack(L,p->outputArgTypeAndSize[2*i+0],p->outputArgTypeAndSize[2*i+1],
                p->outputBool,boolPt,
                p->outputInt,intPt,
                p->outputFloat,floatPt,
                p->outputDouble,doublePt,
                p->outputChar,stringPt,
                p->outputCharBuff,stringBuffPt);
        }
    }
    // We now delete the output buffers:
    delete[] p->outputBool;
    delete[] p->outputInt;
    delete[] p->outputFloat;
    delete[] p->outputDouble;
    delete[] p->outputChar;
    delete[] p->outputCharBuff;
    delete[] p->outputArgTypeAndSize;
    // And we return the number of arguments:
    int outputArgCount=p->outputArgCount;
    if (dontDeleteStructureYet)
    {   // We cannot yet delete the structure because an extension module might still write '0' into
        // p->waitUntilZero!! We delete the structure at the end of the simulation.
        App::ct->luaScriptContainer->addCallbackStructureObjectToDestroyAtTheEndOfSimulation_old(p);
    }
    else
        delete p;
    if (outputArgCount==0)
    {
        luaWrap_lua_pushnil(L);
        outputArgCount=1;
    }
    return(outputArgCount);
}

int _simGenericFunctionHandler(luaWrap_lua_State* L)
{   // THIS FUNCTION SHOULD NOT LOCK THE API (AT LEAST NOT WHILE CALLING THE CALLBACK!!) SINCE IT IS NOT DIRECTLY ACCESSING THE API!!!!
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGenericFunctionHandler");

    luaWrap_lua_pushvalue(L,luaWrap_lua_upvalueindex(1));
    int id=luaWrap_lua_tointeger(L,-1)-1;
    luaWrap_lua_pop(L,1); // we have to pop the pushed value to get the original stack state
    int outputArgCount=0;
    for (int j=0;j<int(App::ct->luaCustomFuncAndVarContainer->allCustomFunctions.size());j++)
    { // we now search for the callback to call:
        CLuaCustomFunction* it=App::ct->luaCustomFuncAndVarContainer->allCustomFunctions[j];
        if (it->getFunctionID()==id)
        { // we have the right one! Now we need to prepare the input and output argument arrays:
            functionName=it->getFunctionName();
            FUNCTION_INSIDE_DEBUG(functionName.c_str());
            if (it->getPluginName().size()!=0)
            {
                functionName+=" @ '";
                functionName+=it->getPluginName();
                functionName+="' plugin";
            }
            else
                functionName+=" @ <unknown plugin>";

            if (it->getUsesStackToExchangeData())
                outputArgCount=_genericFunctionHandler_new(L,it,errorString);
            else
                outputArgCount=_genericFunctionHandler_old(L,it);
            break;
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(outputArgCount);
}

int _simHandleChildScript(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleChildScript");

    luaApiCallWarning(functionName.c_str(),"simHandleChildScript is deprecated. Use simHandleChildScripts instead.");
    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if (!it->checkAndSetWarningAboutSimHandleChildScriptAlreadyIssued_oldCompatibility_7_8_2014())
    {
        std::string title("Compatibility issue with ");
        title+=it->getShortDescriptiveName();
        std::string txt("The command simHandleChildScript is not supported anymore and was replaced&&n");
        txt+="with simHandleChildScripts, which operates in a slightly different manner. Make sure to&&n";
        txt+="adjust this script manually.";
        float titleCols[6]={0.8f,0.0f,0.0f,0.0f,0.0f,0.0f};
        float dlgCols[6]={0.5f,0.0f,0.0f,1.0f,1.0f,1.0f};
        simDisplayDialog_internal(title.c_str(),txt.c_str(),sim_dlgstyle_ok,"",titleCols,dlgCols,NULL);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simHandleChildScripts(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleChildScripts");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int callType=luaWrap_lua_tointeger(L,1);
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        if (it!=NULL)
        { // Should always pass, unless someone playes with the script ID variable
            if ( (it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript) )
            { // only main and child scripts can call this function
                if ( (it->getScriptType()==sim_scripttype_mainscript)||((it->getScriptType()==sim_scripttype_childscript)&&(!it->getThreadedExecution())) )
                { // Threaded scripts cannot call this function
                    if ( it->getAutomaticCascadingCallsDisabled()||(it->getScriptType()==sim_scripttype_mainscript) )
                    {
                        // We read the function input arguments:
                        CInterfaceStack inputArguments;
                        inputArguments.buildFromLuaStack(L,2);
                        retVal=handleChildScriptsRoutine(callType,it,inputArguments);
                    }
                    else
                        errorString=SIM_ERROR_AUTOMATIC_CASCADING_CALLS_NOT_DISABLED;
                }
                else
                    errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_NON_THREADED_CHILD_SCRIPTS;
            }
            else
                errorString=SIM_ERROR_NOT_MAIN_NOR_CHILD_SCRIPT;
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int handleChildScriptsRoutine(int callType,CLuaScriptObject* it,CInterfaceStack& inputArguments)
{
    int executionResult=sim_script_no_error;
    std::vector<int> childScriptIDsToRun;

    // Now find all first child scripts in the hierarchy, including threaded or explicit handling flagged ones:
    if (it->getScriptType()==sim_scripttype_mainscript)
    { // we have a main script here
        for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
        {   // Do we have a parentless object?
            C3DObject* q=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
            if ( (q!=NULL)&&(q->getParent()==NULL) )
                q->getChildScriptsToRun(childScriptIDsToRun);
        }
    }
    else
    { // we have a script linked to a 3d object or a passive script here
        C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo_child());
        if (obj!=NULL)
        {
            for (int i=0;i<int(obj->childList.size());i++)
                obj->childList[i]->getChildScriptsToRun(childScriptIDsToRun);
        }
    }

    // Now we order all scripts according to their execution order settings:
    std::vector<int> firstToExecute;
    std::vector<int> normalToExecute;
    std::vector<int> lastToExecute;
    for (int i=0;i<int(childScriptIDsToRun.size());i++)
    {
        CLuaScriptObject* as=App::ct->luaScriptContainer->getScriptFromID(childScriptIDsToRun[i]);
        if (as!=NULL)//&&(!as->getScriptIsDisabled()) )
        {
            if (as->getThreadedExecution())
            { // We have a threaded script. We just skip it and explore beyond:
                C3DObject* obj=App::ct->objCont->getObject(as->getObjectIDThatScriptIsAttachedTo_child());
                if (obj!=NULL)
                {
                    for (int j=0;j<int(obj->childList.size());j++)
                        obj->childList[j]->getChildScriptsToRun(childScriptIDsToRun); // append to the end of the list
                }
            }
            else
            {
                if (as->getExecutionOrder()==sim_scriptexecorder_first)
                    firstToExecute.push_back(childScriptIDsToRun[i]);
                if (as->getExecutionOrder()==sim_scriptexecorder_normal)
                    normalToExecute.push_back(childScriptIDsToRun[i]);
                if (as->getExecutionOrder()==sim_scriptexecorder_last)
                    lastToExecute.push_back(childScriptIDsToRun[i]);
            }
        }
    }
    childScriptIDsToRun.clear();
    childScriptIDsToRun.insert(childScriptIDsToRun.end(),firstToExecute.begin(),firstToExecute.end());
    childScriptIDsToRun.insert(childScriptIDsToRun.end(),normalToExecute.begin(),normalToExecute.end());
    childScriptIDsToRun.insert(childScriptIDsToRun.end(),lastToExecute.begin(),lastToExecute.end());

    // Now we run all the scripts:
    int retVal=0;
    for (int i=0;i<int(childScriptIDsToRun.size());i++)
    {
        CLuaScriptObject* as=App::ct->luaScriptContainer->getScriptFromID(childScriptIDsToRun[i]);
        if (as!=NULL)
        {
            if (!as->getScriptIsDisabled())
            {
                C3DObject* obj=App::ct->objCont->getObject(as->getObjectIDThatScriptIsAttachedTo_child());
                if ((obj!=NULL)&&((obj->getCumulativeModelProperty()&sim_modelproperty_scripts_inactive)==0))
                {
                    retVal++;
                    executionResult|=as->runNonThreadedChildScript(inputArguments,callType);
                }
            }
            if (!as->getAutomaticCascadingCallsDisabled())
            { // now handle the childen of that one:
                handleChildScriptsRoutine(callType,as,inputArguments);
            }
        }
    }
    return(retVal);
}


int _simLaunchThreadedChildScripts(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simLaunchThreadedChildScripts");

    int retVal=-1; // means error
    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if (it!=NULL)
    { // Should always pass, unless someone plays with the script ID variable
        if (it->getScriptType()==sim_scripttype_mainscript)
        { // only main script can call this function
            retVal=launchThreadedChildScriptsRoutine(it);
        }
        else
            errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int launchThreadedChildScriptsRoutine(CLuaScriptObject* it)
{
    int executionResult=sim_script_no_error;
    std::vector<int> childScriptIDsToRun;

    // Now find all first child scripts in the hierarchy, including non-threaded or explicit handling flagged onces:
    if (it->getScriptType()==sim_scripttype_mainscript)
    { // we have a main script here
        for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
        {   // Do we have a parentless object?
            C3DObject* q=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
            if ( (q!=NULL)&&(q->getParent()==NULL) )
                q->getChildScriptsToRun(childScriptIDsToRun);
        }
    }
    else
    { // we have a script linked to a 3d object or a passive script here
        C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo_child());
        if (obj!=NULL)
        {
            for (int i=0;i<int(obj->childList.size());i++)
                obj->childList[i]->getChildScriptsToRun(childScriptIDsToRun);
        }
    }

    // Now we order all scripts according to their execution order settings:
    std::vector<int> firstToExecute;
    std::vector<int> normalToExecute;
    std::vector<int> lastToExecute;
    for (int i=0;i<int(childScriptIDsToRun.size());i++)
    {
        CLuaScriptObject* as=App::ct->luaScriptContainer->getScriptFromID(childScriptIDsToRun[i]);
        if (as!=NULL) //&&(!as->getScriptIsDisabled()) )
        {
            if (!as->getThreadedExecution())
            { // We have a non-threaded script. We just skip it and explore beyond:
                C3DObject* obj=App::ct->objCont->getObject(as->getObjectIDThatScriptIsAttachedTo_child());
                if (obj!=NULL)
                {
                    for (int j=0;j<int(obj->childList.size());j++)
                        obj->childList[j]->getChildScriptsToRun(childScriptIDsToRun); // append to the end of the list
                }
            }
            else
            {
                if (as->getExecutionOrder()==sim_scriptexecorder_first)
                    firstToExecute.push_back(childScriptIDsToRun[i]);
                if (as->getExecutionOrder()==sim_scriptexecorder_normal)
                    normalToExecute.push_back(childScriptIDsToRun[i]);
                if (as->getExecutionOrder()==sim_scriptexecorder_last)
                    lastToExecute.push_back(childScriptIDsToRun[i]);
            }
        }
    }
    childScriptIDsToRun.clear();
    childScriptIDsToRun.insert(childScriptIDsToRun.end(),firstToExecute.begin(),firstToExecute.end());
    childScriptIDsToRun.insert(childScriptIDsToRun.end(),normalToExecute.begin(),normalToExecute.end());
    childScriptIDsToRun.insert(childScriptIDsToRun.end(),lastToExecute.begin(),lastToExecute.end());

    // Now we run all the scripts, except the disabled ones:
    int retVal=0;
    for (int i=0;i<int(childScriptIDsToRun.size());i++)
    {
        CLuaScriptObject* as=App::ct->luaScriptContainer->getScriptFromID(childScriptIDsToRun[i]);
        if (as!=NULL)
        {
            if (!as->getScriptIsDisabled())
            {
                C3DObject* obj=App::ct->objCont->getObject(as->getObjectIDThatScriptIsAttachedTo_child());
                if ((obj!=NULL)&&((obj->getCumulativeModelProperty()&sim_modelproperty_scripts_inactive)==0))
                {
                    retVal++;
                    executionResult|=as->runThreadedChildScript();
                }
            }
            // now handle the childen of that one:
            launchThreadedChildScriptsRoutine(as);
        }
    }
    return(retVal);
}


int _simHandleSensingChildScripts(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleSensingChildScripts");

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetScriptName(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetScriptName");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int a=luaToInt(L,1);
        if (a==sim_handle_self)
            a=getCurrentScriptID(L);
        char* name=simGetScriptName_internal(a);
        if (name!=NULL)
        {
            luaWrap_lua_pushstring(L,name);
            simReleaseBuffer_internal(name);
            LUA_END(1); 
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetScriptExecutionCount(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetScriptExecutionCount");

    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,it->getNumberOfPasses());
    LUA_END(1); 
}

int _simIsScriptExecutionThreaded(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simIsScriptExecutionThreaded");

    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    int retVal=0;
    if (it->getThreadedExecution())
        retVal=1;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1); 
}

int _simIsScriptRunningInThread(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simIsScriptRunningInThread");

    int retVal=1;
    if (VThread::isCurrentThreadTheMainSimulationThread())
        retVal=0;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1); 
}

int _simGetObjectAssociatedWithScript(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectAssociatedWithScript");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int a=luaToInt(L,1);
        if (a==sim_handle_self)
            a=getCurrentScriptID(L);
        retVal=simGetObjectAssociatedWithScript_internal(a);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1); 
}

int _simGetScriptAssociatedWithObject(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetScriptAssociatedWithObject");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simGetScriptAssociatedWithObject_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1); 
}

int _simGetCustomizationScriptAssociatedWithObject(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetCustomizationScriptAssociatedWithObject");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simGetCustomizationScriptAssociatedWithObject_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simOpenModule(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simOpenModule");

    moduleCommonPart(L,sim_message_eventcallback_moduleopen,&errorString);

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(1);
}
int _simCloseModule(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCloseModule");

    moduleCommonPart(L,sim_message_eventcallback_moduleclose,&errorString);

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(1);
}

int _simHandleModule(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleModule");

    bool sensingPart=false;
    int res=checkOneGeneralInputArgument(L,2,lua_arg_bool,0,true,false,&errorString);
    if ( (res==0)||(res==2) )
    {
        if (res==2)
            sensingPart=(luaWrap_lua_toboolean(L,2)!=0);
        if (sensingPart)
            moduleCommonPart(L,sim_message_eventcallback_modulehandleinsensingpart,&errorString);
        else
            moduleCommonPart(L,sim_message_eventcallback_modulehandle,&errorString);
    }
    else
        luaWrap_lua_pushnumber(L,-1);

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(1);
}

void moduleCommonPart(luaWrap_lua_State* L,int action,std::string* errorString)
{
    LUA_API_FUNCTION_DEBUG;
    std::string functionName;
    if (action==sim_message_eventcallback_moduleopen)
        functionName="simOpenModule";
    if (action==sim_message_eventcallback_moduleclose)
        functionName="simCloseModule";
    if ( (action==sim_message_eventcallback_modulehandle)||(action==sim_message_eventcallback_modulehandleinsensingpart) )
        functionName="simHandleModule";
    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if (it->getScriptType()!=sim_scripttype_mainscript)
    {
        if (errorString!=NULL)
            errorString->assign(SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT);
        luaWrap_lua_pushnumber(L,-1);
    }
    else
    {
        bool handleAll=false;
        if (luaWrap_lua_isnumber(L,1))
        { // We try to check whether we have sim_handle_all as a number:
            if (luaToInt(L,1)==sim_handle_all)
            {
                handleAll=true;
                void* retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(action,NULL,NULL,NULL);
                delete[] ((char*)retVal);
                luaWrap_lua_pushnumber(L,1);
            }
        }
        if (!handleAll)
        {
            if (checkInputArguments(L,errorString,lua_arg_string,0))
            {
                std::string modName(luaWrap_lua_tostring(L,1));
                void* retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(action,NULL,(char*)modName.c_str(),NULL);
                delete[] ((char*)retVal);
                luaWrap_lua_pushnumber(L,1);
            }
            else
                luaWrap_lua_pushnumber(L,-1);
        }
    }
}


int _simBoolOr16(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simBoolOr16");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        luaWrap_lua_Number na=luaWrap_lua_tonumber(L,1);
        luaWrap_lua_Number nb=luaWrap_lua_tonumber(L,2);
        if ((na>=0)&&(nb>=0)&&(na<65535.9)&&(nb<65535.9))
        {
            unsigned int a=(unsigned int)na;
            unsigned int b=(unsigned int)nb;
            retVal=a|b;
        }
        else
            errorString=SIM_ERROR_INVALID_NUMBER_INPUT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simBoolAnd16(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simBoolAnd16");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        luaWrap_lua_Number na=luaWrap_lua_tonumber(L,1);
        luaWrap_lua_Number nb=luaWrap_lua_tonumber(L,2);
        if ((na>=0)&&(nb>=0)&&(na<65535.9)&&(nb<65535.9))
        {
            unsigned int a=(unsigned int)na;
            unsigned int b=(unsigned int)nb;
            retVal=a&b;
        }
        else
            errorString=SIM_ERROR_INVALID_NUMBER_INPUT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simBoolXor16(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simBoolXor16");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        luaWrap_lua_Number na=luaWrap_lua_tonumber(L,1);
        luaWrap_lua_Number nb=luaWrap_lua_tonumber(L,2);
        if ((na>=0)&&(nb>=0)&&(na<65535.9)&&(nb<65535.9))
        {
            unsigned int a=(unsigned int)na;
            unsigned int b=(unsigned int)nb;
            retVal=a^b;
        }
        else
            errorString=SIM_ERROR_INVALID_NUMBER_INPUT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}


int _simBoolOr32(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simBoolOr32");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        luaWrap_lua_Number na=luaWrap_lua_tonumber(L,1);
        luaWrap_lua_Number nb=luaWrap_lua_tonumber(L,2);
        if ((na>=0)&&(nb>=0)&&(na<4294967295.9)&&(nb<4294967295.9))
        {
            unsigned int a=(unsigned int)na;
            unsigned int b=(unsigned int)nb;
            luaWrap_lua_pushnumber(L,(luaWrap_lua_Number)(a|b));
            LUA_END(1);
        }
        else
            errorString=SIM_ERROR_INVALID_NUMBER_INPUT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simBoolAnd32(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simBoolAnd32");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        luaWrap_lua_Number na=luaWrap_lua_tonumber(L,1);
        luaWrap_lua_Number nb=luaWrap_lua_tonumber(L,2);
        if ((na>=0)&&(nb>=0)&&(na<4294967295.9)&&(nb<4294967295.9))
        {
            unsigned int a=(unsigned int)na;
            unsigned int b=(unsigned int)nb;
            luaWrap_lua_pushnumber(L,(luaWrap_lua_Number)(a&b));
            LUA_END(1);
        }
        else
            errorString=SIM_ERROR_INVALID_NUMBER_INPUT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simBoolXor32(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simBoolXor32");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        luaWrap_lua_Number na=luaWrap_lua_tonumber(L,1);
        luaWrap_lua_Number nb=luaWrap_lua_tonumber(L,2);
        if ((na>=0)&&(nb>=0)&&(na<4294967295.9)&&(nb<4294967295.9))
        {
            unsigned int a=(unsigned int)na;
            unsigned int b=(unsigned int)nb;
            luaWrap_lua_pushnumber(L,(luaWrap_lua_Number)(a^b));
            LUA_END(1);
        }
        else
            errorString=SIM_ERROR_INVALID_NUMBER_INPUT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}


int _simHandleDynamics(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleDynamics");

    int retVal=-1; // means error
    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if ( (itScrObj->getScriptType()==sim_scripttype_mainscript)||(itScrObj->getScriptType()==sim_scripttype_childscript) )
    {
        if (checkInputArguments(L,&errorString,lua_arg_number,0))
            retVal=simHandleDynamics_internal(luaToFloat(L,1));
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT_OR_CHILD_SCRIPT;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simHandleIkGroup(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleIkGroup");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simHandleIkGroup_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCheckIkGroup(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCheckIkGroup");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        if (luaWrap_lua_istable(L,2))
        {
            int jointCnt=luaWrap_lua_objlen(L,2);
            int* handles=new int[jointCnt];
            getIntsFromTable(L,2,jointCnt,handles);
            float* values=new float[jointCnt];

            int res=checkOneGeneralInputArgument(L,3,lua_arg_number,jointCnt,true,true,&errorString);
            if (res>=0)
            {
                int* jointOptionsP=NULL;
                std::vector<int> jointOptions;
                if (res==2)
                {
                    jointOptions.resize(jointCnt);
                    getIntsFromTable(L,3,jointCnt,&jointOptions[0]);
                    jointOptionsP=&jointOptions[0];
                }

                int retVal=simCheckIkGroup_internal(luaToInt(L,1),jointCnt,handles,values,jointOptionsP);
                luaWrap_lua_pushnumber(L,retVal);
                pushFloatTableOntoStack(L,jointCnt,values);
                delete[] values;
                delete[] handles;
                LUA_END(2);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,-1);
    LUA_END(1);
}


int _simHandleCollision(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleCollision");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int objHandle=luaToInt(L,1);
        retVal=simHandleCollision_internal(objHandle);
        if ( (retVal>0)&&(objHandle>=0) )
        {
            int collObjHandles[2];
            CRegCollision* it=App::ct->collisions->getObject(objHandle);
            if (it!=NULL)
            {
                it->readCollision(collObjHandles);
                luaWrap_lua_pushnumber(L,retVal);
                pushIntTableOntoStack(L,2,collObjHandles);
                LUA_END(2);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simReadCollision(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simReadCollision");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int objHandle=luaToInt(L,1);
        retVal=simReadCollision_internal(objHandle);
        if (retVal>0)
        {
            int collObjHandles[2];
            CRegCollision* it=App::ct->collisions->getObject(objHandle);
            if (it!=NULL)
            {
                it->readCollision(collObjHandles);
                luaWrap_lua_pushnumber(L,retVal);
                pushIntTableOntoStack(L,2,collObjHandles);
                LUA_END(2);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simHandleDistance(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleDistance");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float d;
        retVal=simHandleDistance_internal(luaToInt(L,1),&d);
        if (retVal==1)
        {
            luaWrap_lua_pushnumber(L,retVal);
            luaWrap_lua_pushnumber(L,d);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simReadDistance(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simReadDistance");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float d;
        retVal=simReadDistance_internal(luaToInt(L,1),&d);
        if (retVal==1)
        {
            luaWrap_lua_pushnumber(L,retVal);
            luaWrap_lua_pushnumber(L,d);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simHandleProximitySensor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleProximitySensor");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float detPt[4];
        int detectedObjectID;
        float surfaceNormal[3];
        retVal=simHandleProximitySensor_internal(luaToInt(L,1),detPt,&detectedObjectID,surfaceNormal);
        if (retVal==1)
        {
            luaWrap_lua_pushnumber(L,retVal);
            luaWrap_lua_pushnumber(L,detPt[3]);
            pushFloatTableOntoStack(L,3,detPt);
            luaWrap_lua_pushnumber(L,detectedObjectID);
            pushFloatTableOntoStack(L,3,surfaceNormal);
            LUA_END(5);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simReadProximitySensor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simReadProximitySensor");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float detPt[4];
        int detectedObjectID;
        float surfaceNormal[3];
        retVal=simReadProximitySensor_internal(luaToInt(L,1),detPt,&detectedObjectID,surfaceNormal);
        if (retVal==1)
        {
            luaWrap_lua_pushnumber(L,retVal);
            luaWrap_lua_pushnumber(L,detPt[3]);
            pushFloatTableOntoStack(L,3,detPt);
            luaWrap_lua_pushnumber(L,detectedObjectID);
            pushFloatTableOntoStack(L,3,surfaceNormal);
            LUA_END(5);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simHandleMill(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleMill");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float cutSurfaceAndVolume[2];
        retVal=simHandleMill_internal(luaToInt(L,1),cutSurfaceAndVolume);
        if (retVal!=-1)
        {
            luaWrap_lua_pushnumber(L,retVal);
            pushFloatTableOntoStack(L,2,cutSurfaceAndVolume);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simHandleVisionSensor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleVisionSensor");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float* auxVals=NULL;
        int* auxValsCount=NULL;
        retVal=simHandleVisionSensor_internal(luaToInt(L,1),&auxVals,&auxValsCount);
        if ((retVal!=-1)&&(auxValsCount!=NULL))
        {
            int off=0;
            luaWrap_lua_pushnumber(L,retVal);
            int tableCount=auxValsCount[0];
            for (int i=0;i<tableCount;i++)
            {
                pushFloatTableOntoStack(L,auxValsCount[i+1],auxVals+off);
                off+=auxValsCount[i+1];
            }
            delete[] auxValsCount;
            delete[] auxVals;
            LUA_END(1+tableCount);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simReadVisionSensor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simReadVisionSensor");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float* auxVals=NULL;
        int* auxValsCount=NULL;
        retVal=simReadVisionSensor_internal(luaToInt(L,1),&auxVals,&auxValsCount);
        if ((retVal!=-1)&&(auxValsCount!=NULL))
        {
            int off=0;
            luaWrap_lua_pushnumber(L,retVal);
            int tableCount=auxValsCount[0];
            for (int i=0;i<tableCount;i++)
            {
                pushFloatTableOntoStack(L,auxValsCount[i+1],auxVals+off);
                off+=auxValsCount[i+1];
            }
            delete[] auxValsCount;
            delete[] auxVals;
            LUA_END(1+tableCount);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simResetCollision(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simResetCollision");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simResetCollision_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simResetDistance(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simResetDistance");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simResetDistance_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simResetProximitySensor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simResetProximitySensor");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simResetProximitySensor_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simResetMill(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simResetMill");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simResetMill_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simResetVisionSensor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simResetVisionSensor");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simResetVisionSensor_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCheckProximitySensor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCheckProximitySensor");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        float detPt[4];
        retVal=simCheckProximitySensor_internal(luaToInt(L,1),luaToInt(L,2),detPt);
        if (retVal==1)
        {
            luaWrap_lua_pushnumber(L,retVal);
            luaWrap_lua_pushnumber(L,detPt[3]);
            pushFloatTableOntoStack(L,3,detPt);
            LUA_END(3);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCheckProximitySensorEx(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCheckProximitySensorEx");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        float detPt[4];
        int detObj;
        float normVect[3];
        retVal=simCheckProximitySensorEx_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),luaToFloat(L,4),luaToFloat(L,5),detPt,&detObj,normVect);
        if (retVal==1)
        {
            luaWrap_lua_pushnumber(L,retVal);
            luaWrap_lua_pushnumber(L,detPt[3]);
            pushFloatTableOntoStack(L,3,detPt);
            luaWrap_lua_pushnumber(L,detObj);
            pushFloatTableOntoStack(L,3,normVect);
            LUA_END(5);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCheckProximitySensorEx2(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCheckProximitySensorEx2");

    int retVal=-1; // means error
    if (checkOneGeneralInputArgument(L,1,lua_arg_number,0,false,false,&errorString)==2)
    { // first argument (sensor handle)
        int sensorID=luaToInt(L,1);
        if (checkOneGeneralInputArgument(L,3,lua_arg_number,0,false,false,&errorString)==2)
        { // third argument (item type)
            int itemType=luaToInt(L,3);
            if (checkOneGeneralInputArgument(L,4,lua_arg_number,0,false,false,&errorString)==2)
            { // forth argument (item count)
                int itemCount=luaToInt(L,4);
                int requiredValues=itemCount*3*(itemType+1);
                if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,requiredValues,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
                {
                    int mode=luaToInt(L,5);
                    float threshold=luaToFloat(L,6);
                    float maxAngle=luaToFloat(L,7);
                    float* vertices=new float[requiredValues];
                    getFloatsFromTable(L,2,requiredValues,vertices);

                    float detPt[4];
                    float normVect[3];
                    retVal=simCheckProximitySensorEx2_internal(sensorID,vertices,itemType,itemCount,mode,threshold,maxAngle,detPt,normVect);
                    delete[] vertices;
                    if (retVal==1)
                    {
                        luaWrap_lua_pushnumber(L,retVal);
                        luaWrap_lua_pushnumber(L,detPt[3]);
                        pushFloatTableOntoStack(L,3,detPt);
                        pushFloatTableOntoStack(L,3,normVect);
                        LUA_END(4);
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetVisionSensorResolution(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetVisionSensorResolution");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int resolution[2];
        if (simGetVisionSensorResolution_internal(luaToInt(L,1),resolution)==1)
        {
            pushIntTableOntoStack(L,2,resolution);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetVisionSensorImage(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetVisionSensorImage");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int arg1=luaToInt(L,1);
        int handleFlags=arg1&0xff00000;
        int objectHandle=arg1&0xfffff;
        int valPerPix=3;
        int rgbOrGreyOrDepth=0;
        if ((handleFlags&sim_handleflag_greyscale)!=0)
        {
            valPerPix=1;
            rgbOrGreyOrDepth=1;
        }
        int posX=0;
        int posY=0;
        int sizeX=0;
        int sizeY=0;
        int retType=0;
        // Now check the optional arguments:
        int res;
        res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            if (res==2)
                posX=luaToInt(L,2);
            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    posY=luaToInt(L,3);
                res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        sizeX=luaToInt(L,4);
                    res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,false,&errorString);
                    if ((res==0)||(res==2))
                    {
                        if (res==2)
                            sizeY=luaToInt(L,5);
                        res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,true,false,&errorString);
                        if ((res==0)||(res==2))
                        {
                            if (res==2)
                                retType=luaToInt(L,6);
                            CVisionSensor* rs=App::ct->objCont->getVisionSensor(objectHandle);
                            if (rs!=NULL)
                            {
                                if ( (sizeX==0)&&(sizeY==0) )
                                { // we have default values here (the whole picture)
                                    int reso[2];
                                    rs->getRealResolution(reso);
                                    sizeX=reso[0];
                                    sizeY=reso[1];
                                }
                                float* buffer=rs->readPortionOfImage(posX,posY,sizeX,sizeY,rgbOrGreyOrDepth);
                                if (buffer!=NULL)
                                {
                                    if (retType==0)
                                        pushFloatTableOntoStack(L,sizeX*sizeY*valPerPix,buffer);
                                    else
                                    { // here we return RGB data in a string
                                        char* str=new char[sizeX*sizeY*valPerPix];
                                        int vvv=sizeX*sizeY*valPerPix;
                                        for (int i=0;i<vvv;i++)
                                            str[i]=char(buffer[i]*255.0001f);
                                        luaWrap_lua_pushlstring(L,(const char*)str,vvv);
                                        delete[] ((char*)str);
                                    }
                                    delete[] ((char*)buffer);
                                    LUA_END(1);
                                }
                                else
                                    errorString=SIM_ERROR_INVALID_ARGUMENTS;
                            }
                            else
                                errorString=SIM_ERROR_VISION_SENSOR_INEXISTANT;
                        }
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetVisionSensorCharImage(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetVisionSensorCharImage");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int arg1=luaToInt(L,1);
        int handleFlags=arg1&0xff00000;
        int objectHandle=arg1&0xfffff;
        int valPerPix=3;
        if ((handleFlags&sim_handleflag_greyscale)!=0)
            valPerPix=1;
        int posX=0;
        int posY=0;
        int sizeX=0;
        int sizeY=0;
        float rgbaCutOff=0.0f;
        // Now check the optional arguments:
        int res;
        res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            if (res==2)
                posX=luaToInt(L,2);
            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    posY=luaToInt(L,3);
                res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        sizeX=luaToInt(L,4);
                    res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,false,&errorString);
                    if ((res==0)||(res==2))
                    {
                        if (res==2)
                            sizeY=luaToInt(L,5);
                        res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,true,false,&errorString);
                        if ((res==0)||(res==2))
                        {
                            if (res==2)
                                rgbaCutOff=luaToFloat(L,6);
                            CVisionSensor* rs=App::ct->objCont->getVisionSensor(objectHandle);
                            if (rs!=NULL)
                            {
                                int reso[2];
                                rs->getRealResolution(reso);
                                if ( (sizeX==0)&&(sizeY==0) )
                                { // we have default values here (the whole picture)
                                    sizeX=reso[0];
                                    sizeY=reso[1];
                                }
                                unsigned char* buffer=rs->readPortionOfCharImage(posX,posY,sizeX,sizeY,rgbaCutOff,valPerPix==1);
                                if (buffer!=NULL)
                                {
                                    int vvv=sizeX*sizeY*valPerPix;
                                    if (rgbaCutOff>0.0f)
                                    {
                                        if (valPerPix==1)
                                            vvv=sizeX*sizeY*2;
                                        else
                                            vvv=sizeX*sizeY*4;
                                    }
                                    luaWrap_lua_pushlstring(L,(const char*)buffer,vvv);
                                    delete[] ((char*)buffer);
                                    luaWrap_lua_pushinteger(L,reso[0]);
                                    luaWrap_lua_pushinteger(L,reso[1]);
                                    LUA_END(3);
                                }
                                else
                                    errorString=SIM_ERROR_INVALID_ARGUMENTS;
                            }
                            else
                                errorString=SIM_ERROR_VISION_SENSOR_INEXISTANT;
                        }
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetVisionSensorImage(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetVisionSensorImage");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int arg1=luaToInt(L,1);
        int handleFlags=arg1&0xff00000;
        int sensHandle=arg1&0xfffff;
        int valPerPix=3;
        if ((handleFlags&sim_handleflag_greyscale)!=0)
            valPerPix=1;
        C3DObject* it=App::ct->objCont->getObject(sensHandle);
        if (it!=NULL)
        { // Ok we have a valid object
            if (it->getObjectType()==sim_object_visionsensor_type)
            { // ok we have a valid vision sensor
                int res[2];
                CVisionSensor* rendSens=(CVisionSensor*)it;
                rendSens->getRealResolution(res);
                // We check if we have a table or string at position 2:
                bool notTableNorString=true;
                if (luaWrap_lua_istable(L,2))
                { // Ok we have a table. Now what size is it?
                    notTableNorString=false;
                    // Now we check if the provided table has correct size:
                    if (int(luaWrap_lua_objlen(L,2))>=res[0]*res[1]*valPerPix)
                    {
                        float* img=new float[res[0]*res[1]*valPerPix];
                        getFloatsFromTable(L,2,res[0]*res[1]*valPerPix,img); // we do the operation directly without going through the c-api
                        if (rendSens->setExternalImage(img,valPerPix==1))
                            retVal=1;
                        delete[] img;
                    }
                    else
                        errorString=SIM_ERROR_ONE_TABLE_SIZE_IS_WRONG;
                }
                if (luaWrap_lua_isstring(L,2))
                { // Ok we have a string. Now what size is it?
                    notTableNorString=false;
                    // Now we check if the provided string has correct size:
                    size_t dataLength;
                    char* data=(char*)luaWrap_lua_tolstring(L,2,&dataLength);
                    if (int(dataLength)>=res[0]*res[1]*valPerPix)
                    {
                        float* img=new float[res[0]*res[1]*valPerPix];
                        for (int i=0;i<res[0]*res[1]*valPerPix;i++)
                            img[i]=float(data[i])/255.0f;
                        if (rendSens->setExternalImage(img,valPerPix==1))
                            retVal=1;
                        delete[] img;
                    }
                    else
                        errorString=SIM_ERROR_ONE_STRING_SIZE_IS_WRONG;
                }
                if (notTableNorString)
                    errorString=SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG;
            }
            else
                errorString=SIM_ERROR_OBJECT_NOT_VISION_SENSOR;
        }
        else
            errorString=SIM_ERROR_OBJECT_INEXISTANT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetVisionSensorCharImage(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetVisionSensorCharImage");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int arg1=luaToInt(L,1);
        int handleFlags=arg1&0xff00000;
        int sensHandle=arg1&0xfffff;
        int valPerPix=3;
        if ((handleFlags&sim_handleflag_greyscale)!=0)
            valPerPix=1;
        C3DObject* it=App::ct->objCont->getObject(sensHandle);
        if (it!=NULL)
        { // Ok we have a valid object
            if (it->getObjectType()==sim_object_visionsensor_type)
            { // ok we have a valid vision sensor
                int res[2];
                CVisionSensor* rendSens=(CVisionSensor*)it;
                rendSens->getRealResolution(res);
                // We check if we have a string at position 2:
                if (luaWrap_lua_isstring(L,2))
                { // Ok we have a string. Now what size is it?
                    // Now we check if the provided string has correct size:
                    size_t dataLength;
                    char* data=(char*)luaWrap_lua_tolstring(L,2,&dataLength);
                    if (int(dataLength)>=res[0]*res[1]*valPerPix)
                    {
                        if (rendSens->setExternalCharImage((unsigned char*)data,valPerPix==1))
                            retVal=1;
                    }
                    else
                        errorString=SIM_ERROR_ONE_STRING_SIZE_IS_WRONG;
                }
                else
                    errorString=SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG;
            }
            else
                errorString=SIM_ERROR_OBJECT_NOT_VISION_SENSOR;
        }
        else
            errorString=SIM_ERROR_OBJECT_INEXISTANT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetVisionSensorDepthBuffer(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetVisionSensorDepthBuffer");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int posX=0;
        int posY=0;
        int sizeX=0;
        int sizeY=0;
        // Now check the optional arguments:
        int res;
        res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            if (res==2)
                posX=luaToInt(L,2);
            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    posY=luaToInt(L,3);
                res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        sizeX=luaToInt(L,4);
                    res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,false,&errorString);
                    if ((res==0)||(res==2))
                    {
                        if (res==2)
                            sizeY=luaToInt(L,5);
                        int sensHandle=luaToInt(L,1);
                        bool returnString=(sensHandle&sim_handleflag_codedstring)!=0;
                        sensHandle=sensHandle&0xfffff;
                        CVisionSensor* rs=App::ct->objCont->getVisionSensor(sensHandle);
                        if (rs!=NULL)
                        {
                            if ( (sizeX==0)&&(sizeY==0) )
                            { // we have default values here (the whole picture)
                                int reso[2];
                                rs->getRealResolution(reso);
                                sizeX=reso[0];
                                sizeY=reso[1];
                            }
                            float* buffer=rs->readPortionOfImage(posX,posY,sizeX,sizeY,2);
                            if (buffer!=NULL)
                            {
                                if (returnString)
                                    luaWrap_lua_pushlstring(L,(char*)buffer,sizeX*sizeY*sizeof(float));
                                else
                                    pushFloatTableOntoStack(L,sizeX*sizeY,buffer);
                                delete[] ((char*)buffer);
                                LUA_END(1);
                            }
                            else
                                errorString=SIM_ERROR_INVALID_ARGUMENTS;
                        }
                        else
                            errorString=SIM_ERROR_VISION_SENSOR_INEXISTANT;
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simCheckVisionSensor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCheckVisionSensor");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        float* auxVals=NULL;
        int* auxValsCount=NULL;
        retVal=simCheckVisionSensor_internal(luaToInt(L,1),luaToInt(L,2),&auxVals,&auxValsCount);
        if ((retVal!=-1)&&(auxValsCount!=NULL))
        {
            int off=0;
            luaWrap_lua_pushnumber(L,retVal);
            int tableCount=auxValsCount[0];
            for (int i=0;i<tableCount;i++)
            {
                pushFloatTableOntoStack(L,auxValsCount[i+1],auxVals+off);
                off+=auxValsCount[i+1];
            }
            delete[] auxValsCount;
            delete[] auxVals;
            LUA_END(1+tableCount);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCheckVisionSensorEx(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCheckVisionSensorEx");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_bool,0))
    {
        simBool returnImage=luaToBool(L,3);
        float* buffer=simCheckVisionSensorEx_internal(luaToInt(L,1),luaToInt(L,2),returnImage);
        if (buffer!=NULL)
        {
            int res[2];
            simGetVisionSensorResolution_internal(luaToInt(L,1),res);
            if (returnImage)
                pushFloatTableOntoStack(L,res[0]*res[1]*3,buffer);
            else
                pushFloatTableOntoStack(L,res[0]*res[1],buffer);
            simReleaseBuffer_internal((char*)buffer);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetObjects(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjects");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simGetObjects_internal(luaToInt(L,1),luaToInt(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectHandle(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectHandle");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string name(luaWrap_lua_tostring(L,1));
        if (suffixAdjustStringIfNeeded(functionName,true,L,name))
        {
            quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
            retVal=simGetObjectHandle_internal(name.c_str());
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAddScript(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddScript");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int scriptType=luaToInt(L,1);
        retVal=simAddScript_internal(scriptType);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAssociateScriptWithObject(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAssociateScriptWithObject");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int scriptHandle=luaToInt(L,1);
        int objectHandle=luaToInt(L,2);
        retVal=simAssociateScriptWithObject_internal(scriptHandle,objectHandle);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetScriptText(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetScriptText");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0))
    {
        int scriptHandle=luaToInt(L,1);
        std::string scriptText(luaWrap_lua_tostring(L,2));
        retVal=simSetScriptText_internal(scriptHandle,scriptText.c_str());
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}


int _simGetScriptHandle(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetScriptHandle");

    int retVal=-1; // means error
    if (luaWrap_lua_gettop(L)==0) // no arguments
        retVal=getCurrentScriptID(L);
    else
    {
        if (checkInputArguments(L,NULL,lua_arg_nil,0)) // we don't output errors here!!
            retVal=getCurrentScriptID(L); // nil argument
        else
        {
            if (checkInputArguments(L,&errorString,lua_arg_string,0))
            {
                std::string name(luaWrap_lua_tostring(L,1));
                if (suffixAdjustStringIfNeeded(functionName,true,L,name))
                {
                    quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
                    retVal=simGetScriptHandle_internal(name.c_str());
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetIkGroupHandle(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetIkGroupHandle");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string name(luaWrap_lua_tostring(L,1));
        if (suffixAdjustStringIfNeeded(functionName,true,L,name))
        {
            quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
            retVal=simGetIkGroupHandle_internal(name.c_str());
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetCollisionHandle(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetCollisionHandle");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string name(luaWrap_lua_tostring(L,1));
        if (suffixAdjustStringIfNeeded(functionName,true,L,name))
        {
            quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
            retVal=simGetCollisionHandle_internal(name.c_str());
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRemoveScript(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRemoveScript");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        if (handle==sim_handle_self)
            handle=getCurrentScriptID(L);
        if (sim_handle_all!=handle)
            retVal=simRemoveScript_internal(handle);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetDistanceHandle(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetDistanceHandle");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string name(luaWrap_lua_tostring(L,1));
        if (suffixAdjustStringIfNeeded(functionName,true,L,name))
        {
            quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
            retVal=simGetDistanceHandle_internal(name.c_str());
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetMechanismHandle(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetMechanismHandle");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string name(luaWrap_lua_tostring(L,1));
        if (suffixAdjustStringIfNeeded(functionName,true,L,name))
        {
            quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
            retVal=simGetMechanismHandle_internal(name.c_str());
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetCollectionHandle(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetCollectionHandle");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string name(luaWrap_lua_tostring(L,1));
        if (suffixAdjustStringIfNeeded(functionName,true,L,name))
        {
            quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
            retVal=simGetCollectionHandle_internal(name.c_str());
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRemoveCollection(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRemoveCollection");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simRemoveCollection_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simEmptyCollection(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simEmptyCollection");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simEmptyCollection_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectPosition(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectPosition");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        float coord[3];
        if (simGetObjectPosition_internal(luaToInt(L,1),luaToInt(L,2),coord)==1)
        {
            pushFloatTableOntoStack(L,3,coord);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetObjectOrientation(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectOrientation");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        float coord[3];
        if (simGetObjectOrientation_internal(luaToInt(L,1),luaToInt(L,2),coord)==1)
        {
            pushFloatTableOntoStack(L,3,coord);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetObjectPosition(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetObjectPosition");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,3))
    {
        float coord[3];
        getFloatsFromTable(L,3,3,coord);
        retVal=simSetObjectPosition_internal(luaToInt(L,1),luaToInt(L,2),coord);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetObjectOrientation(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetObjectOrientation");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,3))
    {
        float coord[3];
        getFloatsFromTable(L,3,3,coord);
        retVal=simSetObjectOrientation_internal(luaToInt(L,1),luaToInt(L,2),coord);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetJointPosition(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetJointPosition");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float jointVal[1];
        if (simGetJointPosition_internal(luaToInt(L,1),jointVal)!=-1)
        {
            luaWrap_lua_pushnumber(L,jointVal[0]);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetJointPosition(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetJointPosition");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetJointPosition_internal(luaToInt(L,1),luaToFloat(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetJointTargetPosition(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetJointTargetPosition");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetJointTargetPosition_internal(luaToInt(L,1),luaToFloat(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetJointTargetPosition(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetJointTargetPosition");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float targetPos;
        if (simGetJointTargetPosition_internal(luaToInt(L,1),&targetPos)!=-1)
        {
            luaWrap_lua_pushnumber(L,targetPos);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetJointForce(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetJointForce");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetJointForce_internal(luaToInt(L,1),luaToFloat(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetPathPosition(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetPathPosition");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float pathVal[1];
        if (simGetPathPosition_internal(luaToInt(L,1),pathVal)!=-1)
        {
            luaWrap_lua_pushnumber(L,pathVal[0]);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetPathPosition(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetPathPosition");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetPathPosition_internal(luaToInt(L,1),luaToFloat(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetPathLength(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetPathLength");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float pathLen[1];
        if (simGetPathLength_internal(luaToInt(L,1),pathLen)!=-1)
        {
            luaWrap_lua_pushnumber(L,pathLen[0]);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetJointTargetVelocity(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetJointTargetVelocity");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetJointTargetVelocity_internal(luaToInt(L,1),luaToFloat(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetJointTargetVelocity(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetJointTargetVelocity");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float targetVel;
        if (simGetJointTargetVelocity_internal(luaToInt(L,1),&targetVel)!=-1)
        {
            luaWrap_lua_pushnumber(L,targetVel);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetPathTargetNominalVelocity(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetPathTargetNominalVelocity");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetPathTargetNominalVelocity_internal(luaToInt(L,1),luaToFloat(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRefreshDialogs(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRefreshDialogs");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simRefreshDialogs_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectName(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectName");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        char* name=simGetObjectName_internal(luaToInt(L,1));
        if (name!=NULL)
        {
            luaWrap_lua_pushstring(L,name);
            simReleaseBuffer_internal(name);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetCollectionName(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetCollectionName");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        char* name=simGetCollectionName_internal(luaToInt(L,1));
        if (name!=NULL)
        {
            luaWrap_lua_pushstring(L,name);
            simReleaseBuffer_internal(name);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetModuleName(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetModuleName");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        unsigned char version;
        char* name=simGetModuleName_internal(luaToInt(L,1),&version);
        if (name!=NULL)
        {
            luaWrap_lua_pushstring(L,name);
            simReleaseBuffer_internal(name);
            luaWrap_lua_pushnumber(L,version);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetSimulationTime(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetSimulationTime");

    float theTime=simGetSimulationTime_internal();
    if (theTime>=0.0f)
    {
        luaWrap_lua_pushnumber(L,theTime);
        LUA_END(1);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,-1);
    LUA_END(1);
}

int _simGetSimulationState(luaWrap_lua_State* L)
{
    // In case we copy-paste a script during execution, the new script doesn't get the sim_simulation_starting message,
    // but that is ok!!! sim_simulation_starting is only for a simulation start. For a first run in a script, use some
    // random variable and check whether it is != from nil!! or use simGetScriptExecutionCount
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetSimulationState");

    int retVal=simGetSimulationState_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetSystemTime(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetSystemTime");

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,simGetSystemTime_internal());
    LUA_END(1);
}

int _simGetSystemTimeInMs(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetSystemTimeInMs");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int lastTime=luaToInt(L,1);
        luaWrap_lua_pushnumber(L,simGetSystemTimeInMs_internal(lastTime));
        LUA_END(1);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simCheckCollision(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCheckCollision");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simCheckCollision_internal(luaToInt(L,1),luaToInt(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCheckCollisionEx(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCheckCollisionEx");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        float* intersections[1];
        retVal=simCheckCollisionEx_internal(luaToInt(L,1),luaToInt(L,2),intersections);
        if (retVal>0)
        {
            luaWrap_lua_pushnumber(L,retVal);
            pushFloatTableOntoStack(L,retVal*6,(*intersections));
            simReleaseBuffer_internal((char*)(*intersections));
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCheckDistance(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCheckDistance");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        float distanceData[7];

        retVal=simCheckDistance_internal(luaToInt(L,1),luaToInt(L,2),luaToFloat(L,3),distanceData);
        if (retVal==1)
        {
            luaWrap_lua_pushnumber(L,1);
            pushFloatTableOntoStack(L,7,distanceData);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectConfiguration(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectConfiguration");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        char* data=simGetObjectConfiguration_internal(luaToInt(L,1));
        if (data==NULL)
            luaWrap_lua_pushnumber(L,-1);
        else
        {
            CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
            luaWrap_lua_pushnumber(L,it->setUserData(data));
        }
    }
    else
        luaWrap_lua_pushnumber(L,-1);

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(1);
}

int _simSetObjectConfiguration(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetObjectConfiguration");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
        char* data=(char*)it->getUserData(luaWrap_lua_tointeger(L,1));
        if (data!=NULL)
            retVal=simSetObjectConfiguration_internal(data);
        else
            errorString=SIM_ERROR_INVALID_HANDLE;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetConfigurationTree(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetConfigurationTree");

    int retVal=-1;
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int id=luaToInt(L,1);
        if (id==sim_handle_self)
        {
            int objID=it->getObjectIDThatScriptIsAttachedTo_child();
            id=objID;
            if (id==-1)
                errorString=SIM_ERROR_ARGUMENT_VALID_ONLY_WITH_CHILD_SCRIPTS;
        }
        if (id!=-1)
        {
            char* data=simGetConfigurationTree_internal(id);
            if (data!=NULL)
                retVal=it->setUserData(data);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetConfigurationTree(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetConfigurationTree");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
        char* data=(char*)it->getUserData(luaWrap_lua_tointeger(L,1));
        if (data==NULL)
            errorString=SIM_ERROR_INVALID_HANDLE;
        else
            retVal=simSetConfigurationTree_internal(data);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simHandleMechanism(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleMechanism");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simHandleMechanism_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetLastError(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetLastError");

    char* it=simGetLastError_internal();
    if (it!=NULL)
    {
        luaWrap_lua_pushstring(L,it);
        simReleaseBuffer_internal(it);
        LUA_END(1);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetSimulationTimeStep(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetSimulationTimeStep");

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,simGetSimulationTimeStep_internal());
    LUA_END(1);
}

int _simGetSimulatorMessage(luaWrap_lua_State* L)
{ // Careful!! This command does not map its corresponding C-API command!! (different message pipeline)
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetSimulatorMessage");

    int auxVals[4];
    float aux2Vals[8];
    int aux2Cnt;
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
    int commandID=it->extractCommandFromOutsideCommandQueue(auxVals,aux2Vals,aux2Cnt);
    if (commandID!=-1)
    {
        luaWrap_lua_pushnumber(L,commandID);
        pushIntTableOntoStack(L,4,auxVals);
        if (aux2Cnt!=0)
        {
            pushFloatTableOntoStack(L,aux2Cnt,aux2Vals);
            LUA_END(3);
        }
        LUA_END(2);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,-1);
    LUA_END(1);
}

int _simResetGraph(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simResetGraph");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simResetGraph_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simHandleGraph(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleGraph");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simHandleGraph_internal(luaToInt(L,1),luaToFloat(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAddStatusbarMessage(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddStatusbarMessage");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
        retVal=simAddStatusbarMessage_internal(luaWrap_lua_tostring(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetScriptSimulationParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetScriptSimulationParameter");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0))
    {
        bool goOn=true;
        int handle=luaWrap_lua_tointeger(L,1);
        if (handle==sim_handle_self)
        {
            handle=getCurrentScriptID(L);
            // Since this routine can also be called by joint callback and customization scripts, check for that here:
            CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(handle);
            if (it->getScriptType()==sim_scripttype_jointctrlcallback)
            {
                handle=it->getObjectIDThatScriptIsAttachedTo_callback();
                it=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(handle);
                if (it!=NULL)
                    handle=it->getScriptID();
                else
                    goOn=false;
            }
            if (it->getScriptType()==sim_scripttype_customizationscript)
            {
                handle=it->getObjectIDThatScriptIsAttachedTo_customization();
                it=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(handle);
                if (it!=NULL)
                    handle=it->getScriptID();
                else
                    goOn=false;
            }
        }
        if (goOn)
        {
            bool returnString=false;
            int ret=checkOneGeneralInputArgument(L,3,lua_arg_bool,0,true,false,&errorString);
            if ((ret==0)||(ret==2))
            {
                if (ret==2)
                    returnString=luaToBool(L,3);
                std::string parameterName(luaWrap_lua_tostring(L,2));
                if ( (handle!=sim_handle_tree)&&(handle!=sim_handle_chain)&&(handle!=sim_handle_all) )
                {
                    int l;
                    char* p=simGetScriptSimulationParameter_internal(handle,parameterName.c_str(),&l);
                    if (p!=NULL)
                    {
                        std::string a;
                        a.assign(p,l);
                        if (returnString)
                            luaWrap_lua_pushlstring(L,a.c_str(),a.length());
                        else
                            pushCorrectTypeOntoLuaStack(L,a);
                        simReleaseBuffer_internal(p);
                        LUA_END(1);
                    }
                }
                else
                {
                    std::vector<int> scriptHandles;
                    if (handle==sim_handle_tree)
                        getScriptTree(L,false,scriptHandles);
                    if (handle==sim_handle_chain)
                        getScriptChain(L,false,false,scriptHandles);
                    if (handle==sim_handle_all)
                    {
                        for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
                        {
                            CLuaScriptObject* it=App::ct->luaScriptContainer->allScripts[i];
                            int scrType=it->getScriptType();
                            if ((scrType==sim_scripttype_mainscript)||(scrType==sim_scripttype_childscript)) // make sure plugin script etc. are not included!
                                scriptHandles.push_back(it->getScriptID());
                        }
                    }
                    std::vector<std::string> retParams;
                    std::vector<int> retHandles;
                    for (int i=0;i<int(scriptHandles.size());i++)
                    {
                        int l;
                        char* p=simGetScriptSimulationParameter_internal(scriptHandles[i],parameterName.c_str(),&l);
                        if (p!=NULL)
                        {
                            std::string a;
                            a.assign(p,l);
                            simReleaseBuffer_internal(p);
                            retParams.push_back(a);
                            retHandles.push_back(scriptHandles[i]);
                        }
                    }
                    if (retParams.size()!=0)
                    { // now we push two tables onto the stack:
                        CInterfaceStack stack;
                        stack.pushTableOntoStack();
                        for (int i=0;i<int(retParams.size());i++)
                        {
                            stack.pushNumberOntoStack((double)i+1); // key
                            int t=getCorrectType(retParams[i]);
                            if (returnString)
                                t=4; // we force for strings!
                            if (t==0)
                                stack.pushNullOntoStack();
                            if ((t==1)||(t==2))
                                stack.pushBoolOntoStack(t==2);
                            if (t==3)
                            {
                                float v;
                                tt::getValidFloat(retParams[i],v);
                                stack.pushNumberOntoStack((double)v);
                            }
                            if (t==4)
                                stack.pushStringOntoStack(retParams[i].c_str(),0);
                            if (stack.getStackSize()<2)
                                stack.pushNullOntoStack();
                            stack.insertDataIntoStackTable();
                        }
                        stack.buildOntoLuaStack(L,true);
                        pushIntTableOntoStack(L,retHandles.size(),&retHandles[0]);
                        LUA_END(2);
                    }
                }
            }
        }
        else
            errorString=SIM_ERROR_NO_ASSOCIATED_CHILD_SCRIPT_FOUND;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetScriptSimulationParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetScriptSimulationParameter");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0,lua_arg_string,0))
    {
        bool goOn=true;
        int handle=luaWrap_lua_tointeger(L,1);
        if (handle==sim_handle_self)
        {
            handle=getCurrentScriptID(L);
            // Since this routine can also be called by joint callback and customization scripts, check for that here:
            CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(handle);
            if (it->getScriptType()==sim_scripttype_jointctrlcallback)
            {
                handle=it->getObjectIDThatScriptIsAttachedTo_callback();
                it=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(handle);
                if (it!=NULL)
                    handle=it->getScriptID();
                else
                    goOn=false;
            }
            if (it->getScriptType()==sim_scripttype_customizationscript)
            {
                handle=it->getObjectIDThatScriptIsAttachedTo_customization();
                it=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(handle);
                if (it!=NULL)
                    handle=it->getScriptID();
                else
                    goOn=false;
            }
        }
        if (goOn)
        {
            std::string parameterName(luaWrap_lua_tostring(L,2));
            size_t parameterValueLength;
            char* parameterValue=(char*)luaWrap_lua_tolstring(L,3,&parameterValueLength);
            if ( (handle!=sim_handle_tree)&&(handle!=sim_handle_chain) )
            {
                retVal=simSetScriptSimulationParameter_internal(handle,parameterName.c_str(),parameterValue,parameterValueLength);
            }
            else
            {
                std::vector<int> scriptHandles;
                if (handle==sim_handle_tree)
                    getScriptTree(L,false,scriptHandles);
                else
                    getScriptChain(L,false,false,scriptHandles);
                retVal=0;
                for (int i=0;i<int(scriptHandles.size());i++)
                {
                    if (simSetScriptSimulationParameter_internal(scriptHandles[i],parameterName.c_str(),parameterValue,parameterValueLength)==1)
                        retVal++;
                }
            }
        }
        else
            errorString=SIM_ERROR_NO_ASSOCIATED_CHILD_SCRIPT_FOUND;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simDisplayDialog(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simDisplayDialog");

    int retVal=-1;// error
    int elementHandle;
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_string,0,lua_arg_number,0,lua_arg_bool,0))
    {
        char* initialText=NULL;
        float* titleColor=NULL;
        float* dialogColor=NULL;
        bool modal=luaToBool(L,4);
        int dialogType=luaToInt(L,3);
        bool errorOccured=false;
        if (modal)
        {
            int currentScriptID=getCurrentScriptID(L);
            CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
            if (it!=NULL)
            {
                if (VThread::isCurrentThreadTheMainSimulationThread())
                {
                    errorString=SIM_ERROR_SCRIPT_MUST_RUN_IN_THREAD_FOR_MODAL_OPERATION;
                    errorOccured=true;
                }
                else
                {
                    if (dialogType==sim_dlgstyle_message)
                    {
                        errorString=SIM_ERROR_CANNOT_USE_THAT_STYLE_IN_MODAL_OPERATION;
                        errorOccured=true;
                    }
                }
            }
            else
                modal=false; // Should anyway never happen!!!
        }
        if (!errorOccured)
        {
            int res=checkOneGeneralInputArgument(L,5,lua_arg_string,0,true,true,&errorString);
            if (res!=-1)
            {
                if (res==2)
                {
                    std::string tmp(luaWrap_lua_tostring(L,5));
                    initialText=new char[tmp.length()+1];
                    initialText[tmp.length()]=0;
                    for (int i=0;i<int(tmp.length());i++)
                        initialText[i]=tmp[i];
                }
                int res=checkOneGeneralInputArgument(L,6,lua_arg_number,6,true,true,&errorString);
                if (res!=-1)
                {
                    if (res==2)
                    {
                        titleColor=new float[6];
                        getFloatsFromTable(L,6,6,titleColor);
                    }
                    int res=checkOneGeneralInputArgument(L,7,lua_arg_number,6,true,true,&errorString);
                    if (res!=-1)
                    {
                        if (res==2)
                        {
                            dialogColor=new float[6];
                            getFloatsFromTable(L,7,6,dialogColor);
                        }
                        retVal=simDisplayDialog_internal(luaWrap_lua_tostring(L,1),luaWrap_lua_tostring(L,2),dialogType,initialText,titleColor,dialogColor,&elementHandle);
                        if (retVal!=-1)
                        {
#ifdef SIM_WITH_GUI
                            CGenericDialog* it=App::ct->genericDialogContainer->getDialogFromID(retVal);
                            if (it!=NULL)
                            {
                                int currentScriptID=getCurrentScriptID(L);
                                CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
                                if ( (itScrObj->getScriptType()==sim_scripttype_mainscript)||(itScrObj->getScriptType()==sim_scripttype_childscript)||(itScrObj->getScriptType()==sim_scripttype_jointctrlcallback)||(itScrObj->getScriptType()==sim_scripttype_contactcallback) )//||(itScrObj->getScriptType()==sim_scripttype_generalcallback) )
                                {
                                    it->setCreatedInMainOrChildScript(true); // this will trigger automatic destruction at simulation end
                                    it->setPauseActive(false); // that dlg should be inactive during pause
                                }
                                it->setModal(modal);
                                if (modal)
                                {
                                    // Now wait here until a button was pressed! (or the simulation is aborted)
                                    while (it->getDialogResult()==sim_dlgret_still_open)
                                    {
                                        CThreadPool::switchBackToPreviousThread();
                                        if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                                            break;
                                    }
                                }
                            }
#endif
                        }
                    }
                }
            }           
        }
        delete[] initialText;
        delete[] titleColor;
        delete[] dialogColor;
    }

    if (retVal!=-1)
    {
        luaWrap_lua_pushnumber(L,retVal);
        luaWrap_lua_pushnumber(L,elementHandle);
        LUA_END(2);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetDialogResult(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetDialogResult");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simGetDialogResult_internal(luaWrap_lua_tointeger(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetDialogInput(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetDialogInput");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        char* v=simGetDialogInput_internal(luaWrap_lua_tointeger(L,1));
        if (v!=NULL)
        {
            luaWrap_lua_pushstring(L,v);
            simReleaseBuffer_internal(v);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simEndDialog(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simEndDialog");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simEndDialog_internal(luaWrap_lua_tointeger(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simStopSimulation(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simStopSimulation");

    int retVal=-1;// error
    retVal=simStopSimulation_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simPauseSimulation(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simPauseSimulation");

    int retVal=-1;// error
    retVal=simPauseSimulation_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simStartSimulation(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simStartSimulation");

    int retVal=-1;// error
    retVal=simStartSimulation_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectMatrix(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectMatrix");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        float arr[12];
        if (simGetObjectMatrix_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2),arr)==1)
        {
            pushFloatTableOntoStack(L,12,arr); // Success
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetObjectMatrix(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetObjectMatrix");

    int retVal=-1; // error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,12))
    {
        float arr[12];
        getFloatsFromTable(L,3,12,arr);
        retVal=simSetObjectMatrix_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2),arr);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetJointMatrix(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetJointMatrix");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float arr[12];
        if (simGetJointMatrix_internal(luaWrap_lua_tointeger(L,1),arr)==1)
        {
            pushFloatTableOntoStack(L,12,arr); // Success
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetSphericalJointMatrix(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetSphericalJointMatrix");

    int retVal=-1; // error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,12))
    {
        float arr[12];
        getFloatsFromTable(L,2,12,arr);
        retVal=simSetSphericalJointMatrix_internal(luaWrap_lua_tointeger(L,1),arr);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simBuildIdentityMatrix(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simBuildIdentityMatrix");

    float arr[12];
    simBuildIdentityMatrix_internal(arr);

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    pushFloatTableOntoStack(L,12,arr);
    LUA_END(1);
}

int _simCopyMatrix(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCopyMatrix");

    if (checkInputArguments(L,&errorString,lua_arg_number,12))
    {
        float arr[12];
        getFloatsFromTable(L,1,12,arr);
        pushFloatTableOntoStack(L,12,arr);
        LUA_END(1);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simBuildMatrix(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simBuildMatrix");

    if (checkInputArguments(L,&errorString,lua_arg_number,3,lua_arg_number,3))
    {
        float arr[12];
        float pos[3];
        float euler[3];
        getFloatsFromTable(L,1,3,pos);
        getFloatsFromTable(L,2,3,euler);
        if (simBuildMatrix_internal(pos,euler,arr)==1)
        {
            pushFloatTableOntoStack(L,12,arr);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetEulerAnglesFromMatrix(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetEulerAnglesFromMatrix");

    if (checkInputArguments(L,&errorString,lua_arg_number,12))
    {
        float arr[12];
        float euler[3];
        getFloatsFromTable(L,1,12,arr);
        if (simGetEulerAnglesFromMatrix_internal(arr,euler)==1)
        {
            pushFloatTableOntoStack(L,3,euler);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simInvertMatrix(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simInvertMatrix");
    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,12))
    {
        float arr[12];
        getFloatsFromTable(L,1,12,arr);
        retVal=simInvertMatrix_internal(arr);
        insertFloatsIntoTableAlreadyOnStack(L,1,12,arr);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simMultiplyMatrices(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simMultiplyMatrices");

    if (checkInputArguments(L,&errorString,lua_arg_number,12,lua_arg_number,12))
    {
        float inM0[12];
        float inM1[12];
        float outM[12];
        getFloatsFromTable(L,1,12,inM0);
        getFloatsFromTable(L,2,12,inM1);
        if (simMultiplyMatrices_internal(inM0,inM1,outM)!=-1)
        {
            pushFloatTableOntoStack(L,12,outM);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simInterpolateMatrices(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simInterpolateMatrices");

    if (checkInputArguments(L,&errorString,lua_arg_number,12,lua_arg_number,12,lua_arg_number,0))
    {
        float inM0[12];
        float inM1[12];
        float outM[12];
        getFloatsFromTable(L,1,12,inM0);
        getFloatsFromTable(L,2,12,inM1);
        if (simInterpolateMatrices_internal(inM0,inM1,luaToFloat(L,3),outM)!=-1)
        {
            pushFloatTableOntoStack(L,12,outM);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simMultiplyVector(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simMultiplyVector");

    if (checkInputArguments(L,&errorString,lua_arg_number,12,lua_arg_number,3))
    {
        float m[12];
        float vect[3];
        getFloatsFromTable(L,1,12,m);
        getFloatsFromTable(L,2,3,vect);
        if (simTransformVector_internal(m,vect)!=-1)
        {
            pushFloatTableOntoStack(L,3,vect);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetObjectParent(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectParent");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simGetObjectParent_internal(luaWrap_lua_tointeger(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectChild(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectChild");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simGetObjectChild_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetObjectParent(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetObjectParent");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_bool,0))
        retVal=simSetObjectParent_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2),luaWrap_lua_toboolean(L,3));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectType(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectType");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simGetObjectType_internal(luaWrap_lua_tointeger(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetJointType(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetJointType");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simGetJointType_internal(luaWrap_lua_tointeger(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetBoolParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetBoolParameter");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_bool,0))
        retVal=simSetBoolParameter_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_toboolean(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetBoolParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetBoolParameter");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int retVal=simGetBoolParameter_internal(luaWrap_lua_tointeger(L,1));
        if (retVal!=-1)
        {
            luaWrap_lua_pushboolean(L,retVal!=0);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetInt32Parameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetInt32Parameter");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetInt32Parameter_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetInt32Parameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetInt32Parameter");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int v;
        int retVal=simGetInt32Parameter_internal(luaWrap_lua_tointeger(L,1),&v);
        if (retVal!=-1)
        {
            luaWrap_lua_pushnumber(L,v);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetFloatParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetFloatParameter");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetFloatParameter_internal(luaWrap_lua_tointeger(L,1),luaToFloat(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetFloatParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetFloatParameter");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float v;
        int retVal=simGetFloatParameter_internal(luaWrap_lua_tointeger(L,1),&v);
        if (retVal!=-1)
        {
            luaWrap_lua_pushnumber(L,v);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(1);
}

int _simSetStringParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetStringParameter");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0))
        retVal=simSetStringParameter_internal(luaToInt(L,1),luaWrap_lua_tostring(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetStringParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetStringParameter");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        char* s=simGetStringParameter_internal(luaWrap_lua_tointeger(L,1));
        if (s!=NULL)
        {
            luaWrap_lua_pushstring(L,s);
            delete[] s;
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetArrayParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetArrayParameter");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_table,0))
    {
        int parameter=luaWrap_lua_tointeger(L,1);
        if (true)
        { // for now all array parameters are tables of 3 floats
            float theArray[3];
            getFloatsFromTable(L,2,3,theArray);
            retVal=simSetArrayParameter_internal(parameter,theArray);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetArrayParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetArrayParameter");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int parameter=luaWrap_lua_tointeger(L,1);
        if (true)
        { // for now all parameters are tables of 3 floats
            float theArray[3];
            int retVal=simGetArrayParameter_internal(parameter,theArray);
            if (retVal!=-1)
            {
                pushFloatTableOntoStack(L,3,theArray);
                LUA_END(1);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simRemoveObject(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRemoveObject");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simRemoveObject_internal(luaWrap_lua_tointeger(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRemoveModel(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRemoveModel");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simRemoveModel_internal(luaWrap_lua_tointeger(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetObjectName(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetObjectName");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0))
        retVal=simSetObjectName_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tostring(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetCollectionName(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetCollectionName");
    int retVal=-1;// error

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0))
        retVal=simSetCollectionName_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tostring(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetJointInterval(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetJointInterval");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        simBool cyclic;
        simFloat interval[2];
        if (simGetJointInterval_internal(luaWrap_lua_tointeger(L,1),&cyclic,interval)==1)
        {
            luaWrap_lua_pushboolean(L,cyclic!=0);
            pushFloatTableOntoStack(L,2,interval);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetJointInterval(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetJointInterval");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_bool,0,lua_arg_number,2))
    {
        float interval[2];
        getFloatsFromTable(L,3,2,interval);
        retVal=simSetJointInterval_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_toboolean(L,2),interval);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simLoadScene(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simLoadScene");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
        retVal=simLoadScene_internal(luaWrap_lua_tostring(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSaveScene(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSaveScene");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
        retVal=simSaveScene_internal(luaWrap_lua_tostring(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simLoadModel(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simLoadModel");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
        retVal=simLoadModel_internal(luaWrap_lua_tostring(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSaveModel(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSaveModel");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0))
        retVal=simSaveModel_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tostring(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simIsObjectInSelection(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simIsObjectInSelection");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simIsObjectInSelection_internal(luaWrap_lua_tointeger(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAddObjectToSelection(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddObjectToSelection");

    int retVal=-1;// error
    // We check if argument 1 is nil (special case):
    if (checkOneGeneralInputArgument(L,1,lua_arg_nil,0,false,true,NULL)==1) // we do not generate an error message!
    {
        retVal=1; // nothing happens
    }
    else
    {
        // We check if we have a table at position 1:
        if (!luaWrap_lua_istable(L,1))
        { // It is not a table!
            if (checkInputArguments(L,NULL,lua_arg_number,0,lua_arg_number,0)) // we don't generate an error
                retVal=simAddObjectToSelection_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2));
            else
            { // Maybe we have a special case with one argument only?
                // nil is a valid argument!
                if (checkInputArguments(L,NULL,lua_arg_nil,0)) // we don't generate an error
                    retVal=1;
                else
                {
                    if (checkInputArguments(L,&errorString,lua_arg_number,0))
                    {
                        if (luaWrap_lua_tointeger(L,1)==sim_handle_all)
                            retVal=simAddObjectToSelection_internal(luaWrap_lua_tointeger(L,1),-1);
                        else
                            checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0); // we just generate an error
                    }
                }
            }
        }
        else
        { // Ok we have a table. Now what size is it?
            int tableLen=int(luaWrap_lua_objlen(L,1));
            int* buffer=new int[tableLen];
            if (getIntsFromTable(L,1,tableLen,buffer))
            {
                for (int i=0;i<tableLen;i++)
                {
                    if (App::ct->objCont->getObject(buffer[i])!=NULL)
                        App::ct->objCont->addObjectToSelection(buffer[i]);
                }
                retVal=1;
            }
            else
                errorString=SIM_ERROR_TABLE_CONTAINS_INVALID_TYPES;
            delete[] buffer;
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRemoveObjectFromSelection(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRemoveObjectFromSelection");

    int retVal=-1;// error
    // We check if argument 1 is nil (special case):
    if (checkOneGeneralInputArgument(L,1,lua_arg_nil,0,false,true,NULL)==1) // we do not generate an error message!
        retVal=1; // nothing happens
    else
    {
        // We check if we have a table at position 1:
        if (!luaWrap_lua_istable(L,1))
        { // It is not a table!
            if (checkInputArguments(L,NULL,lua_arg_number,0,lua_arg_number,0)) // we don't generate an error
                retVal=simRemoveObjectFromSelection_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2));
            else
            {
                if (checkInputArguments(L,&errorString,lua_arg_number,0))
                {
                    if (luaWrap_lua_tointeger(L,1)==sim_handle_all)
                        retVal=simRemoveObjectFromSelection_internal(luaWrap_lua_tointeger(L,1),-1);
                    else
                        checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0); // we just generate an error
                }
            }
        }
        else
        { // Ok we have a table. Now what size is it?
            int tableLen=int(luaWrap_lua_objlen(L,1));
            int* buffer=new int[tableLen];
            if (getIntsFromTable(L,1,tableLen,buffer))
            {
                for (int i=0;i<tableLen;i++)
                    retVal=simRemoveObjectFromSelection_internal(sim_handle_single,buffer[i]);
            }
            else
                errorString=SIM_ERROR_TABLE_CONTAINS_INVALID_TYPES;
            delete[] buffer;
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectSelectionSize(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectSelectionSize");

    int retVal=simGetObjectSelectionSize_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectLastSelection(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectLastSelection");

    int retVal=simGetObjectLastSelection_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectSelection(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectSelection");

    int selSize=App::ct->objCont->getSelSize();
    if (selSize!=0)
    {
        int* sel=new int[selSize];
        int res=simGetObjectSelection_internal(sel);
        if (res>0)
        {
            pushIntTableOntoStack(L,selSize,sel);
            delete[] sel;
            LUA_END(1);
        }
        delete[] sel;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetRealTimeSimulation(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetRealTimeSimulation");

    int retVal=simGetRealTimeSimulation_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simLaunchExecutable(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simLaunchExecutable");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string file(luaWrap_lua_tostring(L,1));
        int res=checkOneGeneralInputArgument(L,2,lua_arg_string,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            std::string args;
            if (res==2)
                args=luaWrap_lua_tostring(L,2);
            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                int showStatus=1;
                if (res==2)
                    showStatus=luaToInt(L,3);
                int sh=VVARIOUS_SHOWNORMAL;
                if (showStatus==0)
                    sh=VVARIOUS_HIDE;
                if (VVarious::executeExternalApplication(file,args,App::directories->executableDirectory,sh)) // executable directory needed because otherwise the shellExecute command might switch directories!
                    retVal=1;
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetConfigForTipPose(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetConfigForTipPose");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,1,lua_arg_number,0,lua_arg_number,0))
    {
        int ikGroupHandle=luaWrap_lua_tointeger(L,1);
        std::vector<int> jointHandles;
        int jointCnt=int(luaWrap_lua_objlen(L,2));
        jointHandles.resize(jointCnt);
        getIntsFromTable(L,2,jointCnt,&jointHandles[0]);
        float thresholdDist=luaWrap_lua_tonumber(L,3);
        int maxTimeInMs=luaWrap_lua_tointeger(L,4);
        float metric[4]={1.0,1.0,1.0,0.1f};
        int res=checkOneGeneralInputArgument(L,5,lua_arg_number,4,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
                getFloatsFromTable(L,5,4,metric);
            int collisionPairCnt=0;
            std::vector<int> _collisionPairs;
            int* collisionPairs=NULL;
            res=checkOneGeneralInputArgument(L,6,lua_arg_number,-1,true,true,&errorString);
            if (res>=0)
            {
                if (res==2)
                {
                    collisionPairCnt=int(luaWrap_lua_objlen(L,6))/2;
                    if (collisionPairCnt>0)
                    {
                        _collisionPairs.resize(collisionPairCnt*2);
                        getIntsFromTable(L,6,collisionPairCnt*2,&_collisionPairs[0]);
                        collisionPairs=&_collisionPairs[0];
                    }
                }
                std::vector<int> _jointOptions;
                int* jointOptions=NULL;
                res=checkOneGeneralInputArgument(L,7,lua_arg_number,jointCnt,true,true,&errorString);
                if (res>=0)
                {
                    if (res==2)
                    {
                        _jointOptions.resize(jointCnt);
                        getIntsFromTable(L,7,jointCnt,&_jointOptions[0]);
                        jointOptions=&_jointOptions[0];
                    }

                    res=checkOneGeneralInputArgument(L,8,lua_arg_number,jointCnt,true,true,&errorString);
                    if (res>=0)
                    {
                        std::vector<float> _lowLimits;
                        _lowLimits.resize(jointCnt);
                        std::vector<float> _ranges;
                        _ranges.resize(jointCnt);
                        float* lowLimits=NULL;
                        float* ranges=NULL;
                        if (res==2)
                        {
                            getFloatsFromTable(L,8,jointCnt,&_lowLimits[0]);
                            lowLimits=&_lowLimits[0];
                        }
                        res=checkOneGeneralInputArgument(L,9,lua_arg_number,jointCnt,lowLimits==NULL,lowLimits==NULL,&errorString);
                        if (res>=0)
                        {
                            if (res==2)
                            {
                                getFloatsFromTable(L,9,jointCnt,&_ranges[0]);
                                ranges=&_ranges[0];
                            }
                            std::vector<float> foundConfig;
                            foundConfig.resize(jointCnt);
                            res=simGetConfigForTipPose_internal(ikGroupHandle,jointCnt,&jointHandles[0],thresholdDist,maxTimeInMs,&foundConfig[0],metric,collisionPairCnt,collisionPairs,jointOptions,lowLimits,ranges,NULL);
                            if (res>0)
                            {
                                pushFloatTableOntoStack(L,jointCnt,&foundConfig[0]);
                                LUA_END(1);
                            }
                        }
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGenerateIkPath(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGenerateIkPath");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,1,lua_arg_number,0))
    {
        int ikGroupHandle=luaWrap_lua_tointeger(L,1);
        std::vector<int> jointHandles;
        int jointCnt=int(luaWrap_lua_objlen(L,2));
        jointHandles.resize(jointCnt);
        getIntsFromTable(L,2,jointCnt,&jointHandles[0]);
        int ptCnt=luaWrap_lua_tonumber(L,3);
        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,-1,true,true,&errorString);
        if (res>=0)
        {
            int collisionPairCnt=0;
            std::vector<int> _collisionPairs;
            int* collisionPairs=NULL;
            if (res==2)
            {
                collisionPairCnt=int(luaWrap_lua_objlen(L,4))/2;
                if (collisionPairCnt>0)
                {
                    _collisionPairs.resize(collisionPairCnt*2);
                    getIntsFromTable(L,4,collisionPairCnt*2,&_collisionPairs[0]);
                    collisionPairs=&_collisionPairs[0];
                }
            }
            res=checkOneGeneralInputArgument(L,5,lua_arg_number,jointCnt,true,true,&errorString);
            if (res>=0)
            {
                std::vector<int> _jointOptions;
                int* jointOptions=NULL;
                if (res==2)
                {
                    _jointOptions.resize(jointCnt);
                    getIntsFromTable(L,5,jointCnt,&_jointOptions[0]);
                    jointOptions=&_jointOptions[0];
                }
                float* path=simGenerateIkPath_internal(ikGroupHandle,jointCnt,&jointHandles[0],ptCnt,collisionPairCnt,collisionPairs,jointOptions,NULL);
                if (path!=NULL)
                {
                    pushFloatTableOntoStack(L,jointCnt*ptCnt,path);
                    simReleaseBuffer_internal((char*)path);
                    LUA_END(1);
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetExtensionString(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetExtensionString");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int objHandle=luaWrap_lua_tointeger(L,1);
        int index=luaWrap_lua_tointeger(L,2);
        int res=checkOneGeneralInputArgument(L,3,lua_arg_string,0,true,true,&errorString);
        if (res>=0)
        {
            std::string key;
            if (res==2)
                key=luaWrap_lua_tostring(L,3);
            char* str=simGetExtensionString_internal(objHandle,index,key.c_str());
            if (str!=NULL)
            {
                luaWrap_lua_pushstring(L,str);
                simReleaseBuffer_internal(str);
                LUA_END(1);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}


int _simComputeMassAndInertia(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simComputeMassAndInertia");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int shapeHandle=luaWrap_lua_tointeger(L,1);
        float density=luaWrap_lua_tonumber(L,2);
        retVal=simComputeMassAndInertia_internal(shapeHandle,density);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushinteger(L,retVal);
    LUA_END(1);
}

int _simTest(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simTest");
    LUA_END(0);
}


int _simSetNavigationMode(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetNavigationMode");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simSetNavigationMode_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetNavigationMode(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetNavigationMode");

    int retVal=-1; //error
    retVal=simGetNavigationMode_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetPage(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetPage");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simSetPage_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetPage(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetPage");
    int retVal=-1; //error

    retVal=simGetPage_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simReleaseScriptRawBuffer(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simReleaseScriptRawBuffer");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        if (handle==sim_handle_self)
            handle=getCurrentScriptID(L);
        if ( (handle!=sim_handle_tree)&&(handle!=sim_handle_chain) )
            retVal=simReleaseScriptRawBuffer_internal(handle,luaToInt(L,2));
        else
        {
            std::vector<int> scriptHandles;
            if (handle==sim_handle_tree)
                getScriptTree(L,false,scriptHandles);
            else
                getScriptChain(L,false,false,scriptHandles);
            for (int i=0;i<int(scriptHandles.size());i++)
                retVal=simReleaseScriptRawBuffer_internal(scriptHandles[i],sim_handle_all);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCopyPasteObjects(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCopyPasteObjects");

    if (checkInputArguments(L,&errorString,lua_arg_number,1,lua_arg_number,0))
    {
        int objCnt=luaWrap_lua_objlen(L,1);
        if (checkInputArguments(L,&errorString,lua_arg_number,objCnt,lua_arg_number,0))
        {
            std::vector<int> objectHandles;
            objectHandles.resize(objCnt,0);
            getIntsFromTable(L,1,objCnt,&objectHandles[0]);
            int retCnt=simCopyPasteObjects_internal(&objectHandles[0],objCnt,luaToInt(L,2));
            pushIntTableOntoStack(L,retCnt,&objectHandles[0]);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simScaleSelectedObjects(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simScaleSelectedObjects");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_bool,0))
        retVal=simScaleSelectedObjects_internal(luaToFloat(L,1),luaToBool(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simScaleObjects(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simScaleObjects");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,1,lua_arg_number,0,lua_arg_bool,0))
    {
        int objCnt=luaWrap_lua_objlen(L,1);
        if (checkInputArguments(L,&errorString,lua_arg_number,objCnt,lua_arg_number,0,lua_arg_bool,0))
        {
            std::vector<int> objectHandles;
            objectHandles.resize(objCnt,0);
            getIntsFromTable(L,1,objCnt,&objectHandles[0]);
            retVal=simScaleObjects_internal(&objectHandles[0],objCnt,luaToFloat(L,2),luaToBool(L,3));
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simDeleteSelectedObjects(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simDeleteSelectedObjects");

    int retVal=simDeleteSelectedObjects_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectUniqueIdentifier(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectUniqueIdentifier");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaToInt(L,1);
        if (handle==sim_handle_all)
        {
            int cnt=App::ct->objCont->objectList.size();
            int* buffer=new int[cnt];
            if (simGetObjectUniqueIdentifier_internal(handle,buffer)!=-1)
            {
                pushIntTableOntoStack(L,cnt,buffer);
                delete[] buffer;
                LUA_END(1);
            }
            delete[] buffer;
        }
        else
        {
            int retVal;
            if (simGetObjectUniqueIdentifier_internal(handle,&retVal)!=-1)
            {
                luaWrap_lua_pushnumber(L,retVal);
                LUA_END(1);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetNameSuffix(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetNameSuffix");

    if (checkInputArguments(L,NULL,lua_arg_nil,0))
    { // we want the suffix of current script
        std::string suffTxt;
        int suffixNumber=-1;
        luaWrap_lua_getglobal(L,SIM_SCRIPT_NAME_SUFFIX);
        if (luaWrap_lua_isstring(L,-1))
            suffTxt=luaWrap_lua_tostring(L,-1);
        luaWrap_lua_pop(L,1);
        if (suffTxt!="")
            tt::getValidInt(suffTxt,suffixNumber);
        luaWrap_lua_pushnumber(L,suffixNumber);
        LUA_END(1);
    }
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    { // we want the suffix of the provided name
        std::string nameWithSuffix(luaWrap_lua_tostring(L,1));
        std::string name(tt::getNameWithoutSuffixNumber(nameWithSuffix.c_str(),true));
        int suffixNumber=tt::getNameSuffixNumber(nameWithSuffix.c_str(),true);
        luaWrap_lua_pushnumber(L,suffixNumber);
        luaWrap_lua_pushstring(L,name.c_str());
        LUA_END(2);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetNameSuffix(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetNameSuffix");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int nb=luaWrap_lua_tointeger(L,1);
        std::string suffTxt("");
        if (nb>=0)
            suffTxt=tt::FNb(nb);
        std::string tmp(SIM_SCRIPT_NAME_SUFFIX);
        tmp+="='";
        tmp+=suffTxt;
        tmp+="'";
        luaWrap_luaL_dostring(L,tmp.c_str());
        retVal=1;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetThreadSwitchTiming(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetThreadSwitchTiming");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        if (CThreadPool::setThreadSwitchTiming(luaWrap_lua_tointeger(L,1)))
            retVal=1;
        else
            retVal=0;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetThreadAutomaticSwitch(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetThreadAutomaticSwitch");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_bool,0))
    {
        if (CThreadPool::setThreadAutomaticSwitch(luaToBool(L,1)))
            retVal=1;
        else
            retVal=0;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetThreadAutomaticSwitch(luaWrap_lua_State* L)
{ // doesn't generate an error
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetThreadAutomaticSwitch");
    luaWrap_lua_pushboolean(L,CThreadPool::getThreadAutomaticSwitch());
    LUA_END(1);
}

int _simSetThreadResumeLocation(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetThreadResumeLocation");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        if (CThreadPool::setThreadResumeLocation(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2)))
            retVal=1;
        else
            retVal=0;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simResumeThreads(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simResumeThreads");

    int retVal=-1;
    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if (it->getScriptType()==sim_scripttype_mainscript)
    {
        if (checkInputArguments(L,&errorString,lua_arg_number,0))
        {
            int loc=luaWrap_lua_tointeger(L,1);
            App::ct->calcInfo->runningThreadedScriptStart(0);
            int startTime=VDateTime::getTimeInMs();
            retVal=CThreadPool::handleAllThreads_withResumeLocation(loc);
            App::ct->calcInfo->mainScriptPaused(VDateTime::getTimeDiffInMs(startTime));
            App::ct->calcInfo->runningThreadedScriptEnd();
        }
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}



int _simSwitchThread(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSwitchThread");

    int retVal=-1;
    if (CThreadPool::switchBackToPreviousThread())
        retVal=1;
    else
        retVal=0;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreateIkGroup(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateIkGroup");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int options=luaToInt(L,1);
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,2,true,true,&errorString);
        if (res>=0)
        {
            int intParams[2];
            int* intP=NULL;
            if (res==2)
            {
                getIntsFromTable(L,2,2,intParams);
                intP=intParams;
            }
            res=checkOneGeneralInputArgument(L,3,lua_arg_number,4,true,true,&errorString);
            if (res>=0)
            {
                float floatParams[4];
                float* floatP=NULL;
                if (res==2)
                {
                    getFloatsFromTable(L,3,4,floatParams);
                    floatP=floatParams;
                }
                retVal=simCreateIkGroup_internal(options,intP,floatP,NULL);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRemoveIkGroup(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRemoveIkGroup");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaToInt(L,1);
        retVal=simRemoveIkGroup_internal(handle);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreateIkElement(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateIkElement");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,4))
    {
        int ikGroup=luaToInt(L,1);
        int options=luaToInt(L,2);
        int intParams[4];
        getIntsFromTable(L,3,4,intParams);

        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,4,true,true,&errorString);
        if (res>=0)
        {
            float floatParams[4];
            float* floatP=NULL;
            if (res==2)
            {
                getFloatsFromTable(L,4,4,floatParams);
                floatP=floatParams;
            }
            retVal=simCreateIkElement_internal(ikGroup,options,intParams,floatP,NULL);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreateCollection(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateCollection");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0))
    {
        std::string collName(luaWrap_lua_tostring(L,1));
        int options=luaToInt(L,2);
        retVal=simCreateCollection_internal(collName.c_str(),options);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAddObjectToCollection(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddObjectToCollection");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int collHandle=luaToInt(L,1);
        int objHandle=luaToInt(L,2);
        int what=luaToInt(L,3);
        int options=luaToInt(L,4);
        retVal=simAddObjectToCollection_internal(collHandle,objHandle,what,options);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSaveImage(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSaveImage");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,2,lua_arg_number,0,lua_arg_string,0,lua_arg_number,0))
    {
        size_t dataLength;
        char* data=((char*)luaWrap_lua_tolstring(L,1,&dataLength));
        std::string img(data,dataLength);
        int res[2];
        getIntsFromTable(L,2,2,res);
        int options=luaToInt(L,3);
        std::string filename(luaWrap_lua_tostring(L,4));
        int quality=luaToInt(L,5);
        int channels=3;
        if (options&1)
            channels=4;
        if (int(dataLength)>=res[0]*res[1]*channels)
        {
            if ((res[0]>0)&&(res[1]>0))
            {
                retVal=simSaveImage_internal((unsigned char*)&img[0],res,options,filename.c_str(),quality,NULL);
            }
            else
                errorString=SIM_ERROR_INVALID_RESOLUTION;
        }
        else
            errorString=SIM_ERROR_ONE_STRING_SIZE_IS_WRONG;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simLoadImage(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simLoadImage");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0))
    {
        int options=luaToInt(L,1);
        std::string filename(luaWrap_lua_tostring(L,2));
        int resol[2];
        unsigned char* img=simLoadImage_internal(resol,options,filename.c_str(),NULL);
        if (img!=NULL)
        {
            int s=resol[0]*resol[1]*3;
            if (options&1)
                s=resol[0]*resol[1]*4;
            luaWrap_lua_pushlstring(L,(const char*)img,s);
            delete[] ((char*)img);
            pushIntTableOntoStack(L,2,resol);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetScaledImage(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetScaledImage");

    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,2,lua_arg_number,2,lua_arg_number,0))
    {
        size_t dataLength;
        char* data=((char*)luaWrap_lua_tolstring(L,1,&dataLength));
        std::string imgIn(data,dataLength);
        int resIn[2];
        getIntsFromTable(L,2,2,resIn);
        int resOut[2];
        getIntsFromTable(L,3,2,resOut);
        int options=luaToInt(L,4);
        int channelsIn=3;
        if (options&1)
            channelsIn=4;
        if (int(dataLength)>=resIn[0]*resIn[1]*channelsIn)
        {
            if ((resIn[0]>0)&&(resIn[1]>0)&&(resOut[0]>0)&&(resOut[1]>0))
            {
                unsigned char* imgOut=simGetScaledImage_internal((unsigned char*)&imgIn[0],resIn,resOut,options,NULL);
                if (imgOut!=NULL)
                {
                    int s=resOut[0]*resOut[1]*3;
                    if (options&2)
                        s=resOut[0]*resOut[1]*4;
                    luaWrap_lua_pushlstring(L,(const char*)imgOut,s);
                    delete[] ((char*)imgOut);
                    pushIntTableOntoStack(L,2,resOut);
                    LUA_END(2);
                }
            }
            else
                errorString=SIM_ERROR_INVALID_RESOLUTION;
        }
        else
            errorString=SIM_ERROR_ONE_STRING_SIZE_IS_WRONG;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simTransformImage(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simTransformImage");
    int retVal=-1;

    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,2,lua_arg_number,0))
    {
        size_t dataLength;
        char* data=((char*)luaWrap_lua_tolstring(L,1,&dataLength));
        int resol[2];
        getIntsFromTable(L,2,2,resol);
        int options=luaToInt(L,3);
        int channels=3;
        if (options&1)
            channels=4;
        if (int(dataLength)>=resol[0]*resol[1]*channels)
        {
            if ((resol[0]>0)&&(resol[1]>0))
                retVal=simTransformImage_internal((unsigned char*)data,resol,options,NULL,NULL,NULL);
            else
                errorString=SIM_ERROR_INVALID_RESOLUTION;
        }
        else
            errorString=SIM_ERROR_ONE_STRING_SIZE_IS_WRONG;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetQHull(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetQHull");

    if (checkInputArguments(L,&errorString,lua_arg_number,9))
    {
        int vl=luaWrap_lua_objlen(L,1);
        if (checkInputArguments(L,&errorString,lua_arg_number,vl))
        {
            float* vertices=new float[vl];
            getFloatsFromTable(L,1,vl,vertices);
            float* vertOut;
            int vertOutL;
            int* indOut;
            int indOutL;
            if (simGetQHull_internal(vertices,vl,&vertOut,&vertOutL,&indOut,&indOutL,0,NULL))
            {
                pushFloatTableOntoStack(L,vertOutL,vertOut);
                pushIntTableOntoStack(L,indOutL,indOut);
                delete[] vertOut;
                delete[] indOut;
                LUA_END(2);
            }
            delete[] vertices;
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetDecimatedMesh(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetDecimatedMesh");

    if (checkInputArguments(L,&errorString,lua_arg_number,9,lua_arg_number,6,lua_arg_number,0))
    {
        int vl=luaWrap_lua_objlen(L,1);
        int il=luaWrap_lua_objlen(L,2);
        float percentage=luaToFloat(L,3);
        if (checkInputArguments(L,&errorString,lua_arg_number,vl,lua_arg_number,il,lua_arg_number,0))
        {
            float* vertices=new float[vl];
            getFloatsFromTable(L,1,vl,vertices);
            int* indices=new int[il];
            getIntsFromTable(L,2,il,indices);
            float* vertOut;
            int vertOutL;
            int* indOut;
            int indOutL;
            if (simGetDecimatedMesh_internal(vertices,vl,indices,il,&vertOut,&vertOutL,&indOut,&indOutL,percentage,0,NULL))
            {
                pushFloatTableOntoStack(L,vertOutL,vertOut);
                pushIntTableOntoStack(L,indOutL,indOut);
                delete[] vertOut;
                delete[] indOut;
                LUA_END(2);
            }
            delete[] vertices;
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simExportIk(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simExportIk");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string pathAndFilename(luaWrap_lua_tostring(L,1));
        retVal=simExportIk_internal(pathAndFilename.c_str(),0,NULL);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simComputeJacobian(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simComputeJacobian");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simComputeJacobian_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2),NULL);

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simPackInt32Table(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simPackInt32Table");

    if (luaWrap_lua_gettop(L)>0)
    {
        if (luaWrap_lua_istable(L,1))
        {
            int startIndex=0;
            int count=0;
            int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    startIndex=luaToInt(L,2);

                res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        count=luaToInt(L,3);

                    int tableSize=int(luaWrap_lua_objlen(L,1));

                    if (count==0)
                        count=tableSize-startIndex;
                    if (count>tableSize-startIndex)
                        count=tableSize-startIndex;
                    if (count>0)
                    {
                        char* data=new char[sizeof(int)*count];
                        for (int i=0;i<count;i++)
                        {
                            luaWrap_lua_rawgeti(L,1,i+1+startIndex);
                            int v=luaWrap_lua_tointeger(L,-1);
                            data[4*i+0]=((char*)&v)[0];
                            data[4*i+1]=((char*)&v)[1];
                            data[4*i+2]=((char*)&v)[2];
                            data[4*i+3]=((char*)&v)[3];
                            luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                        }
                        luaWrap_lua_pushlstring(L,(const char*)data,count*sizeof(int));
                        delete[] data;
                        LUA_END(1);
                    }
                }
            }
        }
        else
            errorString=SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG;
    }
    else
        errorString=SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simPackUInt32Table(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simPackUInt32Table");

    if (luaWrap_lua_gettop(L)>0)
    {
        if (luaWrap_lua_istable(L,1))
        {
            int startIndex=0;
            int count=0;
            int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    startIndex=luaToInt(L,2);

                res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        count=luaToInt(L,3);

                    int tableSize=int(luaWrap_lua_objlen(L,1));

                    if (count==0)
                        count=tableSize-startIndex;
                    if (count>tableSize-startIndex)
                        count=tableSize-startIndex;
                    if (count>0)
                    {
                        char* data=new char[sizeof(unsigned int)*count];
                        for (int i=0;i<count;i++)
                        {
                            luaWrap_lua_rawgeti(L,1,i+1+startIndex);
                            luaWrap_lua_Number na=luaWrap_lua_tonumber(L,-1);
                            if (na<0.0)
                                na=0.0;
                            unsigned int v=(unsigned int)na;
                            data[4*i+0]=((char*)&v)[0];
                            data[4*i+1]=((char*)&v)[1];
                            data[4*i+2]=((char*)&v)[2];
                            data[4*i+3]=((char*)&v)[3];
                            luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                        }
                        luaWrap_lua_pushlstring(L,(const char*)data,count*sizeof(unsigned int));
                        delete[] data;
                        LUA_END(1);
                    }
                }
            }
        }
        else
            errorString=SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG;
    }
    else
        errorString=SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simPackFloatTable(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simPackFloatTable");

    if (luaWrap_lua_gettop(L)>0)
    {
        if (luaWrap_lua_istable(L,1))
        {
            int startIndex=0;
            int count=0;
            int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    startIndex=luaToInt(L,2);

                res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        count=luaToInt(L,3);

                    int tableSize=int(luaWrap_lua_objlen(L,1));

                    if (count==0)
                        count=tableSize-startIndex;
                    if (count>tableSize-startIndex)
                        count=tableSize-startIndex;
                    if (count>0)
                    {
                        char* data=new char[sizeof(float)*count];
                        for (int i=0;i<count;i++)
                        {
                            luaWrap_lua_rawgeti(L,1,i+1+startIndex);
                            float v=(float)luaWrap_lua_tonumber(L,-1);
                            data[4*i+0]=((char*)&v)[0];
                            data[4*i+1]=((char*)&v)[1];
                            data[4*i+2]=((char*)&v)[2];
                            data[4*i+3]=((char*)&v)[3];
                            luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                        }
                        luaWrap_lua_pushlstring(L,(const char*)data,count*sizeof(float));
                        delete[] data;
                        LUA_END(1);
                    }
                }
            }
        }
        else
            errorString=SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG;
    }
    else
        errorString=SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simPackDoubleTable(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simPackDoubleTable");

    if (luaWrap_lua_gettop(L)>0)
    {
        if (luaWrap_lua_istable(L,1))
        {
            int startIndex=0;
            int count=0;
            int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    startIndex=luaToInt(L,2);

                res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        count=luaToInt(L,3);

                    int tableSize=int(luaWrap_lua_objlen(L,1));

                    if (count==0)
                        count=tableSize-startIndex;
                    if (count>tableSize-startIndex)
                        count=tableSize-startIndex;
                    if (count>0)
                    {
                        char* data=new char[sizeof(double)*count];
                        for (int i=0;i<count;i++)
                        {
                            luaWrap_lua_rawgeti(L,1,i+1+startIndex);
                            double v=luaWrap_lua_tonumber(L,-1);
                            data[sizeof(double)*i+0]=((char*)&v)[0];
                            data[sizeof(double)*i+1]=((char*)&v)[1];
                            data[sizeof(double)*i+2]=((char*)&v)[2];
                            data[sizeof(double)*i+3]=((char*)&v)[3];
                            data[sizeof(double)*i+4]=((char*)&v)[4];
                            data[sizeof(double)*i+5]=((char*)&v)[5];
                            data[sizeof(double)*i+6]=((char*)&v)[6];
                            data[sizeof(double)*i+7]=((char*)&v)[7];
                            luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                        }
                        luaWrap_lua_pushlstring(L,(const char*)data,count*sizeof(double));
                        delete[] data;
                        LUA_END(1);
                    }
                }
            }
        }
        else
            errorString=SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG;
    }
    else
        errorString=SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simPackUInt8Table(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simPackUInt8Table");

    if (luaWrap_lua_gettop(L)>0)
    {
        if (luaWrap_lua_istable(L,1))
        {
            int startIndex=0;
            int count=0;
            int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    startIndex=luaToInt(L,2);

                res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        count=luaToInt(L,3);

                    int tableSize=int(luaWrap_lua_objlen(L,1));

                    if (count==0)
                        count=tableSize-startIndex;
                    if (count>tableSize-startIndex)
                        count=tableSize-startIndex;
                    if (count>0)
                    {
                        char* data=new char[sizeof(char)*count];
                        for (int i=0;i<count;i++)
                        {
                            luaWrap_lua_rawgeti(L,1,i+1+startIndex);
                            unsigned char v=(unsigned char)luaWrap_lua_tointeger(L,-1);
                            data[i]=((char*)&v)[0];
                            luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                        }
                        luaWrap_lua_pushlstring(L,(const char*)data,count*sizeof(char));
                        delete[] data;
                        LUA_END(1);
                    }
                }
            }
        }
        else
            errorString=SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG;
    }
    else
        errorString=SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simPackUInt16Table(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simPackUInt16Table");

    if (luaWrap_lua_gettop(L)>0)
    {
        if (luaWrap_lua_istable(L,1))
        {
            int startIndex=0;
            int count=0;
            int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    startIndex=luaToInt(L,2);

                res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        count=luaToInt(L,3);

                    int tableSize=int(luaWrap_lua_objlen(L,1));

                    if (count==0)
                        count=tableSize-startIndex;
                    if (count>tableSize-startIndex)
                        count=tableSize-startIndex;
                    if (count>0)
                    {
                        char* data=new char[sizeof(short)*count];
                        for (int i=0;i<count;i++)
                        {
                            luaWrap_lua_rawgeti(L,1,i+1+startIndex);
                            unsigned short v=(unsigned short)luaWrap_lua_tointeger(L,-1);
                            data[2*i+0]=((char*)&v)[0];
                            data[2*i+1]=((char*)&v)[1];
                            luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                        }
                        luaWrap_lua_pushlstring(L,(const char*)data,count*sizeof(short));
                        delete[] data;
                        LUA_END(1);
                    }
                }
            }
        }
        else
            errorString=SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG;
    }
    else
        errorString=SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simUnpackInt32Table(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simUnpackInt32Table");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        int startIndex=0;
        int count=0;
        int additionalCharOffset=0;
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            if (res==2)
                startIndex=luaToInt(L,2);

            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    count=luaToInt(L,3);
                
                res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        additionalCharOffset=luaToInt(L,4);

                    size_t dataLength;
                    char* data=((char*)luaWrap_lua_tolstring(L,1,&dataLength))+additionalCharOffset;
                    dataLength=sizeof(int)*((dataLength-additionalCharOffset)/sizeof(int));
                    int packetCount=dataLength/sizeof(int);
                    if (count==0)
                        count=int(1999999999);

                    if ((startIndex>=0)&&(startIndex<packetCount))
                    {
                        if (startIndex+count>packetCount)
                            count=packetCount-startIndex;

                        int* theInts=new int[count];
                        for (int i=0;i<int(count);i++)
                        {
                            int v;
                            ((char*)&v)[0]=data[sizeof(int)*(i+startIndex)+0];
                            ((char*)&v)[1]=data[sizeof(int)*(i+startIndex)+1];
                            ((char*)&v)[2]=data[sizeof(int)*(i+startIndex)+2];
                            ((char*)&v)[3]=data[sizeof(int)*(i+startIndex)+3];
                            theInts[i]=v;
                        }
                        pushIntTableOntoStack(L,count,theInts);
                        delete[] theInts;
                        LUA_END(1);
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simUnpackUInt32Table(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simUnpackUInt32Table");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        int startIndex=0;
        int count=0;
        int additionalCharOffset=0;
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            if (res==2)
                startIndex=luaToInt(L,2);

            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    count=luaToInt(L,3);

                res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        additionalCharOffset=luaToInt(L,4);

                    size_t dataLength;
                    char* data=((char*)luaWrap_lua_tolstring(L,1,&dataLength))+additionalCharOffset;
                    dataLength=sizeof(unsigned int)*((dataLength-additionalCharOffset)/sizeof(unsigned int));
                    int packetCount=dataLength/sizeof(unsigned int);
                    if (count==0)
                        count=int(1999999999);

                    if ((startIndex>=0)&&(startIndex<packetCount))
                    {
                        if (startIndex+count>packetCount)
                            count=packetCount-startIndex;

                        unsigned int* theInts=new unsigned int[count];
                        for (int i=0;i<int(count);i++)
                        {
                            unsigned int v;
                            ((char*)&v)[0]=data[sizeof(unsigned int)*(i+startIndex)+0];
                            ((char*)&v)[1]=data[sizeof(unsigned int)*(i+startIndex)+1];
                            ((char*)&v)[2]=data[sizeof(unsigned int)*(i+startIndex)+2];
                            ((char*)&v)[3]=data[sizeof(unsigned int)*(i+startIndex)+3];
                            theInts[i]=v;
                        }
                        pushUIntTableOntoStack(L,count,theInts);
                        delete[] theInts;
                        LUA_END(1);
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simUnpackFloatTable(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simUnpackFloatTable");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        int startIndex=0;
        int count=0;
        int additionalCharOffset=0;
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            if (res==2)
                startIndex=luaToInt(L,2);

            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    count=luaToInt(L,3);
                
                res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        additionalCharOffset=luaToInt(L,4);

                    size_t dataLength;
                    char* data=((char*)luaWrap_lua_tolstring(L,1,&dataLength))+additionalCharOffset;
                    dataLength=sizeof(float)*((dataLength-additionalCharOffset)/sizeof(float));
                    int packetCount=dataLength/sizeof(float);
                    if (count==0)
                        count=int(1999999999);

                    if ((startIndex>=0)&&(startIndex<packetCount))
                    {
                        if (startIndex+count>packetCount)
                            count=packetCount-startIndex;

                        float* theFloats=new float[count];
                        for (int i=0;i<int(count);i++)
                        {
                            float v;
                            ((char*)&v)[0]=data[sizeof(float)*(i+startIndex)+0];
                            ((char*)&v)[1]=data[sizeof(float)*(i+startIndex)+1];
                            ((char*)&v)[2]=data[sizeof(float)*(i+startIndex)+2];
                            ((char*)&v)[3]=data[sizeof(float)*(i+startIndex)+3];
                            theFloats[i]=v;
                        }
                        pushFloatTableOntoStack(L,count,theFloats);
                        delete[] theFloats;
                        LUA_END(1);
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simUnpackDoubleTable(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simUnpackDoubleTable");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        int startIndex=0;
        int count=0;
        int additionalCharOffset=0;
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            if (res==2)
                startIndex=luaToInt(L,2);

            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    count=luaToInt(L,3);

                res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        additionalCharOffset=luaToInt(L,4);

                    size_t dataLength;
                    char* data=((char*)luaWrap_lua_tolstring(L,1,&dataLength))+additionalCharOffset;
                    dataLength=sizeof(double)*((dataLength-additionalCharOffset)/sizeof(double));
                    int packetCount=dataLength/sizeof(double);
                    if (count==0)
                        count=int(1999999999);

                    if ((startIndex>=0)&&(startIndex<packetCount))
                    {
                        if (startIndex+count>packetCount)
                            count=packetCount-startIndex;

                        double* theDoubles=new double[count];
                        for (int i=0;i<int(count);i++)
                        {
                            double v;
                            ((char*)&v)[0]=data[sizeof(double)*(i+startIndex)+0];
                            ((char*)&v)[1]=data[sizeof(double)*(i+startIndex)+1];
                            ((char*)&v)[2]=data[sizeof(double)*(i+startIndex)+2];
                            ((char*)&v)[3]=data[sizeof(double)*(i+startIndex)+3];
                            ((char*)&v)[4]=data[sizeof(double)*(i+startIndex)+4];
                            ((char*)&v)[5]=data[sizeof(double)*(i+startIndex)+5];
                            ((char*)&v)[6]=data[sizeof(double)*(i+startIndex)+6];
                            ((char*)&v)[7]=data[sizeof(double)*(i+startIndex)+7];
                            theDoubles[i]=v;
                        }
                        pushDoubleTableOntoStack(L,count,theDoubles);
                        delete[] theDoubles;
                        LUA_END(1);
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simUnpackUInt8Table(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simUnpackUInt8Table");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        int startIndex=0;
        int count=0;
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            if (res==2)
                startIndex=luaToInt(L,2);

            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    count=luaToInt(L,3);

                size_t dataLength;
                const char* data=(char*)luaWrap_lua_tolstring(L,1,&dataLength);
                int packetCount=dataLength;
                if (count==0)
                    count=int(1999999999);

                if ((startIndex>=0)&&(startIndex<packetCount))
                {
                    if (startIndex+count>packetCount)
                        count=packetCount-startIndex;

                    int* theBytes=new int[count];
                    for (int i=0;i<count;i++)
                    {
                        unsigned char v;
                        ((char*)&v)[0]=data[i+startIndex];
                        theBytes[i]=v;
                    }
                    pushIntTableOntoStack(L,count,theBytes);
                    delete[] theBytes;
                    LUA_END(1);
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simUnpackUInt16Table(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simUnpackUInt16Table");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        int startIndex=0;
        int count=0;
        int additionalCharOffset=0;
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            if (res==2)
                startIndex=luaToInt(L,2);

            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    count=luaToInt(L,3);

                res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        additionalCharOffset=luaToInt(L,4);

                    size_t dataLength;
                    char* data=((char*)luaWrap_lua_tolstring(L,1,&dataLength))+additionalCharOffset;
                    dataLength=2*((dataLength-additionalCharOffset)/2);
                    int packetCount=dataLength/2;
                    if (count==0)
                        count=int(1999999999);

                    if ((startIndex>=0)&&(startIndex<packetCount))
                    {
                        if (startIndex+count>packetCount)
                            count=packetCount-startIndex;

                        int* theWords=new int[count];
                        for (int i=0;i<int(count);i++)
                        {
                            unsigned short v;
                            ((char*)&v)[0]=data[2*(i+startIndex)+0];
                            ((char*)&v)[1]=data[2*(i+startIndex)+1];
                            theWords[i]=v;
                        }
                        pushIntTableOntoStack(L,count,theWords);
                        delete[] theWords;
                        LUA_END(1);
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simTransformBuffer(luaWrap_lua_State* L)
{ // string inBuffer,number inFormat,number multiplier,number offset,number outFormat
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simTransformBuffer");

    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        size_t dataLength;
        char* data=(char*)luaWrap_lua_tolstring(L,1,&dataLength);
        int inFormat=luaToInt(L,2);
        float mult=luaToFloat(L,3);
        float off=luaToFloat(L,4);
        int outFormat=luaToInt(L,5);
        bool something=false;
        if (inFormat==sim_buffer_float)
        {
            something=true;
            dataLength-=(dataLength % sizeof(float));
            dataLength/=sizeof(float);
            if (dataLength!=0)
            {
                if (outFormat==sim_buffer_uint8)
                {
                   unsigned char* dat=new unsigned char[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(unsigned char)(((float*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength);
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_uint8rgb)
                {
                   unsigned char* dat=new unsigned char[dataLength*3];
                   for (size_t i=0;i<dataLength;i++)
                   {
                       dat[3*i+0]=(unsigned char)(((float*)data)[i]*mult+off);
                       dat[3*i+1]=dat[3*i+0];
                       dat[3*i+2]=dat[3*i+0];
                   }
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*3);
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_int8)
                {
                   char* dat=new char[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(char)(((float*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength);
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_uint16)
                {
                   uint16_t* dat=new uint16_t[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(uint16_t)(((float*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*sizeof(uint16_t));
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_int16)
                {
                   int16_t* dat=new int16_t[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(int16_t)(((float*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*sizeof(int16_t));
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_uint32)
                {
                   uint32_t* dat=new uint32_t[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(uint32_t)(((float*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*sizeof(uint32_t));
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_int32)
                {
                   int32_t* dat=new int32_t[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(int32_t)(((float*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*sizeof(int32_t));
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_float)
                {
                   float* dat=new float[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(float)(((float*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*sizeof(float));
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_double)
                {
                   double* dat=new double[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(double)(((float*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*sizeof(double));
                   delete[] dat;
                   LUA_END(1);
                }
                errorString=SIM_ERROR_INVALID_FORMAT;
            }
            else
                errorString=SIM_ERROR_INVALID_DATA;
        }
        if (inFormat==sim_buffer_double)
        {
            something=true;
            dataLength-=(dataLength % sizeof(double));
            dataLength/=sizeof(double);
            if (dataLength!=0)
            {
                if (outFormat==sim_buffer_uint8)
                {
                   unsigned char* dat=new unsigned char[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(unsigned char)(((double*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength);
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_uint8rgb)
                {
                   unsigned char* dat=new unsigned char[dataLength*3];
                   for (size_t i=0;i<dataLength;i++)
                   {
                       dat[3*i+0]=(unsigned char)(((double*)data)[i]*mult+off);
                       dat[3*i+1]=dat[3*i+0];
                       dat[3*i+2]=dat[3*i+0];
                   }
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*3);
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_int8)
                {
                   char* dat=new char[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(char)(((double*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength);
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_uint16)
                {
                   uint16_t* dat=new uint16_t[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(uint16_t)(((double*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*sizeof(uint16_t));
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_int16)
                {
                   int16_t* dat=new int16_t[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(int16_t)(((double*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*sizeof(int16_t));
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_uint32)
                {
                   uint32_t* dat=new uint32_t[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(uint32_t)(((double*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*sizeof(uint32_t));
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_int32)
                {
                   int32_t* dat=new int32_t[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(int32_t)(((double*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*sizeof(int32_t));
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_float)
                {
                   float* dat=new float[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(float)(((double*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*sizeof(float));
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_double)
                {
                   double* dat=new double[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(double)(((double*)data)[i]*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength*sizeof(double));
                   delete[] dat;
                   LUA_END(1);
                }
                errorString=SIM_ERROR_INVALID_FORMAT;
            }
            else
                errorString=SIM_ERROR_INVALID_DATA;
        }
        if ( (inFormat==sim_buffer_uint8rgb)||(inFormat==sim_buffer_uint8bgr) )
        {
            something=true;
            dataLength-=(dataLength % 3);
            dataLength/=3;
            if (dataLength!=0)
            {
                if ( ( (inFormat==sim_buffer_uint8rgb)&&(outFormat==sim_buffer_uint8bgr) )||( (inFormat==sim_buffer_uint8bgr)&&(outFormat==sim_buffer_uint8rgb) ) )
                {
                   unsigned char* dat=new unsigned char[3*dataLength];
                   if ( (mult==1.0)&&(off==0.0) )
                   {
                       for (size_t i=0;i<dataLength;i++)
                       {
                           dat[3*i+0]=data[3*i+2];
                           dat[3*i+1]=data[3*i+1];
                           dat[3*i+2]=data[3*i+0];
                       }
                   }
                   else
                   {
                       for (size_t i=0;i<dataLength;i++)
                       {
                           dat[3*i+0]=(unsigned char)(float(data[3*i+2])*mult+off);
                           dat[3*i+1]=(unsigned char)(float(data[3*i+1])*mult+off);
                           dat[3*i+2]=(unsigned char)(float(data[3*i+0])*mult+off);
                       }
                   }
                   luaWrap_lua_pushlstring(L,(const char*)dat,3*dataLength);
                   delete[] dat;
                   LUA_END(1);
                }
                errorString=SIM_ERROR_INVALID_FORMAT;
            }
            else
                errorString=SIM_ERROR_INVALID_DATA;
        }
        if (inFormat==sim_buffer_uint8)
        {
            something=true;
            if (dataLength!=0)
            {
                if (outFormat==sim_buffer_uint8)
                {
                   unsigned char* dat=new unsigned char[dataLength];
                   for (size_t i=0;i<dataLength;i++)
                       dat[i]=(unsigned char)(float(data[i])*mult+off);
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength);
                   delete[] dat;
                   LUA_END(1);
                }
                if (outFormat==sim_buffer_uint8rgb)
                {
                   unsigned char* dat=new unsigned char[3*dataLength];
                   if ( (mult==1.0)&&(off==0.0) )
                   {
                       for (size_t i=0;i<dataLength;i++)
                       {
                           dat[3*i+0]=data[i];
                           dat[3*i+1]=data[i];
                           dat[3*i+2]=data[i];
                       }
                   }
                   else
                   {
                       for (size_t i=0;i<dataLength;i++)
                       {
                           dat[3*i+0]=(unsigned char)(float(data[i])*mult+off);
                           dat[3*i+1]=dat[3*i+0];
                           dat[3*i+2]=dat[3*i+0];
                       }
                   }
                   luaWrap_lua_pushlstring(L,(const char*)dat,3*dataLength);
                   delete[] dat;
                   LUA_END(1);
                }
                errorString=SIM_ERROR_INVALID_FORMAT;
            }
            else
                errorString=SIM_ERROR_INVALID_DATA;
        }
        if ( (inFormat==sim_buffer_uint8rgb)||(inFormat==sim_buffer_uint8bgr) )
        {
            something=true;
            dataLength-=(dataLength % 3);
            dataLength/=3;
            if (dataLength!=0)
            {
                if (outFormat==sim_buffer_uint8)
                {
                   unsigned char* dat=new unsigned char[dataLength];
                   if ( (mult==1.0)&&(off==0.0) )
                   {
                       for (size_t i=0;i<dataLength;i++)
                           dat[i]=(int(data[3*i+0])+int(data[3*i+1])+int(data[3*i+2]))/3;
                   }
                   else
                   {
                       for (size_t i=0;i<dataLength;i++)
                           dat[i]=(unsigned char)(((float(data[3*i+0])*mult+off)+(float(data[3*i+1])*mult+off)+(float(data[3*i+2])*mult+off))/3.0);
                   }
                   luaWrap_lua_pushlstring(L,(const char*)dat,dataLength);
                   delete[] dat;
                   LUA_END(1);
                }
                errorString=SIM_ERROR_INVALID_FORMAT;
            }
            else
                errorString=SIM_ERROR_INVALID_DATA;
        }
        if (!something)
            errorString=SIM_ERROR_INVALID_FORMAT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simCombineRgbImages(luaWrap_lua_State* L)
{   LUA_API_FUNCTION_DEBUG;
    LUA_START("simCombineRgbImages");

    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,2,lua_arg_string,0,lua_arg_number,2,lua_arg_number,0))
    {
        size_t img1Length,img2Length;
        char* img1=(char*)luaWrap_lua_tolstring(L,1,&img1Length);
        char* img2=(char*)luaWrap_lua_tolstring(L,3,&img2Length);
        int res1[2];
        int res2[2];
        getIntsFromTable(L,2,2,res1);
        getIntsFromTable(L,4,2,res2);
        int op=luaToInt(L,5);
        if ( (img1Length>=res1[0]*res1[1]*3)&&(img2Length>=res2[0]*res2[1]*3) )
        {
            if (op==sim_imgcomb_vertical)
            {
               if (res1[0]==res2[0])
               {
                   unsigned char* dat=new unsigned char[(res1[0]*res1[1]+res2[0]*res2[1])*3];
                   int l=res1[0]*res1[1]*3;
                   for (size_t i=0;i<l;i++)
                       dat[i]=img1[i];
                   for (size_t i=0;i<res2[0]*res2[1]*3;i++)
                       dat[l+i]=img2[i];
                   luaWrap_lua_pushlstring(L,(const char*)dat,(res1[0]*res1[1]+res2[0]*res2[1])*3);
                   delete[] dat;
                   LUA_END(1);
               }
               else
                   errorString=SIM_ERROR_INVALID_RESOLUTION;
            }
            if (op==sim_imgcomb_horizontal)
            {
                if (res1[1]==res2[1])
                {
                    unsigned char* dat=new unsigned char[(res1[0]*res1[1]+res2[0]*res2[1])*3];
                    for (size_t y=0;y<res1[1];y++)
                    {
                        int off1=y*res1[0]*3;
                        int off2=y*res2[0]*3;
                        int off3=off1+off2;
                        for (size_t i=0;i<res1[0]*3;i++)
                            dat[off3+i]=img1[off1+i];
                        off3+=res1[0]*3;
                        for (size_t i=0;i<res2[0]*3;i++)
                            dat[off3+i]=img2[off2+i];
                    }
                    luaWrap_lua_pushlstring(L,(const char*)dat,(res1[0]*res1[1]+res2[0]*res2[1])*3);
                    delete[] dat;
                    LUA_END(1);
                }
                else
                    errorString=SIM_ERROR_INVALID_RESOLUTION;
            }
        }
        else
            errorString=SIM_ERROR_INVALID_DATA;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSendData(luaWrap_lua_State* L)
{ // can be called from the main script or from child scripts
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSendData");

    int retVal=-1;
    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if ( (it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript) )
    {
        if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_string,0,lua_arg_string,0))
        {
            int targetID=luaWrap_lua_tointeger(L,1);
            if ( (targetID<=0)&&(targetID!=sim_handle_all)&&(targetID!=sim_handle_tree)&&(targetID!=sim_handle_chain) )
                errorString=SIM_ERROR_INVALID_TARGET_HANDLE;
            else
            {
                int dataHeader=luaWrap_lua_tointeger(L,2);
                if (dataHeader<0)
                    errorString=SIM_ERROR_INVALID_DATA_HEADER;
                else
                {
                    std::string dataName(luaWrap_lua_tostring(L,3));
                    if ( (dataName.length()==0)||(dataName.find(char(0))!=std::string::npos) )
                        errorString=SIM_ERROR_INVALID_DATA_NAME;
                    else
                    {
                        size_t dataLength;
                        char* data=(char*)luaWrap_lua_tolstring(L,4,&dataLength);
                        if (dataLength<1)
                            errorString=SIM_ERROR_INVALID_DATA;
                        else
                        {
                            // Following are default values:
                            int antennaHandle=sim_handle_self;
                            float actionRadius=100.0f;
                            float emissionAngle1=piValue;
                            float emissionAngle2=piValTimes2;
                            float persistence=0.0f;
                            bool err=false;
                            int res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,false,&errorString);
                            if (res==2)
                                antennaHandle=luaWrap_lua_tointeger(L,5);
                            err|=((res!=0)&&(res!=2));
                            if (!err)
                            {
                                if ( (antennaHandle<0)&&(antennaHandle!=sim_handle_default)&&((antennaHandle!=sim_handle_self)||(it->getScriptType()!=sim_scripttype_childscript)) )
                                {
                                    errorString=SIM_ERROR_INVALID_ANTENNA_HANDLE;
                                    err=true;
                                }
                                else
                                {
                                    if (antennaHandle==sim_handle_self)
                                        antennaHandle=it->getObjectIDThatScriptIsAttachedTo_child();
                                    if (antennaHandle!=sim_handle_default)
                                    {
                                        C3DObject* ant=App::ct->objCont->getObject(antennaHandle);
                                        if (ant==NULL)
                                        {
                                            errorString=SIM_ERROR_INVALID_ANTENNA_HANDLE;
                                            err=true;
                                        }
                                    }
                                }
                            }
                            if (!err)
                            {
                                int res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,true,false,&errorString);
                                if (res==2)
                                    actionRadius=luaToFloat(L,6);
                                err|=((res!=0)&&(res!=2));
                            }
                            if (!err)
                            {
                                int res=checkOneGeneralInputArgument(L,7,lua_arg_number,0,true,false,&errorString);
                                if (res==2)
                                    emissionAngle1=luaToFloat(L,7);
                                err|=((res!=0)&&(res!=2));
                            }
                            if (!err)
                            {
                                int res=checkOneGeneralInputArgument(L,8,lua_arg_number,0,true,false,&errorString);
                                if (res==2)
                                    emissionAngle2=luaToFloat(L,8);
                                err|=((res!=0)&&(res!=2));
                            }
                            if (!err)
                            {
                                int res=checkOneGeneralInputArgument(L,9,lua_arg_number,0,true,false,&errorString);
                                if (res==2)
                                    persistence=luaToFloat(L,9);
                                err|=((res!=0)&&(res!=2));
                            }
                            if (!err)
                            {
                                actionRadius=tt::getLimitedFloat(0.0f,SIM_MAX_FLOAT,actionRadius);
                                emissionAngle1=tt::getLimitedFloat(0.0f,piValue,emissionAngle1);
                                emissionAngle2=tt::getLimitedFloat(0.0f,piValTimes2,emissionAngle2);
                                persistence=tt::getLimitedFloat(0.0f,99999999999999.9f,persistence);
                                if (persistence==0.0f)
                                    persistence=float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())*1.5f/1000000.0f;

                                App::ct->luaScriptContainer->broadcastDataContainer.broadcastData(currentScriptID,targetID,dataHeader,dataName,
                                    float(App::ct->simulation->getSimulationTime_ns())/1000000.0f+persistence,actionRadius,antennaHandle,
                                    emissionAngle1,emissionAngle2,data,dataLength);
                                retVal=1;
                            }
                        }
                    }
                }
            }
        }
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT_OR_CHILD_SCRIPT;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simReceiveData(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simReceiveData");

    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if ( (it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript) )
    {
        int dataHeader=-1;
        std::string dataName;
        int antennaHandle=sim_handle_self;
        int index=-1;
        bool err=false;
        int res=checkOneGeneralInputArgument(L,1,lua_arg_number,0,true,false,&errorString);
        if (res==2)
        {
            dataHeader=luaWrap_lua_tointeger(L,1);
            if (dataHeader<0)
                dataHeader=-1;
        }
        err|=((res!=0)&&(res!=2));
        if (!err)
        {
            int res=checkOneGeneralInputArgument(L,2,lua_arg_string,0,true,true,&errorString);
            if (res==2)
            {
                dataName=luaWrap_lua_tostring(L,2);
                if (dataName.length()<1)
                {
                    errorString=SIM_ERROR_INVALID_DATA_NAME;
                    err=true;
                }
            }
            err|=(res<0);
        }
        if (!err)
        {
            int res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
            if (res==2)
            {
                antennaHandle=luaWrap_lua_tointeger(L,3);
                if ( (antennaHandle<0)&&(antennaHandle!=sim_handle_default)&&((antennaHandle!=sim_handle_self)||(it->getScriptType()!=sim_scripttype_childscript)) )
                {
                    errorString=SIM_ERROR_INVALID_ANTENNA_HANDLE;
                    err=true;
                }
            }
            err|=((res!=0)&&(res!=2));
            if (!err)
            {
                if (antennaHandle==sim_handle_self)
                    antennaHandle=it->getObjectIDThatScriptIsAttachedTo_child();
                if (antennaHandle!=sim_handle_default)
                {
                    C3DObject* ant=App::ct->objCont->getObject(antennaHandle);
                    if (ant==NULL)
                    {
                        errorString=SIM_ERROR_INVALID_ANTENNA_HANDLE;
                        err=true;
                    }
                }
            }
        }
        if (!err)
        {
            int res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,false,&errorString);
            if (res==2)
            {
                index=luaWrap_lua_tointeger(L,4);
                if (index<0)
                    index=-1;
            }
            err|=((res!=0)&&(res!=2));
        }
        if (!err)
        {
            int theDataHeader;
            int theDataLength;
            int theSenderID;
            std::string theDataName;
            char* data0=App::ct->luaScriptContainer->broadcastDataContainer.receiveData(currentScriptID,float(App::ct->simulation->getSimulationTime_ns())/1000000.0f,
                    dataHeader,dataName,antennaHandle,theDataLength,index,theSenderID,theDataHeader,theDataName);
            if (data0!=NULL)
            {
                luaWrap_lua_pushlstring(L,data0,theDataLength);
                luaWrap_lua_pushnumber(L,theSenderID);
                luaWrap_lua_pushnumber(L,theDataHeader);
                luaWrap_lua_pushstring(L,theDataName.c_str());
                LUA_END(4);
            }
        }
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT_OR_CHILD_SCRIPT;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetGraphUserData(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetGraphUserData");

    int retVal=-1; // for error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0,lua_arg_number,0))
    {
        int graphHandle=luaWrap_lua_tointeger(L,1);
        std::string dataName(luaWrap_lua_tostring(L,2));
        float data=luaToFloat(L,3);
        retVal=simSetGraphUserData_internal(graphHandle,dataName.c_str(),data);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAddDrawingObject(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddDrawingObject");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int objType=luaToInt(L,1);
        float size=luaToFloat(L,2);
        float duplicateTolerance=luaToFloat(L,3);
        int parentID=luaToInt(L,4);
        int maxItemCount=luaToInt(L,5);
        float* ambient=NULL;
        float* specular=NULL;
        float* emission=NULL;
        int res=checkOneGeneralInputArgument(L,6,lua_arg_number,3,true,true,&errorString);
        int okToGo=(res!=-1);
        if (okToGo)
        {
            float ambientC[3];
            float specularC[3];
            float emissionC[6];
            if (res>0)
            {
                if (res==2)
                {
                    getFloatsFromTable(L,6,3,ambientC);
                    ambient=ambientC;
                }
                res=checkOneGeneralInputArgument(L,7,lua_arg_number,3,true,true,&errorString);
                okToGo=(res!=-1);
                if (okToGo)
                {
                    if (res>0)
                    {
                        res=checkOneGeneralInputArgument(L,8,lua_arg_number,3,true,true,&errorString);
                        okToGo=(res!=-1);
                        if (okToGo)
                        {
                            if (res>0)
                            {
                                if (res==2)
                                {
                                    getFloatsFromTable(L,8,3,specularC);
                                    specular=specularC;
                                }
                                res=checkOneGeneralInputArgument(L,9,lua_arg_number,3,true,true,&errorString);
                                okToGo=(res!=-1);
                                if (okToGo)
                                {
                                    if (res>0)
                                    {
                                        if (res==2)
                                        {
                                            // following 3 are default aux colors:
                                            emissionC[3]=0.5f;
                                            emissionC[4]=0.0f;
                                            emissionC[5]=0.0f;
                                            if (int(luaWrap_lua_objlen(L,9))<6)
                                                getFloatsFromTable(L,9,3,emissionC);
                                            else
                                            {
                                                objType|=sim_drawing_auxchannelcolor1;
                                                getFloatsFromTable(L,9,6,emissionC);
                                            }
                                            emission=emissionC;
                                        }

                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (okToGo)
            {
                retVal=simAddDrawingObject_internal(objType,size,duplicateTolerance,parentID,maxItemCount,ambient,NULL,specular,emission);
                if ( (retVal!=-1) )//&&((objType&sim_drawing_persistent)==0) )
                { // following condition added on 2011/01/06 so as to not remove objects created from the c/c++ interface or from an add-on:
                    int currentScriptID=getCurrentScriptID(L);
                    CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
                    if ( (itScrObj->getScriptType()==sim_scripttype_mainscript)||(itScrObj->getScriptType()==sim_scripttype_childscript)||(itScrObj->getScriptType()==sim_scripttype_jointctrlcallback)||(itScrObj->getScriptType()==sim_scripttype_contactcallback) ) //||(itScrObj->getScriptType()==sim_scripttype_generalcallback) )
                    {
                        CDrawingObject* anObj=App::ct->drawingCont->getObject(retVal);
                        if (anObj!=NULL)
                        {
                            anObj->setCreatedFromScript(true);
                            anObj->setPersistent((objType&sim_drawing_persistent)!=0);
                        }
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRemoveDrawingObject(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRemoveDrawingObject");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int objectHandle=luaToInt(L,1);
        if (objectHandle==sim_handle_all)
        { // following condition added here on 2011/01/06 so as not to remove objects created from a c/c++ call or from add-on:
            int currentScriptID=getCurrentScriptID(L);
            CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
            App::ct->drawingCont->removeAllObjects((itScrObj->getScriptType()==sim_scripttype_mainscript)||(itScrObj->getScriptType()==sim_scripttype_childscript)||(itScrObj->getScriptType()==sim_scripttype_jointctrlcallback)||(itScrObj->getScriptType()==sim_scripttype_contactcallback),true); //||(itScrObj->getScriptType()==sim_scripttype_generalcallback));
            retVal=1;
        }
        else
            retVal=simRemoveDrawingObject_internal(objectHandle);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAddDrawingObjectItem(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddDrawingObjectItem");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int drawingObjHandle=luaToInt(L,1);
        CDrawingObject* it=App::ct->drawingCont->getObject(drawingObjHandle);
        int d=3;
        if (it!=NULL)
            d=it->verticesPerItem*3+it->normalsPerItem*3+it->otherFloatsPerItem;
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,d,true,true,&errorString);
        if (res==2)
        {
            float vertex[20]; // we should have enough here!
            getFloatsFromTable(L,2,d,vertex);
            retVal=simAddDrawingObjectItem_internal(drawingObjHandle,vertex);
        }
        else
        {
            if (res>=0)
                retVal=simAddDrawingObjectItem_internal(drawingObjHandle,NULL);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAddParticleObject(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddParticleObject");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        // The 4th argument can be nil or a table. Check for that:
        if (luaWrap_lua_gettop(L)<4)
            errorString=SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS;
        else
        {
            if ( (!luaWrap_lua_isnil(L,4))&&((!luaWrap_lua_istable(L,4))||(int(luaWrap_lua_objlen(L,4))<3)) )
                errorString=SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG;
            else
            {
                if ( (checkOneGeneralInputArgument(L,5,lua_arg_number,0,false,false,&errorString)==2)&&
                    (checkOneGeneralInputArgument(L,6,lua_arg_number,0,false,false,&errorString)==2) )
                {
                    int objType=luaToInt(L,1);
                    float size=luaToFloat(L,2);
                    float massOverVolume=luaToFloat(L,3);
                    int paramLen=int(luaWrap_lua_objlen(L,4));
                    paramLen=(paramLen-1)/2;
                    paramLen=paramLen*2+1;
                    void* params=NULL;
                    if (!luaWrap_lua_isnil(L,4))
                    {
                        int intParams[30];
                        float floatParams[30];
                        getIntsFromTable(L,4,paramLen,intParams);
                        getFloatsFromTable(L,4,paramLen,floatParams);
                        params=new char[paramLen*sizeof(int)];
                        ((int*)params)[0]=SIM_MIN(intParams[0],(paramLen-1)/2);
                        for (int i=0;i<(paramLen-1)/2;i++)
                        {
                            ((int*)params)[1+2*i]=intParams[1+2*i+0];
                            ((float*)params)[1+2*i+1]=floatParams[1+2*i+1];
                        }
                    }
                    float lifeTime=luaToFloat(L,5);
                    int maxItemCount=luaToInt(L,6);
                    float* ambient=NULL;
                    float* specular=NULL;
                    float* emission=NULL;
                    int res=checkOneGeneralInputArgument(L,7,lua_arg_number,3,true,true,&errorString);
                    int okToGo=(res!=-1);
                    if (okToGo)
                    {
                        float ambientC[3];
                        float specularC[3];
                        float emissionC[3];
                        if (res>0)
                        {
                            if (res==2)
                            {
                                getFloatsFromTable(L,7,3,ambientC);
                                ambient=ambientC;
                            }
                            res=checkOneGeneralInputArgument(L,8,lua_arg_number,3,true,true,&errorString);
                            okToGo=(res!=-1);
                            if (okToGo)
                            {
                                if (res>0)
                                {
                                    res=checkOneGeneralInputArgument(L,9,lua_arg_number,3,true,true,&errorString);
                                    okToGo=(res!=-1);
                                    if (okToGo)
                                    {
                                        if (res>0)
                                        {
                                            if (res==2)
                                            {
                                                getFloatsFromTable(L,9,3,specularC);
                                                specular=specularC;
                                            }
                                            res=checkOneGeneralInputArgument(L,10,lua_arg_number,3,true,true,&errorString);
                                            okToGo=(res!=-1);
                                            if (okToGo)
                                            {
                                                if (res>0)
                                                {
                                                    if (res==2)
                                                    {
                                                        getFloatsFromTable(L,10,3,emissionC);
                                                        emission=emissionC;
                                                    }

                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (okToGo)
                        {
                            retVal=simAddParticleObject_internal(objType,size,massOverVolume,params,lifeTime,maxItemCount,ambient,NULL,specular,emission);
                        }
                    }
                    delete[] ((char*)params);
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRemoveParticleObject(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRemoveParticleObject");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simRemoveParticleObject_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAddParticleObjectItem(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddParticleObjectItem");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int particleObjHandle=luaToInt(L,1);
        int d=6+CPluginContainer::dyn_getParticleObjectOtherFloatsPerItem(particleObjHandle);
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,d,true,true,&errorString);
        if (res==2)
        {
            float vertex[20]; // we should have enough here!
            getFloatsFromTable(L,2,d,vertex);
            retVal=simAddParticleObjectItem_internal(particleObjHandle,vertex);
        }
        else
        {
            if (res>=0)
                retVal=simAddParticleObjectItem_internal(particleObjHandle,NULL);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectSizeFactor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectSizeFactor");

    float retVal=-1.0f; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simGetObjectSizeFactor_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simResetMilling(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simResetMilling");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simResetMilling_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simApplyMilling(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simApplyMilling");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simApplyMilling_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetIntegerSignal(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetIntegerSignal");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0))
    {
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        App::ct->signalContainer->setIntegerSignal(std::string(luaWrap_lua_tostring(L,1)).c_str(),luaToInt(L,2),(it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript)||(it->getScriptType()==sim_scripttype_jointctrlcallback)||(it->getScriptType()==sim_scripttype_contactcallback)); //||(it->getScriptType()==sim_scripttype_generalcallback));
        retVal=1;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetIntegerSignal(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetIntegerSignal");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        int intVal;
        if (simGetIntegerSignal_internal(std::string(luaWrap_lua_tostring(L,1)).c_str(),&intVal)==1)
        {
            luaWrap_lua_pushnumber(L,intVal);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simClearIntegerSignal(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simClearIntegerSignal");

    int retVal=-1; //error
    int res=checkOneGeneralInputArgument(L,1,lua_arg_string,0,true,true,&errorString);
    if (res>=0)
    {
        if (res!=2)
        {
            int currentScriptID=getCurrentScriptID(L);
            CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
            retVal=App::ct->signalContainer->clearAllIntegerSignals((it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript)||(it->getScriptType()==sim_scripttype_jointctrlcallback)||(it->getScriptType()==sim_scripttype_contactcallback));//||(it->getScriptType()==sim_scripttype_generalcallback));
        }
        else
            retVal=simClearIntegerSignal_internal(std::string(luaWrap_lua_tostring(L,1)).c_str());
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetFloatSignal(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetFloatSignal");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0))
    {
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        App::ct->signalContainer->setFloatSignal(std::string(luaWrap_lua_tostring(L,1)).c_str(),luaToFloat(L,2),(it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript)||(it->getScriptType()==sim_scripttype_jointctrlcallback)||(it->getScriptType()==sim_scripttype_contactcallback));//||(it->getScriptType()==sim_scripttype_generalcallback));
        retVal=1;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetFloatSignal(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetFloatSignal");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        float floatVal;
        if (simGetFloatSignal_internal(std::string(luaWrap_lua_tostring(L,1)).c_str(),&floatVal)==1)
        {
            luaWrap_lua_pushnumber(L,floatVal);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simClearFloatSignal(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simClearFloatSignal");

    int retVal=-1; //error
    int res=checkOneGeneralInputArgument(L,1,lua_arg_string,0,true,true,&errorString);
    if (res>=0)
    {
        if (res!=2)
        {
            int currentScriptID=getCurrentScriptID(L);
            CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
            retVal=App::ct->signalContainer->clearAllFloatSignals((it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript)||(it->getScriptType()==sim_scripttype_jointctrlcallback)||(it->getScriptType()==sim_scripttype_contactcallback));//||(it->getScriptType()==sim_scripttype_generalcallback));
        }
        else
            retVal=simClearFloatSignal_internal(std::string(luaWrap_lua_tostring(L,1)).c_str());
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetStringSignal(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetStringSignal");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_string,0))
    {
        size_t dataLength;
        char* data=(char*)luaWrap_lua_tolstring(L,2,&dataLength);
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        App::ct->signalContainer->setStringSignal(std::string(luaWrap_lua_tostring(L,1)).c_str(),std::string(data,dataLength),(it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript)||(it->getScriptType()==sim_scripttype_jointctrlcallback)||(it->getScriptType()==sim_scripttype_contactcallback));//||(it->getScriptType()==sim_scripttype_generalcallback));
        retVal=1;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetStringSignal(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetStringSignal");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        int stringLength;
        char* str=simGetStringSignal_internal(std::string(luaWrap_lua_tostring(L,1)).c_str(),&stringLength);
        if (str!=NULL)
        {
            luaWrap_lua_pushlstring(L,str,stringLength);
            simReleaseBuffer_internal(str);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simClearStringSignal(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simClearStringSignal");

    int retVal=-1; //error
    int res=checkOneGeneralInputArgument(L,1,lua_arg_string,0,true,true,&errorString);
    if (res>=0)
    {
        if (res!=2)
        {
            int currentScriptID=getCurrentScriptID(L);
            CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
            retVal=App::ct->signalContainer->clearAllStringSignals((it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript)||(it->getScriptType()==sim_scripttype_jointctrlcallback)||(it->getScriptType()==sim_scripttype_contactcallback));//||(it->getScriptType()==sim_scripttype_generalcallback));
        }
        else
            retVal=simClearStringSignal_internal(std::string(luaWrap_lua_tostring(L,1)).c_str());
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetSignalName(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetSignalName");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        char* str=simGetSignalName_internal(luaToInt(L,1),luaToInt(L,2));
        if (str!=NULL)
        {
            luaWrap_lua_pushstring(L,str);
            simReleaseBuffer_internal(str);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simWaitForSignal(luaWrap_lua_State* L)
{ // can only be called from a script running in a thread!!
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simWaitForSignal");

    if (!VThread::isCurrentThreadTheMainSimulationThread())
    {
        if (checkInputArguments(L,&errorString,lua_arg_string,0))
        {
            std::string signalName(luaWrap_lua_tostring(L,1));
            int intVal;
            bool signalPresent=false;

            CSignalContainer* sigCont=NULL;
            sigCont=App::ct->signalContainer;

            while (!signalPresent)
            {
                if (sigCont->getIntegerSignal(signalName.c_str(),intVal))
                {
                    luaWrap_lua_pushnumber(L,intVal);
                    signalPresent=true;
                    LUA_END(1);
                }
                float floatVal;
                if ( (!signalPresent)&&(sigCont->getFloatSignal(signalName.c_str(),floatVal)) )
                {
                    luaWrap_lua_pushnumber(L,floatVal);
                    signalPresent=true;
                    LUA_END(1);
                }
                std::string strVal;
                if ( (!signalPresent)&&(sigCont->getStringSignal(signalName.c_str(),strVal)) )
                {
                    luaWrap_lua_pushstring(L,strVal.c_str());
                    signalPresent=true;
                    LUA_END(1);
                }
                if (!signalPresent)
                {
                    CThreadPool::switchBackToPreviousThread();
                    if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                        break;
                }
            }
        }
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetObjectProperty(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectProperty");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simGetObjectProperty_internal(luaWrap_lua_tointeger(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetObjectProperty(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetObjectProperty");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetObjectProperty_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectSpecialProperty(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectSpecialProperty");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simGetObjectSpecialProperty_internal(luaWrap_lua_tointeger(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetObjectSpecialProperty(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetObjectSpecialProperty");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetObjectSpecialProperty_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetModelProperty(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetModelProperty");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simGetModelProperty_internal(luaWrap_lua_tointeger(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetModelProperty(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetModelProperty");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetModelProperty_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}


int _simMoveToObject(luaWrap_lua_State* L)
{ // can only be called from a script running in a thread!!
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simMoveToObject");

    if (!VThread::isCurrentThreadTheMainSimulationThread())
    {
        if (!(CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L))))
        { // Important to first check if we are supposed to leave the thread!!
            if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
            { // Those are the arguments that are always required! (the rest can be ignored or set to nil!
                int objID=luaWrap_lua_tointeger(L,1);
                int targetObjID=luaWrap_lua_tointeger(L,2);
                float maxVelocity=0.1f;
                float relativeDistanceOnPath=-1.0f;
                int positionAndOrOrientation=3; // position and orientation (default value)
                C3DObject* object=App::ct->objCont->getObject(objID);
                C3DObject* targetObject=App::ct->objCont->getObject(targetObjID);
                float accel=0.0f; // means infinite accel!! (default value)
                bool foundError=false;
                if ((!foundError)&&((object==NULL)||(targetObject==NULL)))
                {
                    errorString=SIM_ERROR_OBJECT_OR_TARGET_OBJECT_DOES_NOT_EXIST;
                    foundError=true;
                }
                if ((!foundError)&&(targetObject==object))
                {
                    errorString=SIM_ERROR_OBJECT_IS_SAME_AS_TARGET_OBJECT;
                    foundError=true;
                }
                // Now check the optional arguments:
                int res;
                if (!foundError) // position and/or orientation argument:
                {
                    res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,true,&errorString);
                    if (res==2)
                    { // get the data
                        positionAndOrOrientation=abs(luaToInt(L,3));
                        if ((positionAndOrOrientation&3)==0)
                            positionAndOrOrientation=1; // position only
                    }
                    foundError=(res==-1);
                }
                if (!foundError) // positionOnPath argument:
                {
                    res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,true,&errorString);
                    if (res==2)
                    { // get the data
                        relativeDistanceOnPath=tt::getLimitedFloat(0.0f,1.0f,luaToFloat(L,4));
                        if (targetObject->getObjectType()!=sim_object_path_type)
                        {
                            errorString=SIM_ERROR_TARGET_OBJECT_IS_NOT_A_PATH;
                            foundError=true;
                        }
                    }
                    foundError=(res==-1);
                }
                if (!foundError) // Velocity argument:
                {
                    res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,false,false,&errorString);
                    if (res==2)
                    { // get the data
                        maxVelocity=luaToFloat(L,5);
                    }
                    else
                        foundError=true; // this argument is not optional!
                }
                if (!foundError) // Accel argument:
                {
                    res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,true,true,&errorString);
                    if (res==2)
                    { // get the data
                        accel=fabs(luaToFloat(L,6));
                    }
                    foundError=(res==-1);
                }
                if (!foundError)
                { // do the job here!
                    C7Vector startTr(object->getCumulativeTransformationPart1());
                    float currentVel=0.0f;
                    CVThreadData* threadData=CThreadPool::getCurrentThreadData();
                    float lastTime=float(App::ct->simulation->getSimulationTime_ns())/1000000.0f+threadData->usedMovementTime;
                    float vdl=1.0f;
                    // vld is the totalvirtual distance
                    float currentPos=0.0f;

                    bool movementFinished=false;
                    float dt=float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f; // this is the time left if we leave here
                    float previousLL=0.0f;
                    while ( (!movementFinished)&&(vdl!=0.0f) )
                    {
                        float currentTime=float(App::ct->simulation->getSimulationTime_ns())/1000000.0f+float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f;
                        dt=currentTime-lastTime;
                        lastTime=currentTime;

                        if (accel==0.0f)
                        { // Means infinite acceleration
                            float timeNeeded=(vdl-currentPos)/maxVelocity;
                            currentVel=maxVelocity;
                            if (timeNeeded>dt)
                            {
                                currentPos+=dt*maxVelocity;
                                dt=0.0f; // this is what is left
                            }
                            else
                            {
                                currentPos=vdl;
                                if (timeNeeded>=0.0f)
                                    dt-=timeNeeded;
                            }
                        }
                        else
                        {
                            double p=currentPos;
                            double v=currentVel;
                            double t=dt;
                            CLinMotionRoutines::getNextValues(p,v,maxVelocity,accel,0.0f,vdl,0.0f,0.0f,t);
                            currentPos=float(p);
                            currentVel=float(v);
                            dt=float(t);
                        }

                        // Now check if we are within tolerances:
                        if (fabs(currentPos-vdl)<=0.00001f)
                            movementFinished=true;

                        // Set the new configuration of the object:
                        float ll=currentPos/vdl;
                        if (ll>1.0f)
                            ll=1.0f;
                        if ((App::ct->objCont->getObject(objID)==object)&&(App::ct->objCont->getObject(targetObjID)==targetObject)) // make sure the objects are still valid (running in a thread)
                        {
                            C7Vector targetTr(targetObject->getCumulativeTransformationPart1());
                            bool goOn=true;
                            if (relativeDistanceOnPath>=0.0f)
                            { // we should have a path here
                                if (targetObject->getObjectType()==sim_object_path_type)
                                {
                                    C7Vector pathLoc;
                                    if ( ((CPath*)targetObject)->pathContainer->getTransformationOnBezierCurveAtNormalizedVirtualDistance(relativeDistanceOnPath,pathLoc))
                                        targetTr*=pathLoc;
                                    else
                                        relativeDistanceOnPath=-1.0f; // the path is empty!
                                }
                                else
                                    goOn=false;
                            }
                            if (goOn)
                            {
                                C7Vector newAbs;
                                newAbs.buildInterpolation(startTr,targetTr,(ll-previousLL)/(1.0f-previousLL));
                                startTr=newAbs;
                                C7Vector parentInv(object->getParentCumulativeTransformation().getInverse());
                                C7Vector currentTr(object->getCumulativeTransformationPart1());
                                if ((positionAndOrOrientation&1)==0)
                                    newAbs.X=currentTr.X;
                                if ((positionAndOrOrientation&2)==0)
                                    newAbs.Q=currentTr.Q;
                                object->setLocalTransformation(parentInv*newAbs);
                            }
                            else
                                movementFinished=true; // the target object is not a path anymore!!
                        }
                        else
                            movementFinished=true; // the object was destroyed during execution of the command!
                        previousLL=ll;
                        if (!movementFinished)
                        {
                            CThreadPool::switchBackToPreviousThread();
                            if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                                break;
                        }
                    }
                    // The movement finished. Now add the time used:
                    threadData->usedMovementTime=float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f-dt;
                    luaWrap_lua_pushnumber(L,dt); // success (deltaTime left)
                    LUA_END(1);
                }
            }
        }
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simFollowPath(luaWrap_lua_State* L)
{ // can only be called from a script running in a thread!!
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simFollowPath");

    if (!VThread::isCurrentThreadTheMainSimulationThread())
    {
        if (!(CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L))))
        { // Important to first check if we are supposed to leave the thread!!
            if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
            { // Those are the arguments that are always required! (the rest can be ignored or set to nil!
                int objID=luaWrap_lua_tointeger(L,1);
                int pathID=luaWrap_lua_tointeger(L,2);
                float posOnPath=luaToFloat(L,4);
                int positionAndOrOrientation=abs(luaToInt(L,3));
                if (positionAndOrOrientation==0)
                    positionAndOrOrientation=1;
                float maxVelocity=luaToFloat(L,5);
                C3DObject* object=App::ct->objCont->getObject(objID);
                CPath* path=App::ct->objCont->getPath(pathID);
                float accel=0.0f; // means infinite accel!! (default value)
                bool foundError=false;
                if ((!foundError)&&(object==NULL))
                {
                    errorString=SIM_ERROR_OBJECT_INEXISTANT;
                    foundError=true;
                }
                if ((!foundError)&&(path==NULL))
                {
                    errorString=SIM_ERROR_PATH_INEXISTANT;
                    foundError=true;
                }
                if (!foundError)
                {
                    if (path->pathContainer->getAttributes()&sim_pathproperty_closed_path)
                    {
                        if (posOnPath<0.0f)
                            posOnPath=0.0f;
                    }
                    else
                        posOnPath=tt::getLimitedFloat(0.0f,1.0f,posOnPath);
                }

                // Now check the optional arguments:
                int res;
                if (!foundError) // Accel argument:
                {
                    res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,true,true,&errorString);
                    if (res==2)
                    { // get the data
                        accel=fabs(luaToFloat(L,6));
                    }
                    foundError=(res==-1);
                }
                if (!foundError)
                { // do the job here!
                    float bezierPathLength=path->pathContainer->getBezierVirtualPathLength();
                    double pos=posOnPath*bezierPathLength;
                    float vel=0.0f;
                    CVThreadData* threadData=CThreadPool::getCurrentThreadData();
                    float lastTime=float(App::ct->simulation->getSimulationTime_ns())/1000000.0f+threadData->usedMovementTime;
                    bool movementFinished=(bezierPathLength==0.0f);
                    float dt=float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f; // this is the time left if we leave here
                    while (!movementFinished)
                    {
                        if ((App::ct->objCont->getObject(objID)!=object)||(App::ct->objCont->getPath(pathID)!=path) ) // make sure the objects are still valid (running in a thread)
                        {
                            dt=float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f;
                            break;
                        }

                        float currentTime=float(App::ct->simulation->getSimulationTime_ns())/1000000.0f+float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f;
                        dt=currentTime-lastTime;
                        lastTime=currentTime;
                        if (accel==0.0f)
                        { // Means infinite acceleration
                            path->pathContainer->handlePath_keepObjectUnchanged(dt,pos,vel,maxVelocity,1.0f,false,true);
                        }
                        else
                        {
                            path->pathContainer->handlePath_keepObjectUnchanged(dt,pos,vel,maxVelocity,accel,false,false);
                        }

                        // Now check if we are within tolerances:
                        if ( ((fabs(pos-bezierPathLength)<=0.00001f)&&(maxVelocity>=0.0f)) || ((fabs(pos-0.0f)<=0.00001f)&&(maxVelocity<=0.0f)) )
                            movementFinished=true;

                        // Set the new configuration of the object:
                        float ll=float(pos/bezierPathLength);
                        C7Vector newAbs;
                        if (path->pathContainer->getTransformationOnBezierCurveAtNormalizedVirtualDistance(ll,newAbs))
                        {
                            newAbs=path->getCumulativeTransformationPart1()*newAbs;
                            C7Vector parentInv(object->getParentCumulativeTransformation().getInverse());
                            C7Vector currAbs(object->getCumulativeTransformationPart1());
                            if ((positionAndOrOrientation&1)==0)
                                newAbs.X=currAbs.X;
                            if ((positionAndOrOrientation&2)==0)
                                newAbs.Q=currAbs.Q;
                            object->setLocalTransformation(parentInv*newAbs);
                        }
                        else
                            movementFinished=true;

                        if (!movementFinished)
                        {
                            CThreadPool::switchBackToPreviousThread();
                            if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                                break;
                        }
                    }
                    // The movement finished. Now add the time used:
                    threadData->usedMovementTime=float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f-dt;
                    luaWrap_lua_pushnumber(L,dt); // success
                    LUA_END(1);
                }
            }
        }
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetDataOnPath(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetDataOnPath");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int auxFlags;
        float auxChannels[4];
        if (simGetDataOnPath_internal(luaToInt(L,1),luaToFloat(L,2),0,&auxFlags,auxChannels)==1)
        {
            luaWrap_lua_pushnumber(L,auxFlags);
            pushFloatTableOntoStack(L,4,auxChannels);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetPositionOnPath(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetPositionOnPath");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        float coord[3];
        if (simGetPositionOnPath_internal(luaToInt(L,1),luaToFloat(L,2),coord)==1)
        {
            pushFloatTableOntoStack(L,3,coord);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetOrientationOnPath(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetOrientationOnPath");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        float coord[3];
        if (simGetOrientationOnPath_internal(luaToInt(L,1),luaToFloat(L,2),coord)==1)
        {
            pushFloatTableOntoStack(L,3,coord);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetClosestPositionOnPath(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetClosestPositionOnPath");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,3))
    {
        float coord[3];
        getFloatsFromTable(L,2,3,coord);
        float dist=0.0f;
        if (simGetClosestPositionOnPath_internal(luaToInt(L,1),coord,&dist)!=-1)
        {
            luaWrap_lua_pushnumber(L,dist);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simDelegateChildScriptExecution(luaWrap_lua_State* L)
{ // can only be called from a script running in a thread!!
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simDelegateChildScriptExecution");

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simWait(luaWrap_lua_State* L)
{ // can only be called from a script running in a thread!!
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simWait");

    if (!VThread::isCurrentThreadTheMainSimulationThread())
    {
        if (!(CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L))))
        { // Important to first check if we are supposed to leave the thread!!
            if (checkInputArguments(L,&errorString,lua_arg_number,0))
            {
                float deltaTime=luaToFloat(L,1);
                int res=checkOneGeneralInputArgument(L,2,lua_arg_bool,0,true,false,&errorString);
                if (res!=-1)
                {
                    bool simulationTime=true;
                    if (res==2)
                        simulationTime=luaToBool(L,2);
                    if (!simulationTime)
                    { // real-time wait
                        CVThreadData* threadData=CThreadPool::getCurrentThreadData();
                        int startTime=VDateTime::getTimeInMs();
                        bool err=false;
                        while (true)
                        {
                            float diff=float(VDateTime::getTimeDiffInMs(startTime))/1000.0f;
                            if (diff>=deltaTime)
                                break;
                            CThreadPool::switchBackToPreviousThread();
                            if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                            {
                                err=true;
                                break;
                            }
                        }
                        threadData->usedMovementTime=0.0f; // important!
                        if (!err)
                        {
                            luaWrap_lua_pushnumber(L,0.0f); // success (deltaTime left)
                            LUA_END(1);
                        }
                    }
                    else
                    { // simulation time wait
                        CVThreadData* threadData=CThreadPool::getCurrentThreadData();
                        float startTime=float(App::ct->simulation->getSimulationTime_ns())/1000000.0f+threadData->usedMovementTime;
                        float overshootTime=0.0f;
                        bool err=false;
                        while (true)
                        {
                            float currentTime=float(App::ct->simulation->getSimulationTime_ns())/1000000.0f;
                            float diff=currentTime-startTime;
                            if (diff>=deltaTime)
                            {
                                overshootTime=diff-deltaTime; // this is the "overshoot" time!
                                break;
                            }
                            CThreadPool::switchBackToPreviousThread();
                            if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                            {
                                err=true;
                                break;
                            }
                        }
                        threadData->usedMovementTime=overshootTime; // important!
                        if (!err)
                        {
                            luaWrap_lua_pushnumber(L,-overshootTime); // success (deltaTime left)
                            LUA_END(1);
                        }
                    }
                }
            }
        }
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simReadForceSensor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simReadForceSensor");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float force[3];
        float torque[3];
        retVal=simReadForceSensor_internal(luaToInt(L,1),force,torque);
        if (!( (retVal==-1)||((retVal&1)==0) ))
        {
            luaWrap_lua_pushnumber(L,retVal);
            pushFloatTableOntoStack(L,3,force);
            pushFloatTableOntoStack(L,3,torque);
            LUA_END(3);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simBreakForceSensor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simBreakForceSensor");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simBreakForceSensor_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetShapeVertex(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetShapeVertex");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        float relPos[3];
        retVal=simGetShapeVertex_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),relPos);
        if (retVal==1)
        {
            luaWrap_lua_pushnumber(L,retVal);
            pushFloatTableOntoStack(L,3,relPos);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetShapeTriangle(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetShapeTriangle");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int indices[3];
        float normals[9];
        retVal=simGetShapeTriangle_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),indices,normals);
        if (retVal==1)
        {
            luaWrap_lua_pushnumber(L,retVal);
            pushIntTableOntoStack(L,3,indices);
            pushFloatTableOntoStack(L,9,normals);
            LUA_END(3);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetLightParameters(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetLightParameters");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float ambientOld[3]={0.0f,0.0f,0.0f};
        float diffuse[3];
        float specular[3];
        retVal=simGetLightParameters_internal(luaToInt(L,1),NULL,diffuse,specular);
        if (retVal>=0)
        {
            luaWrap_lua_pushnumber(L,retVal);
            pushFloatTableOntoStack(L,3,ambientOld);
            pushFloatTableOntoStack(L,3,diffuse);
            pushFloatTableOntoStack(L,3,specular);
            LUA_END(4);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetLightParameters(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetLightParameters");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int objHandle=luaToInt(L,1);
        int state=luaToInt(L,2);
        float* diffuseP=NULL;
        float* specularP=NULL;
        float diffuse_[3]={0.0f,0.0f,0.0f};
        float specular_[3]={0.0f,0.0f,0.0f};
        int res=checkOneGeneralInputArgument(L,3,lua_arg_number,3,true,true,&errorString);
        if (res!=-1)
        {
            int res=checkOneGeneralInputArgument(L,4,lua_arg_number,3,true,true,&errorString);
            if (res!=-1)
            {
                if (res==2)
                { // get the data
                    getFloatsFromTable(L,4,3,diffuse_);
                    diffuseP=diffuse_;
                }
                int res=checkOneGeneralInputArgument(L,5,lua_arg_number,3,true,true,&errorString);
                if (res!=-1)
                {
                    if (res==2)
                    { // get the data
                        getFloatsFromTable(L,5,3,specular_);
                        specularP=specular_;
                    }
                    retVal=simSetLightParameters_internal(objHandle,state,NULL,diffuseP,specularP);
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetVelocity(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetVelocity");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float linVel[3];
        float angVel[3];
        int retVal=simGetVelocity_internal(luaToInt(L,1),linVel,angVel);
        if (retVal!=-1)
        {
            pushFloatTableOntoStack(L,3,linVel);
            pushFloatTableOntoStack(L,3,angVel);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetObjectVelocity(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectVelocity");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float linVel[3];
        float angVel[3];
        int retVal=simGetObjectVelocity_internal(luaToInt(L,1),linVel,angVel);
        if (retVal!=-1)
        {
            pushFloatTableOntoStack(L,3,linVel);
            pushFloatTableOntoStack(L,3,angVel);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simAddForceAndTorque(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddForceAndTorque");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float f[3]={0.0f,0.0f,0.0f};
        float t[3]={0.0f,0.0f,0.0f};
        bool err=false;
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,3,true,true,&errorString);
        if (res==2)
            getFloatsFromTable(L,2,3,f);
        err|=(res<0);
        if (!err)
        {
            int res=checkOneGeneralInputArgument(L,3,lua_arg_number,3,true,true,&errorString);
            if (res==2)
                getFloatsFromTable(L,3,3,t);
            err|=(res<0);
            if (!err)
                retVal=simAddForceAndTorque_internal(luaToInt(L,1),f,t);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAddForce(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddForce");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,3,lua_arg_number,3))
    {
        float r[3];
        float f[3];
        getFloatsFromTable(L,2,3,r);
        getFloatsFromTable(L,3,3,f);
        retVal=simAddForce_internal(luaToInt(L,1),r,f);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetExplicitHandling(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetExplicitHandling");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetExplicitHandling_internal(luaToInt(L,1),luaToInt(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetExplicitHandling(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetExplicitHandling");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simGetExplicitHandling_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetLinkDummy(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetLinkDummy");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simGetLinkDummy_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetLinkDummy(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetLinkDummy");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetLinkDummy_internal(luaToInt(L,1),luaToInt(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetShapeColor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetShapeColor");

    int retVal=-1; // means error
    int shapeHandle=-1;
    bool ok=false;
    bool correctColors=false;
    if (!checkInputArguments(L,NULL,lua_arg_number,0))
    { // this section is to guarantee backward compatibility: color values have changed in the release following 3.1.3. So we need to adjust them
        if (checkInputArguments(L,&errorString,lua_arg_string,0))
        {
            std::string txt(luaWrap_lua_tostring(L,1));
            if (txt.compare(0,20,"@backCompatibility1:")==0)
            {
                txt.assign(txt.begin()+20,txt.end());
                if (tt::getValidInt(txt,shapeHandle)) // try to extract the original number
                {
                    correctColors=true;
                    ok=true;
                }
                else
                    checkInputArguments(L,&errorString,lua_arg_number,0); // just generate an error
            }
            else
                checkInputArguments(L,&errorString,lua_arg_number,0); // just generate an error
        }
    }
    else
    {
        ok=true;
        shapeHandle=luaToInt(L,1);
    }

    if (ok)
    { // First arg ok
        std::string strTmp;
        char* str=NULL;
        int colorComponent=0;
        bool err=false;
        bool transformColor=false;
        int res=checkOneGeneralInputArgument(L,2,lua_arg_string,0,false,true,&errorString);
        if (res==2)
        {
            strTmp=luaWrap_lua_tostring(L,2);
            str=(char*)strTmp.c_str();
            transformColor=((strTmp.length()>1)&&(strTmp[0]=='@'));
        }
        err|=(res<1);
        if (!err)
        {
            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,false,false,&errorString);
            if (res==2)
                colorComponent=luaToInt(L,3);
            err|=(res<2);
            if (!err)
            {
                int floatsInTableExpected=3;
                if (colorComponent==4)
                    floatsInTableExpected=1;
                res=checkOneGeneralInputArgument(L,4,lua_arg_number,floatsInTableExpected,false,false,&errorString);
                if (res==2)
                {
                    float rgbData[3];
                    getFloatsFromTable(L,4,floatsInTableExpected,rgbData);
                    if (correctColors&&(colorComponent==0)&&(!transformColor) )
                    {
                        rgbData[0]=(rgbData[0]+0.25f)/0.85f;
                        rgbData[1]=(rgbData[1]+0.25f)/0.85f;
                        rgbData[2]=(rgbData[2]+0.25f)/0.85f;
                        float mx=SIM_MAX(SIM_MAX(rgbData[0],rgbData[1]),rgbData[2]);
                        if (mx>1.0f)
                        {
                            rgbData[0]/=mx;
                            rgbData[1]/=mx;
                            rgbData[2]/=mx;
                        }
                    }
                    retVal=simSetShapeColor_internal(shapeHandle,str,colorComponent,rgbData);
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetShapeColor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetShapeColor");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    { // First arg ok
        int shapeHandle=luaToInt(L,1);
        std::string strTmp;
        char* str=NULL;
        int colorComponent=0;
        bool err=false;
        int res=checkOneGeneralInputArgument(L,2,lua_arg_string,0,false,true,&errorString);
        if (res==2)
        {
            strTmp=luaWrap_lua_tostring(L,2);
            str=(char*)strTmp.c_str();
        }
        err|=(res<1);
        if (!err)
        {
            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,false,false,&errorString);
            if (res==2)
                colorComponent=luaToInt(L,3);
            err|=(res<2);
            if (!err)
            {
                float rgbData[3];
                retVal=simGetShapeColor_internal(shapeHandle,str,colorComponent,rgbData);
                if (retVal>0)
                {
                    luaWrap_lua_pushnumber(L,retVal);
                    pushFloatTableOntoStack(L,3,rgbData);
                    LUA_END(2);
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simResetDynamicObject(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simResetDynamicObject");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simResetDynamicObject_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetJointMode(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetJointMode");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetJointMode_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetJointMode(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetJointMode");

    int retVal=-1; // means error
    int options=0;
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        retVal=simGetJointMode_internal(luaToInt(L,1),&options);
        if (retVal>=0)
        {
            luaWrap_lua_pushnumber(L,retVal);
            luaWrap_lua_pushnumber(L,options);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSerialOpen(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSerialOpen");

    int retVal=-1; // means error
#ifdef SIM_WITH_SERIAL
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0))
    {
        size_t dataLength;
        const char* portName=luaWrap_lua_tolstring(L,1,&dataLength);
        int baudrate=luaToInt(L,2);
        retVal=App::ct->serialPortContainer->serialPortOpen(true,portName,baudrate);
    }
#endif

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSerialClose(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSerialClose");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        clearSerialPortLeftOver(luaToInt(L,1));
        retVal=simSerialClose_internal(luaToInt(L,1));
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSerialSend(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSerialSend");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0))
    {
        size_t dataLength;
        char* data=(char*)luaWrap_lua_tolstring(L,2,&dataLength);
        retVal=simSerialSend_internal(luaToInt(L,1),data,dataLength);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSerialRead(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSerialRead");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_bool,0))
    {
        bool blocking=luaToBool(L,3);
        std::string closingString="";
        float timeOut=0.0f;
        unsigned int maxLength=(unsigned int)luaToInt(L,2);
        std::string fullDataRead;
        if (blocking)
        {
            if (!VThread::isCurrentThreadTheMainSimulationThread())
            {
                int res=checkOneGeneralInputArgument(L,4,lua_arg_string,0,true,true,&errorString);
                if (res==2)
                {
                    size_t dataLength;
                    char* data=(char*)luaWrap_lua_tolstring(L,4,&dataLength);
                    closingString.assign(data,dataLength);
                }
                if (res!=-1)
                {
                    res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,false,&errorString);
                    if (res==2)
                        timeOut=luaToFloat(L,5);
                    if (res!=-1)
                    {
                        int startTime=VDateTime::getTimeInMs();
                        std::string leftOver=retrieveSerialPortLeftOver(luaToInt(L,1));
                        fullDataRead=leftOver;
                        unsigned int closingCheckedIndex=0;
                        char* data=new char[maxLength-leftOver.length()];
                        unsigned int csl=closingString.length();
                        while (true)
                        {
                            // 1. Check in what we already have:
                            if ( (csl>0)&&(closingCheckedIndex+csl<=fullDataRead.length()) )
                            {
                                bool foundTheString=false;
                                for (unsigned int j=closingCheckedIndex;j<=fullDataRead.length()-csl;j++)
                                {
                                    bool same=true;
                                    for (unsigned int i=0;i<csl;i++)
                                    {
                                        if (fullDataRead[j+i]!=closingString[i])
                                        {
                                            same=false;
                                            break;
                                        }
                                    }
                                    if (same)
                                    {
                                        std::string rest(fullDataRead.begin()+j+csl,fullDataRead.end());
                                        fullDataRead.assign(fullDataRead.begin(),fullDataRead.begin()+j);
                                        insertSerialPortLeftOver(luaToInt(L,1),rest);
                                        foundTheString=true;
                                        break;
                                    }
                                }
                                if (foundTheString)
                                    break;
                                closingCheckedIndex=fullDataRead.length()-csl+1;
                            }
                            // 2. Did we already read enough?
                            if (fullDataRead.length()>=maxLength)
                            { // yes
                                if (fullDataRead.length()==maxLength)
                                    break;
                                std::string rest(fullDataRead.begin()+maxLength,fullDataRead.end());
                                fullDataRead.assign(fullDataRead.begin(),fullDataRead.begin()+maxLength);
                                insertSerialPortLeftOver(luaToInt(L,1),rest);
                                break;
                            }
                            else
                            { // no
                                int nb=simSerialRead_internal(luaToInt(L,1),data,maxLength-fullDataRead.length());
                                if (nb<0)
                                    break; // error
                                std::string partString(data,nb);
                                fullDataRead+=partString;
                            }

                            bool leaveHere=false;
                            if (maxLength>fullDataRead.length())
                            {
                                CThreadPool::switchBackToPreviousThread();
                                if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                                    leaveHere=true;

                            }
                            if ( (timeOut>0.0000001f)&&((float(VDateTime::getTimeDiffInMs(startTime))/1000.0f)>timeOut) )
                                leaveHere=true;

                            if (leaveHere)
                            {
                                insertSerialPortLeftOver(luaToInt(L,1),fullDataRead);
                                fullDataRead.clear();
                                break;
                            }
                        }
                        delete[] data;
                    }
                }
            }
            else
                errorString=SIM_ERROR_BLOCKING_OPERATION_ONLY_FROM_THREAD;
        }
        else
        {
            std::string leftOver=retrieveSerialPortLeftOver(luaToInt(L,1));
            char* data=new char[maxLength-leftOver.length()];
            int nb=simSerialRead_internal(luaToInt(L,1),data,maxLength-leftOver.length());
            if (nb>0)
            { // no error
                std::string nextData(data,nb);
                fullDataRead=leftOver+nextData;
            }
            delete[] data;
        }

        if (fullDataRead.length()>0)
        {
            luaWrap_lua_pushlstring(L,fullDataRead.c_str(),fullDataRead.length());
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSerialCheck(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSerialCheck");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simSerialCheck_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetContactInfo(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetContactInfo");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int collidingObjects[2];
        float contactInfo[9];
        int index=luaToInt(L,3);
        if (index<sim_handleflag_extended)
            index+=sim_handleflag_extended;
        int res=simGetContactInfo_internal(luaToInt(L,1),luaToInt(L,2),index,collidingObjects,contactInfo);
        if (res==1)
        {
            pushIntTableOntoStack(L,2,collidingObjects);
            pushFloatTableOntoStack(L,3,contactInfo);
            pushFloatTableOntoStack(L,3,contactInfo+3);
            pushFloatTableOntoStack(L,3,contactInfo+6);
            LUA_END(4);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetThreadIsFree(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetThreadIsFree");

    int retVal=-1;
    if (!VThread::isCurrentThreadTheMainSimulationThread())
    {
        bool result=false;
        if (checkInputArguments(L,NULL,lua_arg_bool,0))
        {
            result=CThreadPool::setThreadFreeMode(luaToBool(L,1));
            if (result)
                retVal=1;
            else
                retVal=0;
        }
        else
            errorString=SIM_ERROR_INVALID_ARGUMENT;
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simTubeOpen(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simTubeOpen");

    int retVal=-1; // Error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0,lua_arg_number,0))
    {
        std::string strTmp=luaWrap_lua_tostring(L,2);
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        retVal=App::ct->commTubeContainer->openTube(luaToInt(L,1),strTmp.c_str(),(it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript)||(it->getScriptType()==sim_scripttype_jointctrlcallback)||(it->getScriptType()==sim_scripttype_contactcallback),luaToInt(L,3));//||(it->getScriptType()==sim_scripttype_generalcallback),luaToInt(L,3));
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simTubeClose(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simTubeClose");

    int retVal=-1; // Error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simTubeClose_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simTubeWrite(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simTubeWrite");

    int retVal=-1; // Error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0))
    {
        size_t dataLength;
        char* data=(char*)luaWrap_lua_tolstring(L,2,&dataLength);
        retVal=simTubeWrite_internal(luaToInt(L,1),data,dataLength);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simTubeRead(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simTubeRead");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int dataLength;
        bool blocking=false;
        int res=checkOneGeneralInputArgument(L,2,lua_arg_bool,0,true,false,&errorString);
        if (res==2)
            blocking=luaToBool(L,2);
        if (blocking)
        {
            CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
            if (it!=NULL)
            {
                if (!VThread::isCurrentThreadTheMainSimulationThread())
                {
                    char* data=simTubeRead_internal(luaToInt(L,1),&dataLength);
                    while (data==NULL)
                    {
                        // Now wait here until a button was pressed! (or the simulation is aborted)
                        CThreadPool::switchBackToPreviousThread();
                        if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                            break;
                        data=simTubeRead_internal(luaToInt(L,1),&dataLength);
                    }

                    if (data!=NULL)
                    {
                        luaWrap_lua_pushlstring(L,(const char*)data,dataLength);
                        delete[] data;
                        LUA_END(1);
                    }
                }
                else
                    errorString=SIM_ERROR_BLOCKING_OPERATION_ONLY_FROM_THREAD;
            }
        }
        else
        {
            char* data=simTubeRead_internal(luaToInt(L,1),&dataLength);
            if (data)
            {
                luaWrap_lua_pushlstring(L,(const char*)data,dataLength);
                delete[] data;
                LUA_END(1);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simTubeStatus(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simTubeStatus");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int readSize;
        int writeSize;
        int status=simTubeStatus_internal(luaToInt(L,1),&readSize,&writeSize);
        if (status>=0)
        {
            luaWrap_lua_pushnumber(L,status);
            luaWrap_lua_pushnumber(L,readSize);
            luaWrap_lua_pushnumber(L,writeSize);
            LUA_END(3);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simAuxiliaryConsoleOpen(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAuxiliaryConsoleOpen");

    int retVal=-1; // Error
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0,lua_arg_number,0))
    {
        int mode=luaToInt(L,3);
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        if ( (itScrObj->getScriptType()==sim_scripttype_mainscript)||(itScrObj->getScriptType()==sim_scripttype_childscript)||(itScrObj->getScriptType()==sim_scripttype_jointctrlcallback)||(itScrObj->getScriptType()==sim_scripttype_contactcallback) )//||(itScrObj->getScriptType()==sim_scripttype_generalcallback) )
        { // Add-ons and customization scripts do not have this restriction
            if ((mode&1)==0)
                mode++;
        }
        int* p=NULL;
        int* s=NULL;
        float* tc=NULL;
        float* bc=NULL;
        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,2,true,true,&errorString);
        if (res>=0)
        {
            int pp[2]={0,0};
            if (res==2)
            {
                getIntsFromTable(L,4,2,pp);
                p=pp;
            }
            res=checkOneGeneralInputArgument(L,5,lua_arg_number,2,true,true,&errorString);
            if (res>=0)
            {
                int ss[2]={0,0};
                if (res==2)
                {
                    getIntsFromTable(L,5,2,ss);
                    s=ss;
                }
                res=checkOneGeneralInputArgument(L,6,lua_arg_number,3,true,true,&errorString);
                if (res>=0)
                {
                    float tc_[3]={0.0f,0.0f,0.0f};
                    if (res==2)
                    {
                        getFloatsFromTable(L,6,3,tc_);
                        tc=tc_;
                    }
                    res=checkOneGeneralInputArgument(L,7,lua_arg_number,3,true,true,&errorString);
                    if (res>=0)
                    {
                        float bc_[3]={0.0f,0.0f,0.0f};
                        if (res==2)
                        {
                            getFloatsFromTable(L,7,3,bc_);
                            bc=bc_;
                        }
                        retVal=simAuxiliaryConsoleOpen_internal(luaWrap_lua_tostring(L,1),luaToInt(L,2),mode,p,s,tc,bc);
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAuxiliaryConsoleClose(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAuxiliaryConsoleClose");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simAuxiliaryConsoleClose_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAuxiliaryConsoleShow(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAuxiliaryConsoleShow");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_bool,0))
        retVal=simAuxiliaryConsoleShow_internal(luaToInt(L,1),luaToBool(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAuxiliaryConsolePrint(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAuxiliaryConsolePrint");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int res=checkOneGeneralInputArgument(L,2,lua_arg_string,0,false,true,&errorString);
        if (res==1)
            retVal=simAuxiliaryConsolePrint_internal(luaToInt(L,1),NULL);
        if (res==2)
            retVal=simAuxiliaryConsolePrint_internal(luaToInt(L,1),luaWrap_lua_tostring(L,2));
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simImportShape(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simImportShape");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int fileType=luaToInt(L,1);
        std::string pathAndFilename(luaWrap_lua_tostring(L,2));
        int options=luaToInt(L,3);
        float identicalVerticeTolerance=luaToFloat(L,4);
        float scalingFactor=luaToFloat(L,5);
        retVal=simImportShape_internal(fileType,pathAndFilename.c_str(),options,identicalVerticeTolerance,scalingFactor);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simImportMesh(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simImportMesh");

    int retValCnt=1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int fileType=luaToInt(L,1);
        std::string pathAndFilename(luaWrap_lua_tostring(L,2));
        int options=luaToInt(L,3);
        float identicalVerticeTolerance=luaToFloat(L,4);
        float scalingFactor=luaToFloat(L,5);
        float** vertices;
        int* verticesSizes;
        int** indices;
        int* indicesSizes;
        char** names;
        int elementCount=simImportMesh_internal(fileType,pathAndFilename.c_str(),options,identicalVerticeTolerance,scalingFactor,&vertices,&verticesSizes,&indices,&indicesSizes,NULL,&names);
        if (elementCount>0)
        {
            // Vertices:
            luaWrap_lua_newtable(L);
            int tablePos=luaWrap_lua_gettop(L);
            for (int i=0;i<elementCount;i++)
            {
                luaWrap_lua_newtable(L);
                int smallTablePos=luaWrap_lua_gettop(L);
                for (int j=0;j<verticesSizes[i];j++)
                {
                    luaWrap_lua_pushnumber(L,vertices[i][j]);
                    luaWrap_lua_rawseti(L,smallTablePos,j+1);
                }
                luaWrap_lua_rawseti(L,tablePos,i+1);
            }
            // Indices:
            luaWrap_lua_newtable(L);
            tablePos=luaWrap_lua_gettop(L);
            for (int i=0;i<elementCount;i++)
            {
                luaWrap_lua_newtable(L);
                int smallTablePos=luaWrap_lua_gettop(L);
                for (int j=0;j<indicesSizes[i];j++)
                {
                    luaWrap_lua_pushnumber(L,indices[i][j]);
                    luaWrap_lua_rawseti(L,smallTablePos,j+1);
                }
                luaWrap_lua_rawseti(L,tablePos,i+1);
            }
            // For future:
            luaWrap_lua_pushnil(L);

            // Names:
            luaWrap_lua_newtable(L);
            tablePos=luaWrap_lua_gettop(L);
            for (int i=0;i<elementCount;i++)
            {
                luaWrap_lua_pushstring(L,names[i]);
                luaWrap_lua_rawseti(L,tablePos,i+1);
            }
            retValCnt=4;

            for (int i=0;i<elementCount;i++)
            {
                delete[] names[i];
                delete[] indices[i];
                delete[] vertices[i];
            }
            delete[] names;
            delete[] indicesSizes;
            delete[] indices;
            delete[] verticesSizes;
            delete[] vertices;
            LUA_END(retValCnt);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simExportMesh(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simExportMesh");

    int retVal=-1; // indicates an error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0,lua_arg_number,0,lua_arg_number,0))
    {
        int fileType=luaToInt(L,1);
        std::string pathAndFilename(luaWrap_lua_tostring(L,2));
        int options=luaToInt(L,3);
        float scalingFactor=luaToFloat(L,4);
        int elementCount=15487;
        if ( (luaWrap_lua_gettop(L)>=8)&&luaWrap_lua_istable(L,5)&&luaWrap_lua_istable(L,6)&&luaWrap_lua_isnil(L,7)&&luaWrap_lua_istable(L,8) )
        {
            int ve=luaWrap_lua_objlen(L,5);
            int ie=luaWrap_lua_objlen(L,6);
            int nameE=luaWrap_lua_objlen(L,8);
            elementCount=SIM_MIN(SIM_MIN(ve,ie),nameE);
        }
        if ( (checkOneGeneralInputArgument(L,5,lua_arg_table,elementCount,false,false,&errorString)==2)&&
                (checkOneGeneralInputArgument(L,6,lua_arg_table,elementCount,false,false,&errorString)==2)&&
                (checkOneGeneralInputArgument(L,7,lua_arg_nil,0,false,false,&errorString)==2)&&
                (checkOneGeneralInputArgument(L,8,lua_arg_string,elementCount,false,false,&errorString)==2) )
        {
            float** vertices;
            int* verticesSizes; 
            int** indices;
            int* indicesSizes;
            char** names;
            vertices=new float*[elementCount];
            verticesSizes=new int[elementCount];
            indices=new int*[elementCount];
            indicesSizes=new int[elementCount];
            names=new char*[elementCount];
            // Following needed if we break before (because one table in a table is not correct (or not a table)):
            for (int i=0;i<elementCount;i++)
            {
                vertices[i]=NULL;
                verticesSizes[i]=0;
                indices[i]=NULL;
                indicesSizes[i]=0;
                names[i]=NULL;
            }

            bool error=false;
            for (int i=0;i<elementCount;i++)
            {
                if (!error)
                {
                    luaWrap_lua_rawgeti(L,5,i+1);
                    if (luaWrap_lua_istable(L,-1))
                    {
                        int vl=luaWrap_lua_objlen(L,-1);
                        if (checkOneGeneralInputArgument(L,luaWrap_lua_gettop(L),lua_arg_number,vl,false,false,&errorString)==2)
                        {
                            verticesSizes[i]=vl;
                            vertices[i]=new float[vl];
                            getFloatsFromTable(L,luaWrap_lua_gettop(L),vl,vertices[i]);
                        }
                        else
                            error=true;
                    }
                    else
                        error=true;
                    luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                }
                if (!error)
                {
                    luaWrap_lua_rawgeti(L,6,i+1);
                    if (luaWrap_lua_istable(L,-1))
                    {
                        int vl=luaWrap_lua_objlen(L,-1);
                        if (checkOneGeneralInputArgument(L,luaWrap_lua_gettop(L),lua_arg_number,vl,false,false,&errorString)==2)
                        {
                            indicesSizes[i]=vl;
                            indices[i]=new int[vl];
                            getIntsFromTable(L,luaWrap_lua_gettop(L),vl,indices[i]);
                        }
                        else
                            error=true;
                    }
                    else
                        error=true;
                    luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                }
                if (!error)
                {
                    luaWrap_lua_rawgeti(L,8,i+1);
                    std::string name(luaWrap_lua_tostring(L,-1));
                    names[i]=new char[name.length()+1];
                    for (int j=0;j<int(name.length());j++)
                        names[i][j]=name[j];
                    names[i][name.length()]=0; // terminal 0
                    luaWrap_lua_pop(L,1); // we have to pop the value that was pushed with luaWrap_lua_rawgeti
                }
            }

            if (!error)
                retVal=simExportMesh_internal(fileType,pathAndFilename.c_str(),options,scalingFactor,elementCount,vertices,verticesSizes,indices,indicesSizes,NULL,names);

            for (int i=0;i<elementCount;i++)
            {
                delete[] vertices[i];
                delete[] indices[i];
                delete[] names[i];
            }
            delete[] names;
            delete[] indicesSizes;
            delete[] indices;
            delete[] verticesSizes;
            delete[] vertices;
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}



int _simCreateMeshShape(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateMeshShape");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int options=luaToInt(L,1);
        float shadingAngle=luaToFloat(L,2);

        int vl=2;
        int il=2;
        if ( (luaWrap_lua_gettop(L)>=4)&&luaWrap_lua_istable(L,3)&&luaWrap_lua_istable(L,4) )
        {
            vl=luaWrap_lua_objlen(L,3);
            il=luaWrap_lua_objlen(L,4);
        }
        int res=checkOneGeneralInputArgument(L,3,lua_arg_number,vl,false,false,&errorString);
        if (res==2)
        {
            res=checkOneGeneralInputArgument(L,4,lua_arg_number,il,false,false,&errorString);
            if (res==2)
            {
                float* vertices=new float[vl];
                int* indices=new int[il];
                getFloatsFromTable(L,3,vl,vertices);
                getIntsFromTable(L,4,il,indices);
                retVal=simCreateMeshShape_internal(options,shadingAngle,vertices,vl,indices,il,NULL);
                delete[] indices;
                delete[] vertices;
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetShapeMesh(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetShapeMesh");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float* vertices;
        int verticesSize;
        int* indices;
        int indicesSize;
        float* normals;
        int result=simGetShapeMesh_internal(luaToInt(L,1),&vertices,&verticesSize,&indices,&indicesSize,&normals);
        if (result>0)
        {
            pushFloatTableOntoStack(L,verticesSize,vertices);
            pushIntTableOntoStack(L,indicesSize,indices);
            pushFloatTableOntoStack(L,indicesSize*3,normals);
            delete[] vertices;
            delete[] indices;
            delete[] normals;
            LUA_END(3);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simCreatePureShape(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreatePureShape");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,3,lua_arg_number,0))
    {
        int primitiveType=luaToInt(L,1);
        int options=luaToInt(L,2);
        float sizes[3];
        getFloatsFromTable(L,3,3,sizes);
        float mass=luaToFloat(L,4);
        int* precision=NULL;
        int prec[2];
        int res=checkOneGeneralInputArgument(L,5,lua_arg_number,2,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
            {
                getIntsFromTable(L,5,2,prec);
                precision=prec;
            }
            retVal=simCreatePureShape_internal(primitiveType,options,sizes,mass,precision);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreateHeightfieldShape(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateHeightfieldShape");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int options=luaToInt(L,1);
        float shadingAngle=luaToFloat(L,2);
        int xPointCount=luaToInt(L,3);
        int yPointCount=luaToInt(L,4);
        float xSize=luaToFloat(L,5);
        int res=checkOneGeneralInputArgument(L,6,lua_arg_number,xPointCount*yPointCount,false,false,&errorString);
        if (res==2)
        {
            float* heights=new float[xPointCount*yPointCount];
            getFloatsFromTable(L,6,xPointCount*yPointCount,heights);
            retVal=simCreateHeightfieldShape_internal(options,shadingAngle,xPointCount,yPointCount,xSize,heights);
            delete[] heights;
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}


int _simAddBanner(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddBanner");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0,lua_arg_number,0))
    {
        std::string label(luaWrap_lua_tostring(L,1));
        float size=luaToFloat(L,2);
        int options=luaToInt(L,3);
        
        float* positionAndEulerAngles=NULL;
        int parentObjectHandle=-1;
        float* labelColors=NULL;
        float* backgroundColors=NULL;

        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,6,true,true,&errorString);
        int okToGo=(res!=-1);
        if (okToGo)
        {
            float positionAndEulerAnglesC[6];
            if (res>0)
            {
                if (res==2)
                {
                    getFloatsFromTable(L,4,6,positionAndEulerAnglesC);
                    positionAndEulerAngles=positionAndEulerAnglesC;
                }
                res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,true,&errorString);
                okToGo=(res!=-1);
                if (okToGo)
                {
                    if (res>0)
                    {
                        if (res==2)
                            parentObjectHandle=luaToInt(L,5);
                        res=checkOneGeneralInputArgument(L,6,lua_arg_number,12,true,true,&errorString);
                        okToGo=(res!=-1);
                        if (okToGo)
                        {
                            float labelColorsC[12];
                            if (res>0)
                            {
                                if (res==2)
                                {
                                    getFloatsFromTable(L,6,12,labelColorsC);
                                    labelColors=labelColorsC;
                                }
                                res=checkOneGeneralInputArgument(L,7,lua_arg_number,12,true,true,&errorString);
                                okToGo=(res!=-1);
                                if (okToGo)
                                {
                                    float backgroundColorsC[12];
                                    if (res>0)
                                    {
                                        if (res==2)
                                        {
                                            getFloatsFromTable(L,7,12,backgroundColorsC);
                                            backgroundColors=backgroundColorsC;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (okToGo)
            {
                retVal=simAddBanner_internal(label.c_str(),size,options,positionAndEulerAngles,parentObjectHandle,labelColors,backgroundColors);
                if (retVal!=-1)
                { // following condition added on 2011/01/06 so as to not remove objects created from the c/c++ interface or an add-on:
                    int currentScriptID=getCurrentScriptID(L);
                    CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
                    CBannerObject* anObj=App::ct->bannerCont->getObject(retVal);
                    if (anObj!=NULL)
                        anObj->setCreatedFromScript((itScrObj->getScriptType()==sim_scripttype_mainscript)||(itScrObj->getScriptType()==sim_scripttype_childscript)||(itScrObj->getScriptType()==sim_scripttype_jointctrlcallback)||(itScrObj->getScriptType()==sim_scripttype_contactcallback)); //||(itScrObj->getScriptType()==sim_scripttype_generalcallback));
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRemoveBanner(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRemoveBanner");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int objectHandle=luaToInt(L,1);
        if (objectHandle==sim_handle_all)
        { // following condition added here on 2011/01/06 so as not to remove objects created from a C/c++ call
            App::ct->bannerCont->removeAllObjects(true);
            retVal=1;
        }
        else
            retVal=simRemoveBanner_internal(objectHandle);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreateJoint(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateJoint");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int jointType=luaToInt(L,1);
        int jointMode=luaToInt(L,2);
        int options=luaToInt(L,3);
        float* sizes=NULL;
        float* colorA=NULL;
        float* colorB=NULL;
        float s[2];
        float cA[12];
        float cB[12];
        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,2,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
            {
                getFloatsFromTable(L,4,2,s);
                sizes=s;
            }
            res=checkOneGeneralInputArgument(L,5,lua_arg_number,12,true,true,&errorString);
            if (res>=0)
            {
                if (res==2)
                {
                    getFloatsFromTable(L,5,12,cA);
                    colorA=cA;
                }
                res=checkOneGeneralInputArgument(L,6,lua_arg_number,12,true,true,&errorString);
                if (res>=0)
                {
                    if (res==2)
                    {
                        getFloatsFromTable(L,6,12,cB);
                        colorB=cB;
                    }
                    retVal=simCreateJoint_internal(jointType,jointMode,options,sizes,colorA,colorB);
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreateDummy(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateDummy");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float size=luaToFloat(L,1);
        float* color=NULL;
        float c[12];
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,12,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
            {
                getFloatsFromTable(L,2,12,c);
                color=c;
            }
            retVal=simCreateDummy_internal(size,color);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreatePath(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreatePath");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int attribs=luaToInt(L,1);
        int* intP=NULL;
        float* floatP=NULL;
        int intParams[3];
        float floatParams[3];
        float* color=NULL;
        float c[12];
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,3,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
            {
                getIntsFromTable(L,2,3,intParams);
                intP=intParams;
            }
            res=checkOneGeneralInputArgument(L,3,lua_arg_number,3,true,true,&errorString);
            if (res>=0)
            {
                if (res==2)
                {
                    getFloatsFromTable(L,3,3,floatParams);
                    floatP=floatParams;
                }
                res=checkOneGeneralInputArgument(L,4,lua_arg_number,12,true,true,&errorString);
                if (res>=0)
                {
                    if (res==2)
                    {
                        getFloatsFromTable(L,4,12,c);
                        color=c;
                    }
                    retVal=simCreatePath_internal(attribs,intP,floatP,color);
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreateProximitySensor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateProximitySensor");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,8,lua_arg_number,15))
    {
        int sensorType=luaToInt(L,1);
        int subType=luaToInt(L,2);
        int options=luaToInt(L,3);
        int intParams[8];
        float floatParams[15];
        getIntsFromTable(L,4,8,intParams);
        getFloatsFromTable(L,5,15,floatParams);

        float* color=NULL;
        float c[48];
        int res=checkOneGeneralInputArgument(L,6,lua_arg_number,48,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
            {
                getFloatsFromTable(L,6,48,c);
                color=c;
            }
            retVal=simCreateProximitySensor_internal(sensorType,subType,options,intParams,floatParams,color);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreateForceSensor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateForceSensor");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,5,lua_arg_number,5))
    {
        int options=luaToInt(L,1);
        int intParams[5];
        float floatParams[5];
        getIntsFromTable(L,2,5,intParams);
        getFloatsFromTable(L,3,5,floatParams);

        float* color=NULL;
        float c[24];
        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,24,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
            {
                getFloatsFromTable(L,4,24,c);
                color=c;
            }
            retVal=simCreateForceSensor_internal(options,intParams,floatParams,color);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreateVisionSensor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateVisionSensor");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,4,lua_arg_number,11))
    {
        int options=luaToInt(L,1);
        int intParams[4];
        float floatParams[11];
        getIntsFromTable(L,2,4,intParams);
        getFloatsFromTable(L,3,11,floatParams);

        float* color=NULL;
        float c[48];
        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,48,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
            {
                getFloatsFromTable(L,4,48,c);
                color=c;
            }
            retVal=simCreateVisionSensor_internal(options,intParams,floatParams,color);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simFloatingViewAdd(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simFloatingViewAdd");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
        retVal=simFloatingViewAdd_internal(luaToFloat(L,1),luaToFloat(L,2),luaToFloat(L,3),luaToFloat(L,4),luaToInt(L,5));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simFloatingViewRemove(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simFloatingViewRemove");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simFloatingViewRemove_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAdjustView(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAdjustView");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int res=checkOneGeneralInputArgument(L,4,lua_arg_string,0,true,true,&errorString);
        if (res>=0)
        {
            char* txt=NULL;
            if (res==2)
                txt=(char*)luaWrap_lua_tostring(L,4);
            if ( (txt!=NULL)&&(strlen(txt)>0) )
                retVal=simAdjustView_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),txt);
            else
                retVal=simAdjustView_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),NULL);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCameraFitToView(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCameraFitToView");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int* objPtr=NULL;
        int options=0;
        float scaling=1.0f;
        int tableLen=2;
        if (luaWrap_lua_istable(L,2))
        {
            tableLen=int(luaWrap_lua_objlen(L,2));
            int* buffer=new int[tableLen];
            objPtr=buffer;
            getIntsFromTable(L,2,tableLen,buffer);
        }
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,tableLen,true,true,&errorString);
        if (res>=0)
        {
            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
            if (res>=0)
            {
                if (res==2)
                    options=luaToInt(L,3);
                res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,false,&errorString);
                if (res>=0)
                {
                    if (res==2)
                        scaling=luaToFloat(L,4);
                    retVal=simCameraFitToView_internal(luaToInt(L,1),tableLen,objPtr,options,scaling);
                }
            }
        }
        delete[] objPtr;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAnnounceSceneContentChange(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAnnounceSceneContentChange");

    int retVal=simAnnounceSceneContentChange_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectInt32Parameter(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectInt32Parameter");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int param;
        retVal=simGetObjectInt32Parameter_internal(luaToInt(L,1),luaToInt(L,2),&param);
        if (retVal>0)
        {
            luaWrap_lua_pushnumber(L,retVal);
            luaWrap_lua_pushnumber(L,param);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetObjectInt32Parameter(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetObjectInt32Parameter");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetObjectInt32Parameter_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}


int _simGetObjectFloatParameter(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectFloatParameter");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        float param;
        retVal=simGetObjectFloatParameter_internal(luaToInt(L,1),luaToInt(L,2),&param);
        if (retVal>0)
        {
            luaWrap_lua_pushnumber(L,retVal);
            luaWrap_lua_pushnumber(L,param);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetObjectFloatParameter(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetObjectFloatParameter");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetObjectFloatParameter_internal(luaToInt(L,1),luaToInt(L,2),luaToFloat(L,3));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectStringParameter(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectStringParameter");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int paramLength;
        char* strBuff=simGetObjectStringParameter_internal(luaToInt(L,1),luaToInt(L,2),&paramLength);
        if (strBuff!=NULL)
        {
            luaWrap_lua_pushlstring(L,strBuff,paramLength);
            delete[] strBuff;
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetObjectStringParameter(luaWrap_lua_State* L)
{ 
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetObjectStringParameter");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_string,0))
    {
        size_t dataLength;
        char* data=(char*)luaWrap_lua_tolstring(L,3,&dataLength);
        retVal=simSetObjectStringParameter_internal(luaToInt(L,1),luaToInt(L,2),data,dataLength);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetRotationAxis(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetRotationAxis");

    if (checkInputArguments(L,&errorString,lua_arg_number,12,lua_arg_number,12))
    {
        float inM0[12];
        float inM1[12];
        float outAxis[3];
        float angle;
        getFloatsFromTable(L,1,12,inM0);
        getFloatsFromTable(L,2,12,inM1);
        if (simGetRotationAxis_internal(inM0,inM1,outAxis,&angle)!=-1)
        {
            pushFloatTableOntoStack(L,3,outAxis);
            luaWrap_lua_pushnumber(L,angle);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simRotateAroundAxis(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRotateAroundAxis");

    if (checkInputArguments(L,&errorString,lua_arg_number,12,lua_arg_number,3,lua_arg_number,3,lua_arg_number,0))
    {
        float inM[12];
        float axis[3];
        float pos[3];
        float outM[12];
        getFloatsFromTable(L,1,12,inM);
        getFloatsFromTable(L,2,3,axis);
        getFloatsFromTable(L,3,3,pos);
        if (simRotateAroundAxis_internal(inM,axis,pos,luaToFloat(L,4),outM)!=-1)
        {
            pushFloatTableOntoStack(L,12,outM);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetJointForce(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetJointForce");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float jointF[1];
        if (simGetJointForce_internal(luaToInt(L,1),jointF)>0)
        {
            luaWrap_lua_pushnumber(L,jointF[0]);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetIkGroupProperties(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetIkGroupProperties");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetIkGroupProperties_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),luaToFloat(L,4),NULL);

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetIkElementProperties(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetIkElementProperties");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,2,true,true,&errorString);
        if (res>=0)
        {
            float* precision=NULL;
            float prec[2];
            if (res==2)
            {
                getFloatsFromTable(L,4,2,prec);
                precision=prec;
            }
            res=checkOneGeneralInputArgument(L,5,lua_arg_number,2,true,true,&errorString);
            if (res>=0)
            {
                float* weight=NULL;
                float w[2];
                if (res==2)
                {
                    getFloatsFromTable(L,5,2,w);
                    weight=w;
                }
                retVal=simSetIkElementProperties_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),precision,weight,NULL);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simPersistentDataWrite(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simPersistentDataWrite");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_string,0))
    {
        int options=0;
        int res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            if (res==2)
                options=luaToInt(L,3);
            size_t dataLength;
            char* data=(char*)luaWrap_lua_tolstring(L,2,&dataLength);
            retVal=simPersistentDataWrite_internal(std::string(luaWrap_lua_tostring(L,1)).c_str(),data,dataLength,options);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simPersistentDataRead(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simPersistentDataRead");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        int stringLength;
        char* str=simPersistentDataRead_internal(std::string(luaWrap_lua_tostring(L,1)).c_str(),&stringLength);

        if (str!=NULL)
        {
            luaWrap_lua_pushlstring(L,str,stringLength);
            simReleaseBuffer_internal(str);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simIsHandleValid(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simIsHandleValid");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int objType=-1;
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            if (res==2)
                objType=luaToInt(L,2);
            retVal=simIsHandleValid_internal(luaToInt(L,1),objType);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRMLPos(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRMLPos");

    int retVal=-43; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int dofs=luaToInt(L,1);
        double timeStep=luaWrap_lua_tonumber(L,2);
        int flags=luaToInt(L,3);
        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,dofs*3,false,false,&errorString);
        if (res==2)
        {
            double* currentPosVelAccel=new double[dofs*3];
            getDoublesFromTable(L,4,dofs*3,currentPosVelAccel);
            res=checkOneGeneralInputArgument(L,5,lua_arg_number,dofs*3,false,false,&errorString);
            if (res==2)
            {
                double* maxVelAccelJerk=new double[dofs*3];
                getDoublesFromTable(L,5,dofs*3,maxVelAccelJerk);
                res=checkOneGeneralInputArgument(L,6,lua_arg_bool,dofs,false,false,&errorString);
                if (res==2)
                {
                    char* selection=new char[dofs];
                    getBoolsFromTable(L,6,dofs,selection);
                    res=checkOneGeneralInputArgument(L,7,lua_arg_number,dofs*2,false,false,&errorString);
                    if (res==2)
                    {
                        double* targetPosVel=new double[dofs*2];
                        getDoublesFromTable(L,7,dofs*2,targetPosVel);

                        unsigned char auxData[1+4];
                        auxData[0]=1;
                        ((int*)(auxData+1))[0]=0;

                        int currentScriptID=getCurrentScriptID(L);
                        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
                        if ((it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript)||(it->getScriptType()==sim_scripttype_jointctrlcallback)||(it->getScriptType()==sim_scripttype_contactcallback)) //||(it->getScriptType()==sim_scripttype_generalcallback))
                            ((int*)(auxData+1))[0]=1; // destroy at simulation end!
                        retVal=simRMLPos_internal(dofs,timeStep,flags,currentPosVelAccel,maxVelAccelJerk,(unsigned char*)selection,targetPosVel,auxData);
                        delete[] targetPosVel;
                    }
                    delete[] selection;
                }
                delete[] maxVelAccelJerk;
            }
            delete[] currentPosVelAccel;
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRMLStep(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRMLStep");

    int retVal=-1; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int handle=luaToInt(L,1);
        double timeStep=luaWrap_lua_tonumber(L,2);

        // Get the Dofs of this object:
        int dofs=-1;
        {
            int auxVals[4]={0,handle,0,0};
            int replyData[4]={-1,-1,-1,-1};

            if (CPluginContainer::getPluginFromName("ReflexxesTypeIV")!=NULL)
                CPluginContainer::sendEventCallbackMessageToOnePlugin("ReflexxesTypeIV",sim_message_eventcallback_rmlinfo,auxVals,NULL,replyData);
            else
            {
                if (CPluginContainer::getPluginFromName("ReflexxesTypeII")!=NULL)
                    CPluginContainer::sendEventCallbackMessageToOnePlugin("ReflexxesTypeII",sim_message_eventcallback_rmlinfo,auxVals,NULL,replyData);
            }
//            CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_rmlinfo,auxVals,NULL,replyData);
            dofs=replyData[1];
        }

        if (dofs>0)
        {
            double* newPosVelAccel=new double[dofs*3];
            unsigned char auxData[1+8+8];
            auxData[0]=1;
            retVal=simRMLStep_internal(handle,timeStep,newPosVelAccel,auxData,NULL);
            if (retVal>=0)
            {
                luaWrap_lua_pushnumber(L,retVal);
                pushDoubleTableOntoStack(L,dofs*3,newPosVelAccel);
                luaWrap_lua_pushnumber(L,((double*)(auxData+1))[0]);
                delete[] newPosVelAccel;
                LUA_END(3);
            }
            delete[] newPosVelAccel;
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRMLRemove(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRMLRemove");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaToInt(L,1);
        retVal=simRMLRemove_internal(handle);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}


int _simRMLVel(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRMLVel");

    int retVal=-43; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int dofs=luaToInt(L,1);
        double timeStep=luaWrap_lua_tonumber(L,2);
        int flags=luaToInt(L,3);
        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,dofs*3,false,false,&errorString);
        if (res==2)
        {
            double* currentPosVelAccel=new double[dofs*3];
            getDoublesFromTable(L,4,dofs*3,currentPosVelAccel);
            res=checkOneGeneralInputArgument(L,5,lua_arg_number,dofs*2,false,false,&errorString);
            if (res==2)
            {
                double* maxAccelJerk=new double[dofs*2];
                getDoublesFromTable(L,5,dofs*2,maxAccelJerk);
                res=checkOneGeneralInputArgument(L,6,lua_arg_bool,dofs,false,false,&errorString);
                if (res==2)
                {
                    char* selection=new char[dofs];
                    getBoolsFromTable(L,6,dofs,selection);
                    res=checkOneGeneralInputArgument(L,7,lua_arg_number,dofs,false,false,&errorString);
                    if (res==2)
                    {
                        double* targetVel=new double[dofs];
                        getDoublesFromTable(L,7,dofs,targetVel);

                        unsigned char auxData[1+4];
                        auxData[0]=1;
                        ((int*)(auxData+1))[0]=0;

                        int currentScriptID=getCurrentScriptID(L);
                        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
                        if ((it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript)||(it->getScriptType()==sim_scripttype_jointctrlcallback)||(it->getScriptType()==sim_scripttype_contactcallback)) //||(it->getScriptType()==sim_scripttype_generalcallback))
                            ((int*)(auxData+1))[0]=1; // destroy at simulation end!

                        retVal=simRMLVel_internal(dofs,timeStep,flags,currentPosVelAccel,maxAccelJerk,(unsigned char*)selection,targetVel,auxData);
                        delete[] targetVel;
                    }
                    delete[] selection;
                }
                delete[] maxAccelJerk;
            }
            delete[] currentPosVelAccel;
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}


int _simGetObjectQuaternion(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectQuaternion");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        float coord[4];
        if (simGetObjectQuaternion_internal(luaToInt(L,1),luaToInt(L,2),coord)==1)
        {
            pushFloatTableOntoStack(L,4,coord);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetObjectQuaternion(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetObjectQuaternion");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,4))
    {
        float coord[4];
        getFloatsFromTable(L,3,4,coord);
        retVal=simSetObjectQuaternion_internal(luaToInt(L,1),luaToInt(L,2),coord);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simBuildMatrixQ(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simBuildMatrixQ");

    if (checkInputArguments(L,&errorString,lua_arg_number,3,lua_arg_number,4))
    {
        float arr[12];
        float pos[3];
        float quaternion[4];
        getFloatsFromTable(L,1,3,pos);
        getFloatsFromTable(L,2,4,quaternion);
        if (simBuildMatrixQ_internal(pos,quaternion,arr)==1)
        {
            pushFloatTableOntoStack(L,12,arr);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetQuaternionFromMatrix(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetQuaternionFromMatrix");

    if (checkInputArguments(L,&errorString,lua_arg_number,12))
    {
        float arr[12];
        float quaternion[4];
        getFloatsFromTable(L,1,12,arr);
        if (simGetQuaternionFromMatrix_internal(arr,quaternion)==1)
        {
            pushFloatTableOntoStack(L,4,quaternion);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simRMLMoveToPosition(luaWrap_lua_State* L)
{ // can only be called from a script running in a thread!!
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRMLMoveToPosition");

    int retVal=-1;
    if (!VThread::isCurrentThreadTheMainSimulationThread())
    {
        if ((!CThreadPool::getSimulationStopRequested())&&(isObjectAssociatedWithThisThreadedChildScriptValid(L)))
        {
            if (checkInputArguments(L,NULL,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
            { // the 3 first types are ok!
                int objectHandle=luaToInt(L,1);
                int relativeToObjectHandle=luaToInt(L,2);
                int flags=luaToInt(L,3);
                C3DObject* it=App::ct->objCont->getObject(objectHandle);
                C3DObject* relativeIt=NULL;
                bool relParentError=false;
                if (relativeToObjectHandle!=-1)
                {
                    if (relativeToObjectHandle==sim_handle_parent)
                        relativeIt=it->getParent();
                    else
                    {
                        relativeIt=App::ct->objCont->getObject(relativeToObjectHandle);
                        relParentError=(relativeIt==NULL);
                    }
                }
                if ( (it==NULL)||((relativeIt==NULL)&&relParentError) )
                    errorString=SIM_ERROR_OBJECT_INEXISTANT;
                else
                {
                    C7Vector startPose(it->getCumulativeTransformationPart1());
                    C7Vector tr;
                    tr.setIdentity();
                    if (relativeIt!=NULL)
                        tr=relativeIt->getCumulativeTransformationPart1();
                    C7Vector trInv(tr.getInverse());
                    startPose=trInv*startPose;
                    C7Vector goalPose(startPose); // if we specify nil for the goal pos/quat we use the same as start

                    double currentVel[4]={0.0,0.0,0.0,0.0};
                    double currentAccel[4]={0.0,0.0,0.0,0.0};
                    double maxVel[4];
                    double maxAccel[4];
                    double maxJerk[4];
                    double targetVel[4]={0.0,0.0,0.0,0.0};
                    bool argError=false;

                    // currentVel
                    int res=checkOneGeneralInputArgument(L,4,lua_arg_number,4,false,true,&errorString);
                    if ((!argError)&&(res<1))
                        argError=true;
                    else
                    {
                        if (res==2)
                            getDoublesFromTable(L,4,4,currentVel);
                    }

                    // currentAccel
                    res=checkOneGeneralInputArgument(L,5,lua_arg_number,4,false,true,&errorString);
                    if ((!argError)&&(res<1))
                        argError=true;
                    else
                    {
                        if (res==2)
                            getDoublesFromTable(L,5,4,currentAccel);
                    }

                    // maxVel
                    res=checkOneGeneralInputArgument(L,6,lua_arg_number,4,false,false,&errorString);
                    if ((!argError)&&(res<2))
                        argError=true;
                    else
                    {
                        if (res==2)
                            getDoublesFromTable(L,6,4,maxVel);
                    }

                    // maxAccel
                    res=checkOneGeneralInputArgument(L,7,lua_arg_number,4,false,false,&errorString);
                    if ((!argError)&&(res<2))
                        argError=true;
                    else
                    {
                        if (res==2)
                            getDoublesFromTable(L,7,4,maxAccel);
                    }

                    // maxJerk
                    res=checkOneGeneralInputArgument(L,8,lua_arg_number,4,false,false,&errorString);
                    if ((!argError)&&(res<2))
                        argError=true;
                    else
                    {
                        if (res==2)
                            getDoublesFromTable(L,8,4,maxJerk);
                    }

                    // targetPos
                    res=checkOneGeneralInputArgument(L,9,lua_arg_number,3,false,true,&errorString);
                    if ((!argError)&&(res<1))
                        argError=true;
                    else
                    {
                        if (res==2)
                        {
                            float dummy[3];
                            getFloatsFromTable(L,9,3,dummy);
                            goalPose.X.set(dummy);
                        }
                    }

                    // targetQuat
                    res=checkOneGeneralInputArgument(L,10,lua_arg_number,4,false,true,&errorString);
                    if ((!argError)&&(res<1))
                        argError=true;
                    else
                    {
                        if (res==2)
                        {
                            float dummy[4];
                            getFloatsFromTable(L,10,4,dummy);
                            goalPose.Q(0)=dummy[3];
                            goalPose.Q(1)=dummy[0];
                            goalPose.Q(2)=dummy[1];
                            goalPose.Q(3)=dummy[2];
                        }
                    }

                    // targetVel
                    res=checkOneGeneralInputArgument(L,11,lua_arg_number,4,false,true,&errorString);
                    if ((!argError)&&(res<1))
                        argError=true;
                    else
                    {
                        if (res==2)
                            getDoublesFromTable(L,11,4,targetVel);
                    }

                    if (!argError)
                    {
                        float matrStart[12];
                        float matrGoal[12];
                        float axis[3];
                        float angle;
                        float quat[4];
                        quat[0]=startPose.Q(1);
                        quat[1]=startPose.Q(2);
                        quat[2]=startPose.Q(3);
                        quat[3]=startPose.Q(0);
                        simBuildMatrixQ_internal(startPose.X.data,quat,matrStart);
                        quat[0]=goalPose.Q(1);
                        quat[1]=goalPose.Q(2);
                        quat[2]=goalPose.Q(3);
                        quat[3]=goalPose.Q(0);
                        simBuildMatrixQ_internal(goalPose.X.data,quat,matrGoal);
                        simGetRotationAxis_internal(matrStart,matrGoal,axis,&angle);
                        unsigned char auxData[9];
                        auxData[0]=1;
                        double currentPosVelAccel[3*4];
                        currentPosVelAccel[0+0]=(double)startPose.X(0);
                        currentPosVelAccel[0+1]=(double)startPose.X(1);
                        currentPosVelAccel[0+2]=(double)startPose.X(2);
                        currentPosVelAccel[0+3]=0.0;
                        for (int i=0;i<4;i++)
                            currentPosVelAccel[4+i]=currentVel[i];
                        for (int i=0;i<4;i++)
                            currentPosVelAccel[8+i]=currentAccel[i];

                        double maxVelAccelJerk[3*4];
                        for (int i=0;i<4;i++)
                            maxVelAccelJerk[0+i]=maxVel[i];
                        for (int i=0;i<4;i++)
                            maxVelAccelJerk[4+i]=maxAccel[i];
                        for (int i=0;i<4;i++)
                            maxVelAccelJerk[8+i]=maxJerk[i];

                        unsigned char selection[4]={1,1,1,1};

                        double targetPosVel[2*4];
                        targetPosVel[0+0]=(double)goalPose.X(0);
                        targetPosVel[0+1]=(double)goalPose.X(1);
                        targetPosVel[0+2]=(double)goalPose.X(2);
                        targetPosVel[0+3]=(double)angle;
                        for (int i=0;i<4;i++)
                            targetPosVel[4+i]=targetVel[i];

                        double newPosVelAccel[3*4];
                        bool movementFinished=false;


                        int rmlHandle=simRMLPos_internal(4,0.0001,flags,currentPosVelAccel,maxVelAccelJerk,selection,targetPosVel,NULL);
                        while (!movementFinished)
                        {
                            double dt=double(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0;
                            int rmlRes=simRMLStep_internal(rmlHandle,dt,newPosVelAccel,auxData,NULL);
                            it=App::ct->objCont->getObject(objectHandle);
                            if ((rmlRes<0)||(it==NULL))
                                movementFinished=true; // error
                            else
                            {
                                movementFinished=(rmlRes==1);
                                // Set the current position/orientation:
                                for (int i=0;i<3*4;i++)
                                    currentPosVelAccel[i]=newPosVelAccel[i];
                                C7Vector currentPose;
                                currentPose.X(0)=(float)currentPosVelAccel[0];
                                currentPose.X(1)=(float)currentPosVelAccel[1];
                                currentPose.X(2)=(float)currentPosVelAccel[2];
                                float matrOut[12];
                                float axisPos[3]={0.0f,0.0f,0.0f};
                                simRotateAroundAxis_internal(matrStart,axis,axisPos,(float)currentPosVelAccel[3],matrOut);
                                simGetQuaternionFromMatrix_internal(matrOut,quat);
                                currentPose.Q(0)=quat[3];
                                currentPose.Q(1)=quat[0];
                                currentPose.Q(2)=quat[1];
                                currentPose.Q(3)=quat[2];
                                currentPose=tr*currentPose;
                                it->setAbsoluteTransformation(currentPose);
                                    
                                if (movementFinished)
                                {
                                    CVThreadData* threadData=CThreadPool::getCurrentThreadData();
                                    threadData->usedMovementTime=(float)(((double*)(auxData+1))[0]);
                                    luaWrap_lua_pushnumber(L,1);
                                    pushDoubleTableOntoStack(L,3,newPosVelAccel);
                                    pushFloatTableOntoStack(L,4,quat);
                                    pushDoubleTableOntoStack(L,4,newPosVelAccel+4);
                                    pushDoubleTableOntoStack(L,4,newPosVelAccel+8);
                                    luaWrap_lua_pushnumber(L,dt-((double*)(auxData+1))[0]);
                                    simRMLRemove_internal(rmlHandle);
                                    LUA_END(6);
                                }

                                if (!movementFinished)
                                {
                                    CThreadPool::switchBackToPreviousThread();
                                    if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                                        break;
                                }
                            }
                        }
                        simRMLRemove_internal(rmlHandle);
                    }
                }
            }
        }
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal); // error
    LUA_END(1);
}

int _simRMLMoveToJointPositions(luaWrap_lua_State* L)
{ // can only be called from a script running in a thread!!
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRMLMoveToJointPositions");

    int retVal=-1; //error
    if (!VThread::isCurrentThreadTheMainSimulationThread())
    {
        if ((!CThreadPool::getSimulationStopRequested())&&(isObjectAssociatedWithThisThreadedChildScriptValid(L)))
        {
            if (!( (!luaWrap_lua_istable(L,1))||(luaWrap_lua_objlen(L,1)<1) ))
            { 
                int dofs=luaWrap_lua_objlen(L,1);
                int flags=-1;
                int* jointHandles=new int[dofs];
                double* currentVel=new double[dofs];
                double* currentAccel=new double[dofs];
                double* maxVel=new double[dofs];
                double* maxAccel=new double[dofs];
                double* maxJerk=new double[dofs];
                double* targetPos=new double[dofs];
                double* targetVel=new double[dofs];
                int* direction=new int[dofs];
                for (int i=0;i<dofs;i++)
                {
                    currentVel[i]=0.0;
                    currentAccel[i]=0.0;
                    maxVel[i]=0.0;
                    maxAccel[i]=0.0;
                    maxJerk[i]=0.0;
                    targetPos[i]=0.0;
                    targetVel[i]=0.0;
                    direction[i]=0;
                }
                bool argError=false;

                // jointHandles
                int res=checkOneGeneralInputArgument(L,1,lua_arg_number,dofs,false,false,&errorString);
                if ((!argError)&&(res<2))
                    argError=true;
                else
                {
                    if (res==2)
                        getIntsFromTable(L,1,dofs,jointHandles);
                }
                if (!argError)
                { // check if all joint handles are ok:
                    for (int i=0;i<dofs;i++)
                    {
                        CJoint* act=App::ct->objCont->getJoint(jointHandles[i]);
                        if (act==NULL)
                            argError=true;
                    }
                    if (argError)
                        errorString=SIM_ERROR_FOUND_INVALID_HANDLES;
                }

                // flags
                res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,false,false,&errorString);
                if ((!argError)&&(res<2))
                    argError=true;
                else
                {
                    if (res==2)
                        flags=luaToInt(L,2);
                }

                // currentVel
                res=checkOneGeneralInputArgument(L,3,lua_arg_number,dofs,false,true,&errorString);
                if ((!argError)&&(res<1))
                    argError=true;
                else
                {
                    if (res==2)
                        getDoublesFromTable(L,3,dofs,currentVel);
                }

                // currentAccel
                res=checkOneGeneralInputArgument(L,4,lua_arg_number,dofs,false,true,&errorString);
                if ((!argError)&&(res<1))
                    argError=true;
                else
                {
                    if (res==2)
                        getDoublesFromTable(L,4,dofs,currentAccel);
                }

                // maxVel
                res=checkOneGeneralInputArgument(L,5,lua_arg_number,dofs,false,false,&errorString);
                if ((!argError)&&(res<2))
                    argError=true;
                else
                {
                    if (res==2)
                        getDoublesFromTable(L,5,dofs,maxVel);
                }

                // maxAccel
                res=checkOneGeneralInputArgument(L,6,lua_arg_number,dofs,false,false,&errorString);
                if ((!argError)&&(res<2))
                    argError=true;
                else
                {
                    if (res==2)
                        getDoublesFromTable(L,6,dofs,maxAccel);
                }

                // maxJerk
                res=checkOneGeneralInputArgument(L,7,lua_arg_number,dofs,false,false,&errorString);
                if ((!argError)&&(res<2))
                    argError=true;
                else
                {
                    if (res==2)
                        getDoublesFromTable(L,7,dofs,maxJerk);
                }

                // targetPos
                res=checkOneGeneralInputArgument(L,8,lua_arg_number,dofs,false,false,&errorString);
                if ((!argError)&&(res<2))
                    argError=true;
                else
                {
                    if (res==2)
                        getDoublesFromTable(L,8,dofs,targetPos);
                }

                // targetVel
                res=checkOneGeneralInputArgument(L,9,lua_arg_number,dofs,false,true,&errorString);
                if ((!argError)&&(res<1))
                    argError=true;
                else
                {
                    if (res==2)
                        getDoublesFromTable(L,9,dofs,targetVel);
                }

                res=checkOneGeneralInputArgument(L,10,lua_arg_number,dofs,true,true,&errorString);
                if ((!argError)&&(res<0))
                    argError=true;
                else
                {
                    if (res==2)
                        getIntsFromTable(L,10,dofs,direction);
                }

                if (!argError)
                {
                    unsigned char auxData[9];
                    auxData[0]=1;
                    double* currentPosVelAccel=new double[3*dofs];
                    double* newPosVelAccel=new double[3*dofs];
                    double* maxVelAccelJerk=new double[3*dofs];
                    double* targetPosVel=new double[2*dofs];
                    unsigned char* selection=new unsigned char[dofs];
                    for (int i=0;i<dofs;i++)
                    {
                        CJoint* act=App::ct->objCont->getJoint(jointHandles[i]);
                        if (act!=NULL) // should always pass!
                        {
                            selection[i]=1;
                            float ps=act->getPosition();
                            if ( (act->getJointType()==sim_joint_revolute_subtype)&&act->getPositionIsCyclic() )
                            {
                                if (direction[i]==0)
                                { // smallest movement:
                                    float dx=targetPos[i]-ps;
                                    while (dx>=piValTimes2)
                                    {
                                        ps+=piValTimes2;
                                        dx=targetPos[i]-ps;
                                    }
                                    while (dx<0.0f)
                                    {
                                        ps-=piValTimes2;
                                        dx=targetPos[i]-ps;
                                    }
                                    float b=ps+piValTimes2;
                                    if (fabs(targetPos[i]-b)<fabs(targetPos[i]-ps))
                                        ps=b;
                                }
                                if (direction[i]>0)
                                { // positive direction:
                                    float dx=targetPos[i]-ps;
                                    while (dx<piValTimes2*float(direction[i]-1))
                                    {
                                        ps-=piValTimes2;
                                        dx=targetPos[i]-ps;
                                    }
                                    while (dx>=piValTimes2*float(direction[i]))
                                    {
                                        ps+=piValTimes2;
                                        dx=targetPos[i]-ps;
                                    }
                                }
                                if (direction[i]<0)
                                { // negative direction:
                                    float dx=targetPos[i]-ps;
                                    while (dx>-piValTimes2*float(-direction[i]-1))
                                    {
                                        ps+=piValTimes2;
                                        dx=targetPos[i]-ps;
                                    }
                                    while (dx<=-piValTimes2*float(-direction[i]))
                                    {
                                        ps-=piValTimes2;
                                        dx=targetPos[i]-ps;
                                    }
                                }
                                currentPosVelAccel[0*dofs+i]=ps;
                            }
                            else
                                currentPosVelAccel[0*dofs+i]=ps;
                        }
                        else
                            selection[i]=0;

                        currentPosVelAccel[1*dofs+i]=currentVel[i];
                        currentPosVelAccel[2*dofs+i]=currentAccel[i];

                        maxVelAccelJerk[0*dofs+i]=maxVel[i];
                        maxVelAccelJerk[1*dofs+i]=maxAccel[i];
                        maxVelAccelJerk[2*dofs+i]=maxJerk[i];

                        targetPosVel[0*dofs+i]=targetPos[i];
                        targetPosVel[1*dofs+i]=targetVel[i];

                        selection[i]=1;
                    }

                    bool movementFinished=false;


                    int rmlHandle=simRMLPos_internal(dofs,0.0001,flags,currentPosVelAccel,maxVelAccelJerk,selection,targetPosVel,NULL);
                    while (!movementFinished)
                    {
                        double dt=double(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0;
                        int rmlRes=simRMLStep_internal(rmlHandle,dt,newPosVelAccel,auxData,NULL);
                        if (rmlRes<0)
                            movementFinished=true; // error
                        else
                        {
                            movementFinished=(rmlRes==1);
                            // Set the current positions:
                            for (int i=0;i<3*dofs;i++)
                                currentPosVelAccel[i]=newPosVelAccel[i];
                            for (int i=0;i<dofs;i++)
                            {
                                CJoint* act=App::ct->objCont->getJoint(jointHandles[i]);
                                if ( (act!=NULL)&&(act->getJointType()!=sim_joint_spherical_subtype) )
                                { // might have been destroyed in the mean time
                                    if ( (act->getJointMode()==sim_jointmode_force)&&((act->getCumulativeModelProperty()&sim_modelproperty_not_dynamic)==0) )
                                        act->setDynamicMotorPositionControlTargetPosition((float)currentPosVelAccel[0*dofs+i]);
                                    else
                                        act->setPosition((float)currentPosVelAccel[0*dofs+i]);
                                }
                            }

                            if (movementFinished)
                            {
                                CVThreadData* threadData=CThreadPool::getCurrentThreadData();
                                threadData->usedMovementTime=(float)(((double*)(auxData+1))[0]);
                                luaWrap_lua_pushnumber(L,1);
                                pushDoubleTableOntoStack(L,dofs,newPosVelAccel+0*dofs);
                                pushDoubleTableOntoStack(L,dofs,newPosVelAccel+1*dofs);
                                pushDoubleTableOntoStack(L,dofs,newPosVelAccel+2*dofs);
                                luaWrap_lua_pushnumber(L,dt-((double*)(auxData+1))[0]);

                                delete[] currentPosVelAccel;
                                delete[] newPosVelAccel;
                                delete[] maxVelAccelJerk;
                                delete[] targetPosVel;
                                delete[] selection;

                                delete[] jointHandles;
                                delete[] currentVel;
                                delete[] currentAccel;
                                delete[] maxVel;
                                delete[] maxAccel;
                                delete[] maxJerk;
                                delete[] targetPos;
                                delete[] targetVel;
                                simRMLRemove_internal(rmlHandle);
                                LUA_END(5);
                            }

                            if (!movementFinished)
                            {
                                CThreadPool::switchBackToPreviousThread();
                                if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                                    break;
                            }
                        }
                    }
                    simRMLRemove_internal(rmlHandle);

                    delete[] currentPosVelAccel;
                    delete[] newPosVelAccel;
                    delete[] maxVelAccelJerk;
                    delete[] targetPosVel;
                    delete[] selection;
                }

                delete[] jointHandles;
                delete[] currentVel;
                delete[] currentAccel;
                delete[] maxVel;
                delete[] maxAccel;
                delete[] maxJerk;
                delete[] targetPos;
                delete[] targetVel;
            }
            else
                errorString=SIM_ERROR_INVALID_FIRST_ARGUMENT;
        }
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal); // error
    LUA_END(1);
}

int _simFileDialog(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simFileDialog");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0,lua_arg_string,0,lua_arg_string,0,lua_arg_string,0,lua_arg_string,0))
    {
        int mode=luaToInt(L,1);
        std::string title(luaWrap_lua_tostring(L,2));
        std::string startPath(luaWrap_lua_tostring(L,3));
        std::string initName(luaWrap_lua_tostring(L,4));
        std::string extName(luaWrap_lua_tostring(L,5));
        std::string ext(luaWrap_lua_tostring(L,6));
        char* pathAndName=simFileDialog_internal(mode,title.c_str(),startPath.c_str(),initName.c_str(),extName.c_str(),ext.c_str());
        if (pathAndName!=NULL)
        {
            luaWrap_lua_pushstring(L,pathAndName);
            simReleaseBuffer_internal(pathAndName);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simMsgBox(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simMsgBox");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_string,0,lua_arg_string,0))
    {
        int dlgType=luaToInt(L,1);
        int dlgButtons=luaToInt(L,2);
        std::string title(luaWrap_lua_tostring(L,3));
        std::string message(luaWrap_lua_tostring(L,4));
        retVal=simMsgBox_internal(dlgType,dlgButtons,title.c_str(),message.c_str());
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simLoadModule(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simLoadModule");

    int retVal=-3; // means plugin could not be loaded
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_string,0))
    {
        std::string fileAndPath(luaWrap_lua_tostring(L,1));
        std::string pluginName(luaWrap_lua_tostring(L,2));
        retVal=simLoadModule_internal(fileAndPath.c_str(),pluginName.c_str());
        if (retVal>=0)
            registerNewLuaFunctions(L); // otherwise we can only use the custom Lua functions that the plugin registers after this script has re-initialized!
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simUnloadModule(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simUnloadModule");

    int retVal=0; // error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simUnloadModule_internal(luaWrap_lua_tointeger(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCallScriptFunction(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCallScriptFunction");

    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0))
    {
        std::string funcAndScriptName(luaWrap_lua_tostring(L,1));
        int scriptHandleOrType=luaWrap_lua_tointeger(L,2);

        std::string scriptDescription;
        std::string funcName;
        size_t p=funcAndScriptName.find('@');
        if (p!=std::string::npos)
        {
            scriptDescription.assign(funcAndScriptName.begin()+p+1,funcAndScriptName.end());
            funcName.assign(funcAndScriptName.begin(),funcAndScriptName.begin()+p);
        }
        else
            funcName=funcAndScriptName;

        CLuaScriptObject* script=NULL;
        if (scriptHandleOrType>=SIM_IDSTART_LUASCRIPT)
        { // script is identified by its ID
            script=App::ct->luaScriptContainer->getScriptFromID(scriptHandleOrType);
        }
        else
        { // the script is identified by its type sometimes also by its name
            if (scriptHandleOrType==sim_scripttype_mainscript)
                script=App::ct->luaScriptContainer->getMainScript();
            if (scriptHandleOrType==sim_scripttype_generalcallback)
                script=App::ct->luaScriptContainer->getGeneralCallbackHandlingScript_callback();
            if (scriptHandleOrType==sim_scripttype_contactcallback)
                script=App::ct->luaScriptContainer->getCustomContactHandlingScript_callback();
            if (scriptHandleOrType==sim_scripttype_childscript)
            {
                int objId=App::ct->objCont->getObjectIdentifier(scriptDescription);
                script=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(objId);
            }
            if (scriptHandleOrType==sim_scripttype_jointctrlcallback)
            {
                int objId=App::ct->objCont->getObjectIdentifier(scriptDescription);
                script=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_callback(objId);
            }
            if (scriptHandleOrType==sim_scripttype_customizationscript)
            {
                int objId=App::ct->objCont->getObjectIdentifier(scriptDescription);
                script=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_customization(objId);
            }
        }

        if (script!=NULL)
        {
            CInterfaceStack stack;
            stack.buildFromLuaStack(L,3);

            if (script->getThreadedExecutionIsUnderWay())
            { // very special handling here!
                if (VThread::areThreadIDsSame(script->getThreadedScriptThreadId(),VThread::getCurrentThreadId()))
                {
                    if (script->callScriptFunctionEx(funcName.c_str(),&stack)>=0)
                    {
                        stack.buildOntoLuaStack(L,false);
                        LUA_END(stack.getStackSize());
                    }
                    else
                        errorString=SIM_ERROR_FAILED_CALLING_SCRIPT_FUNCTION;
                }
                else
                { // we have to execute that function via another thread!
                    void* d[4];
                    int callType=1;
                    d[0]=&callType;
                    d[1]=script;
                    d[2]=(void*)funcName.c_str();
                    d[3]=&stack;
                    int retVal=CThreadPool::callRoutineViaSpecificThread(script->getThreadedScriptThreadId(),d);
                    if (retVal>=0)
                    {
                        stack.buildOntoLuaStack(L,false);
                        LUA_END(stack.getStackSize());
                    }
                    else
                        errorString=SIM_ERROR_FAILED_CALLING_SCRIPT_FUNCTION;
                }
            }
            else
            {
                if (VThread::isCurrentThreadTheMainSimulationThread())
                { // For now we don't allow non-main threads to call non-threaded scripts!
                    if (script->callScriptFunctionEx(funcName.c_str(),&stack)>=0)
                    {
                        stack.buildOntoLuaStack(L,false);
                        LUA_END(stack.getStackSize());
                    }
                }
                errorString=SIM_ERROR_FAILED_CALLING_SCRIPT_FUNCTION;
            }
        }
        else
            errorString=SIM_ERROR_SCRIPT_INEXISTANT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetShapeMassAndInertia(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetShapeMassAndInertia");

    int result=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,9,lua_arg_number,3))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        float mass=luaToFloat(L,2);
        float inertiaMatrix[9];
        getFloatsFromTable(L,3,9,inertiaMatrix);
        float centerOfMass[3];
        getFloatsFromTable(L,4,3,centerOfMass);
        float* transf=NULL;
        float transformation[12];
        int res=checkOneGeneralInputArgument(L,5,lua_arg_number,12,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
            {
                getFloatsFromTable(L,5,12,transformation);
                transf=transformation;
            }
            result=simSetShapeMassAndInertia_internal(handle,mass,inertiaMatrix,centerOfMass,transf);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,result);
    LUA_END(1);
}

int _simGetShapeMassAndInertia(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetShapeMassAndInertia");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        float* transf=NULL;
        float transformation[12];
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,12,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
            {
                getFloatsFromTable(L,2,12,transformation);
                transf=transformation;
            }
            float mass;
            float inertiaMatrix[9];
            float centerOfMass[3];
            int result=simGetShapeMassAndInertia_internal(handle,&mass,inertiaMatrix,centerOfMass,transf);
            if (result==-1)
                luaWrap_lua_pushnil(L);
            luaWrap_lua_pushnumber(L,mass);
            pushFloatTableOntoStack(L,9,inertiaMatrix);
            pushFloatTableOntoStack(L,3,centerOfMass);
            LUA_END(3);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGroupShapes(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGroupShapes");

    int retVal=-1; // error
    if (checkInputArguments(L,&errorString,lua_arg_number,1))
    {
        int tableSize=int(luaWrap_lua_objlen(L,1));
        int* theTable=new int[tableSize];
        getIntsFromTable(L,1,tableSize,theTable);
        retVal=simGroupShapes_internal(theTable,tableSize);
        delete[] theTable;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simUngroupShape(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simUngroupShape");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int shapeHandle=luaWrap_lua_tointeger(L,1);
        int count;
        int* handles=simUngroupShape_internal(shapeHandle,&count);
        if (handles!=NULL)
        {
            pushIntTableOntoStack(L,count,handles);
            delete[] handles;
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}


int _simConvexDecompose(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simConvexDecompose");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int shapeHandle=luaWrap_lua_tointeger(L,1);
        int options=luaWrap_lua_tointeger(L,2);
        int intParams[10];
        float floatParams[10];
        bool goOn=true;
        if ((options&4)==0)
        {
            goOn=false;
            int ipc=4;
            int fpc=3;
            if (options&128)
            {
                ipc=10;
                fpc=10;
            }
            int res=checkOneGeneralInputArgument(L,3,lua_arg_number,ipc,false,false,&errorString);
            if (res==2)
            {
                res=checkOneGeneralInputArgument(L,4,lua_arg_number,fpc,false,false,&errorString);
                if (res==2)
                {
                    getIntsFromTable(L,3,ipc,intParams);
                    getFloatsFromTable(L,4,fpc,floatParams);
                    goOn=true;
                }
            }
        }
        intParams[4]=0;
        floatParams[3]=0.0f;
        floatParams[4]=0.0f;
        if (goOn)
            retVal=simConvexDecompose_internal(shapeHandle,options,intParams,floatParams);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simInsertPathCtrlPoints(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simInsertPathCtrlPoints");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int pathHandle=luaToInt(L,1);
        int options=luaToInt(L,2);
        int startIndex=luaToInt(L,3);
        int ptCnt=luaToInt(L,4);
        int floatOrIntCountPerPoint=11;
        if (options&2)
            floatOrIntCountPerPoint=16;

        int res=checkOneGeneralInputArgument(L,5,lua_arg_number,ptCnt*floatOrIntCountPerPoint,false,false,&errorString);
        if (res==2)
        {
            float* data=new float[ptCnt*floatOrIntCountPerPoint];
            getFloatsFromTable(L,5,ptCnt*floatOrIntCountPerPoint,data);
            for (int i=0;i<ptCnt;i++)
                ((int*)(data+floatOrIntCountPerPoint*i+8))[0]=int(data[floatOrIntCountPerPoint*i+8]+0.5f);
            if (options&2)
            {
                for (int i=0;i<ptCnt;i++)
                    ((int*)(data+floatOrIntCountPerPoint*i+11))[0]=int(data[floatOrIntCountPerPoint*i+11]+0.5f);
            }
            retVal=simInsertPathCtrlPoints_internal(pathHandle,options,startIndex,ptCnt,data);
            delete[] data;
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCutPathCtrlPoints(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCutPathCtrlPoints");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int pathHandle=luaToInt(L,1);
        int startIndex=luaToInt(L,2);
        int ptCnt=luaToInt(L,3);
        retVal=simCutPathCtrlPoints_internal(pathHandle,startIndex,ptCnt);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}


int _simGetIkGroupMatrix(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetIkGroupMatrix");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int ikGroupHandle=luaToInt(L,1);
        int options=luaToInt(L,2);
        int matrixSize[2];
        float* data=simGetIkGroupMatrix_internal(ikGroupHandle,options,matrixSize);
        if (data!=NULL)
        {
            pushFloatTableOntoStack(L,matrixSize[0]*matrixSize[1],data);
            pushIntTableOntoStack(L,2,matrixSize);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simAddGhost(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddGhost");

    int retVal=-1; // error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int ghostGroup=luaToInt(L,1);
        int objectHandle=luaToInt(L,2);
        int options=luaToInt(L,3);
        float startTime=luaToFloat(L,4);
        float endTime=luaToFloat(L,5);
        int res=checkOneGeneralInputArgument(L,6,lua_arg_number,12,true,true,&errorString);
        if (res>=0)
        {
            bool defaultColors=true;
            float color[12];
            if (res==2)
            {
                defaultColors=false;
                getFloatsFromTable(L,6,12,color);
            }
            if (defaultColors)
                retVal=simAddGhost_internal(ghostGroup,objectHandle,options,startTime,endTime,NULL);
            else
                retVal=simAddGhost_internal(ghostGroup,objectHandle,options,startTime,endTime,color);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simModifyGhost(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simModifyGhost");

    int retVal=-1; // error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int ghostGroup=luaToInt(L,1);
        int ghostId=luaToInt(L,2);
        int operation=luaToInt(L,3);
        float floatValue=luaToFloat(L,4);

        int options=0;
        int optionsMask=0;
        int res=0;
        if (operation==10)
            res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,false,false,&errorString);
        if (res>=0)
        {
            if ((res==2)&&(operation==10))
            {
                options=luaToInt(L,5);
                res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,false,false,&errorString);
                if (res==2)
                    optionsMask=luaToInt(L,6);
                else
                    res=-1;
            }
            if (res>=0)
            {
                int floatCnt=7;
                if (operation==13)
                    floatCnt=12;
                res=0;
                if ((operation>=11)&&(operation<=13))
                    res=checkOneGeneralInputArgument(L,7,lua_arg_number,floatCnt,false,false,&errorString);
                if (res>=0)
                {
                    float colorOrTransfData[12];
                    if ((res==2)&&(operation>=11)&&(operation<=13))
                        getFloatsFromTable(L,7,floatCnt,colorOrTransfData);
                    retVal=simModifyGhost_internal(ghostGroup,ghostId,operation,floatValue,options,optionsMask,colorOrTransfData);
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simQuitSimulator(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simQuitSimulator");

    if (checkInputArguments(L,&errorString,lua_arg_bool,0))
    {
        bool doNotDisplayMessages=luaToBool(L,1);
        simQuitSimulator_internal(doNotDisplayMessages);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetThreadId(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetThreadId");

    int retVal=simGetThreadId_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetMaterialId(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetMaterialId");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string matName(luaWrap_lua_tostring(L,1));
        retVal=simGetMaterialId_internal(matName.c_str());
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetShapeMaterial(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetShapeMaterial");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int shapeHandle=luaToInt(L,1);
        int materialId=luaToInt(L,2);
        retVal=simSetShapeMaterial_internal(shapeHandle,materialId);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetShapeMaterial(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetShapeMaterial");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int shapeHandle=luaToInt(L,1);
        retVal=simGetShapeMaterial_internal(shapeHandle);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetTextureId(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetTextureId");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string matName(luaWrap_lua_tostring(L,1));
        int resolution[2];
        retVal=simGetTextureId_internal(matName.c_str(),resolution);
        if (retVal>=0)
        {
            luaWrap_lua_pushnumber(L,retVal);
            pushIntTableOntoStack(L,2,resolution);
            LUA_END(2);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simReadTexture(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simReadTexture");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int textureId=luaToInt(L,1);
        int options=luaToInt(L,2);
        int posX=0;
        int posY=0;
        int sizeX=0;
        int sizeY=0;
        // Now check the optional arguments:
        int res;
        res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            if (res==2)
                posX=luaToInt(L,3);
            res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    posY=luaToInt(L,4);
                res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        sizeX=luaToInt(L,5);
                    res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,true,false,&errorString);
                    if ((res==0)||(res==2))
                    {
                        if (res==2)
                            sizeY=luaToInt(L,6);

                        CTextureObject* to=App::ct->textureCont->getObject(textureId);
                        if (to!=NULL)
                        {
                            int tSizeX,tSizeY;
                            to->getTextureSize(tSizeX,tSizeY);
                            if ( (posX>=0)&&(posY>=0)&&(sizeX>=0)&&(sizeY>=0)&&(posX+sizeX<=tSizeX)&&(posY+sizeY<=tSizeY) )
                            {
                                if (sizeX==0)
                                {
                                    posX=0;
                                    sizeX=tSizeX;
                                }
                                if (sizeY==0)
                                {
                                    posY=0;
                                    sizeY=tSizeY;
                                }
                                char* textureData=simReadTexture_internal(textureId,options,posX,posY,sizeX,sizeY);
                                if (textureData!=NULL)
                                { // here we return RGB data in a string
                                    luaWrap_lua_pushlstring(L,(const char*)textureData,sizeX*sizeY*3);
                                    simReleaseBuffer_internal(textureData);
                                    LUA_END(1);
                                }
                            }
                            else
                                errorString=SIM_ERROR_INVALID_ARGUMENTS;
                        }
                        else
                            errorString=SIM_ERROR_TEXTURE_INEXISTANT;
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simWriteTexture(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simWriteTexture");

    int retVal=-1; // error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_string,0))
    {
        int textureId=luaToInt(L,1);
        int options=luaToInt(L,2);
        size_t dataLength;
        char* data=(char*)luaWrap_lua_tolstring(L,3,&dataLength);
        int posX=0;
        int posY=0;
        int sizeX=0;
        int sizeY=0;
        float interpol=0.0f;
        // Now check the optional arguments:
        int res;
        res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,false,&errorString);
        if ((res==0)||(res==2))
        {
            if (res==2)
                posX=luaToInt(L,4);
            res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,false,&errorString);
            if ((res==0)||(res==2))
            {
                if (res==2)
                    posY=luaToInt(L,5);
                res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,true,false,&errorString);
                if ((res==0)||(res==2))
                {
                    if (res==2)
                        sizeX=luaToInt(L,6);
                    res=checkOneGeneralInputArgument(L,7,lua_arg_number,0,true,false,&errorString);
                    if ((res==0)||(res==2))
                    {
                        if (res==2)
                            sizeY=luaToInt(L,7);
                        res=checkOneGeneralInputArgument(L,8,lua_arg_number,0,true,false,&errorString);
                        if ((res==0)||(res==2))
                        {
                            if (res==2)
                                interpol=luaToFloat(L,8);
                            CTextureObject* to=App::ct->textureCont->getObject(textureId);
                            if (to!=NULL)
                            {
                                int tSizeX,tSizeY;
                                to->getTextureSize(tSizeX,tSizeY);
                                if ( (posX>=0)&&(posY>=0)&&(sizeX>=0)&&(sizeY>=0)&&(posX+sizeX<=tSizeX)&&(posY+sizeY<=tSizeY)&&(int(dataLength)>=sizeX*sizeY*3) )
                                    retVal=simWriteTexture_internal(textureId,options,data,posX,posY,sizeX,sizeY,interpol);
                                else
                                    errorString=SIM_ERROR_INVALID_ARGUMENTS;
                            }
                            else
                                errorString=SIM_ERROR_TEXTURE_INEXISTANT;
                        }
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreateTexture(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateTexture");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0))
    {
        std::string fileName(luaWrap_lua_tostring(L,1));
        int options=luaToInt(L,2);
        float* planeSizesP=NULL;
        float planeSizes[2];
        float* scalingUVP=NULL;
        float scalingUV[2];
        float* xy_gP=NULL;
        float xy_g[3];
        int resolution[2]={0,0}; // means: just any!

        // Now check the optional arguments:
        int res;
        res=checkOneGeneralInputArgument(L,3,lua_arg_number,2,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
            {
                getFloatsFromTable(L,3,2,planeSizes);
                planeSizesP=planeSizes;
            }

            res=checkOneGeneralInputArgument(L,4,lua_arg_number,2,true,true,&errorString);
            if (res>=0)
            {
                if (res==2)
                {
                    getFloatsFromTable(L,4,2,scalingUV);
                    scalingUVP=scalingUV;
                }

                res=checkOneGeneralInputArgument(L,5,lua_arg_number,3,true,true,&errorString);
                if (res>=0)
                {
                    if (res==2)
                    {
                        getFloatsFromTable(L,5,3,xy_g);
                        xy_gP=xy_g;
                    }

                    res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,true,true,&errorString);
                    if (res>=0)
                    {
                        int maxTextureSize=0; // just the original
                        if (res==2)
                            maxTextureSize=luaToInt(L,6);

                        res=checkOneGeneralInputArgument(L,7,lua_arg_number,2,fileName.length()!=0,fileName.length()!=0,&errorString);
                        if (res>=0)
                        {
                            if (res==2)
                                getIntsFromTable(L,7,2,resolution);

                            if (fileName.length()==0)
                            { // we are not loading a texture, but creating it!
                                resolution[0]=tt::getLimitedInt(1,4096,resolution[0]);
                                resolution[1]=tt::getLimitedInt(1,4096,resolution[1]);
                            }

                            int textureId;
                            int shapeHandle=simCreateTexture_internal(fileName.c_str(),options,planeSizesP,scalingUVP,xy_gP,maxTextureSize,&textureId,resolution,NULL);
                            if (shapeHandle>=0)
                            {
                                luaWrap_lua_pushnumber(L,shapeHandle);
                                luaWrap_lua_pushnumber(L,textureId);
                                pushIntTableOntoStack(L,2,resolution);
                                LUA_END(3);
                            }
                        }
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal); // error
    LUA_END(1);
}


int _simWriteCustomDataBlock(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simWriteCustomDataBlock");

    int retVal=-1; // error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0))
    {
        int objectHandle=luaToInt(L,1);
        if (objectHandle==sim_handle_self)
            objectHandle=getCurrentScriptID(L);

        std::string dataName(luaWrap_lua_tostring(L,2));
        int res;
        res=checkOneGeneralInputArgument(L,3,lua_arg_string,0,false,true,&errorString);
        if (res>=1)
        {
            size_t dataLength=0;
            char* data=NULL;
            if (res==2)
                data=(char*)luaWrap_lua_tolstring(L,3,&dataLength);
            retVal=simWriteCustomDataBlock_internal(objectHandle,dataName.c_str(),data,dataLength);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}


int _simReadCustomDataBlock(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simReadCustomDataBlock");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0))
    {
        int objectHandle=luaToInt(L,1);
        if (objectHandle==sim_handle_self)
            objectHandle=getCurrentScriptID(L);

        std::string dataName(luaWrap_lua_tostring(L,2));
        int dataLength;
        char* data=simReadCustomDataBlock_internal(objectHandle,dataName.c_str(),&dataLength);
        if (data!=NULL)
        {
            luaWrap_lua_pushlstring(L,(const char*)data,dataLength);
            simReleaseBuffer_internal(data);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simReadCustomDataBlockTags(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simReadCustomDataBlockTags");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int objectHandle=luaToInt(L,1);
        if (objectHandle==sim_handle_self)
            objectHandle=getCurrentScriptID(L);

        int tagCount;
        char* data=simReadCustomDataBlockTags_internal(objectHandle,&tagCount);
        if (data!=NULL)
        {
            std::vector<std::string> stringTable;
            size_t off=0;
            for (int i=0;i<tagCount;i++)
            {
                stringTable.push_back(data+off);
                off+=strlen(data+off)+1;
            }
            pushStringTableOntoStack(L,stringTable);
            simReleaseBuffer_internal(data);

            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simAddPointCloud(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddPointCloud");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,3))
    {
        int pageMask=luaToInt(L,1);
        int layerMask=luaToInt(L,2);
        int objectHandle=luaToInt(L,3);
        int options=luaToInt(L,4);
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        if ( (it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript)||(it->getScriptType()==sim_scripttype_jointctrlcallback)||(it->getScriptType()==sim_scripttype_contactcallback) )//||(it->getScriptType()==sim_scripttype_generalcallback) )
            options=(options|1)-1; // cloud is automatically removed at the end of the simulation (i.e. is not persistent)
        float pointSize=luaToFloat(L,5);
        int pointCnt=luaWrap_lua_objlen(L,6)/3;
        std::vector<float> pointCoordinates(pointCnt*3,0.0f);
        getFloatsFromTable(L,6,pointCnt*3,&pointCoordinates[0]);
        int res;
        res=checkOneGeneralInputArgument(L,7,lua_arg_number,12,true,true,&errorString);
        if (res>=0)
        {
            unsigned char* defaultColors=NULL;
            std::vector<unsigned char> _defCols(12,0);
            if (res==2)
            {
                std::vector<int> _defCols_(12,0);
                getIntsFromTable(L,7,12,&_defCols_[0]);
                for (int i=0;i<12;i++)
                    _defCols[i]=(unsigned char)_defCols_[i];
                defaultColors=&_defCols[0];
            }
            res=checkOneGeneralInputArgument(L,8,lua_arg_number,pointCnt*3,true,true,&errorString);
            if (res>=0)
            {
                unsigned char* pointColors=NULL;
                std::vector<unsigned char> _pointCols;
                if (res==2)
                {
                    _pointCols.resize(pointCnt*3,0);
                    std::vector<int> _pointCols_(pointCnt*3,0);
                    getIntsFromTable(L,8,pointCnt*3,&_pointCols_[0]);
                    for (int i=0;i<pointCnt*3;i++)
                        _pointCols[i]=(unsigned char)_pointCols_[i];
                    pointColors=&_pointCols[0];
                }
                res=checkOneGeneralInputArgument(L,9,lua_arg_number,pointCnt*3,true,true,&errorString);
                if (res>=0)
                {
                    float* pointNormals=NULL;
                    std::vector<float> _pointNormals;
                    if (res==2)
                    {
                        _pointNormals.resize(pointCnt*3,0);
                        getFloatsFromTable(L,9,pointCnt*3,&_pointNormals[0]);
                        pointNormals=&_pointNormals[0];
                    }
                    retVal=simAddPointCloud_internal(pageMask,layerMask,objectHandle,options,pointSize,pointCoordinates.size()/3,&pointCoordinates[0],(char*)defaultColors,(char*)pointColors,pointNormals);
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simModifyPointCloud(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simModifyPointCloud");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int pointCloudHandle=luaToInt(L,1);
        int operation=luaToInt(L,2);
        retVal=simModifyPointCloud_internal(pointCloudHandle,operation,NULL,NULL);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetShapeGeomInfo(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetShapeGeomInfo");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaToInt(L,1);
        int intData[5];
        float floatData[5];
        retVal=simGetShapeGeomInfo_internal(handle,intData,floatData,NULL);
        if (retVal>=0)
        {
            luaWrap_lua_pushnumber(L,retVal);
            luaWrap_lua_pushnumber(L,intData[0]);
            pushFloatTableOntoStack(L,4,floatData);
            LUA_END(3);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectsInTree(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectsInTree");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaToInt(L,1);
        int objType=sim_handle_all;
        int options=0;
        int res=checkOneGeneralInputArgument(L,2,lua_arg_number,0,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
                objType=luaToInt(L,2);
            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,true,&errorString);
            if (res>=0)
            {
                if (res==2)
                    options=luaToInt(L,3);
                int objCnt=0;
                int* objHandles=simGetObjectsInTree_internal(handle,objType,options,&objCnt);
                if (objHandles!=NULL)
                {
                    pushIntTableOntoStack(L,objCnt,objHandles);
                    simReleaseBuffer_internal((char*)objHandles);
                    LUA_END(1);
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetObjectSizeValues(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetObjectSizeValues");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,3))
    {
        int handle=luaToInt(L,1);
        float s[3];
        getFloatsFromTable(L,2,3,s);
        retVal=simSetObjectSizeValues_internal(handle,s);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetObjectSizeValues(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectSizeValues");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaToInt(L,1);
        float s[3];
        if (simGetObjectSizeValues_internal(handle,s)!=-1)
        {
            pushFloatTableOntoStack(L,3,s);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simScaleObject(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simScaleObject");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int handle=luaToInt(L,1);
        float x=luaToFloat(L,2);
        float y=luaToFloat(L,3);
        float z=luaToFloat(L,4);
        int options=0;
        int res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
                options=luaToInt(L,5);
            retVal=simScaleObject_internal(handle,x,y,z,options);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetShapeTexture(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetShapeTexture");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,2))
    {
        int handle=luaToInt(L,1);
        int textureId=luaToInt(L,2);
        int mapMode=luaToInt(L,3);
        int options=luaToInt(L,4);
        float uvScaling[2];
        getFloatsFromTable(L,5,2,uvScaling);
        float* posP=NULL;
        float* orP=NULL;
        float _pos[3];
        float _or[3];
        int res=checkOneGeneralInputArgument(L,6,lua_arg_number,3,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
            {
                getFloatsFromTable(L,6,3,_pos);
                posP=_pos;
            }
            res=checkOneGeneralInputArgument(L,7,lua_arg_number,3,true,true,&errorString);
            if (res>=0)
            {
                if (res==2)
                {
                    getFloatsFromTable(L,7,3,_or);
                    orP=_or;
                }
                retVal=simSetShapeTexture_internal(handle,textureId,mapMode,options,uvScaling,posP,orP);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetShapeTextureId(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetShapeTextureId");
    int retVal=-1;

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaToInt(L,1);
        retVal=simGetShapeTextureId_internal(handle);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetCollectionObjects(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetCollectionObjects");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaToInt(L,1);
        int cnt;
        int* objHandles=simGetCollectionObjects_internal(handle,&cnt);
        pushIntTableOntoStack(L,cnt,objHandles);
        delete[] objHandles;
        LUA_END(1);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simHandleCustomizationScripts(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleCustomizationScripts");

    int retVal=-1;
    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if (itScrObj->getScriptType()==sim_scripttype_mainscript)
    {
        if (checkInputArguments(L,&errorString,lua_arg_number,0))
        {
            int callType=luaToInt(L,1);
            retVal=simHandleCustomizationScripts_internal(callType);
        }
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetScriptAttribute(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetScriptAttribute");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int scriptID=luaToInt(L,1);
        if (scriptID==sim_handle_self)
            scriptID=getCurrentScriptID(L);
        int attribID=luaToInt(L,2);
        int thirdArgType=lua_arg_number;
        if ( (attribID==sim_customizationscriptattribute_activeduringsimulation)||(attribID==sim_childscriptattribute_automaticcascadingcalls)||(attribID==sim_childscriptattribute_enabled)||(attribID==sim_customizationscriptattribute_cleanupbeforesave) )
            thirdArgType=lua_arg_bool;


        if ( (attribID==sim_scriptattribute_executionorder)||(attribID==sim_scriptattribute_executioncount) )
            thirdArgType=lua_arg_number;
        int res=checkOneGeneralInputArgument(L,3,thirdArgType,0,false,false,&errorString);
        if (res==2)
        {
            if ( (attribID==sim_customizationscriptattribute_activeduringsimulation)||(attribID==sim_childscriptattribute_automaticcascadingcalls)||(attribID==sim_childscriptattribute_enabled)||(attribID==sim_customizationscriptattribute_cleanupbeforesave) )
                retVal=simSetScriptAttribute_internal(scriptID,attribID,0.0f,luaToBool(L,3));
            if ( (attribID==sim_scriptattribute_executionorder)||(attribID==sim_scriptattribute_executioncount) )
                retVal=simSetScriptAttribute_internal(scriptID,attribID,0.0f,luaToInt(L,3));
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetScriptAttribute(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetScriptAttribute");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int scriptID=luaToInt(L,1);
        if (scriptID==sim_handle_self)
            scriptID=getCurrentScriptID(L);
        int attribID=luaToInt(L,2);
        int intVal;
        float floatVal;
        int result=simGetScriptAttribute_internal(scriptID,attribID,&floatVal,&intVal);
        if (result!=-1)
        {
            if ( (attribID==sim_customizationscriptattribute_activeduringsimulation)||(attribID==sim_childscriptattribute_automaticcascadingcalls)||(attribID==sim_childscriptattribute_enabled)||(attribID==sim_customizationscriptattribute_cleanupbeforesave) )
                luaWrap_lua_pushboolean(L,intVal);
            if ( (attribID==sim_scriptattribute_executionorder)||(attribID==sim_scriptattribute_executioncount) )
                luaWrap_lua_pushnumber(L,intVal);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}


int _simReorientShapeBoundingBox(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simReorientShapeBoundingBox");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int shapeHandle=luaToInt(L,1);
        int relativeToHandle=luaToInt(L,2);
        retVal=simReorientShapeBoundingBox_internal(shapeHandle,relativeToHandle,0);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetScriptVariable(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetScriptVariable");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0))
    {
        std::string varAndScriptName(luaWrap_lua_tostring(L,1));
        int scriptHandleOrType=luaWrap_lua_tointeger(L,2);
        int numberOfArguments=luaWrap_lua_gettop(L);
        if (numberOfArguments>=3)
        {
            CInterfaceStack* stack=new CInterfaceStack();
            int stackHandle=App::ct->interfaceStackContainer->addStack(stack);
            stack->buildFromLuaStack(L,3,1);
            retVal=simSetScriptVariable_internal(scriptHandleOrType,varAndScriptName.c_str(),stackHandle);
            App::ct->interfaceStackContainer->destroyStack(stackHandle);
        }
        else
            errorString=SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetEngineFloatParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetEngineFloatParameter");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int paramId=luaWrap_lua_tointeger(L,1);
        int objectHandle=luaWrap_lua_tointeger(L,2);
        simBool ok;
        float paramVal=simGetEngineFloatParameter_internal(paramId,objectHandle,NULL,&ok);
        if (ok>0)
        {
            luaWrap_lua_pushnumber(L,paramVal);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetEngineInt32Parameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetEngineInt32Parameter");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int paramId=luaWrap_lua_tointeger(L,1);
        int objectHandle=luaWrap_lua_tointeger(L,2);
        simBool ok;
        int paramVal=simGetEngineInt32Parameter_internal(paramId,objectHandle,NULL,&ok);
        if (ok>0)
        {
            luaWrap_lua_pushnumber(L,paramVal);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetEngineBoolParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetEngineBoolParameter");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int paramId=luaWrap_lua_tointeger(L,1);
        int objectHandle=luaWrap_lua_tointeger(L,2);
        simBool ok;
        simBool paramVal=simGetEngineBoolParameter_internal(paramId,objectHandle,NULL,&ok);
        if (ok>0)
        {
            luaWrap_lua_pushboolean(L,paramVal>0);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetEngineFloatParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetEngineFloatParameter");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int paramId=luaWrap_lua_tointeger(L,1);
        int objectHandle=luaWrap_lua_tointeger(L,2);
        float paramVal=luaToFloat(L,3);
        retVal=simSetEngineFloatParameter_internal(paramId,objectHandle,NULL,paramVal);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetEngineInt32Parameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetEngineInt32Parameter");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int paramId=luaWrap_lua_tointeger(L,1);
        int objectHandle=luaWrap_lua_tointeger(L,2);
        int paramVal=luaWrap_lua_tointeger(L,3);
        retVal=simSetEngineInt32Parameter_internal(paramId,objectHandle,NULL,paramVal);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetEngineBoolParameter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetEngineBoolParameter");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_bool,0))
    {
        int paramId=luaWrap_lua_tointeger(L,1);
        int objectHandle=luaWrap_lua_tointeger(L,2);
        simBool paramVal=(simBool)luaWrap_lua_toboolean(L,3);
        retVal=simSetEngineBoolParameter_internal(paramId,objectHandle,NULL,paramVal);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreateOctree(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateOctree");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        float voxelSize=(float)luaWrap_lua_tonumber(L,1);
        int options=luaWrap_lua_tointeger(L,2);
        float pointSize=(float)luaWrap_lua_tonumber(L,3);
        retVal=simCreateOctree_internal(voxelSize,options,pointSize,NULL);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreatePointCloud(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreatePointCloud");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        float maxVoxelSize=(float)luaWrap_lua_tonumber(L,1);
        int maxPtCntPerVoxel=luaWrap_lua_tointeger(L,2);
        int options=luaWrap_lua_tointeger(L,3);
        float pointSize=(float)luaWrap_lua_tonumber(L,4);
        retVal=simCreatePointCloud_internal(maxVoxelSize,maxPtCntPerVoxel,options,pointSize,NULL);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetPointCloudOptions(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetPointCloudOptions");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        float maxVoxelSize=(float)luaWrap_lua_tonumber(L,2);
        int maxPtCntPerVoxel=luaWrap_lua_tointeger(L,3);
        int options=luaWrap_lua_tointeger(L,4);
        float pointSize=(float)luaWrap_lua_tonumber(L,5);
        retVal=simSetPointCloudOptions_internal(handle,maxVoxelSize,maxPtCntPerVoxel,options,pointSize,NULL);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetPointCloudOptions(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetPointCloudOptions");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        float maxVoxelSize;
        int maxPtCntPerVoxel;
        int options;
        float pointSize;
        int retVal=simGetPointCloudOptions_internal(handle,&maxVoxelSize,&maxPtCntPerVoxel,&options,&pointSize,NULL);
        if (retVal>0)
        {
            luaWrap_lua_pushnumber(L,maxVoxelSize);
            luaWrap_lua_pushnumber(L,maxPtCntPerVoxel);
            luaWrap_lua_pushnumber(L,options);
            luaWrap_lua_pushnumber(L,pointSize);
            LUA_END(4);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simInsertVoxelsIntoOctree(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simInsertVoxelsIntoOctree");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,3))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        int options=luaWrap_lua_tointeger(L,2);
        int ptCnt=int(luaWrap_lua_objlen(L,3))/3;
        std::vector<float> pts;
        pts.resize(ptCnt*3);
        unsigned char* cols=NULL;
        std::vector<unsigned char> _cols;
        int v=3;
        if (options&2)
            v=ptCnt*3;
        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,v,true,true,&errorString);
        if (res>=0)
        {
            getFloatsFromTable(L,3,ptCnt*3,&pts[0]);
            if (res==2)
            {
                _cols.resize(v);
                getUCharsFromTable(L,4,v,&_cols[0]);
                cols=&_cols[0];
            }
            res=checkOneGeneralInputArgument(L,5,lua_arg_number,v/3,true,true,&errorString);
            if (res>=0)
            {
                if (cols==NULL)
                    retVal=simInsertVoxelsIntoOctree_internal(handle,options,&pts[0],ptCnt,NULL,NULL,NULL);
                else
                {
                    unsigned int* tags=NULL;
                    std::vector<unsigned int> _tags;
                    if (res==2)
                    {
                        _tags.resize(v/3);
                        getUIntsFromTable(L,5,v/3,&_tags[0]);
                        tags=&_tags[0];
                    }
                    retVal=simInsertVoxelsIntoOctree_internal(handle,options,&pts[0],ptCnt,cols,tags,NULL);
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRemoveVoxelsFromOctree(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRemoveVoxelsFromOctree");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        int options=luaWrap_lua_tointeger(L,2);
        int res=checkOneGeneralInputArgument(L,3,lua_arg_number,3,false,true,&errorString);
        if (res>=1)
        {
            if (res==2)
            { // remove some voxels
                int ptCnt=int(luaWrap_lua_objlen(L,3))/3;
                std::vector<float> pts;
                pts.resize(ptCnt*3);
                getFloatsFromTable(L,3,ptCnt*3,&pts[0]);
                retVal=simRemoveVoxelsFromOctree_internal(handle,options,&pts[0],ptCnt,NULL);
            }
            else
                retVal=simRemoveVoxelsFromOctree_internal(handle,options,NULL,0,NULL); // remove all voxels!
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simInsertPointsIntoPointCloud(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simInsertPointsIntoPointCloud");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,3))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        int options=luaWrap_lua_tointeger(L,2);
        int ptCnt=int(luaWrap_lua_objlen(L,3))/3;
        float optionalValues[2];
        ((int*)optionalValues)[0]=1; // duplicate tolerance bit
        optionalValues[1]=0.0; // duplicate tolerance
        std::vector<float> pts;
        pts.resize(ptCnt*3);
        unsigned char* cols=NULL;
        std::vector<unsigned char> _cols;
        int v=3;
        if (options&2)
            v=ptCnt*3;
        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,v,true,true,&errorString);
        if (res>=0)
        {
            getFloatsFromTable(L,3,ptCnt*3,&pts[0]);
            if (res==2)
            {
                _cols.resize(v);
                getUCharsFromTable(L,4,v,&_cols[0]);
                cols=&_cols[0];
            }
            res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,true,&errorString);
            if (res>=0)
            {
                if (res==2)
                {
                    optionalValues[1]=(float)luaWrap_lua_tonumber(L,5); // duplicate tolerance
                    retVal=simInsertPointsIntoPointCloud_internal(handle,options,&pts[0],ptCnt,cols,optionalValues);
                }
                else
                    retVal=simInsertPointsIntoPointCloud_internal(handle,options,&pts[0],ptCnt,cols,NULL);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRemovePointsFromPointCloud(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRemovePointsFromPointCloud");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        int options=luaWrap_lua_tointeger(L,2);
        int res=checkOneGeneralInputArgument(L,3,lua_arg_number,3,false,true,&errorString);
        if (res>=1)
        {
            int res2=checkOneGeneralInputArgument(L,4,lua_arg_number,0,false,false,&errorString);
            if (res2==2)
            {
                float tolerance=(float)luaWrap_lua_tonumber(L,4);
                if (res==2)
                { // remove some points
                    int ptCnt=int(luaWrap_lua_objlen(L,3))/3;
                    std::vector<float> pts;
                    pts.resize(ptCnt*3);
                    getFloatsFromTable(L,3,ptCnt*3,&pts[0]);
                    retVal=simRemovePointsFromPointCloud_internal(handle,options,&pts[0],ptCnt,tolerance,NULL);
                }
                else
                    retVal=simRemovePointsFromPointCloud_internal(handle,options,NULL,0,0.0,NULL); // remove all points
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simIntersectPointsWithPointCloud(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simIntersectPointsWithPointCloud");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        int options=luaWrap_lua_tointeger(L,2);
        int res=checkOneGeneralInputArgument(L,3,lua_arg_number,3,false,true,&errorString);
        if (res>=1)
        {
            int res2=checkOneGeneralInputArgument(L,4,lua_arg_number,0,false,false,&errorString);
            if (res2==2)
            {
                float tolerance=(float)luaWrap_lua_tonumber(L,4);
                if (res==2)
                { // intersect some points
                    int ptCnt=int(luaWrap_lua_objlen(L,3))/3;
                    std::vector<float> pts;
                    pts.resize(ptCnt*3);
                    getFloatsFromTable(L,3,ptCnt*3,&pts[0]);
                    retVal=simIntersectPointsWithPointCloud_internal(handle,options,&pts[0],ptCnt,tolerance,NULL);
                }
                else
                    retVal=simRemovePointsFromPointCloud_internal(handle,options,NULL,0,0.0,NULL); // remove all points
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetOctreeVoxels(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetOctreeVoxels");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        int ptCnt=-1;
        const float* p=simGetOctreeVoxels_internal(handle,&ptCnt,NULL);
        if (ptCnt>=0)
        {
            pushFloatTableOntoStack(L,ptCnt*3,p);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetPointCloudPoints(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetPointCloudPoints");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        int ptCnt=-1;
        const float* p=simGetPointCloudPoints_internal(handle,&ptCnt,NULL);
        if (ptCnt>=0)
        {
            pushFloatTableOntoStack(L,ptCnt*3,p);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simInsertObjectIntoOctree(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simInsertObjectIntoOctree");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int handle1=luaWrap_lua_tointeger(L,1);
        int handle2=luaWrap_lua_tointeger(L,2);
        int options=luaWrap_lua_tointeger(L,3);
        unsigned char col[3];
        unsigned char* c=NULL;
        int tag=0;
        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,3,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
            {
                getUCharsFromTable(L,4,3,col);
                c=col;
            }
            res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,true,&errorString);
            if (res==2)
                tag=(unsigned int)luaWrap_lua_tonumber(L,5);
            retVal=simInsertObjectIntoOctree_internal(handle1,handle2,options,c,tag,NULL);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSubtractObjectFromOctree(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSubtractObjectFromOctree");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int handle1=luaWrap_lua_tointeger(L,1);
        int handle2=luaWrap_lua_tointeger(L,2);
        int options=luaWrap_lua_tointeger(L,3);
        retVal=simSubtractObjectFromOctree_internal(handle1,handle2,options,NULL);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simInsertObjectIntoPointCloud(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simInsertObjectIntoPointCloud");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int handle1=luaWrap_lua_tointeger(L,1);
        int handle2=luaWrap_lua_tointeger(L,2);
        int options=luaWrap_lua_tointeger(L,3);
        float gridSize=(float)luaWrap_lua_tonumber(L,4);
        float optionalValues[2];
        ((int*)optionalValues)[0]=1; // duplicate tolerance bit
        optionalValues[1]=0.0; // duplicate tolerance
        unsigned char col[3];
        unsigned char* c=NULL;
        int res=checkOneGeneralInputArgument(L,5,lua_arg_number,3,true,true,&errorString);
        if (res>=0)
        {
            if (res==2)
            {
                getUCharsFromTable(L,5,3,col);
                c=col;
            }
            res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,true,true,&errorString);
            if (res>=0)
            {
                if (res==2)
                {
                    optionalValues[1]=(float)luaWrap_lua_tonumber(L,6); // duplicate tolerance
                    retVal=simInsertObjectIntoPointCloud_internal(handle1,handle2,options,gridSize,c,optionalValues);
                }
                else
                    retVal=simInsertObjectIntoPointCloud_internal(handle1,handle2,options,gridSize,c,NULL);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSubtractObjectFromPointCloud(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSubtractObjectFromPointCloud");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int handle1=luaWrap_lua_tointeger(L,1);
        int handle2=luaWrap_lua_tointeger(L,2);
        int options=luaWrap_lua_tointeger(L,3);
        float tolerance=luaWrap_lua_tonumber(L,4);
        retVal=simSubtractObjectFromPointCloud_internal(handle1,handle2,options,tolerance,NULL);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCheckOctreePointOccupancy(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCheckOctreePointOccupancy");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,3))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        int options=luaWrap_lua_tointeger(L,2);
        int ptCnt=int(luaWrap_lua_objlen(L,3))/3;
        std::vector<float> points;
        points.resize(ptCnt*3);
        getFloatsFromTable(L,3,ptCnt*3,&points[0]);
        unsigned int tag=0;
        unsigned long long int location=0;
        retVal=simCheckOctreePointOccupancy_internal(handle,options,&points[0],ptCnt,&tag,&location,NULL);
        if ( (retVal>0)&&(ptCnt==1) )
        { // in this case we return 4 values:
            unsigned int locLow=location&0xffffffff;
            unsigned int locHigh=(location>>32)&0xffffffff;
            luaWrap_lua_pushnumber(L,retVal);
            luaWrap_lua_pushnumber(L,tag);
            luaWrap_lua_pushnumber(L,locLow);
            luaWrap_lua_pushnumber(L,locHigh);
            LUA_END(4);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}


int _simOpenTextEditor(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simOpenTextEditor");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string initText(luaWrap_lua_tostring(L,1));

        int res=checkOneGeneralInputArgument(L,2,lua_arg_string,0,true,true,&errorString);
        if (res>=0)
        {
            std::string xml;
            const char* _xml=NULL;
            if (res==2)
            {
                xml=luaWrap_lua_tostring(L,2);
                _xml=xml.c_str();
            }
            int various[5];
            various[0]=4;
            char* outText=simOpenTextEditor_internal(initText.c_str(),_xml,various);
            if (outText!=NULL)
            {
                luaWrap_lua_pushstring(L,outText);
                delete[] outText;
                pushIntTableOntoStack(L,2,various+1);
                pushIntTableOntoStack(L,2,various+3);
                LUA_END(3);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simPackTable(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simPackTable");

    if (luaWrap_lua_gettop(L)>=1)
    {
        if (luaWrap_lua_istable(L,1))
        {
            CInterfaceStack stack;
            stack.buildFromLuaStack(L,1,1);
            std::string s(stack.getBufferFromTable());
            luaWrap_lua_pushlstring(L,s.c_str(),s.length());
            LUA_END(1);
        }
        else
            errorString.assign(SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG);
    }
    else
        errorString.assign(SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS);

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simUnpackTable(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simUnpackTable");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        size_t l;
        const char* s=luaWrap_lua_tolstring(L,1,&l);
        CInterfaceStack stack;
        if (stack.pushTableFromBuffer(s,l))
        {
            stack.buildOntoLuaStack(L,true);
            LUA_END(1);
        }
        errorString.assign(SIM_ERROR_INVALID_DATA);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetVisionSensorFilter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetVisionSensorFilter");
    int retVal=-1; // error

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,-1,lua_arg_number,-1,lua_arg_number,-1,lua_arg_string,0))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        int index=luaWrap_lua_tointeger(L,2);
        int options=luaWrap_lua_tointeger(L,3);
        int byteS=int(luaWrap_lua_objlen(L,4));
        int intS=int(luaWrap_lua_objlen(L,5));
        int floatS=int(luaWrap_lua_objlen(L,6));
        unsigned char* byteP=new unsigned char[byteS];
        int* intP=new int[intS];
        float* floatP=new float[floatS];
        getUCharsFromTable(L,4,byteS,byteP);
        getIntsFromTable(L,5,intS,intP);
        getFloatsFromTable(L,6,floatS,floatP);
        size_t custS;
        const char* cust=luaWrap_lua_tolstring(L,7,&custS);
        int sizes[4]={byteS,intS,floatS,int(custS)};
        retVal=simSetVisionSensorFilter_internal(handle,index,options,sizes,byteP,intP,floatP,(unsigned char*)cust);
        delete[] byteP;
        delete[] intP;
        delete[] floatP;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetVisionSensorFilter(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetVisionSensorFilter");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int handle=luaWrap_lua_tointeger(L,1);
        int index=luaWrap_lua_tointeger(L,2);
        int options=0;
        int sizes[4]={0,0,0,0};
        unsigned char* bytes;
        int* ints;
        float* floats;
        unsigned char* custom;
        int filterT=simGetVisionSensorFilter_internal(handle,index,&options,sizes,&bytes,&ints,&floats,&custom);
        luaWrap_lua_pushnumber(L,filterT);
        if (filterT>0)
        {
            luaWrap_lua_pushnumber(L,options);
            pushUCharTableOntoStack(L,sizes[0],bytes);
            pushIntTableOntoStack(L,sizes[1],ints);
            pushFloatTableOntoStack(L,sizes[2],floats);
            luaWrap_lua_pushlstring(L,(char*)custom,sizes[3]);
            delete[] bytes;
            delete[] ints;
            delete[] floats;
            delete[] custom;
            LUA_END(6);
        }
        LUA_END(1);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,-1);
    LUA_END(1);
}

int _simHandleSimulationStart(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleSimulationStart");

    int retVal=-1;
    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if (itScrObj->getScriptType()==sim_scripttype_mainscript)
    {
        // Following is for velocity measurement (initial):
        float dt=float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f;
        for (size_t i=0;i<App::ct->objCont->jointList.size();i++)
            App::ct->objCont->getJoint(App::ct->objCont->jointList[i])->measureJointVelocity(dt);
        for (size_t i=0;i<App::ct->objCont->objectList.size();i++)
            App::ct->objCont->getObject(App::ct->objCont->objectList[i])->measureVelocity(dt);
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simHandleSensingStart(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleSensingStart");

    int retVal=-1;
    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if (itScrObj->getScriptType()==sim_scripttype_mainscript)
    {
        // Following is for camera tracking!
        for (size_t i=0;i<App::ct->objCont->cameraList.size();i++)
        {
            CCamera*  it=App::ct->objCont->getCamera(App::ct->objCont->cameraList[i]);
            it->handleTrackingAndHeadAlwaysUp();
        }

        // Following is for velocity measurement:
        float dt=float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f;
        for (size_t i=0;i<App::ct->objCont->jointList.size();i++)
            App::ct->objCont->getJoint(App::ct->objCont->jointList[i])->measureJointVelocity(dt);
        for (size_t i=0;i<App::ct->objCont->objectList.size();i++)
            App::ct->objCont->getObject(App::ct->objCont->objectList[i])->measureVelocity(dt);
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simAuxFunc(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAuxFunc");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string cmd(luaWrap_lua_tostring(L,1));
        if (cmd.compare("activateMainWindow")==0)
        {
#ifndef SIM_WITHOUT_QT_AT_ALL
            if (App::mainWindow!=NULL)
                App::mainWindow->activateMainWindow();
#endif
            LUA_END(0);
        }
        if (cmd.compare("curveToClipboard")==0)
        {
            if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0,lua_arg_number,0,lua_arg_string,0))
            {
                int graphHandle=luaWrap_lua_tointeger(L,2);
                int curveType=luaWrap_lua_tointeger(L,3);
                std::string curveName(luaWrap_lua_tostring(L,4));
                CGraph* it=App::ct->objCont->getGraph(graphHandle);
                if (it!=NULL)
                {
                    it->curveToClipboard(curveType,curveName);
                    LUA_END(0);
                }
                else
                    errorString=SIM_ERROR_INVALID_HANDLE;
            }
        }
        if (cmd.compare("curveToStatic")==0)
        {
            if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0,lua_arg_number,0,lua_arg_string,0))
            {
                int graphHandle=luaWrap_lua_tointeger(L,2);
                int curveType=luaWrap_lua_tointeger(L,3);
                std::string curveName(luaWrap_lua_tostring(L,4));
                CGraph* it=App::ct->objCont->getGraph(graphHandle);
                if (it!=NULL)
                {
                    it->curveToStatic(curveType,curveName);
                    LUA_END(0);
                }
                else
                    errorString=SIM_ERROR_INVALID_HANDLE;
            }
        }
        if (cmd.compare("removeStaticCurve")==0)
        {
            if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0,lua_arg_number,0,lua_arg_string,0))
            {
                int graphHandle=luaWrap_lua_tointeger(L,2);
                int curveType=luaWrap_lua_tointeger(L,3);
                std::string curveName(luaWrap_lua_tostring(L,4));
                CGraph* it=App::ct->objCont->getGraph(graphHandle);
                if (it!=NULL)
                {
                    it->removeStaticCurve(curveType,curveName);
                    LUA_END(0);
                }
                else
                    errorString=SIM_ERROR_INVALID_HANDLE;
            }
        }
        if (cmd.compare("setAssemblyMatchValues")==0)
        {
            if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0,lua_arg_bool,0,lua_arg_string,0))
            {
                int objHandle=luaWrap_lua_tointeger(L,2);
                bool childAttr=luaWrap_lua_toboolean(L,3);
                std::string matchValues(luaWrap_lua_tostring(L,4));
                C3DObject* it=App::ct->objCont->getObject(objHandle);
                if (it!=NULL)
                {
                    it->setAssemblyMatchValues(childAttr,matchValues.c_str());
                    LUA_END(0);
                }
                else
                    errorString=SIM_ERROR_INVALID_HANDLE;
            }
        }
        if (cmd.compare("getAssemblyMatchValues")==0)
        {
            if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0,lua_arg_bool,0))
            {
                int objHandle=luaWrap_lua_tointeger(L,2);
                bool childAttr=luaWrap_lua_toboolean(L,3);
                C3DObject* it=App::ct->objCont->getObject(objHandle);
                if (it!=NULL)
                {
                    std::string str(it->getAssemblyMatchValues(childAttr));
                    luaWrap_lua_pushstring(L,str.c_str());
                    LUA_END(1);
                }
                else
                    errorString=SIM_ERROR_INVALID_HANDLE;
            }
        }
    }
    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetReferencedHandles(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetReferencedHandles");

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int objHandle=(luaWrap_lua_tointeger(L,1));
        if (luaWrap_lua_istable(L,2))
        {
            int cnt=luaWrap_lua_objlen(L,2);
            if (cnt>0)
            {
                std::vector<int> handles;
                handles.resize(cnt);
                getIntsFromTable(L,2,cnt,&handles[0]);
                retVal=simSetReferencedHandles_internal(objHandle,cnt,&handles[0],NULL,NULL);
            }
            else
                retVal=simSetReferencedHandles_internal(objHandle,0,NULL,NULL,NULL);
        }
        else
            errorString=SIM_ERROR_INVALID_ARGUMENT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetReferencedHandles(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetReferencedHandles");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int objHandle=(luaWrap_lua_tointeger(L,1));
        int* handles;
        int cnt=simGetReferencedHandles_internal(objHandle,&handles,NULL,NULL);
        if (cnt>=0)
        {
            pushIntTableOntoStack(L,cnt,handles);
            delete[] handles;
            LUA_END(1);
        }
    }
    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetGraphCurve(luaWrap_lua_State* L)
{
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetGraphCurve");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int graphHandle=(luaWrap_lua_tointeger(L,1));
        int graphType=(luaWrap_lua_tointeger(L,2));
        int index=(luaWrap_lua_tointeger(L,3));
        CGraph* graph=App::ct->objCont->getGraph(graphHandle);
        if (graph!=NULL)
        {
            std::vector<float> xVals;
            std::vector<float> yVals;
            std::vector<float> zVals;
            std::string label;
            int curveType;
            float col[3];
            float minMax[6];
            if (graph->getGraphCurve(graphType,index,label,xVals,yVals,zVals,curveType,col,minMax))
            {
                luaWrap_lua_pushstring(L,label.c_str());
                luaWrap_lua_pushnumber(L,curveType);
                pushFloatTableOntoStack(L,3,col);
                if (xVals.size()>0)
                    pushFloatTableOntoStack(L,xVals.size(),&xVals[0]);
                else
                    pushFloatTableOntoStack(L,0,NULL);
                if (yVals.size()>0)
                    pushFloatTableOntoStack(L,yVals.size(),&yVals[0]);
                else
                    pushFloatTableOntoStack(L,0,NULL);
                if (graphType==2)
                {
                    if (zVals.size()>0)
                        pushFloatTableOntoStack(L,zVals.size(),&zVals[0]);
                    else
                        pushFloatTableOntoStack(L,0,NULL);
                    if (xVals.size()>0)
                    {
                        pushFloatTableOntoStack(L,6,minMax);
                        LUA_END(7);
                    }
                    LUA_END(6);
                }
                else
                {
                    if (xVals.size()>0)
                    {
                        pushFloatTableOntoStack(L,4,minMax);
                        LUA_END(6);
                    }
                    LUA_END(5);
                }
            }
            else
            { // this should not generate an error!
            }
        }
        else
            errorString=SIM_ERROR_OBJECT_NOT_GRAPH;
    }
    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simHandleVarious(luaWrap_lua_State* L)
{ // DEPRECATED since V3.4.0
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleVarious");

    int retVal=simHandleVarious_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetMpConfigForTipPose(luaWrap_lua_State* L)
{ // DEPRECATED since V3.3.0
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetMpConfigForTipPose");

    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if (!scr->checkAndSetWarning_simGetMpConfigForTipPose_oldCompatibility_21_1_2016())
        luaApiCallWarning(functionName.c_str(),"Function is deprecated. Use simGetConfigForTipPose instead.");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,7))
    {
        int mpHandle=luaToInt(L,1);
        int options=luaToInt(L,2);
        float closeNodesDistance=luaToFloat(L,3);
        int trialCount=luaToInt(L,4);
        float tipPose[7];
        int maxTimeInMs=0;
        getFloatsFromTable(L,5,7,tipPose);
        CMotionPlanningTask* it=App::ct->motionPlanning->getObject(mpHandle);
        if (it!=NULL)
        {
            int res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,true,true,&errorString);
            if (res>=0)
            {
                if (res==2)
                    maxTimeInMs=luaToInt(L,6);
                int res=checkOneGeneralInputArgument(L,7,lua_arg_number,it->getJointCount(),true,true,&errorString);
                if (res>=0)
                {
                    std::vector<float> jointPositions(it->getJointCount(),0.0f);
                    if (res==2)
                    { // we have some ref. configs we want to take into account
                        int tableSize=int(luaWrap_lua_objlen(L,7));
                        int refCount=tableSize/it->getJointCount();
                        if (refCount>0)
                        {
                            std::vector<float> referenceConfigs(it->getJointCount()*refCount,0.0f);
                            getFloatsFromTable(L,7,it->getJointCount()*refCount,&referenceConfigs[0]);
                            res=checkOneGeneralInputArgument(L,8,lua_arg_number,it->getJointCount(),true,true,&errorString);
                            if (res>=0)
                            {
                                std::vector<float> jointWeights(it->getJointCount(),1.0f); // default weights
                                if (res==2)
                                { // we have some weights we want to take into account
                                    getFloatsFromTable(L,8,it->getJointCount(),&jointWeights[0]);
                                }

                                // Check the behaviour and correction pass arguments:
                                res=checkOneGeneralInputArgument(L,9,lua_arg_number,it->getJointCount(),true,true,&errorString);
                                if (res>=0)
                                {
                                    std::vector<int> behaviour(it->getJointCount(),0);
                                    int correctionPasses=0;
                                    if (res==2)
                                    { // we have some "behaviour" values we want to take into account:
                                        getIntsFromTable(L,9,it->getJointCount(),&behaviour[0]);
                                    }
                                    res=checkOneGeneralInputArgument(L,10,lua_arg_number,0,true,true,&errorString);
                                    if (res>=0)
                                    {
                                        if (res==2)
                                            correctionPasses=luaToInt(L,10);
                                        retVal=simGetMpConfigForTipPose_internal(mpHandle,options,closeNodesDistance,trialCount,tipPose,maxTimeInMs,&jointPositions[0],&referenceConfigs[0],refCount,&jointWeights[0],&behaviour[0],correctionPasses);
                                    }
                                }
                            }
                        }
                        else
                            retVal=simGetMpConfigForTipPose_internal(mpHandle,options,closeNodesDistance,trialCount,tipPose,maxTimeInMs,&jointPositions[0],NULL,0,NULL,NULL,0);
                    }
                    else
                        retVal=simGetMpConfigForTipPose_internal(mpHandle,options,closeNodesDistance,trialCount,tipPose,maxTimeInMs,&jointPositions[0],NULL,0,NULL,NULL,0);
                    if (retVal>0)
                    {
                        luaWrap_lua_pushnumber(L,retVal);
                        pushFloatTableOntoStack(L,it->getJointCount(),&jointPositions[0]);
                        LUA_END(2);
                    }
                }
            }
        }
        else
            errorString=SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}


int _simResetPath(luaWrap_lua_State* L)
{ // DEPRECATED
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simResetPath");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simResetPath_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simHandlePath(luaWrap_lua_State* L)
{ // DEPRECATED
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandlePath");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simHandlePath_internal(luaToInt(L,1),luaToFloat(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simResetJoint(luaWrap_lua_State* L)
{ // DEPRECATED
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simResetJoint");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simResetJoint_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simHandleJoint(luaWrap_lua_State* L)
{ // DEPRECATED
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleJoint");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simHandleJoint_internal(luaToInt(L,1),luaToFloat(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simResetTracing(luaWrap_lua_State* L)
{ // deprecated
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simResetTracing");

    luaApiCallWarning(functionName.c_str(),SIM_ERROR_FUNCTION_DEPRECATED_AND_HAS_NO_EFFECT);

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simHandleTracing(luaWrap_lua_State* L)
{ // deprecated
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simHandleTracing");

    luaApiCallWarning(functionName.c_str(),SIM_ERROR_FUNCTION_DEPRECATED_AND_HAS_NO_EFFECT);

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simMoveToPosition(luaWrap_lua_State* L)
{ // can only be called from a script running in a thread!! DEPRECATED
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simMoveToPosition");

    if (!VThread::isCurrentThreadTheMainSimulationThread())
    {
        if (!(CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L))))
        { // Important to first check if we are supposed to leave the thread!!
            if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
            { // Those are the arguments that are always required! (the rest can be ignored or set to nil!
                int objID=luaWrap_lua_tointeger(L,1);
                int relativeToObjID=luaWrap_lua_tointeger(L,2);
                float posTarget[3];
                float eulerTarget[3];
                float maxVelocity;
                C3DObject* object=App::ct->objCont->getObject(objID);
                C3DObject* relToObject=NULL;
                float accel=0.0f; // means infinite accel!! (default value)
                float angleToLinearCoeff=0.1f/(90.0f*degToRad); // (default value)
                int distCalcMethod=sim_distcalcmethod_dl_if_nonzero; // (default value)
                bool foundError=false;
                if ((!foundError)&&(object==NULL))
                {
                    errorString=SIM_ERROR_OBJECT_INEXISTANT;
                    foundError=true;
                }
                if ((!foundError)&&(relativeToObjID==sim_handle_parent))
                {
                    relativeToObjID=-1;
                    C3DObject* parent=object->getParent();
                    if (parent!=NULL)
                        relativeToObjID=parent->getID();
                }
                if ((!foundError)&&(relativeToObjID!=-1))
                {
                    relToObject=App::ct->objCont->getObject(relativeToObjID);
                    if (relToObject==NULL)
                    { // error, object doesn't exist!
                        errorString=SIM_ERROR_OBJECT_INEXISTANT;
                        foundError=true;
                    }
                }

                // Now check the optional arguments:
                int res;
                unsigned char posAndOrient=0;
                if (!foundError) // position argument:
                {
                    res=checkOneGeneralInputArgument(L,3,lua_arg_number,3,true,true,&errorString);
                    if (res==2)
                    { // get the data
                        getFloatsFromTable(L,3,3,posTarget);
                        posAndOrient|=1;
                    }
                    foundError=(res==-1);
                }
                if (!foundError) // orientation argument:
                {
                    res=checkOneGeneralInputArgument(L,4,lua_arg_number,3,true,true,&errorString);
                    if (res==2)
                    { // get the data
                        getFloatsFromTable(L,4,3,eulerTarget);
                        posAndOrient|=2;
                    }
                    foundError=(res==-1);
                }
                if ((!foundError)&&(posAndOrient==0))
                {
                    foundError=true;
                    errorString="Target position and/or target orientation has to be specified.";
                }
                if (!foundError) // target velocity argument:
                {
                    res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,false,false,&errorString);
                    if (res==2)
                    { // get the data
                        maxVelocity=luaToFloat(L,5);
                    }
                    else
                        foundError=true;
                }
                if (!foundError) // Accel argument:
                {
                    res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,true,true,&errorString);
                    if (res==2)
                    { // get the data
                        accel=fabs(luaToFloat(L,6));
                    }
                    foundError=(res==-1);
                }
                if (!foundError) // distance method:
                {
                    res=checkOneGeneralInputArgument(L,7,lua_arg_number,2,true,true,&errorString);
                    if (res==2)
                    { // get the data
                        float tmpF[2];
                        int tmpI[2];
                        getFloatsFromTable(L,7,2,tmpF);
                        getIntsFromTable(L,7,2,tmpI);
                        distCalcMethod=tmpI[0];
                        angleToLinearCoeff=tmpF[1];
                    }
                    foundError=(res==-1);
                }
                if (!foundError)
                { // do the job here!
                    C7Vector startTr(object->getCumulativeTransformationPart1());
                    C7Vector relTr;
                    relTr.setIdentity();
                    if (relToObject!=NULL)
                        relTr=relToObject->getCumulativeTransformation();
                    startTr=relTr.getInverse()*startTr;

                    C7Vector targetTr(startTr);
                    if (posAndOrient&1)
                        targetTr.X.set(posTarget);
                    if (posAndOrient&2)
                        targetTr.Q.setEulerAngles(eulerTarget[0],eulerTarget[1],eulerTarget[2]);
                    float currentVel=0.0f;
                    CVThreadData* threadData=CThreadPool::getCurrentThreadData();
                    float lastTime=float(App::ct->simulation->getSimulationTime_ns())/1000000.0f+threadData->usedMovementTime;

                    float dl=(targetTr.X-startTr.X).getLength();
                    float da=targetTr.Q.getAngleBetweenQuaternions(startTr.Q)*angleToLinearCoeff;
                    float vdl=dl;
                    if (distCalcMethod==sim_distcalcmethod_dl)
                        vdl=dl;
                    if (distCalcMethod==sim_distcalcmethod_dac)
                        vdl=da;
                    if (distCalcMethod==sim_distcalcmethod_max_dl_dac)
                        vdl=SIM_MAX(dl,da);
                    if (distCalcMethod==sim_distcalcmethod_dl_and_dac)
                        vdl=dl+da;
                    if (distCalcMethod==sim_distcalcmethod_sqrt_dl2_and_dac2)
                        vdl=sqrtf(dl*dl+da*da);
                    if (distCalcMethod==sim_distcalcmethod_dl_if_nonzero)
                    {
                        vdl=dl;
                        if (dl<0.00005f) // Was dl==0.0f before (tolerance problem). Changed on 1/4/2011
                            vdl=da;
                    }
                    if (distCalcMethod==sim_distcalcmethod_dac_if_nonzero)
                    {
                        vdl=da;
                        if (da<0.01f*degToRad) // Was da==0.0f before (tolerance problem). Changed on 1/4/2011
                            vdl=dl;
                    }
                    // vld is the totalvirtual distance
                    float currentPos=0.0f;
                    bool movementFinished=false;
                    float dt=float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f; // this is the time left if we leave here

                    if (vdl==0.0f)
                    { // if the path length is 0 (the two positions might still be not-coincident, depending on the calculation method!)
                        if (App::ct->objCont->getObject(objID)==object) // make sure the object is still valid (running in a thread)
                        {
                            if (relToObject==NULL)
                            { // absolute
                                C7Vector parentInv(object->getParentCumulativeTransformation().getInverse());
                                object->setLocalTransformation(parentInv*targetTr);
                            }
                            else
                            { // relative to a specific object (2009/11/17)
                                if (App::ct->objCont->getObject(relativeToObjID)==relToObject) // make sure the object is still valid (running in a thread)
                                { // ok
                                    C7Vector relToTr(relToObject->getCumulativeTransformation());
                                    targetTr=relToTr*targetTr;
                                    C7Vector parentInv(object->getParentCumulativeTransformation().getInverse());
                                    object->setLocalTransformation(parentInv*targetTr);
                                }
                            }
                        }
                        movementFinished=true;
                    }

                    while (!movementFinished)
                    {
                        float currentTime=float(App::ct->simulation->getSimulationTime_ns())/1000000.0f+float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f;
                        dt=currentTime-lastTime;
                        lastTime=currentTime;

                        if (accel==0.0f)
                        { // Means infinite acceleration
                            float timeNeeded=(vdl-currentPos)/maxVelocity;
                            currentVel=maxVelocity;
                            if (timeNeeded>dt)
                            {
                                currentPos+=dt*maxVelocity;
                                dt=0.0f; // this is what is left
                            }
                            else
                            {
                                currentPos=vdl;
                                if (timeNeeded>=0.0f)
                                    dt-=timeNeeded;
                            }
                        }
                        else
                        {
                            double p=currentPos;
                            double v=currentVel;
                            double t=dt;
                            CLinMotionRoutines::getNextValues(p,v,maxVelocity,accel,0.0f,vdl,0.0f,0.0f,t);
                            currentPos=float(p);
                            currentVel=float(v);
                            dt=float(t);
                        }

                        // Now check if we are within tolerances:
                        if (fabs(currentPos-vdl)<=0.00001f)//tol[0])
                            movementFinished=true;

                        // Set the new configuration of the object:
                        float ll=currentPos/vdl;
                        if (ll>1.0f)
                            ll=1.0f;
                        C7Vector newAbs;
                        newAbs.buildInterpolation(startTr,targetTr,ll);
                        if (App::ct->objCont->getObject(objID)==object) // make sure the object is still valid (running in a thread)
                        {
                            if ( (relToObject!=NULL)&&(App::ct->objCont->getObject(relativeToObjID)!=relToObject) )
                                movementFinished=true; // the object was destroyed during execution of the command!
                            else
                            {
                                C7Vector parentInv(object->getParentCumulativeTransformation().getInverse());
                                C7Vector currAbs(object->getCumulativeTransformationPart1());
                                C7Vector relToTr;
                                relToTr.setIdentity();
                                if (relToObject!=NULL)
                                    relToTr=relToObject->getCumulativeTransformation();
                                currAbs=relToTr.getInverse()*currAbs;
                                if ((posAndOrient&1)==0)
                                    newAbs.X=currAbs.X;
                                if ((posAndOrient&2)==0)
                                    newAbs.Q=currAbs.Q;
                                newAbs=relToTr*newAbs;
                                object->setLocalTransformation(parentInv*newAbs);
                            }
                        }
                        else
                            movementFinished=true; // the object was destroyed during execution of the command!

                        if (!movementFinished)
                        {
                            CThreadPool::switchBackToPreviousThread();
                            if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                                break; // error
                        }
                    }
                    // The movement finished. Now add the time used:
                    threadData->usedMovementTime=float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f-dt;
                    luaWrap_lua_pushnumber(L,dt); // success (deltaTime left)
                    LUA_END(1);
                }
            }
        }
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simMoveToJointPositions(luaWrap_lua_State* L)
{ // can only be called from a script running in a thread!! DEPRECATED
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simMoveToJointPositions");

    if (!VThread::isCurrentThreadTheMainSimulationThread())
    {
        if (!(CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L))))
        { // Important to first check if we are supposed to leave the thread!!
            if (!( (!luaWrap_lua_istable(L,1))||(!luaWrap_lua_istable(L,2))||(luaWrap_lua_objlen(L,1)>luaWrap_lua_objlen(L,2))||(luaWrap_lua_objlen(L,1)==0) ))
            { // Ok we have 2 tables with same sizes.
                int tableLen=luaWrap_lua_objlen(L,1);
                bool sameTimeFinish=true;
                float maxVelocity=0.0f;
                float accel=0.0f; // means infinite accel!! (default value)
                bool accelTablePresent=false;
                float angleToLinearCoeff=1.0f;
                bool foundError=false;
                // Now check the other arguments:
                int res;
                if (luaWrap_lua_istable(L,3))
                    sameTimeFinish=false; // we do not finish at the same time!
                if (!foundError) // velocity or velocities argument (not optional!):
                {
                    if (sameTimeFinish)
                    {
                        res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,false,false,&errorString);
                        if (res==2)
                            maxVelocity=luaToFloat(L,3);
                        else
                            foundError=true;
                    }
                    else
                    {
                        res=checkOneGeneralInputArgument(L,3,lua_arg_number,tableLen,false,false,&errorString);
                        if (res!=2)
                            foundError=true;
                    }
                }
                if (!foundError) // Accel argument:
                {
                    if (sameTimeFinish)
                    {
                        res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,true,true,&errorString);
                        if (res==2)
                        { // get the data
                            accel=fabs(luaToFloat(L,4));
                        }
                        foundError=(res==-1);
                    }
                    else
                    {
                        res=checkOneGeneralInputArgument(L,4,lua_arg_number,tableLen,true,true,&errorString);
                        if (res==2)
                            accelTablePresent=true;
                        foundError=(res==-1);
                    }
                }
                if (!foundError) // angleToLinearCoeff argument:
                {
                    if (sameTimeFinish)
                    {
                        res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,true,&errorString);
                        if (res==2)
                        { // get the data
                            angleToLinearCoeff=fabs(luaToFloat(L,5));
                        }
                        foundError=(res==-1);
                    }
                    else
                        angleToLinearCoeff=1.0f; // no conversion!
                }
                if (!foundError)
                { // do the job here!
                    int* jointHandles=new int[tableLen];
                    float* jointCurrentVirtualPositions=new float[tableLen];
                    float* jointCurrentVirtualVelocities=new float[tableLen];
                    float* jointStartPositions=new float[tableLen];
                    float* jointTargetPositions=new float[tableLen];
                    float* jointVirtualDistances=new float[tableLen];
                    float* jointMaxVelocities=new float[tableLen];
                    float* jointAccels=new float[tableLen];

                    getIntsFromTable(L,1,tableLen,jointHandles);
                    getFloatsFromTable(L,2,tableLen,jointTargetPositions);
                    if (!sameTimeFinish)
                    {
                        getFloatsFromTable(L,3,tableLen,jointMaxVelocities);
                        for (int i=0;i<tableLen;i++)
                            jointMaxVelocities[i]=fabs(jointMaxVelocities[i]);
                        if (accelTablePresent)
                        {
                            getFloatsFromTable(L,4,tableLen,jointAccels);
                            for (int i=0;i<tableLen;i++)
                                jointAccels[i]=fabs(jointAccels[i]);
                        }
                    }
                    float maxVirtualDist=0.0f;
                    int maxVirtualDistIndex=0;
                    for (int i=0;i<tableLen;i++)
                    {
                        jointCurrentVirtualPositions[i]=0.0f;
                        jointCurrentVirtualVelocities[i]=0.0f;
                        if (sameTimeFinish)
                            jointMaxVelocities[i]=maxVelocity;
                        if (!accelTablePresent)
                            jointAccels[i]=accel;

                        CJoint* it=App::ct->objCont->getJoint(jointHandles[i]);
                        if ((it!=NULL)&&(it->getJointType()!=sim_joint_spherical_subtype))
                        { // make sure target is within allowed range, and check the maximum virtual distance:
                            jointStartPositions[i]=it->getPosition();
                            float minP=it->getPositionIntervalMin();
                            float maxP=minP+it->getPositionIntervalRange();
                            if (it->getPositionIsCyclic())
                            {
                                float da=tt::getAngleMinusAlpha(jointTargetPositions[i],jointStartPositions[i]);
                                jointTargetPositions[i]=jointStartPositions[i]+da;
                            }
                            else
                            {
                                if (minP>jointTargetPositions[i])
                                    jointTargetPositions[i]=minP;
                                if (maxP<jointTargetPositions[i])
                                    jointTargetPositions[i]=maxP;
                            }
                            float d=fabs(jointTargetPositions[i]-jointStartPositions[i]);
                            if (it->getJointType()==sim_joint_revolute_subtype)
                                d*=angleToLinearCoeff;
                            jointVirtualDistances[i]=d;
                            if (d>maxVirtualDist)
                            {
                                maxVirtualDist=d;
                                maxVirtualDistIndex=i;
                            }
                        }
                        else
                        {
                            // Following are default values in case the joint doesn't exist or is spherical:
                            jointStartPositions[i]=0.0f;
                            jointTargetPositions[i]=0.0f;
                            jointVirtualDistances[i]=0.0f;
                        }
                    }
                    CVThreadData* threadData=CThreadPool::getCurrentThreadData();
                    float lastTime=float(App::ct->simulation->getSimulationTime_ns())/1000000.0f+threadData->usedMovementTime;
                    bool movementFinished=false;
                    float dt=float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f; // this is the time left if we leave here

                    if (maxVirtualDist==0.0f)
                        movementFinished=true;

                    while (!movementFinished)
                    {
                        float currentTime=float(App::ct->simulation->getSimulationTime_ns())/1000000.0f+float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f;
                        dt=currentTime-lastTime;
                        float minTimeLeft=dt;
                        lastTime=currentTime;
                        if (sameTimeFinish)
                        {
                            float timeLeftLocal=dt;
                            // 1. handle the joint with longest distance first:
                            // Does the main joint still exist?
                            if (App::ct->objCont->getJoint(jointHandles[maxVirtualDistIndex])!=NULL)
                            {
                                if (accel==0.0f)
                                { // means infinite accel
                                    float timeNeeded=(jointVirtualDistances[maxVirtualDistIndex]-jointCurrentVirtualPositions[maxVirtualDistIndex])/maxVelocity;
                                    jointCurrentVirtualVelocities[maxVirtualDistIndex]=maxVelocity;
                                    if (timeNeeded>timeLeftLocal)
                                    {
                                        jointCurrentVirtualPositions[maxVirtualDistIndex]+=timeLeftLocal*maxVelocity;
                                        timeLeftLocal=0.0f; // this is what is left
                                    }
                                    else
                                    {
                                        jointCurrentVirtualPositions[maxVirtualDistIndex]=jointVirtualDistances[maxVirtualDistIndex];
                                        if (timeNeeded>=0.0f)
                                            timeLeftLocal-=timeNeeded;
                                    }
                                }
                                else
                                {
                                    double p=jointCurrentVirtualPositions[maxVirtualDistIndex];
                                    double v=jointCurrentVirtualVelocities[maxVirtualDistIndex];
                                    double t=timeLeftLocal;
                                    CLinMotionRoutines::getNextValues(p,v,maxVelocity,accel,0.0f,jointVirtualDistances[maxVirtualDistIndex],0.0f,0.0f,t);
                                    jointCurrentVirtualPositions[maxVirtualDistIndex]=float(p);
                                    jointCurrentVirtualVelocities[maxVirtualDistIndex]=float(v);
                                    timeLeftLocal=float(t);
                                }
                                minTimeLeft=timeLeftLocal;
                                // 2. We adjust the other joints accordingly:
                                float f=1;
                                if (jointVirtualDistances[maxVirtualDistIndex]!=0.0f)
                                    f=jointCurrentVirtualPositions[maxVirtualDistIndex]/jointVirtualDistances[maxVirtualDistIndex];
                                for (int i=0;i<tableLen;i++)
                                {
                                    if (i!=maxVirtualDistIndex)
                                        jointCurrentVirtualPositions[i]=jointVirtualDistances[i]*f;
                                }
                                // 3. Check if within tolerance:
                                if (fabs(jointCurrentVirtualPositions[maxVirtualDistIndex]-jointVirtualDistances[maxVirtualDistIndex])<=0.00001f)
                                    movementFinished=true;
                            }
                            else
                            { // the main joint was removed. End here!
                                movementFinished=true;
                            }
                        }
                        else
                        {
                            bool withinTolerance=true;
                            for (int i=0;i<tableLen;i++)
                            {
                                if (App::ct->objCont->getJoint(jointHandles[i])!=NULL)
                                {
                                    // Check if within tolerance (before):
                                    if (fabs(jointCurrentVirtualPositions[i]-jointVirtualDistances[i])>0.00001f)
                                    {
                                        float timeLeftLocal=dt;
                                        if (jointAccels[i]==0.0f)
                                        { // means infinite accel
                                            float timeNeeded=(jointVirtualDistances[i]-jointCurrentVirtualPositions[i])/jointMaxVelocities[i];
                                            jointCurrentVirtualVelocities[i]=jointMaxVelocities[i];
                                            if (timeNeeded>timeLeftLocal)
                                            {
                                                jointCurrentVirtualPositions[i]+=timeLeftLocal*jointMaxVelocities[i];
                                                timeLeftLocal=0.0f; // this is what is left
                                            }
                                            else
                                            {
                                                jointCurrentVirtualPositions[i]=jointVirtualDistances[i];
                                                if (timeNeeded>=0.0f)
                                                    timeLeftLocal-=timeNeeded;
                                            }
                                        }
                                        else
                                        {
                                            double p=jointCurrentVirtualPositions[i];
                                            double v=jointCurrentVirtualVelocities[i];
                                            double t=timeLeftLocal;
                                            CLinMotionRoutines::getNextValues(p,v,jointMaxVelocities[i],jointAccels[i],0.0f,jointVirtualDistances[i],0.0f,0.0f,t);
                                            jointCurrentVirtualPositions[i]=float(p);
                                            jointCurrentVirtualVelocities[i]=float(v);
                                            timeLeftLocal=float(t);
                                        }
                                        if (timeLeftLocal<minTimeLeft)
                                            minTimeLeft=timeLeftLocal;
                                        // Check if within tolerance (after):
                                        if (fabs(jointCurrentVirtualPositions[i]-jointVirtualDistances[i])>0.00001f)
                                            withinTolerance=false;
                                    }
                                }
                            }
                            if (withinTolerance)
                                movementFinished=true;
                        }
                        dt=minTimeLeft;

                        // We set all joint positions:
                        for (int i=0;i<tableLen;i++)
                        {
                            CJoint* joint=App::ct->objCont->getJoint(jointHandles[i]);
                            if ( (joint!=NULL)&&(joint->getJointType()!=sim_joint_spherical_subtype)&&(jointVirtualDistances[i]!=0.0f) )
                            {
                                if (joint->getJointMode()==sim_jointmode_force)
                                    joint->setDynamicMotorPositionControlTargetPosition(jointStartPositions[i]+(jointTargetPositions[i]-jointStartPositions[i])*jointCurrentVirtualPositions[i]/jointVirtualDistances[i]);
                                else
                                    joint->setPosition(jointStartPositions[i]+(jointTargetPositions[i]-jointStartPositions[i])*jointCurrentVirtualPositions[i]/jointVirtualDistances[i]);
                            }
                        }

                        if (!movementFinished)
                        {
                            CThreadPool::switchBackToPreviousThread();
                            if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                                break; // error
                        }
                    }
                    // The movement finished. Now add the time used:
                    threadData->usedMovementTime=float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f-dt;
                    luaWrap_lua_pushnumber(L,dt); // success (deltaTime left)

                    delete[] jointAccels;
                    delete[] jointMaxVelocities;
                    delete[] jointVirtualDistances;
                    delete[] jointTargetPositions;
                    delete[] jointStartPositions;
                    delete[] jointCurrentVirtualVelocities;
                    delete[] jointCurrentVirtualPositions;
                    delete[] jointHandles;
                    LUA_END(1);
                }
            }
            else
                errorString="One of the function's argument type is not correct or table sizes are invalid or do not match";
        }
    }
    else
        errorString=SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSerialPortOpen(luaWrap_lua_State* L)
{ // DEPRECATED For backward compatibility (10/04/2012)
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSerialPortOpen");

    int retVal=-1; // means error
#ifdef SIM_WITH_SERIAL
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=App::ct->serialPortContainer->serialPortOpen_old(true,luaToInt(L,1),luaToInt(L,2));
#endif

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSerialPortClose(luaWrap_lua_State* L)
{ // DEPRECATED For backward compatibility (10/04/2012)
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSerialPortClose");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simSerialPortClose_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSerialPortSend(luaWrap_lua_State* L)
{ // DEPRECATED For backward compatibility (10/04/2012)
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSerialPortSend");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0))
    {
        size_t dataLength;
        char* data=(char*)luaWrap_lua_tolstring(L,2,&dataLength);
        retVal=simSerialPortSend_internal(luaToInt(L,1),data,dataLength);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSerialPortRead(luaWrap_lua_State* L)
{ // DEPRECATED For backward compatibility (10/04/2012)
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSerialPortRead");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_bool,0))
    {
        bool blocking=luaToBool(L,3);
        std::string closingString="";
        float timeOut=0.0f;
        bool err=false;
        if (blocking)
        {
            if (!VThread::isCurrentThreadTheMainSimulationThread())
            {
                int res=checkOneGeneralInputArgument(L,4,lua_arg_string,0,true,true,&errorString);
                if (res==2)
                {
                    size_t dataLength;
                    char* data=(char*)luaWrap_lua_tolstring(L,4,&dataLength);
                    closingString.assign(data,dataLength);
                }
                if (res!=-1)
                {
                    res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,false,&errorString);
                    if (res==2)
                        timeOut=luaToFloat(L,5);
                    if (res!=-1)
                        err=true;
                }
                else
                    err=true;
            }
            else
            {
                errorString=SIM_ERROR_BLOCKING_OPERATION_ONLY_FROM_THREAD;
                err=true;
            }
        }
        if (!err)
        {
            int maxLength=luaToInt(L,2);
            char* data=new char[maxLength];
            int dataRead=0;
            if (blocking)
            {
                int startTime=VDateTime::getTimeInMs();
                while (maxLength>dataRead)
                {
                    int nb=simSerialPortRead_internal(luaToInt(L,1),data+dataRead,maxLength-dataRead);
                    if (nb<0)
                        break; // error
                    dataRead+=nb;
                    int csl=int(closingString.length());
                    if ( (csl!=0)&&(dataRead>=csl) )
                    { // Break if we find the closing signature:
                        bool same=true;
                        for (int i=0;i<csl;i++)
                        {
                            if (data[dataRead-csl+i]!=closingString[i])
                            {
                                same=false;
                                break;
                            }
                        }
                        if (same)
                        {
                            dataRead-=csl;
                            break;
                        }
                    }
                    if (maxLength>dataRead)
                    {
                        CThreadPool::switchBackToPreviousThread();
                        if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                            break;
                    }
                    if ( (timeOut>0.0000001f)&&((float(VDateTime::getTimeDiffInMs(startTime))/1000.0f)>timeOut) )
                        break;
                }
            }
            else
                dataRead=simSerialPortRead_internal(luaToInt(L,1),data,maxLength);
            if (dataRead>0)
            {
                luaWrap_lua_pushlstring(L,(const char*)data,dataRead);
                delete[] data;
                LUA_END(1);
            }
            delete[] data;
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetInstanceIndex(luaWrap_lua_State* L)
{ // DEPRECATED
    LUA_API_FUNCTION_DEBUG;
    LUA_START("");

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,0);
    LUA_END(1);
}

int _simGetVisibleInstanceIndex(luaWrap_lua_State* L)
{ // DEPRECATED
    LUA_API_FUNCTION_DEBUG;
    LUA_START("");

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,0);
    LUA_END(1);
}

int _simGetSystemTimeInMilliseconds(luaWrap_lua_State* L)
{ // DEPRECATED
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetSystemTimeInMilliseconds");

    int res=checkOneGeneralInputArgument(L,1,lua_arg_number,0,true,false,&errorString);
    if (res==0)
    {
        luaWrap_lua_pushnumber(L,VDateTime::getTimeInMs());
        LUA_END(1);
    }
    if (res==2)
    {
        int lastTime=luaToInt(L,1);
        luaWrap_lua_pushnumber(L,VDateTime::getTimeDiffInMs(lastTime));
        LUA_END(1);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simLockInterface(luaWrap_lua_State* L)
{ // DEPRECATED
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simLockInterface");

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,-1);
    LUA_END(1);
}

int _simJointGetForce(luaWrap_lua_State* L)
{ // DEPRECATED since release 3.1.2
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simJointGetForce");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        float jointF[1];
        if (simGetJointForce_internal(luaToInt(L,1),jointF)>0)
        {
            luaWrap_lua_pushnumber(L,jointF[0]);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simRMLPosition(luaWrap_lua_State* L)
{ // DEPRECATED since release 3.1.2
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRMLPosition");

    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if (!scr->checkAndSetWarning_simRMLPosition_oldCompatibility_30_8_2014())
        luaApiCallWarning(functionName.c_str(),"Function is deprecated. Use simRMLPos instead.");

    int retVal=-43; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int dofs=luaToInt(L,1);
        double timeStep=luaWrap_lua_tonumber(L,2);
        int flags=luaToInt(L,3);
        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,dofs*3,false,false,&errorString);
        if (res==2)
        {
            double* currentPosVelAccel=new double[dofs*3];
            getDoublesFromTable(L,4,dofs*3,currentPosVelAccel);
            res=checkOneGeneralInputArgument(L,5,lua_arg_number,dofs*3,false,false,&errorString);
            if (res==2)
            {
                double* maxVelAccelJerk=new double[dofs*3];
                getDoublesFromTable(L,5,dofs*3,maxVelAccelJerk);
                res=checkOneGeneralInputArgument(L,6,lua_arg_bool,dofs,false,false,&errorString);
                if (res==2)
                {
                    char* selection=new char[dofs];
                    getBoolsFromTable(L,6,dofs,selection);
                    res=checkOneGeneralInputArgument(L,7,lua_arg_number,dofs*2,false,false,&errorString);
                    if (res==2)
                    {
                        double* targetPosVel=new double[dofs*2];
                        getDoublesFromTable(L,7,dofs*2,targetPosVel);

                        double* newPosVelAccel=new double[dofs*3];
                        unsigned char auxData[1+8+8];
                        auxData[0]=1;
                        retVal=simRMLPosition_internal(dofs,timeStep,flags,currentPosVelAccel,maxVelAccelJerk,(unsigned char*)selection,targetPosVel,newPosVelAccel,auxData);
                        if (retVal>=0)
                        {
                            luaWrap_lua_pushnumber(L,retVal);
                            pushDoubleTableOntoStack(L,dofs*3,newPosVelAccel);
                            luaWrap_lua_pushnumber(L,((double*)(auxData+1))[0]);
                        }
                        delete[] newPosVelAccel;
                        delete[] targetPosVel;
                    }
                    delete[] selection;
                }
                delete[] maxVelAccelJerk;
            }
            delete[] currentPosVelAccel;
            if (retVal>=0)
            {
                LUA_END(3);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRMLVelocity(luaWrap_lua_State* L)
{ // DEPRECATED since release 3.1.2
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRMLVelocity");

    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if (!scr->checkAndSetWarning_simRMLVelocity_oldCompatibility_30_8_2014())
        luaApiCallWarning(functionName.c_str(),"Function is deprecated. Use simRMLVel instead.");

    int retVal=-43; //error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int dofs=luaToInt(L,1);
        double timeStep=luaWrap_lua_tonumber(L,2);
        int flags=luaToInt(L,3);
        int res=checkOneGeneralInputArgument(L,4,lua_arg_number,dofs*3,false,false,&errorString);
        if (res==2)
        {
            double* currentPosVelAccel=new double[dofs*3];
            getDoublesFromTable(L,4,dofs*3,currentPosVelAccel);
            res=checkOneGeneralInputArgument(L,5,lua_arg_number,dofs*2,false,false,&errorString);
            if (res==2)
            {
                double* maxAccelJerk=new double[dofs*2];
                getDoublesFromTable(L,5,dofs*2,maxAccelJerk);
                res=checkOneGeneralInputArgument(L,6,lua_arg_bool,dofs,false,false,&errorString);
                if (res==2)
                {
                    char* selection=new char[dofs];
                    getBoolsFromTable(L,6,dofs,selection);
                    res=checkOneGeneralInputArgument(L,7,lua_arg_number,dofs,false,false,&errorString);
                    if (res==2)
                    {
                        double* targetVel=new double[dofs];
                        getDoublesFromTable(L,7,dofs,targetVel);

                        double* newPosVelAccel=new double[dofs*3];
                        unsigned char auxData[1+8+8];
                        auxData[0]=1;
                        retVal=simRMLVelocity_internal(dofs,timeStep,flags,currentPosVelAccel,maxAccelJerk,(unsigned char*)selection,targetVel,newPosVelAccel,auxData);
                        if (retVal>=0)
                        {
                            luaWrap_lua_pushnumber(L,retVal);
                            pushDoubleTableOntoStack(L,dofs*3,newPosVelAccel);
                            luaWrap_lua_pushnumber(L,((double*)(auxData+1))[0]);
                        }
                        delete[] newPosVelAccel;
                        delete[] targetVel;
                    }
                    delete[] selection;
                }
                delete[] maxAccelJerk;
            }
            delete[] currentPosVelAccel;
            if (retVal>=0)
            {
                LUA_END(3);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCopyPasteSelectedObjects(luaWrap_lua_State* L)
{ // DEPRECATED since release 3.1.3
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCopyPasteSelectedObjects");

    int retVal=simCopyPasteSelectedObjects_internal();

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simFindIkPath(luaWrap_lua_State* L)
{ // DEPRECATED since 3.3.0
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simFindIkPath");

    int currentScriptID=getCurrentScriptID(L);
    CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
    if (!scr->checkAndSetWarning_simFindIkPath_oldCompatibility_2_2_2016())
        luaApiCallWarning(functionName.c_str(),"Function is deprecated. Use simGenerateIkPath instead.");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,1,lua_arg_number,1,lua_arg_number,0,lua_arg_number,0))
    {
        int mpHandle=luaToInt(L,1);
        int options=luaToInt(L,4);
        float stepSize=luaToFloat(L,5);
        CMotionPlanningTask* it=App::ct->motionPlanning->getObject(mpHandle);
        if (it!=NULL)
        {
            int dof=it->getJointCount();
            // now check the args again, this time taking the table sizes into account
            if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,dof,lua_arg_number,7,lua_arg_number,0,lua_arg_number,0))
            {
                std::vector<float> startConfig(dof,0.0f);
                std::vector<float> goalPose(7,0.0f);
                getFloatsFromTable(L,2,dof,&startConfig[0]);
                getFloatsFromTable(L,3,7,&goalPose[0]);

                int configCount=0;
                float* configs=simFindIkPath_internal(mpHandle,&startConfig[0],&goalPose[0],options,stepSize,&configCount,NULL,NULL,NULL);
                if (configs!=NULL)
                {
                    pushFloatTableOntoStack(L,dof*configCount,configs);
                    pushFloatTableOntoStack(L,configCount,configs+dof*configCount);
                    pushFloatTableOntoStack(L,configCount*3,configs+dof*configCount+configCount);
                    pushFloatTableOntoStack(L,configCount*4,configs+dof*configCount+configCount+configCount*3);
                    pushFloatTableOntoStack(L,configCount,configs+dof*configCount+configCount+configCount*3+configCount*4);
                    delete[] configs;
                    LUA_END(5);
                }
            }
        }
        else
            errorString=SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simCreateMotionPlanning(luaWrap_lua_State* L)
{ // DEPRECATED since 3.3.0
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateMotionPlanning");

    if (!App::userSettings->enableOldMotionPlanningGui)
    {
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        if (!scr->checkAndSetWarning_oldMotionPlanningFunctionality_oldCompatibility_11_2_2016())
            luaApiCallWarning(functionName.c_str(),"It is recommended to use the new path/motion planning functionality based on the OMPL plugin for V-REP.");
    }

    int retVal=-1;
    if (luaWrap_lua_gettop(L)>0)
    {
        if (luaWrap_lua_istable(L,1))
        {
            int jointCnt=int(luaWrap_lua_objlen(L,1));
            int* jointHandles=new int[jointCnt];
            int* jointSubdiv=new int[jointCnt];
            float* jointWeights=new float[jointCnt];
            int* _jointSubdiv=NULL;
            float* _jointWeights=NULL;
            getIntsFromTable(L,1,jointCnt,jointHandles);
            int res=checkOneGeneralInputArgument(L,2,lua_arg_number,jointCnt,true,true,&errorString);
            if (res>=0)
            {
                if (res==2)
                {
                    getIntsFromTable(L,2,jointCnt,jointSubdiv);
                    _jointSubdiv=jointSubdiv;
                }
                res=checkOneGeneralInputArgument(L,3,lua_arg_number,jointCnt,true,true,&errorString);
                if (res>=0)
                {
                    if (res==2)
                    {
                        getFloatsFromTable(L,3,jointCnt,jointWeights);
                        _jointWeights=jointWeights;
                    }
                    res=checkOneGeneralInputArgument(L,4,lua_arg_number,0,false,false,&errorString);
                    if (res==2)
                    {
                        int options=luaToInt(L,4);
                        res=checkOneGeneralInputArgument(L,5,lua_arg_number,5,false,false,&errorString);
                        if (res==2)
                        {
                            int intParams[5];
                            getIntsFromTable(L,5,5,intParams);
                            res=checkOneGeneralInputArgument(L,6,lua_arg_number,6,true,true,&errorString);
                            if (res>=0)
                            {
                                float floatParams[6];
                                float* _floatParams=NULL;
                                if (res==2)
                                {
                                    getFloatsFromTable(L,6,6,floatParams);
                                    _floatParams=floatParams;
                                }
                                retVal=simCreateMotionPlanning_internal(jointCnt,jointHandles,_jointSubdiv,_jointWeights,options,intParams,_floatParams,NULL);
                            }
                        }
                    }
                }
            }

            delete[] jointWeights;
            delete[] jointSubdiv;
            delete[] jointHandles;
        }
        else
            errorString=SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG;
    }
    else
        errorString=SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS;

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simRemoveMotionPlanning(luaWrap_lua_State* L)
{ // DEPRECATED since 3.3.0
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRemoveMotionPlanning");

    if (!App::userSettings->enableOldMotionPlanningGui)
    {
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        if (!scr->checkAndSetWarning_oldMotionPlanningFunctionality_oldCompatibility_11_2_2016())
            luaApiCallWarning(functionName.c_str(),"It is recommended to use the new path/motion planning functionality based on the OMPL plugin for V-REP.");
    }

    int retVal=-1;
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int handle=luaToInt(L,1);
        retVal=simRemoveMotionPlanning_internal(handle);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetPathPlanningHandle(luaWrap_lua_State* L)
{ // DEPRECATED since 3.3.0
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetPathPlanningHandle");

    if (!App::userSettings->enableOldPathPlanningGui)
    {
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        if (!scr->checkAndSetWarning_oldPathPlanningFunctionality_oldCompatibility_11_2_2016())
            luaApiCallWarning(functionName.c_str(),"It is recommended to use the new path/motion planning functionality based on the OMPL plugin for V-REP.");
    }

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string name(luaWrap_lua_tostring(L,1));
        if (suffixAdjustStringIfNeeded(functionName,true,L,name))
        {
            quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
            retVal=simGetPathPlanningHandle_internal(name.c_str());
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetMotionPlanningHandle(luaWrap_lua_State* L)
{ // DEPRECATED since 3.3.0
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetMotionPlanningHandle");

    if (!App::userSettings->enableOldMotionPlanningGui)
    {
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        if (!scr->checkAndSetWarning_oldMotionPlanningFunctionality_oldCompatibility_11_2_2016())
            luaApiCallWarning(functionName.c_str(),"It is recommended to use the new path/motion planning functionality based on the OMPL plugin for V-REP.");
    }

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string name(luaWrap_lua_tostring(L,1));
        if (suffixAdjustStringIfNeeded(functionName,true,L,name))
        {
            quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
            retVal=simGetMotionPlanningHandle_internal(name.c_str());
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSearchPath(luaWrap_lua_State* L)
{ // DEPRECATED since 3.3.0
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSearchPath");

    if (!App::userSettings->enableOldPathPlanningGui)
    {
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        if (!scr->checkAndSetWarning_oldPathPlanningFunctionality_oldCompatibility_11_2_2016())
            luaApiCallWarning(functionName.c_str(),"It is recommended to use the new path/motion planning functionality based on the OMPL plugin for V-REP.");
    }

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int pathPlanningObjectHandle=luaToInt(L,1);
        float maximumSearchTime=tt::getLimitedFloat(0.001f,36000.0f,luaToFloat(L,2));
        float subDt=0.05f; // 50 ms at a time (default)
        bool foundError=false;
        // Now check the optional argument:
        int res;
        if (!foundError) // sub-dt argument:
        {
            res=checkOneGeneralInputArgument(L,3,lua_arg_number,0,true,true,&errorString);
            if (res==2)
            { // get the data
                subDt=tt::getLimitedFloat(0.001f,SIM_MIN(1.0f,maximumSearchTime),luaToFloat(L,3));
            }
            foundError=(res==-1);
        }
        if (!foundError)
        {
            CPathPlanningTask* it=App::ct->pathPlanning->getObject(pathPlanningObjectHandle);
            if (it==NULL)
                errorString=SIM_ERROR_PATH_PLANNING_OBJECT_INEXISTANT;
            else
            {
                retVal=0;
                if (VThread::isCurrentThreadTheMainSimulationThread())
                { // non-threaded
                    if (it->performSearch(false,maximumSearchTime))
                        retVal=1;
                }
                else
                { // threaded call:
                    CPathPlanningTask* oldIt=it;
                    it=oldIt->copyYourself(); // we copy it because the original might be destroyed at any time
                    it->setOriginalTask(oldIt);
                    retVal=-1; // for error
                    bool err=false;
                    if (it->initiateSteppedSearch(false,maximumSearchTime,subDt))
                    {
                        retVal=-2; // means search not yet finished
                        while (retVal==-2)
                        {
                            retVal=it->performSteppedSearch();
                            if (retVal==-2)
                            { // we are not yet finished with the search!
                                CThreadPool::switchBackToPreviousThread();
                                if (CThreadPool::getSimulationStopRequested()||(!isObjectAssociatedWithThisThreadedChildScriptValid(L)))
                                {
                                    retVal=-1; // generate an error
                                    break; // will generate an error (retVal is -1)
                                }
                            }
                        }
                        if (retVal==-1)
                        {
                            errorString=SIM_ERROR_PATH_PLANNING_OBJECT_NOT_CONSISTENT_ANYMORE;
                            retVal=-1; // for error
                            err=true;
                        }
                        // Return values are -1 (error), 0 (no path found) 1 (partial path found) and 2 (full path found)
                    }
                    else
                    { // the task is not consistent!
                        errorString=SIM_ERROR_PATH_PLANNING_OBJECT_NOT_CONSISTENT;
                        err=true;
                    }

                    if (!err)
                    {
                        CPathPlanningTask* originalIt=it->getOriginalTask();
                        int tree1Handle,tree2Handle;
                        it->getAndDisconnectSearchTrees(tree1Handle,tree2Handle); // to keep trees visible!
                        delete it;
                        // Now we connect the trees only if the originalTask still exists:
                        bool found=false;
                        for (int ot=0;ot<int(App::ct->pathPlanning->allObjects.size());ot++)
                        {
                            if (App::ct->pathPlanning->allObjects[ot]==originalIt)
                            {
                                found=true;
                                break;
                            }
                        }
                        if (found)
                            originalIt->connectExternalSearchTrees(tree1Handle,tree2Handle);
                        else
                        {
                            App::ct->drawingCont->removeObject(tree1Handle);
                            App::ct->drawingCont->removeObject(tree2Handle);
                        }
                    }
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simInitializePathSearch(luaWrap_lua_State* L)
{ // DEPRECATED since 3.3.0
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simInitializePathSearch");

    if (!App::userSettings->enableOldPathPlanningGui)
    {
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        if (!scr->checkAndSetWarning_oldPathPlanningFunctionality_oldCompatibility_11_2_2016())
            luaApiCallWarning(functionName.c_str(),"It is recommended to use the new path/motion planning functionality based on the OMPL plugin for V-REP.");
    }

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
    {
        int pathPlanningObjectHandle=luaToInt(L,1);
        float maximumSearchTime=luaToFloat(L,2);
        float searchTimeStep=luaToFloat(L,3);
        retVal=simInitializePathSearch_internal(pathPlanningObjectHandle,maximumSearchTime,searchTimeStep);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simPerformPathSearchStep(luaWrap_lua_State* L)
{ // DEPRECATED since 3.3.0
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simPerformPathSearchStep");

    if (!App::userSettings->enableOldPathPlanningGui)
    {
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        if (!scr->checkAndSetWarning_oldPathPlanningFunctionality_oldCompatibility_11_2_2016())
            luaApiCallWarning(functionName.c_str(),"It is recommended to use the new path/motion planning functionality based on the OMPL plugin for V-REP.");
    }

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_bool,0))
    {
        int temporaryPathSearchObjectHandle=luaToInt(L,1);
        bool abortSearch=luaToBool(L,2);
        retVal=simPerformPathSearchStep_internal(temporaryPathSearchObjectHandle,abortSearch);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simFindMpPath(luaWrap_lua_State* L)
{ // DEPRECATED since 3.3.0
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simFindMpPath");

    if (!App::userSettings->enableOldMotionPlanningGui)
    {
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        if (!scr->checkAndSetWarning_oldMotionPlanningFunctionality_oldCompatibility_11_2_2016())
            luaApiCallWarning(functionName.c_str(),"It is recommended to use the new path/motion planning functionality based on the OMPL plugin for V-REP.");
    }

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,1,lua_arg_number,1,lua_arg_number,0,lua_arg_number,0))
    {
        int mpHandle=luaToInt(L,1);
        int options=luaToInt(L,4);
        float stepSize=luaToFloat(L,5);
        int maxTimeInMs=0;
        CMotionPlanningTask* it=App::ct->motionPlanning->getObject(mpHandle);
        if (it!=NULL)
        {
            int dof=it->getJointCount();
            // now check the args again, this time taking the table sizes into account
            if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,dof,lua_arg_number,dof,lua_arg_number,0,lua_arg_number,0))
            {
                std::vector<float> startConfig(dof,0.0f);
                std::vector<float> goalConfig(dof,0.0f);
                getFloatsFromTable(L,2,dof,&startConfig[0]);
                getFloatsFromTable(L,3,dof,&goalConfig[0]);

                int res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,true,true,&errorString);
                if (res>=0)
                {
                    if (res==2)
                        maxTimeInMs=luaToInt(L,6);
                    int configCount=0;
                    float* configs=simFindMpPath_internal(mpHandle,&startConfig[0],&goalConfig[0],options,stepSize,&configCount,maxTimeInMs,NULL,NULL,NULL);
                    if (configs!=NULL)
                    {
                        pushFloatTableOntoStack(L,dof*configCount,configs);
                        pushFloatTableOntoStack(L,configCount,configs+dof*configCount);
                        pushFloatTableOntoStack(L,configCount*3,configs+dof*configCount+configCount);
                        pushFloatTableOntoStack(L,configCount*4,configs+dof*configCount+configCount+configCount*3);
                        pushFloatTableOntoStack(L,configCount,configs+dof*configCount+configCount+configCount*3+configCount*4);
                        delete[] configs;
                        LUA_END(5);
                    }
                }
            }
        }
        else
            errorString=SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSimplifyMpPath(luaWrap_lua_State* L)
{ // DEPRECATED since 3.3.0
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSimplifyMpPath");

    if (!App::userSettings->enableOldMotionPlanningGui)
    {
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        if (!scr->checkAndSetWarning_oldMotionPlanningFunctionality_oldCompatibility_11_2_2016())
            luaApiCallWarning(functionName.c_str(),"It is recommended to use the new path/motion planning functionality based on the OMPL plugin for V-REP.");
    }

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int mpHandle=luaToInt(L,1);
        CMotionPlanningTask* it=App::ct->motionPlanning->getObject(mpHandle);
        if (it!=NULL)
        {
            int dof=it->getJointCount();
            if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,dof))
            {
                int configCnt=luaWrap_lua_objlen(L,2)/dof;
                if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,dof*configCnt,lua_arg_number,0,lua_arg_number,0))
                {
                    std::vector<float> configs(dof*configCnt,0.0f);
                    getFloatsFromTable(L,2,dof*configCnt,&configs[0]);
                    int options=luaToInt(L,3);
                    float stepSize=luaToFloat(L,4);
                    int increment=1;
                    int maxTimeInMs=0;
                    int res=checkOneGeneralInputArgument(L,5,lua_arg_number,0,true,true,&errorString);
                    if (res>=0)
                    {
                        if (res==2)
                            increment=luaToInt(L,5);
                        res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,true,true,&errorString);
                        if (res>=0)
                        {
                            if (res==2)
                                maxTimeInMs=luaToInt(L,6);
                            int outConfigCnt=0;

                            float* outConfigs=simSimplifyMpPath_internal(mpHandle,&configs[0],configCnt,options,stepSize,increment,&outConfigCnt,maxTimeInMs,NULL,NULL,NULL);
                            if (outConfigs!=NULL)
                            {
                                pushFloatTableOntoStack(L,dof*outConfigCnt,outConfigs);
                                pushFloatTableOntoStack(L,outConfigCnt,outConfigs+dof*outConfigCnt);
                                pushFloatTableOntoStack(L,outConfigCnt*3,outConfigs+dof*outConfigCnt+outConfigCnt);
                                pushFloatTableOntoStack(L,outConfigCnt*4,outConfigs+dof*outConfigCnt+outConfigCnt+outConfigCnt*3);
                                pushFloatTableOntoStack(L,outConfigCnt,outConfigs+dof*outConfigCnt+outConfigCnt+outConfigCnt*3+outConfigCnt*4);
                                delete[] outConfigs;
                                LUA_END(5);
                            }
                        }
                    }
                }
            }
        }
        else
            errorString=SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetMpConfigTransition(luaWrap_lua_State* L)
{ // DEPRECATED since 3.3.0
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetMpConfigTransition");

    if (!App::userSettings->enableOldMotionPlanningGui)
    {
        int currentScriptID=getCurrentScriptID(L);
        CLuaScriptObject* scr=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
        if (!scr->checkAndSetWarning_oldMotionPlanningFunctionality_oldCompatibility_11_2_2016())
            luaApiCallWarning(functionName.c_str(),"It is recommended to use the new path/motion planning functionality based on the OMPL plugin for V-REP.");
    }

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,1,lua_arg_number,1,lua_arg_number,0))
    {
        int mpHandle=luaToInt(L,1);
        int options=luaToInt(L,4);
        CMotionPlanningTask* it=App::ct->motionPlanning->getObject(mpHandle);
        if (it!=NULL)
        {
            int dof=it->getJointCount();
            // now check the args again, this time taking the table sizes into account
            if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,dof,lua_arg_number,dof,lua_arg_number,0))
            {
                std::vector<float> startConfig(dof,0.0f);
                std::vector<float> goalConfig(dof,0.0f);
                getFloatsFromTable(L,2,dof,&startConfig[0]);
                getFloatsFromTable(L,3,dof,&goalConfig[0]);

                int res=checkOneGeneralInputArgument(L,5,lua_arg_number,1,false,true,&errorString);
                if (res>=1)
                {
                    int* selectPtr=NULL;
                    std::vector<int> selectDat;
                    if (res==2)
                    {
                        int selectTableSize=int(luaWrap_lua_objlen(L,5));
                        selectDat.resize(selectTableSize+1,0);
                        selectDat[0]=selectTableSize/2;
                        getIntsFromTable(L,5,selectTableSize,&selectDat[1]);
                        selectPtr=&selectDat[0];
                    }

                    res=checkOneGeneralInputArgument(L,6,lua_arg_number,0,false,false,&errorString);
                    if (res==2)
                    {
                        res=checkOneGeneralInputArgument(L,7,lua_arg_number,0,false,false,&errorString);
                        if (res==2)
                        {
                            float stepSize1=luaToFloat(L,6);
                            float stepSize2=luaToFloat(L,7);
                            float* wayPoints=NULL;
                            std::vector<float> way;
                            res=checkOneGeneralInputArgument(L,8,lua_arg_number,14,true,true,&errorString);
                            if (res>=0)
                            {
                                int wayPointCnt=0;
                                if (res==2)
                                {
                                    int wpc=luaWrap_lua_objlen(L,8);
                                    wayPointCnt=wpc/7;
                                    way.resize(wayPointCnt*7,0.0f);
                                    getFloatsFromTable(L,8,wayPointCnt*7,&way[0]);
                                    wayPoints=&way[0];
                                }

                                int configCount=0;
                                float* configs=simGetMpConfigTransition_internal(mpHandle,&startConfig[0],&goalConfig[0],options,selectPtr,stepSize1,stepSize2,wayPointCnt,wayPoints,&configCount,NULL,NULL);
                                if (configs!=NULL)
                                {
                                    pushFloatTableOntoStack(L,dof*configCount,configs);
                                    pushFloatTableOntoStack(L,configCount,configs+dof*configCount);
                                    pushFloatTableOntoStack(L,configCount*3,configs+dof*configCount+configCount);
                                    pushFloatTableOntoStack(L,configCount*4,configs+dof*configCount+configCount+configCount*3);
                                    pushFloatTableOntoStack(L,configCount,configs+dof*configCount+configCount+configCount*3+configCount*4);
                                    delete[] configs;
                                    LUA_END(5);
                                }
                            }
                        }
                    }
                }
            }
        }
        else
            errorString=SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simEnableWorkThreads(luaWrap_lua_State* L)
{ // DEPRECATED since 3/4/2016
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simEnableWorkThreads");

    luaWrap_lua_pushnumber(L,0);
    LUA_END(1);
}

int _simWaitForWorkThreads(luaWrap_lua_State* L)
{ // DEPRECATED since 3/4/2016
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simWaitForWorkThreads");

    LUA_END(0);
}

int _simGetInvertedMatrix(luaWrap_lua_State* L)
{ // DEPRECATED since 10/5/2016
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetInvertedMatrix");

    if (checkInputArguments(L,&errorString,lua_arg_number,12))
    {
        float arr[12];
        getFloatsFromTable(L,1,12,arr);
        simInvertMatrix_internal(arr);
        pushFloatTableOntoStack(L,12,arr);
        LUA_END(1);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simAddSceneCustomData(luaWrap_lua_State* L)
{ // DEPRECATED since 26/12/2016
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddSceneCustomData");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_string,0))
    {
        int headerNumber=luaToInt(L,1);
        size_t dataLength;
        char* data=(char*)luaWrap_lua_tolstring(L,2,&dataLength);
        retVal=simAddSceneCustomData_internal(headerNumber,data,dataLength);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetSceneCustomData(luaWrap_lua_State* L)
{ // DEPRECATED since 26/12/2016
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetSceneCustomData");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int headerNumber=luaToInt(L,1);
        int dataLength=simGetSceneCustomDataLength_internal(headerNumber);
        if (dataLength>=0)
        {
            char* data=new char[dataLength];
            int retVal=simGetSceneCustomData_internal(headerNumber,data);
            if (retVal==-1)
                delete[] data;
            else
            {
                luaWrap_lua_pushlstring(L,data,dataLength);
                delete[] data;
                LUA_END(1);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}


int _simAddObjectCustomData(luaWrap_lua_State* L)
{ // DEPRECATED since 26/12/2016
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simAddObjectCustomData");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_string,0))
    {
        int objectHandle=luaToInt(L,1);
        int headerNumber=luaToInt(L,2);
        size_t dataLength;
        char* data=(char*)luaWrap_lua_tolstring(L,3,&dataLength);
        retVal=simAddObjectCustomData_internal(objectHandle,headerNumber,data,dataLength);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}


int _simGetObjectCustomData(luaWrap_lua_State* L)
{ // DEPRECATED since 26/12/2016
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetObjectCustomData");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int objectHandle=luaToInt(L,1);
        int headerNumber=luaToInt(L,2);
        int dataLength=simGetObjectCustomDataLength_internal(objectHandle,headerNumber);
        if (dataLength>=0)
        {
            char* data=new char[dataLength];
            int retVal=simGetObjectCustomData_internal(objectHandle,headerNumber,data);
            if (retVal==-1)
                delete[] data;
            else
            {
                luaWrap_lua_pushlstring(L,data,dataLength);
                delete[] data;
                LUA_END(1);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetUIHandle(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetUIHandle");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        std::string name(luaWrap_lua_tostring(L,1));
        if (suffixAdjustStringIfNeeded(functionName,true,L,name))
        {
            quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
            retVal=simGetUIHandle_internal(name.c_str());
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetUIProperty(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetUIProperty");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simGetUIProperty_internal(luaToInt(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetUIEventButton(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetUIEventButton");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int auxVals[2];
        retVal=simGetUIEventButton_internal(luaToInt(L,1),auxVals);
        luaWrap_lua_pushnumber(L,retVal);
        if (retVal==-1)
        {
            LUA_END(1);
        }
        pushIntTableOntoStack(L,2,auxVals);
        LUA_END(2);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetUIProperty(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetUIProperty");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetUIProperty_internal(luaToInt(L,1),luaToInt(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetUIButtonSize(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetUIButtonSize");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        int size[2];
        if (simGetUIButtonSize_internal(luaToInt(L,1),luaToInt(L,2),size)==1)
        {
            pushIntTableOntoStack(L,2,size);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simGetUIButtonProperty(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetUIButtonProperty");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simGetUIButtonProperty_internal(luaToInt(L,1),luaToInt(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetUIButtonProperty(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetUIButtonProperty");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetUIButtonProperty_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetUISlider(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetUISlider");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simGetUISlider_internal(luaToInt(L,1),luaToInt(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetUISlider(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetUISlider");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
        retVal=simSetUISlider_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetUIButtonLabel(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetUIButtonLabel");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        std::string stra;
        std::string strb;
        char* str1=NULL;
        char* str2=NULL;
        int res=checkOneGeneralInputArgument(L,3,lua_arg_string,0,false,true,&errorString);
        if (res!=-1)
        {
            if (res==2)
            {
                stra=luaWrap_lua_tostring(L,3);
                str1=(char*)stra.c_str();
            }
            res=checkOneGeneralInputArgument(L,4,lua_arg_string,0,true,true,&errorString);
            if (res!=-1)
            {
                if (res==2)
                {
                    strb=luaWrap_lua_tostring(L,4);
                    str2=(char*)strb.c_str();
                }
                retVal=simSetUIButtonLabel_internal(luaToInt(L,1),luaToInt(L,2),str1,str2);
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetUIButtonLabel(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetUIButtonLabel");

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        char* label=simGetUIButtonLabel_internal(luaToInt(L,1),luaToInt(L,2));
        if (label!=NULL)
        {
            luaWrap_lua_pushstring(L,label);
            simReleaseBuffer_internal(label);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simCreateUIButtonArray(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateUIButtonArray");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simCreateUIButtonArray_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetUIButtonArrayColor(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetUIButtonArrayColor");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0,lua_arg_number,2,lua_arg_number,3))
    {
        int pos[2];
        float col[3];
        getIntsFromTable(L,3,2,pos);
        getFloatsFromTable(L,4,3,col);
        retVal=simSetUIButtonArrayColor_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2),pos,col);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simDeleteUIButtonArray(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simDeleteUIButtonArray");
    int retVal=-1;// error

    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
        retVal=simDeleteUIButtonArray_internal(luaWrap_lua_tointeger(L,1),luaWrap_lua_tointeger(L,2));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreateUI(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateUI");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_string,0,lua_arg_number,0,lua_arg_number,2,lua_arg_number,2))
    {
        int clientSize[2];
        int cellSize[2];
        getIntsFromTable(L,3,2,clientSize);
        getIntsFromTable(L,4,2,cellSize);
        int menuAttributes=luaWrap_lua_tointeger(L,2);
        int b=0;
        for (int i=0;i<8;i++)
        {
            if (menuAttributes&(1<<i))
                b++;
        }
        int* buttonHandles=new int[b];
        menuAttributes=(menuAttributes|sim_ui_menu_systemblock)-sim_ui_menu_systemblock;
        retVal=simCreateUI_internal(luaWrap_lua_tostring(L,1),menuAttributes,clientSize,cellSize,buttonHandles);
        if (retVal!=-1)
        {
            luaWrap_lua_pushnumber(L,retVal);
            pushIntTableOntoStack(L,b,buttonHandles);
            delete[] buttonHandles;
            LUA_END(2);
        }
        delete[] buttonHandles;
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simCreateUIButton(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simCreateUIButton");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,2,lua_arg_number,2,lua_arg_number,0))
    {
        int pos[2];
        int size[2];
        getIntsFromTable(L,2,2,pos);
        getIntsFromTable(L,3,2,size);
        retVal=simCreateUIButton_internal(luaWrap_lua_tointeger(L,1),pos,size,luaWrap_lua_tointeger(L,4));
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSaveUI(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSaveUI");

    int retVal=-1;// error
    int res=checkOneGeneralInputArgument(L,1,lua_arg_table,1,false,true,&errorString);
    if (res>0)
    {
        int res2=checkOneGeneralInputArgument(L,2,lua_arg_string,0,false,false,&errorString);
        if (res2==2)
        {
            if (res==1)
                retVal=simSaveUI_internal(0,NULL,luaWrap_lua_tostring(L,2));
            else
            {
                int tl=int(luaWrap_lua_objlen(L,1));
                int* tble=new int[tl];
                getIntsFromTable(L,1,tl,tble);
                retVal=simSaveUI_internal(tl,tble,luaWrap_lua_tostring(L,2));
                delete[] tble;
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simLoadUI(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simLoadUI");

    if (checkInputArguments(L,&errorString,lua_arg_string,0))
    {
        int tble[1000];
        int res=simLoadUI_internal(luaWrap_lua_tostring(L,1),1000,tble);
        if (res>0)
        {
            pushIntTableOntoStack(L,res,tble);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simRemoveUI(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simRemoveUI");

    int retVal=-1;// error
    if (checkInputArguments(L,&errorString,lua_arg_number,0))
        retVal=simRemoveUI_internal(luaWrap_lua_tointeger(L,1));

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simGetUIPosition(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simGetUIPosition");

    if (checkInputArguments(L,&errorString,lua_arg_number,0))
    {
        int position[2];
        if (simGetUIPosition_internal(luaToInt(L,1),position)!=-1)
        {
            pushIntTableOntoStack(L,2,position);
            LUA_END(1);
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    LUA_END(0);
}

int _simSetUIPosition(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetUIPosition");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,2))
    {
        int position[2];
        getIntsFromTable(L,2,2,position);
        retVal=simSetUIPosition_internal(luaToInt(L,1),position);
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}

int _simSetUIButtonColor(luaWrap_lua_State* L)
{ // DEPRECATED since 09/02/2017
    LUA_API_FUNCTION_DEBUG;
    LUA_START("simSetUIButtonColor");

    int retVal=-1; // means error
    if (checkInputArguments(L,&errorString,lua_arg_number,0,lua_arg_number,0))
    {
        float col[9];
        float* acp[3]={col,col+3,col+6};
//      bool failed=true;
        int res=checkOneGeneralInputArgument(L,3,lua_arg_number,3,false,true,&errorString);
        if (res!=-1)
        {
            if (res!=2)
                acp[0]=NULL;
            res=checkOneGeneralInputArgument(L,4,lua_arg_number,3,true,true,&errorString);
            if (res!=-1)
            {
                if (res!=2)
                    acp[1]=NULL;
                res=checkOneGeneralInputArgument(L,5,lua_arg_number,3,true,true,&errorString);
                if (res!=-1)
                {
                    if (res!=2)
                        acp[2]=NULL;
                    for (int i=0;i<3;i++)
                    {
                        if (acp[i]!=NULL)
                            getFloatsFromTable(L,3+i,3,acp[i]);
                    }
                    retVal=simSetUIButtonColor_internal(luaToInt(L,1),luaToInt(L,2),acp[0],acp[1],acp[2]);
                }
            }
        }
    }

    LUA_SET_OR_RAISE_ERROR(); // we might never return from this!
    luaWrap_lua_pushnumber(L,retVal);
    LUA_END(1);
}
