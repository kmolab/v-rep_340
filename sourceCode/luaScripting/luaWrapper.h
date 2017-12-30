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

enum {  STACK_OBJECT_NULL=0,
        STACK_OBJECT_NUMBER,
        STACK_OBJECT_BOOL,
        STACK_OBJECT_STRING,
        STACK_OBJECT_TABLE,
        STACK_OBJECT_FUNC,
        STACK_OBJECT_USERDAT,
        STACK_OBJECT_THREAD,
        STACK_OBJECT_LIGHTUSERDAT
};

typedef void luaWrap_lua_State;
typedef void luaWrap_lua_Debug;
typedef double luaWrap_lua_Number;
typedef void (*luaWrap_lua_Hook) (luaWrap_lua_State* L,luaWrap_lua_Debug* ar);
typedef int (*luaWrap_lua_CFunction) (luaWrap_lua_State* L);

void loadExtLuaLibrary(bool useExternalLuaLibrary,bool headless);
void unloadExtLuaLibrary();

bool _loadExtLuaLibrary(const char* pathAndFilename);

int luaWrapGet_LUA_MULTRET();
int luaWrapGet_LUA_MASKCOUNT();
int luaWrapGet_LUA_GLOBALSINDEX();

luaWrap_lua_State* luaWrap_luaL_newstate();
void luaWrap_lua_close(luaWrap_lua_State* L);
void luaWrap_luaL_openlibs(luaWrap_lua_State* L);
int luaWrap_lua_sethook(luaWrap_lua_State* L,luaWrap_lua_Hook func,int mask,int cnt);
void luaWrap_lua_register(luaWrap_lua_State* L,const char* name,luaWrap_lua_CFunction func);
void luaWrap_lua_pushnumber(luaWrap_lua_State* L,luaWrap_lua_Number n);
void luaWrap_lua_pushnil(luaWrap_lua_State* L);
void luaWrap_lua_pushboolean(luaWrap_lua_State* L,int b);
void luaWrap_lua_pushinteger(luaWrap_lua_State* L,int n);
void luaWrap_lua_pushstring(luaWrap_lua_State* L,const char* str);
void luaWrap_lua_pushlstring(luaWrap_lua_State* L,const char* str,size_t l);
void luaWrap_lua_pushcclosure(luaWrap_lua_State* L,luaWrap_lua_CFunction func,int n);
void luaWrap_lua_pushvalue(luaWrap_lua_State* L,int idx);
int luaWrap_lua_tointeger(luaWrap_lua_State* L,int idx);
luaWrap_lua_Number luaWrap_lua_tonumber(luaWrap_lua_State* L,int idx);
int luaWrap_lua_toboolean(luaWrap_lua_State* L,int idx);
const char* luaWrap_lua_tostring(luaWrap_lua_State* L,int idx);
const char* luaWrap_lua_tolstring(luaWrap_lua_State* L,int idx,size_t* len);
int luaWrap_lua_isnumber(luaWrap_lua_State* L,int idx);
int luaWrap_lua_isstring(luaWrap_lua_State* L,int idx);
bool luaWrap_lua_isnil(luaWrap_lua_State* L,int idx);
bool luaWrap_lua_isboolean(luaWrap_lua_State* L,int idx);
bool luaWrap_lua_istable(luaWrap_lua_State* L,int idx);
bool luaWrap_lua_isfunction(luaWrap_lua_State* L,int idx);
int luaWrap_lua_getstack(luaWrap_lua_State* L,int level,luaWrap_lua_Debug* deb);
int luaWrap_lua_getinfo(luaWrap_lua_State* L,const char* what,luaWrap_lua_Debug* deb);
void luaWrap_lua_setglobal(luaWrap_lua_State* L,const char* name);
void luaWrap_lua_getfield(luaWrap_lua_State* L,int idx,const char* name);
void luaWrap_lua_setfield(luaWrap_lua_State* L,int idx,const char* name);
void luaWrap_lua_yield(luaWrap_lua_State* L,int nresults);
int luaWrap_luaL_dostring(luaWrap_lua_State* L,const char* str);
void luaWrap_lua_getglobal(luaWrap_lua_State* L,const char* str);
void luaWrap_lua_pop(luaWrap_lua_State* L,int n);
int luaWrap_lua_gettop(luaWrap_lua_State* L);
void luaWrap_lua_settop(luaWrap_lua_State* L,int idx);
size_t luaWrap_lua_objlen(luaWrap_lua_State* L,int idx);
void luaWrap_lua_rawgeti(luaWrap_lua_State* L,int idx,int n);
void luaWrap_lua_rawseti(luaWrap_lua_State* L,int idx,int n);
void luaWrap_lua_newtable(luaWrap_lua_State* L);
int luaWrap_luaL_loadbuffer(luaWrap_lua_State* L,const char* buff,size_t sz,const char* name);
int luaWrap_lua_pcall(luaWrap_lua_State* L,int nargs,int nresult,int errfunc);
void luaWrap_lua_remove(luaWrap_lua_State* L,int idx);
void luaWrap_lua_insert(luaWrap_lua_State* L,int idx);
int luaWrap_lua_isuserdata(luaWrap_lua_State* L,int idx);
int luaWrap_lua_upvalueindex(int i);
int luaWrap_getCurrentCodeLine(luaWrap_lua_State* L);
void luaWrap_lua_settable(luaWrap_lua_State* L,int idx);
int luaWrap_lua_next(luaWrap_lua_State* L,int idx);
int luaWrap_lua_stype(luaWrap_lua_State* L,int idx);
int luaWrap_lua_error(luaWrap_lua_State* L);

