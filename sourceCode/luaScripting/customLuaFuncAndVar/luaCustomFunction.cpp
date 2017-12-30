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
#include "luaCustomFunction.h"

CLuaCustomFunction::CLuaCustomFunction(const char* theFullFunctionName,const char* theCallTips,std::vector<int>& theInputArgTypes,void(*callBack)(struct SLuaCallBack* p))
{ // the old way, called through simRegisterCustomLuaFunction
    useStackToExchangeData=false;
    if (theFullFunctionName!=NULL)
    {
        functionName=_getFunctionNameFromFull(theFullFunctionName);
        pluginName=_getPluginNameFromFull(theFullFunctionName);
    }
    if (theCallTips!=NULL)
        callTips=theCallTips;
    for (int i=0;i<int(theInputArgTypes.size());i++)
        inputArgTypes.push_back(theInputArgTypes[i]);
    callBackFunction_old=callBack;
}

CLuaCustomFunction::CLuaCustomFunction(const char* theFullFunctionName,const char* theCallTips,void(*callBack)(struct SScriptCallBack* cb))
{ // the new way, called through simRegisterScriptCallbackFunction
    useStackToExchangeData=true;
    if (theFullFunctionName!=NULL)
    {
        functionName=_getFunctionNameFromFull(theFullFunctionName);
        pluginName=_getPluginNameFromFull(theFullFunctionName);
    }
    if (theCallTips!=NULL)
        callTips=theCallTips;
    callBackFunction_new=callBack;
}

CLuaCustomFunction::~CLuaCustomFunction()
{
}

void CLuaCustomFunction::registerCustomLuaFunction(luaWrap_lua_State* L,luaWrap_lua_CFunction func)
{
    luaWrap_lua_pushnumber(L,functionID+1);
    luaWrap_lua_pushcclosure(L,func,1);
    luaWrap_lua_setfield(L,luaWrapGet_LUA_GLOBALSINDEX(),functionName.c_str());
/* removed on 31/5/2016
#ifdef LIN_VREP
    luaWrap_lua_setglobal(L,functionName.c_str());
#else
    luaWrap_lua_setfield(L,luaWrapGet_LUA_GLOBALSINDEX(),functionName.c_str());
#endif
*/
}

bool CLuaCustomFunction::getUsesStackToExchangeData()
{
    return(useStackToExchangeData);
}

bool CLuaCustomFunction::isFunctionNameSame(const char* fullName)
{
    std::string funcName(_getFunctionNameFromFull(fullName));
    return(functionName.compare(funcName)==0);
}

std::string CLuaCustomFunction::getFunctionName()
{
    return(functionName);
}

std::string CLuaCustomFunction::getPluginName()
{
    return(pluginName);
}

std::string CLuaCustomFunction::getCallTips()
{
    return(callTips);
}

int CLuaCustomFunction::getFunctionID()
{
    return(functionID);
}

void CLuaCustomFunction::setFunctionID(int newID)
{
    functionID=newID;
}

std::string CLuaCustomFunction::_getFunctionNameFromFull(const char* fullName)
{
    std::string f(fullName);
    size_t p=f.find('@');
    if (p==std::string::npos)
        return(f);
    std::string subF(f.begin(),f.begin()+p);
    return(subF);
}

std::string CLuaCustomFunction::_getPluginNameFromFull(const char* fullName)
{
    std::string f(fullName);
    size_t p=f.find('@');
    if (p==std::string::npos)
        return("");
    std::string subF(f.begin()+p+1,f.end());
    return(subF);
}
