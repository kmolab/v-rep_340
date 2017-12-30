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
#include "funcDebug.h"
#include "easyLock.h"
#include "v_rep_internal.h"
#include "luaScriptObject.h"
#include "luaScriptFunctions.h"
#include "threadPool.h"
#include "tt.h"
#include <boost/lexical_cast.hpp>
#include "vDateTime.h"
#include "app.h"
#include "apiErrors.h"

int CLuaScriptObject::_nextIdForExternalScriptEditor=-1;
bool CLuaScriptObject::emergencyStopButtonPressed=false;

CLuaScriptObject::CLuaScriptObject(int scriptTypeOrMinusOneForSerialization)
{
    scriptID=SIM_IDSTART_LUASCRIPT;
    _objectIDAttachedTo_child=-1; // used for child scripts
    _objectIDAttachedTo_callback=-1; // used for callback scripts
    _objectIDAttachedTo_customization=-1; // used for customization scripts
    _scriptText=NULL;
    _scriptTextLength=0;
    _scriptTextExec=NULL;       // the one getting executed!
    _scriptTextExecLength=0;    // the one getting executed!
    _errorReportMode=sim_api_error_output|sim_api_warning_output;
    _lastErrorString=SIM_API_CALL_NO_ERROR;
    _numberOfPasses=0;
    _threadedExecution=false;
    _scriptIsDisabled=false;
    _executeJustOnce=false;
    _automaticCascadingCallsDisabled=false;
    _threadedExecutionUnderWay=false;
    _insideCustomLuaFunction=0;
    _flaggedForDestruction=false;
    _mainScriptIsDefaultMainScript=true;
    _executionOrder=sim_scriptexecorder_normal;
    _customizationScriptIsTemporarilyDisabled=false;
    _customizationScriptIsDisabledDuringSimulation=false;
    _customizationScriptCleanupBeforeSave=false;
    _addOn_executionState=0; // stopped
    _addOn_desiredExecutionState=0; // stop

    _previousEditionWindowPosAndSize[0]=50;
    _previousEditionWindowPosAndSize[1]=50;
    _previousEditionWindowPosAndSize[2]=1000;
    _previousEditionWindowPosAndSize[3]=800;

    _warningAboutSimHandleChildScriptAlreadyIssued_oldCompatibility_7_8_2014=false;
    _warning_simRMLPosition_oldCompatibility_30_8_2014=false;
    _warning_simRMLVelocity_oldCompatibility_30_8_2014=false;
    _warning_simGetMpConfigForTipPose_oldCompatibility_21_1_2016=false;
    _warning_simFindIkPath_oldCompatibility_2_2_2016=false;
    _warning_oldPathPlanningFunctionality_oldCompatibility_11_2_2016=false;
    _warning_oldMotionPlanningFunctionality_oldCompatibility_11_2_2016=false;

    _customObjectData=NULL;
    _customObjectData_tempData=NULL;

    scriptParameters=new CLuaScriptParameters();
    _outsideCommandQueue=new COutsideCommandQueueForScript();

    _scriptType=scriptTypeOrMinusOneForSerialization;

    L=NULL;
    _loadBufferResult=-1;
    _inExecutionNow=false;

    if (_nextIdForExternalScriptEditor==-1)
    { // new since 10/9/2014, otherwise there can be conflicts between simultaneously opened V-REP instances
        _nextIdForExternalScriptEditor=(VDateTime::getOSTimeInMs()&0xffff)*1000;
    }

    _filenameForExternalScriptEditor="embScript_"+tt::FNb(_nextIdForExternalScriptEditor++)+".lua";
}

CLuaScriptObject::~CLuaScriptObject()
{
    FUNCTION_DEBUG;
    killLuaState(); // should already have been done outside of the destructor!
    clearAllUserData();
    delete[] _scriptText;
    delete scriptParameters;
    delete _outsideCommandQueue;
    delete _customObjectData;
    delete _customObjectData_tempData;
    if (!App::userSettings->useBuiltInScriptEditor())
    {
        if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript)||(_scriptType==sim_scripttype_jointctrlcallback)||(_scriptType==sim_scripttype_contactcallback)||(_scriptType==sim_scripttype_generalcallback) )
        {       
            // destroy file
            std::string fname=App::directories->extScriptEditorTempFileDirectory+VREP_SLASH;
            fname.append(_filenameForExternalScriptEditor);
            if (VFile::doesFileExist(fname))
                VFile::eraseFile(fname);
        }
    }
}

void CLuaScriptObject::setInsideCustomLuaFunction(bool inside)
{
    if (inside)
        _insideCustomLuaFunction++;
    else
        _insideCustomLuaFunction--;
}

bool CLuaScriptObject::getInsideCustomLuaFunction() const
{
    return(_insideCustomLuaFunction>0);
}



bool CLuaScriptObject::checkAndSetWarningAboutSimHandleChildScriptAlreadyIssued_oldCompatibility_7_8_2014()
{
    bool retVal=_warningAboutSimHandleChildScriptAlreadyIssued_oldCompatibility_7_8_2014;
    _warningAboutSimHandleChildScriptAlreadyIssued_oldCompatibility_7_8_2014=true;
    return(retVal);
}
bool CLuaScriptObject::checkAndSetWarning_simRMLPosition_oldCompatibility_30_8_2014()
{
    bool retVal=_warning_simRMLPosition_oldCompatibility_30_8_2014;
    _warning_simRMLPosition_oldCompatibility_30_8_2014=true;
    return(retVal);
}
bool CLuaScriptObject::checkAndSetWarning_simRMLVelocity_oldCompatibility_30_8_2014()
{
    bool retVal=_warning_simRMLVelocity_oldCompatibility_30_8_2014;
    _warning_simRMLVelocity_oldCompatibility_30_8_2014=true;
    return(retVal);
}
bool CLuaScriptObject::checkAndSetWarning_simGetMpConfigForTipPose_oldCompatibility_21_1_2016()
{
    bool retVal=_warning_simGetMpConfigForTipPose_oldCompatibility_21_1_2016;
    _warning_simGetMpConfigForTipPose_oldCompatibility_21_1_2016=true;
    return(retVal);
}
bool CLuaScriptObject::checkAndSetWarning_simFindIkPath_oldCompatibility_2_2_2016()
{
    bool retVal=_warning_simFindIkPath_oldCompatibility_2_2_2016;
    _warning_simFindIkPath_oldCompatibility_2_2_2016=true;
    return(retVal);
}
bool CLuaScriptObject::checkAndSetWarning_oldPathPlanningFunctionality_oldCompatibility_11_2_2016()
{
    bool retVal=_warning_oldPathPlanningFunctionality_oldCompatibility_11_2_2016;
    _warning_oldPathPlanningFunctionality_oldCompatibility_11_2_2016=true;
    return(retVal);
}
bool CLuaScriptObject::checkAndSetWarning_oldMotionPlanningFunctionality_oldCompatibility_11_2_2016()
{
    bool retVal=_warning_oldMotionPlanningFunctionality_oldCompatibility_11_2_2016;
    _warning_oldMotionPlanningFunctionality_oldCompatibility_11_2_2016=true;
    return(retVal);
}

int CLuaScriptObject::getErrorReportMode() const
{
    return(_errorReportMode);
}

void CLuaScriptObject::setErrorReportMode(int e)
{
    _errorReportMode=e;
}

std::string CLuaScriptObject::getLastErrorString() const
{
    return(_lastErrorString);
}

void CLuaScriptObject::setLastErrorString(const char* txt)
{
    _lastErrorString=txt;
}

void CLuaScriptObject::setAutomaticCascadingCallsDisabled(bool disabled)
{
    _automaticCascadingCallsDisabled=disabled;
}

bool CLuaScriptObject::getAutomaticCascadingCallsDisabled() const
{
    return(_automaticCascadingCallsDisabled);
}

int CLuaScriptObject::getScriptExecutionTimeInMs() const
{
    return(VDateTime::getTimeDiffInMs(_scriptExecStartTime));
}

void CLuaScriptObject::setObjectCustomData(int header,const char* data,int dataLength)
{
    if (_customObjectData==NULL)
        _customObjectData=new CCustomData();
    _customObjectData->setData(header,data,dataLength);
}
int CLuaScriptObject::getObjectCustomDataLength(int header) const
{
    if (_customObjectData==NULL)
        return(0);
    return(_customObjectData->getDataLength(header));
}
void CLuaScriptObject::getObjectCustomData(int header,char* data) const
{
    if (_customObjectData==NULL)
        return;
    _customObjectData->getData(header,data);
}

bool CLuaScriptObject::getObjectCustomDataHeader(int index,int& header) const
{
    if (_customObjectData==NULL)
        return(false);
    return(_customObjectData->getHeader(index,header));
}



void CLuaScriptObject::setObjectCustomData_tempData(int header,const char* data,int dataLength)
{
    if (_customObjectData_tempData==NULL)
        _customObjectData_tempData=new CCustomData();
    _customObjectData_tempData->setData(header,data,dataLength);
}
int CLuaScriptObject::getObjectCustomDataLength_tempData(int header) const
{
    if (_customObjectData_tempData==NULL)
        return(0);
    return(_customObjectData_tempData->getDataLength(header));
}
void CLuaScriptObject::getObjectCustomData_tempData(int header,char* data) const
{
    if (_customObjectData_tempData==NULL)
        return;
    _customObjectData_tempData->getData(header,data);
}

bool CLuaScriptObject::getObjectCustomDataHeader_tempData(int index,int& header) const
{
    if (_customObjectData_tempData==NULL)
        return(false);
    return(_customObjectData_tempData->getHeader(index,header));
}



void CLuaScriptObject::getPreviousEditionWindowPosAndSize(int posAndSize[4]) const
{
    for (int i=0;i<4;i++)
        posAndSize[i]=_previousEditionWindowPosAndSize[i];
}

void CLuaScriptObject::setPreviousEditionWindowPosAndSize(const int posAndSize[4])
{
    for (int i=0;i<4;i++)
        _previousEditionWindowPosAndSize[i]=posAndSize[i];
}

std::string CLuaScriptObject::getAddOnName() const
{
    return(_addOnName);
}

int CLuaScriptObject::getAddOnExecutionState() const
{
    return(_addOn_executionState);
}

void CLuaScriptObject::setAddOnDesiredExecutionState(int state)
{
    _addOn_desiredExecutionState=state;
}


std::string CLuaScriptObject::getFilenameForExternalScriptEditor() const
{
    std::string fname=App::directories->extScriptEditorTempFileDirectory+VREP_SLASH;
    fname.append(_filenameForExternalScriptEditor);
    return(fname);
}

void CLuaScriptObject::fromFileToBuffer()
{
    if (!App::userSettings->useBuiltInScriptEditor())
    { // read file
        _scriptFoldingInfo.clear();
        if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript)||(_scriptType==sim_scripttype_jointctrlcallback)||(_scriptType==sim_scripttype_contactcallback)||(_scriptType==sim_scripttype_generalcallback) )
        {
            std::string fname=App::directories->extScriptEditorTempFileDirectory+VREP_SLASH;
            fname.append(_filenameForExternalScriptEditor);

            VFile myFile(fname.c_str(),VFile::READ|VFile::SHARE_DENY_NONE,true);
            if (myFile.getFile()!=NULL)
            {
                VArchive arW(&myFile,VArchive::LOAD);
                unsigned int archiveLength=(unsigned int)myFile.getLength();
                _scriptTextLength=archiveLength+1;
                delete[] _scriptText;
                _scriptText=new char[_scriptTextLength];
                char ab;
                for (int i=0;i<_scriptTextLength-1;i++)
                {
                    arW >> ab;
                    _scriptText[i]=ab;
                }
                // there is no closing zero in the file!!
                _scriptText[_scriptTextLength-1]=0;
                arW.close();
                myFile.close();
            }
        }
    }
}

void CLuaScriptObject::fromBufferToFile()
{
    if (!App::userSettings->useBuiltInScriptEditor())
    { // write file
        _scriptFoldingInfo.clear();
        if ((App::ct->environment==NULL)||(!App::ct->environment->getSceneLocked()))
        {
            if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript)||(_scriptType==sim_scripttype_jointctrlcallback)||(_scriptType==sim_scripttype_contactcallback)||(_scriptType==sim_scripttype_generalcallback) )
            {
                std::string fname=App::directories->extScriptEditorTempFileDirectory+VREP_SLASH;
                fname.append(_filenameForExternalScriptEditor);

                VFile myFile(fname.c_str(),VFile::CREATE_WRITE|VFile::SHARE_EXCLUSIVE,true);
                if (myFile.getFile()!=NULL)
                {
                    VArchive arW(&myFile,VArchive::STORE);
                    if (_scriptText!=NULL)
                    {
                        for (int i=0;i<_scriptTextLength-1;i++)
                            arW << _scriptText[i];
                    }
                    // Do not write the closing zero!
                    arW.close();
                    myFile.close();
                }
            }
        }
    }
}

void CLuaScriptObject::setExecuteJustOnce(bool justOnce)
{
    _executeJustOnce=justOnce;
}

bool CLuaScriptObject::getExecuteJustOnce() const
{
    return(_executeJustOnce);
}

void CLuaScriptObject::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
    if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript)||(_scriptType==sim_scripttype_jointctrlcallback)||(_scriptType==sim_scripttype_contactcallback) )//||(_scriptType==sim_scripttype_generalcallback) )
    {
        _initialValuesInitialized=simulationIsRunning;
        if (simulationIsRunning)
        {

        }
        if (scriptParameters!=NULL)
            scriptParameters->initializeInitialValues(simulationIsRunning);
        if (_outsideCommandQueue!=NULL)
            _outsideCommandQueue->initializeInitialValues(simulationIsRunning);
    }
}

void CLuaScriptObject::simulationAboutToStart()
{
    if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript)||(_scriptType==sim_scripttype_jointctrlcallback)||(_scriptType==sim_scripttype_contactcallback) )//||(_scriptType==sim_scripttype_generalcallback) )
    {
        killLuaState(); // should already be reset! (should have been done in simulationEnded routine)
        _numberOfPasses=0;
        _automaticCascadingCallsDisabled=false;
        initializeInitialValues(true);
        _warningAboutSimHandleChildScriptAlreadyIssued_oldCompatibility_7_8_2014=false;
        _warning_simRMLPosition_oldCompatibility_30_8_2014=false;
        _warning_simRMLVelocity_oldCompatibility_30_8_2014=false;
        _warning_simGetMpConfigForTipPose_oldCompatibility_21_1_2016=false;
        _warning_simFindIkPath_oldCompatibility_2_2_2016=false;
        _warning_oldPathPlanningFunctionality_oldCompatibility_11_2_2016=false;
        _warning_oldMotionPlanningFunctionality_oldCompatibility_11_2_2016=false;
    }
}

void CLuaScriptObject::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
    if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript)||(_scriptType==sim_scripttype_jointctrlcallback)||(_scriptType==sim_scripttype_contactcallback) )//||(_scriptType==sim_scripttype_generalcallback) )
    {
        if (scriptParameters!=NULL)
            scriptParameters->simulationEnded();
        if (_outsideCommandQueue!=NULL)
            _outsideCommandQueue->simulationEnded();
        delete[] _scriptTextExec;
        _scriptTextExec=NULL;
        clearAllUserData();
        _scriptTextExecLength=0;
        if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
        {

        }
        _initialValuesInitialized=false;
    }
}

void CLuaScriptObject::simulationAboutToEnd()
{ // Added this on 7/8/2014.
    if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript)||(_scriptType==sim_scripttype_jointctrlcallback)||(_scriptType==sim_scripttype_contactcallback) ) // not for customization or general callback script!    ||(_scriptType==sim_scripttype_generalcallback) )// not for customization scripts!!  ||(_scriptType==sim_scripttype_customizationscript))
        killLuaState(); // this has to happen while simulation is still running!!
}

void CLuaScriptObject::setCustomizationScriptIsTemporarilyDisabled(bool disabled)
{
    _customizationScriptIsTemporarilyDisabled=disabled;
}

void CLuaScriptObject::setCustomizationScriptIsDisabledDuringSimulation(bool disabled)
{
    _customizationScriptIsDisabledDuringSimulation=disabled;
}

bool CLuaScriptObject::getCustomizationScriptIsTemporarilyDisabled() const
{
    return(_customizationScriptIsTemporarilyDisabled);
}

void CLuaScriptObject::setCustomizationScriptCleanupBeforeSave(bool doIt)
{
    _customizationScriptCleanupBeforeSave=doIt;
}

bool CLuaScriptObject::getCustomizationScriptCleanupBeforeSave() const
{
    return(_customizationScriptCleanupBeforeSave);
}

bool CLuaScriptObject::getCustomizationScriptIsDisabledDuringSimulation() const
{
    return(_customizationScriptIsDisabledDuringSimulation);
}

bool CLuaScriptObject::hasCustomizationScripAnyChanceToGetExecuted(bool whenSimulationRuns,bool forCleanUpSection) const
{
    // Following 2 since 23/1/2017
    if (forCleanUpSection)
        return(true);

    if (!App::userSettings->runCustomizationScripts)
        return(false);
    if (_scriptIsDisabled|_customizationScriptIsTemporarilyDisabled)
        return(false);
    if (whenSimulationRuns&&_customizationScriptIsDisabledDuringSimulation)
        return(false);
    C3DObject* obj=App::ct->objCont->getObject(getObjectIDThatScriptIsAttachedTo_customization());
    if (obj==NULL) // should never happen!
    { // can happen when the object attached to the script is already destroyed.
        // In that case we still want to run the customization script, at least the clean-up section of it!
 //       if (forCleanUpSection)
 //           return(true);
        return(false);
    }
    if (obj->getCumulativeModelProperty()&sim_modelproperty_scripts_inactive)
        return(false);
    return(true);
}

void CLuaScriptObject::setExecutionOrder(int order)
{
    _executionOrder=tt::getLimitedInt(0,2,order);
}

int CLuaScriptObject::getExecutionOrder() const
{
    return(_executionOrder);
}

void CLuaScriptObject::setCustomizedMainScript(bool customized)
{
    if (_scriptType==sim_scripttype_mainscript)
    {
        _mainScriptIsDefaultMainScript=!customized;
        App::setFullDialogRefreshFlag();
    }
}

bool CLuaScriptObject::isDefaultMainScript() const
{
    return(_mainScriptIsDefaultMainScript);
}

void CLuaScriptObject::setScriptIsDisabled(bool isDisabled)
{
    if (isDisabled)
    {
        if (_scriptType!=sim_scripttype_mainscript)
            _scriptIsDisabled=true;
    }
    else
        _scriptIsDisabled=false;
}

bool CLuaScriptObject::getScriptIsDisabled() const
{
    return(_scriptIsDisabled);
}

int CLuaScriptObject::getScriptType() const
{
    return(_scriptType);
}

bool CLuaScriptObject::getFlaggedForDestruction() const
{
    return(_flaggedForDestruction);
}

void CLuaScriptObject::setScriptText(const char* scriptTxt,const std::vector<int>* scriptFoldingInfo)
{
    EASYLOCK(_localMutex);
    if (_scriptText!=NULL)
        delete[] _scriptText;
    _scriptText=NULL;
    _scriptTextLength=0;
    if (scriptTxt!=NULL)
    {
        int l=0;
        while (scriptTxt[l]!=0)
            l++;
        _scriptTextLength=l+1;
        _scriptText=new char[_scriptTextLength];
        for (int i=0;i<_scriptTextLength;i++)
            _scriptText[i]=scriptTxt[i];
    }
    if (scriptFoldingInfo!=NULL)
        _scriptFoldingInfo.assign(scriptFoldingInfo[0].begin(),scriptFoldingInfo[0].end());
    else
        _scriptFoldingInfo.clear();
    if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript)||(_scriptType==sim_scripttype_jointctrlcallback)||(_scriptType==sim_scripttype_contactcallback)||(_scriptType==sim_scripttype_generalcallback) )
        fromBufferToFile();
}

char* CLuaScriptObject::getScriptText(std::vector<int>* scriptFoldingInfo)
{
    if (scriptFoldingInfo!=NULL)
        scriptFoldingInfo[0].assign(_scriptFoldingInfo.begin(),_scriptFoldingInfo.end());
    if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript)||(_scriptType==sim_scripttype_jointctrlcallback)||(_scriptType==sim_scripttype_contactcallback)||(_scriptType==sim_scripttype_generalcallback) )
        fromFileToBuffer();
    return(_scriptText);
}

int CLuaScriptObject::getScriptTextLength() const
{
    return(_scriptTextLength);
}

int CLuaScriptObject::getScriptID() const
{
    return(scriptID);
}

void CLuaScriptObject::setScriptID(int newID)
{ // careful with that function!
    scriptID=newID;
}

bool CLuaScriptObject::isSceneScript() const
{
    return ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript)||(_scriptType==sim_scripttype_jointctrlcallback)||(_scriptType==sim_scripttype_contactcallback)||(_scriptType==sim_scripttype_generalcallback)||(_scriptType==sim_scripttype_customizationscript) );
}

std::string CLuaScriptObject::getIncludeScriptFilePathAndName() const
{ // Will return a non-empty string only with script contents similar to following example:

    //require('utils')
    //include('/BlueWorkforce/modelScripts/ragnar_child.lua')
    //-- Do not add any code in here and leave above 2 lines in position

    std::string stt(_scriptText,_scriptTextLength);
    size_t line2Start=stt.find('\n')+1;
    if ((line2Start!=std::string::npos))
    {
        std::string line1(_scriptText,_scriptText+line2Start);
        size_t line2End=stt.find(')',line2Start);
        if ((line2End!=std::string::npos)&&(line1.compare(0,7,"require")==0)&&(line2End-line2Start>8))
        {
            std::string line2(_scriptText+line2Start,_scriptText+line2End);
            size_t fileStart=stt.find('(',line2Start);
            if ((line2.compare(0,7,"include")==0)&&(fileStart!=std::string::npos)&&(fileStart<line2End))
            {
                std::string filename(_scriptText+fileStart+2,_scriptText+line2End-1);
                filename=App::directories->executableDirectory+filename;
                return(filename);
            }
        }
    }
    return("");
}

std::string CLuaScriptObject::getDescriptiveName() const
{ // Cannot put following strings to resources since they are also used in openGL!!!
    std::string pref;
    if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript) )
    {
        if (!App::userSettings->useBuiltInScriptEditor())
        {
            pref="[";
            pref+=_filenameForExternalScriptEditor;
            pref+="] ";
        }
    }

    if (_scriptType==sim_scripttype_mainscript)
    {
        if (_mainScriptIsDefaultMainScript)
            return(strTranslate(pref+"Main script (default)"));
        return(strTranslate(pref+"Main script (customized)"));
    }
    if (_scriptType==sim_scripttype_childscript)
    {
        if (_flaggedForDestruction)
        {
            if (_threadedExecution)
                return(strTranslate(pref+"Threaded child script (destroyed)"));
            return(strTranslate(pref+"Non-threaded Child script (destroyed)"));
        }
        C3DObject* it=App::ct->objCont->getObject(_objectIDAttachedTo_child);
        if (it==NULL)
        {
            if (_threadedExecution)
                return(strTranslate(pref+"Threaded child script (unassociated)"));
            return(strTranslate(pref+"Non-threaded child script (unassociated)"));
        }
        std::string retVal;
        if (_threadedExecution)
            retVal=strTranslate(pref+"Threaded child script (");
        else
            retVal=strTranslate(pref+"Non-threaded child script (");
        retVal+=it->getName();
        retVal+=")";
        return(retVal);
    }

    if (_scriptType==sim_scripttype_addonscript)
    {
        std::string retVal;
        retVal=strTranslate(pref+"Add-on script '");
        retVal+=_addOnName;
        retVal+="'";
        return(retVal);
    }
    if (_scriptType==sim_scripttype_addonfunction)
    {
        std::string retVal;
        retVal=strTranslate(pref+"Add-on function '");
        retVal+=_addOnName;
        retVal+="'";
        return(retVal);
    }
    if (_scriptType==sim_scripttype_jointctrlcallback)
    {
        std::string retVal;
        retVal=strTranslate(pref+"Joint ctrl callback script ");
        C3DObject* it=App::ct->objCont->getObject(_objectIDAttachedTo_callback);
        if (it==NULL)
            return(strTranslate(pref+"Joint ctrl callback script (unassociated)"));

        retVal+="(";
        retVal+=it->getName();
        retVal+=")";
        return(retVal);
    }
    if (_scriptType==sim_scripttype_customizationscript)
    {
        std::string retVal;
        retVal=strTranslate(pref+"Customization script ");
        C3DObject* it=App::ct->objCont->getObject(_objectIDAttachedTo_customization);
        if (it==NULL)
            return(strTranslate(pref+"Customization script (unassociated)"));

        retVal+="(";
        retVal+=it->getName();
        retVal+=")";
        return(retVal);
    }
    if (_scriptType==sim_scripttype_contactcallback)
    {
        std::string retVal;
        retVal=strTranslate(pref+"Contact callback script");
        return(retVal);
    }
    if (_scriptType==sim_scripttype_generalcallback)
    {
        std::string retVal;
        retVal=strTranslate(pref+"General callback script");
        return(retVal);
    }
    return("ERROR");
}

std::string CLuaScriptObject::getShortDescriptiveName() const
{ // since 2/10/2012 (because if the name is too long, it is truncated when an error message mentions it)
    std::string pref;
    if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript) )
    {
        if (!App::userSettings->useBuiltInScriptEditor())
        {
            pref="[";
            pref+=_filenameForExternalScriptEditor;
            pref+="] ";
        }
    }

    if (_scriptType==sim_scripttype_mainscript)
        return(strTranslate(pref+"MAIN SCRIPT"));
    if (_scriptType==sim_scripttype_childscript)
    {
        if (_flaggedForDestruction)
            return(strTranslate(pref+"CHILD SCRIPT (DESTROYED)"));
        C3DObject* it=App::ct->objCont->getObject(_objectIDAttachedTo_child);
        if (it==NULL)
            return(strTranslate(pref+"UNASSOCIATED CHILD SCRIPT"));

        std::string retVal;
        retVal=strTranslate(pref+"SCRIPT ");
        retVal+=it->getName();
        return(retVal);
    }
    if (_scriptType==sim_scripttype_addonscript)
    {
        std::string retVal;
        retVal=strTranslate(pref+"ADDON SCRIPT ");
        retVal+=_addOnName;
        return(retVal);
    }
    if (_scriptType==sim_scripttype_addonfunction)
    {
        std::string retVal;
        retVal=strTranslate(pref+"ADDON FUNCTION ");
        retVal+=_addOnName;
        return(retVal);
    }
    if (_scriptType==sim_scripttype_jointctrlcallback)
    {
        std::string retVal;
        retVal=strTranslate(pref+"JOINT CTRL CALLBACK ");
        C3DObject* it=App::ct->objCont->getObject(_objectIDAttachedTo_callback);
        if (it==NULL)
            return(strTranslate(pref+"JOINT CTRL CALLBACK (UNASSOCIATED)"));
        retVal+=it->getName();
        return(retVal);
    }
    if (_scriptType==sim_scripttype_customizationscript)
    {
        std::string retVal;
        retVal=strTranslate(pref+"CUSTOMIZATION SCRIPT ");
        C3DObject* it=App::ct->objCont->getObject(_objectIDAttachedTo_customization);
        if (it==NULL)
            return(strTranslate(pref+"CUSTOMIZATION SCRIPT  (UNASSOCIATED)"));
        retVal+=it->getName();
        return(retVal);
    }
    if (_scriptType==sim_scripttype_contactcallback)
    {
        std::string retVal;
        retVal=strTranslate(pref+"CONTACT CALLBACK");
        return(retVal);
    }
    if (_scriptType==sim_scripttype_generalcallback)
    {
        std::string retVal;
        retVal=strTranslate(pref+"GENERAL CALLBACK");
        return(retVal);
    }
    return("ERROR");
}

void CLuaScriptObject::setAddOnName(const char* name)
{
    _addOnName=name;
}

std::string CLuaScriptObject::getScriptSuffixNumberString() const
{
    C3DObject* it=NULL;
    if (_scriptType==sim_scripttype_childscript)
        it=App::ct->objCont->getObject(_objectIDAttachedTo_child);
    if (_scriptType==sim_scripttype_jointctrlcallback)
        it=App::ct->objCont->getObject(_objectIDAttachedTo_callback);
    if (_scriptType==sim_scripttype_customizationscript)
        it=App::ct->objCont->getObject(_objectIDAttachedTo_customization);
    if (it==NULL)
        return("");
    int suffNb=tt::getNameSuffixNumber(it->getName().c_str(),true);
    std::string suffix("");
    if (suffNb!=-1)
        suffix=boost::lexical_cast<std::string>(suffNb);
    return(suffix);
}

std::string CLuaScriptObject::getScriptPseudoName() const
{
    if (_scriptType==sim_scripttype_childscript)
    {
        C3DObject* it=App::ct->objCont->getObject(_objectIDAttachedTo_child);
        if (it!=NULL)
            return(it->getName());
    }
    if ( (_scriptType==sim_scripttype_addonscript)||(_scriptType==sim_scripttype_addonfunction) )
        return(_addOnName);
    if (_scriptType==sim_scripttype_jointctrlcallback)
    {
        C3DObject* it=App::ct->objCont->getObject(_objectIDAttachedTo_callback);
        if (it!=NULL)
            return(it->getName());
    }
    if (_scriptType==sim_scripttype_customizationscript)
    {
        C3DObject* it=App::ct->objCont->getObject(_objectIDAttachedTo_customization);
        if (it!=NULL)
            return(it->getName());
    }
    return("");
}

void CLuaScriptObject::setThreadedExecution(bool threadedExec)
{
    if (threadedExec)
    {
        if (_scriptType==sim_scripttype_childscript)
            _threadedExecution=true;
    }
    else
        _threadedExecution=false;
}

bool CLuaScriptObject::getThreadedExecution() const
{
    return(_threadedExecution);
}

bool CLuaScriptObject::getThreadedExecutionIsUnderWay() const
{
    return(_threadedExecutionUnderWay);
}

void CLuaScriptObject::perform3DObjectLoadingMapping(std::vector<int>* map)
{
    if (App::ct->objCont!=NULL)
    {
        _objectIDAttachedTo_child=App::ct->objCont->getLoadingMapping(map,_objectIDAttachedTo_child);
        _objectIDAttachedTo_callback=App::ct->objCont->getLoadingMapping(map,_objectIDAttachedTo_callback);
        _objectIDAttachedTo_customization=App::ct->objCont->getLoadingMapping(map,_objectIDAttachedTo_customization);
    }
}

bool CLuaScriptObject::announce3DObjectWillBeErased(int objectID,bool copyBuffer)
{ // script will be erased if attached to objectID (if threaded simulation is not running!)
    if (_objectIDAttachedTo_child==objectID)
    {
        if (copyBuffer)
            return(true);
        if (!App::ct->simulation->isSimulationStopped()) // Removed the if(_threadedExecution()) thing on 2008/12/08
        { // threaded scripts cannot be directly erased, since the Lua state needs to be cleared in the thread that created it
#ifdef SIM_WITH_GUI
            if (App::mainWindow!=NULL)
                App::mainWindow->scintillaEditorContainer->closeEditor(scriptID);
#endif
            _objectIDAttachedTo_child=-1; // This is for a potential threaded simulation running
            _flaggedForDestruction=true;
            return(!_inExecutionNow); // from false to !_inExecutionNow on 8/9/2016
        }
        else
            return(true);
    }
    if (_objectIDAttachedTo_callback==objectID)
    {
        if (copyBuffer)
            return(true);
        _flaggedForDestruction=true;
#ifdef SIM_WITH_GUI
        if (App::mainWindow!=NULL)
            App::mainWindow->scintillaEditorContainer->closeEditor(scriptID);
#endif
        return(!_inExecutionNow); // from false to !_inExecutionNow on 26/8/2016 (i.e. no delayed destruction anymore)
    }
    if (_objectIDAttachedTo_customization==objectID)
    {
        if (copyBuffer)
            return(true);
        _flaggedForDestruction=true;
#ifdef SIM_WITH_GUI
        if (App::mainWindow!=NULL)
            App::mainWindow->scintillaEditorContainer->closeEditor(scriptID);
#endif
        return(!_inExecutionNow); // from false to !_inExecutionNow on 26/8/2016 (i.e. no delayed destruction anymore. Important since the clean-up section of custom. scripts can contain code that refers to the attached object, etc.)
    }
    return(false);
}

int CLuaScriptObject::flagScriptForRemoval()
{ // retVal: 0--> cannot be removed, 1 --> will be removed in a delayed manner, 2--> can be removed now
#ifdef SIM_WITH_GUI
    if (App::mainWindow!=NULL)
        App::mainWindow->scintillaEditorContainer->closeEditor(scriptID);
#endif

    if (App::ct->simulation->isSimulationStopped())
    {
        if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript)||(_scriptType==sim_scripttype_jointctrlcallback)||(_scriptType==sim_scripttype_contactcallback) )//||(_scriptType==sim_scripttype_generalcallback) )
            return(2);
    }
    if (_scriptType==sim_scripttype_customizationscript)
    {
        _flaggedForDestruction=true;
        return(1);
    }
    return(0);
}

int CLuaScriptObject::getObjectIDThatScriptIsAttachedTo_child() const
{
    return(_objectIDAttachedTo_child);
}

int CLuaScriptObject::getObjectIDThatScriptIsAttachedTo_callback() const
{
    return(_objectIDAttachedTo_callback);
}

int CLuaScriptObject::getObjectIDThatScriptIsAttachedTo_customization() const
{
    return(_objectIDAttachedTo_customization);
}

void CLuaScriptObject::setObjectIDThatScriptIsAttachedTo_child(int newObjectID)
{
    _objectIDAttachedTo_child=newObjectID;
    if (newObjectID!=-1)
        _scriptType=sim_scripttype_childscript;
}

void CLuaScriptObject::setObjectIDThatScriptIsAttachedTo_callback(int newObjectID)
{
    _objectIDAttachedTo_callback=newObjectID;
}

void CLuaScriptObject::setObjectIDThatScriptIsAttachedTo_customization(int newObjectID)
{
    _objectIDAttachedTo_customization=newObjectID;
}

int CLuaScriptObject::getNumberOfPasses() const
{
    return(_numberOfPasses);
}

void CLuaScriptObject::setNumberOfPasses(int p)
{
    _numberOfPasses=p;
}

int CLuaScriptObject::runMainScript()
{ // return value 0 indicates nothing to execute, 1 no error, 2 a lua compilation or runtime error, 3 a lua simulator call error (from this script or its child scripts), 4 if a script is being called several times (reentrance problem)
    FUNCTION_DEBUG;
    int retVal=sim_script_no_error;

    // PUT THESE THINGS SOMEWHERE ELSE (E.G. INTO AN OWN simPrepareSimulationStep)!!!!!!!
    //***********************************************************************
    App::ct->luaScriptContainer->broadcastDataContainer.removeTimedOutObjects(float(App::ct->simulation->getSimulationTime_ns())/1000000.0f); // remove invalid elements

    CThreadPool::prepareAllThreadsForResume_calledBeforeMainScript();

    //***********************************************************************
    if (CThreadPool::getSimulationEmergencyStop())
    {
        CThreadPool::handleAllThreads_withResumeLocation(-1); // Added on 24/02/2011 (important, otherwise threads might never resume!!!)
        return(retVal); // this is important in the case where we pressed the emergency stop (the stop button in the middle of the screen)
    }

    if (_scriptTextExec==NULL)
    {
        fromFileToBuffer();
        if ( (_scriptTextLength!=0)&&(_scriptText!=NULL) )
        { // we copy the script buffer (so that if we modify the script during a simulation, the script behaviour stays same!)
            _scriptTextExec=new char[_scriptTextLength];
            _scriptTextExecLength=_scriptTextLength;
            for (int i=0;i<_scriptTextLength;i++)
                _scriptTextExec[i]=_scriptText[i];
        }
    }

    if ( (_scriptTextExecLength==0)||(_scriptTextExec==NULL) )
        return(retVal);

    retVal=_runMainScript();

    if (App::ct->simulation->getPauseOnErrorRequested())
        App::ct->simulation->pauseSimulation();
    return(retVal);
}

int CLuaScriptObject::_runMainScript()
{
    FUNCTION_DEBUG;
    App::ct->calcInfo->scriptStart(false,true);

    CApiErrors::pushLocation(scriptID); // for correct error handling (i.e. assignement to the correct script and output)
    if (App::ct->simulation->getSimulationState()==sim_simulation_advancing_firstafterstop)
        _runMainScriptNow(sim_mainscriptcall_initialization);

    int retVal=_runMainScriptNow(sim_mainscriptcall_regular);

    if (App::ct->simulation->getSimulationState()==sim_simulation_advancing_lastbeforestop)
        _runMainScriptNow(sim_mainscriptcall_cleanup);
    CApiErrors::popLocation(); // for correct error handling (i.e. assignement to the correct script and output)

    App::ct->calcInfo->scriptEnd(false,true);

    // Here we have to prepare the error message for the info box:
    std::string tmpTxt;
    if (retVal&(sim_script_lua_error|sim_script_call_error))
        tmpTxt="&&fg930(script error)";
    App::ct->calcInfo->setMainScriptMessage(tmpTxt.c_str());
    return(retVal);
}

int CLuaScriptObject::_runMainScriptNow(int callType)
{
    FUNCTION_DEBUG;
    int retVal=sim_script_no_error;
    if (L==NULL)
    {
        _errorReportMode=sim_api_error_output|sim_api_warning_output;
        _lastErrorString=SIM_API_CALL_NO_ERROR;
        L=initializeNewLuaState(getScriptSuffixNumberString().c_str());
    }
    int oldTop=luaWrap_lua_gettop(L);

    std::string tmp("sim_current_script_id=");
    tmp+=boost::lexical_cast<std::string>(scriptID);
    tmp+="\nsim_call_type=";
    tmp+=boost::lexical_cast<std::string>(callType);
    luaWrap_luaL_dostring(L,tmp.c_str());

    if (_luaLoadBuffer(L,_scriptTextExec,_scriptTextExecLength-1,getShortDescriptiveName().c_str()))
    {
        _scriptExecStartTime=VDateTime::getTimeInMs();
        App::ct->luaScriptContainer->setInMainScriptNow(true,VDateTime::getTimeInMs());
        // Following 6 lines new since 8/11/2014:
        luaWrap_lua_getglobal(L,"debug");
        luaWrap_lua_getfield(L,-1,"traceback");
        luaWrap_lua_remove(L,-2);
        int argCnt=0;
        int errIndex=-argCnt-2;
        luaWrap_lua_insert(L,errIndex);
        if (_luaPCall(L,argCnt,luaWrapGet_LUA_MULTRET(),errIndex)!=0)
        { // a runtime error occurred!
            std::string errMsg;
            if (luaWrap_lua_isstring(L,-1))
                errMsg=std::string(luaWrap_lua_tostring(L,-1));
            else
                errMsg="(error unknown)";
            luaWrap_lua_pop(L,-1); // pop error from stack

            _displayScriptError(errMsg.c_str(),1);
            retVal|=sim_script_lua_error;
            // Here we can decide what happens uppon error:
            App::ct->simulation->pauseOnErrorRequested();
        }
        App::ct->luaScriptContainer->setInMainScriptNow(false,0);
    }
    else
    { // A compilation error occurred!
        std::string errMsg;
        if (luaWrap_lua_isstring(L,-1))
            errMsg=std::string(luaWrap_lua_tostring(L,-1));
        else
            errMsg="(error unknown)";
        _displayScriptError(errMsg.c_str(),0);
        luaWrap_lua_pop(L,-1); // pop error from stack
        retVal|=sim_script_lua_error;
        // Here we can decide what happens uppon error:
        App::ct->simulation->pauseOnErrorRequested();
    }
    _numberOfPasses++;

    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack


    return(retVal);
}

int CLuaScriptObject::runNonThreadedChildScript(CInterfaceStack& inputArguments,int callType)
{ // return value 0 indicates nothing to execute, 1 no error, 2 a lua compilation or runtime error, 3 a lua simulator call error (from this script or its child scripts), 4 if a script is being called several times (reentrance problem)
    FUNCTION_DEBUG;
    int retVal=sim_script_no_error;

    if (_scriptTextExec==NULL)
    {
        fromFileToBuffer();
        if ( (_scriptTextLength!=0)&&(_scriptText!=NULL) )
        { // we copy the script buffer (so that if we modify the script during a simulation, the script behaviour stays same!)
            _scriptTextExec=new char[_scriptTextLength];
            _scriptTextExecLength=_scriptTextLength;
            for (int i=0;i<_scriptTextLength;i++)
                _scriptTextExec[i]=_scriptText[i];
        }
    }

    if ( (_scriptTextExecLength==0)||(_scriptTextExec==NULL) )
        return(retVal);

    retVal=_runNonThreadedChildScript(inputArguments,callType);

    return(retVal);
}

int CLuaScriptObject::_runNonThreadedChildScript(CInterfaceStack& inputArguments,int callType)
{
    FUNCTION_DEBUG;
    App::ct->calcInfo->scriptStart(false,false);

    int retVal=0;
    CApiErrors::pushLocation(scriptID); // for correct error handling (i.e. assignement to the correct script and output)
    if (L==NULL)
    {
        CInterfaceStack in;
        retVal=_runNonThreadedChildScriptNow(in,sim_childscriptcall_initialization);
    }

    if (callType!=sim_childscriptcall_initialization) // sim_childscriptcall_initialization was already executed just above!
        retVal=_runNonThreadedChildScriptNow(inputArguments,callType);
    CApiErrors::popLocation(); // for correct error handling (i.e. assignement to the correct script and output)

    App::ct->calcInfo->scriptEnd(false,false);
    return(retVal);
}

int CLuaScriptObject::_runNonThreadedChildScriptNow(CInterfaceStack& inputArguments,int callType)
{
    FUNCTION_DEBUG;
    int retVal=sim_script_no_error;
    if (L==NULL)
    {
        _errorReportMode=sim_api_error_output|sim_api_warning_output;
        _lastErrorString=SIM_API_CALL_NO_ERROR;
        L=initializeNewLuaState(getScriptSuffixNumberString().c_str());
    }
    int oldTop=luaWrap_lua_gettop(L);   // We store lua's stack

    std::string tmp("sim_current_script_id=");
    tmp+=boost::lexical_cast<std::string>(getScriptID());
    tmp+="\nsim_call_type=";
    tmp+=boost::lexical_cast<std::string>(callType);
    luaWrap_luaL_dostring(L,tmp.c_str());

    if (_luaLoadBuffer(L,_scriptTextExec,_scriptTextExecLength-1,getShortDescriptiveName().c_str()))
    {
        inputArguments.buildOntoLuaStack(L,false);

        // Following 6 lines new since 8/11/2014:
        luaWrap_lua_getglobal(L,"debug");
        luaWrap_lua_getfield(L,-1,"traceback");
        luaWrap_lua_remove(L,-2);
        int argCnt=inputArguments.getStackSize();
        int errindex=-argCnt-2;
        luaWrap_lua_insert(L,errindex);
        _scriptExecStartTime=VDateTime::getTimeInMs();
        if (_luaPCall(L,argCnt,luaWrapGet_LUA_MULTRET(),errindex)!=0)
        { // a runtime error occurred!
            std::string errMsg;
            if (luaWrap_lua_isstring(L,-1))
                errMsg=std::string(luaWrap_lua_tostring(L,-1));
            else
                errMsg="(error unknown)";
            luaWrap_lua_pop(L,-1); // pop error from stack

            _displayScriptError(errMsg.c_str(),1);
            retVal|=sim_script_lua_error;
            // Here we can decide what happens uppon error:
            App::ct->simulation->pauseOnErrorRequested();
        }
    }
    else
    { // A compilation error occurred!
        std::string errMsg;
        if (luaWrap_lua_isstring(L,-1))
            errMsg=std::string(luaWrap_lua_tostring(L,-1));
        else
            errMsg="(error unknown)";
        _displayScriptError(errMsg.c_str(),0);

        luaWrap_lua_pop(L,-1); // pop error from stack
        retVal|=sim_script_lua_error;
        // Here we can decide what happens uppon error:
        App::ct->simulation->pauseOnErrorRequested();
    }
    _numberOfPasses++;

    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack

    return(retVal);
}

int CLuaScriptObject::runThreadedChildScript()
{ // return value 0 indicates nothing to execute, 1 no error, 2 a lua compilation or runtime error, 3 a lua simulator call error (from this script or its child scripts), 4 if a script is being called several times (reentrance problem)
    FUNCTION_DEBUG;
    int retVal=sim_script_no_error;
    if (_threadedExecutionUnderWay)
        return(retVal); // this script is being executed by another thread!

    if (_scriptTextExec==NULL)
    {
        fromFileToBuffer();
        if ( (_scriptTextLength!=0)&&(_scriptText!=NULL) )
        { // we copy the script buffer (so that if we modify the script during a simulation, the script behaviour stays same!)
            _scriptTextExec=new char[_scriptTextLength];
            _scriptTextExecLength=_scriptTextLength;
            for (int i=0;i<_scriptTextLength;i++)
                _scriptTextExec[i]=_scriptText[i];
        }
    }

    if ( (_scriptTextExecLength==0)||(_scriptTextExec==NULL) )
        return(retVal);

    if (_executeJustOnce&&(_numberOfPasses>0))
        return(0);

    if (CThreadPool::getSimulationStopRequested())// will also return true in case of emergency stop request
        return(retVal);

    _threadedExecutionUnderWay=true;
    _globalMutex.lock();
    toBeCalledByThread.push_back(this);
    _globalMutex.unlock();
    _threadedScript_associatedFiberOrThreadID=CThreadPool::createNewThread(_startAddressForThreadedScripts);
    CThreadPool::switchToThread(_threadedScript_associatedFiberOrThreadID);

    return(retVal);
}

VTHREAD_ID_TYPE CLuaScriptObject::getThreadedScriptThreadId() const
{
    return(_threadedScript_associatedFiberOrThreadID);
}

VTHREAD_RETURN_TYPE CLuaScriptObject::_startAddressForThreadedScripts(VTHREAD_ARGUMENT_TYPE lpData)
{
    FUNCTION_DEBUG;
    _globalMutex.lock();
    CLuaScriptObject* it=toBeCalledByThread[0];
    toBeCalledByThread.erase(toBeCalledByThread.begin());
    _globalMutex.unlock();

    App::ct->calcInfo->scriptStart(true,false);

    CApiErrors::addNewThreadForErrorReporting(it->getScriptID()); // for correct error handling (i.e. assignement to the correct script and output)
    it->runThreadedChildScriptNow_();
    CApiErrors::removeThreadFromErrorReporting(); // for correct error handling (i.e. assignement to the correct script and output)

    App::ct->calcInfo->scriptEnd(true,false);

    it->killLuaState(); // added on 2010/03/05 because of some crashes with threaded scripts (Lua said: resource destroyed by other thread than the one that created it!)
    it->_threadedExecutionUnderWay=false;

    return(0);
}

void CLuaScriptObject::runThreadedChildScriptNow_()
{
    FUNCTION_DEBUG;
    if (L==NULL)
    {
        _errorReportMode=sim_api_error_output|sim_api_warning_output;
        _lastErrorString=SIM_API_CALL_NO_ERROR;
        L=initializeNewLuaState(getScriptSuffixNumberString().c_str());
    }
    int oldTop=luaWrap_lua_gettop(L);   // We store lua's stack

    std::string tmp("sim_current_script_id=");
    tmp+=boost::lexical_cast<std::string>(getScriptID());
    tmp+="\nsim_call_type=";
    tmp+=boost::lexical_cast<std::string>((int)sim_childscriptcall_threaded);
    luaWrap_luaL_dostring(L,tmp.c_str());

    if (_luaLoadBuffer(L,_scriptTextExec,_scriptTextExecLength-1,getShortDescriptiveName().c_str()))
    {
        // Following 6 lines new since 8/11/2014:
        luaWrap_lua_getglobal(L,"debug");
        luaWrap_lua_getfield(L,-1,"traceback");
        luaWrap_lua_remove(L,-2);
        int argCnt=0;
        int errIndex=-argCnt-2;
        luaWrap_lua_insert(L,errIndex);
        _scriptExecStartTime=VDateTime::getTimeInMs();
        if (_luaPCall(L,argCnt,luaWrapGet_LUA_MULTRET(),errIndex)!=0)
        { // a runtime error occurred!
            // We have to exit the thread free mode if we are still in there (the instance should automatically be restored when this thread resumes):
            if (CThreadPool::isThreadInFreeMode())
                CThreadPool::setThreadFreeMode(false);

            std::string errMsg;
            if (luaWrap_lua_isstring(L,-1))
                errMsg=std::string(luaWrap_lua_tostring(L,-1));
            else
                errMsg="(error unknown)";
            luaWrap_lua_pop(L,-1); // pop error from stack

            _displayScriptError(errMsg.c_str(),1);
            // Here we can decide what happens uppon error:
            App::ct->simulation->pauseOnErrorRequested();
        }
        else
        {
            if (CThreadPool::isThreadInFreeMode())
                CThreadPool::setThreadFreeMode(false);
        }
    }
    else
    { // A compilation error occurred!
        std::string errMsg;
        if (luaWrap_lua_isstring(L,-1))
            errMsg=std::string(luaWrap_lua_tostring(L,-1));
        else
            errMsg="(error unknown)";
        _displayScriptError(errMsg.c_str(),0);
        luaWrap_lua_pop(L,-1); // pop error from stack
        // Here we can decide what happens uppon error:
        App::ct->simulation->pauseOnErrorRequested();
    }
    _numberOfPasses++;

    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack
}

void CLuaScriptObject::killAddOnScriptState()
{
    FUNCTION_DEBUG;
    int runMode=4; //0=starting, 1=running, 2=pausing, 3=restarting, 4=deinit

    if ( (_scriptTextExecLength==0)||(_scriptTextExec==NULL)||(L==NULL) )
    {
        _addOn_executionState=3; // error
        return;
    }

    CApiErrors::pushLocation(scriptID); // for correct error handling (i.e. assignement to the correct script and output)

    int oldTop=luaWrap_lua_gettop(L);   // We store lua's stack

    std::string tmp("sim_current_script_id=");
    tmp+=boost::lexical_cast<std::string>(getScriptID());
    tmp+="\nsim_call_type=";
    tmp+=boost::lexical_cast<std::string>(runMode);
    luaWrap_luaL_dostring(L,tmp.c_str());

    if (_luaLoadBuffer(L,_scriptTextExec,_scriptTextExecLength-1,getShortDescriptiveName().c_str()))
    {
        luaWrap_lua_pushnumber(L,runMode);
        _scriptExecStartTime=VDateTime::getTimeInMs();
        App::ct->luaScriptContainer->setInAddOnNow(true);
        // Following 6 lines new since 8/11/2014:
        luaWrap_lua_getglobal(L,"debug");
        luaWrap_lua_getfield(L,-1,"traceback");
        luaWrap_lua_remove(L,-2);
        int argCnt=1;
        int errIndex=-argCnt-2;
        luaWrap_lua_insert(L,errIndex);
        if (_luaPCall(L,argCnt,luaWrapGet_LUA_MULTRET(),errIndex)!=0)
        { // a runtime error occurred!
            std::string errMsg;
            if (luaWrap_lua_isstring(L,-1))
                errMsg=std::string(luaWrap_lua_tostring(L,-1));
            else
                errMsg="(error unknown)";
            luaWrap_lua_pop(L,-1); // pop error from stack

            _displayScriptError(errMsg.c_str(),1);
            _addOn_executionState=0;// we restart the script next time it is activated //3; // error
            _addOn_desiredExecutionState=0;
        }
        else
        { // return values:
            int numberOfArgs=luaWrap_lua_gettop(L); // the first arg is linked to the debug mechanism
            if (numberOfArgs>=2)
            {
                int val=luaToInt(L,2);
                if (val==0)
                {
                    _addOn_executionState=0;// we restart the script next time it is activated
                    _addOn_desiredExecutionState=0;
                }
            }
        }
        App::ct->luaScriptContainer->setInAddOnNow(false);
    }
    else
    { // A compilation error occurred!
        std::string errMsg;
        if (luaWrap_lua_isstring(L,-1))
            errMsg=std::string(luaWrap_lua_tostring(L,-1));
        else
            errMsg="(error unknown)";
        _displayScriptError(errMsg.c_str(),0);
        luaWrap_lua_pop(L,-1); // pop error from stack
        _addOn_executionState=0;// we restart the script next time it is activated //3; // error
        _addOn_desiredExecutionState=0;
    }
    _numberOfPasses++;
    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack
    CApiErrors::popLocation(); // for correct error handling (i.e. assignement to the correct script and output)
}


void CLuaScriptObject::runAddOn()
{
    FUNCTION_DEBUG;
    if (_addOn_executionState==3)
        return;
    int runMode=0; //0=starting, 1=running, 2=pausing, 3=restarting
    if (_addOn_desiredExecutionState==1)
    { // we wanna run it
        if (_addOn_executionState==0)
            runMode=0;
        if (_addOn_executionState==1)
            runMode=1;
        if (_addOn_executionState==2)
            runMode=3;
        _addOn_executionState=1;
    }
    else
    { // we wanna pause it
        if (_addOn_executionState==0)
            return;
        if (_addOn_executionState==1)
            runMode=2;
        if (_addOn_executionState==2)
            return;
        _addOn_executionState=2;
    }

    if (_scriptTextExec==NULL)
    {
        if ( (_scriptTextLength!=0)&&(_scriptText!=NULL) )
        { // we copy the script buffer
            _scriptTextExec=new char[_scriptTextLength];
            _scriptTextExecLength=_scriptTextLength;
            for (int i=0;i<_scriptTextLength;i++)
                _scriptTextExec[i]=_scriptText[i];
        }
    }
    if ( (_scriptTextExecLength==0)||(_scriptTextExec==NULL) )
    {
        _addOn_executionState=3; // error
        return;
    }

    CApiErrors::pushLocation(scriptID); // for correct error handling (i.e. assignement to the correct script and output)

    if (L==NULL)
    {
        _errorReportMode=sim_api_error_output|sim_api_warning_output;
        _lastErrorString=SIM_API_CALL_NO_ERROR;
        L=initializeNewLuaState("");
    }
    int oldTop=luaWrap_lua_gettop(L);   // We store lua's stack

    std::string tmp("sim_current_script_id=");
    tmp+=boost::lexical_cast<std::string>(getScriptID());
    tmp+="\nsim_call_type=";
    tmp+=boost::lexical_cast<std::string>(runMode);
    luaWrap_luaL_dostring(L,tmp.c_str());

    if (_luaLoadBuffer(L,_scriptTextExec,_scriptTextExecLength-1,getShortDescriptiveName().c_str()))
    {
        luaWrap_lua_pushnumber(L,runMode);
        _scriptExecStartTime=VDateTime::getTimeInMs();
        App::ct->luaScriptContainer->setInAddOnNow(true);
        // Following 6 lines new since 8/11/2014:
        luaWrap_lua_getglobal(L,"debug");
        luaWrap_lua_getfield(L,-1,"traceback");
        luaWrap_lua_remove(L,-2);
        int argCnt=1;
        int errIndex=-argCnt-2;
        luaWrap_lua_insert(L,errIndex);
        if (_luaPCall(L,argCnt,luaWrapGet_LUA_MULTRET(),errIndex)!=0)
        { // a runtime error occurred!
            std::string errMsg;
            if (luaWrap_lua_isstring(L,-1))
                errMsg=std::string(luaWrap_lua_tostring(L,-1));
            else
                errMsg="(error unknown)";
            luaWrap_lua_pop(L,-1); // pop error from stack

            _displayScriptError(errMsg.c_str(),1);
            _addOn_executionState=0;// we restart the script next time it is activated //3; // error
            _addOn_desiredExecutionState=0;
        }
        else
        { // return values:
            int numberOfArgs=luaWrap_lua_gettop(L); // the first arg is linked to the debug mechanism
            if (numberOfArgs>=2)
            {
                int val=luaToInt(L,2);
                if (val==0)
                {
                    _addOn_executionState=0;// we restart the script next time it is activated
                    _addOn_desiredExecutionState=0;
                }
            }
        }
        App::ct->luaScriptContainer->setInAddOnNow(false);
    }
    else
    { // A compilation error occurred!
        std::string errMsg;
        if (luaWrap_lua_isstring(L,-1))
            errMsg=std::string(luaWrap_lua_tostring(L,-1));
        else
            errMsg="(error unknown)";
        _displayScriptError(errMsg.c_str(),0);
        luaWrap_lua_pop(L,-1); // pop error from stack
        _addOn_executionState=0;// we restart the script next time it is activated //3; // error
        _addOn_desiredExecutionState=0;
    }
    _numberOfPasses++;
    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack
    CApiErrors::popLocation(); // for correct error handling (i.e. assignement to the correct script and output)
}

void CLuaScriptObject::runJointCtrlCallback(const std::vector<bool>& inDataBool,const std::vector<int>& inDataInt,const std::vector<float>& inDataFloat,std::vector<float>& outDataFloat)
{ // args are not very generic, but for now that works fine (this routine is only used by joints with custom control loop)
    FUNCTION_DEBUG;
    outDataFloat.clear();
    if (_scriptTextExec==NULL)
    {
        if ( (_scriptTextLength!=0)&&(_scriptText!=NULL) )
        { // we copy the script buffer
            _scriptTextExec=new char[_scriptTextLength];
            _scriptTextExecLength=_scriptTextLength;
            for (int i=0;i<_scriptTextLength;i++)
                _scriptTextExec[i]=_scriptText[i];
        }
    }
    if ( (_scriptTextExecLength==0)||(_scriptTextExec==NULL) )
    {
        return; // error
    }

    CApiErrors::pushLocation(scriptID); // for correct error handling (i.e. assignement to the correct script and output)

    if (L==NULL)
    {
        _errorReportMode=sim_api_error_output|sim_api_warning_output;
        _lastErrorString=SIM_API_CALL_NO_ERROR;
        L=initializeNewLuaState(getScriptSuffixNumberString().c_str());
    }

    int oldTop=luaWrap_lua_gettop(L);   // We store lua's stack

    std::string tmp("sim_current_script_id=");
    tmp+=boost::lexical_cast<std::string>(getScriptID());
    luaWrap_luaL_dostring(L,tmp.c_str());

    if (_luaLoadBuffer(L,_scriptTextExec,_scriptTextExecLength-1,getShortDescriptiveName().c_str()))
    {
        for (int i=0;i<int(inDataBool.size());i++)
            luaWrap_lua_pushboolean(L,inDataBool[i]);
        for (int i=0;i<int(inDataInt.size());i++)
            luaWrap_lua_pushnumber(L,inDataInt[i]);
        for (int i=0;i<int(inDataFloat.size());i++)
            luaWrap_lua_pushnumber(L,inDataFloat[i]);
        _scriptExecStartTime=VDateTime::getTimeInMs();
        // Following 6 lines new since 8/11/2014:
        luaWrap_lua_getglobal(L,"debug");
        luaWrap_lua_getfield(L,-1,"traceback");
        luaWrap_lua_remove(L,-2);
        int argCnt=int(inDataBool.size())+int(inDataInt.size())+int(inDataFloat.size());
        int errIndex=-argCnt-2;
        luaWrap_lua_insert(L,errIndex);
        if (_luaPCall(L,argCnt,luaWrapGet_LUA_MULTRET(),errIndex)!=0)
        { // a runtime error occurred!
            std::string errMsg;
            if (luaWrap_lua_isstring(L,-1))
                errMsg=std::string(luaWrap_lua_tostring(L,-1));
            else
                errMsg="(error unknown)";
            luaWrap_lua_pop(L,-1); // pop error from stack

            _displayScriptError(errMsg.c_str(),1);
        }
        else
        { // return values:
            int numberOfArgs=luaWrap_lua_gettop(L); // the first arg is linked to the debug mechanism
            for (int i=1;i<numberOfArgs;i++)
            {
                if (luaWrap_lua_isnumber(L,i+1))
                    outDataFloat.push_back(float(luaWrap_lua_tonumber(L,i+1)));
            }
        }
    }
    else
    { // A compilation error occurred!
        std::string errMsg;
        if (luaWrap_lua_isstring(L,-1))
            errMsg=std::string(luaWrap_lua_tostring(L,-1));
        else
            errMsg="(error unknown)";
        _displayScriptError(errMsg.c_str(),0);
        luaWrap_lua_pop(L,-1); // pop error from stack
    }
    _numberOfPasses++;
    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack
    CApiErrors::popLocation(); // for correct error handling (i.e. assignement to the correct script and output)
}

bool CLuaScriptObject::runCustomizationScript(int callType)
{
    FUNCTION_DEBUG;
    // Can we run the script? Was it temporarily disabled? Is it allowed to run during simulation? etc. is checked here:
    if (!hasCustomizationScripAnyChanceToGetExecuted(App::ct->simulation->isSimulationRunning(),callType==sim_customizationscriptcall_cleanup))
        return(false);

    bool retVal=false;
    if (L==NULL)
    {   // The first time we call this script

        // Generate the sim_customizationscriptcall_initialization call:
        retVal=_runCustomizationScript(sim_customizationscriptcall_initialization);

        // Above call might have disabled the script. Check again:
        if (!hasCustomizationScripAnyChanceToGetExecuted(App::ct->simulation->isSimulationRunning(),false))
            return(false);

        if (App::ct->simulation->isSimulationPaused())
        {   // Here we have copied an object with customization script into the scene while simulation was paused.
            // We need to generate the sim_customizationscriptcall_simulationpausefirst call from here (special case):
            retVal=_runCustomizationScript(sim_customizationscriptcall_simulationpausefirst);

            // Above call might have disabled the script. Check again:
            if (!hasCustomizationScripAnyChanceToGetExecuted(App::ct->simulation->isSimulationRunning(),false))
                return(false);
        }
    }

    if (callType!=sim_customizationscriptcall_initialization) // sim_customizationscriptcall_initialization was already handled just here above
        retVal=_runCustomizationScript(callType);
    return(retVal);
}

bool CLuaScriptObject::_runCustomizationScript(int callType)
{
    if (_scriptTextExec==NULL)
    {
        if ( (_scriptTextLength!=0)&&(_scriptText!=NULL) )
        { // we copy the script buffer
            _scriptTextExec=new char[_scriptTextLength];
            _scriptTextExecLength=_scriptTextLength;
            for (int i=0;i<_scriptTextLength;i++)
                _scriptTextExec[i]=_scriptText[i];
        }
    }
    if ( (_scriptTextExecLength==0)||(_scriptTextExec==NULL) )
        return(false); // error

    CApiErrors::pushLocation(scriptID); // for correct error handling (i.e. assignement to the correct script and output)

    if (L==NULL)
    {
        _errorReportMode=sim_api_error_output|sim_api_warning_output;
        _lastErrorString=SIM_API_CALL_NO_ERROR;
        L=initializeNewLuaState(getScriptSuffixNumberString().c_str());
    }

    int oldTop=luaWrap_lua_gettop(L);   // We store lua's stack

    std::string tmp("sim_current_script_id=");
    tmp+=boost::lexical_cast<std::string>(getScriptID());
    tmp+="\nsim_call_type=";
    tmp+=boost::lexical_cast<std::string>(callType);
    luaWrap_luaL_dostring(L,tmp.c_str());

    if (_luaLoadBuffer(L,_scriptTextExec,_scriptTextExecLength-1,getShortDescriptiveName().c_str()))
    {
        App::ct->luaScriptContainer->setInCustomizationScriptNow(true);
        _scriptExecStartTime=VDateTime::getTimeInMs();
        // Following 6 lines new since 8/11/2014:
        luaWrap_lua_getglobal(L,"debug");
        luaWrap_lua_getfield(L,-1,"traceback");
        luaWrap_lua_remove(L,-2);
        int argCnt=0;
        int errIndex=-argCnt-2;
        luaWrap_lua_insert(L,errIndex);
        if (_luaPCall(L,argCnt,luaWrapGet_LUA_MULTRET(),errIndex)!=0)
        { // a runtime error occurred!
            std::string errMsg;
            if (luaWrap_lua_isstring(L,-1))
                errMsg=std::string(luaWrap_lua_tostring(L,-1));
            else
                errMsg="(error unknown)";
            luaWrap_lua_pop(L,-1); // pop error from stack

            _displayScriptError(errMsg.c_str(),1);
        }
        else
        { // return values:
            int numberOfArgs=luaWrap_lua_gettop(L); // the first arg is linked to the debug mechanism
            for (int i=1;i<numberOfArgs;i++)
            {
//              if (luaWrap_lua_isnumber(L,i+1))
//                  outDataFloat.push_back(float(luaWrap_lua_tonumber(L,i+1)));
            }
        }
        App::ct->luaScriptContainer->setInCustomizationScriptNow(false);
    }
    else
    { // A compilation error occurred!
        std::string errMsg;
        if (luaWrap_lua_isstring(L,-1))
            errMsg=std::string(luaWrap_lua_tostring(L,-1));
        else
            errMsg="(error unknown)";
        _displayScriptError(errMsg.c_str(),0);
        luaWrap_lua_pop(L,-1); // pop error from stack
    }
    _numberOfPasses++;
    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack
    CApiErrors::popLocation(); // for correct error handling (i.e. assignement to the correct script and output)
    return(true);
}



int CLuaScriptObject::runContactCallback(const int inDataInt[3],int outDataInt[3],float outDataFloat[14])
{ // args are not very generic, but for now that works fine
    int retVal=0;
    if (_scriptTextExec==NULL)
    {
        if ( (_scriptTextLength!=0)&&(_scriptText!=NULL) )
        { // we copy the script buffer
            _scriptTextExec=new char[_scriptTextLength];
            _scriptTextExecLength=_scriptTextLength;
            for (int i=0;i<_scriptTextLength;i++)
                _scriptTextExec[i]=_scriptText[i];
        }
    }
    if ( (_scriptTextExecLength==0)||(_scriptTextExec==NULL) )
        return(retVal); // error

    CApiErrors::pushLocation(scriptID); // for correct error handling (i.e. assignement to the correct script and output)

    if (L==NULL)
    {
        _errorReportMode=sim_api_error_output|sim_api_warning_output;
        _lastErrorString=SIM_API_CALL_NO_ERROR;
        L=initializeNewLuaState("");
    }

    int oldTop=luaWrap_lua_gettop(L);   // We store lua's stack

    std::string tmp("sim_current_script_id=");
    tmp+=boost::lexical_cast<std::string>(getScriptID());
    luaWrap_luaL_dostring(L,tmp.c_str());

    if (_luaLoadBuffer(L,_scriptTextExec,_scriptTextExecLength-1,getShortDescriptiveName().c_str()))
    {
        for (int i=0;i<3;i++)
            luaWrap_lua_pushnumber(L,inDataInt[i]);
        _scriptExecStartTime=VDateTime::getTimeInMs();
        // Following 6 lines new since 8/11/2014:
        luaWrap_lua_getglobal(L,"debug");
        luaWrap_lua_getfield(L,-1,"traceback");
        luaWrap_lua_remove(L,-2);
        int argCnt=3;
        int errIndex=-argCnt-2;
        luaWrap_lua_insert(L,errIndex);
        if (_luaPCall(L,argCnt,luaWrapGet_LUA_MULTRET(),errIndex)!=0)
        { // a runtime error occurred!
            std::string errMsg;
            if (luaWrap_lua_isstring(L,-1))
                errMsg=std::string(luaWrap_lua_tostring(L,-1));
            else
                errMsg="(error unknown)";
            luaWrap_lua_pop(L,-1); // pop error from stack

            _displayScriptError(errMsg.c_str(),1);
        }
        else
        { // return values:
            int numberOfArgs=luaWrap_lua_gettop(L); // the first arg is linked to the debug mechanism
            if (numberOfArgs>=2)
            {
                retVal=luaToInt(L,2);
                if (retVal>0)
                {
                    if ((numberOfArgs>=4)&&luaWrap_lua_istable(L,3)&&luaWrap_lua_istable(L,4))
                    { // we expect 2 tables!
                        getIntsFromTable(L,3,SIM_MIN(int(luaWrap_lua_objlen(L,3)),3),outDataInt);
                        getFloatsFromTable(L,4,SIM_MIN(int(luaWrap_lua_objlen(L,4)),14),outDataFloat);
                    }
                    else
                        retVal=0; // error
                }
            }
        }
    }
    else
    { // A compilation error occurred!
        std::string errMsg;
        if (luaWrap_lua_isstring(L,-1))
            errMsg=std::string(luaWrap_lua_tostring(L,-1));
        else
            errMsg="(error unknown)";
        _displayScriptError(errMsg.c_str(),0);
        luaWrap_lua_pop(L,-1); // pop error from stack
    }
    _numberOfPasses++;
    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack
    CApiErrors::popLocation(); // for correct error handling (i.e. assignement to the correct script and output)
    return(retVal);
}

int CLuaScriptObject::runGeneralCallback(int callbackId,int callbackTag,void* additionalData)
{
    int retVal=-1; // means error
    if (_scriptTextExec==NULL)
    {
        if ( (_scriptTextLength!=0)&&(_scriptText!=NULL) )
        { // we copy the script buffer
            _scriptTextExec=new char[_scriptTextLength];
            _scriptTextExecLength=_scriptTextLength;
            for (int i=0;i<_scriptTextLength;i++)
                _scriptTextExec[i]=_scriptText[i];
        }
    }
    if ( (_scriptTextExecLength==0)||(_scriptTextExec==NULL) )
        return(retVal); // error

    CApiErrors::pushLocation(scriptID); // for correct error handling (i.e. assignement to the correct script and output)

    if (L==NULL)
    {
        _errorReportMode=sim_api_error_output|sim_api_warning_output;
        _lastErrorString=SIM_API_CALL_NO_ERROR;
        L=initializeNewLuaState("");
    }

    int oldTop=luaWrap_lua_gettop(L);   // We store lua's stack

    std::string tmp("sim_current_script_id=");
    tmp+=boost::lexical_cast<std::string>(getScriptID());
    tmp+="\nsim_callback_id=";
    tmp+=boost::lexical_cast<std::string>(callbackId);
    tmp+="\nsim_callback_tag=";
    tmp+=boost::lexical_cast<std::string>(callbackTag);
    luaWrap_luaL_dostring(L,tmp.c_str());

    if (_luaLoadBuffer(L,_scriptTextExec,_scriptTextExecLength-1,getShortDescriptiveName().c_str()))
    {
        _scriptExecStartTime=VDateTime::getTimeInMs();
        // Following 6 lines new since 8/11/2014:
        luaWrap_lua_getglobal(L,"debug");
        luaWrap_lua_getfield(L,-1,"traceback");
        luaWrap_lua_remove(L,-2);
        int argCnt=0;
        int errIndex=-argCnt-2;
        luaWrap_lua_insert(L,errIndex);
        if (_luaPCall(L,argCnt,luaWrapGet_LUA_MULTRET(),errIndex)!=0)
        { // a runtime error occurred!
            std::string errMsg;
            if (luaWrap_lua_isstring(L,-1))
                errMsg=std::string(luaWrap_lua_tostring(L,-1));
            else
                errMsg="(error unknown)";
            luaWrap_lua_pop(L,-1); // pop error from stack

            _displayScriptError(errMsg.c_str(),1);
        }
        else
        { // return values:
            int numberOfArgs=luaWrap_lua_gettop(L); // the first arg is linked to the debug mechanism
            if (numberOfArgs>=2)
                retVal=luaToInt(L,2);
        }
    }
    else
    { // A compilation error occurred!
        std::string errMsg;
        if (luaWrap_lua_isstring(L,-1))
            errMsg=std::string(luaWrap_lua_tostring(L,-1));
        else
            errMsg="(error unknown)";
        _displayScriptError(errMsg.c_str(),0);
        luaWrap_lua_pop(L,-1); // pop error from stack
    }
    _numberOfPasses++;
    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack
    CApiErrors::popLocation(); // for correct error handling (i.e. assignement to the correct script and output)
    return(retVal);
}


int CLuaScriptObject::callScriptFunction(const char* functionName,SLuaCallBack* pdata)
{
    int retVal=-1; // means error

    if (!_initializeScriptForFunctionCallOrVarSettingIfNeeded())
        return(retVal);

    CApiErrors::pushLocation(scriptID); // for correct error handling (i.e. assignement to the correct script and output)
    int oldTop=luaWrap_lua_gettop(L);   // We store lua's stack

    // New since 7/3/2016:
    std::string tmp("sim_current_script_id=");
    tmp+=boost::lexical_cast<std::string>(getScriptID());
    tmp+="\nsim_call_type=-1";
    luaWrap_luaL_dostring(L,tmp.c_str());

    // Push the function name onto the stack (will be automatically popped from stack after _luaPCall):
    luaWrap_lua_getglobal(L,functionName);

    // Push the arguments onto the stack (will be automatically popped from stack after _luaPCall):
    int inputArgs=pdata->inputArgCount;
    if (inputArgs!=0)
    {
        int boolPt=0;
        int intPt=0;
        int floatPt=0;
        int doublePt=0;
        int stringPt=0;
        int stringBuffPt=0;
        for (int i=0;i<pdata->inputArgCount;i++)
            writeCustomFunctionDataOntoStack(L,pdata->inputArgTypeAndSize[2*i+0],pdata->inputArgTypeAndSize[2*i+1],pdata->inputBool,boolPt,pdata->inputInt,intPt,pdata->inputFloat,floatPt,pdata->inputDouble,doublePt,pdata->inputChar,stringPt,pdata->inputCharBuff,stringBuffPt);
    }

    // Following 6 lines new since 7/3/2016:
    luaWrap_lua_getglobal(L,"debug");
    luaWrap_lua_getfield(L,-1,"traceback");
    luaWrap_lua_remove(L,-2);
    int argCnt=inputArgs;
    int errindex=-argCnt-2;
    luaWrap_lua_insert(L,errindex);

    // Following line new since 7/3/2016:
    if (_luaPCall(L,argCnt,luaWrapGet_LUA_MULTRET(),errindex)!=0)
//  if (_luaPCall(L,inputArgs,luaWrapGet_LUA_MULTRET(),0)!=0)
    { // a runtime error occurred!
        std::string errMsg;
        if (luaWrap_lua_isstring(L,-1))
            errMsg=std::string(luaWrap_lua_tostring(L,-1));
        else
            errMsg="(error unknown)";
        luaWrap_lua_pop(L,-1); // pop error from stack
        _displayScriptError(errMsg.c_str(),1);

        // Following probably not needed:
        pdata->outputBool=new unsigned char[0];
        pdata->outputInt=new int[0];
        pdata->outputFloat=new float[0];
        pdata->outputDouble=new double[0];
        pdata->outputChar=new char[0];
        pdata->outputCharBuff=new char[0];
    }
    else
    { // return values:
        int currentTop=luaWrap_lua_gettop(L);

        // Following line new since 7/3/2016:
        int numberOfArgs=currentTop-oldTop-1; // the first arg is linked to the debug mechanism
//      int numberOfArgs=currentTop-oldTop;

        // We read all arguments from the stack
        std::vector<char> outBoolVector;
        std::vector<int> outIntVector;
        std::vector<float> outFloatVector;
        std::vector<double> outDoubleVector;
        std::vector<std::string> outStringVector;
        std::vector<std::string> outCharVector;
        std::vector<int> outInfoVector;
        for (int i=0;i<numberOfArgs;i++)
        {
            // Following line new since 7/3/2016:
            if (!readCustomFunctionDataFromStack(L,oldTop+i+1+1,pdata->outputArgTypeAndSize[i*2+0],outBoolVector,outIntVector,outFloatVector,outDoubleVector,outStringVector,outCharVector,outInfoVector))
//          if (!readCustomFunctionDataFromStack(L,oldTop+i+1,pdata->outputArgTypeAndSize[i*2+0],outBoolVector,outIntVector,outFloatVector,outDoubleVector,outStringVector,outCharVector,outInfoVector))
                break;
        }

        // Now we prepare the output buffers:
        pdata->outputBool=new unsigned char[outBoolVector.size()];
        pdata->outputInt=new int[outIntVector.size()];
        pdata->outputFloat=new float[outFloatVector.size()];
        pdata->outputDouble=new double[outDoubleVector.size()];
        int charCnt=0;
        for (int k=0;k<int(outStringVector.size());k++)
            charCnt+=outStringVector[k].length()+1; // terminal 0
        pdata->outputChar=new char[charCnt];

        int charBuffCnt=0;
        for (int k=0;k<int(outCharVector.size());k++)
            charBuffCnt+=outCharVector[k].length();
        pdata->outputCharBuff=new char[charBuffCnt];

        pdata->outputArgCount=outInfoVector.size()/2;
        delete[] pdata->outputArgTypeAndSize;
        pdata->outputArgTypeAndSize=new int[outInfoVector.size()];
        // We fill the output buffers:
        for (int k=0;k<int(outBoolVector.size());k++)
            pdata->outputBool[k]=outBoolVector[k];
        for (int k=0;k<int(outIntVector.size());k++)
            pdata->outputInt[k]=outIntVector[k];
        for (int k=0;k<int(outFloatVector.size());k++)
            pdata->outputFloat[k]=outFloatVector[k];
        for (int k=0;k<int(outDoubleVector.size());k++)
            pdata->outputDouble[k]=outDoubleVector[k];
        charCnt=0;
        for (int k=0;k<int(outStringVector.size());k++)
        {
            for (int l=0;l<int(outStringVector[k].length());l++)
                pdata->outputChar[charCnt+l]=outStringVector[k][l];
            charCnt+=outStringVector[k].length();
            // terminal 0:
            pdata->outputChar[charCnt]=0;
            charCnt++;
        }

        charBuffCnt=0;
        for (int k=0;k<int(outCharVector.size());k++)
        {
            for (int l=0;l<int(outCharVector[k].length());l++)
                pdata->outputCharBuff[charBuffCnt+l]=outCharVector[k][l];
            charBuffCnt+=outCharVector[k].length();
        }

        for (int k=0;k<int(outInfoVector.size());k++)
            pdata->outputArgTypeAndSize[k]=outInfoVector[k];

        retVal=0;
    }
    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack
    CApiErrors::popLocation(); // for correct error handling (i.e. assignement to the correct script and output)
    return(retVal);
}

int CLuaScriptObject::callScriptFunctionEx(const char* functionName,CInterfaceStack* stack)
{
    int retVal=-1; // means error

    if (!_initializeScriptForFunctionCallOrVarSettingIfNeeded())
        return(retVal);

    CApiErrors::pushLocation(scriptID); // for correct error handling (i.e. assignement to the correct script and output)
    int oldTop=luaWrap_lua_gettop(L);   // We store lua's stack

    // New since 7/3/2016:
    std::string tmp("sim_current_script_id=");
    tmp+=boost::lexical_cast<std::string>(getScriptID());
    tmp+="\nsim_call_type=-1";
    luaWrap_luaL_dostring(L,tmp.c_str());


    // Push the function name onto the stack (will be automatically popped from stack after _luaPCall):
    luaWrap_lua_getglobal(L,functionName);

    // Push the arguments onto the stack (will be automatically popped from stack after _luaPCall):
    int inputArgs=stack->getStackSize();

    if (inputArgs!=0)
        stack->buildOntoLuaStack(L,false);

    stack->clear();

    // Following 6 lines new since 7/3/2016:
    luaWrap_lua_getglobal(L,"debug");
    luaWrap_lua_getfield(L,-1,"traceback");
    luaWrap_lua_remove(L,-2);
    int argCnt=inputArgs;
    int errindex=-argCnt-2;
    luaWrap_lua_insert(L,errindex);

    // Following line new since 7/3/2016:
    if (_luaPCall(L,argCnt,luaWrapGet_LUA_MULTRET(),errindex)!=0)
//  if (_luaPCall(L,inputArgs,luaWrapGet_LUA_MULTRET(),0)!=0)
    { // a runtime error occurred!
        std::string errMsg;
        if (luaWrap_lua_isstring(L,-1))
            errMsg=std::string(luaWrap_lua_tostring(L,-1));
        else
            errMsg="(error unknown)";
        luaWrap_lua_pop(L,-1); // pop error from stack
        _displayScriptError(errMsg.c_str(),1);
    }
    else
    { // return values:
        int currentTop=luaWrap_lua_gettop(L);

        // Following 2 lines new since 7/3/2016:
        int numberOfArgs=currentTop-oldTop-1; // the first arg is linked to the debug mechanism
        stack->buildFromLuaStack(L,oldTop+1+1,numberOfArgs); // the first arg is linked to the debug mechanism
//      int numberOfArgs=currentTop-oldTop;
//      stack->buildFromLuaStack(L,oldTop+1,numberOfArgs);
        retVal=0;
    }
    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack
    CApiErrors::popLocation(); // for correct error handling (i.e. assignement to the correct script and output)
    return(retVal);
}

int CLuaScriptObject::setScriptVariable(const char* variableName,CInterfaceStack* stack)
{
    if (!_initializeScriptForFunctionCallOrVarSettingIfNeeded())
        return(-1);

    int oldTop=luaWrap_lua_gettop(L);   // We store lua's stack

    if (stack!=NULL)
        stack->buildOntoLuaStack(L,true);
    else
        luaWrap_lua_pushnil(L);
    luaWrap_lua_setglobal(L,variableName);

    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack
    return(0);
}

int CLuaScriptObject::clearVariable(const char* variableName)
{
    if ((_scriptTextExec==NULL)||(L==NULL))
        return(-1);

    if (_threadedExecution)
    {
        if (!VThread::areThreadIDsSame(_threadedScript_associatedFiberOrThreadID,VThread::getCurrentThreadId()))
            return(-1); // only from the same thread when threaded!!
    }

    if (_scriptIsDisabled)
        return(-1);


    int oldTop=luaWrap_lua_gettop(L);   // We store lua's stack

    luaWrap_lua_pushnil(L);
    luaWrap_lua_setglobal(L,variableName);

    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack
    return(0);
}


void CLuaScriptObject::killLuaState()
{
    if (L!=NULL)
    {
        CApiErrors::pushLocation(scriptID); // for correct error handling (i.e. assignement to the correct script and output)
        if (_scriptType==sim_scripttype_customizationscript)
        {
            if (hasCustomizationScripAnyChanceToGetExecuted(false,true)) // run the last pass even if we do not wish to execute the script during simulation
                _runCustomizationScript(sim_customizationscriptcall_cleanup);
        }

        if (_scriptType==sim_scripttype_mainscript)
        {
            // Not needed for the main script (was already handled elsewhere!)
        }

        if ((_scriptType==sim_scripttype_childscript)&&(!_threadedExecution) )
        {
            CInterfaceStack in;
            // Following few not elegant, but important otherwise we don't have the emergency stop button
            bool wasInMainScript=App::ct->luaScriptContainer->getInMainScriptNow();
            if (!wasInMainScript)
                App::ct->luaScriptContainer->setInMainScriptNow(true,VDateTime::getTimeInMs());
            _runNonThreadedChildScriptNow(in,sim_childscriptcall_cleanup);
            if (!wasInMainScript)
                App::ct->luaScriptContainer->setInMainScriptNow(false,0);
        }
        CApiErrors::popLocation(); // for correct error handling (i.e. assignement to the correct script and output)
        luaWrap_lua_close(L);
    }
    L=NULL;
    _loadBufferResult=-1;
    delete[] _scriptTextExec;
    _scriptTextExec=NULL;
    _scriptTextExecLength=0;
    _customizationScriptIsTemporarilyDisabled=false;

    //scriptParameters->clearInitialParameters();
}

int CLuaScriptObject::setUserData(char* data)
{
    _userData.push_back(data);
    int id=0;
    for (int i=0;i<int(_userDataIds.size());i++)
    {
        if (_userDataIds[i]==id)
        {
            id++;
            i=-1; // we need to restart the search from the beginning!
        }
    }
    _userDataIds.push_back(id);
    return(id);
}

char* CLuaScriptObject::getUserData(int id) const
{
    for (size_t i=0;i<_userDataIds.size();i++)
    {
        if (_userDataIds[i]==id)
            return(_userData[i]);
    }
    return(NULL);
}

void CLuaScriptObject::releaseUserData(int id)
{
    for (int i=0;i<int(_userDataIds.size());i++)
    {
        if (_userDataIds[i]==id)
        {
            delete[] _userData[i];
            _userData.erase(_userData.begin()+i);
            _userDataIds.erase(_userDataIds.begin()+i);
            break;
        }
    }
}

void CLuaScriptObject::clearAllUserData()
{
    for (int i=0;i<int(_userData.size());i++)
        delete[] _userData[i];
    _userData.clear();
    _userDataIds.clear();
}

CLuaScriptObject* CLuaScriptObject::copyYourself()
{
    CLuaScriptObject* it=new CLuaScriptObject(_scriptType);
    it->scriptID=scriptID;
    it->_objectIDAttachedTo_child=_objectIDAttachedTo_child;
    it->_objectIDAttachedTo_callback=_objectIDAttachedTo_callback;
    it->_objectIDAttachedTo_customization=_objectIDAttachedTo_customization;
    it->_threadedExecution=_threadedExecution;
    it->_scriptIsDisabled=_scriptIsDisabled;
    it->_executionOrder=_executionOrder;
    it->_customizationScriptIsTemporarilyDisabled=_customizationScriptIsTemporarilyDisabled;
    std::vector<int> foldingThing;
    it->setScriptText(getScriptText(&foldingThing),&foldingThing);
//  // DONT COPY THE _adjustNameSuffix-THING!!!!
    delete it->scriptParameters;
    it->scriptParameters=scriptParameters->copyYourself();
    it->_executeJustOnce=_executeJustOnce;

    delete it->_customObjectData;
    it->_customObjectData=NULL;
    if (_customObjectData!=NULL)
        it->_customObjectData=_customObjectData->copyYourself();

    delete it->_customObjectData_tempData;
    it->_customObjectData_tempData=NULL;
    if (_customObjectData_tempData!=NULL)
        it->_customObjectData_tempData=_customObjectData_tempData->copyYourself();

    it->_initialValuesInitialized=_initialValuesInitialized;
    return(it);
}

CLuaScriptParameters* CLuaScriptObject::getScriptParametersObject()
{
    return(scriptParameters);
}

bool CLuaScriptObject::addCommandToOutsideCommandQueue(int commandID,int auxVal1,int auxVal2,int auxVal3,int auxVal4,const float aux2Vals[8],int aux2Count)
{
    if (_outsideCommandQueue!=NULL)
        return(_outsideCommandQueue->addCommand(commandID,auxVal1,auxVal2,auxVal3,auxVal4,aux2Vals,aux2Count));
    return(true);
}

int CLuaScriptObject::extractCommandFromOutsideCommandQueue(int auxVals[4],float aux2Vals[8],int& aux2Count)
{
    if (_outsideCommandQueue!=NULL)
        return(_outsideCommandQueue->extractOneCommand(auxVals,aux2Vals,aux2Count));
    return(-1);
}

bool CLuaScriptObject::_initializeScriptForFunctionCallOrVarSettingIfNeeded()
{
    if ( ( (_scriptTextExec==NULL)||(L==NULL) )&&_threadedExecution )
        return(false); // threaded scripts must be called from within, or already initialized!

    if (_threadedExecution)
    {
        if (!VThread::areThreadIDsSame(_threadedScript_associatedFiberOrThreadID,VThread::getCurrentThreadId()))
            return(false); // only from the same thread when threaded!!
    }

    if (_scriptIsDisabled)
        return(false);

    if ((_scriptTextExec!=NULL)&&(L!=NULL))
        return(true);

    // this script was never run before!
    if (_scriptType==sim_scripttype_generalcallback)
        runGeneralCallback(-1,0,NULL); // dummy call
    if (_scriptType==sim_scripttype_customizationscript)
        runCustomizationScript(sim_customizationscriptcall_initialization); // dummy call
    if (!App::ct->simulation->isSimulationStopped())
    { // following only when simulation is running!
        if (_scriptType==sim_scripttype_contactcallback)
        {
            int in[3]={0,0,-1};
            int out1[3];
            float out2[14];
            runContactCallback(in,out1,out2); // dummy call
        }
        if (_scriptType==sim_scripttype_childscript)
        {
            CInterfaceStack in;
            runNonThreadedChildScript(in,sim_childscriptcall_initialization); // dummy call
        }
        if (_scriptType==sim_scripttype_jointctrlcallback)
        {
            std::vector<bool> inDataBool;
            std::vector<int> inDataInt;
            std::vector<float> inDataFloat;
            std::vector<float> outData;
            inDataBool.push_back(true); // init
            inDataBool.push_back(true); // revolute
            inDataBool.push_back(true); // cyclic
            inDataInt.push_back(-1); // joint ID
            inDataInt.push_back(0); // pass count
            inDataInt.push_back(0); // total passes
            inDataFloat.push_back(0.0f); // current pos
            inDataFloat.push_back(0.0f); // target pos
            inDataFloat.push_back(0.0f); // error v
            inDataFloat.push_back(0.0f); // effort
            inDataFloat.push_back(0.0f); // dyn step size
            inDataFloat.push_back(0.0f); // low limit
            inDataFloat.push_back(0.0f); // high limit
            inDataFloat.push_back(0.0f); // target vel
            inDataFloat.push_back(0.0f); // max force
            inDataFloat.push_back(0.0f); // upper vel limit
            runJointCtrlCallback(inDataBool,inDataInt,inDataFloat,outData); // dummy call
        }
    }

    return((_scriptTextExec!=NULL)&&(L!=NULL));
}

void CLuaScriptObject::_displayScriptError(const char* errMsg,int errorType)
{
    std::string message("Lua unknown error: ");
    if (errorType==0)
        message="Lua compilation error: ";
    if (errorType==1)
    {
        std::string err(errMsg);
        if (err.find("attempt to yield across metamethod/C-call boundary")!=std::string::npos)
            return; // silent error when breaking out of a threaded child script at simulation end
        message="Lua runtime error: ";
    }
    message+=errMsg;
    App::addStatusbarMessage(message.c_str());
    SUIThreadCommand cmdIn;
    SUIThreadCommand cmdOut;
    cmdIn.cmdId=FLASH_STATUSBAR_UITHREADCMD;
    App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
}


void CLuaScriptObject::serialize(CSer& ar)
{
    if (ar.isStoring())
    {       // Storing
        ar.storeDataName("Si2");
        ar << scriptID << _objectIDAttachedTo_child << _scriptType;
        ar.flush();

        // Keep following close to the beginning!
        ar.storeDataName("Va2");
        unsigned char nothing=0;
        SIM_SET_CLEAR_BIT(nothing,0,_threadedExecution);
        SIM_SET_CLEAR_BIT(nothing,1,_scriptIsDisabled);
        // RESERVED
        SIM_SET_CLEAR_BIT(nothing,3,!_mainScriptIsDefaultMainScript);
        SIM_SET_CLEAR_BIT(nothing,4,_executeJustOnce);
        // RESERVED!!
        SIM_SET_CLEAR_BIT(nothing,6,true); // this indicates we have the 'almost' new script execution engine (since V3.1.3)
        SIM_SET_CLEAR_BIT(nothing,7,true); // this indicates we have the new script execution engine (since V3.1.3)
        ar << nothing;
        ar.flush();

        // Keep following close to the beginning!
        ar.storeDataName("Va3"); // this is also used as a marked for the color correction introduced in V3.1.4 and later!
        nothing=0;
        SIM_SET_CLEAR_BIT(nothing,0,true); // needed for a code correction
        // FREE SIM_SET_CLEAR_BIT(nothing,1,XXXXX);
        // FREE SIM_SET_CLEAR_BIT(nothing,2,XXXXX);
        // FREE SIM_SET_CLEAR_BIT(nothing,3,XXXXX);
        // FREE SIM_SET_CLEAR_BIT(nothing,4,XXXXX);
        // FREE SIM_SET_CLEAR_BIT(nothing,5,XXXXX);
        // FREE SIM_SET_CLEAR_BIT(nothing,6,XXXXX);
        // FREE SIM_SET_CLEAR_BIT(nothing,7,XXXXX);
        ar << nothing;
        ar.flush();

        ar.storeDataName("Seo");
        ar << _executionOrder;
        ar.flush();


        std::string stt(_scriptText,_scriptTextLength);
        bool storeAlsoFoldingInfo=true;
        if (!App::ct->undoBufferContainer->isUndoSavingOrRestoringUnderWay())
        { // normal serialization (i.e. no undo/redo serialization):
            if (App::ct->luaScriptContainer->getSaveIncludeScriptFiles())
            {
                std::string filename(getIncludeScriptFilePathAndName());
                if (VFile::doesFileExist(filename))
                {
                    storeAlsoFoldingInfo=false;
                    try
                    {
                        VFile file(filename,VFile::READ|VFile::SHARE_DENY_NONE);
                        VArchive archive(&file,VArchive::LOAD);
                        stt.clear();
                        size_t archiveLength=(size_t)file.getLength();
                        char chr;
                        for (size_t fl=0;fl<archiveLength;fl++)
                        {
                            archive >> chr;
                            stt.push_back(chr);
                        }
                        stt.push_back('\0');
                        archive.close();
                        file.close();
                    }
                    catch(VFILE_EXCEPTION_TYPE e)
                    {
                    }
                }
            }
        }

        // We store scripts in a light encoded way:
        ar.storeDataName("Ste");
        tt::lightEncodeBuffer(&stt[0],stt.length());
        for (size_t i=0;i<stt.length();i++)
            ar << stt[i];
        ar.flush();

        if (storeAlsoFoldingInfo)
        {
            ar.storeDataName("Sfi");
            ar << int(_scriptFoldingInfo.size());
            for (int i=0;i<int(_scriptFoldingInfo.size());i++)
                ar << _scriptFoldingInfo[i];
            ar.flush();
        }


        ar.storeDataName("Prm");
        ar.setCountingMode();
        scriptParameters->serialize(ar);
        if (ar.setWritingMode())
            scriptParameters->serialize(ar);

        ar.storeDataName("Coi");
        ar << _objectIDAttachedTo_callback;
        ar.flush();

        ar.storeDataName("Coc");
        ar << _objectIDAttachedTo_customization;
        ar.flush();

        if (_customObjectData!=NULL)
        {
            ar.storeDataName("Cod");
            ar.setCountingMode();
            _customObjectData->serializeData(ar);
            if (ar.setWritingMode())
                _customObjectData->serializeData(ar);
        }

        ar.storeDataName(SER_END_OF_OBJECT);
    }
    else
    {       // Loading
        int byteQuantity;
        std::string theName="";
        bool backwardCompatibility_7_8_2014=false;
        bool backwardCompatibility_13_8_2014=false;
        bool executeInSensingPhase_oldCompatibility_7_8_2014=false;
        bool backwardCompatibilityCorrectionNeeded_13_10_2014=true;
        bool backwardCompatibilityCorrectionNeeded_8_11_2014=true;
        while (theName.compare(SER_END_OF_OBJECT)!=0)
        {
            theName=ar.readDataName();
            if (theName.compare(SER_END_OF_OBJECT)!=0)
            {
                bool noHit=true;
                bool justLoadedCustomScriptBuffer=false;
                if (theName.compare("Si2")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> scriptID >> _objectIDAttachedTo_child >> _scriptType;
                }
                if (theName.compare("Seo")==0)
                { 
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> _executionOrder;
                }

                if (theName.compare("Sfi")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    int q;
                    ar >> q;
                    _scriptFoldingInfo.resize(q);
                    for (int i=0;i<q;i++)
                        ar >> _scriptFoldingInfo[i];
                }

                if (theName.compare("Ste")==0)
                { // The script is stored encoded!
                    noHit=false;
                    ar >> byteQuantity;
                    _scriptTextLength=byteQuantity;
                    if (_scriptText!=NULL)
                        delete[] _scriptText;
                    _scriptText=NULL;
                    if (_scriptTextLength!=0)
                    {
                        _scriptText=new char[_scriptTextLength];
                        char dummy;
                        for (int i=0;i<_scriptTextLength;i++)
                        {
                            ar >> dummy;
                            _scriptText[i]=dummy;
                        }
                        tt::lightDecodeBuffer(_scriptText,_scriptTextLength);
                    }
                    justLoadedCustomScriptBuffer=true;
                }
                if (justLoadedCustomScriptBuffer)
                { // We just loaded the script text.
                    if (_scriptType==sim_scripttype_mainscript)
                    { // We just loaded a main script text. Do we want to load the default main script instead?
                        if (_mainScriptIsDefaultMainScript)
                        { // Yes!
                            std::string filenameAndPath(App::directories->systemDirectory+VREP_SLASH);
                            filenameAndPath+=DEFAULT_MAINSCRIPT_NAME;
                            if (VFile::doesFileExist(filenameAndPath))
                            {
                                try
                                {
                                    VFile file(filenameAndPath.c_str(),VFile::READ|VFile::SHARE_DENY_NONE);
                                    VArchive archive2(&file,VArchive::LOAD);
                                    unsigned int archiveLength=(unsigned int)file.getLength();
                                    // We replace current script with a default main script
                                    _scriptTextLength=archiveLength+1;
                                    if (_scriptText!=NULL)
                                        delete[] _scriptText;
                                    _scriptText=NULL;
                                    if (_scriptTextLength!=0)
                                    {
                                        _scriptText=new char[_scriptTextLength];
                                        char dummy;
                                        for (int i=0;i<_scriptTextLength-1;i++)
                                        {
                                            archive2 >> dummy;
                                            _scriptText[i]=dummy;
                                        }
                                    }
                                    _scriptText[_scriptTextLength-1]=0;
                                    archive2.close();
                                    file.close();
                                }
                                catch(VFILE_EXCEPTION_TYPE e)
                                {
                                    VFile::reportAndHandleFileExceptionError(e);
// Removed following line on 2010/03/03: even if the default main script is not there, we might still want the default main script next time (if there maybe).
// Following line also causes problems when converting to a new fileformat!
//                              _mainScriptIsDefaultMainScript=false; // We couldn't find the default main script, we turn this one into a customized main script!
                                }
                            }
                        }
                    }
                }
                if (theName=="Va2")
                {
                    noHit=false;
                    ar >> byteQuantity;
                    unsigned char nothing;
                    ar >> nothing;
                    _threadedExecution=SIM_IS_BIT_SET(nothing,0);
                    _scriptIsDisabled=SIM_IS_BIT_SET(nothing,1);
                    // RESERVED
                    _mainScriptIsDefaultMainScript=!SIM_IS_BIT_SET(nothing,3);
                    _executeJustOnce=SIM_IS_BIT_SET(nothing,4);
                    executeInSensingPhase_oldCompatibility_7_8_2014=SIM_IS_BIT_SET(nothing,5);
                    backwardCompatibility_7_8_2014=!SIM_IS_BIT_SET(nothing,6);
                    backwardCompatibility_13_8_2014=!SIM_IS_BIT_SET(nothing,7);
                }
                if (theName=="Va3")
                {
                    noHit=false;
                    ar >> byteQuantity;
                    unsigned char nothing;
                    ar >> nothing;
                    backwardCompatibilityCorrectionNeeded_8_11_2014=!SIM_IS_BIT_SET(nothing,0);
                    // FREE XXXX=SIM_IS_BIT_SET(nothing,1);
                    // FREE XXXX=SIM_IS_BIT_SET(nothing,2);
                    // FREE XXXX=SIM_IS_BIT_SET(nothing,3);
                    // FREE XXXX=SIM_IS_BIT_SET(nothing,4);
                    // FREE XXXX=SIM_IS_BIT_SET(nothing,5);
                    // FREE XXXX=SIM_IS_BIT_SET(nothing,6);
                    // FREE XXXX=SIM_IS_BIT_SET(nothing,7);
                    backwardCompatibilityCorrectionNeeded_13_10_2014=false;
                }
                if (theName.compare("Prm")==0)
                {
                    noHit=false;
                    ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                    delete scriptParameters;
                    scriptParameters=new CLuaScriptParameters();
                    scriptParameters->serialize(ar);
                    if (_scriptType==sim_scripttype_mainscript)
                    { // We just loaded a main script. Do we want to load the default main script parameters instead?
                        if (_mainScriptIsDefaultMainScript)
                        { // Yes!
                            // For now we just clear all parameters! (in future we might load default  parameters)
                            delete scriptParameters;
                            scriptParameters=new CLuaScriptParameters();
                        }
                    }
                }

                if (theName.compare("Coi")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> _objectIDAttachedTo_callback;
                }
                if (theName.compare("Coc")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> _objectIDAttachedTo_customization;
                }
                if (theName.compare("Cod")==0)
                {
                    noHit=false;
                    ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                    _customObjectData=new CCustomData();
                    _customObjectData->serializeData(ar);
                }

                if (noHit)
                    ar.loadUnknownData();
            }
        }
        if (backwardCompatibility_7_8_2014)
        { // here we have to adjust for the new script execution engine (since V3.1.3):
            if ( (_scriptType==sim_scripttype_mainscript)&&(!_mainScriptIsDefaultMainScript) )
            {
                // We will comment out the customized main script, load the default main script, and display a message so the user knows what happened!!
                std::string filenameAndPath(App::directories->systemDirectory+VREP_SLASH);
                filenameAndPath+=DEFAULT_MAINSCRIPT_NAME;
                std::string defaultMainScriptContent;
                if (VFile::doesFileExist(filenameAndPath))
                {
                    try
                    {
                        VFile file(filenameAndPath.c_str(),VFile::READ|VFile::SHARE_DENY_NONE);
                        VArchive archive2(&file,VArchive::LOAD);
                        unsigned int archiveLength=(unsigned int)file.getLength();
                        char dummy;
                        for (int i=0;i<int(archiveLength);i++)
                        {
                            archive2 >> dummy;
                            defaultMainScriptContent+=dummy;
                        }
                        archive2.close();
                        file.close();
                    }
                    catch(VFILE_EXCEPTION_TYPE e)
                    {
                        VFile::reportAndHandleFileExceptionError(e);
                        defaultMainScriptContent="";
                    }
                }
                std::string txt;

                // Comment out the old script:
                txt+=" \n";
                txt+=" \n";
                txt+="------------------------------------------------------------------------------ \n";
                txt+="-- Following main script automatically commented out by V-REP to guarantee \n";
                txt+="-- compatibility with V-REP 3.1.3 and later: \n";
                txt+=" \n";
                txt+="--[[ \n";
                txt+=" \n";
                _insertScriptText(true,txt.c_str());
                txt="";
                txt+="\n";
                txt+=" \n";
                txt+=" \n";
                txt+="--]] \n";
                txt+="------------------------------------------------------------------------------ \n";
                _insertScriptText(false,txt.c_str());

                if (defaultMainScriptContent.size()>200)
                {
                    // Insert the default main script:
                    _insertScriptText(true,defaultMainScriptContent.c_str());
                    // Insert some message to the user:
                    txt="";
                    txt+="------------------------------------------------------------------------------ \n";
                    txt+="-- The main script was automatically adjusted by V-REP to guarantee compatibility \n";
                    txt+="-- with V-REP 3.1.3 and later. You will find the original main script \n";
                    txt+="-- commented out below \n";
                    txt+="if (sim_call_type==sim_childscriptcall_initialization) then \n";
                    txt+="  simSetScriptAttribute(sim_handle_self,sim_scriptattribute_executioncount,-1) \n";
                    txt+="  local theTxt='The main script of this scene was customized (which is anyway not recommended),&&nand probably not compatible with V-REP release 3.1.3 and later. For that reason,&&nthe main script was replaced with the default main script. If your scene does not run&&nas expected, have a look at the main script code. '\n";
                    txt+="  local h=simDisplayDialog('Compatibility issue',theTxt,sim_dlgstyle_ok,false,'',{0.8,0,0,0,0,0},{0.5,0,0,1,1,1}) \n";
                    txt+="end \n";
                    txt+="------------------------------------------------------------------------------ \n";
                    txt+=" \n";
                    txt+=" \n";
                    _insertScriptText(true,txt.c_str());

                    txt="\n";
                    txt+="Compatibility issue with @@REPLACE@@\n";
                    txt+="Since V-REP 3.1.3, the functions simHandleChildScript and simHandleSensingChildScripts\n";
                    txt+="were replaced with simHandleChildScripts (i.e. with an additional 's'), which\n";
                    txt+="operates slightly differently. In addition to this, a new function was introduced that\n";
                    txt+="handles threaded child scripts: simLaunchThreadedChildScripts. For that reason, V-REP\n";
                    txt+="has automatically adjusted the customized main script. Make sure that everything\n";
                    txt+="still works as expected.\n";
                    txt+="\n";
                    App::ct->objCont->appendLoadOperationIssue(txt.c_str(),scriptID);

                }
                else
                { // there was a problem loading the default main script.
                    // Insert some message to the user:
                    txt="";
                    txt+="------------------------------------------------------------------------------ \n";
                    txt+="-- Please manually adjust the customized main script \n";
                    txt+="if (sim_call_type==sim_childscriptcall_initialization) then \n";
                    txt+="  simSetScriptAttribute(sim_handle_self,sim_scriptattribute_executioncount,-1) \n";
                    txt+="  local theTxt='Please manually adjust the main script!!'\n";
                    txt+="  local h=simDisplayDialog('Compatibility issue',theTxt,sim_dlgstyle_ok,false,'',{0.8,0,0,0,0,0},{0.5,0,0,1,1,1}) \n";
                    txt+="end \n";
                    txt+="------------------------------------------------------------------------------ \n";
                    txt+=" \n";
                    txt+=" \n";
                    _insertScriptText(true,txt.c_str());

                    txt="\n";
                    txt+="Compatibility issue with @@REPLACE@@\n";
                    txt+="Since V-REP 3.1.3, the functions simHandleChildScript and simHandleSensingChildScripts\n";
                    txt+="were replaced with simHandleChildScripts (i.e. with an additional 's'), which\n";
                    txt+="operates slightly differently. In addition to this, a new function was introduced that\n";
                    txt+="handles threaded child scripts: simLaunchThreadedChildScripts. For that reason, V-REP\n";
                    txt+="tried to automatically adjusted the customized main script, but couldn't find the default\n";
                    txt+="main script normally located in system/dltmscpt.txt. Please manually adjust the main script.\n";
                    txt+="\n";
                    App::ct->objCont->appendLoadOperationIssue(txt.c_str(),scriptID);
                }
            }
            if (_scriptType==sim_scripttype_childscript)
            {
                if (!_threadedExecution)
                {

                    _replaceScriptText("\n","\n\t"); // "\r\n" is also handled

                    std::string txt;
                    if (executeInSensingPhase_oldCompatibility_7_8_2014)
                    {
                        // Add text to the beginning:
                        txt+="------------------------------------------------------------------------------ \n";
                        txt+="-- Following few lines automatically added by V-REP to guarantee compatibility \n";
                        txt+="-- with V-REP 3.1.3 and later: \n";
                        txt+="if (sim_call_type==sim_childscriptcall_initialization) then \n";
                        txt+=" \n";
                        txt+="end \n";
                        txt+="if (sim_call_type==sim_childscriptcall_cleanup) then \n";
                        txt+=" \n";
                        txt+="end \n";
                        txt+="if (sim_call_type==sim_childscriptcall_sensing) then \n";
                        txt+="  if not firstTimeHere93846738 then \n";
                        txt+="      firstTimeHere93846738=0 \n";
                        txt+="  end \n";
                        txt+="  simSetScriptAttribute(sim_handle_self,sim_scriptattribute_executioncount,firstTimeHere93846738) \n";
                        txt+="  firstTimeHere93846738=firstTimeHere93846738+1 \n";
                        txt+=" \n";
                        txt+="------------------------------------------------------------------------------ \n";
                        txt+=" \n";
                        txt+=" \n";
                        _insertScriptText(true,txt.c_str());

                        // Add text to the end:
                        txt="\n";
                        txt+=" \n";
                        txt+=" \n";
                        txt+="------------------------------------------------------------------------------ \n";
                        txt+="-- Following few lines automatically added by V-REP to guarantee compatibility \n";
                        txt+="-- with V-REP 3.1.3 and later: \n";
                        txt+="end \n";
                        txt+="------------------------------------------------------------------------------ \n";
                        _insertScriptText(false,txt.c_str());

                        // Because in old sensing child scripts, simHandleChildScript didn't anyway have an effect:
                        _replaceScriptText("simHandleChildScript(","-- commented by V-REP: s@imHandleChildScript(");
                        _replaceScriptText("s@imHandleChildScript","simHandleChildScript");
                    }
                    else
                    { // actuation child script
                        // Add text to the beginning:
                        txt+="------------------------------------------------------------------------------ \n";
                        txt+="-- Following few lines automatically added by V-REP to guarantee compatibility \n";
                        txt+="-- with V-REP 3.1.3 and later: \n";
                        txt+="if (sim_call_type==sim_childscriptcall_initialization) then \n";
                        txt+="  simSetScriptAttribute(sim_handle_self,sim_childscriptattribute_automaticcascadingcalls,false) \n";
                        txt+="end \n";
                        txt+="if (sim_call_type==sim_childscriptcall_cleanup) then \n";
                        txt+=" \n";
                        txt+="end \n";
                        txt+="if (sim_call_type==sim_childscriptcall_sensing) then \n";
                        txt+="  simHandleChildScripts(sim_call_type) \n";
                        txt+="end \n";
                        txt+="if (sim_call_type==sim_childscriptcall_actuation) then \n";
                        txt+="  if not firstTimeHere93846738 then \n";
                        txt+="      firstTimeHere93846738=0 \n";
                        txt+="  end \n";
                        txt+="  simSetScriptAttribute(sim_handle_self,sim_scriptattribute_executioncount,firstTimeHere93846738) \n";
                        txt+="  firstTimeHere93846738=firstTimeHere93846738+1 \n";
                        txt+=" \n";
                        txt+="------------------------------------------------------------------------------ \n";
                        txt+=" \n";
                        txt+=" \n";
                        _insertScriptText(true,txt.c_str());

                        // Add text to the end:
                        txt="\n";
                        txt+=" \n";
                        txt+=" \n";
                        txt+="------------------------------------------------------------------------------ \n";
                        txt+="-- Following few lines automatically added by V-REP to guarantee compatibility \n";
                        txt+="-- with V-REP 3.1.3 and later: \n";
                        txt+="end \n";
                        txt+="------------------------------------------------------------------------------ \n";
                        _insertScriptText(false,txt.c_str());
                        _replaceScriptText("simHandleChildScript(","sim_handle_all_except_explicit",")","simHandleChildScripts(sim_call_type)");
                        _replaceScriptText("simHandleChildScript(","sim_handle_all",")","simHandleChildScripts(sim_call_type)");
                        _replaceScriptText("simHandleChildScript(","sim_handle_all_except_explicit",",","simHandleChildScripts(sim_call_type,");
                        _replaceScriptText("simHandleChildScript(","sim_handle_all",",","simHandleChildScripts(sim_call_type,");

                        if (_containsScriptText("simHandleChildScript("))
                        { // output a warning
                            txt="\n";
                            txt+="Compatibility issue with @@REPLACE@@\n";
                            txt+="Since V-REP 3.1.3, the function simHandleChildScript is not supported anymore.\n";
                            txt+="It was replaced with simHandleChildScripts (i.e. with an additional 's'),\n";
                            txt+="and operates slightly differently. V-REP has tried to automatically adjust\n";
                            txt+="the script, but failed. Please correct this issue yourself by editing the script.\n";
                            txt+="\n";
                            App::ct->objCont->appendLoadOperationIssue(txt.c_str(),scriptID);
                        }
                    }
                }
                else
                {
                    if (_containsScriptText("simHandleChildScript("))
                    { // output a warning
                        std::string txt="\n";
                        txt+="Compatibility issue with @@REPLACE@@\n";
                        txt+="Since V-REP 3.1.3, the function simHandleChildScript is not supported anymore.\n";
                        txt+="It was replaced with simHandleChildScripts (i.e. with an additional 's'),\n";
                        txt+="and operates slightly differently. In addition to this, simhandleChildScripts\n";
                        txt+="cannot be called from threaded child scripts anymore. Please correct this issue\n";
                        txt+="yourself by editing the script.\n";
                        txt+="\n";
                        App::ct->objCont->appendLoadOperationIssue(txt.c_str(),scriptID);
                    }
                }
            }
            if (_scriptType==sim_scripttype_customizationscript)
            {
                _replaceScriptText("sim_customizationscriptcall_firstaftersimulation","sim_customizationscriptcall_@irstaftersimulation");
                _replaceScriptText("sim_customizationscriptcall_lastbeforesimulation","sim_customizationscriptcall_@astbeforesimulation");
                _replaceScriptText("sim_customizationscriptcall_first","sim_customizationscriptcall_initialization");
                _replaceScriptText("sim_customizationscriptcall_last","sim_customizationscriptcall_cleanup");
                _replaceScriptText("sim_customizationscriptcall_@irstaftersimulation","sim_customizationscriptcall_firstaftersimulation");
                _replaceScriptText("sim_customizationscriptcall_@astbeforesimulation","sim_customizationscriptcall_lastbeforesimulation");
            }
        }
        if ( (_scriptType==sim_scripttype_childscript)&&_threadedExecution&&(!backwardCompatibility_7_8_2014)&&backwardCompatibility_13_8_2014 )
        { // to correct for a forgotten thing. Happens only with files I modified between 11/8/2014 and 13/8/2014 (half of the demo scenes and models)
            _replaceScriptText("pcall(threadFunction)","@@call(threadFunction)");
            _replaceScriptText("@@call(threadFunction)","res,err=xpcall(threadFunction,function(err) return debug.traceback(err) end)\nif not res then\n\tsimAddStatusbarMessage('Lua runtime error: '..err)\nend");
        }
        if (backwardCompatibilityCorrectionNeeded_13_10_2014)
        {
            // 1. check if we haven't previously added the correction:
            if (!_containsScriptText("@backCompatibility1:"))
            {
                bool modifiedSomething=_replaceScriptTextKeepMiddleUnchanged("simSetShapeColor(",",","simSetShapeColor(colorCorrectionFunction(","),");

                if (modifiedSomething)
                {
                    // Add text to the beginning:
                    std::string txt;
                    txt+="------------------------------------------------------------------------------ \n";
                    txt+="-- Following few lines automatically added by V-REP to guarantee compatibility \n";
                    txt+="-- with V-REP 3.1.3 and earlier: \n";
                    txt+="colorCorrectionFunction=function(_aShapeHandle_) \n";
                    txt+="  local version=simGetIntegerParameter(sim_intparam_program_version) \n";
                    txt+="  local revision=simGetIntegerParameter(sim_intparam_program_revision) \n";
                    txt+="  if (version<30104)and(revision<3) then \n";
                    txt+="      return _aShapeHandle_ \n";
                    txt+="  end \n";
                    txt+="  return '@backCompatibility1:'.._aShapeHandle_ \n";
                    txt+="end \n";
                    txt+="------------------------------------------------------------------------------ \n";
                    txt+=" \n";
                    txt+=" \n";
                    _insertScriptText(true,txt.c_str());
                }
            }
        }
        if (backwardCompatibilityCorrectionNeeded_8_11_2014)
        {
            _replaceScriptText("res,err=pcall(threadFunction)","res,err=xpcall(threadFunction,function(err) return debug.traceback(err) end)");
        }

        // Following since 19/12/2015: not really needed, but better.
        if (ar.getVrepVersionThatWroteThisFile()<30300)
        {
            _replaceScriptText("simGetBooleanParameter","simGetBoolParameter");
            _replaceScriptText("simSetBooleanParameter","simSetBoolParameter");
            _replaceScriptText("simGetIntegerParameter","simGetInt32Parameter");
            _replaceScriptText("simSetIntegerParameter","simSetInt32Parameter");
            _replaceScriptText("simGetFloatingParameter","simGetFloatParameter");
            _replaceScriptText("simSetFloatingParameter","simSetFloatParameter");
            _replaceScriptText("simGetObjectIntParameter","simGetObjectInt32Parameter");
            _replaceScriptText("simSetObjectIntParameter","simSetObjectInt32Parameter");
        }

        // since 19/1/2016 we don't use tabs anymore in embedded scripts:
        if (ar.getVrepVersionThatWroteThisFile()<30300)
            _replaceScriptText("\t","    "); // tab to 4 spaces

        // Following since 13/9/2016, but active only since V3.3.3 (or V3.4.0)
        if ( (ar.getVrepVersionThatWroteThisFile()<=30302)&&(VREP_PROGRAM_VERSION_NB>30302) )
        {
            _replaceScriptText("simPackInts","simPackInt32Table");
            _replaceScriptText("simPackUInts","simPackUInt32Table");
            _replaceScriptText("simPackFloats","simPackFloatTable");
            _replaceScriptText("simPackDoubles","simPackDoubleTable");
            _replaceScriptText("simPackBytes","simPackUInt8Table");
            _replaceScriptText("simPackWords","simPackUInt16Table");
            _replaceScriptText("simUnpackInts","simUnpackInt32Table");
            _replaceScriptText("simUnpackUInts","simUnpackUInt32Table");
            _replaceScriptText("simUnpackFloats","simUnpackFloatTable");
            _replaceScriptText("simUnpackDoubles","simUnpackDoubleTable");
            _replaceScriptText("simUnpackBytes","simUnpackUInt8Table");
            _replaceScriptText("simUnpackWords","simUnpackUInt16Table");
        }

        fromBufferToFile();
    }
}

void CLuaScriptObject::_insertScriptText(bool toFront,const char* txt)
{
    _scriptFoldingInfo.clear();
    if (_scriptText==NULL)
        return;
    std::string theScript(_scriptText);
    if (toFront)
        theScript=std::string(txt)+theScript;
    else
        theScript+=txt;
    delete[] _scriptText;
    _scriptTextLength=theScript.length()+1;
    _scriptText=new char[_scriptTextLength];
    for (int i=0;i<_scriptTextLength-1;i++)
        _scriptText[i]=theScript[i];
    _scriptText[_scriptTextLength-1]=0;
}

bool CLuaScriptObject::_replaceScriptText(const char* oldTxt,const char* newTxt)
{
    _scriptFoldingInfo.clear();
    if (_scriptText==NULL)
        return(false);
    std::string theScript(_scriptText);
    size_t startPos=theScript.find(oldTxt,0);
    bool replacedSomething=false;
    while (startPos!=std::string::npos)
    {
        theScript.replace(startPos,strlen(oldTxt),newTxt);
        startPos=theScript.find(oldTxt,startPos+1);
        replacedSomething=true;
    }
    if (replacedSomething)
    {
        delete[] _scriptText;
        _scriptTextLength=theScript.length()+1;
        _scriptText=new char[_scriptTextLength];
        for (int i=0;i<_scriptTextLength-1;i++)
            _scriptText[i]=theScript[i];
        _scriptText[_scriptTextLength-1]=0;
    }
    return(replacedSomething);
}

bool CLuaScriptObject::_replaceScriptTextKeepMiddleUnchanged(const char* oldTxtStart,const char* oldTxtEnd,const char* newTxtStart,const char* newTxtEnd)
{ // Will do following: oldTextStart*oldTextEnd --> nextTextStart*newTextEnd
    _scriptFoldingInfo.clear();
    if (_scriptText==NULL)
        return(false);
    std::string theScript(_scriptText);
    size_t startPos=theScript.find(oldTxtStart,0);
    bool replacedSomething=false;
    while (startPos!=std::string::npos)
    {
        size_t startPos2=theScript.find(oldTxtEnd,startPos+strlen(oldTxtStart));
        if (startPos2!=std::string::npos)
        {
            // check if we have a line break in-between:
            bool lineBreak=false;
            for (unsigned int i=startPos;i<startPos2;i++)
            {
                if ( (theScript[i]==(unsigned char)13)||(theScript[i]==(unsigned char)10) )
                {
                    lineBreak=true;
                    break;
                }
            }
            if (!lineBreak)
            {
                theScript.replace(startPos2,strlen(oldTxtEnd),newTxtEnd);
                theScript.replace(startPos,strlen(oldTxtStart),newTxtStart);
                startPos=theScript.find(oldTxtStart,startPos2+strlen(newTxtEnd)+strlen(newTxtStart)-strlen(oldTxtStart));
                replacedSomething=true;
            }
            else
                startPos=theScript.find(oldTxtStart,startPos+1);
        }
        else
            startPos=theScript.find(oldTxtStart,startPos+1);
    }
    if (replacedSomething)
    {
        delete[] _scriptText;
        _scriptTextLength=theScript.length()+1;
        _scriptText=new char[_scriptTextLength];
        for (int i=0;i<_scriptTextLength-1;i++)
            _scriptText[i]=theScript[i];
        _scriptText[_scriptTextLength-1]=0;
    }
    return(replacedSomething);
}

bool CLuaScriptObject::_replaceScriptText(const char* oldTxt1,const char* oldTxt2,const char* oldTxt3,const char* newTxt)
{ // there can be spaces between the 3 words
    _scriptFoldingInfo.clear();
    if (_scriptText==NULL)
        return(false);
    std::string theScript(_scriptText);
    int l1=strlen(oldTxt1);
    int l2=strlen(oldTxt2);
    int l3=strlen(oldTxt3);
    bool replacedSomething=false;
    size_t searchStart=0;
    while (searchStart<theScript.length())
    {
        size_t startPos1=theScript.find(oldTxt1,searchStart);
        if (startPos1!=std::string::npos)
        {
            searchStart=startPos1+1;
            size_t startPos2=theScript.find(oldTxt2,startPos1+l1);
            if (startPos2!=std::string::npos)
            {
                bool onlySpaces=true;
                size_t p=startPos1+l1;
                while (p<startPos2)
                {
                    if (theScript[p]!=' ')
                        onlySpaces=false;
                    p++;
                }
                if (onlySpaces)
                {
                    size_t startPos3=theScript.find(oldTxt3,startPos2+l2);
                    if (startPos3!=std::string::npos)
                    {
                        onlySpaces=true;
                        p=startPos2+l2;
                        while (p<startPos3)
                        {
                            if (theScript[p]!=' ')
                                onlySpaces=false;
                            p++;
                        }
                        if (onlySpaces)
                        { // ok!
                            theScript.replace(startPos1,startPos3-startPos1+l3,newTxt);
                            replacedSomething=true;
                        }
                    }
                }
            }
        }
        else
            searchStart=theScript.length();
    }
    if (replacedSomething)
    {
        delete[] _scriptText;
        _scriptTextLength=theScript.length()+1;
        _scriptText=new char[_scriptTextLength];
        for (int i=0;i<_scriptTextLength-1;i++)
            _scriptText[i]=theScript[i];
        _scriptText[_scriptTextLength-1]=0;
    }
    return(replacedSomething);
}

bool CLuaScriptObject::_containsScriptText(const char* txt)
{
    if (_scriptText==NULL)
        return(false);
    std::string theScript(_scriptText);
    size_t startPos=theScript.find(txt);
    return(startPos!=std::string::npos);
}

int CLuaScriptObject::appendTableEntry(const char* arrayName,const char* keyName,const char* data,const int what[2])
{ // DEPRECATED since 23/2/2016
    if ( ( (_scriptTextExec==NULL)||(L==NULL) )&&_threadedExecution )
        return(-1); // threaded scripts must be called from within!

    if (_threadedExecution)
    {
        if (!VThread::areThreadIDsSame(_threadedScript_associatedFiberOrThreadID,VThread::getCurrentThreadId()))
            return(-1); // only from the same thread when threaded!!
    }

    if (_scriptIsDisabled)
        return(-1);

    if ((_scriptTextExec==NULL)||(L==NULL))
    {  // this script was never run before!
        if (_scriptType==sim_scripttype_generalcallback)
            runGeneralCallback(-1,0,NULL); // dummy call
        if (_scriptType==sim_scripttype_customizationscript)
            runCustomizationScript(sim_customizationscriptcall_initialization); // dummy call
        if (!App::ct->simulation->isSimulationStopped())
        { // following only when simulation is running!
            if (_scriptType==sim_scripttype_contactcallback)
            {
                int in[3]={0,0,-1};
                int out1[3];
                float out2[14];
                runContactCallback(in,out1,out2); // dummy call
            }
            if (_scriptType==sim_scripttype_childscript)
            {
                CInterfaceStack in;
                runNonThreadedChildScript(in,sim_childscriptcall_initialization); // dummy call
            }
            if (_scriptType==sim_scripttype_jointctrlcallback)
            {
                std::vector<bool> inDataBool;
                std::vector<int> inDataInt;
                std::vector<float> inDataFloat;
                std::vector<float> outData;
                inDataBool.push_back(true); // init
                inDataBool.push_back(true); // revolute
                inDataBool.push_back(true); // cyclic
                inDataInt.push_back(-1); // joint ID
                inDataInt.push_back(0); // pass count
                inDataInt.push_back(0); // total passes
                inDataFloat.push_back(0.0f); // current pos
                inDataFloat.push_back(0.0f); // target pos
                inDataFloat.push_back(0.0f); // error v
                inDataFloat.push_back(0.0f); // effort
                inDataFloat.push_back(0.0f); // dyn step size
                inDataFloat.push_back(0.0f); // low limit
                inDataFloat.push_back(0.0f); // high limit
                inDataFloat.push_back(0.0f); // target vel
                inDataFloat.push_back(0.0f); // max force
                inDataFloat.push_back(0.0f); // upper vel limit
                runJointCtrlCallback(inDataBool,inDataInt,inDataFloat,outData); // dummy call
            }
        }
    }

    if ((_scriptTextExec==NULL)||(L==NULL))
        return(-1);

    int oldTop=luaWrap_lua_gettop(L);   // We store lua's stack

    // First check if the table where we want to append a value exists. If not, or not a table, create it!
    luaWrap_lua_getglobal(L,arrayName);
    if (!luaWrap_lua_istable(L,-1))
    { // the table is inexistant
        luaWrap_lua_newtable(L);
        luaWrap_lua_setglobal(L,arrayName);
    }
    luaWrap_lua_pop(L,1);

    // The table where we want to append a value:
    luaWrap_lua_getglobal(L,arrayName);
    int theTablePos=luaWrap_lua_gettop(L);
    int theTableLength=luaWrap_lua_objlen(L,theTablePos);

    // Do we want to simply insert the value, or do we want to insert a keyed value?
    if ( (keyName==NULL)||(strlen(keyName)==0) )
    { // not keyed value:
    }
    else
    { // keyed value:
        luaWrap_lua_pushstring(L,keyName);
    }

    // Now push the value, which might itself be a table:
    int w=what[0];
    if ( (w&sim_script_arg_table)!=0 )
    { // we have a table
        w-=sim_script_arg_table;
        luaWrap_lua_newtable(L);
        int newTablePos=luaWrap_lua_gettop(L);
        int stringOff=0;
        for (int i=0;i<what[1];i++)
        {
            if (w==sim_script_arg_null)
                luaWrap_lua_pushnil(L);
            if (w==sim_script_arg_bool)
                luaWrap_lua_pushboolean(L,data[i]);
            if (w==sim_script_arg_int32)
                luaWrap_lua_pushinteger(L,((int*)data)[i]);
            if (w==sim_script_arg_float)
                luaWrap_lua_pushnumber(L,((float*)data)[i]);
            if (w==sim_script_arg_double)
                luaWrap_lua_pushnumber(L,((double*)data)[i]);
            if (w==sim_script_arg_string)
            {
                luaWrap_lua_pushstring(L,data+stringOff);
                stringOff+=strlen(data+stringOff)+1;
            }
            luaWrap_lua_rawseti(L,newTablePos,i+1);
        }
    }
    else
    { // we don't have a table
        if (w==sim_script_arg_null)
            luaWrap_lua_pushnil(L);
        if (w==sim_script_arg_bool)
            luaWrap_lua_pushboolean(L,data[0]);
        if (w==sim_script_arg_int32)
            luaWrap_lua_pushinteger(L,((int*)data)[0]);
        if (w==sim_script_arg_float)
            luaWrap_lua_pushnumber(L,((float*)data)[0]);
        if (w==sim_script_arg_double)
            luaWrap_lua_pushnumber(L,((double*)data)[0]);
        if (w==sim_script_arg_string)
            luaWrap_lua_pushstring(L,data);
        if (w==sim_script_arg_charbuff)
            luaWrap_lua_pushlstring(L,data,what[1]);
    }

    // Finally, insert the value in the table:
    if ( (keyName==NULL)||(strlen(keyName)==0) )
    { // not keyed value:
        luaWrap_lua_rawseti(L,theTablePos,theTableLength+1);
    }
    else
    { // keyed value:
        luaWrap_lua_settable(L,-3);
    }

    luaWrap_lua_settop(L,oldTop);       // We restore lua's stack
    return(0);
}

int CLuaScriptObject::_luaPCall(luaWrap_lua_State* luaState,int nargs,int nresult,int errfunc)
{
    _inExecutionNow=true;
    int retVal=luaWrap_lua_pcall(luaState,nargs,nresult,errfunc);
    _inExecutionNow=false;
    return(retVal);
}

bool CLuaScriptObject::_luaLoadBuffer(luaWrap_lua_State* luaState,const char* buff,size_t sz,const char* name)
{
    // This is the slow version (loading and compiling the buffer over and over):
//    int loadBufferRes=luaWrap_luaL_loadbuffer(luaState,buff,sz,name);
//    return(loadBufferRes==0);

    // This is the faster version (loading and compiling the buffer once only):
    if (_loadBufferResult!=0)
    {
        _loadBufferResult=luaWrap_luaL_loadbuffer(luaState,buff,sz,name);
        if (_loadBufferResult==0)
            luaWrap_lua_setglobal(L,"sim_code_function_to_run");
    }
    if (_loadBufferResult==0)
        luaWrap_lua_getglobal(L,"sim_code_function_to_run");
    return(_loadBufferResult==0);
}


/*
void CLuaScriptObject::_fixObjectParameters(const char* txt)
{
    _scriptFoldingInfo.clear();
    if (_scriptText==NULL)
        return;
    std::string theScript(_scriptText);
    size_t startPos=theScript.find(txt,0);
    bool replacedSomething=false;
    while (startPos!=std::string::npos)
    {
        size_t startPos2=theScript.find(",",startPos+strlen(txt));
        size_t startPos3=theScript.find(",",startPos2+1);
        if ( (startPos2!=std::string::npos)&&(startPos3!=std::string::npos) )
        {
            // check if we have a line break in-between:
            bool lineBreak=false;
            for (unsigned int i=startPos;i<startPos3;i++)
            {
                if ( (theScript[i]==(unsigned char)13)||(theScript[i]==(unsigned char)10) )
                {
                    lineBreak=true;
                    break;
                }
            }
            if (!lineBreak)
            {
                std::string val;
                val.assign(theScript.c_str()+startPos2+1,theScript.c_str()+startPos3);
                std::string val2=_getObjParamString(val);
                printf("%s --> %s\n",val.c_str(),val2.c_str());
                theScript.replace(startPos2+1,val.size(),val2.c_str());
                startPos=theScript.find(txt,startPos3);
                replacedSomething=true;
            }
            else
                startPos=theScript.find(txt,startPos+1);
        }
        else
            startPos=theScript.find(txt,startPos+1);
    }
    if (replacedSomething)
    {
        delete[] _scriptText;
        _scriptTextLength=theScript.length()+1;
        _scriptText=new char[_scriptTextLength];
        for (int i=0;i<_scriptTextLength-1;i++)
            _scriptText[i]=theScript[i];
        _scriptText[_scriptTextLength-1]=0;
    }
}

void CLuaScriptObject::_fixObjectParameters2(const char* txt)
{
    _scriptFoldingInfo.clear();
    if (_scriptText==NULL)
        return;
    std::string theScript(_scriptText);
    size_t startPos=theScript.find(txt,0);
    bool replacedSomething=false;
    while (startPos!=std::string::npos)
    {
        size_t startPos2=theScript.find(",",startPos+strlen(txt));
        size_t startPos3=theScript.find(")",startPos2+1);
        if ( (startPos2!=std::string::npos)&&(startPos3!=std::string::npos) )
        {
            // check if we have a line break in-between:
            bool lineBreak=false;
            for (unsigned int i=startPos;i<startPos3;i++)
            {
                if ( (theScript[i]==(unsigned char)13)||(theScript[i]==(unsigned char)10) )
                {
                    lineBreak=true;
                    break;
                }
            }
            if (!lineBreak)
            {
                std::string val;
                val.assign(theScript.c_str()+startPos2+1,theScript.c_str()+startPos3);
                std::string val2=_getObjParamString(val);
                printf("%s --> %s\n",val.c_str(),val2.c_str());
                theScript.replace(startPos2+1,val.size(),val2.c_str());
                startPos=theScript.find(txt,startPos3);
                replacedSomething=true;
            }
            else
                startPos=theScript.find(txt,startPos+1);
        }
        else
            startPos=theScript.find(txt,startPos+1);
    }
    if (replacedSomething)
    {
        delete[] _scriptText;
        _scriptTextLength=theScript.length()+1;
        _scriptText=new char[_scriptTextLength];
        for (int i=0;i<_scriptTextLength-1;i++)
            _scriptText[i]=theScript[i];
        _scriptText[_scriptTextLength-1]=0;
    }
}

std::string CLuaScriptObject::_getObjParamString(const std::string& val)
{
    std::string retVal(value);
    int value;
    if (tt::stringToInt(val.c_str(),value))
    {
    if (value==sim_objintparam_visibility_layer) retVal="sim_objintparam_visibility_layer";
    if (value==sim_objfloatparam_abs_x_velocity) retVal="sim_objfloatparam_abs_x_velocity";
    if (value==sim_objfloatparam_abs_y_velocity) retVal="sim_objfloatparam_abs_y_velocity";
    if (value==sim_objfloatparam_abs_z_velocity) retVal="sim_objfloatparam_abs_z_velocity";
    if (value==sim_objfloatparam_abs_rot_velocity) retVal="sim_objfloatparam_abs_rot_velocity";
    if (value==sim_objfloatparam_objbbox_min_x) retVal="sim_objfloatparam_objbbox_min_x";
    if (value==sim_objfloatparam_objbbox_min_y) retVal="sim_objfloatparam_objbbox_min_y";
    if (value==sim_objfloatparam_objbbox_min_z) retVal="sim_objfloatparam_objbbox_min_z";
    if (value==sim_objfloatparam_objbbox_max_x) retVal="sim_objfloatparam_objbbox_max_x";
    if (value==sim_objfloatparam_objbbox_max_y) retVal="sim_objfloatparam_objbbox_max_y";
    if (value==sim_objfloatparam_objbbox_max_z) retVal="sim_objfloatparam_objbbox_max_z";
    if (value==sim_objfloatparam_modelbbox_min_x) retVal="sim_objfloatparam_modelbbox_min_x";
    if (value==sim_objfloatparam_modelbbox_min_y) retVal="sim_objfloatparam_modelbbox_min_y";
    if (value==sim_objfloatparam_modelbbox_min_z) retVal="sim_objfloatparam_modelbbox_min_z";
    if (value==sim_objfloatparam_modelbbox_max_x) retVal="sim_objfloatparam_modelbbox_max_x";
    if (value==sim_objfloatparam_modelbbox_max_y) retVal="sim_objfloatparam_modelbbox_max_y";
    if (value==sim_objfloatparam_modelbbox_max_z) retVal="sim_objfloatparam_modelbbox_max_z";
    if (value==sim_objintparam_collection_self_collision_indicator) retVal="sim_objintparam_collection_self_collision_indicator";
    if (value==sim_objfloatparam_transparency_offset) retVal="sim_objfloatparam_transparency_offset";
    if (value==sim_objintparam_child_role) retVal="sim_objintparam_child_role";
    if (value==sim_objintparam_parent_role) retVal="sim_objintparam_parent_role";
    if (value==sim_objintparam_manipulation_permissions) retVal="sim_objintparam_manipulation_permissions";
    if (value==sim_objintparam_illumination_handle) retVal="sim_objintparam_illumination_handle";
    if (value==sim_visionfloatparam_near_clipping) retVal="sim_visionfloatparam_near_clipping";
    if (value==sim_visionfloatparam_far_clipping) retVal="sim_visionfloatparam_far_clipping";
    if (value==sim_visionintparam_resolution_x) retVal="sim_visionintparam_resolution_x";
    if (value==sim_visionintparam_resolution_y) retVal="sim_visionintparam_resolution_y";
    if (value==sim_visionfloatparam_perspective_angle) retVal="sim_visionfloatparam_perspective_angle";
    if (value==sim_visionfloatparam_ortho_size) retVal="sim_visionfloatparam_ortho_size";
    if (value==sim_visionintparam_disabled_light_components) retVal="sim_visionintparam_disabled_light_components";
    if (value==sim_visionintparam_rendering_attributes) retVal="sim_visionintparam_rendering_attributes";
    if (value==sim_visionintparam_entity_to_render) retVal="sim_visionintparam_entity_to_render";
    if (value==sim_visionintparam_windowed_size_x) retVal="sim_visionintparam_windowed_size_x";
    if (value==sim_visionintparam_windowed_size_y) retVal="sim_visionintparam_windowed_size_y";
    if (value==sim_visionintparam_windowed_pos_x) retVal="sim_visionintparam_windowed_pos_x";
    if (value==sim_visionintparam_windowed_pos_y) retVal="sim_visionintparam_windowed_pos_y";
    if (value==sim_visionintparam_pov_focal_blur) retVal="sim_visionintparam_pov_focal_blur";
    if (value==sim_visionfloatparam_pov_blur_distance) retVal="sim_visionfloatparam_pov_blur_distance";
    if (value==sim_visionfloatparam_pov_aperture) retVal="sim_visionfloatparam_pov_aperture";
    if (value==sim_visionintparam_pov_blur_sampled) retVal="sim_visionintparam_pov_blur_sampled";
    if (value==sim_visionintparam_render_mode) retVal="sim_visionintparam_render_mode";
    if (value==sim_jointintparam_motor_enabled) retVal="sim_jointintparam_motor_enabled";
    if (value==sim_jointintparam_ctrl_enabled) retVal="sim_jointintparam_ctrl_enabled";
    if (value==sim_jointfloatparam_pid_p) retVal="sim_jointfloatparam_pid_p";
    if (value==sim_jointfloatparam_pid_i) retVal="sim_jointfloatparam_pid_i";
    if (value==sim_jointfloatparam_pid_d) retVal="sim_jointfloatparam_pid_d";
    if (value==sim_jointfloatparam_intrinsic_x) retVal="sim_jointfloatparam_intrinsic_x";
    if (value==sim_jointfloatparam_intrinsic_y) retVal="sim_jointfloatparam_intrinsic_y";
    if (value==sim_jointfloatparam_intrinsic_z) retVal="sim_jointfloatparam_intrinsic_z";
    if (value==sim_jointfloatparam_intrinsic_qx) retVal="sim_jointfloatparam_intrinsic_qx";
    if (value==sim_jointfloatparam_intrinsic_qy) retVal="sim_jointfloatparam_intrinsic_qy";
    if (value==sim_jointfloatparam_intrinsic_qz) retVal="sim_jointfloatparam_intrinsic_qz";
    if (value==sim_jointfloatparam_intrinsic_qw) retVal="sim_jointfloatparam_intrinsic_qw";
    if (value==sim_jointfloatparam_velocity) retVal="sim_jointfloatparam_velocity";
    if (value==sim_jointfloatparam_spherical_qx) retVal="sim_jointfloatparam_spherical_qx";
    if (value==sim_jointfloatparam_spherical_qy) retVal="sim_jointfloatparam_spherical_qy";
    if (value==sim_jointfloatparam_spherical_qz) retVal="sim_jointfloatparam_spherical_qz";
    if (value==sim_jointfloatparam_spherical_qw) retVal="sim_jointfloatparam_spherical_qw";
    if (value==sim_jointfloatparam_upper_limit) retVal="sim_jointfloatparam_upper_limit";
    if (value==sim_jointfloatparam_kc_k) retVal="sim_jointfloatparam_kc_k";
    if (value==sim_jointfloatparam_kc_c) retVal="sim_jointfloatparam_kc_c";
    if (value==sim_jointfloatparam_ik_weight) retVal="sim_jointfloatparam_ik_weight";
    if (value==sim_jointfloatparam_error_x) retVal="sim_jointfloatparam_error_x";
    if (value==sim_jointfloatparam_error_y) retVal="sim_jointfloatparam_error_y";
    if (value==sim_jointfloatparam_error_z) retVal="sim_jointfloatparam_error_z";
    if (value==sim_jointfloatparam_error_a) retVal="sim_jointfloatparam_error_a";
    if (value==sim_jointfloatparam_error_b) retVal="sim_jointfloatparam_error_b";
    if (value==sim_jointfloatparam_error_g) retVal="sim_jointfloatparam_error_g";
    if (value==sim_jointfloatparam_error_pos) retVal="sim_jointfloatparam_error_pos";
    if (value==sim_jointfloatparam_error_angle) retVal="sim_jointfloatparam_error_angle";
    if (value==sim_jointintparam_velocity_lock) retVal="sim_jointintparam_velocity_lock";
    if (value==sim_jointintparam_vortex_dep_handle) retVal="sim_jointintparam_vortex_dep_handle";
    if (value==sim_jointfloatparam_vortex_dep_multiplication) retVal="sim_jointfloatparam_vortex_dep_multiplication";
    if (value==sim_jointfloatparam_vortex_dep_offset) retVal="sim_jointfloatparam_vortex_dep_offset";
    if (value==sim_shapefloatparam_init_velocity_x) retVal="sim_shapefloatparam_init_velocity_x";
    if (value==sim_shapefloatparam_init_velocity_y) retVal="sim_shapefloatparam_init_velocity_y";
    if (value==sim_shapefloatparam_init_velocity_z) retVal="sim_shapefloatparam_init_velocity_z";
    if (value==sim_shapeintparam_static) retVal="sim_shapeintparam_static";
    if (value==sim_shapeintparam_respondable) retVal="sim_shapeintparam_respondable";
    if (value==sim_shapefloatparam_mass) retVal="sim_shapefloatparam_mass";
    if (value==sim_shapefloatparam_texture_x) retVal="sim_shapefloatparam_texture_x";
    if (value==sim_shapefloatparam_texture_y) retVal="sim_shapefloatparam_texture_y";
    if (value==sim_shapefloatparam_texture_z) retVal="sim_shapefloatparam_texture_z";
    if (value==sim_shapefloatparam_texture_a) retVal="sim_shapefloatparam_texture_a";
    if (value==sim_shapefloatparam_texture_b) retVal="sim_shapefloatparam_texture_b";
    if (value==sim_shapefloatparam_texture_g) retVal="sim_shapefloatparam_texture_g";
    if (value==sim_shapefloatparam_texture_scaling_x) retVal="sim_shapefloatparam_texture_scaling_x";
    if (value==sim_shapefloatparam_texture_scaling_y) retVal="sim_shapefloatparam_texture_scaling_y";
    if (value==sim_shapeintparam_culling) retVal="sim_shapeintparam_culling";
    if (value==sim_shapeintparam_wireframe) retVal="sim_shapeintparam_wireframe";
    if (value==sim_shapeintparam_compound) retVal="sim_shapeintparam_compound";
    if (value==sim_shapeintparam_convex) retVal="sim_shapeintparam_convex";
    if (value==sim_shapeintparam_convex_check) retVal="sim_shapeintparam_convex_check";
    if (value==sim_shapeintparam_respondable_mask) retVal="sim_shapeintparam_respondable_mask";
    if (value==sim_shapefloatparam_init_velocity_a) retVal="sim_shapefloatparam_init_velocity_a";
    if (value==sim_shapefloatparam_init_velocity_b) retVal="sim_shapefloatparam_init_velocity_b";
    if (value==sim_shapefloatparam_init_velocity_g) retVal="sim_shapefloatparam_init_velocity_g";
    if (value==sim_shapestringparam_color_name) retVal="sim_shapestringparam_color_name";
    if (value==sim_shapeintparam_edge_visibility) retVal="sim_shapeintparam_edge_visibility";
    if (value==sim_shapefloatparam_shading_angle) retVal="sim_shapefloatparam_shading_angle";
    if (value==sim_shapefloatparam_edge_angle) retVal="sim_shapefloatparam_edge_angle";
    if (value==sim_shapeintparam_edge_borders_hidden) retVal="sim_shapeintparam_edge_borders_hidden";
    if (value==sim_proxintparam_ray_invisibility) retVal="sim_proxintparam_ray_invisibility";
    if (value==sim_forcefloatparam_error_x) retVal="sim_forcefloatparam_error_x";
    if (value==sim_forcefloatparam_error_y) retVal="sim_forcefloatparam_error_y";
    if (value==sim_forcefloatparam_error_z) retVal="sim_forcefloatparam_error_z";
    if (value==sim_forcefloatparam_error_a) retVal="sim_forcefloatparam_error_a";
    if (value==sim_forcefloatparam_error_b) retVal="sim_forcefloatparam_error_b";
    if (value==sim_forcefloatparam_error_g) retVal="sim_forcefloatparam_error_g";
    if (value==sim_forcefloatparam_error_pos) retVal="sim_forcefloatparam_error_pos";
    if (value==sim_forcefloatparam_error_angle) retVal="sim_forcefloatparam_error_angle";
    if (value==sim_lightintparam_pov_casts_shadows) retVal="sim_lightintparam_pov_casts_shadows";
    if (value==sim_cameraintparam_disabled_light_components) retVal="sim_cameraintparam_disabled_light_components";
    if (value==sim_camerafloatparam_perspective_angle) retVal="sim_camerafloatparam_perspective_angle";
    if (value==sim_camerafloatparam_ortho_size) retVal="sim_camerafloatparam_ortho_size";
    if (value==sim_cameraintparam_rendering_attributes) retVal="sim_cameraintparam_rendering_attributes";
    if (value==sim_cameraintparam_pov_focal_blur) retVal="sim_cameraintparam_pov_focal_blur";
    if (value==sim_camerafloatparam_pov_blur_distance) retVal="sim_camerafloatparam_pov_blur_distance";
    if (value==sim_camerafloatparam_pov_aperture) retVal="sim_camerafloatparam_pov_aperture";
    if (value==sim_cameraintparam_pov_blur_samples) retVal="sim_cameraintparam_pov_blur_samples";
    if (value==sim_dummyintparam_link_type) retVal="sim_dummyintparam_link_type";
    if (value==sim_mirrorfloatparam_width) retVal="sim_mirrorfloatparam_width";
    if (value==sim_mirrorfloatparam_height) retVal="sim_mirrorfloatparam_height";
    if (value==sim_mirrorfloatparam_reflectance) retVal="sim_mirrorfloatparam_reflectance";
    if (value==sim_mirrorintparam_enable) retVal="sim_mirrorintparam_enable";
    if (value==sim_pplanfloatparam_x_min) retVal="sim_pplanfloatparam_x_min";
    if (value==sim_pplanfloatparam_x_range) retVal="sim_pplanfloatparam_x_range";
    if (value==sim_pplanfloatparam_y_min) retVal="sim_pplanfloatparam_y_min";
    if (value==sim_pplanfloatparam_y_range) retVal="sim_pplanfloatparam_y_range";
    if (value==sim_pplanfloatparam_z_min) retVal="sim_pplanfloatparam_z_min";
    if (value==sim_pplanfloatparam_z_range) retVal="sim_pplanfloatparam_z_range";
    if (value==sim_pplanfloatparam_delta_min) retVal="sim_pplanfloatparam_delta_min";
    if (value==sim_pplanfloatparam_delta_range) retVal="sim_pplanfloatparam_delta_range";
    if (value==sim_mplanintparam_nodes_computed) retVal="sim_mplanintparam_nodes_computed";
    if (value==sim_mplanintparam_prepare_nodes) retVal="sim_mplanintparam_prepare_nodes";
    if (value==sim_mplanintparam_clear_nodes) retVal="sim_mplanintparam_clear_nodes";
    }
    return(retVal);
}
*/
VMutex CLuaScriptObject::_globalMutex;

std::vector<CLuaScriptObject*> CLuaScriptObject::toBeCalledByThread;
