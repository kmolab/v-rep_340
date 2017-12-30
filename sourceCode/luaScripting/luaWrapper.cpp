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

#include "luaWrapper.h"
#include "vVarious.h"
#include "app.h"

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}

typedef int (__cdecl *pluaLibGet_LUA_MULTRET)(void);
typedef int (__cdecl *pluaLibGet_LUA_MASKCOUNT)(void);
typedef int (__cdecl *pluaLibGet_LUA_GLOBALSINDEX)(void);
typedef luaWrap_lua_State* (__cdecl *pluaLib_luaL_newstate)(void);
typedef void (__cdecl *pluaLib_lua_close)(luaWrap_lua_State* L);
typedef void (__cdecl *pluaLib_luaL_openlibs)(luaWrap_lua_State* L);
typedef int (__cdecl *pluaLib_lua_sethook)(luaWrap_lua_State* L,luaWrap_lua_Hook func,int mask,int cnt);
typedef void (__cdecl *pluaLib_lua_register)(luaWrap_lua_State* L,const char* name,luaWrap_lua_CFunction func);
typedef void (__cdecl *pluaLib_lua_pushnumber)(luaWrap_lua_State* L,luaWrap_lua_Number n);
typedef void (__cdecl *pluaLib_lua_pushnil)(luaWrap_lua_State* L);
typedef void (__cdecl *pluaLib_lua_pushboolean)(luaWrap_lua_State* L,int b);
typedef void (__cdecl *pluaLib_lua_pushinteger)(luaWrap_lua_State* L,int n);
typedef void (__cdecl *pluaLib_lua_pushstring)(luaWrap_lua_State* L,const char* str);
typedef void (__cdecl *pluaLib_lua_pushlstring)(luaWrap_lua_State* L,const char* str,size_t l);
typedef void (__cdecl *pluaLib_lua_pushcclosure)(luaWrap_lua_State* L,luaWrap_lua_CFunction func,int n);
typedef void (__cdecl *pluaLib_lua_pushvalue)(luaWrap_lua_State* L,int idx);
typedef int (__cdecl *pluaLib_lua_tointeger)(luaWrap_lua_State* L,int idx);
typedef luaWrap_lua_Number (__cdecl *pluaLib_lua_tonumber)(luaWrap_lua_State* L,int idx);
typedef int (__cdecl *pluaLib_lua_toboolean)(luaWrap_lua_State* L,int idx);
typedef const char* (__cdecl *pluaLib_lua_tostring)(luaWrap_lua_State* L,int idx);
typedef const char* (__cdecl *pluaLib_lua_tolstring)(luaWrap_lua_State* L,int idx,size_t* len);
typedef int (__cdecl *pluaLib_lua_isnumber)(luaWrap_lua_State* L,int idx);
typedef int (__cdecl *pluaLib_lua_isstring)(luaWrap_lua_State* L,int idx);
typedef bool (__cdecl *pluaLib_lua_isnil)(luaWrap_lua_State* L,int idx);
typedef bool (__cdecl *pluaLib_lua_isboolean)(luaWrap_lua_State* L,int idx);
typedef bool (__cdecl *pluaLib_lua_istable)(luaWrap_lua_State* L,int idx);
typedef bool (__cdecl *pluaLib_lua_isfunction)(luaWrap_lua_State* L,int idx);
typedef int (__cdecl *pluaLib_lua_getstack)(luaWrap_lua_State* L,int level,luaWrap_lua_Debug* deb);
typedef int (__cdecl *pluaLib_lua_getinfo)(luaWrap_lua_State* L,const char* what,luaWrap_lua_Debug* deb);
typedef void (__cdecl *pluaLib_lua_setglobal)(luaWrap_lua_State* L,const char* name);
typedef void (__cdecl *pluaLib_lua_getfield)(luaWrap_lua_State* L,int idx,const char* name);
typedef void (__cdecl *pluaLib_lua_setfield)(luaWrap_lua_State* L,int idx,const char* name);
typedef void (__cdecl *pluaLib_lua_yield)(luaWrap_lua_State* L,int nresults);
typedef int (__cdecl *pluaLib_luaL_dostring)(luaWrap_lua_State* L,const char* str);
typedef void (__cdecl *pluaLib_lua_getglobal)(luaWrap_lua_State* L,const char* str);
typedef void (__cdecl *pluaLib_lua_pop)(luaWrap_lua_State* L,int n);
typedef int (__cdecl *pluaLib_lua_gettop)(luaWrap_lua_State* L);
typedef void (__cdecl *pluaLib_lua_settop)(luaWrap_lua_State* L,int idx);
typedef size_t (__cdecl *pluaLib_lua_objlen)(luaWrap_lua_State* L,int idx);
typedef void (__cdecl *pluaLib_lua_rawgeti)(luaWrap_lua_State* L,int idx,int n);
typedef void (__cdecl *pluaLib_lua_rawseti)(luaWrap_lua_State* L,int idx,int n);
typedef void (__cdecl *pluaLib_lua_newtable)(luaWrap_lua_State* L);
typedef int (__cdecl *pluaLib_luaL_loadbuffer)(luaWrap_lua_State* L,const char* buff,size_t sz,const char* name);
typedef int (__cdecl *pluaLib_lua_pcall)(luaWrap_lua_State* L,int nargs,int nresult,int errfunc);
typedef void (__cdecl *pluaLib_lua_remove)(luaWrap_lua_State* L,int idx);
typedef void (__cdecl *pluaLib_lua_insert)(luaWrap_lua_State* L,int idx);
typedef int (__cdecl *pluaLib_lua_isuserdata)(luaWrap_lua_State* L,int idx);
typedef int (__cdecl *pluaLib_lua_upvalueindex)(int i);
typedef int (__cdecl *pluaLib_getCurrentCodeLine)(luaWrap_lua_State* L);
typedef void (__cdecl *pluaLib_lua_settable)(luaWrap_lua_State* L,int idx);
typedef int (__cdecl *pluaLib_lua_next)(luaWrap_lua_State* L,int idx);
typedef int (__cdecl *pluaLib_lua_type)(luaWrap_lua_State* L,int idx);
typedef int (__cdecl *pluaLib_lua_error)(luaWrap_lua_State* L);


pluaLibGet_LUA_MULTRET luaLibGet_LUA_MULTRET;
pluaLibGet_LUA_MASKCOUNT luaLibGet_LUA_MASKCOUNT;
pluaLibGet_LUA_GLOBALSINDEX luaLibGet_LUA_GLOBALSINDEX;
pluaLib_luaL_newstate luaLib_luaL_newstate;
pluaLib_lua_close luaLib_lua_close;
pluaLib_luaL_openlibs luaLib_luaL_openlibs;
pluaLib_lua_sethook luaLib_lua_sethook;
pluaLib_lua_register luaLib_lua_register;
pluaLib_lua_pushnumber luaLib_lua_pushnumber;
pluaLib_lua_pushnil luaLib_lua_pushnil;
pluaLib_lua_pushboolean luaLib_lua_pushboolean;
pluaLib_lua_pushinteger luaLib_lua_pushinteger;
pluaLib_lua_pushstring luaLib_lua_pushstring;
pluaLib_lua_pushlstring luaLib_lua_pushlstring;
pluaLib_lua_pushcclosure luaLib_lua_pushcclosure;
pluaLib_lua_pushvalue luaLib_lua_pushvalue;
pluaLib_lua_tointeger luaLib_lua_tointeger;
pluaLib_lua_tonumber luaLib_lua_tonumber;
pluaLib_lua_toboolean luaLib_lua_toboolean;
pluaLib_lua_tostring luaLib_lua_tostring;
pluaLib_lua_tolstring luaLib_lua_tolstring;
pluaLib_lua_isnumber luaLib_lua_isnumber;
pluaLib_lua_isstring luaLib_lua_isstring;
pluaLib_lua_isnil luaLib_lua_isnil;
pluaLib_lua_isboolean luaLib_lua_isboolean;
pluaLib_lua_istable luaLib_lua_istable;
pluaLib_lua_isfunction luaLib_lua_isfunction;
pluaLib_lua_getstack luaLib_lua_getstack;
pluaLib_lua_getinfo luaLib_lua_getinfo;
pluaLib_lua_setglobal luaLib_lua_setglobal;
pluaLib_lua_getfield luaLib_lua_getfield;
pluaLib_lua_setfield luaLib_lua_setfield;
pluaLib_lua_yield luaLib_lua_yield;
pluaLib_luaL_dostring luaLib_luaL_dostring;
pluaLib_lua_getglobal luaLib_lua_getglobal;
pluaLib_lua_pop luaLib_lua_pop;
pluaLib_lua_gettop luaLib_lua_gettop;
pluaLib_lua_settop luaLib_lua_settop;
pluaLib_lua_objlen luaLib_lua_objlen;
pluaLib_lua_rawgeti luaLib_lua_rawgeti;
pluaLib_lua_rawseti luaLib_lua_rawseti;
pluaLib_lua_newtable luaLib_lua_newtable;
pluaLib_luaL_loadbuffer luaLib_luaL_loadbuffer;
pluaLib_lua_pcall luaLib_lua_pcall;
pluaLib_lua_remove luaLib_lua_remove;
pluaLib_lua_insert luaLib_lua_insert;
pluaLib_lua_isuserdata luaLib_lua_isuserdata;
pluaLib_lua_upvalueindex luaLib_lua_upvalueindex;
pluaLib_getCurrentCodeLine luaLib_getCurrentCodeLine;
pluaLib_lua_settable luaLib_lua_settable;
pluaLib_lua_next luaLib_lua_next;
pluaLib_lua_type luaLib_lua_type;
pluaLib_lua_error luaLib_lua_error;

WLibrary lib;

WLibraryFunc _getProcAddress(const char* funcName)
{
    return(VVarious::resolveLibraryFuncName(lib,funcName));
}

bool _getLibProcAddresses()
{
    luaLibGet_LUA_MULTRET=(pluaLibGet_LUA_MULTRET)(_getProcAddress("luaLibGet_LUA_MULTRET"));
    luaLibGet_LUA_MASKCOUNT=(pluaLibGet_LUA_MASKCOUNT)(_getProcAddress("luaLibGet_LUA_MASKCOUNT"));
    luaLibGet_LUA_GLOBALSINDEX=(pluaLibGet_LUA_GLOBALSINDEX)(_getProcAddress("luaLibGet_LUA_GLOBALSINDEX"));
    luaLib_luaL_newstate=(pluaLib_luaL_newstate)(_getProcAddress("luaLib_luaL_newstate"));
    luaLib_lua_close=(pluaLib_lua_close)(_getProcAddress("luaLib_lua_close"));
    luaLib_luaL_openlibs=(pluaLib_luaL_openlibs)(_getProcAddress("luaLib_luaL_openlibs"));
    luaLib_lua_sethook=(pluaLib_lua_sethook)(_getProcAddress("luaLib_lua_sethook"));
    luaLib_lua_register=(pluaLib_lua_register)(_getProcAddress("luaLib_lua_register"));
    luaLib_lua_pushnumber=(pluaLib_lua_pushnumber)(_getProcAddress("luaLib_lua_pushnumber"));
    luaLib_lua_pushnil=(pluaLib_lua_pushnil)(_getProcAddress("luaLib_lua_pushnil"));
    luaLib_lua_pushboolean=(pluaLib_lua_pushboolean)(_getProcAddress("luaLib_lua_pushboolean"));
    luaLib_lua_pushinteger=(pluaLib_lua_pushinteger)(_getProcAddress("luaLib_lua_pushinteger"));
    luaLib_lua_pushstring=(pluaLib_lua_pushstring)(_getProcAddress("luaLib_lua_pushstring"));
    luaLib_lua_pushlstring=(pluaLib_lua_pushlstring)(_getProcAddress("luaLib_lua_pushlstring"));
    luaLib_lua_pushcclosure=(pluaLib_lua_pushcclosure)(_getProcAddress("luaLib_lua_pushcclosure"));
    luaLib_lua_pushvalue=(pluaLib_lua_pushvalue)(_getProcAddress("luaLib_lua_pushvalue"));
    luaLib_lua_tointeger=(pluaLib_lua_tointeger)(_getProcAddress("luaLib_lua_tointeger"));
    luaLib_lua_tonumber=(pluaLib_lua_tonumber)(_getProcAddress("luaLib_lua_tonumber"));
    luaLib_lua_toboolean=(pluaLib_lua_toboolean)(_getProcAddress("luaLib_lua_toboolean"));
    luaLib_lua_tostring=(pluaLib_lua_tostring)(_getProcAddress("luaLib_lua_tostring"));
    luaLib_lua_tolstring=(pluaLib_lua_tolstring)(_getProcAddress("luaLib_lua_tolstring"));
    luaLib_lua_isnumber=(pluaLib_lua_isnumber)(_getProcAddress("luaLib_lua_isnumber"));
    luaLib_lua_isstring=(pluaLib_lua_isstring)(_getProcAddress("luaLib_lua_isstring"));
    luaLib_lua_isnil=(pluaLib_lua_isnil)(_getProcAddress("luaLib_lua_isnil"));
    luaLib_lua_isboolean=(pluaLib_lua_isboolean)(_getProcAddress("luaLib_lua_isboolean"));
    luaLib_lua_istable=(pluaLib_lua_istable)(_getProcAddress("luaLib_lua_istable"));
    luaLib_lua_isfunction=(pluaLib_lua_isfunction)(_getProcAddress("luaLib_lua_isfunction"));
    luaLib_lua_getstack=(pluaLib_lua_getstack)(_getProcAddress("luaLib_lua_getstack"));
    luaLib_lua_getinfo=(pluaLib_lua_getinfo)(_getProcAddress("luaLib_lua_getinfo"));
    luaLib_lua_setglobal=(pluaLib_lua_setglobal)(_getProcAddress("luaLib_lua_setglobal"));
    luaLib_lua_getfield=(pluaLib_lua_getfield)(_getProcAddress("luaLib_lua_getfield"));
    luaLib_lua_setfield=(pluaLib_lua_setfield)(_getProcAddress("luaLib_lua_setfield"));
    luaLib_lua_yield=(pluaLib_lua_yield)(_getProcAddress("luaLib_lua_yield"));
    luaLib_luaL_dostring=(pluaLib_luaL_dostring)(_getProcAddress("luaLib_luaL_dostring"));
    luaLib_lua_getglobal=(pluaLib_lua_getglobal)(_getProcAddress("luaLib_lua_getglobal"));
    luaLib_lua_pop=(pluaLib_lua_pop)(_getProcAddress("luaLib_lua_pop"));
    luaLib_lua_gettop=(pluaLib_lua_gettop)(_getProcAddress("luaLib_lua_gettop"));
    luaLib_lua_settop=(pluaLib_lua_settop)(_getProcAddress("luaLib_lua_settop"));
    luaLib_lua_objlen=(pluaLib_lua_objlen)(_getProcAddress("luaLib_lua_objlen"));
    luaLib_lua_rawgeti=(pluaLib_lua_rawgeti)(_getProcAddress("luaLib_lua_rawgeti"));
    luaLib_lua_rawseti=(pluaLib_lua_rawseti)(_getProcAddress("luaLib_lua_rawseti"));
    luaLib_lua_newtable=(pluaLib_lua_newtable)(_getProcAddress("luaLib_lua_newtable"));
    luaLib_luaL_loadbuffer=(pluaLib_luaL_loadbuffer)(_getProcAddress("luaLib_luaL_loadbuffer"));
    luaLib_lua_pcall=(pluaLib_lua_pcall)(_getProcAddress("luaLib_lua_pcall"));
    luaLib_lua_remove=(pluaLib_lua_remove)(_getProcAddress("luaLib_lua_remove"));
    luaLib_lua_insert=(pluaLib_lua_insert)(_getProcAddress("luaLib_lua_insert"));
    luaLib_lua_isuserdata=(pluaLib_lua_isuserdata)(_getProcAddress("luaLib_lua_isuserdata"));
    luaLib_lua_upvalueindex=(pluaLib_lua_upvalueindex)(_getProcAddress("luaLib_lua_upvalueindex"));
    luaLib_getCurrentCodeLine=(pluaLib_getCurrentCodeLine)(_getProcAddress("luaLib_getCurrentCodeLine"));
    luaLib_lua_settable=(pluaLib_lua_settable)(_getProcAddress("luaLib_lua_settable"));
    luaLib_lua_next=(pluaLib_lua_next)(_getProcAddress("luaLib_lua_next"));
    luaLib_lua_type=(pluaLib_lua_type)(_getProcAddress("luaLib_lua_type"));
    luaLib_lua_error=(pluaLib_lua_error)(_getProcAddress("luaLib_lua_error"));


    if (luaLibGet_LUA_MULTRET==NULL) return false;
    if (luaLibGet_LUA_MASKCOUNT==NULL) return false;
    if (luaLibGet_LUA_GLOBALSINDEX==NULL) return false;
    if (luaLib_luaL_newstate==NULL) return false;
    if (luaLib_lua_close==NULL) return false;
    if (luaLib_luaL_openlibs==NULL) return false;
    if (luaLib_lua_sethook==NULL) return false;
    if (luaLib_lua_register==NULL) return false;
    if (luaLib_lua_pushnumber==NULL) return false;
    if (luaLib_lua_pushnil==NULL) return false;
    if (luaLib_lua_pushboolean==NULL) return false;
    if (luaLib_lua_pushinteger==NULL) return false;
    if (luaLib_lua_pushstring==NULL) return false;
    if (luaLib_lua_pushlstring==NULL) return false;
    if (luaLib_lua_pushcclosure==NULL) return false;
    if (luaLib_lua_pushvalue==NULL) return false;
    if (luaLib_lua_tointeger==NULL) return false;
    if (luaLib_lua_tonumber==NULL) return false;
    if (luaLib_lua_toboolean==NULL) return false;
    if (luaLib_lua_tostring==NULL) return false;
    if (luaLib_lua_tolstring==NULL) return false;
    if (luaLib_lua_isnumber==NULL) return false;
    if (luaLib_lua_isstring==NULL) return false;
    if (luaLib_lua_isnil==NULL) return false;
    if (luaLib_lua_isboolean==NULL) return false;
    if (luaLib_lua_istable==NULL) return false;
    if (luaLib_lua_isfunction==NULL) return false;
    if (luaLib_lua_getstack==NULL) return false;
    if (luaLib_lua_getinfo==NULL) return false;
    if (luaLib_lua_setglobal==NULL) return false;
    if (luaLib_lua_getfield==NULL) return false;
    if (luaLib_lua_setfield==NULL) return false;
    if (luaLib_lua_yield==NULL) return false;
    if (luaLib_luaL_dostring==NULL) return false;
    if (luaLib_lua_getglobal==NULL) return false;
    if (luaLib_lua_pop==NULL) return false;
    if (luaLib_lua_gettop==NULL) return false;
    if (luaLib_lua_settop==NULL) return false;
    if (luaLib_lua_objlen==NULL) return false;
    if (luaLib_lua_rawgeti==NULL) return false;
    if (luaLib_lua_rawseti==NULL) return false;
    if (luaLib_lua_newtable==NULL) return false;
    if (luaLib_luaL_loadbuffer==NULL) return false;
    if (luaLib_lua_pcall==NULL) return false;
    if (luaLib_lua_remove==NULL) return false;
    if (luaLib_lua_insert==NULL) return false;
    if (luaLib_lua_isuserdata==NULL) return false;
    if (luaLib_lua_upvalueindex==NULL) return false;
    if (luaLib_getCurrentCodeLine==NULL) return false;
    if (luaLib_lua_settable==NULL) return false;
    if (luaLib_lua_next==NULL) return false;
    if (luaLib_lua_type==NULL) return false;
    if (luaLib_lua_error==NULL) return false;

    return true;
}

void loadExtLuaLibrary(bool useExternalLuaLibrary,bool headless)
{
    std::string luaLibPathAndName(VVarious::getModulePath());
    luaLibPathAndName+=VREP_SLASH;
#ifdef WIN_VREP
    luaLibPathAndName+="v_repLua.dll";
#endif
#ifdef MAC_VREP
    luaLibPathAndName+="libv_repLua.dylib";
#endif
#ifdef LIN_VREP
    luaLibPathAndName+="libv_repLua.so";
#endif
    if (!useExternalLuaLibrary)
        luaLibPathAndName="";
    if (!_loadExtLuaLibrary(luaLibPathAndName.c_str()))
    {
        if (!headless)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=DISPLAY_MESSAGE_CMD;
            cmd.intParams.push_back(sim_msgbox_type_info);
            cmd.stringParams.push_back("External Lua library");
            cmd.stringParams.push_back("Could not find or correctly load the external Lua library.\nReverting to the default library.");
            App::appendSimulationThreadCommand(cmd,5000);
        }
        printf("Could not find or correctly load the external Lua library (v_repLua).\n");
    }
    else
    {
        if (useExternalLuaLibrary)
            printf("Using the external Lua library (v_repLua).\n");
        else
            printf("Using the default Lua library.\n");
    }
}

bool _loadExtLuaLibrary(const char* pathAndFilename)
{
    if ( (pathAndFilename==NULL)||(strlen(pathAndFilename)==0) )
    {
        lib=NULL;
        return(true);
    }
    else
    {
        lib=VVarious::openLibrary(pathAndFilename);
        if (lib!=NULL)
        {
            if (!_getLibProcAddresses())
            {
                VVarious::closeLibrary(lib);
                lib=NULL;
            }
        }
        return (lib!=NULL);
    }
}

void unloadExtLuaLibrary()
{
    if (lib!=NULL)
    {
        VVarious::closeLibrary(lib);
        lib=NULL;
    }
}



int luaWrapGet_LUA_MULTRET()
{
    if (lib!=NULL)
        return(luaLibGet_LUA_MULTRET());
    return(LUA_MULTRET);
}

int luaWrapGet_LUA_MASKCOUNT()
{
    if (lib!=NULL)
        return(luaLibGet_LUA_MASKCOUNT());
    return(LUA_MASKCOUNT);
}

int luaWrapGet_LUA_GLOBALSINDEX()
{
    if (lib!=NULL)
        return(luaLibGet_LUA_GLOBALSINDEX());
    return(LUA_GLOBALSINDEX);
}

luaWrap_lua_State* luaWrap_luaL_newstate()
{
    if (lib!=NULL)
        return(luaLib_luaL_newstate());
    return((luaWrap_lua_State*)luaL_newstate());
}

void luaWrap_lua_close(luaWrap_lua_State* L)
{
    if (lib!=NULL)
        luaLib_lua_close(L);
    else
        lua_close((lua_State*)L);
}

void luaWrap_luaL_openlibs(luaWrap_lua_State* L)
{
    if (lib!=NULL)
        luaLib_luaL_openlibs(L);
    else
        luaL_openlibs((lua_State*)L);
}

int luaWrap_lua_sethook(luaWrap_lua_State* L,luaWrap_lua_Hook func,int mask,int cnt)
{
    if (lib!=NULL)
        return(luaLib_lua_sethook(L,func,mask,cnt));
    return(lua_sethook((lua_State*)L,(lua_Hook)func,mask,cnt));
}

void luaWrap_lua_register(luaWrap_lua_State* L,const char* name,luaWrap_lua_CFunction func)
{
    if (lib!=NULL)
        luaLib_lua_register(L,name,func);
    else
        lua_register((lua_State*)L,name,(lua_CFunction)func);
}

void luaWrap_lua_pushnumber(luaWrap_lua_State* L,luaWrap_lua_Number n)
{
    if (lib!=NULL)
        luaLib_lua_pushnumber(L,n);
    else
        lua_pushnumber((lua_State*)L,n);
}

void luaWrap_lua_pushnil(luaWrap_lua_State* L)
{
    if (lib!=NULL)
        luaLib_lua_pushnil(L);
    else
        lua_pushnil((lua_State*)L);
}

void luaWrap_lua_pushboolean(luaWrap_lua_State* L,int b)
{
    if (lib!=NULL)
        luaLib_lua_pushboolean(L,b);
    else
        lua_pushboolean((lua_State*)L,b);
}

void luaWrap_lua_pushinteger(luaWrap_lua_State* L,int n)
{
    if (lib!=NULL)
        luaLib_lua_pushinteger(L,n);
    else
        lua_pushinteger((lua_State*)L,n);
}

void luaWrap_lua_pushstring(luaWrap_lua_State* L,const char* str)
{
    if (lib!=NULL)
        luaLib_lua_pushstring(L,str);
    else
        lua_pushstring((lua_State*)L,str);
}

void luaWrap_lua_pushlstring(luaWrap_lua_State* L,const char* str,size_t l)
{
    if (lib!=NULL)
        luaLib_lua_pushlstring(L,str,l);
    else
        lua_pushlstring((lua_State*)L,str,l);
}

void luaWrap_lua_pushcclosure(luaWrap_lua_State* L,luaWrap_lua_CFunction func,int n)
{
    if (lib!=NULL)
        luaLib_lua_pushcclosure(L,func,n);
    else
        lua_pushcclosure((lua_State*)L,(lua_CFunction)func,n);
}

void luaWrap_lua_pushvalue(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        luaLib_lua_pushvalue(L,idx);
    else
        lua_pushvalue((lua_State*)L,idx);
}

int luaWrap_lua_tointeger(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        return(luaLib_lua_tointeger(L,idx));
    return((int)lua_tointeger((lua_State*)L,idx));
}

luaWrap_lua_Number luaWrap_lua_tonumber(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        return(luaLib_lua_tonumber(L,idx));
    return((luaWrap_lua_Number)lua_tonumber((lua_State*)L,idx));
}

int luaWrap_lua_toboolean(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        return(luaLib_lua_toboolean(L,idx));
    return(lua_toboolean((lua_State*)L,idx));
}

const char* luaWrap_lua_tostring(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        return(luaLib_lua_tostring(L,idx));
    return(lua_tostring((lua_State*)L,idx));
}

const char* luaWrap_lua_tolstring(luaWrap_lua_State* L,int idx,size_t* len)
{
    if (lib!=NULL)
        return(luaLib_lua_tolstring(L,idx,len));
    return(lua_tolstring((lua_State*)L,idx,len));
}


int luaWrap_lua_isnumber(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        return(luaLib_lua_isnumber(L,idx));
    return(lua_isnumber((lua_State*)L,idx));
}

int luaWrap_lua_isstring(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        return(luaLib_lua_isstring(L,idx));
    return(lua_isstring((lua_State*)L,idx));
}

bool luaWrap_lua_isboolean(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        return(luaLib_lua_isboolean(L,idx));
    return(lua_isboolean((lua_State*)L,idx));
}

bool luaWrap_lua_isnil(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        return(luaLib_lua_isnil(L,idx));
    return(lua_isnil((lua_State*)L,idx));
}

bool luaWrap_lua_istable(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        return(luaLib_lua_istable(L,idx));
    return(lua_istable((lua_State*)L,idx));
}

bool luaWrap_lua_isfunction(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        return(luaLib_lua_isfunction(L,idx));
    return(lua_isfunction((lua_State*)L,idx));
}

int luaWrap_lua_getstack(luaWrap_lua_State* L,int level,luaWrap_lua_Debug* deb)
{
    if (lib!=NULL)
        return(luaLib_lua_getstack(L,level,deb));
    return(lua_getstack((lua_State*)L,level,(lua_Debug*)deb));
}

int luaWrap_lua_getinfo(luaWrap_lua_State* L,const char* what,luaWrap_lua_Debug* deb)
{
    if (lib!=NULL)
        return(luaLib_lua_getinfo(L,what,deb));
    return(lua_getinfo((lua_State*)L,what,(lua_Debug*)deb));
}

void luaWrap_lua_setglobal(luaWrap_lua_State* L,const char* name)
{
    if (lib!=NULL)
        luaLib_lua_setglobal(L,name);
    else
        lua_setglobal((lua_State*)L,name);
}

void luaWrap_lua_getfield(luaWrap_lua_State* L,int idx,const char* name)
{
    if (lib!=NULL)
        luaLib_lua_getfield(L,idx,name);
    else
        lua_getfield((lua_State*)L,idx,name);
}

void luaWrap_lua_setfield(luaWrap_lua_State* L,int idx,const char* name)
{
    if (lib!=NULL)
        luaLib_lua_setfield(L,idx,name);
    else
        lua_setfield((lua_State*)L,idx,name);
}

void luaWrap_lua_yield(luaWrap_lua_State* L,int nresults)
{
    if (lib!=NULL)
        luaLib_lua_yield(L,nresults);
    else
        lua_yield((lua_State*)L,nresults);
}

int luaWrap_luaL_dostring(luaWrap_lua_State* L,const char* str)
{
    if (lib!=NULL)
        return(luaLib_luaL_dostring(L,str));
    return(luaL_dostring((lua_State*)L,str));
}

void luaWrap_lua_getglobal(luaWrap_lua_State* L,const char* str)
{
    if (lib!=NULL)
        luaLib_lua_getglobal(L,str);
    else
        lua_getglobal((lua_State*)L,str);
}

void luaWrap_lua_pop(luaWrap_lua_State* L,int n)
{
    if (lib!=NULL)
        luaLib_lua_pop(L,n);
    else
        lua_pop((lua_State*)L,n);
}

int luaWrap_lua_gettop(luaWrap_lua_State* L)
{
    if (lib!=NULL)
        return(luaLib_lua_gettop(L));
    return(lua_gettop((lua_State*)L));
}

void luaWrap_lua_settop(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        luaLib_lua_settop(L,idx);
    else
        lua_settop((lua_State*)L,idx);
}

size_t luaWrap_lua_objlen(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        return(luaLib_lua_objlen(L,idx));
    return(lua_objlen((lua_State*)L,idx));
}

void luaWrap_lua_rawgeti(luaWrap_lua_State* L,int idx,int n)
{
    if (lib!=NULL)
        luaLib_lua_rawgeti(L,idx,n);
    else
        lua_rawgeti((lua_State*)L,idx,n);
}

void luaWrap_lua_rawseti(luaWrap_lua_State* L,int idx,int n)
{
    if (lib!=NULL)
        luaLib_lua_rawseti(L,idx,n);
    else
        lua_rawseti((lua_State*)L,idx,n);
}

void luaWrap_lua_newtable(luaWrap_lua_State* L)
{
    if (lib!=NULL)
        luaLib_lua_newtable(L);
    else
        lua_newtable((lua_State*)L);
}

int luaWrap_luaL_loadbuffer(luaWrap_lua_State* L,const char* buff,size_t sz,const char* name)
{
    if (lib!=NULL)
        return(luaLib_luaL_loadbuffer(L,buff,sz,name));
    return(luaL_loadbuffer((lua_State*)L,buff,sz,name));
}

void luaWrap_lua_remove(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        luaLib_lua_remove(L,idx);
    else
        lua_remove((lua_State*)L,idx);
}

void luaWrap_lua_insert(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        luaLib_lua_insert(L,idx);
    else
        lua_insert((lua_State*)L,idx);
}

int luaWrap_lua_pcall(luaWrap_lua_State* L,int nargs,int nresult,int errfunc)
{
    if (lib!=NULL)
        return(luaLib_lua_pcall(L,nargs,nresult,errfunc));
    return(lua_pcall((lua_State*)L,nargs,nresult,errfunc));
}

int luaWrap_lua_isuserdata(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        return(luaLib_lua_isuserdata(L,idx));
    return(lua_isuserdata((lua_State*)L,idx));
}

int luaWrap_lua_upvalueindex(int i)
{
    if (lib!=NULL)
        return(luaLib_lua_upvalueindex(i));
    return(lua_upvalueindex(i));
}

int luaWrap_getCurrentCodeLine(luaWrap_lua_State* L)
{
    if (lib!=NULL)
        return(luaLib_getCurrentCodeLine(L));
    int r=-1;
    lua_Debug ar;
    if (lua_getstack((lua_State*)L,1,&ar)==1)
    {
        if (lua_getinfo((lua_State*)L,"l",&ar)==1)
            r=ar.currentline;
    }
    return(r);
}

void luaWrap_lua_settable(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        luaLib_lua_settable(L,idx);
    else
        lua_settable((lua_State*)L,idx);
}

int luaWrap_lua_next(luaWrap_lua_State* L,int idx)
{
    if (lib!=NULL)
        return(luaLib_lua_next(L,idx));
    else
        return(lua_next((lua_State*)L,idx));
}

int luaWrap_lua_stype(luaWrap_lua_State* L,int idx)
{
    int t;
    if (lib!=NULL)
        t=luaLib_lua_type(L,idx);
    else
        t=lua_type((lua_State*)L,idx);
    if (t==LUA_TNIL)
        return(STACK_OBJECT_NULL);
    if (t==LUA_TNUMBER)
        return(STACK_OBJECT_NUMBER);
    if (t==LUA_TBOOLEAN)
        return(STACK_OBJECT_BOOL);
    if (t==LUA_TSTRING)
        return(STACK_OBJECT_STRING);
    if (t==LUA_TTABLE)
        return(STACK_OBJECT_TABLE);
    if (t==LUA_TFUNCTION)
        return(STACK_OBJECT_FUNC);
    if (t==LUA_TUSERDATA)
        return(STACK_OBJECT_USERDAT);
    if (t==LUA_TTHREAD)
        return(STACK_OBJECT_THREAD);
    if (t==LUA_TLIGHTUSERDATA)
        return(STACK_OBJECT_LIGHTUSERDAT);
    return(STACK_OBJECT_NULL);
}

int luaWrap_lua_error(luaWrap_lua_State* L)
{
    if (lib!=NULL)
        return(luaLib_lua_error(L));
    else
        return(lua_error((lua_State*)L));
}
