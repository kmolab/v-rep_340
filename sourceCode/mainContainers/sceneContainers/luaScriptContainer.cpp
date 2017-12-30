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
#include "v_rep_internal.h"
#include "luaScriptContainer.h"
#include "tt.h"
#include "vVarious.h"
#include "threadPool.h"
#include "vFileFinder.h"
#include "v_repStrings.h"
#include "app.h"
#include "vDateTime.h"
#include "funcDebug.h"

std::string CLuaScriptContainer::_additionalAddOnScriptFirstScene;
std::string CLuaScriptContainer::_additionalAddOnScriptAllScenes;
bool CLuaScriptContainer::_saveIncludeScriptFiles=false;

bool CLuaScriptContainer::getSaveIncludeScriptFiles()
{
   return(_saveIncludeScriptFiles);
}

void CLuaScriptContainer::setSaveIncludeScriptFiles(bool save)
{
    _saveIncludeScriptFiles=save;
}

CLuaScriptContainer::CLuaScriptContainer()
{
    _inMainScriptNow=false;
    _inAddOnNow=false;
    _inCustomizationScriptNow=false;
    insertDefaultScript_mainAndChildScriptsOnly(sim_scripttype_mainscript,false);
    insertAddOnScripts();
    prepareAddOnFunctionNames();
}

CLuaScriptContainer::~CLuaScriptContainer()
{
    removeAllScripts(false);
    for (size_t i=0;i<_callbackStructureToDestroyAtEndOfSimulation_new.size();i++)
        delete _callbackStructureToDestroyAtEndOfSimulation_new[i];
    for (size_t i=0;i<_callbackStructureToDestroyAtEndOfSimulation_old.size();i++)
        delete _callbackStructureToDestroyAtEndOfSimulation_old[i];
    broadcastDataContainer.removeAllObjects();
}

bool CLuaScriptContainer::hasSceneIncludeScripts() const
{
    for (size_t i=0;i<allScripts.size();i++)
    {
        CLuaScriptObject* it=allScripts[i];
        if (it->isSceneScript()&&(it->getIncludeScriptFilePathAndName().length()>0))
            return(true);
    }
    return(false);
}

bool CLuaScriptContainer::hasModelIncludeScripts(int modelBase) const
{
    C3DObject* modelBaseObject=App::ct->objCont->getObject(modelBase);
    std::vector<C3DObject*> objects;
    modelBaseObject->getAllObjectsRecursive(&objects,true,true);
    for (size_t i=0;i<objects.size();i++)
    {
        std::vector<CLuaScriptObject*> scripts;
        getScriptsFromObjectAttachedTo(objects[i]->getID(),scripts);
        for (size_t j=0;j<scripts.size();j++)
        {
            if (scripts[j]->getIncludeScriptFilePathAndName().length()>0)
                return(true);
        }
    }
    return(false);
}

void CLuaScriptContainer::simulationAboutToStart()
{
    broadcastDataContainer.simulationAboutToStart();
    for (size_t i=0;i<allScripts.size();i++)
        allScripts[i]->simulationAboutToStart();
}

void CLuaScriptContainer::simulationEnded()
{
    for (size_t i=0;i<allScripts.size();i++)
        allScripts[i]->simulationEnded();

    broadcastDataContainer.simulationEnded();
    removeDestroyedScripts(sim_scripttype_mainscript);
    removeDestroyedScripts(sim_scripttype_childscript);
    removeDestroyedScripts(sim_scripttype_jointctrlcallback);
    removeDestroyedScripts(sim_scripttype_contactcallback);
    for (size_t i=0;i<_callbackStructureToDestroyAtEndOfSimulation_new.size();i++)
        delete _callbackStructureToDestroyAtEndOfSimulation_new[i];
    _callbackStructureToDestroyAtEndOfSimulation_new.clear();
    for (size_t i=0;i<_callbackStructureToDestroyAtEndOfSimulation_old.size();i++)
        delete _callbackStructureToDestroyAtEndOfSimulation_old[i];
    _callbackStructureToDestroyAtEndOfSimulation_old.clear();
//  if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
//  {
//  }
}

void CLuaScriptContainer::simulationAboutToEnd()
{
    for (size_t i=0;i<allScripts.size();i++)
        allScripts[i]->simulationAboutToEnd();
}

void CLuaScriptContainer::setAdditionalAddOnScript(std::string scriptNameWithExtension,bool firstSceneOnly)
{
    if (firstSceneOnly)
        _additionalAddOnScriptFirstScene=scriptNameWithExtension;
    else
        _additionalAddOnScriptAllScenes=scriptNameWithExtension;
}

void CLuaScriptContainer::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

void CLuaScriptContainer::addCallbackStructureObjectToDestroyAtTheEndOfSimulation_new(SScriptCallBack* object)
{
    _callbackStructureToDestroyAtEndOfSimulation_new.push_back(object);
}
void CLuaScriptContainer::addCallbackStructureObjectToDestroyAtTheEndOfSimulation_old(SLuaCallBack* object)
{
    _callbackStructureToDestroyAtEndOfSimulation_old.push_back(object);
}
int CLuaScriptContainer::removeDestroyedScripts(int scriptType)
{
    FUNCTION_DEBUG;
    int retVal=0;
    for (int i=0;i<int(allScripts.size());i++)
    {
        if ( (allScripts[i]->getScriptType()==scriptType)&&allScripts[i]->getFlaggedForDestruction() )
        {
            if ( (!allScripts[i]->getThreadedExecution())||(!allScripts[i]->getThreadedExecutionIsUnderWay()) )
            {
                retVal++;
                CLuaScriptObject* it=allScripts[i];
                it->killLuaState(); // should not be done in the destructor!
                allScripts.erase(allScripts.begin()+i);
                i--;
                delete it;
            }
        }
    }
    return(retVal);
}

void CLuaScriptContainer::setInMainScriptNow(bool launched,int startTimeInMs)
{
    _inMainScriptNow=launched;
    _mainScriptStartTimeInMs=startTimeInMs;
}

bool CLuaScriptContainer::getInMainScriptNow() const
{
    return(_inMainScriptNow);
}

int CLuaScriptContainer::getMainScriptExecTimeInMs() const
{
    return(VDateTime::getTimeDiffInMs(_mainScriptStartTimeInMs));
}

void CLuaScriptContainer::setInAddOnNow(bool inAddOn)
{
    _inAddOnNow=inAddOn;
}

void CLuaScriptContainer::setInCustomizationScriptNow(bool inCustomizationScript)
{
    _inCustomizationScriptNow=inCustomizationScript;
}

bool CLuaScriptContainer::getInAddOnNow() const
{
    return(_inAddOnNow);
}

bool CLuaScriptContainer::getInCustomizationScriptNow() const
{
    return(_inCustomizationScriptNow);
}

void CLuaScriptContainer::removeAllScripts(bool keepAddOnScripts)
{
    FUNCTION_DEBUG;
    if (!keepAddOnScripts)
    {
        while (allScripts.size()>0)
        {
            CLuaScriptObject* it=allScripts[0];
            it->killLuaState(); // should not be done in the destructor!
            allScripts.erase(allScripts.begin());
            delete it;
        }
    }
    else
    {
        for (int i=0;i<int(allScripts.size());i++)
        {
            CLuaScriptObject* it=allScripts[i];
            if ( (it->getScriptType()!=sim_scripttype_addonscript)&&(it->getScriptType()!=sim_scripttype_addonfunction) )
            {
                removeScript(it->getScriptID());
                i=-1; // ordering might have changed
            }
        }
    }
}

void CLuaScriptContainer::killAllSimulationLuaStates()
{
    for (size_t i=0;i<allScripts.size();i++)
    {
        if ((allScripts[i]->getScriptType()==sim_scripttype_mainscript)||(allScripts[i]->getScriptType()==sim_scripttype_childscript)||(allScripts[i]->getScriptType()==sim_scripttype_jointctrlcallback)||(allScripts[i]->getScriptType()==sim_scripttype_contactcallback)) //||(allScripts[i]->getScriptType()==sim_scripttype_generalcallback)) // not the customization scripts!!||(allScripts[i]->getScriptType()==sim_scripttype_customizationscript))
            allScripts[i]->killLuaState();
    }
}

void CLuaScriptContainer::announceObjectWillBeErased(int objectID)
{ // Never called from copy buffer!
    size_t i=0;
    while (i<allScripts.size())
    {
        if (allScripts[i]->announce3DObjectWillBeErased(objectID,false))
        {
            if (removeScript(allScripts[i]->getScriptID()))
                i=0; // ordering may have changed
            else
                i++;
        }
        else
            i++;
    }
}

bool CLuaScriptContainer::removeScript_safe(int scriptId)
{ // removal may happen in a delayed fashion
    CLuaScriptObject* it=getScriptFromID(scriptId);
    if (it==NULL)
        return(false);
    int res=it->flagScriptForRemoval();
    if (res==0)
        return(false);
    if (res==2)
        removeScript(scriptId);
    return(true);
}

bool CLuaScriptContainer::removeScript(int scriptID)
{
    FUNCTION_DEBUG;
    for (size_t i=0;i<allScripts.size();i++)
    {
        if (allScripts[i]->getScriptID()==scriptID)
        {
            CLuaScriptObject* it=allScripts[i];
            it->killLuaState(); // should not be done in the destructor!
            allScripts.erase(allScripts.begin()+i);
            delete it;
            break;
        }
    }
    App::setFullDialogRefreshFlag();
    return(true);
}

CLuaScriptObject* CLuaScriptContainer::getScriptFromID(int scriptID) const
{
    for (size_t i=0;i<allScripts.size();i++)
    {
        if (allScripts[i]->getScriptID()==scriptID)
            return(allScripts[i]);
    }
    return(NULL);
}

CLuaScriptObject* CLuaScriptContainer::getScriptFromObjectAttachedTo_child(int threeDObjectID) const
{ // used for child scripts
    if (threeDObjectID<0)
        return(NULL); // 10/1/2016
    for (size_t i=0;i<allScripts.size();i++)
    {
        if (allScripts[i]->getObjectIDThatScriptIsAttachedTo_child()==threeDObjectID)
            return(allScripts[i]);
    }
    return(NULL);
}

CLuaScriptObject* CLuaScriptContainer::getScriptFromObjectAttachedTo_callback(int threeDObjectID) const
{ // used for callback scripts
    for (size_t i=0;i<allScripts.size();i++)
    {
        if (allScripts[i]->getObjectIDThatScriptIsAttachedTo_callback()==threeDObjectID)
            return(allScripts[i]);
    }
    return(NULL);
}

CLuaScriptObject* CLuaScriptContainer::getScriptFromObjectAttachedTo_customization(int threeDObjectID) const
{ // used for customization scripts
    for (size_t i=0;i<allScripts.size();i++)
    {
        if (allScripts[i]->getObjectIDThatScriptIsAttachedTo_customization()==threeDObjectID)
            return(allScripts[i]);
    }
    return(NULL);
}

int CLuaScriptContainer::getScriptsFromObjectAttachedTo(int threeDObjectID,std::vector<CLuaScriptObject*>& scripts) const
{
    scripts.clear();
    CLuaScriptObject* it=getScriptFromObjectAttachedTo_child(threeDObjectID);
    if (it!=NULL)
        scripts.push_back(it);
    it=getScriptFromObjectAttachedTo_callback(threeDObjectID);
    if (it!=NULL)
        scripts.push_back(it);
    it=getScriptFromObjectAttachedTo_customization(threeDObjectID);
    if (it!=NULL)
        scripts.push_back(it);
    return(int(scripts.size()));
}

CLuaScriptObject* CLuaScriptContainer::getCustomContactHandlingScript_callback() const
{
    for (size_t i=0;i<allScripts.size();i++)
    {
        if (allScripts[i]->getScriptType()==sim_scripttype_contactcallback)
            return(allScripts[i]);
    }
    return(NULL);
}

CLuaScriptObject* CLuaScriptContainer::getGeneralCallbackHandlingScript_callback() const
{
    for (size_t i=0;i<allScripts.size();i++)
    {
        if (allScripts[i]->getScriptType()==sim_scripttype_generalcallback)
            return(allScripts[i]);
    }
    return(NULL);
}

CLuaScriptObject* CLuaScriptContainer::getMainScript() const
{
    for (size_t i=0;i<allScripts.size();i++)
    {
        if (allScripts[i]->getScriptType()==sim_scripttype_mainscript)
            return(allScripts[i]);
    }
    return(NULL);
}

int CLuaScriptContainer::insertScript(CLuaScriptObject* script)
{
    // We make sure the id is unique:
    int newID=SIM_IDSTART_LUASCRIPT;
    while (getScriptFromID(newID)!=NULL)
        newID++;
    script->setScriptID(newID);
    allScripts.push_back(script);

    return(newID);
}

int CLuaScriptContainer::insertDefaultScript_mainAndChildScriptsOnly(int scriptType,bool threaded)
{ 
    if (scriptType!=sim_scripttype_childscript)
        threaded=false; // just to make sure
    int retVal=-1;
    std::string filenameAndPath(App::directories->systemDirectory+VREP_SLASH);

    if (scriptType==sim_scripttype_mainscript)
        filenameAndPath+=DEFAULT_MAINSCRIPT_NAME;
    if (scriptType==sim_scripttype_childscript)
    {
        if (threaded)
            filenameAndPath+=DEFAULT_THREADEDCHILDSCRIPT_NAME;
        else
            filenameAndPath+=DEFAULT_NONTHREADEDCHILDSCRIPT_NAME;
    }

    if (VFile::doesFileExist(filenameAndPath))
    {
        try
        {
            VFile file(filenameAndPath,VFile::READ|VFile::SHARE_DENY_NONE);
            VArchive archive(&file,VArchive::LOAD);
            unsigned int archiveLength=(unsigned int)file.getLength();
            char* defaultScript=new char[archiveLength+1];
            for (int i=0;i<int(archiveLength);i++)
                archive >> defaultScript[i];
            defaultScript[archiveLength]=0;
            CLuaScriptObject* defScript=new CLuaScriptObject(scriptType);
            retVal=insertScript(defScript);
            defScript->setScriptText(defaultScript,NULL);
            defScript->setThreadedExecution(threaded);
            if (threaded)
                defScript->setExecuteJustOnce(true);
            delete[] defaultScript;     
            archive.close();
            file.close();
        }
        catch(VFILE_EXCEPTION_TYPE e)
        {
            VFile::reportAndHandleFileExceptionError(e);
            char defaultMessage[]="Default script file could not be found!"; // do not use comments ("--"), we want to cause an execution error!
            CLuaScriptObject* defScript=new CLuaScriptObject(scriptType);
            retVal=insertScript(defScript);
            defScript->setScriptText(defaultMessage,NULL);
            defScript->setThreadedExecution(threaded);
        }
    }
    else
    {
        char defaultMessage[]="Default script file could not be found!"; // do not use comments ("--"), we want to cause an execution error!
        CLuaScriptObject* defScript=new CLuaScriptObject(scriptType);
        retVal=insertScript(defScript);
        defScript->setScriptText(defaultMessage,NULL);
        defScript->setThreadedExecution(threaded);
    }
    App::setLightDialogRefreshFlag();
    return(retVal);
}

int CLuaScriptContainer::insertAddOnScripts()
{
    int addOnsCount=0;
    VFileFinder finder;
    finder.searchFilesWithExtension(App::directories->executableDirectory,ADDON_EXTENTION);
    int cnt=0;
    SFileOrFolder* foundItem=finder.getFoundItem(cnt);
    while (foundItem!=NULL)
    {
        if ( (foundItem->name.find(ADDON_SCRIPT_PREFIX_AUTOSTART)==0)||(foundItem->name.find(ADDON_SCRIPT_PREFIX_NOAUTOSTART)==0) )
        {
            std::string fp(App::directories->executableDirectory+VREP_SLASH);
            fp+=foundItem->name;

            if (VFile::doesFileExist(fp))
            {
                try
                {
                    VFile file(fp,VFile::READ|VFile::SHARE_DENY_NONE);
                    VArchive archive(&file,VArchive::LOAD);
                    unsigned int archiveLength=(unsigned int)file.getLength();
                    char* script=new char[archiveLength+1];
                    for (int i=0;i<int(archiveLength);i++)
                        archive >> script[i];
                    script[archiveLength]=0;
                    CLuaScriptObject* defScript=new CLuaScriptObject(sim_scripttype_addonscript);
                    insertScript(defScript);
                    defScript->setScriptText(script,NULL);

                    std::string nm(foundItem->name);
                    nm.erase(nm.begin(),nm.begin()+strlen(ADDON_SCRIPT_PREFIX_AUTOSTART));
                    nm.erase(nm.end()-strlen(ADDON_EXTENTION)-1,nm.end());

                    defScript->setAddOnName(nm.c_str());
                    defScript->setThreadedExecution(false);
                    if (foundItem->name.find(ADDON_SCRIPT_PREFIX_AUTOSTART)==0)
                        defScript->setAddOnDesiredExecutionState(1); // running

                    delete[] script;        
                    archive.close();
                    file.close();
                    addOnsCount++;
                    printf("Add-on script '%s' was loaded.\n",foundItem->name.c_str());
                }
                catch(VFILE_EXCEPTION_TYPE e)
                {
                    printf("Failed loading add-on script '%s'.\n",foundItem->name.c_str());
                }
            }

        }
        cnt++;
        foundItem=finder.getFoundItem(cnt);
    }

    // Now the additional add-on scripts (this is enabled via command line options -a and -b:
    std::vector<std::string> additionalScripts;
    if (_additionalAddOnScriptFirstScene.length()!=0)
    {
        additionalScripts.push_back(_additionalAddOnScriptFirstScene);
        _additionalAddOnScriptFirstScene="";
    }
    if (_additionalAddOnScriptAllScenes.length()!=0)
        additionalScripts.push_back(_additionalAddOnScriptAllScenes);
    for (int addScr=0;addScr<int(additionalScripts.size());addScr++)
    {
        std::string fp(additionalScripts[addScr]);
        if (!VVarious::isAbsolutePath(fp))
        {
            fp=App::directories->executableDirectory+VREP_SLASH;
            fp+=additionalScripts[addScr];
        }
        std::string fileName_noExtension(VVarious::splitPath_fileBase(fp));
        std::string fileName_withExtension(VVarious::splitPath_fileBaseAndExtension(fp));
        if (VFile::doesFileExist(fp))
        {
            try
            {
                VFile file(fp,VFile::READ|VFile::SHARE_DENY_NONE);
                VArchive archive(&file,VArchive::LOAD);
                unsigned int archiveLength=(unsigned int)file.getLength();
                char* script=new char[archiveLength+1];
                for (int i=0;i<int(archiveLength);i++)
                    archive >> script[i];
                script[archiveLength]=0;
                CLuaScriptObject* defScript=new CLuaScriptObject(sim_scripttype_addonscript);
                insertScript(defScript);
                defScript->setScriptText(script,NULL);

                defScript->setAddOnName(fileName_noExtension.c_str());
                defScript->setThreadedExecution(false);
                defScript->setAddOnDesiredExecutionState(1); // running

                delete[] script;
                archive.close();
                file.close();
                addOnsCount++;
                printf("Add-on script '%s' was loaded.\n",fileName_withExtension.c_str());
            }
            catch(VFILE_EXCEPTION_TYPE e)
            {
                printf("Failed loading add-on script '%s'.\n",fileName_withExtension.c_str());
            }
        }
    }

    return(addOnsCount);
}

int CLuaScriptContainer::prepareAddOnFunctionNames()
{
    int addOnsCount=0;
    VFileFinder finder;
    finder.searchFilesWithExtension(App::directories->executableDirectory,ADDON_EXTENTION);
    int cnt=0;
    SFileOrFolder* foundItem=finder.getFoundItem(cnt);
    while (foundItem!=NULL)
    {
        if (foundItem->name.find(ADDON_FUNCTION_PREFIX)==0)
        {
            std::string nm(foundItem->name);
            nm.erase(nm.begin(),nm.begin()+strlen(ADDON_FUNCTION_PREFIX));
            nm.erase(nm.end()-strlen(ADDON_EXTENTION)-1,nm.end());
            allAddOnFunctionNames.push_back(nm);
            addOnsCount++;
        }
        cnt++;
        foundItem=finder.getFoundItem(cnt);
    }
    return(addOnsCount);
}

void CLuaScriptContainer::handleAddOnScriptExecution()
{
    for (size_t i=0;i<allScripts.size();i++)
    {
        CLuaScriptObject* it=allScripts[i];
        if (it->getScriptType()==sim_scripttype_addonscript)
            it->runAddOn();
    }
}

void CLuaScriptContainer::killAddOnScriptStates()
{
    for (size_t i=0;i<allScripts.size();i++)
    {
        CLuaScriptObject* it=allScripts[i];
        if (it->getScriptType()==sim_scripttype_addonscript)
            it->killAddOnScriptState();
    }
}

void CLuaScriptContainer::sceneOrModelAboutToBeSaved(int modelBase)
{
    C3DObject* obj=App::ct->objCont->getObject(modelBase);
    if (obj!=NULL)
    {
        std::vector<C3DObject*> toExplore;
        toExplore.push_back(obj);
        while (toExplore.size()!=0)
        {
            obj=toExplore[toExplore.size()-1];
            toExplore.pop_back();
            CLuaScriptObject* it=getScriptFromObjectAttachedTo_customization(obj->getID());
            if (it!=NULL)
            {
                if (it->getCustomizationScriptCleanupBeforeSave())
                    it->killLuaState();
            }
            for (size_t i=0;i<obj->childList.size();i++)
                toExplore.push_back(obj->childList[i]);
        }
    }
    else
    {
        for (size_t i=0;i<allScripts.size();i++)
        {
            CLuaScriptObject* it=allScripts[i];
            if (it->getScriptType()==sim_scripttype_customizationscript)
            {
                if (it->getCustomizationScriptCleanupBeforeSave())
                    it->killLuaState();
            }
        }
    }
}

int CLuaScriptContainer::handleCustomizationScriptExecution(int callType)
{ // returns the number of different customization scripts executed
    int retVal=0;

    // Do 3 loops, for the first, normal and last execution order.
    // We always go through the whole scripts, since some scripts might be destroyed
    // on the way:
    for (int priority=sim_scriptexecorder_first;priority<=sim_scriptexecorder_last;priority++)
    {
        for (int i=0;i<int(allScripts.size());i++)
        {
            CLuaScriptObject* it=allScripts[i];
            if ( (it->getScriptType()==sim_scripttype_customizationscript)&&(it->getExecutionOrder()==priority) )
            {
                C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo_customization());
                if (obj!=NULL) // we could still run it in that situation, but is not desired, since the script itself will shortly be destroyed.. or is unattached!
                {
                    if (it->runCustomizationScript(callType))
                        retVal++;
                }
            }
        }
    }
    return(retVal);
}

int CLuaScriptContainer::getScriptSimulationParameter_mainAndChildScriptsOnly(int scriptHandle,const char* parameterName,std::string& parameterValue) const
{
    int retVal=0;
    for (size_t i=0;i<allScripts.size();i++)
    {
        if (!allScripts[i]->getFlaggedForDestruction())
        {
            if ( (scriptHandle==allScripts[i]->getScriptID())||
                (scriptHandle==sim_handle_all)||
                ( (scriptHandle==sim_handle_main_script)&&(allScripts[i]->getScriptType()==sim_scripttype_mainscript) ) )
            {
                if ((allScripts[i]->getScriptType()==sim_scripttype_mainscript)||(allScripts[i]->getScriptType()==sim_scripttype_childscript))
                { // We only access main script and child script1
                    bool res=allScripts[i]->getScriptParametersObject()->getParameterValue(parameterName,parameterValue);
                    if (res)
                        retVal++;
                }
            }
        }
    }
    return(retVal);
}

int CLuaScriptContainer::setScriptSimulationParameter_mainAndChildScriptsOnly(int scriptHandle,const char* parameterName,const char* parameterValue,int parameterValueLength)
{
    int retVal=0;
    for (int i=0;i<int(allScripts.size());i++)
    {
        if (!allScripts[i]->getFlaggedForDestruction())
        {
            if ( (scriptHandle==allScripts[i]->getScriptID())||
                (scriptHandle==sim_handle_all)||
                ( (scriptHandle==sim_handle_main_script)&&(allScripts[i]->getScriptType()==sim_scripttype_mainscript) ) )
            {
                if ((allScripts[i]->getScriptType()==sim_scripttype_mainscript)||(allScripts[i]->getScriptType()==sim_scripttype_childscript))
                { // We only access main script and child script1
                    bool res=allScripts[i]->getScriptParametersObject()->setParameterValue(parameterName,parameterValue,parameterValueLength);
                    if (res)
                        retVal++;
                }
            }
        }
    }
    return(retVal);
}

bool CLuaScriptContainer::addCommandToOutsideCommandQueues(int commandID,int auxVal1,int auxVal2,int auxVal3,int auxVal4,const float aux2Vals[8],int aux2Count)
{
    for (int i=0;i<int(allScripts.size());i++)
    {
        if (!allScripts[i]->getFlaggedForDestruction())
            allScripts[i]->addCommandToOutsideCommandQueue(commandID,auxVal1,auxVal2,auxVal3,auxVal4,aux2Vals,aux2Count);
    }
    return(true);
}

bool CLuaScriptContainer::processCommand(int commandID)
{ // Return value is true if the command belonged to hierarchy menu and was executed
    if ( (commandID>=SCRIPT_CONT_COMMANDS_ADDON_FUNCTION_MENU_ITEM_START_SCCMD)&&(commandID<=SCRIPT_CONT_COMMANDS_ADDON_FUNCTION_MENU_ITEM_END_SCCMD) )
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            int index=commandID-SCRIPT_CONT_COMMANDS_ADDON_FUNCTION_MENU_ITEM_START_SCCMD;
            if (index<int(allAddOnFunctionNames.size()))
            {
                std::string txt(IDSNS_STARTING_ADDON_FUNCTION);
                txt+=" ";
                txt+=allAddOnFunctionNames[index];
                App::addStatusbarMessage(txt.c_str());

                // execute the add-on function here!!
                std::string fp(App::directories->executableDirectory+VREP_SLASH);
                fp+=ADDON_FUNCTION_PREFIX;
                fp+=allAddOnFunctionNames[index];
                fp+=".";
                fp+=ADDON_EXTENTION;
                if (VFile::doesFileExist(fp))
                {
                    try
                    {
                        VFile file(fp,VFile::READ|VFile::SHARE_DENY_NONE);
                        VArchive archive(&file,VArchive::LOAD);
                        unsigned int archiveLength=(unsigned int)file.getLength();
                        char* script=new char[archiveLength+1];
                        for (int i=0;i<int(archiveLength);i++)
                            archive >> script[i];
                        script[archiveLength]=0;
                        CLuaScriptObject* defScript=new CLuaScriptObject(sim_scripttype_addonfunction);
                        int scriptID=insertScript(defScript);
                        defScript->setScriptText(script,NULL);
                        defScript->setAddOnName(allAddOnFunctionNames[index].c_str());
                        defScript->setThreadedExecution(false);
                        defScript->setAddOnDesiredExecutionState(1); // make sure it will run
                        delete[] script;
                        archive.close();
                        file.close();

                        defScript->runAddOn();
                        removeScript(scriptID);
                    }
                    catch(VFILE_EXCEPTION_TYPE e)
                    {
    //                  printf("Failed loading add-on script '%s'.\n",foundItem->name.c_str());
    //                  VFile::reportAndHandleFileExceptionError(e);
                    }
                }
                App::addStatusbarMessage(IDSNS_ENDED_ADDON_FUNCTION);
            }
        }
        else
        { // We are in the UI thread. Execute the command via the main thread:
            SSimulationThreadCommand cmd;
            cmd.cmdId=commandID;
            App::appendSimulationThreadCommand(cmd);
        }
        return(true);
    }

    if ( (commandID>=SCRIPT_CONT_COMMANDS_ADDON_SCRIPT_MENU_ITEM_START_SCCMD)&&(commandID<=SCRIPT_CONT_COMMANDS_ADDON_SCRIPT_MENU_ITEM_END_SCCMD) )
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            int index=commandID-SCRIPT_CONT_COMMANDS_ADDON_SCRIPT_MENU_ITEM_START_SCCMD;
            int cnt=0;
            CLuaScriptObject* it=NULL;
            for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
            {
                it=App::ct->luaScriptContainer->allScripts[i];
                if (it->getScriptType()==sim_scripttype_addonscript)
                {
                    if (index==cnt)
                        break;
                    it=NULL;
                    cnt++;
                }
            }
            if ( (it!=NULL)&&(it->getAddOnExecutionState()!=3) )
            {
                std::string txt;
                int st=it->getAddOnExecutionState();
                if (st==2)
                {
                    txt=IDSNS_RESUMED_ADDON_SCRIPT;
                    it->setAddOnDesiredExecutionState(1);
                }
                if (st==1)
                {
                    txt=IDSNS_PAUSED_ADDON_SCRIPT;
                    it->setAddOnDesiredExecutionState(2);
                }
                if (st==0)
                {
                    txt=IDSNS_STARTED_ADDON_SCRIPT;
                    it->setAddOnDesiredExecutionState(1);
                }
                txt+=" ";
                txt+=it->getAddOnName();
                App::addStatusbarMessage(txt.c_str());
            }
        }
        else
        { // We are in the UI thread. Execute the command via the main thread:
            SSimulationThreadCommand cmd;
            cmd.cmdId=commandID;
            App::appendSimulationThreadCommand(cmd);
        }
        return(true);
    }

    return(false);
}

#ifdef SIM_WITH_GUI
void CLuaScriptContainer::addMenu(VMenu* menu)
{
    // First add-on functions:
    int id=SCRIPT_CONT_COMMANDS_ADDON_FUNCTION_MENU_ITEM_START_SCCMD;
    for (int i=0;i<int(App::ct->luaScriptContainer->allAddOnFunctionNames.size());i++)
    {
        menu->appendMenuItem(true,false,id,App::ct->luaScriptContainer->allAddOnFunctionNames[i].c_str());
        id++;
    }

    menu->appendMenuSeparator();

    // Now add-on scripts:
    id=SCRIPT_CONT_COMMANDS_ADDON_SCRIPT_MENU_ITEM_START_SCCMD;
    for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
    {
        CLuaScriptObject* it=App::ct->luaScriptContainer->allScripts[i];
        if (it->getScriptType()==sim_scripttype_addonscript)
        {
            int st=it->getAddOnExecutionState();
            std::string txt;
            txt=it->getAddOnName();
            if (st==3)
                txt+=" (stopped, error)";
            if (st==2)
                txt+=" (paused)";
            if (st==1)
                txt+=" (running)";
            if (st==0)
                txt+=" (stopped)";
            menu->appendMenuItem(st!=3,st==1,id,txt.c_str(),true);
            id++;
        }
    }
}
#endif
