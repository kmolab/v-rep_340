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

#include "vrepMainHeader.h"
#include "luaWrapper.h"

class CLuaCustomFunction
{
public:
    // the old way:
    CLuaCustomFunction(const char* theFullFunctionName,const char* theCallTips,std::vector<int>& theInputArgTypes,void(*callBack)(struct SLuaCallBack* p));
    // the new way:
    CLuaCustomFunction(const char* theFullFunctionName,const char* theCallTips,void(*callBack)(struct SScriptCallBack* cb));

    virtual ~CLuaCustomFunction();
    
    void registerCustomLuaFunction(luaWrap_lua_State* L,luaWrap_lua_CFunction func);
    bool getUsesStackToExchangeData();
    std::string getFunctionName();
    bool isFunctionNameSame(const char* fullName);
    std::string getPluginName();
    std::string getCallTips();
    int getFunctionID();
    void setFunctionID(int newID);
    static std::string _getFunctionNameFromFull(const char* fullName);
    static std::string _getPluginNameFromFull(const char* fullName);

    void(*callBackFunction_new)(struct SScriptCallBack* p);
    void(*callBackFunction_old)(struct SLuaCallBack* p);
    std::vector<int> inputArgTypes;

protected:
    std::string pluginName;
    std::string functionName;
    std::string callTips;
    int functionID;
    bool useStackToExchangeData;
};
