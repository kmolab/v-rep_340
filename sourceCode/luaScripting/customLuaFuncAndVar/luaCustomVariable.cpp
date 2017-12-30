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
#include "luaCustomVariable.h"
#include "app.h"

CLuaCustomVariable::CLuaCustomVariable(const char* theFullVariableName,const char* theVariableValue,int theVariableStackValue)
{
    _variableName=_getVariableNameFromFull(theFullVariableName);
    _pluginName=_getPluginNameFromFull(theFullVariableName);

    if (theVariableValue!=NULL)
    { // we register a simple variable
        _variableValue=theVariableValue;
        _variableStackValue=0; // i.e. not using the stack
    }
    else
    {
        _variableValue="";
        _variableStackValue=theVariableStackValue;
    }
}

CLuaCustomVariable::~CLuaCustomVariable()
{
    if (_variableStackValue!=0)
        App::ct->interfaceStackContainer->destroyStack(_variableStackValue);
}

void CLuaCustomVariable::pushVariableOntoLuaStack(luaWrap_lua_State* L)
{
    if (_variableStackValue==0)
    { // simple variable
        std::string tmp(_variableName);
        tmp+="="+_variableValue;
        luaWrap_luaL_dostring(L,tmp.c_str());
    }
    else
    { // stack variable
        CInterfaceStack* stack=App::ct->interfaceStackContainer->getStack(_variableStackValue);
        stack->buildOntoLuaStack(L,true);
        luaWrap_lua_setglobal(L,_variableName.c_str());
    }
}

bool CLuaCustomVariable::isVariableNameSame(const char* fullName)
{
    std::string varName(_getVariableNameFromFull(fullName));
    return(_variableName.compare(varName)==0);
}

bool CLuaCustomVariable::shouldBeDestroyed(const char* pluginName)
{
    return(_pluginName.compare(pluginName)==0);
}

std::string CLuaCustomVariable::getVariableName()
{
    return(_variableName);
}

std::string CLuaCustomVariable::_getVariableNameFromFull(const char* fullName)
{
    std::string f(fullName);
    size_t p=f.find('@');
    if (p==std::string::npos)
        return(f);
    std::string subF(f.begin(),f.begin()+p);
    return(subF);
}

std::string CLuaCustomVariable::_getPluginNameFromFull(const char* fullName)
{
    std::string f(fullName);
    size_t p=f.find('@');
    if (p==std::string::npos)
        return("");
    std::string subF(f.begin()+p+1,f.end());
    return(subF);
}
