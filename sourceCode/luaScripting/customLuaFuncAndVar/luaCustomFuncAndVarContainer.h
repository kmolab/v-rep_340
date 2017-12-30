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

#pragma once

#include "luaCustomFunction.h"
#include "luaCustomVariable.h"

class CLuaCustomFuncAndVarContainer
{
public:
    CLuaCustomFuncAndVarContainer();
    virtual ~CLuaCustomFuncAndVarContainer();

    void removeAllCustomFunctions();
    bool removeCustomFunction(const char* fullFunctionName);
    void announcePluginWasKilled(const char* pluginName);
    bool insertCustomFunction(CLuaCustomFunction* function);
    bool doesCustomFunctionAlreadyExist(CLuaCustomFunction* function);
    CLuaCustomFunction* getCustomFunctionFromID(int functionID);
    void appendAllFunctionNames_spaceSeparated(std::string& v);
    void registerCustomLuaFunctions(luaWrap_lua_State* L,luaWrap_lua_CFunction func);

    void outputWarningWithFunctionNamesWithoutPlugin(bool o);

    void removeAllCustomVariables();
    bool removeCustomVariable(const char* fullVariableName);
    bool insertCustomVariable(const char* fullVariableName,const char* variableValue,int stackHandle);
    void assignCustomVariables(luaWrap_lua_State* L);
    void appendAllVariableNames_spaceSeparated(std::string& v);
    void pushAllVariableNamesThatStartSame(const std::string& txt,std::vector<std::string>& v);


    std::vector<CLuaCustomFunction*> allCustomFunctions;
    std::vector<CLuaCustomVariable*> allCustomVariables;

    bool warningAboutFunctionNamesWithoutPlugin;
};
