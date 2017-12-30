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

#include "luaWrapper.h"
#include "v_repTypes.h"
#include "luaScriptParameters.h"
#include "outsideCommandQueueForScript.h"
#include "vMutex.h"
#include "vThread.h"
#include "customData.h"
#include "interfaceStack.h"

#define DEFAULT_MAINSCRIPT_NAME "dltmscpt.txt"
#define DEFAULT_NONTHREADEDCHILDSCRIPT_NAME "dltcscpt.txt"
#define DEFAULT_THREADEDCHILDSCRIPT_NAME "dlttscpt.txt"
#define DEFAULT_JOINTCALLBACKSCRIPT_NAME "defaultJointCtrlCallbackScript.txt"
#define DEFAULT_CONTACTCALLBACKSCRIPT_NAME "defaultContactCallbackScript.txt"
#define DEFAULT_GENERALCALLBACKSCRIPT_NAME "defaultGeneralCallbackScript.txt"
#define DEFAULT_CUSTOMIZATIONSCRIPT_NAME "defaultCustomizationScript.txt"

class CLuaScriptObject
{
public:
    CLuaScriptObject(int scriptTypeOrMinusOneForSerialization);
    virtual ~CLuaScriptObject();

    void initializeInitialValues(bool simulationIsRunning);
    void simulationAboutToStart();
    void simulationAboutToEnd();
    void simulationEnded();

    int getScriptID() const;
    void setScriptID(int newID);
    bool isSceneScript() const;
    std::string getIncludeScriptFilePathAndName() const;

    std::string getDescriptiveName() const;
    std::string getShortDescriptiveName() const;
    std::string getScriptSuffixNumberString() const;
    std::string getScriptPseudoName() const;
    void setAddOnName(const char* name);
    std::string getAddOnName() const;

    CLuaScriptObject* copyYourself();
    void serialize(CSer& ar);
    void perform3DObjectLoadingMapping(std::vector<int>* map);
    bool announce3DObjectWillBeErased(int objectID,bool copyBuffer);
    int flagScriptForRemoval();
    int getObjectIDThatScriptIsAttachedTo_child() const; // for child scripts
    int getObjectIDThatScriptIsAttachedTo_callback() const; // for callback scripts
    int getObjectIDThatScriptIsAttachedTo_customization() const; // for customization scripts
    void setObjectIDThatScriptIsAttachedTo_child(int newObjectID); // for child scripts
    void setObjectIDThatScriptIsAttachedTo_callback(int newObjectID); // for callback scripts
    void setObjectIDThatScriptIsAttachedTo_customization(int newObjectID); // for customization scripts

    void setScriptText(const char* scriptTxt,const std::vector<int>* scriptFoldingInfo);
    char* getScriptText(std::vector<int>* scriptFoldingInfo);
    int getScriptTextLength() const;

    int runMainScript();
    int runNonThreadedChildScript(CInterfaceStack& inputArguments,int callType);
    int runThreadedChildScript();
    void runThreadedChildScriptNow_();

    void runAddOn();
    void killAddOnScriptState();

    bool runCustomizationScript(int callType);
    void runJointCtrlCallback(const std::vector<bool>& inDataBool,const std::vector<int>& inDataInt,const std::vector<float>& inDataFloat,std::vector<float>& outDataFloat);




    int runContactCallback(const int inDataInt[3],int outDataInt[3],float outDataFloat[14]);
    int runGeneralCallback(int callbackId,int callbackTag,void* additionalData);

    int callScriptFunction(const char* functionName, SLuaCallBack* pdata);
    int callScriptFunctionEx(const char* functionName,CInterfaceStack* stack);
    int setScriptVariable(const char* variableName,CInterfaceStack* stack);
    int clearVariable(const char* variableName);

    // Following is deprecated:
    int appendTableEntry(const char* arrayName,const char* keyName,const char* data,const int what[2]);


    void _displayScriptError(const char* errMsg,int errorType);

    void killLuaState();
    int getNumberOfPasses() const;
    void setNumberOfPasses(int p);
    int setUserData(char* data);
    char* getUserData(int id) const;
    void releaseUserData(int id);
    void clearAllUserData();
    void setThreadedExecution(bool threadedExec);
    bool getThreadedExecution() const;
    bool getThreadedExecutionIsUnderWay() const;
    void setExecutionOrder(int order);
    int getExecutionOrder() const;
    bool getFlaggedForDestruction() const;
    int getScriptType() const;
    void setScriptIsDisabled(bool isDisabled);
    bool getScriptIsDisabled() const;
    void setExecuteJustOnce(bool justOnce);
    bool getExecuteJustOnce() const;

    void getPreviousEditionWindowPosAndSize(int posAndSize[4]) const;
    void setPreviousEditionWindowPosAndSize(const int posAndSize[4]);

    void setObjectCustomData(int header,const char* data,int dataLength);
    int getObjectCustomDataLength(int header) const;
    void getObjectCustomData(int header,char* data) const;
    bool getObjectCustomDataHeader(int index,int& header) const;

    // Same as above, but data is not serialized (but copied):
    void setObjectCustomData_tempData(int header,const char* data,int dataLength);
    int getObjectCustomDataLength_tempData(int header) const;
    void getObjectCustomData_tempData(int header,char* data) const;
    bool getObjectCustomDataHeader_tempData(int index,int& header) const;

    CLuaScriptParameters* getScriptParametersObject();

    void setCustomizedMainScript(bool customized);
    bool isDefaultMainScript() const;

    void setAutomaticCascadingCallsDisabled(bool disabled);
    bool getAutomaticCascadingCallsDisabled() const;
    bool checkAndSetWarningAboutSimHandleChildScriptAlreadyIssued_oldCompatibility_7_8_2014();
    bool checkAndSetWarning_simRMLPosition_oldCompatibility_30_8_2014();
    bool checkAndSetWarning_simRMLVelocity_oldCompatibility_30_8_2014();
    bool checkAndSetWarning_simGetMpConfigForTipPose_oldCompatibility_21_1_2016();
    bool checkAndSetWarning_simFindIkPath_oldCompatibility_2_2_2016();
    bool checkAndSetWarning_oldPathPlanningFunctionality_oldCompatibility_11_2_2016();
    bool checkAndSetWarning_oldMotionPlanningFunctionality_oldCompatibility_11_2_2016();

    void setCustomizationScriptIsTemporarilyDisabled(bool disabled);
    bool getCustomizationScriptIsTemporarilyDisabled() const;
    void setCustomizationScriptIsDisabledDuringSimulation(bool disabled);
    bool getCustomizationScriptIsDisabledDuringSimulation() const;
    void setCustomizationScriptCleanupBeforeSave(bool doIt);
    bool getCustomizationScriptCleanupBeforeSave() const;

    bool hasCustomizationScripAnyChanceToGetExecuted(bool whenSimulationRuns,bool forCleanUpSection) const;

    int getScriptExecutionTimeInMs() const;

    int getErrorReportMode() const;
    void setErrorReportMode(int e);

    std::string getLastErrorString() const;
    void setLastErrorString(const char* txt);

    int getAddOnExecutionState() const;
    void setAddOnDesiredExecutionState(int state);

    bool addCommandToOutsideCommandQueue(int commandID,int auxVal1,int auxVal2,int auxVal3,int auxVal4,const float aux2Vals[8],int aux2Count);
    int extractCommandFromOutsideCommandQueue(int auxVals[4],float aux2Vals[8],int& aux2Count);

    static bool emergencyStopButtonPressed;

    void fromFileToBuffer(); // when using an external editor
    void fromBufferToFile();

    void setInsideCustomLuaFunction(bool inside);
    bool getInsideCustomLuaFunction() const;

    std::string getFilenameForExternalScriptEditor() const;

    VTHREAD_ID_TYPE getThreadedScriptThreadId() const;

protected:
    bool _luaLoadBuffer(luaWrap_lua_State* luaState,const char* buff,size_t sz,const char* name);
    int _luaPCall(luaWrap_lua_State* luaState,int nargs,int nresult,int errfunc);

    int _runMainScript();
    int _runMainScriptNow(int callType);
    int _runNonThreadedChildScript(CInterfaceStack& inputArguments,int callType);
    int _runNonThreadedChildScriptNow(CInterfaceStack& inputArguments,int callType);


    bool _runCustomizationScript(int callType);
    void _insertScriptText(bool toFront,const char* txt);
    bool _replaceScriptText(const char* oldTxt,const char* newTxt);
    bool _replaceScriptText(const char* oldTxt1,const char* oldTxt2,const char* oldTxt3,const char* newTxt);
    bool _replaceScriptTextKeepMiddleUnchanged(const char* oldTxtStart,const char* oldTxtEnd,const char* newTxtStart,const char* newTxtEnd);
    bool _containsScriptText(const char* txt);

    bool _initializeScriptForFunctionCallOrVarSettingIfNeeded();

    // Variables that need to be copied and serialized:
    int scriptID;
    int _scriptType; // sim_scriptproperty_mainscript, etc.
    bool _threadedExecution;
    bool _scriptIsDisabled;
    bool _executeJustOnce;
    bool _mainScriptIsDefaultMainScript;
    int _executionOrder;
    int _objectIDAttachedTo_child;
    int _objectIDAttachedTo_callback;
    int _objectIDAttachedTo_customization;
    char* _scriptText;
    int _scriptTextLength;
    std::vector<int> _scriptFoldingInfo;
    char* _scriptTextExec;      // the one getting executed!
    int _scriptTextExecLength;  // the one getting executed!
    CLuaScriptParameters* scriptParameters;
    COutsideCommandQueueForScript* _outsideCommandQueue;
    CCustomData* _customObjectData;
    CCustomData* _customObjectData_tempData; // same as above, but is not serialized (but copied!)

    // Other variables that don't need serialization:
    luaWrap_lua_State* L;
    VTHREAD_ID_TYPE _threadedScript_associatedFiberOrThreadID;
    int _numberOfPasses;
    bool _threadedExecutionUnderWay;
    int _insideCustomLuaFunction;
    bool _inExecutionNow;
    int _loadBufferResult;

    bool _flaggedForDestruction;

    bool _customizationScriptIsTemporarilyDisabled;
    bool _customizationScriptIsDisabledDuringSimulation;
    bool _customizationScriptCleanupBeforeSave;
    int _scriptExecStartTime;
    int _errorReportMode;
    std::string _lastErrorString;

    int _messageReportingOverride;
    bool _warningAboutSimHandleChildScriptAlreadyIssued_oldCompatibility_7_8_2014;
    bool _warning_simRMLPosition_oldCompatibility_30_8_2014;
    bool _warning_simRMLVelocity_oldCompatibility_30_8_2014;
    bool _warning_simGetMpConfigForTipPose_oldCompatibility_21_1_2016;
    bool _warning_simFindIkPath_oldCompatibility_2_2_2016;
    bool _warning_oldPathPlanningFunctionality_oldCompatibility_11_2_2016;
    bool _warning_oldMotionPlanningFunctionality_oldCompatibility_11_2_2016;

    bool _automaticCascadingCallsDisabled; // reset to false at simulation start!

    VMutex _localMutex;
    static VMutex _globalMutex;
    std::string _addOnName;
    int _addOn_executionState; // 0=stopped, 1=running, 2=paused, 3=error
    int _addOn_desiredExecutionState; // 1=run, 2=pause

    std::string _filenameForExternalScriptEditor;
    static int _nextIdForExternalScriptEditor;

    static std::vector<CLuaScriptObject*> toBeCalledByThread;
    static VTHREAD_RETURN_TYPE _startAddressForThreadedScripts(VTHREAD_ARGUMENT_TYPE lpData);


    std::vector<char*> _userData;
    std::vector<int> _userDataIds;

    bool _initialValuesInitialized;

    int _previousEditionWindowPosAndSize[4];
};
