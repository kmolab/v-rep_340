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

#include "mainCont.h"
#include "luaScriptObject.h"
#include "broadcastDataContainer.h"
#include "v_rep_internal.h"
#ifdef SIM_WITH_GUI
    #include "vMenubar.h"
#endif

#define ADDON_SCRIPT_PREFIX_AUTOSTART "vrepAddOnScript_"
#define ADDON_SCRIPT_PREFIX_NOAUTOSTART "vrepAddOnScript-"
#define ADDON_FUNCTION_PREFIX "vrepAddOnFunc_"
#define ADDON_EXTENTION "lua"

class CLuaScriptContainer : public CMainCont 
{
public:
    CLuaScriptContainer();
    virtual ~CLuaScriptContainer();
    void simulationAboutToStart();
    void simulationEnded();
    void simulationAboutToEnd();
    void renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib);
    void removeAllScripts(bool keepAddOnScripts);
    void announceObjectWillBeErased(int objectID);
    bool processCommand(int commandID);
    bool removeScript_safe(int scriptId);
    bool removeScript(int scriptID);
    int insertScript(CLuaScriptObject* script);
    CLuaScriptObject* getScriptFromID(int scriptID) const;
    CLuaScriptObject* getMainScript() const;
    CLuaScriptObject* getScriptFromObjectAttachedTo_child(int threeDObjectID) const;
    CLuaScriptObject* getScriptFromObjectAttachedTo_callback(int threeDObjectID) const;
    CLuaScriptObject* getScriptFromObjectAttachedTo_customization(int threeDObjectID) const;
    CLuaScriptObject* getCustomContactHandlingScript_callback() const;
    CLuaScriptObject* getGeneralCallbackHandlingScript_callback() const;
    int getScriptsFromObjectAttachedTo(int threeDObjectID,std::vector<CLuaScriptObject*>& scripts) const;
    void killAllSimulationLuaStates();
    int insertDefaultScript_mainAndChildScriptsOnly(int scriptType,bool threaded);
    int insertAddOnScripts();
    int prepareAddOnFunctionNames();
    void setInMainScriptNow(bool launched,int startTimeInMs);
    bool getInMainScriptNow() const;
    int getMainScriptExecTimeInMs() const;
    void setInAddOnNow(bool inAddOn);
    bool getInAddOnNow() const;
    void setInCustomizationScriptNow(bool inCustomizationScript);
    bool getInCustomizationScriptNow() const;

    int removeDestroyedScripts(int scriptType);

    void addCallbackStructureObjectToDestroyAtTheEndOfSimulation_new(SScriptCallBack* object);
    void addCallbackStructureObjectToDestroyAtTheEndOfSimulation_old(SLuaCallBack* object);
    bool addCommandToOutsideCommandQueues(int commandID,int auxVal1,int auxVal2,int auxVal3,int auxVal4,const float aux2Vals[8],int aux2Count);

    void handleAddOnScriptExecution();
    void killAddOnScriptStates();

    int handleCustomizationScriptExecution(int callType);
    void sceneOrModelAboutToBeSaved(int modelBase);

    static void setAdditionalAddOnScript(std::string scriptNameWithExtension,bool firstSceneOnly);

    std::vector<CLuaScriptObject*> allScripts;

    std::vector<std::string> allAddOnFunctionNames;

    bool hasSceneIncludeScripts() const;
    bool hasModelIncludeScripts(int modelBase) const;

    int getScriptSimulationParameter_mainAndChildScriptsOnly(int scriptHandle,const char* parameterName,std::string& parameterValue) const;
    int setScriptSimulationParameter_mainAndChildScriptsOnly(int scriptHandle,const char* parameterName,const char* parameterValue,int parameterValueLength);

    CBroadcastDataContainer broadcastDataContainer;

    static bool getSaveIncludeScriptFiles();
    static void setSaveIncludeScriptFiles(bool save);

#ifdef SIM_WITH_GUI
    void addMenu(VMenu* menu);
#endif

protected:
    bool _inMainScriptNow;
    int _mainScriptStartTimeInMs;
    bool _inAddOnNow;
    bool _inCustomizationScriptNow;

    std::vector<SScriptCallBack*> _callbackStructureToDestroyAtEndOfSimulation_new;
    std::vector<SLuaCallBack*> _callbackStructureToDestroyAtEndOfSimulation_old;
    static std::string _additionalAddOnScriptFirstScene;
    static std::string _additionalAddOnScriptAllScenes;
    static bool _saveIncludeScriptFiles;
};
