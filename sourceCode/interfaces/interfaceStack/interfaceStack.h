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

#include "interfaceStackObject.h"
#include "interfaceStackTable.h"
#include <vector>
#include <string>

class CInterfaceStack
{
public:
    CInterfaceStack();
    virtual ~CInterfaceStack();

    void setId(int id);
    int getId();
    void clear();

    // Lua interface (creation):
    void buildFromLuaStack(luaWrap_lua_State* L,int fromPos=1,int cnt=0);
    // Lua interface (read-out):
    void buildOntoLuaStack(luaWrap_lua_State* L,bool takeOnlyTop);
    // C interface (creation):
    CInterfaceStack* copyYourself();
    void pushNullOntoStack();
    void pushBoolOntoStack(bool v);
    void pushNumberOntoStack(double v);
    void pushStringOntoStack(const char* str,int l);
    void pushUCharArrayTableOntoStack(const unsigned char* arr,int l);
    void pushIntArrayTableOntoStack(const int* arr,int l);
    void pushFloatArrayTableOntoStack(const float* arr,int l);
    void pushDoubleArrayTableOntoStack(const double* arr,int l);
    void pushTableOntoStack();
    bool insertDataIntoStackTable();
    bool pushTableFromBuffer(const char* data,unsigned int l);


    // C interface (read-out)
    int getStackSize();
    void popStackValue(int cnt);
    bool moveStackItemToTop(int cIndex);
    bool getStackBoolValue(bool& theValue);
    bool getStackNumberValue(double& theValue);
    bool getStackStringValue(std::string& theValue);
    bool isStackValueNull();
    int getStackTableInfo(int infoType);
    bool getStackUCharArray(unsigned char* array,int count);
    bool getStackIntArray(int* array,int count);
    bool getStackFloatArray(float* array,int count);
    bool getStackDoubleArray(double* array,int count);
    bool unfoldStackTable();
    std::string getBufferFromTable();

    void printContent(int cIndex);

protected:
    CInterfaceStackObject* _generateObjectFromLuaStack(luaWrap_lua_State* L,int index);
    CInterfaceStackTable* _generateTableArrayFromLuaStack(luaWrap_lua_State* L,int index);
    CInterfaceStackTable* _generateTableMapFromLuaStack(luaWrap_lua_State* L,int index);
    int _countLuaStackTableEntries(luaWrap_lua_State* L,int index);

    void _pushOntoLuaStack(luaWrap_lua_State* L,CInterfaceStackObject* obj);

    int _interfaceStackId;
    std::vector<CInterfaceStackObject*> _stackObjects;
};
