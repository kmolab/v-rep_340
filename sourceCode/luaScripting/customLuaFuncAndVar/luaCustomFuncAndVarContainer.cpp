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
#include "app.h"
#include "luaCustomFuncAndVarContainer.h"

CLuaCustomFuncAndVarContainer::CLuaCustomFuncAndVarContainer()
{
    warningAboutFunctionNamesWithoutPlugin=false;
}

CLuaCustomFuncAndVarContainer::~CLuaCustomFuncAndVarContainer()
{
    removeAllCustomFunctions();
    removeAllCustomVariables();
}

void CLuaCustomFuncAndVarContainer::outputWarningWithFunctionNamesWithoutPlugin(bool o)
{
    warningAboutFunctionNamesWithoutPlugin=o;
}

void CLuaCustomFuncAndVarContainer::removeAllCustomFunctions()
{
    for (size_t i=0;i<allCustomFunctions.size();i++)
        delete allCustomFunctions[i];
    allCustomFunctions.clear();
}

bool CLuaCustomFuncAndVarContainer::removeCustomFunction(const char* fullFunctionName)
{
    for (size_t i=0;i<allCustomFunctions.size();i++)
    {
        if (allCustomFunctions[i]->isFunctionNameSame(fullFunctionName))
        { // we have to remove this one
            delete allCustomFunctions[i];
            allCustomFunctions.erase(allCustomFunctions.begin()+i);
            return(true);
        }
    }
    return(false);
}

void CLuaCustomFuncAndVarContainer::announcePluginWasKilled(const char* pluginName)
{
    for (size_t i=0;i<allCustomFunctions.size();i++)
    {
        if (allCustomFunctions[i]->isFunctionNameSame(pluginName))
        { // we have to remove this one
            delete allCustomFunctions[i];
            allCustomFunctions.erase(allCustomFunctions.begin()+i);
            i--; // reprocess this position
        }
    }

    for (size_t i=0;i<allCustomVariables.size();i++)
    {
        if (allCustomVariables[i]->isVariableNameSame(pluginName))
        { // we have to remove this one
            delete allCustomVariables[i];
            allCustomVariables.erase(allCustomVariables.begin()+i);
            i--; // reprocess this position
        }
    }
}

bool CLuaCustomFuncAndVarContainer::insertCustomFunction(CLuaCustomFunction* function)
{
    if (doesCustomFunctionAlreadyExist(function))
        return(false);
    if (function->getFunctionName().length()==0)
        return(false);

    if ((function->getPluginName()=="")&&warningAboutFunctionNamesWithoutPlugin)
    {
        printf("Warning: Detected a custom function name that is not formatted as\n");
        printf("         funcName@pluginName: %s\n",function->getFunctionName().c_str());
        printf("         Unloading this plugin dynamically might lead to a crash.\n");
    }

    int newID=0;
    while (getCustomFunctionFromID(newID)!=NULL)
        newID++;
    function->setFunctionID(newID);
    allCustomFunctions.push_back(function);
    return(true);
}

bool CLuaCustomFuncAndVarContainer::doesCustomFunctionAlreadyExist(CLuaCustomFunction* function)
{
    for (int i=0;i<int(allCustomFunctions.size());i++)
    {
        if (function->getFunctionName()==allCustomFunctions[i]->getFunctionName())
            return(true);
    }
    return(false);
}

CLuaCustomFunction* CLuaCustomFuncAndVarContainer::getCustomFunctionFromID(int functionID)
{
    for (int i=0;i<int(allCustomFunctions.size());i++)
    {
        if (functionID==allCustomFunctions[i]->getFunctionID())
            return(allCustomFunctions[i]);
    }
    return(NULL);
}

void CLuaCustomFuncAndVarContainer::appendAllFunctionNames_spaceSeparated(std::string& v)
{
    for (size_t i=0;i<allCustomFunctions.size();i++)
    {
        v+=allCustomFunctions[i]->getFunctionName();
        v+=" ";
    }
}

void CLuaCustomFuncAndVarContainer::registerCustomLuaFunctions(luaWrap_lua_State* L,luaWrap_lua_CFunction func)
{
    for (size_t i=0;i<allCustomFunctions.size();i++)
        allCustomFunctions[i]->registerCustomLuaFunction(L,func);
}

void CLuaCustomFuncAndVarContainer::removeAllCustomVariables()
{
    for (int i=0;i<int(allCustomVariables.size());i++)
        delete allCustomVariables[i];
    allCustomVariables.clear();
}

bool CLuaCustomFuncAndVarContainer::removeCustomVariable(const char* fullVariableName)
{
    for (size_t i=0;i<allCustomVariables.size();i++)
    {
        if (allCustomVariables[i]->isVariableNameSame(fullVariableName))
        { // we have to remove this one
            delete allCustomVariables[i];
            allCustomVariables.erase(allCustomVariables.begin()+i);
            return(true);
        }
    }
    return(false);
}

bool CLuaCustomFuncAndVarContainer::insertCustomVariable(const char* fullVariableName,const char* variableValue,int stackHandle)
{
    if (variableValue!=NULL)
    { // we register a simple variable
        removeCustomVariable(fullVariableName);
        CLuaCustomVariable* v=new CLuaCustomVariable(fullVariableName,variableValue,0);
        allCustomVariables.push_back(v);
        return(true);
    }
    else
    { // register a stack variable
        CInterfaceStack* stack=App::ct->interfaceStackContainer->getStack(stackHandle);
        if (stack==NULL)
            return(false);
        if (stack->getStackSize()<1)
            return(false);
        while (stack->getStackSize()>1)
        { // keep one item in the stack (the top item)
            stack->moveStackItemToTop(0);
            stack->popStackValue(1);
        }
        removeCustomVariable(fullVariableName);
        CLuaCustomVariable* v=new CLuaCustomVariable(fullVariableName,NULL,stackHandle);
        allCustomVariables.push_back(v);
        return(true);
    }
}

void CLuaCustomFuncAndVarContainer::assignCustomVariables(luaWrap_lua_State* L)
{
    for (size_t i=0;i<allCustomVariables.size();i++)
        allCustomVariables[i]->pushVariableOntoLuaStack(L);
}

void CLuaCustomFuncAndVarContainer::appendAllVariableNames_spaceSeparated(std::string& v)
{
    for (size_t i=0;i<allCustomVariables.size();i++)
    {
        v+=allCustomVariables[i]->getVariableName();
        v+=" ";
    }
}

void CLuaCustomFuncAndVarContainer::pushAllVariableNamesThatStartSame(const std::string& txt,std::vector<std::string>& v)
{
    for (size_t i=0;i<allCustomVariables.size();i++)
    {
        std::string n(allCustomVariables[i]->getVariableName());
        if ((n.size()>=txt.size())&&(n.compare(0,txt.size(),txt)==0))
            v.push_back(n);
    }
}

