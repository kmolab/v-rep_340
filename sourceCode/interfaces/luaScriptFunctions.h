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
#include "vMutex.h"

class CLuaScriptObject;
class CInterfaceStack;
class CLuaCustomFunction;

enum {lua_arg_empty,lua_arg_nil,lua_arg_number,lua_arg_bool,lua_arg_string,lua_arg_function,lua_arg_userdata,lua_arg_table};


luaWrap_lua_State* initializeNewLuaState(const char* scriptSuffixNumberString);
void registerNewLuaFunctions(luaWrap_lua_State* L);
void prepareNewLuaVariables(luaWrap_lua_State* L,const char* scriptSuffixNumberString);
void setNewLuaVariable(luaWrap_lua_State* L,const char* name,int identifier);

void pushCorrectTypeOntoLuaStack(luaWrap_lua_State* L,const std::string& txt);
int getCorrectType(const std::string& txt);

bool readCustomFunctionDataFromStack(luaWrap_lua_State* L,int ind,int dataType,
                                     std::vector<char>& inBoolVector,
                                     std::vector<int>& inIntVector,
                                     std::vector<float>& inFloatVector,
                                     std::vector<double>& inDoubleVector,
                                     std::vector<std::string>& inStringVector,
                                     std::vector<std::string>& inCharVector,
                                    std::vector<int>& inInfoVector);
void writeCustomFunctionDataOntoStack(luaWrap_lua_State* L,int dataType,int dataSize,
                                      unsigned char* boolData,int& boolDataPos,
                                      int* intData,int& intDataPos,
                                      float* floatData,int& floatDataPos,
                                      double* doubleData,int& doubleDataPos,
                                      char* stringData,int& stringDataPos,
                                      char* charData,int& charDataPos);



void getFloatsFromTable(luaWrap_lua_State* L,int tablePos,int floatCount,float* arrayField);
void getDoublesFromTable(luaWrap_lua_State* L,int tablePos,int doubleCount,double* arrayField);
bool getIntsFromTable(luaWrap_lua_State* L,int tablePos,int intCount,int* arrayField);
bool getUIntsFromTable(luaWrap_lua_State* L,int tablePos,int intCount,unsigned int* arrayField);
bool getUCharsFromTable(luaWrap_lua_State* L,int tablePos,int intCount,unsigned char* arrayField);
void getBoolsFromTable(luaWrap_lua_State* L,int tablePos,int boolCount,char* arrayField);
void pushFloatTableOntoStack(luaWrap_lua_State* L,int floatCount,const float* arrayField);
void pushDoubleTableOntoStack(luaWrap_lua_State* L,int doubleCount,const double* arrayField);
void pushIntTableOntoStack(luaWrap_lua_State* L,int intCount,const int* arrayField);
void pushUIntTableOntoStack(luaWrap_lua_State* L,int intCount,const unsigned int* arrayField);
void pushUCharTableOntoStack(luaWrap_lua_State* L,int intCount,const unsigned char* arrayField);
void pushStringTableOntoStack(luaWrap_lua_State* L,const std::vector<std::string>& stringTable);
void insertFloatsIntoTableAlreadyOnStack(luaWrap_lua_State* L,int tablePos,int floatCount,const float* arrayField);


int luaToInt(luaWrap_lua_State* L,int pos);
float luaToFloat(luaWrap_lua_State* L,int pos);
double luaToDouble(luaWrap_lua_State* L,int pos);
bool luaToBool(luaWrap_lua_State* L,int pos);

bool isDashFree(const std::string& functionName,const std::string& name);
bool suffixAdjustStringIfNeeded(const std::string& functionName,bool outputError,luaWrap_lua_State* L,std::string& name);
int getCurrentScriptID(luaWrap_lua_State* L);

void getScriptTree_mainOr(luaWrap_lua_State* L,bool selfIncluded,std::vector<int>& scriptHandles);
void getScriptChain(luaWrap_lua_State* L,bool selfIncluded,bool mainIncluded,std::vector<int>& scriptHandles);

void luaApiCallWarning(const char* functionName,const char* message);
bool _hasErrors(std::string& funcErrorString);

void memorizeLocation(luaWrap_lua_State* L);
int getLocationIndex(VTHREAD_ID_TYPE threadID);
void forgetLocation();
std::string getLocationString();


// Input argument checking:
bool checkInputArguments(luaWrap_lua_State* L,std::string* errStr,
                         int type1=lua_arg_empty,int type1Cnt_zeroIfNotTable=-2,
                         int type2=lua_arg_empty,int type2Cnt_zeroIfNotTable=-2,
                         int type3=lua_arg_empty,int type3Cnt_zeroIfNotTable=-2,
                         int type4=lua_arg_empty,int type4Cnt_zeroIfNotTable=-2,
                         int type5=lua_arg_empty,int type5Cnt_zeroIfNotTable=-2,
                         int type6=lua_arg_empty,int type6Cnt_zeroIfNotTable=-2,
                         int type7=lua_arg_empty,int type7Cnt_zeroIfNotTable=-2,
                         int type8=lua_arg_empty,int type8Cnt_zeroIfNotTable=-2,
                         int type9=lua_arg_empty,int type9Cnt_zeroIfNotTable=-2,
                         int type10=lua_arg_empty,int type10Cnt_zeroIfNotTable=-2,
                         int type11=lua_arg_empty,int type11Cnt_zeroIfNotTable=-2);
int checkOneGeneralInputArgument(luaWrap_lua_State* L,int index,
                           int type,int cnt_orZeroIfNotTable,bool optional,bool nilInsteadOfTypeAndCountAllowed,std::string* errStr);
bool checkOneInputArgument(luaWrap_lua_State* L,int index,int type,std::string* errStr);


void luaHookFunction(luaWrap_lua_State* L,luaWrap_lua_Debug* ar);
void moduleCommonPart(luaWrap_lua_State* L,int action,std::string* errorString);


int handleChildScriptsRoutine(int callType,CLuaScriptObject* it,CInterfaceStack& inputArguments);
int launchThreadedChildScriptsRoutine(CLuaScriptObject* it);

int _simHandleChildScript(luaWrap_lua_State* L);
int _simHandleChildScripts(luaWrap_lua_State* L);
int _simLaunchThreadedChildScripts(luaWrap_lua_State* L);
int _simHandleSensingChildScripts(luaWrap_lua_State* L);
int _simGetScriptName(luaWrap_lua_State* L);
int _simGetObjectAssociatedWithScript(luaWrap_lua_State* L);
int _simGetScriptAssociatedWithObject(luaWrap_lua_State* L);
int _simGetCustomizationScriptAssociatedWithObject(luaWrap_lua_State* L);

int _simGenericFunctionHandler(luaWrap_lua_State* L);
int _genericFunctionHandler_new(luaWrap_lua_State* L,CLuaCustomFunction* func,std::string& raiseErrorWithMsg);
int _genericFunctionHandler_old(luaWrap_lua_State* L,CLuaCustomFunction* func);
int _simGetScriptExecutionCount(luaWrap_lua_State* L);
int _simIsScriptExecutionThreaded(luaWrap_lua_State* L);
int _simIsScriptRunningInThread(luaWrap_lua_State* L);
int _simOpenModule(luaWrap_lua_State* L);
int _simCloseModule(luaWrap_lua_State* L);
int _simHandleModule(luaWrap_lua_State* L);

int _simBoolOr16(luaWrap_lua_State* L);
int _simBoolAnd16(luaWrap_lua_State* L);
int _simBoolXor16(luaWrap_lua_State* L);
int _simBoolOr32(luaWrap_lua_State* L);
int _simBoolAnd32(luaWrap_lua_State* L);
int _simBoolXor32(luaWrap_lua_State* L);

int _simHandleDynamics(luaWrap_lua_State* L);
int _simHandleIkGroup(luaWrap_lua_State* L);
int _simCheckIkGroup(luaWrap_lua_State* L);
int _simHandleCollision(luaWrap_lua_State* L);
int _simReadCollision(luaWrap_lua_State* L);
int _simHandleDistance(luaWrap_lua_State* L);
int _simReadDistance(luaWrap_lua_State* L);
int _simHandleProximitySensor(luaWrap_lua_State* L);
int _simReadProximitySensor(luaWrap_lua_State* L);
int _simHandleMill(luaWrap_lua_State* L);
int _simResetCollision(luaWrap_lua_State* L);
int _simResetDistance(luaWrap_lua_State* L);
int _simResetProximitySensor(luaWrap_lua_State* L);
int _simResetMill(luaWrap_lua_State* L);
int _simCheckProximitySensor(luaWrap_lua_State* L);
int _simCheckProximitySensorEx(luaWrap_lua_State* L);
int _simCheckProximitySensorEx2(luaWrap_lua_State* L);
int _simGetObjectHandle(luaWrap_lua_State* L);
int _simGetScriptHandle(luaWrap_lua_State* L);
int _simGetCollectionHandle(luaWrap_lua_State* L);
int _simRemoveCollection(luaWrap_lua_State* L);
int _simEmptyCollection(luaWrap_lua_State* L);
int _simGetObjectPosition(luaWrap_lua_State* L);
int _simGetObjectOrientation(luaWrap_lua_State* L);
int _simSetObjectPosition(luaWrap_lua_State* L);
int _simSetObjectOrientation(luaWrap_lua_State* L);
int _simGetJointPosition(luaWrap_lua_State* L);
int _simSetJointPosition(luaWrap_lua_State* L);
int _simSetJointTargetPosition(luaWrap_lua_State* L);
int _simGetJointTargetPosition(luaWrap_lua_State* L);
int _simSetJointForce(luaWrap_lua_State* L);
int _simGetPathPosition(luaWrap_lua_State* L);
int _simSetPathPosition(luaWrap_lua_State* L);
int _simGetPathLength(luaWrap_lua_State* L);
int _simSetJointTargetVelocity(luaWrap_lua_State* L);
int _simGetJointTargetVelocity(luaWrap_lua_State* L);
int _simSetPathTargetNominalVelocity(luaWrap_lua_State* L);
int _simGetObjectName(luaWrap_lua_State* L);
int _simGetCollectionName(luaWrap_lua_State* L);
int _simSetObjectName(luaWrap_lua_State* L);
int _simSetCollectionName(luaWrap_lua_State* L);
int _simRemoveObject(luaWrap_lua_State* L);
int _simRemoveModel(luaWrap_lua_State* L);
int _simGetSimulationTime(luaWrap_lua_State* L);
int _simGetSimulationState(luaWrap_lua_State* L);
int _simGetSystemTime(luaWrap_lua_State* L);
int _simGetSystemTimeInMs(luaWrap_lua_State* L);
int _simCheckCollision(luaWrap_lua_State* L);
int _simCheckCollisionEx(luaWrap_lua_State* L);
int _simCheckDistance(luaWrap_lua_State* L);
int _simGetObjectConfiguration(luaWrap_lua_State* L);
int _simSetObjectConfiguration(luaWrap_lua_State* L);
int _simGetConfigurationTree(luaWrap_lua_State* L);
int _simSetConfigurationTree(luaWrap_lua_State* L);
int _simHandleMechanism(luaWrap_lua_State* L);
int _simGetSimulationTimeStep(luaWrap_lua_State* L);
int _simGetSimulatorMessage(luaWrap_lua_State* L);

int _simAddScript(luaWrap_lua_State* L);
int _simAssociateScriptWithObject(luaWrap_lua_State* L);
int _simSetScriptText(luaWrap_lua_State* L);

int _simResetTracing(luaWrap_lua_State* L);
int _simHandleTracing(luaWrap_lua_State* L);

int _simResetGraph(luaWrap_lua_State* L);
int _simHandleGraph(luaWrap_lua_State* L);
int _simAddStatusbarMessage(luaWrap_lua_State* L);
int _simGetLastError(luaWrap_lua_State* L);
int _simGetObjects(luaWrap_lua_State* L);
int _simRefreshDialogs(luaWrap_lua_State* L);
int _simGetModuleName(luaWrap_lua_State* L);
int _simGetIkGroupHandle(luaWrap_lua_State* L);
int _simGetCollisionHandle(luaWrap_lua_State* L);
int _simRemoveScript(luaWrap_lua_State* L);
int _simGetDistanceHandle(luaWrap_lua_State* L);
int _simGetScriptSimulationParameter(luaWrap_lua_State* L);
int _simSetScriptSimulationParameter(luaWrap_lua_State* L);
int _simDisplayDialog(luaWrap_lua_State* L);
int _simGetDialogResult(luaWrap_lua_State* L);
int _simGetDialogInput(luaWrap_lua_State* L);
int _simEndDialog(luaWrap_lua_State* L);
int _simStopSimulation(luaWrap_lua_State* L);
int _simPauseSimulation(luaWrap_lua_State* L);
int _simStartSimulation(luaWrap_lua_State* L);

int _simGetObjectMatrix(luaWrap_lua_State* L);
int _simSetObjectMatrix(luaWrap_lua_State* L);
int _simGetJointMatrix(luaWrap_lua_State* L);
int _simSetSphericalJointMatrix(luaWrap_lua_State* L);

int _simBuildIdentityMatrix(luaWrap_lua_State* L);
int _simCopyMatrix(luaWrap_lua_State* L);
int _simBuildMatrix(luaWrap_lua_State* L);
int _simGetEulerAnglesFromMatrix(luaWrap_lua_State* L);
int _simInvertMatrix(luaWrap_lua_State* L);
int _simMultiplyMatrices(luaWrap_lua_State* L);
int _simInterpolateMatrices(luaWrap_lua_State* L);
int _simMultiplyVector(luaWrap_lua_State* L);

int _simGetObjectParent(luaWrap_lua_State* L);
int _simSetObjectParent(luaWrap_lua_State* L);
int _simGetObjectChild(luaWrap_lua_State* L);
int _simGetObjectType(luaWrap_lua_State* L);
int _simGetJointType(luaWrap_lua_State* L);

int _simSetBoolParameter(luaWrap_lua_State* L);
int _simGetBoolParameter(luaWrap_lua_State* L);
int _simSetInt32Parameter(luaWrap_lua_State* L);
int _simGetInt32Parameter(luaWrap_lua_State* L);
int _simSetFloatParameter(luaWrap_lua_State* L);
int _simGetFloatParameter(luaWrap_lua_State* L);
int _simSetStringParameter(luaWrap_lua_State* L);
int _simGetStringParameter(luaWrap_lua_State* L);
int _simSetArrayParameter(luaWrap_lua_State* L);
int _simGetArrayParameter(luaWrap_lua_State* L);


int _simGetJointInterval(luaWrap_lua_State* L);
int _simSetJointInterval(luaWrap_lua_State* L);

int _simLoadScene(luaWrap_lua_State* L);
int _simSaveScene(luaWrap_lua_State* L);
int _simLoadModel(luaWrap_lua_State* L);
int _simSaveModel(luaWrap_lua_State* L);


int _simIsObjectInSelection(luaWrap_lua_State* L);
int _simAddObjectToSelection(luaWrap_lua_State* L);
int _simRemoveObjectFromSelection(luaWrap_lua_State* L);
int _simGetObjectSelectionSize(luaWrap_lua_State* L);
int _simGetObjectLastSelection(luaWrap_lua_State* L);
int _simGetObjectSelection(luaWrap_lua_State* L);

int _simGetRealTimeSimulation(luaWrap_lua_State* L);

int _simLockInterface(luaWrap_lua_State* L);

int _simGetMechanismHandle(luaWrap_lua_State* L);
int _simGetPathPlanningHandle(luaWrap_lua_State* L);
int _simSearchPath(luaWrap_lua_State* L);
int _simInitializePathSearch(luaWrap_lua_State* L);
int _simPerformPathSearchStep(luaWrap_lua_State* L);

int _simSetNavigationMode(luaWrap_lua_State* L);
int _simGetNavigationMode(luaWrap_lua_State* L);
int _simSetPage(luaWrap_lua_State* L);
int _simGetPage(luaWrap_lua_State* L);

int _simReleaseScriptRawBuffer(luaWrap_lua_State* L);

int _simCopyPasteSelectedObjects(luaWrap_lua_State* L);
int _simCopyPasteObjects(luaWrap_lua_State* L);
int _simDeleteSelectedObjects(luaWrap_lua_State* L);
int _simScaleSelectedObjects(luaWrap_lua_State* L);
int _simScaleObjects(luaWrap_lua_State* L);

int _simGetObjectUniqueIdentifier(luaWrap_lua_State* L);

int _simGetNameSuffix(luaWrap_lua_State* L);
int _simSetNameSuffix(luaWrap_lua_State* L);

int _simSetThreadAutomaticSwitch(luaWrap_lua_State* L);
int _simGetThreadAutomaticSwitch(luaWrap_lua_State* L);
int _simSetThreadSwitchTiming(luaWrap_lua_State* L);
int _simSetThreadResumeLocation(luaWrap_lua_State* L);
int _simResumeThreads(luaWrap_lua_State* L);
int _simSwitchThread(luaWrap_lua_State* L);
int _simCreateIkGroup(luaWrap_lua_State* L);
int _simRemoveIkGroup(luaWrap_lua_State* L);
int _simCreateIkElement(luaWrap_lua_State* L);
int _simCreateMotionPlanning(luaWrap_lua_State* L);
int _simRemoveMotionPlanning(luaWrap_lua_State* L);
int _simCreateCollection(luaWrap_lua_State* L);
int _simAddObjectToCollection(luaWrap_lua_State* L);
int _simSaveImage(luaWrap_lua_State* L);
int _simLoadImage(luaWrap_lua_State* L);
int _simGetScaledImage(luaWrap_lua_State* L);
int _simTransformImage(luaWrap_lua_State* L);
int _simGetQHull(luaWrap_lua_State* L);
int _simGetDecimatedMesh(luaWrap_lua_State* L);
int _simExportIk(luaWrap_lua_State* L);
int _simComputeJacobian(luaWrap_lua_State* L);


int _simSendData(luaWrap_lua_State* L);
int _simReceiveData(luaWrap_lua_State* L);


int _simPackTable(luaWrap_lua_State* L);
int _simUnpackTable(luaWrap_lua_State* L);

int _simPackInt32Table(luaWrap_lua_State* L);
int _simPackUInt32Table(luaWrap_lua_State* L);
int _simPackFloatTable(luaWrap_lua_State* L);
int _simPackDoubleTable(luaWrap_lua_State* L);
int _simPackUInt8Table(luaWrap_lua_State* L);
int _simPackUInt16Table(luaWrap_lua_State* L);
int _simUnpackInt32Table(luaWrap_lua_State* L);
int _simUnpackUInt32Table(luaWrap_lua_State* L);
int _simUnpackFloatTable(luaWrap_lua_State* L);
int _simUnpackDoubleTable(luaWrap_lua_State* L);
int _simUnpackUInt8Table(luaWrap_lua_State* L);
int _simUnpackUInt16Table(luaWrap_lua_State* L);
int _simTransformBuffer(luaWrap_lua_State* L);
int _simCombineRgbImages(luaWrap_lua_State* L);

int _simGetVelocity(luaWrap_lua_State* L);
int _simGetObjectVelocity(luaWrap_lua_State* L);
int _simAddForceAndTorque(luaWrap_lua_State* L);
int _simAddForce(luaWrap_lua_State* L);

int _simSetExplicitHandling(luaWrap_lua_State* L);
int _simGetExplicitHandling(luaWrap_lua_State* L);

int _simGetLinkDummy(luaWrap_lua_State* L);
int _simSetLinkDummy(luaWrap_lua_State* L);

int _simSetGraphUserData(luaWrap_lua_State* L);


int _simAddDrawingObject(luaWrap_lua_State* L);
int _simRemoveDrawingObject(luaWrap_lua_State* L);
int _simAddDrawingObjectItem(luaWrap_lua_State* L);

int _simAddParticleObject(luaWrap_lua_State* L);
int _simRemoveParticleObject(luaWrap_lua_State* L);
int _simAddParticleObjectItem(luaWrap_lua_State* L);

int _simSerialOpen(luaWrap_lua_State* L);
int _simSerialClose(luaWrap_lua_State* L);
int _simSerialSend(luaWrap_lua_State* L);
int _simSerialRead(luaWrap_lua_State* L);
int _simSerialCheck(luaWrap_lua_State* L);

int _simSerialPortOpen(luaWrap_lua_State* L);
int _simSerialPortClose(luaWrap_lua_State* L);
int _simSerialPortSend(luaWrap_lua_State* L);
int _simSerialPortRead(luaWrap_lua_State* L);

int _simGetObjectSizeFactor(luaWrap_lua_State* L);

int _simResetMilling(luaWrap_lua_State* L);
int _simApplyMilling(luaWrap_lua_State* L);


int _simSetIntegerSignal(luaWrap_lua_State* L);
int _simGetIntegerSignal(luaWrap_lua_State* L);
int _simClearIntegerSignal(luaWrap_lua_State* L);
int _simSetFloatSignal(luaWrap_lua_State* L);
int _simGetFloatSignal(luaWrap_lua_State* L);
int _simClearFloatSignal(luaWrap_lua_State* L);
int _simSetStringSignal(luaWrap_lua_State* L);
int _simGetStringSignal(luaWrap_lua_State* L);
int _simClearStringSignal(luaWrap_lua_State* L);
int _simGetSignalName(luaWrap_lua_State* L);
int _simWaitForSignal(luaWrap_lua_State* L);

int _simPersistentDataWrite(luaWrap_lua_State* L);
int _simPersistentDataRead(luaWrap_lua_State* L);

int _simSetObjectProperty(luaWrap_lua_State* L);
int _simGetObjectProperty(luaWrap_lua_State* L);

int _simSetObjectSpecialProperty(luaWrap_lua_State* L);
int _simGetObjectSpecialProperty(luaWrap_lua_State* L);

int _simSetModelProperty(luaWrap_lua_State* L);
int _simGetModelProperty(luaWrap_lua_State* L);

int _simMoveToPosition(luaWrap_lua_State* L);
int _simMoveToObject(luaWrap_lua_State* L);
int _simFollowPath(luaWrap_lua_State* L);
int _simMoveToJointPositions(luaWrap_lua_State* L);

int _simWait(luaWrap_lua_State* L);
int _simDelegateChildScriptExecution(luaWrap_lua_State* L);

int _simGetDataOnPath(luaWrap_lua_State* L);
int _simGetPositionOnPath(luaWrap_lua_State* L);
int _simGetOrientationOnPath(luaWrap_lua_State* L);
int _simGetClosestPositionOnPath(luaWrap_lua_State* L);

int _simReadForceSensor(luaWrap_lua_State* L);
int _simBreakForceSensor(luaWrap_lua_State* L);

int _simGetShapeVertex(luaWrap_lua_State* L);
int _simGetShapeTriangle(luaWrap_lua_State* L);

int _simGetLightParameters(luaWrap_lua_State* L);
int _simSetLightParameters(luaWrap_lua_State* L);

int _simSetShapeColor(luaWrap_lua_State* L);
int _simGetShapeColor(luaWrap_lua_State* L);
int _simResetDynamicObject(luaWrap_lua_State* L);
int _simSetJointMode(luaWrap_lua_State* L);
int _simGetJointMode(luaWrap_lua_State* L);

int _simGetContactInfo(luaWrap_lua_State* L);
int _simSetThreadIsFree(luaWrap_lua_State* L);

int _simTubeOpen(luaWrap_lua_State* L);
int _simTubeClose(luaWrap_lua_State* L);
int _simTubeWrite(luaWrap_lua_State* L);
int _simTubeRead(luaWrap_lua_State* L);
int _simTubeStatus(luaWrap_lua_State* L);

int _simAuxiliaryConsoleOpen(luaWrap_lua_State* L);
int _simAuxiliaryConsoleClose(luaWrap_lua_State* L);
int _simAuxiliaryConsolePrint(luaWrap_lua_State* L);
int _simAuxiliaryConsoleShow(luaWrap_lua_State* L);

int _simImportShape(luaWrap_lua_State* L);
int _simImportMesh(luaWrap_lua_State* L);
int _simExportMesh(luaWrap_lua_State* L);
int _simCreateMeshShape(luaWrap_lua_State* L);
int _simGetShapeMesh(luaWrap_lua_State* L);
int _simCreatePureShape(luaWrap_lua_State* L);
int _simCreateHeightfieldShape(luaWrap_lua_State* L);

int _simAddBanner(luaWrap_lua_State* L);
int _simRemoveBanner(luaWrap_lua_State* L);

int _simCreateJoint(luaWrap_lua_State* L);
int _simCreateDummy(luaWrap_lua_State* L);
int _simCreateProximitySensor(luaWrap_lua_State* L);
int _simCreatePath(luaWrap_lua_State* L);
int _simInsertPathCtrlPoints(luaWrap_lua_State* L);
int _simCutPathCtrlPoints(luaWrap_lua_State* L);
int _simGetIkGroupMatrix(luaWrap_lua_State* L);
int _simCreateForceSensor(luaWrap_lua_State* L);
int _simCreateVisionSensor(luaWrap_lua_State* L);

int _simFloatingViewAdd(luaWrap_lua_State* L);
int _simFloatingViewRemove(luaWrap_lua_State* L);
int _simAdjustView(luaWrap_lua_State* L);
int _simCameraFitToView(luaWrap_lua_State* L);
int _simAnnounceSceneContentChange(luaWrap_lua_State* L);

int _simGetObjectInt32Parameter(luaWrap_lua_State* L);
int _simSetObjectInt32Parameter(luaWrap_lua_State* L);
int _simGetObjectFloatParameter(luaWrap_lua_State* L);
int _simSetObjectFloatParameter(luaWrap_lua_State* L);
int _simGetObjectStringParameter(luaWrap_lua_State* L);
int _simSetObjectStringParameter(luaWrap_lua_State* L);

int _simGetRotationAxis(luaWrap_lua_State* L);
int _simRotateAroundAxis(luaWrap_lua_State* L);

int _simLaunchExecutable(luaWrap_lua_State* L);

int _simGetJointForce(luaWrap_lua_State* L);
int _simJointGetForce(luaWrap_lua_State* L);

int _simSetIkGroupProperties(luaWrap_lua_State* L);
int _simSetIkElementProperties(luaWrap_lua_State* L);

int _simIsHandleValid(luaWrap_lua_State* L);


int _simHandleVisionSensor(luaWrap_lua_State* L);
int _simReadVisionSensor(luaWrap_lua_State* L);
int _simResetVisionSensor(luaWrap_lua_State* L);
int _simGetVisionSensorResolution(luaWrap_lua_State* L);
int _simGetVisionSensorImage(luaWrap_lua_State* L);
int _simGetVisionSensorCharImage(luaWrap_lua_State* L);
int _simSetVisionSensorImage(luaWrap_lua_State* L);
int _simSetVisionSensorCharImage(luaWrap_lua_State* L);
int _simGetVisionSensorDepthBuffer(luaWrap_lua_State* L);
int _simCheckVisionSensor(luaWrap_lua_State* L);
int _simCheckVisionSensorEx(luaWrap_lua_State* L);

int _simRMLPos(luaWrap_lua_State* L);
int _simRMLVel(luaWrap_lua_State* L);
int _simRMLStep(luaWrap_lua_State* L);
int _simRMLRemove(luaWrap_lua_State* L);

int _simRMLMoveToPosition(luaWrap_lua_State* L);
int _simRMLMoveToJointPositions(luaWrap_lua_State* L);

int _simGetObjectQuaternion(luaWrap_lua_State* L);
int _simSetObjectQuaternion(luaWrap_lua_State* L);

int _simSetShapeMassAndInertia(luaWrap_lua_State* L);
int _simGetShapeMassAndInertia(luaWrap_lua_State* L);

int _simGroupShapes(luaWrap_lua_State* L);
int _simUngroupShape(luaWrap_lua_State* L);
int _simConvexDecompose(luaWrap_lua_State* L);

int _simGetMotionPlanningHandle(luaWrap_lua_State* L);
int _simFindMpPath(luaWrap_lua_State* L);
int _simSimplifyMpPath(luaWrap_lua_State* L);
int _simFindIkPath(luaWrap_lua_State* L);
int _simGetMpConfigTransition(luaWrap_lua_State* L);

int _simAddGhost(luaWrap_lua_State* L);
int _simModifyGhost(luaWrap_lua_State* L);

int _simQuitSimulator(luaWrap_lua_State* L);
int _simGetThreadId(luaWrap_lua_State* L);

int _simSetShapeMaterial(luaWrap_lua_State* L);

int _simGetTextureId(luaWrap_lua_State* L);
int _simReadTexture(luaWrap_lua_State* L);
int _simWriteTexture(luaWrap_lua_State* L);
int _simCreateTexture(luaWrap_lua_State* L);

int _simWriteCustomDataBlock(luaWrap_lua_State* L);
int _simReadCustomDataBlock(luaWrap_lua_State* L);
int _simReadCustomDataBlockTags(luaWrap_lua_State* L);

int _simAddPointCloud(luaWrap_lua_State* L);
int _simModifyPointCloud(luaWrap_lua_State* L);

int _simGetShapeGeomInfo(luaWrap_lua_State* L);
int _simGetObjectsInTree(luaWrap_lua_State* L);

int _simSetObjectSizeValues(luaWrap_lua_State* L);
int _simGetObjectSizeValues(luaWrap_lua_State* L);
int _simScaleObject(luaWrap_lua_State* L);
int _simSetShapeTexture(luaWrap_lua_State* L);
int _simGetShapeTextureId(luaWrap_lua_State* L);
int _simGetCollectionObjects(luaWrap_lua_State* L);
int _simHandleCustomizationScripts(luaWrap_lua_State* L);
int _simSetScriptAttribute(luaWrap_lua_State* L);
int _simGetScriptAttribute(luaWrap_lua_State* L);
int _simReorientShapeBoundingBox(luaWrap_lua_State* L);



int _simBuildMatrixQ(luaWrap_lua_State* L);
int _simGetQuaternionFromMatrix(luaWrap_lua_State* L);
int _simFileDialog(luaWrap_lua_State* L);
int _simMsgBox(luaWrap_lua_State* L);

int _simLoadModule(luaWrap_lua_State* L);
int _simUnloadModule(luaWrap_lua_State* L);
int _simCallScriptFunction(luaWrap_lua_State* L);
int _simGetConfigForTipPose(luaWrap_lua_State* L);
int _simGenerateIkPath(luaWrap_lua_State* L);
int _simGetExtensionString(luaWrap_lua_State* L);
int _simComputeMassAndInertia(luaWrap_lua_State* L);
int _simSetScriptVariable(luaWrap_lua_State* L);
int _simGetEngineFloatParameter(luaWrap_lua_State* L);
int _simGetEngineInt32Parameter(luaWrap_lua_State* L);
int _simGetEngineBoolParameter(luaWrap_lua_State* L);
int _simSetEngineFloatParameter(luaWrap_lua_State* L);
int _simSetEngineInt32Parameter(luaWrap_lua_State* L);
int _simSetEngineBoolParameter(luaWrap_lua_State* L);
int _simCreateOctree(luaWrap_lua_State* L);
int _simCreatePointCloud(luaWrap_lua_State* L);
int _simSetPointCloudOptions(luaWrap_lua_State* L);
int _simGetPointCloudOptions(luaWrap_lua_State* L);
int _simInsertVoxelsIntoOctree(luaWrap_lua_State* L);
int _simRemoveVoxelsFromOctree(luaWrap_lua_State* L);
int _simInsertPointsIntoPointCloud(luaWrap_lua_State* L);
int _simRemovePointsFromPointCloud(luaWrap_lua_State* L);
int _simIntersectPointsWithPointCloud(luaWrap_lua_State* L);
int _simGetOctreeVoxels(luaWrap_lua_State* L);
int _simGetPointCloudPoints(luaWrap_lua_State* L);
int _simInsertObjectIntoOctree(luaWrap_lua_State* L);
int _simSubtractObjectFromOctree(luaWrap_lua_State* L);
int _simInsertObjectIntoPointCloud(luaWrap_lua_State* L);
int _simSubtractObjectFromPointCloud(luaWrap_lua_State* L);
int _simCheckOctreePointOccupancy(luaWrap_lua_State* L);
int _simOpenTextEditor(luaWrap_lua_State* L);
int _simSetVisionSensorFilter(luaWrap_lua_State* L);
int _simGetVisionSensorFilter(luaWrap_lua_State* L);
int _simHandleSimulationStart(luaWrap_lua_State* L);
int _simHandleSensingStart(luaWrap_lua_State* L);
int _simAuxFunc(luaWrap_lua_State* L);
int _simSetReferencedHandles(luaWrap_lua_State* L);
int _simGetReferencedHandles(luaWrap_lua_State* L);
int _simGetGraphCurve(luaWrap_lua_State* L);




// DEPRECATED
int _simGetMaterialId(luaWrap_lua_State* L);
int _simGetShapeMaterial(luaWrap_lua_State* L);
int _simHandleVarious(luaWrap_lua_State* L);
int _simGetInstanceIndex(luaWrap_lua_State* L);
int _simGetVisibleInstanceIndex(luaWrap_lua_State* L);
int _simRMLPosition(luaWrap_lua_State* L); // deprecated since 23/5/2014
int _simRMLVelocity(luaWrap_lua_State* L); // deprecated since 23/5/2014
int _simResetPath(luaWrap_lua_State* L);
int _simHandlePath(luaWrap_lua_State* L);
int _simResetJoint(luaWrap_lua_State* L);
int _simHandleJoint(luaWrap_lua_State* L);
int _simGetMpConfigForTipPose(luaWrap_lua_State* L);
int _simEnableWorkThreads(luaWrap_lua_State* L);
int _simWaitForWorkThreads(luaWrap_lua_State* L);
int _simGetInvertedMatrix(luaWrap_lua_State* L);
int _simGetSystemTimeInMilliseconds(luaWrap_lua_State* L); // deprecated
int _simAddSceneCustomData(luaWrap_lua_State* L);
int _simGetSceneCustomData(luaWrap_lua_State* L);
int _simAddObjectCustomData(luaWrap_lua_State* L);
int _simGetObjectCustomData(luaWrap_lua_State* L);
int _simSetUIPosition(luaWrap_lua_State* L);
int _simGetUIPosition(luaWrap_lua_State* L);
int _simGetUIHandle(luaWrap_lua_State* L);
int _simGetUIProperty(luaWrap_lua_State* L);
int _simSetUIProperty(luaWrap_lua_State* L);
int _simGetUIEventButton(luaWrap_lua_State* L);
int _simGetUIButtonProperty(luaWrap_lua_State* L);
int _simSetUIButtonProperty(luaWrap_lua_State* L);
int _simGetUIButtonSize(luaWrap_lua_State* L);
int _simSetUIButtonLabel(luaWrap_lua_State* L);
int _simGetUIButtonLabel(luaWrap_lua_State* L);
int _simSetUISlider(luaWrap_lua_State* L);
int _simGetUISlider(luaWrap_lua_State* L);
int _simCreateUIButtonArray(luaWrap_lua_State* L);
int _simSetUIButtonArrayColor(luaWrap_lua_State* L);
int _simDeleteUIButtonArray(luaWrap_lua_State* L);
int _simCreateUI(luaWrap_lua_State* L);
int _simCreateUIButton(luaWrap_lua_State* L);
int _simLoadUI(luaWrap_lua_State* L);
int _simSaveUI(luaWrap_lua_State* L);
int _simRemoveUI(luaWrap_lua_State* L);
int _simSetUIButtonColor(luaWrap_lua_State* L);


int _simTest(luaWrap_lua_State* L);



struct SLuaCommands
{
    std::string name;
    luaWrap_lua_CFunction func;
    std::string callTip;
};
struct SLuaVariables
{
    std::string name;
    int val;
};

static SLuaCommands simLuaCommands[]= 
{
    // START_CALL_TIPS
    {"simGetScriptName",_simGetScriptName,                      "string scriptName=simGetScriptName(number scriptHandle)"},
    {"simGetObjectAssociatedWithScript",_simGetObjectAssociatedWithScript,"number objectHandle=simGetObjectAssociatedWithScript(number scriptHandle)"},
    {"simGetScriptAssociatedWithObject",_simGetScriptAssociatedWithObject,"number scriptHandle=simGetScriptAssociatedWithObject(number objectHandle)"},
    {"simGetCustomizationScriptAssociatedWithObject",_simGetCustomizationScriptAssociatedWithObject,"number scriptHandle=simGetCustomizationScriptAssociatedWithObject(number objectHandle)"},
    {"simGetScriptExecutionCount",_simGetScriptExecutionCount,  "number executionCount=simGetScriptExecutionCount()"},
    {"simIsScriptExecutionThreaded",_simIsScriptExecutionThreaded,"number result=simIsScriptExecutionThreaded()"},
    {"simIsScriptRunningInThread",_simIsScriptRunningInThread,  "number result=simIsScriptRunningInThread()"},
    {"simOpenModule",_simOpenModule,                            "number result=simOpenModule(sim_handle_all)\nnumber result=simOpenModule(string moduleName)"},
    {"simCloseModule",_simCloseModule,                          "number result=simCloseModule(sim_handle_all)\nnumber result=simCloseModule(string moduleName)"},
    {"simHandleModule",_simHandleModule,                        "number result=simHandleModule(sim_handle_all)\nnumber result=simHandleModule(string moduleName)"},
    {"simBoolOr16",_simBoolOr16,                                "number result=simBoolOr16(number value1,number value2)"},
    {"simBoolAnd16",_simBoolAnd16,                              "number result=simBoolAnd16(number value1,number value2)"},
    {"simBoolXor16",_simBoolXor16,                              "number result=simBoolXor16(number value1,number value2)"},
    {"simBoolOr32",_simBoolOr32,                                "number result=simBoolOr32(number value1,number value2)"},
    {"simBoolAnd32",_simBoolAnd32,                              "number result=simBoolAnd32(number value1,number value2)"},
    {"simBoolXor32",_simBoolXor32,                              "number result=simBoolXor32(number value1,number value2)"},
    {"simHandleDynamics",_simHandleDynamics,                    "number result=simHandleDynamics(number deltaTime)"},
    {"simHandleIkGroup",_simHandleIkGroup,                      "number performedIkGroupCalculationCount=simHandleIkGroup(number ikGroupHandle)"},
    {"simCheckIkGroup",_simCheckIkGroup,                        "number ikCalculationResult,table jointValues=simCheckIkGroup(number ikGroupHandle,table jointHandles,table jointOptions=nil)"},
    {"simHandleCollision",_simHandleCollision,                  "number collisionCount,table_2 collidingObjectHandles=simHandleCollision(number collisionObjectHandle)"},
    {"simReadCollision",_simReadCollision,                      "number collisionState,table_2 collidingObjectHandles=simReadCollision(number collisionObjectHandle)"},
    {"simHandleDistance",_simHandleDistance,                    "number result,number smallestDistance=simHandleDistance(number distanceObjectHandle)"},
    {"simReadDistance",_simReadDistance,                        "number result,number smallestDistance=simReadDistance(number distanceObjectHandle)"},
    {"simHandleProximitySensor",_simHandleProximitySensor,      "number result,number distance,table_3 detectedPoint,number detectedObjectHandle,table_3 normalVector=\nsimHandleProximitySensor(number sensorHandle)"},
    {"simReadProximitySensor",_simReadProximitySensor,          "number result,number distance,table_3 detectedPoint,number detectedObjectHandle,table_3 normalVector=\nsimReadProximitySensor(number sensorHandle)"},
    {"simHandleMill",_simHandleMill,                            "number cutCount,table_2 removedSurfaceAndVolume=simHandleMill(number millHandle)"},
    {"simResetCollision",_simResetCollision,                    "number result=simResetCollision(number collisionObjectHandle)"},
    {"simResetDistance",_simResetDistance,                      "number result=simResetDistance(number distanceObjectHandle)"},
    {"simResetProximitySensor",_simResetProximitySensor,        "number result=simResetProximitySensor(number objectHandle)"},
    {"simResetMill",_simResetMill,                              "number result=simResetMill(number objectHandle)"},
    {"simCheckProximitySensor",_simCheckProximitySensor,        "number result,number distance,table_3 detectedPoint=simCheckProximitySensor(number sensorHandle,number entityHandle)"},
    {"simCheckProximitySensorEx",_simCheckProximitySensorEx,    "number result,number distance,table_3 detectedPoint,number detectedObjectHandle,table_3 normalVector=\nsimCheckProximitySensorEx(number sensorHandle,number entityHandle,number mode,number threshold,number maxAngle)"},
    {"simCheckProximitySensorEx2",_simCheckProximitySensorEx2,  "number result,number distance,table_3 detectedPoint,table_3 normalVector=\nsimCheckProximitySensorEx2(number sensorHandle,table vertices,number itemType,number itemCount,number mode,number threshold,number maxAngle)"},
    {"simGetNameSuffix",_simGetNameSuffix,                      "number suffix,string name=simGetNameSuffix(string nameWithSuffix)"},
    {"simSetNameSuffix",_simSetNameSuffix,                      "number result=simSetNameSuffix(number nameSuffixNumber)"},
    {"simGetObjectHandle",_simGetObjectHandle,                  "number objectHandle=simGetObjectHandle(string objectName)"},
    {"simAddScript",_simAddScript,                              "number scriptHandle=simAddScript(number scriptType)"},
    {"simAssociateScriptWithObject",_simAssociateScriptWithObject,"number result=simAssociateScriptWithObject(number scriptHandle,number objectHandle)"},
    {"simSetScriptText",_simSetScriptText,                      "number result=simSetScriptText(number scriptHandle,string scriptText)"},
    {"simGetScriptHandle",_simGetScriptHandle,                  "number scriptHandle=simGetScriptHandle(string scriptName=nil)"},
    {"simGetCollectionHandle",_simGetCollectionHandle,          "number collectionHandle=simGetCollectionHandle(string collectionName)"},
    {"simRemoveCollection",_simRemoveCollection,                "number result=simRemoveCollection(number collectionHandle)"},
    {"simEmptyCollection",_simEmptyCollection,                  "number result=simEmptyCollection(number collectionHandle)"},
    {"simGetObjectPosition",_simGetObjectPosition,              "table_3 position=simGetObjectPosition(number objectHandle,number relativeToObjectHandle)"},
    {"simGetObjectOrientation",_simGetObjectOrientation,        "table_3 eulerAngles=simGetObjectOrientation(number objectHandle,number relativeToObjectHandle)"},
    {"simSetObjectPosition",_simSetObjectPosition,              "number result=simSetObjectPosition(number objectHandle,number relativeToObjectHandle,table_3 position)"},
    {"simSetObjectOrientation",_simSetObjectOrientation,        "number result=simSetObjectOrientation(number objectHandle,number relativeToObjectHandle,table_3 eulerAngles)"},
    {"simGetJointPosition",_simGetJointPosition,                "number position=simGetJointPosition(number objectHandle)"},
    {"simSetJointPosition",_simSetJointPosition,                "number result=simSetJointPosition(number objectHandle,number position)"},
    {"simSetJointTargetPosition",_simSetJointTargetPosition,    "number result=simSetJointTargetPosition(number objectHandle,number targetPosition)"},
    {"simGetJointTargetPosition",_simGetJointTargetPosition,    "number result,number targetPosition=simGetJointTargetPosition(number objectHandle)"},
    {"simSetJointForce",_simSetJointForce,                      "number result=simSetJointForce(number objectHandle,number forceOrTorque)"},
    {"simGetPathPosition",_simGetPathPosition,                  "number position=simGetPathPosition(number objectHandle)"},
    {"simSetPathPosition",_simSetPathPosition,                  "number result=simSetPathPosition(number objectHandle,number position)"},
    {"simGetPathLength",_simGetPathLength,                      "number length=simGetPathLength(number objectHandle)"},
    {"simSetJointTargetVelocity",_simSetJointTargetVelocity,    "number result=simSetJointTargetVelocity(number objectHandle,number targetVelocity)"},
    {"simGetJointTargetVelocity",_simGetJointTargetVelocity,    "number targetVelocity=simGetJointTargetVelocity(number objectHandle)"},
    {"simSetPathTargetNominalVelocity",_simSetPathTargetNominalVelocity,"number result=simSetPathTargetNominalVelocity(number objectHandle,number targetVelocity)"},
    {"simGetObjectName",_simGetObjectName,                      "string objectName=simGetObjectName(number objectHandle)"},
    {"simGetCollectionName",_simGetCollectionName,              "string collectionName=simGetCollectionName(number collectionHandle)"},
    {"simRemoveObject",_simRemoveObject,                        "number result=simRemoveObject(number objectHandle)"},
    {"simRemoveModel",_simRemoveModel,                          "number removedObjects=simRemoveModel(number objectHandle)"},
    {"simGetSimulationTime",_simGetSimulationTime,              "number simulationTime=simGetSimulationTime()"},
    {"simGetSimulationState",_simGetSimulationState,            "number simulationState=simGetSimulationState()"},
    {"simGetSystemTime",_simGetSystemTime,                      "number systemTime=simGetSystemTime()"},
    {"simGetSystemTimeInMs",_simGetSystemTimeInMs,              "number systemTimeOrTimeDiff=simGetSystemTimeInMs(number previousTime)"},
    {"simCheckCollision",_simCheckCollision,                    "number result=simCheckCollision(number entity1Handle,number entity2Handle)"},
    {"simCheckCollisionEx",_simCheckCollisionEx,                "number segmentCount,table segmentData=simCheckCollisionEx(number entity1Handle,number entity2Handle)"},
    {"simCheckDistance",_simCheckDistance,                      "number result,table_7 distanceData=simCheckDistance(number entity1Handle,number entity2Handle,number threshold)"},
    {"simGetObjectConfiguration",_simGetObjectConfiguration,    "number rawBufferHandle=simGetObjectConfiguration(number objectHandle)"},
    {"simSetObjectConfiguration",_simSetObjectConfiguration,    "number result=simSetObjectConfiguration(number rawBufferHandle)"},
    {"simGetConfigurationTree",_simGetConfigurationTree,        "number rawBufferHandle=simGetConfigurationTree(number objectHandle)"},
    {"simSetConfigurationTree",_simSetConfigurationTree,        "number result=simSetConfigurationTree(number rawBufferHandle)"},
    {"simHandleMechanism",_simHandleMechanism,                  "number result=simHandleMechanism(number mechanismHandle)"},
    {"simGetSimulationTimeStep",_simGetSimulationTimeStep,      "number timeStep=simGetSimulationTimeStep()"},
    {"simGetSimulatorMessage",_simGetSimulatorMessage,          "number messageID,table_4 auxiliaryData,table auxiliaryData2=simGetSimulatorMessage()"},
    {"simResetGraph",_simResetGraph,                            "number result=simResetGraph(number objectHandle)"},
    {"simHandleGraph",_simHandleGraph,                          "number result=simHandleGraph(number objectHandle,number simulationTime)"},
    {"simAddStatusbarMessage",_simAddStatusbarMessage,          "number result=simAddStatusbarMessage(string message)"},
    {"simGetLastError",_simGetLastError,                        "string lastError=simGetLastError()"},
    {"simGetObjects",_simGetObjects,                            "number objectHandle=simGetObjects(number index,number objectType)"},
    {"simRefreshDialogs",_simRefreshDialogs,                    "number result=simRefreshDialogs(number refreshDegree)"},
    {"simGetModuleName",_simGetModuleName,                      "string moduleName,number version=simGetModuleName(number index)"},
    {"simGetIkGroupHandle",_simGetIkGroupHandle,                "number ikGroupHandle=simGetIkGroupHandle(string ikGroupName)"},
    {"simRemoveScript",_simRemoveScript,                        "number result=simRemoveScript(number scriptHandle)"},
    {"simGetCollisionHandle",_simGetCollisionHandle,            "number collisionObjectHandle=simGetCollisionHandle(string collisionObjectName)"},
    {"simGetDistanceHandle",_simGetDistanceHandle,              "number distanceObjectHandle=simGetDistanceHandle(string distanceObjectName)"},
    {"simGetMechanismHandle",_simGetMechanismHandle,            "number mechanismHandle=simGetMechanismHandle(string mechanismName)"},
    {"simGetPathPlanningHandle",_simGetPathPlanningHandle,      "number pathPlanningObjectHandle=simGetPathPlanningHandle(string pathPlanningObjectName)"},
    {"simSearchPath",_simSearchPath,                            "number result=simSearchPath(number pathPlanningObjectHandle,number maximumSearchTime,number subTimeStep)"},
    {"simInitializePathSearch",_simInitializePathSearch,        "number temporaryPathSearchObjectHandle=simInitializePathSearch(number pathPlanningObjectHandle,\nnumber maximumSearchTime,number searchTimeStep)"},
    {"simPerformPathSearchStep",_simPerformPathSearchStep,      "number result=simPerformPathSearchStep(number temporaryPathSearchObjectHandle,boolean abortSearch)"},
    {"simGetScriptSimulationParameter",_simGetScriptSimulationParameter,"boolean/number/string parameterValue=simGetScriptSimulationParameter(number scriptHandle,string parameterName,boolean forceStringReturn=false)\ntable parameterValues,table scriptHandles=simGetScriptSimulationParameter(number targetScripts,string parameterName,boolean forceStringReturn=false)"},
    {"simSetScriptSimulationParameter",_simSetScriptSimulationParameter,"number setCount=simSetScriptSimulationParameter(number scriptHandle,string parameterName,string parameterValue)"},
    {"simDisplayDialog",_simDisplayDialog,                      "number genericDlgHandle,number UIHandle=simDisplayDialog(string title,string mainText,number style,boolean modal,\nstring initTxt,table_6 titleColors,table_6 dlgColors)"},
    {"simGetDialogResult",_simGetDialogResult,                  "number result=simGetDialogResult(number genericDlgHandle)"},
    {"simGetDialogInput",_simGetDialogInput,                    "string input=simGetDialogInput(number genericDlgHandle)"},
    {"simEndDialog",_simEndDialog,                              "number result=simEndDialog(number genericDlgHandle)"},
    {"simStopSimulation",_simStopSimulation,                    "number result=simStopSimulation()"},
    {"simPauseSimulation",_simPauseSimulation,                  "number result=simPauseSimulation()"},
    {"simStartSimulation",_simStartSimulation,                  "number result=simStartSimulation()"},
    {"simGetObjectMatrix",_simGetObjectMatrix,                  "table_12 matrix=simGetObjectMatrix(number objectHandle,number relativeToObjectHandle)"},
    {"simSetObjectMatrix",_simSetObjectMatrix,                  "number result=simSetObjectMatrix(number objectHandle,number relativeToObjectHandle,table_12 matrix)"},
    {"simGetJointMatrix",_simGetJointMatrix,                    "table_12 matrix=simGetJointMatrix(number objectHandle)"},
    {"simSetSphericalJointMatrix",_simSetSphericalJointMatrix,  "number result=simSetSphericalJointMatrix(number objectHandle,table_12 matrix"},
    {"simBuildIdentityMatrix",_simBuildIdentityMatrix,          "table_12 matrix=simBuildIdentityMatrix()"},
    {"simCopyMatrix",_simCopyMatrix,                            "table_12 matrix=simCopyMatrix(table_12 matrixToCopy)"},
    {"simBuildMatrix",_simBuildMatrix,                          "table_12 matrix=simBuildMatrix(table_3 position,table_3 eulerAngles)"},
    {"simGetEulerAnglesFromMatrix",_simGetEulerAnglesFromMatrix,"table_3 eulerAngles=simGetEulerAnglesFromMatrix(table_12 matrix)"},
    {"simInvertMatrix",_simInvertMatrix,                        "number result=simInvertMatrix(table_12 matrix)"},
    {"simMultiplyMatrices",_simMultiplyMatrices,                "table_12 resultMatrix=simMultiplyMatrices(table_12 matrixIn1,table_12 matrixIn2)"},
    {"simInterpolateMatrices",_simInterpolateMatrices,          "table_12 resultMatrix=simInterpolateMatrices(table_12 matrixIn1,table_12 matrixIn2,number interpolFactor)"},
    {"simMultiplyVector",_simMultiplyVector,                    "table_3 resultVector=simMultiplyVector(table_12 matrix,table_3 vector)"},
    {"simGetObjectChild",_simGetObjectChild,                    "number childObjectHandle=simGetObjectChild(number objectHandle,number index)"},
    {"simGetObjectParent",_simGetObjectParent,                  "number parentObjectHandle=simGetObjectParent(number objectHandle)"},
    {"simSetObjectParent",_simSetObjectParent,                  "number result=simSetObjectParent(number objectHandle,number parentObjectHandle,boolean keepInPlace)"},
    {"simGetObjectType",_simGetObjectType,                      "number objectType=simGetObjectType(number objectHandle)"},
    {"simGetJointType",_simGetJointType,                        "number jointType=simGetJointType(number objectHandle)"},
    {"simSetBoolParameter",_simSetBoolParameter,                "number result=simSetBoolParameter(number parameter,boolean boolState)"},
    {"simGetBoolParameter",_simGetBoolParameter,                "boolean boolState=simGetBoolParameter(number parameter)"},
    {"simSetInt32Parameter",_simSetInt32Parameter,              "number result=simSetInt32Parameter(number parameter,number intState)"},
    {"simGetInt32Parameter",_simGetInt32Parameter,              "number intState=simGetInt32Parameter(number parameter)"},
    {"simSetFloatParameter",_simSetFloatParameter,              "number result=simSetFloatParameter(number parameter,number floatState)"},
    {"simGetFloatParameter",_simGetFloatParameter,              "number floatState=simGetFloatParameter(number parameter)"},
    {"simSetStringParameter",_simSetStringParameter,            "number result=simSetStringParameter(number parameter,string stringState)"},
    {"simGetStringParameter",_simGetStringParameter,            "string stringState=simGetStringParameter(number parameter)"},
    {"simSetArrayParameter",_simSetArrayParameter,              "number result=simSetArrayParameter(number parameter,table arrayOfValues)"},
    {"simGetArrayParameter",_simGetArrayParameter,              "table arrayOfValues=simGetArrayParameter(number parameter)"},
    {"simSetObjectName",_simSetObjectName,                      "number result=simSetObjectName(number objectHandle,string objectName)"},
    {"simSetCollectionName",_simSetCollectionName,              "number result=simSetCollectionName(number collectionHandle,string collectionName)"},
    {"simGetJointInterval",_simGetJointInterval,                "boolean cyclic,table_2 interval=simGetJointInterval(number objectHandle)"},
    {"simSetJointInterval",_simSetJointInterval,                "number result=simSetJointInterval(number objectHandle,boolean cyclic,table_2 interval)"},
    {"simLoadScene",_simLoadScene,                              "number result=simLoadScene(string filename)"},
    {"simSaveScene",_simSaveScene,                              "number result=simSaveScene(string filename)"},
    {"simLoadModel",_simLoadModel,                              "number objectHandle=simLoadModel(string filename)"},
    {"simSaveModel",_simSaveModel,                              "number result=simSaveModel(number modelBaseHandle,string filename)"},
    {"simIsObjectInSelection",_simIsObjectInSelection,          "number selectionState=simIsObjectInSelection(number objectHandle)"},
    {"simAddObjectToSelection",_simAddObjectToSelection,        "number result=simAddObjectToSelection(number what,number objectHandle)\nnumber result=simAddObjectToSelection(table objectHandles)"},
    {"simRemoveObjectFromSelection",_simRemoveObjectFromSelection,"number result=simRemoveObjectFromSelection(number what,number objectHandle)\nnumber result=simRemoveObjectFromSelection(table objectHandles)"},
    {"simGetObjectSelectionSize",_simGetObjectSelectionSize,    "number selectionSize=simGetObjectSelectionSize()"},
    {"simGetObjectLastSelection",_simGetObjectLastSelection,    "number objectHandle=simGetObjectLastSelection()"},
    {"simGetObjectSelection",_simGetObjectSelection,            "table selectedObjectHandles=simGetObjectSelection()"},
    {"simGetRealTimeSimulation",_simGetRealTimeSimulation,      "number result=simGetRealTimeSimulation()"},
    {"simSetNavigationMode",_simSetNavigationMode,              "number result=simSetNavigationMode(number navigationMode)"},
    {"simGetNavigationMode",_simGetNavigationMode,              "number navigationMode=simGetNavigationMode()"},
    {"simSetPage",_simSetPage,                                  "number result=simSetPage(number pageIndex)"},
    {"simGetPage",_simGetPage,                                  "number pageIndex=simGetPage()"},
    {"simReleaseScriptRawBuffer",_simReleaseScriptRawBuffer,    "number result=simReleaseScriptRawBuffer(number scriptHandle,number bufferHandle)"},
    {"simCopyPasteObjects",_simCopyPasteObjects,                "table copiedObjectHandles=simCopyPasteObjects(table objectHandles,number options)"},
    {"simScaleObjects",_simScaleObjects,                        "number result=simScaleObjects(table objectHandles,number scalingFactor,boolean scalePositionsToo)"},
    {"simGetObjectUniqueIdentifier",_simGetObjectUniqueIdentifier,"number uniqueIdentifier=simGetObjectUniqueIdentifier(number objectHandle)\ntable uniqueIdentifiers=simGetObjectUniqueIdentifier(sim_handle_all)"},
    {"simSetThreadAutomaticSwitch",_simSetThreadAutomaticSwitch,"number result=simSetThreadAutomaticSwitch(boolean automaticSwitch)"},
    {"simGetThreadAutomaticSwitch",_simGetThreadAutomaticSwitch,"boolean result=simGetThreadAutomaticSwitch()"},
    {"simSetThreadSwitchTiming",_simSetThreadSwitchTiming,      "number result=simSetThreadSwitchTiming(number deltaTimeInMilliseconds)"},
    {"simSetThreadResumeLocation",_simSetThreadResumeLocation,  "number result=simSetThreadResumeLocation(number location,number order)"},
    {"simResumeThreads",_simResumeThreads,                      "number count=simResumeThreads(number location)"},
    {"simSwitchThread",_simSwitchThread,                        "number result=simSwitchThread()"},
    {"simCreateIkGroup",_simCreateIkGroup,                      "number ikGroupHandle=simCreateIkGroup(number options,table intParams=nil,table floatParams=nil)"},
    {"simRemoveIkGroup",_simRemoveIkGroup,                      "number result=simRemoveIkGroup(number ikGroupHandle)"},
    {"simCreateIkElement",_simCreateIkElement,                  "number result=simCreateIkElement(number ikGroupHandle,number options,table intParams,table floatParams=nil)"},
    {"simCreateMotionPlanning",_simCreateMotionPlanning,        "number motionPlanningHandle=simCreateMotionPlanning(table jointHandles,table jointRangeSubdivisions=nil,\ntable jointMetricWeights=nil,number options,table intParams,table floatParams=nil)"},
    {"simRemoveMotionPlanning",_simRemoveMotionPlanning,        "number result=simRemoveMotionPlanning(number motionPlanningHandle)"},
    {"simCreateCollection",_simCreateCollection,                "number collectionHandle=simCreateCollection(string collectionName,number options)"},
    {"simAddObjectToCollection",_simAddObjectToCollection,      "number result=simAddObjectToCollection(number collectionHandle,number objectHandle,number what,number options)"},
    {"simSaveImage",_simSaveImage,                              "number result=simSaveImage(string image,table_2 resolution,number options,string filename,number quality)"},
    {"simLoadImage",_simLoadImage,                              "string image,table_2 resolution=simLoadImage(number options,string filename)"},
    {"simGetScaledImage",_simGetScaledImage,                    "string imageOut,table_2 effectiveResolutionOut=simGetScaledImage(string imageIn,table_2 resolutionIn,\ntable_2 desiredResolutionOut,number options)"},
    {"simTransformImage",_simTransformImage,                    "number result=simTransformImage(string image,table_2 resolution,number options)"},
    {"simGetQHull",_simGetQHull,                                "table verticesOut,table indicesOut=simGetQHull(table verticesIn)"},
    {"simGetDecimatedMesh",_simGetDecimatedMesh,                "table verticesOut,table indicesOut=simGetDecimatedMesh(table verticesIn,table indicesIn,number decimationPercentage)"},
    {"simExportIk",_simExportIk,                                "number result=simExportIk(string pathAndFilename)"},
    {"simComputeJacobian",_simComputeJacobian,                  "number result=simComputeJacobian(number ikGroupHandle,number options)"},
    {"simSendData",_simSendData,                                "number result=simSendData(number targetID,number dataHeader,string dataName,string data,number antennaHandle=sim_handle_self,\nnumber actionRadius=100,number emissionAngle1=3.1415,number emissionAngle2=6.283,number persistence=0)"},
    {"simReceiveData",_simReceiveData,                          "string data,number senderID,number dataHeader,string dataName=simReceiveData(number dataHeader=-1,string dataName=nil,\nnumber antennaHandle=sim_handle_self,number index=-1)"},

    {"simPackInt32Table",_simPackInt32Table,                    "string data=simPackInt32Table(table int32Numbers,number startInt32Index=0,number int32Count=0)"},
    {"simPackUInt32Table",_simPackUInt32Table,                  "string data=simPackUInt32Table(table uint32Numbers,number startUInt32Index=0,number uint32Count=0)"},
    {"simPackFloatTable",_simPackFloatTable,                    "string data=simPackFloatTable(table floatNumbers,number startFloatIndex=0,number floatCount=0)"},
    {"simPackDoubleTable",_simPackDoubleTable,                  "string data=simPackDoubleTable(table doubleNumbers,number startDoubleIndex=0,number doubleCount=0)"},
    {"simPackUInt8Table",_simPackUInt8Table,                    "string data=simPackUInt8Table(table uint8Numbers,number startUint8Index=0,number uint8count=0)"},
    {"simPackUInt16Table",_simPackUInt16Table,                  "string data=simPackUInt16Table(table uint16Numbers,number startUint16Index=0,number uint16Count=0)"},
    {"simUnpackInt32Table",_simUnpackInt32Table,                "table int32Numbers=simUnpackInt32Table(string data,number startInt32Index=0,number int32Count=0,number additionalByteOffset=0)"},
    {"simUnpackUInt32Table",_simUnpackUInt32Table,              "table uint32Numbers=simUnpackUInt32Table(string data,number startUint32Index=0,number uint32Count=0,number additionalByteOffset=0)"},
    {"simUnpackFloatTable",_simUnpackFloatTable,                "table floatNumbers=simUnpackFloatTable(string data,number startFloatIndex=0,number floatCount=0,number additionalByteOffset=0)"},
    {"simUnpackDoubleTable",_simUnpackDoubleTable,              "table doubleNumbers=simUnpackDoubleTable(string data,number startDoubleIndex=0,number doubleCount=0,number additionalByteOffset=0)"},
    {"simUnpackUInt8Table",_simUnpackUInt8Table,                "table uint8Numbers=simUnpackUInt8Table(string data,number startUint8Index=0,number uint8count=0)"},
    {"simUnpackUInt16Table",_simUnpackUInt16Table,              "table uint16Numbers=simUnpackUInt16Table(string data,number startUint16Index=0,number uint16Count=0,number additionalByteOffset=0)"},

    {"simPackTable",_simPackTable,                              "string buffer=simPackTable(table aTable)"},
    {"simUnpackTable",_simUnpackTable,                          "table aTable=simUnpackTable(string buffer)"},
    {"simTransformBuffer",_simTransformBuffer,                  "string outBuffer=simTransformBuffer(string inBuffer,number inFormat,number multiplier,number offset,number outFormat)"},
    {"simCombineRgbImages",_simCombineRgbImages,                "string outImg=simCombineRgbImages(string img1,table_2 img1Res,string img2,table_2 img2Res,number operation)"},

    {"simGetVelocity",_simGetVelocity,                          "table_3 linearVelocity,table_3 angularVelocity=simGetVelocity(number shapeHandle)"},
    {"simGetObjectVelocity",_simGetObjectVelocity,              "table_3 linearVelocity,table_3 angularVelocity=simGetObjectVelocity(number objectHandle)"},
    {"simAddForceAndTorque",_simAddForceAndTorque,              "number result=simAddForceAndTorque(number shapeHandle,table_3 force=nil,table_3 torque=nil)"},
    {"simAddForce",_simAddForce,                                "number result=simAddForce(number shapeHandle,table_3 position,table_3 force)"},
    {"simSetExplicitHandling",_simSetExplicitHandling,          "number result=simSetExplicitHandling(number generalObjectHandle,number explicitHandlingFlags)"},
    {"simGetExplicitHandling",_simGetExplicitHandling,          "number explicitHandlingFlags=simGetExplicitHandling(number generalObjectHandle)"},
    {"simSetGraphUserData",_simSetGraphUserData,                "number result=simSetGraphUserData(number graphHandle,string dataStreamName,number data)"},
    {"simAddDrawingObject",_simAddDrawingObject,                "number drawingObjectHandle=simAddDrawingObject(number objectType,number size,number duplicateTolerance,\nnumber parentObjectHandle,number maxItemCount,table_3 ambient_diffuse=nil,nil,table_3 specular=nil,\ntable_3 emission=nil)"},
    {"simRemoveDrawingObject",_simRemoveDrawingObject,          "number result=simRemoveDrawingObject(number drawingObjectHandle)"},
    {"simAddDrawingObjectItem",_simAddDrawingObjectItem,        "number result=simAddDrawingObjectItem(number drawingObjectHandle,table itemData)"},
    {"simAddParticleObject",_simAddParticleObject,              "number particleObjectHandle=simAddParticleObject(number objectType,number size,number density,table params,number lifeTime,\nnumber maxItemCount,table_3 ambient_diffuse=nil,nil,table_3 specular=nil,table_3 emission=nil)"},
    {"simRemoveParticleObject",_simRemoveParticleObject,        "number result=simRemoveParticleObject(number particleObjectHandle)"},
    {"simAddParticleObjectItem",_simAddParticleObjectItem,      "number result=simAddParticleObjectItem(number particleObjectHandle,table itemData)"},
    {"simGetObjectSizeFactor",_simGetObjectSizeFactor,          "number sizeFactor=simGetObjectSizeFactor(number ObjectHandle)"},
    {"simResetMilling",_simResetMilling,                        "number result=simResetMilling(number objectHandle)"},
    {"simApplyMilling",_simApplyMilling,                        "number result=simApplyMilling(number objectHandle)"},
    {"simSetIntegerSignal",_simSetIntegerSignal,                "number result=simSetIntegerSignal(string signalName,number signalValue)"},
    {"simGetIntegerSignal",_simGetIntegerSignal,                "number signalValue=simGetIntegerSignal(string signalName)"},
    {"simClearIntegerSignal",_simClearIntegerSignal,            "number clearCount=simClearIntegerSignal(string signalName)"},
    {"simSetFloatSignal",_simSetFloatSignal,                    "number result=simSetFloatSignal(string signalName,number signalValue)"},
    {"simGetFloatSignal",_simGetFloatSignal,                    "number signalValue=simGetFloatSignal(string signalName)"},
    {"simClearFloatSignal",_simClearFloatSignal,                "number clearCount=simClearFloatSignal(string signalName)"},
    {"simSetStringSignal",_simSetStringSignal,                  "number result=simSetStringSignal(string signalName,string signalValue)"},
    {"simGetStringSignal",_simGetStringSignal,                  "string signalValue=simGetStringSignal(string signalName)"},
    {"simClearStringSignal",_simClearStringSignal,              "number clearCount=simClearStringSignal(string signalName)"},
    {"simGetSignalName",_simGetSignalName,                      "string signalName=simGetSignalName(number signalIndex,number signalType)"},
    {"simWaitForSignal",_simWaitForSignal,                      "number/string signalValue=simWaitForSignal(string signalName)"},
    {"simPersistentDataWrite",_simPersistentDataWrite,          "number result=simPersistentDataWrite(string dataName,string dataValue,number options=0)"},
    {"simPersistentDataRead",_simPersistentDataRead,            "string dataValue=simPersistentDataRead(string dataName)"},
    {"simSetObjectProperty",_simSetObjectProperty,              "number result=simSetObjectProperty(number objectHandle,number property)"},
    {"simGetObjectProperty",_simGetObjectProperty,              "number property=simGetObjectProperty(number objectHandle)"},
    {"simSetObjectSpecialProperty",_simSetObjectSpecialProperty,"number result=simSetObjectSpecialProperty(number objectHandle,number property)"},
    {"simGetObjectSpecialProperty",_simGetObjectSpecialProperty,"number property=simGetObjectSpecialProperty(number objectHandle)"},
    {"simSetModelProperty",_simSetModelProperty,                "number result=simSetModelProperty(number objectHandle,number property)"},
    {"simGetModelProperty",_simGetModelProperty,                "number property=simGetModelProperty(number objectHandle)"},
    {"simMoveToPosition",_simMoveToPosition,                    "number deltaTimeLeft=simMoveToPosition(number objectHandle,number relativeToObjectHandle,table_3 position,table_3 orientation,\nnumber velocity,number accel,table_2 distCalcMethod)"},
    {"simMoveToObject",_simMoveToObject,                        "number deltaTimeLeft=simMoveToObject(number objectHandle,number targetObjectHandle,number positionAndOrOrientation,\nnumber relativeDistanceOnPath,number velocity,number accel)"},
    {"simFollowPath",_simFollowPath,                            "number deltaTimeLeft=simFollowPath(number objectHandle,number pathHandle,number positionAndOrOrientation,\nnumber relativeDistanceOnPath,number velocity,number accel)"},
    {"simMoveToJointPositions",_simMoveToJointPositions,        "number deltaTimeLeft=simMoveToJointPositions(table jointHandles,table jointPositions,number/table velocity,\nnumber/table accel,number angleToLinearCoeff)"},
    {"simWait",_simWait,                                        "number deltaTimeLeft=simWait(number deltaTime,boolean simulationTime=true)"},
    {"simGetDataOnPath",_simGetDataOnPath,                      "number auxFlags,table_4 auxChannels=simGetDataOnPath(number pathHandle,number relativeDistance)"},
    {"simGetPositionOnPath",_simGetPositionOnPath,              "table_3 position=simGetPositionOnPath(number pathHandle,number relativeDistance)"},
    {"simGetOrientationOnPath",_simGetOrientationOnPath,        "table_3 eulerAngles=simGetOrientationOnPath(number pathHandle,number relativeDistance)"},
    {"simGetClosestPositionOnPath",_simGetClosestPositionOnPath,"number positionOnPath=simGetClosestPositionOnPath(number pathHandle,table_3 positionRelativeToPathFrame)"},
    {"simReadForceSensor",_simReadForceSensor,                  "number result,table_3 forceVector,table_3 torqueVector=simReadForceSensor(number objectHandle)"},
    {"simBreakForceSensor",_simBreakForceSensor,                "number result=simBreakForceSensor(number objectHandle)"},
    {"simGetLightParameters",_simGetLightParameters,            "number state,table_3 zero,table_3 diffusePart,table_3 specular=simGetLightParameters(number lightHandle)"},
    {"simSetLightParameters",_simSetLightParameters,            "number result=simSetLightParameters(number lightHandle,number state,nil,table_3 diffusePart,table_3 specularPart)"},
    {"simGetLinkDummy",_simGetLinkDummy,                        "number linkDummyHandle=simGetLinkDummy(number dummyHandle)"},
    {"simSetLinkDummy",_simSetLinkDummy,                        "number result=simSetLinkDummy(number dummyHandle,number linkDummyHandle)"},
    {"simSetShapeColor",_simSetShapeColor,                      "number result=simSetShapeColor(number shapeHandle,string colorName,number colorComponent,table_3 rgbData)"},
    {"simGetShapeColor",_simGetShapeColor,                      "number result,table_3 rgbData=simGetShapeColor(number shapeHandle,string colorName,number colorComponent)"},
    {"simResetDynamicObject",_simResetDynamicObject,            "number result=simResetDynamicObject(number objectHandle)"},
    {"simSetJointMode",_simSetJointMode,                        "number result=simSetJointMode(number jointHandle,number jointMode,number options)"},
    {"simGetJointMode",_simGetJointMode,                        "number jointMode,number options=simGetJointMode(number jointHandle)"},
    {"simSerialOpen",_simSerialOpen,                            "number portHandle=simSerialOpen(string portString,number baudRate)"},
    {"simSerialClose",_simSerialClose,                          "number result=simSerialClose(number portHandle)"},
    {"simSerialSend",_simSerialSend,                            "number charsSent=simSerialSend(number portHandle,string data)"},
    {"simSerialRead",_simSerialRead,                            "string data=simSerialRead(number portHandle,number dataLengthToRead,boolean blockingOperation,\nstring closingString='',number timeout=0)"},
    {"simSerialCheck",_simSerialCheck,                          "number byteCount=simSerialCheck(number portHandle)"},
    {"simGetContactInfo",_simGetContactInfo,                    "table_2 collidingObjects,table_3 collisionPoint,table_3 reactionForce,table_3 normalVector=simGetContactInfo(number dynamicPass,\nnumber objectHandle,number index)"},
    {"simSetThreadIsFree",_simSetThreadIsFree,                  "number result=simSetThreadIsFree(boolean freeMode)"},
    {"simTubeOpen",_simTubeOpen,                                "number tubeHandle=simTubeOpen(number dataHeader,string dataName,number readBufferSize)"},
    {"simTubeClose",_simTubeClose,                              "number result=simTubeClose(number tubeHandle)"},
    {"simTubeWrite",_simTubeWrite,                              "number result=simTubeWrite(number tubeHandle,string data)"},
    {"simTubeRead",_simTubeRead,                                "string data=simTubeRead(number tubeHandle,boolean blockingOperation=false)"},
    {"simTubeStatus",_simTubeStatus,                            "number status,number readPacketsCount,number writePacketsCount=simTubeStatus(number tubeHandle)"},
    {"simAuxiliaryConsoleOpen",_simAuxiliaryConsoleOpen,        "number consoleHandle=simAuxiliaryConsoleOpen(string title,number maxLines,number mode,table_2 position=nil,table_2 size=nil,\ntable_3 textColor=nil,table_3 backgroundColor=nil)"},
    {"simAuxiliaryConsoleClose",_simAuxiliaryConsoleClose,      "number result=simAuxiliaryConsoleClose(number consoleHandle)"},
    {"simAuxiliaryConsolePrint",_simAuxiliaryConsolePrint,      "number result=simAuxiliaryConsolePrint(number consoleHandle,string text)"},
    {"simAuxiliaryConsoleShow",_simAuxiliaryConsoleShow,        "number result=simAuxiliaryConsoleShow(number consoleHandle,Boolean showState)"},
    {"simImportShape",_simImportShape,                          "number shapeHandle=simImportShape(number fileformat,string pathAndFilename,number options,number identicalVerticeTolerance\n,number scalingFactor)"},
    {"simImportMesh",_simImportMesh,                            "table_of_table vertices,table_of_table indices,nil,table names=simImportMesh(number fileformat,string pathAndFilename,\nnumber options,number identicalVerticeTolerance,number scalingFactor)"},
    {"simExportMesh",_simExportMesh,                            "number result=simExportMesh(number fileformat,string pathAndFilename,number options,number scalingFactor,\ntable_of_table vertices,table_of_table indices,nil,table names)"},
    {"simCreateMeshShape",_simCreateMeshShape,                  "number objectHandle=simCreateMeshShape(number options,number shadingAngle,table vertices,table indices)"},
    {"simGetShapeMesh",_simGetShapeMesh,                        "table vertices,table indices,table normals=simGetShapeMesh(number shapeHandle)"},
    {"simCreatePureShape",_simCreatePureShape,                  "number objectHandle=simCreatePureShape(number primitiveType,number options,table_3 sizes,number mass,table_2 precision=nil)"},
    {"simCreateHeightfieldShape",_simCreateHeightfieldShape,    "number objectHandle=simCreateHeightfieldShape(number options,number shadingAngle,number xPointCount,\nnumber yPointCount,number xSize,table heights)"},
    {"simAddBanner",_simAddBanner,                              "number bannerID=simAddBanner(string label,number size,number options,table_6 positionAndEulerAngles=nil,\nnumber parentObjectHandle=nil,table_12 labelColors=nil,table_12 backgroundColors=nil)"},
    {"simRemoveBanner",_simRemoveBanner,                        "number result=simRemoveBanner(number bannerID)"},
    {"simCreateJoint",_simCreateJoint,                          "number jointHandle=simCreateJoint(number jointType,number jointMode,number options,table_2 sizes=nil,\ntable_12 colorA=nil,table_12 colorB=nil)"},
    {"simCreateDummy",_simCreateDummy,                          "number dummyHandle=simCreateDummy(number size,table_12 color=nil)"},
    {"simCreateProximitySensor",_simCreateProximitySensor,      "number sensorHandle=simCreateProximitySensor(number sensorType,number subType,number options,table_8 intParams,\ntable_15 floatParams,table_48 color=nil)"},
    {"simCreatePath",_simCreatePath,                            "number pathHandle=simCreatePath(number attributes,table_3 intParams=nil,table_3 floatParams=nil,table_12 color=nil)"},
    {"simCreateForceSensor",_simCreateForceSensor,              "number sensorHandle=simCreateForceSensor(number options,table_5 intParams,table_5 floatParams,table_24 color=nil)"},
    {"simCreateVisionSensor",_simCreateVisionSensor,            "number sensorHandle=simCreateVisionSensor(number options,table_4 intParams,table_11 floatParams,table_48 color=nil)"},
    {"simInsertPathCtrlPoints",_simInsertPathCtrlPoints,        "number result=simInsertPathCtrlPoints(number pathHandle,number options,number startIndex,number ptCnt,table ptData)"},
    {"simCutPathCtrlPoints",_simCutPathCtrlPoints,              "number result=simCutPathCtrlPoints(number pathHandle,number startIndex,number ptCnt)"},
    {"simGetIkGroupMatrix",_simGetIkGroupMatrix,                "table matrix,table_2 matrixSize=simGetIkGroupMatrix(number ikGroupHandle,number options)"},
    {"simFloatingViewAdd",_simFloatingViewAdd,                  "number floatingViewHandle=simFloatingViewAdd(number posX,number posY,number sizeX,number sizeY,number options)"},
    {"simFloatingViewRemove",_simFloatingViewRemove,            "number result=simFloatingViewRemove(number floatingViewHandle)"},
    {"simAdjustView",_simAdjustView,                            "number result=simAdjustView(number viewHandleOrIndex,number associatedViewableObjectHandle,number options,string viewLabel=nil)"},
    {"simCameraFitToView",_simCameraFitToView,                  "number result=simCameraFitToView(number viewHandleOrIndex,table objectHandles=nil,simInt options=0,simFloat scaling=1)"},
    {"simAnnounceSceneContentChange",_simAnnounceSceneContentChange,"number result=simAnnounceSceneContentChange()"},
    {"simGetObjectInt32Parameter",_simGetObjectInt32Parameter,  "number result,number parameter=simGetObjectInt32Parameter(number objectHandle,number parameterID)"},
    {"simSetObjectInt32Parameter",_simSetObjectInt32Parameter,  "number result=simSetObjectInt32Parameter(number objectHandle,number parameterID,number parameter)"},
    {"simGetObjectFloatParameter",_simGetObjectFloatParameter,  "number result,number parameter=simGetObjectFloatParameter(number objectHandle,number parameterID)"},
    {"simSetObjectFloatParameter",_simSetObjectFloatParameter,  "number result=simSetObjectFloatParameter(number objectHandle,number parameterID,number parameter)"},
    {"simGetObjectStringParameter",_simGetObjectStringParameter,"string parameter=simGetObjectStringParameter(number objectHandle,number parameterID)"},
    {"simSetObjectStringParameter",_simSetObjectStringParameter,"number result=simSetObjectStringParameter(number objectHandle,number parameterID,string parameter)"},
    {"simGetRotationAxis",_simGetRotationAxis,                  "table_3 axis,number angle=simGetRotationAxis(table_12 matrixStart,table_12 matrixGoal)"},
    {"simRotateAroundAxis",_simRotateAroundAxis,                "table_12 matrixOut=simRotateAroundAxis(table_12 matrixIn,table_3 axis,table_3 axisPos,number angle)"},
    {"simLaunchExecutable",_simLaunchExecutable,                "number result=simLaunchExecutable(string filename,string parameters='',number showStatus=1)"},
    {"simGetJointForce",_simGetJointForce,                      "number forceOrTorque=simGetJointForce(number jointHandle)"},
    {"simSetIkGroupProperties",_simSetIkGroupProperties,        "number result=simSetIkGroupProperties(number ikGroupHandle,number resolutionMethod,number maxIterations,number damping)"},
    {"simSetIkElementProperties",_simSetIkElementProperties,    "number result=simSetIkElementProperties(number ikGroupHandle,number tipDummyHandle,number constraints,\ntable_2 precision=nil,table_2 weight=nil)"},
    {"simIsHandleValid",_simIsHandleValid,                      "number result=simIsHandleValid(number generalObjectHandle,number generalObjectType=-1)"},
    {"simGetObjectQuaternion",_simGetObjectQuaternion,          "table_4 quaternion=simGetObjectQuaternion(number objectHandle,number relativeToObjectHandle)"},
    {"simSetObjectQuaternion",_simSetObjectQuaternion,          "number result=simSetObjectQuaternion(number objectHandle,number relativeToObjectHandle,table_4 quaternion)"},
    {"simSetShapeMassAndInertia",_simSetShapeMassAndInertia,    "number result=simSetShapeMassAndInertia(number shapeHandle,number mass,table_9 inertiaMatrix,table_3 centerOfMass,\ntable_12 transformation=nil)"},
    {"simGetShapeMassAndInertia",_simGetShapeMassAndInertia,    "number mass,table_9 inertiaMatrix,table_3 centerOfMass=simGetShapeMassAndInertia(number shapeHandle,table_12 transformation=nil)"},
    {"simGroupShapes",_simGroupShapes,                          "number shapeHandle=simGroupShapes(table shapeHandles)"},
    {"simUngroupShape",_simUngroupShape,                        "table simpleShapeHandles=simUngroupShape(number shapeHandle)"},
    {"simConvexDecompose",_simConvexDecompose,                  "number shapeHandle=simConvexDecompose(number shapeHandle,number options,table_4 intParams,table_3 floatParams)"},
    {"simGetMotionPlanningHandle",_simGetMotionPlanningHandle,  "number motionPlanningObjectHandle=simGetMotionPlanningHandle(string motionPlanningObjectName)"},
    {"simFindMpPath",_simFindMpPath,                            "table path,table confSpaceLengths,table_3 tipPositions,table_4 tipQuaternions,table cartesianSpaceLengths=simFindMpPath(\nnumber motionPlanningObjectHandle,table startConfig,table goalConfig,number options,number stepSize,\nnumber maxTimeInMs=0,table auxIntParams=nil,table auxFloatParams=nil)"},
    {"simSimplifyMpPath",_simSimplifyMpPath,                    "table path,table confSpaceLengths,table_3 tipPositions,table_4 tipQuaternions,table cartesianSpaceLengths=simSimplifyMpPath(\nnumber motionPlanningObjectHandle,table path,number options,number stepSize,\nnumber increment=1,number maxTimeInMs=0,table auxIntParams=nil,table auxFloatParams=nil)"},
    {"simGetMpConfigTransition",_simGetMpConfigTransition,      "table path,table confSpaceLengths,table_3 tipPositions,table_4 tipQuaternions,table cartesianSpaceLengths=simGetMpConfigTransition(\nnumber motionPlanningObjectHandle,table startConfig,table goalConfig,number options,table select,number calcStepSize,\nnumber maxOutStepSize,table wayPoints=nil)"},
    {"simAddGhost",_simAddGhost,                                "number ghostId=simAddGhost(number ghostGroup,number objectHandle,number options,number startTime,number endTime,table_12 color=nil)"},
    {"simModifyGhost",_simModifyGhost,                          "number result=simModifyGhost(number ghostGroup,number ghostId,number operation,number floatValue,number options=nil,\nnumber optionsMask=nil,table colorOrTransformation=nil)"},
    {"simQuitSimulator",_simQuitSimulator,                      "simQuitSimulator(boolean doNotDisplayMessages)"},
    {"simGetThreadId",_simGetThreadId,                          "number threadId=simGetThreadId()"},
    {"simSetShapeMaterial",_simSetShapeMaterial,                "number result=simSetShapeMaterial(number shapeHandle,number materialIdOrShapeHandle)"},
    {"simGetTextureId",_simGetTextureId,                        "number textureId,table_2 resolution=simGetTextureId(string textureName)"},
    {"simReadTexture",_simReadTexture,                          "string textureData=simReadTexture(number textureId,number options,number posX=0,number posY=0,number sizeX=0,number sizeY=0)"},
    {"simWriteTexture",_simWriteTexture,                        "number result=simWriteTexture(number textureId,number options,string textureData,number posX=0,number posY=0,number sizeX=0,\nnumber sizeY=0,number interpol=0)"},
    {"simCreateTexture",_simCreateTexture,                      "number shapeHandle,number textureId,table_2 resolution=simCreateTexture(string fileName,number options,table_2 planeSizes=nil,\ntable_2 scalingUV=nil,table_2 xy_g=nil,number fixedResolution=0,table_2 resolution=nil}"},
    {"simWriteCustomDataBlock",_simWriteCustomDataBlock,        "number result=simWriteCustomDataBlock(number objectHandle,string tagName,string data)"},
    {"simReadCustomDataBlock",_simReadCustomDataBlock,          "string data=simReadCustomDataBlock(number objectHandle,string tagName)"},
    {"simReadCustomDataBlockTags",_simReadCustomDataBlockTags,  "table tags=simReadCustomDataBlockTags(number objectHandle)"},
    {"simAddPointCloud",_simAddPointCloud,                      "number pointCloudHandle=simAddPointCloud(number pageMask,number layerMask,number objectHandle,number options,number pointSize,\ntable pointCoordinates,table_12 defaultColors=nil,table pointColors=nil,table pointNormals=nil)"},
    {"simModifyPointCloud",_simModifyPointCloud,                "number result=simModifyPointCloud(number pointCloudHandle,number operation)"},
    {"simGetShapeGeomInfo",_simGetShapeGeomInfo,                "number result,number pureType,table_4 dimensions=simGetShapeGeomInfo(number shapeHandle)"},
    {"simGetObjectsInTree",_simGetObjectsInTree,                "table objects=simGetObjectsInTree(number treeBaseHandle,number objectType=sim_handle_all,number options=0)"},
    {"simSetObjectSizeValues",_simSetObjectSizeValues,          "number result=simSetObjectSizeValues(number objectHandle,table_3 sizeValues)"},
    {"simGetObjectSizeValues",_simGetObjectSizeValues,          "table_3 sizeValues=simGetObjectSizeValues(number objectHandle)"},
    {"simScaleObject",_simScaleObject,                          "number result=simScaleObject(number objectHandle,number xScale,number yScale,number zScale,number options=0)"},
    {"simSetShapeTexture",_simSetShapeTexture,                  "number result=simSetShapeTexture(number shapeHandle,number textureId,number mappingMode,number options,table_2 uvScaling,\ntable_3 position=nil,table_3 orientation=nil)"},
    {"simGetShapeTextureId",_simGetShapeTextureId,              "number textureId=simGetShapeTextureId(number shapeHandle)"},
    {"simGetCollectionObjects",_simGetCollectionObjects,        "table objectHandles=simGetCollectionObjects(number collectionHandle)"},
    {"simHandleCustomizationScripts",_simHandleCustomizationScripts,"number count=simHandleCustomizationScripts(number callType)"},
    {"simSetScriptAttribute",_simSetScriptAttribute,            "number result=simSetScriptAttribute(number scriptHandle,number attributeID,number/boolean attribute)"},
    {"simGetScriptAttribute",_simGetScriptAttribute,            "number/boolean attribute=simGetScriptAttribute(number scriptHandle,number attributeID)"},
    {"simHandleChildScripts",_simHandleChildScripts,            "number executedScriptCount=simHandleChildScripts(number callType,...<objects to be passed>)"},
    {"simLaunchThreadedChildScripts",_simLaunchThreadedChildScripts,"number launchCount=simLaunchThreadedChildScripts()"},
    {"simReorientShapeBoundingBox",_simReorientShapeBoundingBox,"number result=simReorientShapeBoundingBox(number shapeHandle,number relativeToHandle)"},
    {"simHandleVisionSensor",_simHandleVisionSensor,            "number detectionCount,table auxiliaryValuesPacket1,table auxiliaryValuesPacket2,etc.=simHandleVisionSensor(number sensorHandle)"},
    {"simReadVisionSensor",_simReadVisionSensor,                "number result,table auxiliaryValues=simReadVisionSensor(number sensorHandle)"},
    {"simResetVisionSensor",_simResetVisionSensor,              "number result=simResetVisionSensor(number sensorHandle)"},
    {"simGetVisionSensorResolution",_simGetVisionSensorResolution,"table_2 resolution=simGetVisionSensorResolution(number sensorHandle)"},
    {"simGetVisionSensorImage",_simGetVisionSensorImage,        "table/string imageBuffer=simGetVisionSensorImage(number sensorHandle,number posX=0,number posY=0,number sizeX=0,\nnumber sizeY=0,number returnType=0)"},
    {"simSetVisionSensorImage",_simSetVisionSensorImage,        "number result=simSetVisionSensorImage(number sensorHandle,table imageBuffer)\nnumber result=simSetVisionSensorImage(number sensorHandle,string imageBuffer)"},
    {"simGetVisionSensorCharImage",_simGetVisionSensorCharImage,"string imageBuffer,number resolutionX,number resolutionY=simGetVisionSensorCharImage(number sensorHandle,number posX=0,\nnumber posY=0,number sizeX=0,number sizeY=0,number RgbaCutoff=0)"},
    {"simSetVisionSensorCharImage",_simSetVisionSensorCharImage,"number result=simSetVisionSensorCharImage(number sensorHandle,string imageBuffer)"},
    {"simGetVisionSensorDepthBuffer",_simGetVisionSensorDepthBuffer,"table/string depthBuffer=simGetVisionSensorDepthBuffer(number sensorHandle,number posX=0,number posY=0,\nnumber sizeX=0,number sizeY=0)"},
    {"simCheckVisionSensor",_simCheckVisionSensor,              "number result,table auxiliaryValuesPacket1,table auxiliaryValuesPacket2,etc.=simCheckVisionSensor(number sensorHandle,\nnumber entityHandle)"},
    {"simCheckVisionSensorEx",_simCheckVisionSensorEx,          "table buffer=simCheckVisionSensorEx(number sensorHandle,number entityHandle,boolean returnImage)"},
    {"simRMLPos",_simRMLPos,                                    "number handle=simRMLPos(number dofs,number smallestTimeStep,number flags,table currentPosVelAccel,table maxVelAccelJerk,\ntable selection,table targetPosVel)"},
    {"simRMLVel",_simRMLVel,                                    "number handle=simRMLVel(number dofs,number smallestTimeStep,number flags,table currentPosVelAccel,table maxAccelJerk,\ntable selection,table targetVel)"},
    {"simRMLStep",_simRMLStep,                                  "number result,table newPosVelAccel,number synchronizationTime=simRMLStep(number handle,number timeStep)"},
    {"simRMLRemove",_simRMLRemove,                              "number result=simRMLRemove(number handle)"},
    {"simRMLMoveToPosition",_simRMLMoveToPosition,              "number result,table_3 newPos,table_4 newQuaternion,table_4 newVel,table_4 newAccel,number timeLeft=simRMLMoveToPosition(number objectHandle,\nnumber relativeToObjectHandle,number flags,table_4 currentVel,table_4 currentAccel,table_4 maxVel,table_4 maxAccel,table_4 maxJerk,\ntable_3 targetPosition,table_4 targetQuaternion,table_4 targetVel)"},
    {"simRMLMoveToJointPositions",_simRMLMoveToJointPositions,  "number result,table newPos,table newVel,table newAccel,number timeLeft=simRMLMoveToJointPositions(table jointHandles,number flags,\ntable currentVel,table currentAccel,table maxVel,table maxAccel,table maxJerk,table targetPos,table targetVel,table direction=nil)"},
    {"simBuildMatrixQ",_simBuildMatrixQ,                        "table_12 matrix=simBuildMatrixQ(table_3 position,table_4 quaternion)"},
    {"simGetQuaternionFromMatrix",_simGetQuaternionFromMatrix,  "table_4 quaternion=simGetQuaternionFromMatrix(table_12 matrix)"},
    {"simFileDialog",_simFileDialog,                            "string pathAndName=simFileDialog(number mode,string title,string startPath,string initName,string extName,string ext)"},
    {"simMsgBox",_simMsgBox,                                    "number returnValue=simMsgBox(number dlgType,number buttons,string title,string message)"},
    {"simLoadModule",_simLoadModule,                            "number pluginHandle=simLoadModule(string filenameAndPath,string pluginName)"},
    {"simUnloadModule",_simUnloadModule,                        "number result=simUnloadModule(number pluginHandle)"},
    {"simCallScriptFunction",_simCallScriptFunction,            "...=simCallScriptFunction(string functionNameAtScriptName,number scriptHandleOrType,...)"},
    {"simGetConfigForTipPose",_simGetConfigForTipPose,          "table jointPositions=simGetConfigForTipPose(number ikGroupHandle,table jointHandles,number distanceThreshold,number maxTimeInMs,\ntable_4 metric=nil,table collisionPairs=nil,table jointOptions=nil,\ntable lowLimits=nil,table ranges=nil)"},
    {"simGenerateIkPath",_simGenerateIkPath,                    "table path=simGenerateIkPath(number ikGroupHandle,table jointHandles,number ptCnt,\ntable collisionPairs=nil,table jointOptions=nil)"},
    {"simGetExtensionString",_simGetExtensionString,            "string theString=simGetExtensionString(number objectHandle,number index,string key=nil)"},
    {"simComputeMassAndInertia",_simComputeMassAndInertia,      "number result=simComputeMassAndInertia(number shapeHandle,number density)"},
    {"simSetScriptVariable",_simSetScriptVariable,              "number result=simSetScriptVariable(string variableNameAtScriptName,number scriptHandleOrType,variable)"},
    {"simGetEngineFloatParameter",_simGetEngineFloatParameter,  "number floatParam=simGetEngineFloatParameter(number paramId,number objectHandle)"},
    {"simGetEngineInt32Parameter",_simGetEngineInt32Parameter,  "number int32Param=simGetEngineInt32Parameter(number paramId,number objectHandle)"},
    {"simGetEngineBoolParameter",_simGetEngineBoolParameter,    "boolean boolParam=simGetEngineBoolParameter(number paramId,number objectHandle)"},
    {"simSetEngineFloatParameter",_simSetEngineFloatParameter,  "number result=simSetEngineFloatParameter(number paramId,number objectHandle,number floatParam)"},
    {"simSetEngineInt32Parameter",_simSetEngineInt32Parameter,  "number result=simSetEngineInt32Parameter(number paramId,number objectHandle,number int32Param)"},
    {"simSetEngineBoolParameter",_simSetEngineBoolParameter,    "number result=simSetEngineBoolParameter(number paramId,number objectHandle,boolean boolParam)"},
    {"simCreateOctree",_simCreateOctree,                        "number handle=simCreateOctree(number voxelSize,number options,number pointSize)"},
    {"simCreatePointCloud",_simCreatePointCloud,                "number handle=simCreatePointCloud(number maxVoxelSize,number maxPtCntPerVoxel,number options,number pointSize)"},
    {"simSetPointCloudOptions",_simSetPointCloudOptions,        "number result=simSetPointCloudOptions(number pointCloudHandle,number maxVoxelSize,\nnumber maxPtCntPerVoxel,number options,number pointSize)"},
    {"simGetPointCloudOptions",_simGetPointCloudOptions,        "number maxVoxelSize,number maxPtCntPerVoxel,number options,number pointSize=\nsimGetPointCloudOptions(number pointCloudHandle)"},
    {"simInsertVoxelsIntoOctree",_simInsertVoxelsIntoOctree,    "number totalVoxelCnt=simInsertVoxelsIntoOctree(number octreeHandle,number options,table points,table color=nil,table tag=nil)"},
    {"simRemoveVoxelsFromOctree",_simRemoveVoxelsFromOctree,    "number totalVoxelCnt=simRemoveVoxelsFromOctree(number octreeHandle,number options,table points)"},
    {"simInsertPointsIntoPointCloud",_simInsertPointsIntoPointCloud,"number totalPointCnt=simInsertPointsIntoPointCloud(number pointCloudHandle,\nnumber options,table points,table color=nil,number duplicateTolerance=nil)"},
    {"simRemovePointsFromPointCloud",_simRemovePointsFromPointCloud,"number totalPointCnt=simRemovePointsFromPointCloud(number pointCloudHandle,\nnumber options,table points,number tolerance)"},
    {"simIntersectPointsWithPointCloud",_simIntersectPointsWithPointCloud,"number totalPointCnt=simIntersectPointsWithPointCloud(number pointCloudHandle,\nnumber options,table points,number tolerance)"},
    {"simGetOctreeVoxels",_simGetOctreeVoxels,                  "table voxels=simGetOctreeVoxels(number octreeHandle)"},
    {"simGetPointCloudPoints",_simGetPointCloudPoints,          "table points=simGetPointCloudPoints(number pointCloudHandle)"},
    {"simInsertObjectIntoOctree",_simInsertObjectIntoOctree,    "number totalVoxelCnt=simInsertObjectIntoOctree(number octreeHandle,number objectHandle,number options,table color=nil,number tag=0)"},
    {"simSubtractObjectFromOctree",_simSubtractObjectFromOctree,    "number totalVoxelCnt=simSubtractObjectFromOctree(number octreeHandle,number objectHandle,number options)"},
    {"simInsertObjectIntoPointCloud",_simInsertObjectIntoPointCloud,"number totalPointCnt=simInsertObjectIntoPointCloud(number pointCloudHandle,\nnumber objectHandle,number options,number gridSize,table color=nil,number duplicateTolerance=nil)"},
    {"simSubtractObjectFromPointCloud",_simSubtractObjectFromPointCloud,    "number totalPointCnt=simSubtractObjectFromPointCloud(number pointCloudHandle,number objectHandle,number options,number tolerance)"},
    {"simCheckOctreePointOccupancy",_simCheckOctreePointOccupancy,"number result,number tag,number locationLow,number locationHigh=simCheckOctreePointOccupancy(number octreeHandle,number options,table points)"},
    {"simOpenTextEditor",_simOpenTextEditor,                    "string text,table_2 size,table_2 position=simOpenTextEditor(string initText,string xml=nil)"},
    {"simSetVisionSensorFilter",_simSetVisionSensorFilter,      "number filterType=simSetVisionSensorFilter(number sensorHandle,number filterIndex,number options\ntable byteVals,table intVals,table floatVals,string customBuffer)"},
    {"simGetVisionSensorFilter",_simGetVisionSensorFilter,      "number filterType,number options,table byteVals,table intVals,table floatVals,string customBuffer=\nsimGetVisionSensorFilter(number sensorHandle,number filterIndex)"},
    {"simHandleSimulationStart",_simHandleSimulationStart,      "number result=simHandleSimulationStart()"},
    {"simHandleSensingStart",_simHandleSensingStart,            "number result=simHandleSensingStart()"},
    {"simAuxFunc",_simAuxFunc,                                  "... =simAuxFunc(...)"},
    {"simSetReferencedHandles",_simSetReferencedHandles,        "number result=simSetReferencedHandles(number objectHandle,table referencedHandles)"},
    {"simGetReferencedHandles",_simGetReferencedHandles,        "table referencedHandles=simGetReferencedHandles(number objectHandle)"},
    {"simGetGraphCurve",_simGetGraphCurve,                        "string label,number curveType,table curveColor,table xData,table yData,table zData,table minMax=\nsimGetGraphCurve(number graphHandle,number graphType,number curveIndex)"},


    // END_CALL_TIPS                                                                                                                                                                            \n
    // Following deprecated:

    // Following deprecated since 09/02/2017:
    {"simLoadUI",_simLoadUI,                                    "Deprecated. Use Qt-based custom UIs instead."},
    {"simSaveUI",_simSaveUI,                                    "Deprecated. Use Qt-based custom UIs instead."},
    {"simRemoveUI",_simRemoveUI,                                "Deprecated. Use Qt-based custom UIs instead."},
    {"simCreateUI",_simCreateUI,                                "Deprecated. Use Qt-based custom UIs instead."},
    {"simCreateUIButton",_simCreateUIButton,                    "Deprecated. Use Qt-based custom UIs instead."},
    {"simGetUIPosition",_simGetUIPosition,                      "Deprecated. Use Qt-based custom UIs instead."},
    {"simSetUIPosition",_simSetUIPosition,                      "Deprecated. Use Qt-based custom UIs instead."},
    {"simGetUIHandle",_simGetUIHandle,                          "Deprecated. Use Qt-based custom UIs instead."},
    {"simGetUIProperty",_simGetUIProperty,                      "Deprecated. Use Qt-based custom UIs instead."},
    {"simGetUIEventButton",_simGetUIEventButton,                "Deprecated. Use Qt-based custom UIs instead."},
    {"simSetUIProperty",_simSetUIProperty,                      "Deprecated. Use Qt-based custom UIs instead."},
    {"simGetUIButtonProperty",_simGetUIButtonProperty,          "Deprecated. Use Qt-based custom UIs instead."},
    {"simSetUIButtonProperty",_simSetUIButtonProperty,          "Deprecated. Use Qt-based custom UIs instead."},
    {"simGetUIButtonSize",_simGetUIButtonSize,                  "Deprecated. Use Qt-based custom UIs instead."},
    {"simSetUIButtonLabel",_simSetUIButtonLabel,                "Deprecated. Use Qt-based custom UIs instead."},
    {"simGetUIButtonLabel",_simGetUIButtonLabel,                "Deprecated. Use Qt-based custom UIs instead."},
    {"simSetUISlider",_simSetUISlider,                          "Deprecated. Use Qt-based custom UIs instead."},
    {"simGetUISlider",_simGetUISlider,                          "Deprecated. Use Qt-based custom UIs instead."},
    {"simSetUIButtonColor",_simSetUIButtonColor,                "Deprecated. Use Qt-based custom UIs instead."},
    {"simCreateUIButtonArray",_simCreateUIButtonArray,          "Deprecated. Use Qt-based custom UIs instead."},
    {"simSetUIButtonArrayColor",_simSetUIButtonArrayColor,      "Deprecated. Use Qt-based custom UIs instead."},
    {"simDeleteUIButtonArray",_simDeleteUIButtonArray,          "Deprecated. Use Qt-based custom UIs instead."},

    // Following deprecated since 26/12/2016:
    {"simAddSceneCustomData",_simAddSceneCustomData,                "Deprecated. Use simWriteCustomDataBlock instead."},
    {"simGetSceneCustomData",_simGetSceneCustomData,                "Deprecated. Use simReadCustomDataBlock instead."},
    {"simAddObjectCustomData",_simAddObjectCustomData,              "Deprecated. Use simWriteCustomDataBlock instead."},
    {"simGetObjectCustomData",_simGetObjectCustomData,              "Deprecated. Use simReadCustomDataBlock instead."},
    // Following deprecated since 29/10/2016:
    {"simGetMaterialId",_simGetMaterialId,                          "Deprecated. Use simSetShapeMaterial instead."},
    {"simGetShapeMaterial",_simGetShapeMaterial,                    "Deprecated. Use simSetShapeMaterial instead."},
    {"simHandleVarious",_simHandleVarious,                          "Deprecated. Use simHandleSimulationStart and simHandleSensingStart instead."},
    // Following deprecated since 13/9/2016:
    {"simPackInts",_simPackInt32Table,                              "Deprecated. Use simPackInt32Table instead."},
    {"simPackUInts",_simPackUInt32Table,                            "Deprecated. Use simPackUInt32Table instead."},
    {"simPackFloats",_simPackFloatTable,                            "Deprecated. Use simPackFloatTable instead."},
    {"simPackDoubles",_simPackDoubleTable,                          "Deprecated. Use simPackDoubleTable instead."},
    {"simPackBytes",_simPackUInt8Table,                             "Deprecated. Use simPackUInt8Table instead."},
    {"simPackWords",_simPackUInt16Table,                            "Deprecated. Use simPackUInt16Table instead."},
    {"simUnpackInts",_simUnpackInt32Table,                          "Deprecated. Use simUnpackInt32Table instead."},
    {"simUnpackUInts",_simUnpackUInt32Table,                        "Deprecated. Use simUnpackUInt32Table instead."},
    {"simUnpackFloats",_simUnpackFloatTable,                        "Deprecated. Use simUnpackFloatTable instead."},
    {"simUnpackDoubles",_simUnpackDoubleTable,                      "Deprecated. Use simUnpackDoubleTable instead."},
    {"simUnpackBytes",_simUnpackUInt8Table,                         "Deprecated. Use simUnpackUInt8Table instead."},
    {"simUnpackWords",_simUnpackUInt16Table,                        "Deprecated. Use simUnpackUInt16Table instead."},

    {"simGetInvertedMatrix",_simGetInvertedMatrix,"Deprecated. Use simInvertMatrix instead."}, // 10/05/2016
    {"simEnableWorkThreads",_simEnableWorkThreads,"Deprecated. Has no effect."},
    {"simWaitForWorkThreads",_simWaitForWorkThreads,"Deprecated. Has no effect."},
    {"simFindIkPath",_simFindIkPath,"Deprecated. See simGenerateIkPath instead."},
    {"simHandleChildScript",_simHandleChildScript,"Deprecated. See simHandleChildScripts instead."},
    {"simHandleSensingChildScripts",_simHandleSensingChildScripts,"Deprecated. See simHandleChildScripts instead."},
    {"simDelegateChildScriptExecution",_simDelegateChildScriptExecution,"Deprecated. Has no effect anymore."},
    {"simResetTracing",_simResetTracing,"Deprecated. Has no effect."},
    {"simHandleTracing",_simHandleTracing,"Deprecated. Has no effect."},
    {"simCopyPasteSelectedObjects",_simCopyPasteSelectedObjects,"Deprecated. See simCopyPasteObjects instead."},
    {"simGetShapeVertex",_simGetShapeVertex,"Deprecated. See simGetShapeMesh instead."},//"number result,table_3 relativePosition=simGetShapeVertex(number shapeHandle,number groupElementIndex,number vertexIndex)"},
    {"simGetShapeTriangle",_simGetShapeTriangle,"Deprecated. See simGetShapeMesh instead."},//"number result,table_3 vertexIndices,table_9 triangleNormals=simGetShapeTriangle(number shapeHandle,number groupElementIndex,number triangleIndex)"},
    {"simGetInstanceIndex",_simGetInstanceIndex,"Deprecated. Returns 0."},//"number index=simGetInstanceIndex()"},
    {"simGetVisibleInstanceIndex",_simGetVisibleInstanceIndex,"Deprecated. Returns 0."},//"number index=simGetVisibleInstanceIndex()"},
    {"simGetSystemTimeInMilliseconds",_simGetSystemTimeInMilliseconds,"Deprecated. See simGetSystemTimeInMs instead."},
    {"simLockInterface",_simLockInterface,"Deprecated. Has no effect anymore."},
    {"simJointGetForce",_simJointGetForce,"Deprecated. See simGetJointForce instead."},
    {"simScaleSelectedObjects",_simScaleSelectedObjects,"Deprecated. See simScaleObjects instead."},
    {"simDeleteSelectedObjects",_simDeleteSelectedObjects,"Deprecated. See simRemoveObject instead."},
    {"simResetPath",_simResetPath,"Deprecated."},
    {"simHandlePath",_simHandlePath,"Deprecated."},
    {"simResetJoint",_simResetJoint,"Deprecated."},
    {"simHandleJoint",_simHandleJoint,"Deprecated."},
    {"simGetMpConfigForTipPose",_simGetMpConfigForTipPose,"Deprecated. See simGetConfigForTipPose instead."},


    // Following for backward compatibility (Dec 2015):
    // ********************************************************
    {"simSetBooleanParameter",_simSetBoolParameter,"Deprecated. see simSetBoolParameter instead."},
    {"simGetBooleanParameter",_simGetBoolParameter,"Deprecated. see simGetBoolParameter instead."},
    {"simSetIntegerParameter",_simSetInt32Parameter,"Deprecated. see simSetInt32Parameter instead."},
    {"simGetIntegerParameter",_simGetInt32Parameter,"Deprecated. see simGetInt32Parameter instead."},
    {"simSetFloatingParameter",_simSetFloatParameter,"Deprecated. see simSetFloatParameter instead."},
    {"simGetFloatingParameter",_simGetFloatParameter,"Deprecated. see simGetFloatParameter instead."},
    {"simGetObjectIntParameter",_simGetObjectInt32Parameter,"Deprecated. see simGetObjectInt32Parameter instead."},
    {"simSetObjectIntParameter",_simSetObjectInt32Parameter,"Deprecated. see simSetObjectInt32Parameter instead."},
    // ********************************************************

    // Following for backward compatibility (nov-dec 2011):
    // ********************************************************
    {"simGet2DElementHandle",_simGetUIHandle,"Deprecated. see simGetUIHandle instead."},
    {"simGet2DElementProperty",_simGetUIProperty,"Deprecated. see simGetUIProperty instead."},
    {"simGet2DElementEventButton",_simGetUIEventButton,"Deprecated. see simGetUIEventButton instead."},
    {"simSet2DElementProperty",_simSetUIProperty,"Deprecated. see simSetUIProperty instead."},
    {"simGet2DElementButtonProperty",_simGetUIButtonProperty,"Deprecated. see simGetUIButtonProperty instead."},
    {"simSet2DElementButtonProperty",_simSetUIButtonProperty,"Deprecated. see simSetUIButtonProperty instead."},
    {"simGet2DElementButtonSize",_simGetUIButtonSize,"Deprecated. see simGetUIButtonSize instead."},
    {"simSet2DElementButtonLabel",_simSetUIButtonLabel,"Deprecated. see simSetUIButtonLabel instead."},
    {"simGet2DElementButtonLabel",_simGetUIButtonLabel,"Deprecated. see simGetUIButtonLabel instead."},
    {"simSet2DElementSlider",_simSetUISlider,"Deprecated. see simSetUISlider instead."},
    {"simGet2DElementSlider",_simGetUISlider,"Deprecated. see simGetUISlider instead."},
    {"simSet2DElementButtonColor",_simSetUIButtonColor,"Deprecated. see simSetUIButtonColor instead."},
    {"simCreate2DElementButtonArray",_simCreateUIButtonArray,"Deprecated. see simCreateUIButtonArray instead."},
    {"simSet2DElementButtonArrayColor",_simSetUIButtonArrayColor,"Deprecated. see simSetUIButtonArrayColor instead."},
    {"simDelete2DElementButtonArray",_simDeleteUIButtonArray,"Deprecated. see simDeleteUIButtonArray instead."},
    {"simRemove2DElement",_simRemoveUI,"Deprecated. see simRemoveUI instead."},
    {"simCreate2DElement",_simCreateUI,"Deprecated. see simCreateUI instead."},
    {"simCreate2DElementButton",_simCreateUIButton,"Deprecated. see simCreateUIButton instead."},
    {"simGet2DElementPosition",_simGetUIPosition,"Deprecated. see simGetUIPosition instead."},
    {"simSet2DElementPosition",_simSetUIPosition,"Deprecated. see simSetUIPosition instead."},
    // ********************************************************

    // Following for backward compatibility (10/04/2012):
    // ********************************************************
    {"simSerialPortOpen",_simSerialPortOpen,"Deprecated. See simSerialOpen instead."},
    {"simSerialPortClose",_simSerialPortClose,"Deprecated. See simSerialClose instead."},
    {"simSerialPortSend",_simSerialPortSend,"Deprecated. See simSerialSend instead."},
    {"simSerialPortRead",_simSerialPortRead,"Deprecated. See simSerialRead instead."},
    // ********************************************************


    // Following for backward compatibility:
    // ********************************************************
    {"simHandleRenderingSensor",_simHandleVisionSensor,"Deprecated. see simHandleVisionSensor instead."},
    {"simReadRenderingSensor",_simReadVisionSensor,"Deprecated. see simReadVisionSensor instead."},
    {"simResetRenderingSensor",_simResetVisionSensor,"Deprecated. see simResetVisionSensor instead."},
    {"simGetRenderingSensorResolution",_simGetVisionSensorResolution,"Deprecated. see simGetVisionSensorResolution instead."},
    {"simGetRenderingSensorImage",_simGetVisionSensorImage,"Deprecated. see simGetVisionSensorImage instead."},
    {"simSetRenderingSensorImage",_simSetVisionSensorImage,"Deprecated. see simSetVisionSensorImage instead."},
    {"simGetRenderingSensorDepthBuffer",_simGetVisionSensorDepthBuffer,"Deprecated. see simGetVisionSensorDepthBuffer instead."},
    {"simCheckRenderingSensor",_simCheckVisionSensor,"Deprecated. see simCheckVisionSensor instead."},
    {"simCheckRenderingSensorEx",_simCheckVisionSensorEx,"Deprecated. see simCheckVisionSensorEx instead."},
    // ********************************************************

    // Following for backward compatibility (deprecated since 23/5/2014):
    // ********************************************************
    {"simRMLPosition",_simRMLPosition,"Deprecated. See simRMLPos instead."},
    {"simRMLVelocity",_simRMLVelocity,"Deprecated. See simRMLVel instead."},
    // ********************************************************


    {"simTest",_simTest,"test function - shouldn't be used"},

    {"",NULL,""}
};


static SLuaVariables simLuaVariables[]= 
{
    // START_CONSTANTS

    // 3D object types (main types):
    {"sim_object_shape_type",sim_object_shape_type},
    {"sim_object_joint_type",sim_object_joint_type},
    {"sim_object_graph_type",sim_object_graph_type},
    {"sim_object_camera_type",sim_object_camera_type},
    {"sim_object_dummy_type",sim_object_dummy_type},
    {"sim_object_proximitysensor_type",sim_object_proximitysensor_type},
    {"sim_object_path_type",sim_object_path_type},
    {"sim_object_renderingsensor_type",sim_object_visionsensor_type},
    {"sim_object_visionsensor_type",sim_object_visionsensor_type},
    {"sim_object_mill_type",sim_object_mill_type},
    {"sim_object_forcesensor_type",sim_object_forcesensor_type},
    {"sim_object_light_type",sim_object_light_type},
    {"sim_object_mirror_type",sim_object_mirror_type},
    {"sim_object_octree_type",sim_object_octree_type},
    {"sim_object_pointcloud_type",sim_object_pointcloud_type},

    // 3D object sub-types:
    {"sim_light_omnidirectional_subtype",sim_light_omnidirectional_subtype},
    {"sim_light_spot_subtype",sim_light_spot_subtype},
    {"sim_light_directional_subtype",sim_light_directional_subtype},
    {"sim_joint_revolute_subtype",sim_joint_revolute_subtype},
    {"sim_joint_prismatic_subtype",sim_joint_prismatic_subtype},
    {"sim_joint_spherical_subtype",sim_joint_spherical_subtype},
    {"sim_shape_simpleshape_subtype",sim_shape_simpleshape_subtype},
    {"sim_shape_multishape_subtype",sim_shape_multishape_subtype},
    {"sim_proximitysensor_pyramid_subtype",sim_proximitysensor_pyramid_subtype},
    {"sim_proximitysensor_cylinder_subtype",sim_proximitysensor_cylinder_subtype},
    {"sim_proximitysensor_disc_subtype",sim_proximitysensor_disc_subtype},
    {"sim_proximitysensor_cone_subtype",sim_proximitysensor_cone_subtype},
    {"sim_proximitysensor_ray_subtype",sim_proximitysensor_ray_subtype},
    {"sim_mill_pyramid_subtype",sim_mill_pyramid_subtype},
    {"sim_mill_cylinder_subtype",sim_mill_cylinder_subtype},
    {"sim_mill_disc_subtype",sim_mill_disc_subtype},
    {"sim_mill_cone_subtype",sim_mill_cone_subtype},
    {"sim_object_no_subtype",sim_object_no_subtype},

    // Other object types:
    {"sim_appobj_object_type",sim_appobj_object_type},
    {"sim_appobj_collision_type",sim_appobj_collision_type},
    {"sim_appobj_distance_type",sim_appobj_distance_type},
    {"sim_appobj_simulation_type",sim_appobj_simulation_type},
    {"sim_appobj_ik_type",sim_appobj_ik_type},
    {"sim_appobj_constraintsolver_type",sim_appobj_constraintsolver_type},
    {"sim_appobj_collection_type",sim_appobj_collection_type},
    {"sim_appobj_2delement_type",sim_appobj_ui_type},  // for backward compatibility
    {"sim_appobj_ui_type",sim_appobj_ui_type},
    {"sim_appobj_script_type",sim_appobj_script_type},
    {"sim_appobj_pathplanning_type",sim_appobj_pathplanning_type},
    {"sim_appobj_texture_type",sim_appobj_texture_type},
    {"sim_appobj_motionplanning_type",sim_appobj_motionplanning_type},

    // IK calculation methods:
    {"sim_ik_pseudo_inverse_method",sim_ik_pseudo_inverse_method},
    {"sim_ik_damped_least_squares_method",sim_ik_damped_least_squares_method},
    {"sim_ik_jacobian_transpose_method",sim_ik_jacobian_transpose_method},

    // IK constraints:
    {"sim_ik_x_constraint",sim_ik_x_constraint},
    {"sim_ik_y_constraint",sim_ik_y_constraint},
    {"sim_ik_z_constraint",sim_ik_z_constraint},
    {"sim_ik_alpha_beta_constraint",sim_ik_alpha_beta_constraint},
    {"sim_ik_gamma_constraint",sim_ik_gamma_constraint},
    {"sim_ik_avoidance_constraint",sim_ik_avoidance_constraint},

    // IK calculation results:
    {"sim_ikresult_not_performed",sim_ikresult_not_performed},
    {"sim_ikresult_success",sim_ikresult_success},
    {"sim_ikresult_fail",sim_ikresult_fail},

    // Simulation messages:
    {"sim_message_ui_button_state_change",sim_message_ui_button_state_change},
    {"sim_message_model_loaded",sim_message_model_loaded},
    {"sim_message_scene_loaded",sim_message_scene_loaded},
    {"sim_message_object_selection_changed",sim_message_object_selection_changed},
    {"sim_message_keypress",sim_message_keypress},
    {"sim_message_bannerclicked",sim_message_bannerclicked},
    {"sim_message_prox_sensor_select_down",sim_message_prox_sensor_select_down},
    {"sim_message_prox_sensor_select_up",sim_message_prox_sensor_select_up},
    {"sim_message_pick_select_down",sim_message_pick_select_down},
    
    // Button properties:
    {"sim_buttonproperty_button",sim_buttonproperty_button},
    {"sim_buttonproperty_label",sim_buttonproperty_label},
    {"sim_buttonproperty_editbox",sim_buttonproperty_editbox},
    {"sim_buttonproperty_slider",sim_buttonproperty_slider},
    {"sim_buttonproperty_staydown",sim_buttonproperty_staydown},
    {"sim_buttonproperty_enabled",sim_buttonproperty_enabled},
    {"sim_buttonproperty_borderless",sim_buttonproperty_borderless},
    {"sim_buttonproperty_horizontallycentered",sim_buttonproperty_horizontallycentered},
    {"sim_buttonproperty_ignoremouse",sim_buttonproperty_ignoremouse},
    {"sim_buttonproperty_isdown",sim_buttonproperty_isdown},
    {"sim_buttonproperty_transparent",sim_buttonproperty_transparent},
    {"sim_buttonproperty_nobackgroundcolor",sim_buttonproperty_nobackgroundcolor},
    {"sim_buttonproperty_rollupaction",sim_buttonproperty_rollupaction},
    {"sim_buttonproperty_closeaction",sim_buttonproperty_closeaction},
    {"sim_buttonproperty_verticallycentered",sim_buttonproperty_verticallycentered},
    {"sim_buttonproperty_downupevent",sim_buttonproperty_downupevent},

    // object properties. Combine with the | operator
    {"sim_objectproperty_collapsed",sim_objectproperty_collapsed},
    {"sim_objectproperty_selectable",sim_objectproperty_selectable},
    {"sim_objectproperty_selectmodelbaseinstead",sim_objectproperty_selectmodelbaseinstead},
    {"sim_objectproperty_dontshowasinsidemodel",sim_objectproperty_dontshowasinsidemodel},
    {"sim_objectproperty_canupdatedna",sim_objectproperty_canupdatedna},
    {"sim_objectproperty_selectinvisible",sim_objectproperty_selectinvisible},
    {"sim_objectproperty_depthinvisible",sim_objectproperty_depthinvisible},

    // Simulation status:
    {"sim_simulation_stopped",sim_simulation_stopped},
    {"sim_simulation_paused",sim_simulation_paused},
    {"sim_simulation_advancing",sim_simulation_advancing},
    {"sim_simulation_advancing_firstafterstop",sim_simulation_advancing_firstafterstop},
    {"sim_simulation_advancing_running",sim_simulation_advancing_running},
    {"sim_simulation_advancing_lastbeforepause",sim_simulation_advancing_lastbeforepause},
    {"sim_simulation_advancing_firstafterpause",sim_simulation_advancing_firstafterpause},
    {"sim_simulation_advancing_abouttostop",sim_simulation_advancing_abouttostop},
    {"sim_simulation_advancing_lastbeforestop",sim_simulation_advancing_lastbeforestop},

    // Texture mapping modes:
    {"sim_texturemap_plane",sim_texturemap_plane},
    {"sim_texturemap_cylinder",sim_texturemap_cylinder},
    {"sim_texturemap_sphere",sim_texturemap_sphere},
    {"sim_texturemap_cube",sim_texturemap_cube},

    // script types:
    {"sim_scripttype_mainscript",sim_scripttype_mainscript},
    {"sim_scripttype_childscript",sim_scripttype_childscript},
    {"sim_scripttype_addonscript",sim_scripttype_addonscript},
    {"sim_scripttype_addonfunction",sim_scripttype_addonfunction},
    {"sim_scripttype_jointctrlcallback",sim_scripttype_jointctrlcallback},
    {"sim_scripttype_contactcallback",sim_scripttype_contactcallback},
    {"sim_scripttype_generalcallback",sim_scripttype_generalcallback},
    {"sim_scripttype_customizationscript",sim_scripttype_customizationscript},
    {"sim_scripttype_threaded",sim_scripttype_threaded},

    // Main script call types:
    {"sim_mainscriptcall_initialization",sim_mainscriptcall_initialization},
    {"sim_mainscriptcall_cleanup",sim_mainscriptcall_cleanup},
    {"sim_mainscriptcall_regular",sim_mainscriptcall_regular},

    // Child script call types:
    {"sim_childscriptcall_initialization",sim_childscriptcall_initialization},
    {"sim_childscriptcall_cleanup",sim_childscriptcall_cleanup},
    {"sim_childscriptcall_actuation",sim_childscriptcall_actuation},
    {"sim_childscriptcall_sensing",sim_childscriptcall_sensing},
    {"sim_childscriptcall_threaded",sim_childscriptcall_threaded},

    // Customization script call types:
    {"sim_customizationscriptcall_initialization",sim_customizationscriptcall_initialization},
    {"sim_customizationscriptcall_cleanup",sim_customizationscriptcall_cleanup},
    {"sim_customizationscriptcall_nonsimulation",sim_customizationscriptcall_nonsimulation},
    {"sim_customizationscriptcall_lastbeforesimulation",sim_customizationscriptcall_lastbeforesimulation},
    {"sim_customizationscriptcall_firstaftersimulation",sim_customizationscriptcall_firstaftersimulation},
    {"sim_customizationscriptcall_simulationactuation",sim_customizationscriptcall_simulationactuation},
    {"sim_customizationscriptcall_simulationsensing",sim_customizationscriptcall_simulationsensing},
    {"sim_customizationscriptcall_simulationpause",sim_customizationscriptcall_simulationpause},
    {"sim_customizationscriptcall_simulationpausefirst",sim_customizationscriptcall_simulationpausefirst},
    {"sim_customizationscriptcall_simulationpauselast",sim_customizationscriptcall_simulationpauselast},
    {"sim_customizationscriptcall_lastbeforeinstanceswitch",sim_customizationscriptcall_lastbeforeinstanceswitch},
    {"sim_customizationscriptcall_firstafterinstanceswitch",sim_customizationscriptcall_firstafterinstanceswitch},

    // Add-on script call types
    {"sim_addonscriptcall_initialization",sim_addonscriptcall_initialization},
    {"sim_addonscriptcall_run",sim_addonscriptcall_run},
    {"sim_addonscriptcall_suspend",sim_addonscriptcall_suspend},
    {"sim_addonscriptcall_restarting",sim_addonscriptcall_restarting},
    {"sim_addonscriptcall_cleanup",sim_addonscriptcall_cleanup},

    // script attributes:
    {"sim_customizationscriptattribute_activeduringsimulation",sim_customizationscriptattribute_activeduringsimulation},
    {"sim_scriptattribute_executionorder",sim_scriptattribute_executionorder},
    {"sim_scriptattribute_executioncount",sim_scriptattribute_executioncount},
    {"sim_childscriptattribute_automaticcascadingcalls",sim_childscriptattribute_automaticcascadingcalls},
    {"sim_childscriptattribute_enabled",sim_childscriptattribute_enabled},
    {"sim_customizationscriptattribute_cleanupbeforesave",sim_customizationscriptattribute_cleanupbeforesave},

    // script execution order:
    {"sim_scriptexecorder_first",sim_scriptexecorder_first},
    {"sim_scriptexecorder_normal",sim_scriptexecorder_normal},
    {"sim_scriptexecorder_last",sim_scriptexecorder_last},

    // threaded script resume location:
    {"sim_scriptthreadresume_allnotyetresumed",sim_scriptthreadresume_allnotyetresumed},
    {"sim_scriptthreadresume_default",sim_scriptthreadresume_default},
    {"sim_scriptthreadresume_actuation_first",sim_scriptthreadresume_actuation_first},
    {"sim_scriptthreadresume_actuation_last",sim_scriptthreadresume_actuation_last},
    {"sim_scriptthreadresume_sensing_first",sim_scriptthreadresume_sensing_first},
    {"sim_scriptthreadresume_sensing_last",sim_scriptthreadresume_sensing_last},
    {"sim_scriptthreadresume_custom",sim_scriptthreadresume_custom},

    // General callback IDs:
    {"sim_callbackid_rossubscriber",sim_callbackid_rossubscriber},
    {"sim_callbackid_dynstep",sim_callbackid_dynstep},
    {"sim_callbackid_userdefined",sim_callbackid_userdefined},

    // Script execution result:
    {"sim_script_no_error",sim_script_no_error},
    {"sim_script_main_script_nonexistent",sim_script_main_script_nonexistent},
    {"sim_script_main_not_called",sim_script_main_script_not_called},
    {"sim_script_reentrance_error",sim_script_reentrance_error},
    {"sim_script_lua_error",sim_script_lua_error},
    {"sim_script_call_error",sim_script_call_error},

    // API call error messages
    {"sim_api_error_report",sim_api_error_report},
    {"sim_api_error_output",sim_api_error_output},
    {"sim_api_warning_output",sim_api_warning_output},
    // Following 3 for backward compatibility:
    {"sim_api_errormessage_ignore",sim_api_errormessage_ignore},
    {"sim_api_errormessage_report",sim_api_errormessage_report},
    {"sim_api_errormessage_output",sim_api_errormessage_output},


    // special arguments for some functions:
    {"sim_handle_all",sim_handle_all},
    {"sim_handle_all_except_explicit",sim_handle_all_except_explicit},
    {"sim_handle_self",sim_handle_self},
    {"sim_handle_main_script",sim_handle_main_script},
    {"sim_handle_tree",sim_handle_tree},
    {"sim_handle_chain",sim_handle_chain},
    {"sim_handle_single",sim_handle_single},
    {"sim_handle_default",sim_handle_default},
    {"sim_handle_all_except_self",sim_handle_all_except_self},
    {"sim_handle_parent",sim_handle_parent},
    {"sim_handle_scene",sim_handle_scene},
    {"sim_handle_app",sim_handle_app},

    // special handle flags:
    {"sim_handleflag_assembly",sim_handleflag_assembly},
    {"sim_handleflag_camera",sim_handleflag_camera},
    {"sim_handleflag_togglevisibility",sim_handleflag_togglevisibility},
    {"sim_handleflag_extended",sim_handleflag_extended},
    {"sim_handleflag_greyscale",sim_handleflag_greyscale},
    {"sim_handleflag_codedstring",sim_handleflag_codedstring},
    {"sim_handleflag_model",sim_handleflag_model},
    {"sim_handleflag_rawvalue",sim_handleflag_rawvalue},


    // General object main properties:
    {"sim_objectspecialproperty_collidable",sim_objectspecialproperty_collidable},
    {"sim_objectspecialproperty_measurable",sim_objectspecialproperty_measurable},
    {"sim_objectspecialproperty_detectable_ultrasonic",sim_objectspecialproperty_detectable_ultrasonic},
    {"sim_objectspecialproperty_detectable_infrared",sim_objectspecialproperty_detectable_infrared},
    {"sim_objectspecialproperty_detectable_laser",sim_objectspecialproperty_detectable_laser},
    {"sim_objectspecialproperty_detectable_inductive",sim_objectspecialproperty_detectable_inductive},
    {"sim_objectspecialproperty_detectable_capacitive",sim_objectspecialproperty_detectable_capacitive},
    {"sim_objectspecialproperty_renderable",sim_objectspecialproperty_renderable},
    {"sim_objectspecialproperty_detectable_all",sim_objectspecialproperty_detectable_all},
    {"sim_objectspecialproperty_cuttable",sim_objectspecialproperty_cuttable},
    {"sim_objectspecialproperty_pathplanning_ignored",sim_objectspecialproperty_pathplanning_ignored},

    // Model override properties:
    {"sim_modelproperty_not_collidable",sim_modelproperty_not_collidable},
    {"sim_modelproperty_not_measurable",sim_modelproperty_not_measurable},
    {"sim_modelproperty_not_renderable",sim_modelproperty_not_renderable},
    {"sim_modelproperty_not_detectable",sim_modelproperty_not_detectable},
    {"sim_modelproperty_not_cuttable",sim_modelproperty_not_cuttable},
    {"sim_modelproperty_not_dynamic",sim_modelproperty_not_dynamic},
    {"sim_modelproperty_not_respondable",sim_modelproperty_not_respondable},
    {"sim_modelproperty_not_reset",sim_modelproperty_not_reset},
    {"sim_modelproperty_not_visible",sim_modelproperty_not_visible},
    {"sim_modelproperty_scripts_inactive",sim_modelproperty_scripts_inactive},
    {"sim_modelproperty_not_showasinsidemodel",sim_modelproperty_not_showasinsidemodel},
    {"sim_modelproperty_not_model",sim_modelproperty_not_model},
    
    // Generic dialog styles:
    {"sim_dlgstyle_message",sim_dlgstyle_message},
    {"sim_dlgstyle_input",sim_dlgstyle_input},
    {"sim_dlgstyle_ok",sim_dlgstyle_ok},
    {"sim_dlgstyle_ok_cancel",sim_dlgstyle_ok_cancel},
    {"sim_dlgstyle_yes_no",sim_dlgstyle_yes_no},
    {"sim_dlgstyle_dont_center",sim_dlgstyle_dont_center},

    // Generic dialog return value:
    {"sim_dlgret_still_open",sim_dlgret_still_open},
    {"sim_dlgret_ok",sim_dlgret_ok},
    {"sim_dlgret_cancel",sim_dlgret_cancel},
    {"sim_dlgret_yes",sim_dlgret_yes},
    {"sim_dlgret_no",sim_dlgret_no},

    // Path properties: 
    {"sim_pathproperty_show_line",sim_pathproperty_show_line},
    {"sim_pathproperty_show_orientation",sim_pathproperty_show_orientation},
    {"sim_pathproperty_closed_path",sim_pathproperty_closed_path},
    {"sim_pathproperty_automatic_orientation",sim_pathproperty_automatic_orientation},
    {"sim_pathproperty_flat_path",sim_pathproperty_flat_path},
    {"sim_pathproperty_show_position",sim_pathproperty_show_position},
    {"sim_pathproperty_keep_x_up",sim_pathproperty_keep_x_up},

    // distance calculation methods:
    {"sim_distcalcmethod_dl",sim_distcalcmethod_dl},
    {"sim_distcalcmethod_dac",sim_distcalcmethod_dac},
    {"sim_distcalcmethod_max_dl_dac",sim_distcalcmethod_max_dl_dac},
    {"sim_distcalcmethod_dl_and_dac",sim_distcalcmethod_dl_and_dac},
    {"sim_distcalcmethod_sqrt_dl2_and_dac2",sim_distcalcmethod_sqrt_dl2_and_dac2},
    {"sim_distcalcmethod_dl_if_nonzero",sim_distcalcmethod_dl_if_nonzero},
    {"sim_distcalcmethod_dac_if_nonzero",sim_distcalcmethod_dac_if_nonzero},

    // Boolean parameters:  
    {"sim_boolparam_hierarchy_visible",sim_boolparam_hierarchy_visible},
    {"sim_boolparam_console_visible",sim_boolparam_console_visible},
    {"sim_boolparam_collision_handling_enabled",sim_boolparam_collision_handling_enabled},
    {"sim_boolparam_distance_handling_enabled",sim_boolparam_distance_handling_enabled},
    {"sim_boolparam_ik_handling_enabled",sim_boolparam_ik_handling_enabled},
    {"sim_boolparam_gcs_handling_enabled",sim_boolparam_gcs_handling_enabled},
    {"sim_boolparam_dynamics_handling_enabled",sim_boolparam_dynamics_handling_enabled},
    {"sim_boolparam_proximity_sensor_handling_enabled",sim_boolparam_proximity_sensor_handling_enabled},
    {"sim_boolparam_vision_sensor_handling_enabled",sim_boolparam_vision_sensor_handling_enabled},
    {"sim_boolparam_rendering_sensor_handling_enabled",sim_boolparam_vision_sensor_handling_enabled},
    {"sim_boolparam_mill_handling_enabled",sim_boolparam_mill_handling_enabled},
    {"sim_boolparam_browser_visible",sim_boolparam_browser_visible},
    {"sim_boolparam_scene_and_model_load_messages",sim_boolparam_scene_and_model_load_messages},
    {"sim_boolparam_shape_textures_are_visible",sim_boolparam_shape_textures_are_visible},
    {"sim_boolparam_display_enabled",sim_boolparam_display_enabled},
    {"sim_boolparam_infotext_visible",sim_boolparam_infotext_visible},
    {"sim_boolparam_statustext_open",sim_boolparam_statustext_open},
    {"sim_boolparam_fog_enabled",sim_boolparam_fog_enabled},
    {"sim_boolparam_rml2_available",sim_boolparam_rml2_available},
    {"sim_boolparam_rml4_available",sim_boolparam_rml4_available},
    {"sim_boolparam_mirrors_enabled",sim_boolparam_mirrors_enabled},
    {"sim_boolparam_aux_clip_planes_enabled",sim_boolparam_aux_clip_planes_enabled},
    {"sim_boolparam_full_model_copy_from_api",sim_boolparam_reserved3},
    {"sim_boolparam_realtime_simulation",sim_boolparam_realtime_simulation},
    {"sim_boolparam_use_glfinish_cmd",sim_boolparam_use_glfinish_cmd},
    {"sim_boolparam_force_show_wireless_emission",sim_boolparam_force_show_wireless_emission},
    {"sim_boolparam_force_show_wireless_reception",sim_boolparam_force_show_wireless_reception},
    {"sim_boolparam_video_recording_triggered",sim_boolparam_video_recording_triggered},
    {"sim_boolparam_threaded_rendering_enabled",sim_boolparam_threaded_rendering_enabled},
    {"sim_boolparam_fullscreen",sim_boolparam_fullscreen},
    {"sim_boolparam_headless",sim_boolparam_headless},
    {"sim_boolparam_hierarchy_toolbarbutton_enabled",sim_boolparam_hierarchy_toolbarbutton_enabled},
    {"sim_boolparam_browser_toolbarbutton_enabled",sim_boolparam_browser_toolbarbutton_enabled},
    {"sim_boolparam_objectshift_toolbarbutton_enabled",sim_boolparam_objectshift_toolbarbutton_enabled},
    {"sim_boolparam_objectrotate_toolbarbutton_enabled",sim_boolparam_objectrotate_toolbarbutton_enabled},
    {"sim_boolparam_force_calcstruct_all_visible",sim_boolparam_force_calcstruct_all_visible},
    {"sim_boolparam_force_calcstruct_all",sim_boolparam_force_calcstruct_all},
    {"sim_boolparam_exit_request",sim_boolparam_exit_request},
    {"sim_boolparam_play_toolbarbutton_enabled",sim_boolparam_play_toolbarbutton_enabled},
    {"sim_boolparam_pause_toolbarbutton_enabled",sim_boolparam_pause_toolbarbutton_enabled},
    {"sim_boolparam_stop_toolbarbutton_enabled",sim_boolparam_stop_toolbarbutton_enabled},
    {"sim_boolparam_waiting_for_trigger",sim_boolparam_waiting_for_trigger},
    {"sim_boolparam_objproperties_toolbarbutton_enabled",sim_boolparam_objproperties_toolbarbutton_enabled},
    {"sim_boolparam_calcmodules_toolbarbutton_enabled",sim_boolparam_calcmodules_toolbarbutton_enabled},
    {"sim_boolparam_rosinterface_donotrunmainscript",sim_boolparam_rosinterface_donotrunmainscript},

    // Integer parameters:  
    {"sim_intparam_error_report_mode",sim_intparam_error_report_mode},
    {"sim_intparam_program_version",sim_intparam_program_version},
    {"sim_intparam_compilation_version",sim_intparam_compilation_version},
    {"sim_intparam_current_page",sim_intparam_current_page},
    {"sim_intparam_flymode_camera_handle",sim_intparam_flymode_camera_handle},
    {"sim_intparam_dynamic_step_divider",sim_intparam_dynamic_step_divider},
    {"sim_intparam_dynamic_engine",sim_intparam_dynamic_engine},
    {"sim_intparam_server_port_start",sim_intparam_server_port_start},
    {"sim_intparam_server_port_range",sim_intparam_server_port_range},
    {"sim_intparam_server_port_next",sim_intparam_server_port_next},
    {"sim_intparam_visible_layers",sim_intparam_visible_layers},
    {"sim_intparam_infotext_style",sim_intparam_infotext_style},
    {"sim_intparam_settings",sim_intparam_settings},
    {"sim_intparam_qt_version",sim_intparam_qt_version},
    {"sim_intparam_event_flags_read",sim_intparam_event_flags_read},
    {"sim_intparam_event_flags_read_clear",sim_intparam_event_flags_read_clear},
    {"sim_intparam_platform",sim_intparam_platform},
    {"sim_intparam_scene_unique_id",sim_intparam_scene_unique_id},
    {"sim_intparam_edit_mode_type",sim_intparam_edit_mode_type},
    {"sim_intparam_work_thread_count",sim_intparam_work_thread_count}, // deprecated
    {"sim_intparam_mouse_x",sim_intparam_mouse_x},
    {"sim_intparam_mouse_y",sim_intparam_mouse_y},
    {"sim_intparam_core_count",sim_intparam_core_count},
    {"sim_intparam_work_thread_calc_time_ms",sim_intparam_work_thread_calc_time_ms}, // deprecated
    {"sim_intparam_idle_fps",sim_intparam_idle_fps},
    {"sim_intparam_prox_sensor_select_down",sim_intparam_prox_sensor_select_down},
    {"sim_intparam_prox_sensor_select_up",sim_intparam_prox_sensor_select_up},
    {"sim_intparam_stop_request_counter",sim_intparam_stop_request_counter},
    {"sim_intparam_program_revision",sim_intparam_program_revision},
    {"sim_intparam_mouse_buttons",sim_intparam_mouse_buttons},
    {"sim_intparam_dynamic_warning_disabled_mask",sim_intparam_dynamic_warning_disabled_mask},
    {"sim_intparam_simulation_warning_disabled_mask",sim_intparam_simulation_warning_disabled_mask},
    {"sim_intparam_scene_index",sim_intparam_scene_index},
    {"sim_intparam_motionplanning_seed",sim_intparam_motionplanning_seed},
    {"sim_intparam_speedmodifier",sim_intparam_speedmodifier},
    {"sim_intparam_dynamic_iteration_count",sim_intparam_dynamic_iteration_count},

    // Float parameters:
    {"sim_floatparam_rand",sim_floatparam_rand},
    {"sim_floatparam_simulation_time_step",sim_floatparam_simulation_time_step},
    {"sim_floatparam_stereo_distance",sim_floatparam_stereo_distance},
    {"sim_floatparam_dynamic_step_size",sim_floatparam_dynamic_step_size},
    {"sim_floatparam_mouse_wheel_zoom_factor",sim_floatparam_mouse_wheel_zoom_factor},

    // Array parameters:
    {"sim_arrayparam_gravity",sim_arrayparam_gravity},
    {"sim_arrayparam_fog",sim_arrayparam_fog},
    {"sim_arrayparam_fog_color",sim_arrayparam_fog_color},
    {"sim_arrayparam_background_color1",sim_arrayparam_background_color1},
    {"sim_arrayparam_background_color2",sim_arrayparam_background_color2},
    {"sim_arrayparam_ambient_light",sim_arrayparam_ambient_light},
    {"sim_arrayparam_random_euler",sim_arrayparam_random_euler},


    // String parameters:
    {"sim_stringparam_application_path",sim_stringparam_application_path},
    {"sim_stringparam_video_filename",sim_stringparam_video_filename},
    {"sim_stringparam_app_arg1",sim_stringparam_app_arg1},
    {"sim_stringparam_app_arg2",sim_stringparam_app_arg2},
    {"sim_stringparam_app_arg3",sim_stringparam_app_arg3},
    {"sim_stringparam_app_arg4",sim_stringparam_app_arg4},
    {"sim_stringparam_app_arg5",sim_stringparam_app_arg5},
    {"sim_stringparam_app_arg6",sim_stringparam_app_arg6},
    {"sim_stringparam_app_arg7",sim_stringparam_app_arg7},
    {"sim_stringparam_app_arg8",sim_stringparam_app_arg8},
    {"sim_stringparam_app_arg9",sim_stringparam_app_arg9},
    {"sim_stringparam_scene_path_and_name",sim_stringparam_scene_path_and_name},
    {"sim_stringparam_remoteapi_temp_file_dir",sim_stringparam_remoteapi_temp_file_dir},
    {"sim_stringparam_scene_path",sim_stringparam_scene_path},
    {"sim_stringparam_scene_name",sim_stringparam_scene_name},

    // Rendering attributes:
    {"sim_displayattribute_renderpass",sim_displayattribute_renderpass},
    {"sim_displayattribute_depthpass",sim_displayattribute_depthpass},
    {"sim_displayattribute_pickpass",sim_displayattribute_pickpass},
    {"sim_displayattribute_selected",sim_displayattribute_selected},
    {"sim_displayattribute_groupselection",sim_displayattribute_groupselection},
    {"sim_displayattribute_mainselection",sim_displayattribute_mainselection},
    {"sim_displayattribute_forcewireframe",sim_displayattribute_forcewireframe},
    {"sim_displayattribute_forbidwireframe",sim_displayattribute_forbidwireframe},
    {"sim_displayattribute_forbidedges",sim_displayattribute_forbidedges},
    {"sim_displayattribute_originalcolors",sim_displayattribute_originalcolors},
    {"sim_displayattribute_ignorelayer",sim_displayattribute_ignorelayer},
    {"sim_displayattribute_forvisionsensor",sim_displayattribute_forvisionsensor},
    {"sim_displayattribute_colorcodedpickpass",sim_displayattribute_colorcodedpickpass},
    {"sim_displayattribute_colorcoded",sim_displayattribute_colorcoded},
    {"sim_displayattribute_trianglewireframe",sim_displayattribute_trianglewireframe},
//  {"sim_displayattribute_simplifyasboundingbox",sim_displayattribute_simplifyasboundingbox},
    {"sim_displayattribute_thickEdges",sim_displayattribute_thickEdges},
    {"sim_displayattribute_dynamiccontentonly",sim_displayattribute_dynamiccontentonly},
    {"sim_displayattribute_mirror",sim_displayattribute_mirror},
    {"sim_displayattribute_useauxcomponent",sim_displayattribute_useauxcomponent},
    {"sim_displayattribute_ignorerenderableflag",sim_displayattribute_ignorerenderableflag},
    {"sim_displayattribute_noopenglcallbacks",sim_displayattribute_noopenglcallbacks},
//  {"sim_displayattribute_forraytracingvisionsensor",sim_displayattribute_forraytracingvisionsensor},
    {"sim_displayattribute_noghosts",sim_displayattribute_noghosts},
    {"sim_displayattribute_nopointclouds",sim_displayattribute_nopointclouds},
    {"sim_displayattribute_nodrawingobjects",sim_displayattribute_nodrawingobjects},
    {"sim_displayattribute_noparticles",sim_displayattribute_noparticles},
    {"sim_displayattribute_colorcodedtriangles",sim_displayattribute_colorcodedtriangles},

    // UI menu attributes:
    {"sim_ui_menu_title",sim_ui_menu_title},
    {"sim_ui_menu_minimize",sim_ui_menu_minimize},
    {"sim_ui_menu_close",sim_ui_menu_close},

    // Navigation and selection modes with the mouse:
    {"sim_navigation_passive",sim_navigation_passive},
    {"sim_navigation_camerashift",sim_navigation_camerashift},
    {"sim_navigation_camerarotate",sim_navigation_camerarotate},
    {"sim_navigation_camerazoom",sim_navigation_camerazoom},
    {"sim_navigation_cameratilt",sim_navigation_cameratilt},
    {"sim_navigation_cameraangle",sim_navigation_cameraangle},
    {"sim_navigation_camerafly",sim_navigation_camerafly},
    {"sim_navigation_objectshift",sim_navigation_objectshift},
    {"sim_navigation_objectrotate",sim_navigation_objectrotate},
    {"sim_navigation_createpathpoint",sim_navigation_createpathpoint},
    {"sim_navigation_clickselection",sim_navigation_clickselection},
    {"sim_navigation_ctrlselection",sim_navigation_ctrlselection},
    {"sim_navigation_shiftselection",sim_navigation_shiftselection},
    {"sim_navigation_camerazoomwheel",sim_navigation_camerazoomwheel},
    {"sim_navigation_camerarotaterightbutton",sim_navigation_camerarotaterightbutton},
    {"sim_navigation_camerarotatemiddlebutton",sim_navigation_camerarotatemiddlebutton},

    // drawing objects
    {"sim_drawing_points",sim_drawing_points},
    {"sim_drawing_lines",sim_drawing_lines},
    {"sim_drawing_triangles",sim_drawing_triangles},
    {"sim_drawing_trianglepoints",sim_drawing_trianglepoints},
    {"sim_drawing_quadpoints",sim_drawing_quadpoints},
    {"sim_drawing_discpoints",sim_drawing_discpoints},
    {"sim_drawing_cubepoints",sim_drawing_cubepoints},
    {"sim_drawing_spherepoints",sim_drawing_spherepoints},
    {"sim_drawing_itemcolors",sim_drawing_itemcolors},
    {"sim_drawing_vertexcolors",sim_drawing_vertexcolors},
    {"sim_drawing_itemsizes",sim_drawing_itemsizes},
    {"sim_drawing_backfaceculling",sim_drawing_backfaceculling},
    {"sim_drawing_wireframe",sim_drawing_wireframe},
    {"sim_drawing_painttag",sim_drawing_painttag},
    {"sim_drawing_followparentvisibility",sim_drawing_followparentvisibility},
    {"sim_drawing_cyclic",sim_drawing_cyclic},
    {"sim_drawing_50percenttransparency",sim_drawing_50percenttransparency},
    {"sim_drawing_25percenttransparency",sim_drawing_25percenttransparency},
    {"sim_drawing_12percenttransparency",sim_drawing_12percenttransparency},
    {"sim_drawing_emissioncolor",sim_drawing_emissioncolor},
    {"sim_drawing_facingcamera",sim_drawing_facingcamera},
    {"sim_drawing_overlay",sim_drawing_overlay},
    {"sim_drawing_itemtransparency",sim_drawing_itemtransparency},
    {"sim_drawing_persistent",sim_drawing_persistent},
    {"sim_drawing_auxchannelcolor1",sim_drawing_auxchannelcolor1},
    {"sim_drawing_auxchannelcolor2",sim_drawing_auxchannelcolor2},

    
    // Banners
    {"sim_banner_left",sim_banner_left},
    {"sim_banner_right",sim_banner_right},
    {"sim_banner_nobackground",sim_banner_nobackground},
    {"sim_banner_overlay",sim_banner_overlay},
    {"sim_banner_followparentvisibility",sim_banner_followparentvisibility},
    {"sim_banner_clickselectsparent",sim_banner_clickselectsparent},
    {"sim_banner_clicktriggersevent",sim_banner_clicktriggersevent},
    {"sim_banner_facingcamera",sim_banner_facingcamera},
    {"sim_banner_fullyfacingcamera",sim_banner_fullyfacingcamera},
    {"sim_banner_backfaceculling",sim_banner_backfaceculling},
    {"sim_banner_keepsamesize",sim_banner_keepsamesize},
    {"sim_banner_bitmapfont",sim_banner_bitmapfont},
    
    // particle objects
    {"sim_particle_points1",sim_particle_points1},
    {"sim_particle_points2",sim_particle_points2},
    {"sim_particle_points4",sim_particle_points4},
    {"sim_particle_roughspheres",sim_particle_roughspheres},
    {"sim_particle_spheres",sim_particle_spheres},
    {"sim_particle_respondable1to4",sim_particle_respondable1to4},
    {"sim_particle_respondable5to8",sim_particle_respondable5to8},
    {"sim_particle_particlerespondable",sim_particle_particlerespondable},
    {"sim_particle_ignoresgravity",sim_particle_ignoresgravity},
    {"sim_particle_invisible",sim_particle_invisible},
    {"sim_particle_painttag",sim_particle_painttag},
    {"sim_particle_itemsizes",sim_particle_itemsizes},
    {"sim_particle_itemdensities",sim_particle_itemdensities},
    {"sim_particle_itemcolors",sim_particle_itemcolors},
    {"sim_particle_cyclic",sim_particle_cyclic},
    {"sim_particle_emissioncolor",sim_particle_emissioncolor},
    {"sim_particle_water",sim_particle_water},

    // joint modes
    {"sim_jointmode_passive",sim_jointmode_passive},
    {"sim_jointmode_ik",sim_jointmode_ik},
    {"sim_jointmode_ikdependent",sim_jointmode_reserved_previously_ikdependent},
    {"sim_jointmode_dependent",sim_jointmode_dependent},
    {"sim_jointmode_force",sim_jointmode_force},

    // file dialog styles
    {"sim_filedlg_type_load",sim_filedlg_type_load},
    {"sim_filedlg_type_save",sim_filedlg_type_save},
    {"sim_filedlg_type_load_multiple",sim_filedlg_type_load_multiple},
    {"sim_filedlg_type_folder",sim_filedlg_type_folder},

    // message box styles
    {"sim_msgbox_type_info",sim_msgbox_type_info},
    {"sim_msgbox_type_question",sim_msgbox_type_question},
    {"sim_msgbox_type_warning",sim_msgbox_type_warning},
    {"sim_msgbox_type_critical",sim_msgbox_type_critical},

    // message box buttons
    {"sim_msgbox_buttons_ok",sim_msgbox_buttons_ok},
    {"sim_msgbox_buttons_yesno",sim_msgbox_buttons_yesno},
    {"sim_msgbox_buttons_yesnocancel",sim_msgbox_buttons_yesnocancel},
    {"sim_msgbox_buttons_okcancel",sim_msgbox_buttons_okcancel},

    // message box return values
    {"sim_msgbox_return_cancel",sim_msgbox_return_cancel},
    {"sim_msgbox_return_no",sim_msgbox_return_no},
    {"sim_msgbox_return_yes",sim_msgbox_return_yes},
    {"sim_msgbox_return_ok",sim_msgbox_return_ok},
    {"sim_msgbox_return_error",sim_msgbox_return_error},

    // physics engine
    {"sim_physics_bullet",sim_physics_bullet},
    {"sim_physics_ode",sim_physics_ode},
    {"sim_physics_vortex",sim_physics_vortex},
    {"sim_physics_newton",sim_physics_newton},

    // pure primitives type
    {"sim_pure_primitive_none",sim_pure_primitive_none},
    {"sim_pure_primitive_plane",sim_pure_primitive_plane},
    {"sim_pure_primitive_disc",sim_pure_primitive_disc},
    {"sim_pure_primitive_cuboid",sim_pure_primitive_cuboid},
    {"sim_pure_primitive_spheroid",sim_pure_primitive_spheroid},
    {"sim_pure_primitive_cylinder",sim_pure_primitive_cylinder},
    {"sim_pure_primitive_cone",sim_pure_primitive_cone},
    {"sim_pure_primitive_heightfield",sim_pure_primitive_heightfield},


    // dummy-dummy link types
    {"sim_dummy_linktype_dynamics_loop_closure",sim_dummy_linktype_dynamics_loop_closure},
    {"sim_dummy_linktype_dynamics_force_constraint",sim_dummy_linktype_dynamics_force_constraint},
    {"sim_dummy_linktype_gcs_loop_closure",sim_dummy_linktype_gcs_loop_closure},
    {"sim_dummy_linktype_gcs_tip",sim_dummy_linktype_gcs_tip},
    {"sim_dummy_linktype_gcs_target",sim_dummy_linktype_gcs_target},
    {"sim_dummy_linktype_ik_tip_target",sim_dummy_linktype_ik_tip_target},


    // color components
    {"sim_colorcomponent_ambient",sim_colorcomponent_ambient},
    {"sim_colorcomponent_ambient_diffuse",sim_colorcomponent_ambient_diffuse},
    {"sim_colorcomponent_diffuse",sim_colorcomponent_diffuse},
    {"sim_colorcomponent_specular",sim_colorcomponent_specular},
    {"sim_colorcomponent_emission",sim_colorcomponent_emission},
    {"sim_colorcomponent_transparency",sim_colorcomponent_transparency},
    {"sim_colorcomponent_auxiliary",sim_colorcomponent_auxiliary},

    // volume types
    {"sim_volume_ray",sim_volume_ray},
    {"sim_volume_randomizedray",sim_volume_randomizedray},
    {"sim_volume_pyramid",sim_volume_pyramid},
    {"sim_volume_cylinder",sim_volume_cylinder},
    {"sim_volume_disc",sim_volume_disc},
    {"sim_volume_cone",sim_volume_cone},

    // Object int/float/string parameters
    // scene objects
    {"sim_objintparam_visibility_layer",sim_objintparam_visibility_layer},
    {"sim_objfloatparam_abs_x_velocity",sim_objfloatparam_abs_x_velocity},
    {"sim_objfloatparam_abs_y_velocity",sim_objfloatparam_abs_y_velocity},
    {"sim_objfloatparam_abs_z_velocity",sim_objfloatparam_abs_z_velocity},
    {"sim_objfloatparam_abs_rot_velocity",sim_objfloatparam_abs_rot_velocity},
    {"sim_objfloatparam_objbbox_min_x",sim_objfloatparam_objbbox_min_x},
    {"sim_objfloatparam_objbbox_min_y",sim_objfloatparam_objbbox_min_y},
    {"sim_objfloatparam_objbbox_min_z",sim_objfloatparam_objbbox_min_z},
    {"sim_objfloatparam_objbbox_max_x",sim_objfloatparam_objbbox_max_x},
    {"sim_objfloatparam_objbbox_max_y",sim_objfloatparam_objbbox_max_y},
    {"sim_objfloatparam_objbbox_max_z",sim_objfloatparam_objbbox_max_z},
    {"sim_objfloatparam_modelbbox_min_x",sim_objfloatparam_modelbbox_min_x},
    {"sim_objfloatparam_modelbbox_min_y",sim_objfloatparam_modelbbox_min_y},
    {"sim_objfloatparam_modelbbox_min_z",sim_objfloatparam_modelbbox_min_z},
    {"sim_objfloatparam_modelbbox_max_x",sim_objfloatparam_modelbbox_max_x},
    {"sim_objfloatparam_modelbbox_max_y",sim_objfloatparam_modelbbox_max_y},
    {"sim_objfloatparam_modelbbox_max_z",sim_objfloatparam_modelbbox_max_z},
    {"sim_objintparam_collection_self_collision_indicator",sim_objintparam_collection_self_collision_indicator},
    {"sim_objfloatparam_transparency_offset",sim_objfloatparam_transparency_offset},
    {"sim_objintparam_child_role",sim_objintparam_child_role},
    {"sim_objintparam_parent_role",sim_objintparam_parent_role},
    {"sim_objintparam_manipulation_permissions",sim_objintparam_manipulation_permissions},
    {"sim_objintparam_illumination_handle",sim_objintparam_illumination_handle},
    {"sim_objstringparam_dna",sim_objstringparam_dna},
    // vision_sensors
    {"sim_visionfloatparam_near_clipping",sim_visionfloatparam_near_clipping},
    {"sim_visionfloatparam_far_clipping",sim_visionfloatparam_far_clipping},
    {"sim_visionintparam_resolution_x",sim_visionintparam_resolution_x},
    {"sim_visionintparam_resolution_y",sim_visionintparam_resolution_y},
    {"sim_visionfloatparam_perspective_angle",sim_visionfloatparam_perspective_angle},
    {"sim_visionfloatparam_ortho_size",sim_visionfloatparam_ortho_size},
    {"sim_visionintparam_disabled_light_components",sim_visionintparam_disabled_light_components},
    {"sim_visionintparam_rendering_attributes",sim_visionintparam_rendering_attributes},
    {"sim_visionintparam_entity_to_render",sim_visionintparam_entity_to_render},
    {"sim_visionintparam_windowed_size_x",sim_visionintparam_windowed_size_x},
    {"sim_visionintparam_windowed_size_y",sim_visionintparam_windowed_size_y},
    {"sim_visionintparam_windowed_pos_x",sim_visionintparam_windowed_pos_x},
    {"sim_visionintparam_windowed_pos_y",sim_visionintparam_windowed_pos_y},
    {"sim_visionintparam_pov_focal_blur",sim_visionintparam_pov_focal_blur},
    {"sim_visionfloatparam_pov_blur_distance",sim_visionfloatparam_pov_blur_distance},
    {"sim_visionfloatparam_pov_aperture",sim_visionfloatparam_pov_aperture},
    {"sim_visionintparam_pov_blur_sampled",sim_visionintparam_pov_blur_sampled},
    {"sim_visionintparam_render_mode",sim_visionintparam_render_mode},
    // joints
    {"sim_jointintparam_motor_enabled",sim_jointintparam_motor_enabled},
    {"sim_jointintparam_ctrl_enabled",sim_jointintparam_ctrl_enabled},
    {"sim_jointfloatparam_pid_p",sim_jointfloatparam_pid_p},
    {"sim_jointfloatparam_pid_i",sim_jointfloatparam_pid_i},
    {"sim_jointfloatparam_pid_d",sim_jointfloatparam_pid_d},
    {"sim_jointfloatparam_intrinsic_x",sim_jointfloatparam_intrinsic_x},
    {"sim_jointfloatparam_intrinsic_y",sim_jointfloatparam_intrinsic_y},
    {"sim_jointfloatparam_intrinsic_z",sim_jointfloatparam_intrinsic_z},
    {"sim_jointfloatparam_intrinsic_qx",sim_jointfloatparam_intrinsic_qx},
    {"sim_jointfloatparam_intrinsic_qy",sim_jointfloatparam_intrinsic_qy},
    {"sim_jointfloatparam_intrinsic_qz",sim_jointfloatparam_intrinsic_qz},
    {"sim_jointfloatparam_intrinsic_qw",sim_jointfloatparam_intrinsic_qw},
    {"sim_jointfloatparam_velocity",sim_jointfloatparam_velocity},
    {"sim_jointfloatparam_spherical_qx",sim_jointfloatparam_spherical_qx},
    {"sim_jointfloatparam_spherical_qy",sim_jointfloatparam_spherical_qy},
    {"sim_jointfloatparam_spherical_qz",sim_jointfloatparam_spherical_qz},
    {"sim_jointfloatparam_spherical_qw",sim_jointfloatparam_spherical_qw},
    {"sim_jointfloatparam_upper_limit",sim_jointfloatparam_upper_limit},
    {"sim_jointfloatparam_kc_k",sim_jointfloatparam_kc_k},
    {"sim_jointfloatparam_kc_c",sim_jointfloatparam_kc_c},
    {"sim_jointfloatparam_ik_weight",sim_jointfloatparam_ik_weight},
    {"sim_jointfloatparam_error_x",sim_jointfloatparam_error_x},
    {"sim_jointfloatparam_error_y",sim_jointfloatparam_error_y},
    {"sim_jointfloatparam_error_z",sim_jointfloatparam_error_z},
    {"sim_jointfloatparam_error_a",sim_jointfloatparam_error_a},
    {"sim_jointfloatparam_error_b",sim_jointfloatparam_error_b},
    {"sim_jointfloatparam_error_g",sim_jointfloatparam_error_g},
    {"sim_jointfloatparam_error_pos",sim_jointfloatparam_error_pos},
    {"sim_jointfloatparam_error_angle",sim_jointfloatparam_error_angle},
    {"sim_jointintparam_velocity_lock",sim_jointintparam_velocity_lock},
    {"sim_jointintparam_vortex_dep_handle",sim_jointintparam_vortex_dep_handle},
    {"sim_jointfloatparam_vortex_dep_multiplication",sim_jointfloatparam_vortex_dep_multiplication},
    {"sim_jointfloatparam_vortex_dep_offset",sim_jointfloatparam_vortex_dep_offset},
    // shapes
    {"sim_shapefloatparam_init_velocity_x",sim_shapefloatparam_init_velocity_x},
    {"sim_shapefloatparam_init_velocity_y",sim_shapefloatparam_init_velocity_y},
    {"sim_shapefloatparam_init_velocity_z",sim_shapefloatparam_init_velocity_z},
    {"sim_shapeintparam_static",sim_shapeintparam_static},
    {"sim_shapeintparam_respondable",sim_shapeintparam_respondable},
    {"sim_shapefloatparam_mass",sim_shapefloatparam_mass},
    {"sim_shapefloatparam_texture_x",sim_shapefloatparam_texture_x},
    {"sim_shapefloatparam_texture_y",sim_shapefloatparam_texture_y},
    {"sim_shapefloatparam_texture_z",sim_shapefloatparam_texture_z},
    {"sim_shapefloatparam_texture_a",sim_shapefloatparam_texture_a},
    {"sim_shapefloatparam_texture_b",sim_shapefloatparam_texture_b},
    {"sim_shapefloatparam_texture_g",sim_shapefloatparam_texture_g},
    {"sim_shapefloatparam_texture_scaling_x",sim_shapefloatparam_texture_scaling_x},
    {"sim_shapefloatparam_texture_scaling_y",sim_shapefloatparam_texture_scaling_y},
    {"sim_shapeintparam_culling",sim_shapeintparam_culling},
    {"sim_shapeintparam_wireframe",sim_shapeintparam_wireframe},
    {"sim_shapeintparam_compound",sim_shapeintparam_compound},
    {"sim_shapeintparam_convex",sim_shapeintparam_convex},
    {"sim_shapeintparam_convex_check",sim_shapeintparam_convex_check},
    {"sim_shapeintparam_respondable_mask",sim_shapeintparam_respondable_mask},
    {"sim_shapefloatparam_init_velocity_a",sim_shapefloatparam_init_velocity_a},
    {"sim_shapefloatparam_init_velocity_b",sim_shapefloatparam_init_velocity_b},
    {"sim_shapefloatparam_init_velocity_g",sim_shapefloatparam_init_velocity_g},
    {"sim_shapestringparam_color_name",sim_shapestringparam_color_name},
    {"sim_shapeintparam_edge_visibility",sim_shapeintparam_edge_visibility},
    {"sim_shapefloatparam_shading_angle",sim_shapefloatparam_shading_angle},
    {"sim_shapefloatparam_edge_angle",sim_shapefloatparam_edge_angle},
    {"sim_shapeintparam_edge_borders_hidden",sim_shapeintparam_edge_borders_hidden},
    // proximity sensors
    {"sim_proxintparam_ray_invisibility",sim_proxintparam_ray_invisibility},
    {"sim_proxintparam_volume_type",sim_proxintparam_volume_type},
    {"sim_proxintparam_entity_to_detect",sim_proxintparam_entity_to_detect},
    // proximity sensors
    {"sim_forcefloatparam_error_x",sim_forcefloatparam_error_x},
    {"sim_forcefloatparam_error_y",sim_forcefloatparam_error_y},
    {"sim_forcefloatparam_error_z",sim_forcefloatparam_error_z},
    {"sim_forcefloatparam_error_a",sim_forcefloatparam_error_a},
    {"sim_forcefloatparam_error_b",sim_forcefloatparam_error_b},
    {"sim_forcefloatparam_error_g",sim_forcefloatparam_error_g},
    {"sim_forcefloatparam_error_pos",sim_forcefloatparam_error_pos},
    {"sim_forcefloatparam_error_angle",sim_forcefloatparam_error_angle},
    // lights
    {"sim_lightintparam_pov_casts_shadows",sim_lightintparam_pov_casts_shadows},
    // cameras
    {"sim_cameraintparam_disabled_light_components",sim_cameraintparam_disabled_light_components},
    {"sim_camerafloatparam_perspective_angle",sim_camerafloatparam_perspective_angle},
    {"sim_camerafloatparam_ortho_size",sim_camerafloatparam_ortho_size},
    {"sim_cameraintparam_rendering_attributes",sim_cameraintparam_rendering_attributes},
    {"sim_cameraintparam_pov_focal_blur",sim_cameraintparam_pov_focal_blur},
    {"sim_camerafloatparam_pov_blur_distance",sim_camerafloatparam_pov_blur_distance},
    {"sim_camerafloatparam_pov_aperture",sim_camerafloatparam_pov_aperture},
    {"sim_cameraintparam_pov_blur_samples",sim_cameraintparam_pov_blur_samples},
    // dummies
    {"sim_dummyintparam_link_type",sim_dummyintparam_link_type},
    {"sim_dummyintparam_follow_path",sim_dummyintparam_follow_path},
    {"sim_dummyfloatparam_follow_path_offset",sim_dummyfloatparam_follow_path_offset},
    // mills
    {"sim_millintparam_volume_type",sim_millintparam_volume_type},
    // mirrors
    {"sim_mirrorfloatparam_width",sim_mirrorfloatparam_width},
    {"sim_mirrorfloatparam_height",sim_mirrorfloatparam_height},
    {"sim_mirrorfloatparam_reflectance",sim_mirrorfloatparam_reflectance},
    {"sim_mirrorintparam_enable",sim_mirrorintparam_enable},
    // path planning
    {"sim_pplanfloatparam_x_min",sim_pplanfloatparam_x_min},
    {"sim_pplanfloatparam_x_range",sim_pplanfloatparam_x_range},
    {"sim_pplanfloatparam_y_min",sim_pplanfloatparam_y_min},
    {"sim_pplanfloatparam_y_range",sim_pplanfloatparam_y_range},
    {"sim_pplanfloatparam_z_min",sim_pplanfloatparam_z_min},
    {"sim_pplanfloatparam_z_range",sim_pplanfloatparam_z_range},
    {"sim_pplanfloatparam_delta_min",sim_pplanfloatparam_delta_min},
    {"sim_pplanfloatparam_delta_range",sim_pplanfloatparam_delta_range},
    // motion planning
    {"sim_mplanintparam_nodes_computed",sim_mplanintparam_nodes_computed},
    {"sim_mplanintparam_prepare_nodes",sim_mplanintparam_prepare_nodes},
    {"sim_mplanintparam_clear_nodes",sim_mplanintparam_clear_nodes},
    // Bullet engine params
    {"sim_bullet_global_stepsize",sim_bullet_global_stepsize},
    {"sim_bullet_global_internalscalingfactor",sim_bullet_global_internalscalingfactor},
    {"sim_bullet_global_collisionmarginfactor",sim_bullet_global_collisionmarginfactor},
    {"sim_bullet_global_constraintsolvingiterations",sim_bullet_global_constraintsolvingiterations},
    {"sim_bullet_global_bitcoded",sim_bullet_global_bitcoded},
    {"sim_bullet_global_constraintsolvertype",sim_bullet_global_constraintsolvertype},
    {"sim_bullet_global_fullinternalscaling",sim_bullet_global_fullinternalscaling},
    {"sim_bullet_joint_stoperp",sim_bullet_joint_stoperp},
    {"sim_bullet_joint_stopcfm",sim_bullet_joint_stopcfm},
    {"sim_bullet_joint_normalcfm",sim_bullet_joint_normalcfm},
    {"sim_bullet_body_restitution",sim_bullet_body_restitution},
    {"sim_bullet_body_oldfriction",sim_bullet_body_oldfriction},
    {"sim_bullet_body_friction",sim_bullet_body_friction},
    {"sim_bullet_body_lineardamping",sim_bullet_body_lineardamping},
    {"sim_bullet_body_angulardamping",sim_bullet_body_angulardamping},
    {"sim_bullet_body_nondefaultcollisionmargingfactor",sim_bullet_body_nondefaultcollisionmargingfactor},
    {"sim_bullet_body_nondefaultcollisionmargingfactorconvex",sim_bullet_body_nondefaultcollisionmargingfactorconvex},
    {"sim_bullet_body_bitcoded",sim_bullet_body_bitcoded},
    {"sim_bullet_body_sticky",sim_bullet_body_sticky},
    {"sim_bullet_body_usenondefaultcollisionmargin",sim_bullet_body_usenondefaultcollisionmargin},
    {"sim_bullet_body_usenondefaultcollisionmarginconvex",sim_bullet_body_usenondefaultcollisionmarginconvex},
    {"sim_bullet_body_autoshrinkconvex",sim_bullet_body_autoshrinkconvex},
    // Ode engine params
    {"sim_ode_global_stepsize",sim_ode_global_stepsize},
    {"sim_ode_global_internalscalingfactor",sim_ode_global_internalscalingfactor},
    {"sim_ode_global_cfm",sim_ode_global_cfm},
    {"sim_ode_global_erp",sim_ode_global_erp},
    {"sim_ode_global_constraintsolvingiterations",sim_ode_global_constraintsolvingiterations},
    {"sim_ode_global_bitcoded",sim_ode_global_bitcoded},
    {"sim_ode_global_randomseed",sim_ode_global_randomseed},
    {"sim_ode_global_fullinternalscaling",sim_ode_global_fullinternalscaling},
    {"sim_ode_global_quickstep",sim_ode_global_quickstep},
    {"sim_ode_joint_stoperp",sim_ode_joint_stoperp},
    {"sim_ode_joint_stopcfm",sim_ode_joint_stopcfm},
    {"sim_ode_joint_bounce",sim_ode_joint_bounce},
    {"sim_ode_joint_fudgefactor",sim_ode_joint_fudgefactor},
    {"sim_ode_joint_normalcfm",sim_ode_joint_normalcfm},
    {"sim_ode_body_friction",sim_ode_body_friction},
    {"sim_ode_body_softerp",sim_ode_body_softerp},
    {"sim_ode_body_softcfm",sim_ode_body_softcfm},
    {"sim_ode_body_lineardamping",sim_ode_body_lineardamping},
    {"sim_ode_body_angulardamping",sim_ode_body_angulardamping},
    {"sim_ode_body_maxcontacts",sim_ode_body_maxcontacts},
    // Vortex engine params
    {"sim_vortex_global_stepsize",sim_vortex_global_stepsize},
    {"sim_vortex_global_internalscalingfactor",sim_vortex_global_internalscalingfactor},
    {"sim_vortex_global_contacttolerance",sim_vortex_global_contacttolerance},
    {"sim_vortex_global_constraintlinearcompliance",sim_vortex_global_constraintlinearcompliance},
    {"sim_vortex_global_constraintlineardamping",sim_vortex_global_constraintlineardamping},
    {"sim_vortex_global_constraintlinearkineticloss",sim_vortex_global_constraintlinearkineticloss},
    {"sim_vortex_global_constraintangularcompliance",sim_vortex_global_constraintangularcompliance},
    {"sim_vortex_global_constraintangulardamping",sim_vortex_global_constraintangulardamping},
    {"sim_vortex_global_constraintangularkineticloss",sim_vortex_global_constraintangularkineticloss},
    {"sim_vortex_global_bitcoded",sim_vortex_global_bitcoded},
    {"sim_vortex_global_autosleep",sim_vortex_global_autosleep},
    {"sim_vortex_global_multithreading",sim_vortex_global_multithreading},
    {"sim_vortex_joint_lowerlimitdamping",sim_vortex_joint_lowerlimitdamping},
    {"sim_vortex_joint_upperlimitdamping",sim_vortex_joint_upperlimitdamping},
    {"sim_vortex_joint_lowerlimitstiffness",sim_vortex_joint_lowerlimitstiffness},
    {"sim_vortex_joint_upperlimitstiffness",sim_vortex_joint_upperlimitstiffness},
    {"sim_vortex_joint_lowerlimitrestitution",sim_vortex_joint_lowerlimitrestitution},
    {"sim_vortex_joint_upperlimitrestitution",sim_vortex_joint_upperlimitrestitution},
    {"sim_vortex_joint_lowerlimitmaxforce",sim_vortex_joint_lowerlimitmaxforce},
    {"sim_vortex_joint_upperlimitmaxforce",sim_vortex_joint_upperlimitmaxforce},
    {"sim_vortex_joint_motorconstraintfrictioncoeff",sim_vortex_joint_motorconstraintfrictioncoeff},
    {"sim_vortex_joint_motorconstraintfrictionmaxforce",sim_vortex_joint_motorconstraintfrictionmaxforce},
    {"sim_vortex_joint_motorconstraintfrictionloss",sim_vortex_joint_motorconstraintfrictionloss},
    {"sim_vortex_joint_p0loss",sim_vortex_joint_p0loss},
    {"sim_vortex_joint_p0stiffness",sim_vortex_joint_p0stiffness},
    {"sim_vortex_joint_p0damping",sim_vortex_joint_p0damping},
    {"sim_vortex_joint_p0frictioncoeff",sim_vortex_joint_p0frictioncoeff},
    {"sim_vortex_joint_p0frictionmaxforce",sim_vortex_joint_p0frictionmaxforce},
    {"sim_vortex_joint_p0frictionloss",sim_vortex_joint_p0frictionloss},
    {"sim_vortex_joint_p1loss",sim_vortex_joint_p1loss},
    {"sim_vortex_joint_p1stiffness",sim_vortex_joint_p1stiffness},
    {"sim_vortex_joint_p1damping",sim_vortex_joint_p1damping},
    {"sim_vortex_joint_p1frictioncoeff",sim_vortex_joint_p1frictioncoeff},
    {"sim_vortex_joint_p1frictionmaxforce",sim_vortex_joint_p1frictionmaxforce},
    {"sim_vortex_joint_p1frictionloss",sim_vortex_joint_p1frictionloss},
    {"sim_vortex_joint_p2loss",sim_vortex_joint_p2loss},
    {"sim_vortex_joint_p2stiffness",sim_vortex_joint_p2stiffness},
    {"sim_vortex_joint_p2damping",sim_vortex_joint_p2damping},
    {"sim_vortex_joint_p2frictioncoeff",sim_vortex_joint_p2frictioncoeff},
    {"sim_vortex_joint_p2frictionmaxforce",sim_vortex_joint_p2frictionmaxforce},
    {"sim_vortex_joint_p2frictionloss",sim_vortex_joint_p2frictionloss},
    {"sim_vortex_joint_a0loss",sim_vortex_joint_a0loss},
    {"sim_vortex_joint_a0stiffness",sim_vortex_joint_a0stiffness},
    {"sim_vortex_joint_a0damping",sim_vortex_joint_a0damping},
    {"sim_vortex_joint_a0frictioncoeff",sim_vortex_joint_a0frictioncoeff},
    {"sim_vortex_joint_a0frictionmaxforce",sim_vortex_joint_a0frictionmaxforce},
    {"sim_vortex_joint_a0frictionloss",sim_vortex_joint_a0frictionloss},
    {"sim_vortex_joint_a1loss",sim_vortex_joint_a1loss},
    {"sim_vortex_joint_a1stiffness",sim_vortex_joint_a1stiffness},
    {"sim_vortex_joint_a1damping",sim_vortex_joint_a1damping},
    {"sim_vortex_joint_a1frictioncoeff",sim_vortex_joint_a1frictioncoeff},
    {"sim_vortex_joint_a1frictionmaxforce",sim_vortex_joint_a1frictionmaxforce},
    {"sim_vortex_joint_a1frictionloss",sim_vortex_joint_a1frictionloss},
    {"sim_vortex_joint_a2loss",sim_vortex_joint_a2loss},
    {"sim_vortex_joint_a2stiffness",sim_vortex_joint_a2stiffness},
    {"sim_vortex_joint_a2damping",sim_vortex_joint_a2damping},
    {"sim_vortex_joint_a2frictioncoeff",sim_vortex_joint_a2frictioncoeff},
    {"sim_vortex_joint_a2frictionmaxforce",sim_vortex_joint_a2frictionmaxforce},
    {"sim_vortex_joint_a2frictionloss",sim_vortex_joint_a2frictionloss},
    {"sim_vortex_joint_dependencyfactor",sim_vortex_joint_dependencyfactor},
    {"sim_vortex_joint_dependencyoffset",sim_vortex_joint_dependencyoffset},
    {"sim_vortex_joint_bitcoded",sim_vortex_joint_bitcoded},
    {"sim_vortex_joint_relaxationenabledbc",sim_vortex_joint_relaxationenabledbc},
    {"sim_vortex_joint_frictionenabledbc",sim_vortex_joint_frictionenabledbc},
    {"sim_vortex_joint_frictionproportionalbc",sim_vortex_joint_frictionproportionalbc},
    {"sim_vortex_joint_objectid",sim_vortex_joint_objectid},
    {"sim_vortex_joint_dependentobjectid",sim_vortex_joint_dependentobjectid},
    {"sim_vortex_joint_motorfrictionenabled",sim_vortex_joint_motorfrictionenabled},
    {"sim_vortex_joint_proportionalmotorfriction",sim_vortex_joint_proportionalmotorfriction},
    {"sim_vortex_body_primlinearaxisfriction",sim_vortex_body_primlinearaxisfriction},
    {"sim_vortex_body_seclinearaxisfriction",sim_vortex_body_seclinearaxisfriction},
    {"sim_vortex_body_primangularaxisfriction",sim_vortex_body_primangularaxisfriction},
    {"sim_vortex_body_secangularaxisfriction",sim_vortex_body_secangularaxisfriction},
    {"sim_vortex_body_normalangularaxisfriction",sim_vortex_body_normalangularaxisfriction},
    {"sim_vortex_body_primlinearaxisstaticfrictionscale",sim_vortex_body_primlinearaxisstaticfrictionscale},
    {"sim_vortex_body_seclinearaxisstaticfrictionscale",sim_vortex_body_seclinearaxisstaticfrictionscale},
    {"sim_vortex_body_primangularaxisstaticfrictionscale",sim_vortex_body_primangularaxisstaticfrictionscale},
    {"sim_vortex_body_secangularaxisstaticfrictionscale",sim_vortex_body_secangularaxisstaticfrictionscale},
    {"sim_vortex_body_normalangularaxisstaticfrictionscale",sim_vortex_body_normalangularaxisstaticfrictionscale},
    {"sim_vortex_body_compliance",sim_vortex_body_compliance},
    {"sim_vortex_body_damping",sim_vortex_body_damping},
    {"sim_vortex_body_restitution",sim_vortex_body_restitution},
    {"sim_vortex_body_restitutionthreshold",sim_vortex_body_restitutionthreshold},
    {"sim_vortex_body_adhesiveforce",sim_vortex_body_adhesiveforce},
    {"sim_vortex_body_linearvelocitydamping",sim_vortex_body_linearvelocitydamping},
    {"sim_vortex_body_angularvelocitydamping",sim_vortex_body_angularvelocitydamping},
    {"sim_vortex_body_primlinearaxisslide",sim_vortex_body_primlinearaxisslide},
    {"sim_vortex_body_seclinearaxisslide",sim_vortex_body_seclinearaxisslide},
    {"sim_vortex_body_primangularaxisslide",sim_vortex_body_primangularaxisslide},
    {"sim_vortex_body_secangularaxisslide",sim_vortex_body_secangularaxisslide},
    {"sim_vortex_body_normalangularaxisslide",sim_vortex_body_normalangularaxisslide},
    {"sim_vortex_body_primlinearaxisslip",sim_vortex_body_primlinearaxisslip},
    {"sim_vortex_body_seclinearaxisslip",sim_vortex_body_seclinearaxisslip},
    {"sim_vortex_body_primangularaxisslip",sim_vortex_body_primangularaxisslip},
    {"sim_vortex_body_secangularaxisslip",sim_vortex_body_secangularaxisslip},
    {"sim_vortex_body_normalangularaxisslip",sim_vortex_body_normalangularaxisslip},
    {"sim_vortex_body_autosleeplinearspeedthreshold",sim_vortex_body_autosleeplinearspeedthreshold},
    {"sim_vortex_body_autosleeplinearaccelthreshold",sim_vortex_body_autosleeplinearaccelthreshold},
    {"sim_vortex_body_autosleepangularspeedthreshold",sim_vortex_body_autosleepangularspeedthreshold},
    {"sim_vortex_body_autosleepangularaccelthreshold",sim_vortex_body_autosleepangularaccelthreshold},
    {"sim_vortex_body_skinthickness",sim_vortex_body_skinthickness},
    {"sim_vortex_body_autoangulardampingtensionratio",sim_vortex_body_autoangulardampingtensionratio},
    {"sim_vortex_body_primaxisvectorx",sim_vortex_body_primaxisvectorx},
    {"sim_vortex_body_primaxisvectory",sim_vortex_body_primaxisvectory},
    {"sim_vortex_body_primaxisvectorz",sim_vortex_body_primaxisvectorz},
    {"sim_vortex_body_primlinearaxisfrictionmodel",sim_vortex_body_primlinearaxisfrictionmodel},
    {"sim_vortex_body_seclinearaxisfrictionmodel",sim_vortex_body_seclinearaxisfrictionmodel},
    {"sim_vortex_body_primangulararaxisfrictionmodel",sim_vortex_body_primangulararaxisfrictionmodel},
    {"sim_vortex_body_secmangulararaxisfrictionmodel",sim_vortex_body_secmangulararaxisfrictionmodel},
    {"sim_vortex_body_normalmangulararaxisfrictionmodel",sim_vortex_body_normalmangulararaxisfrictionmodel},
    {"sim_vortex_body_bitcoded",sim_vortex_body_bitcoded},
    {"sim_vortex_body_autosleepsteplivethreshold",sim_vortex_body_autosleepsteplivethreshold},
    {"sim_vortex_body_materialuniqueid",sim_vortex_body_materialuniqueid},
    {"sim_vortex_body_pureshapesasconvex",sim_vortex_body_pureshapesasconvex},
    {"sim_vortex_body_convexshapesasrandom",sim_vortex_body_convexshapesasrandom},
    {"sim_vortex_body_randomshapesasterrain",sim_vortex_body_randomshapesasterrain},
    {"sim_vortex_body_fastmoving",sim_vortex_body_fastmoving},
    {"sim_vortex_body_autoslip",sim_vortex_body_autoslip},
    {"sim_vortex_body_seclinaxissameasprimlinaxis",sim_vortex_body_seclinaxissameasprimlinaxis},
    {"sim_vortex_body_secangaxissameasprimangaxis",sim_vortex_body_secangaxissameasprimangaxis},
    {"sim_vortex_body_normangaxissameasprimangaxis",sim_vortex_body_normangaxissameasprimangaxis},
    {"sim_vortex_body_autoangulardamping",sim_vortex_body_autoangulardamping},
    // Newton engine params
    {"sim_newton_global_stepsize",sim_newton_global_stepsize},
    {"sim_newton_global_contactmergetolerance",sim_newton_global_contactmergetolerance},
    {"sim_newton_global_constraintsolvingiterations",sim_newton_global_constraintsolvingiterations},
    {"sim_newton_global_bitcoded",sim_newton_global_bitcoded},
    {"sim_newton_global_multithreading",sim_newton_global_multithreading},
    {"sim_newton_global_exactsolver",sim_newton_global_exactsolver},
    {"sim_newton_global_highjointaccuracy",sim_newton_global_highjointaccuracy},
    {"sim_newton_joint_dependencyfactor",sim_newton_joint_dependencyfactor},
    {"sim_newton_joint_dependencyoffset",sim_newton_joint_dependencyoffset},
    {"sim_newton_joint_objectid",sim_newton_joint_objectid},
    {"sim_newton_joint_dependentobjectid",sim_newton_joint_dependentobjectid},
    {"sim_newton_body_staticfriction",sim_newton_body_staticfriction},
    {"sim_newton_body_kineticfriction",sim_newton_body_kineticfriction},
    {"sim_newton_body_restitution",sim_newton_body_restitution},
    {"sim_newton_body_lineardrag",sim_newton_body_lineardrag},
    {"sim_newton_body_angulardrag",sim_newton_body_angulardrag},
    {"sim_newton_body_bitcoded",sim_newton_body_bitcoded},
    {"sim_newton_body_fastmoving",sim_newton_body_fastmoving},
    // Vortex friction models
    {"sim_vortex_bodyfrictionmodel_box",sim_vortex_bodyfrictionmodel_box},
    {"sim_vortex_bodyfrictionmodel_scaledbox",sim_vortex_bodyfrictionmodel_scaledbox},
    {"sim_vortex_bodyfrictionmodel_proplow",sim_vortex_bodyfrictionmodel_proplow},
    {"sim_vortex_bodyfrictionmodel_prophigh",sim_vortex_bodyfrictionmodel_prophigh},
    {"sim_vortex_bodyfrictionmodel_scaledboxfast",sim_vortex_bodyfrictionmodel_scaledboxfast},
    {"sim_vortex_bodyfrictionmodel_neutral",sim_vortex_bodyfrictionmodel_neutral},
    {"sim_vortex_bodyfrictionmodel_none",sim_vortex_bodyfrictionmodel_none},
    // Bullet constraint solver types
    {"sim_bullet_constraintsolvertype_sequentialimpulse",sim_bullet_constraintsolvertype_sequentialimpulse},
    {"sim_bullet_constraintsolvertype_nncg",sim_bullet_constraintsolvertype_nncg},
    {"sim_bullet_constraintsolvertype_dantzig",sim_bullet_constraintsolvertype_dantzig},
    {"sim_bullet_constraintsolvertype_projectedgaussseidel",sim_bullet_constraintsolvertype_projectedgaussseidel},


    // Filter component types:
    {"sim_filtercomponent_originalimage",sim_filtercomponent_originalimage},
    {"sim_filtercomponent_originaldepth",sim_filtercomponent_originaldepth},
    {"sim_filtercomponent_uniformimage",sim_filtercomponent_uniformimage},
    {"sim_filtercomponent_tooutput",sim_filtercomponent_tooutput},
    {"sim_filtercomponent_tobuffer1",sim_filtercomponent_tobuffer1},
    {"sim_filtercomponent_tobuffer2",sim_filtercomponent_tobuffer2},
    {"sim_filtercomponent_frombuffer1",sim_filtercomponent_frombuffer1},
    {"sim_filtercomponent_frombuffer2",sim_filtercomponent_frombuffer2},
    {"sim_filtercomponent_swapbuffers",sim_filtercomponent_swapbuffers},
    {"sim_filtercomponent_addbuffer1",sim_filtercomponent_addbuffer1},
    {"sim_filtercomponent_subtractbuffer1",sim_filtercomponent_subtractbuffer1},
    {"sim_filtercomponent_multiplywithbuffer1",sim_filtercomponent_multiplywithbuffer1},
    {"sim_filtercomponent_horizontalflip",sim_filtercomponent_horizontalflip},
    {"sim_filtercomponent_verticalflip",sim_filtercomponent_verticalflip},
    {"sim_filtercomponent_rotate",sim_filtercomponent_rotate},
    {"sim_filtercomponent_shift",sim_filtercomponent_shift},
    {"sim_filtercomponent_resize",sim_filtercomponent_resize},
    {"sim_filtercomponent_3x3filter",sim_filtercomponent_3x3filter},
    {"sim_filtercomponent_5x5filter",sim_filtercomponent_5x5filter},
    {"sim_filtercomponent_sharpen",sim_filtercomponent_sharpen},
    {"sim_filtercomponent_edge",sim_filtercomponent_edge},
    {"sim_filtercomponent_rectangularcut",sim_filtercomponent_rectangularcut},
    {"sim_filtercomponent_circularcut",sim_filtercomponent_circularcut},
    {"sim_filtercomponent_normalize",sim_filtercomponent_normalize},
    {"sim_filtercomponent_intensityscale",sim_filtercomponent_intensityscale},
    {"sim_filtercomponent_keeporremovecolors",sim_filtercomponent_keeporremovecolors},
    {"sim_filtercomponent_scaleandoffsetcolors",sim_filtercomponent_scaleandoffsetcolors},
    {"sim_filtercomponent_binary",sim_filtercomponent_binary},
    {"sim_filtercomponent_swapwithbuffer1",sim_filtercomponent_swapwithbuffer1},
    {"sim_filtercomponent_addtobuffer1",sim_filtercomponent_addtobuffer1},
    {"sim_filtercomponent_subtractfrombuffer1",sim_filtercomponent_subtractfrombuffer1},
    {"sim_filtercomponent_correlationwithbuffer1",sim_filtercomponent_correlationwithbuffer1},
    {"sim_filtercomponent_colorsegmentation",sim_filtercomponent_colorsegmentation},
    {"sim_filtercomponent_blobextraction",sim_filtercomponent_blobextraction},
    {"sim_filtercomponent_imagetocoord",sim_filtercomponent_imagetocoord},
    {"sim_filtercomponent_pixelchange",sim_filtercomponent_pixelchange},
    {"sim_filtercomponent_velodyne",sim_filtercomponent_velodyne},
    {"sim_filtercomponent_todepthoutput",sim_filtercomponent_todepthoutput},
    {"sim_filtercomponent_customized",sim_filtercomponent_customized},

    // buffer types:
    {"sim_buffer_uint8",sim_buffer_uint8},
    {"sim_buffer_int8",sim_buffer_int8},
    {"sim_buffer_uint16",sim_buffer_uint16},
    {"sim_buffer_int16",sim_buffer_int16},
    {"sim_buffer_uint32",sim_buffer_uint32},
    {"sim_buffer_int32",sim_buffer_int32},
    {"sim_buffer_float",sim_buffer_float},
    {"sim_buffer_double",sim_buffer_double},
    {"sim_buffer_uint8rgb",sim_buffer_uint8rgb},
    {"sim_buffer_uint8bgr",sim_buffer_uint8bgr},

    // Image combination:
    {"sim_imgcomb_vertical",sim_imgcomb_vertical},
    {"sim_imgcomb_horizontal",sim_imgcomb_horizontal},

    // predefined material types:
    {"sim_dynmat_default",sim_dynmat_default},
    {"sim_dynmat_highfriction",sim_dynmat_highfriction},
    {"sim_dynmat_lowfriction",sim_dynmat_lowfriction},
    {"sim_dynmat_nofriction",sim_dynmat_nofriction},
    {"sim_dynmat_reststackgrasp",sim_dynmat_reststackgrasp},
    {"sim_dynmat_foot",sim_dynmat_foot},
    {"sim_dynmat_wheel",sim_dynmat_wheel},
    {"sim_dynmat_gripper",sim_dynmat_gripper},
    {"sim_dynmat_floor",sim_dynmat_floor},

    // END_CONSTANTS

    // for backward compatibility:
    {"sim_ui_property_visible",sim_ui_property_visible},
    {"sim_ui_property_visibleduringsimulationonly",sim_ui_property_visibleduringsimulationonly},
    {"sim_ui_property_moveable",sim_ui_property_moveable},
    {"sim_ui_property_relativetoleftborder",sim_ui_property_relativetoleftborder},
    {"sim_ui_property_relativetotopborder",sim_ui_property_relativetotopborder},
    {"sim_ui_property_fixedwidthfont",sim_ui_property_fixedwidthfont},
    {"sim_ui_property_systemblock",sim_ui_property_systemblock},
    {"sim_ui_property_settocenter",sim_ui_property_settocenter},
    {"sim_ui_property_rolledup",sim_ui_property_rolledup},
    {"sim_ui_property_selectassociatedobject",sim_ui_property_selectassociatedobject},
    {"sim_ui_property_visiblewhenobjectselected",sim_ui_property_visiblewhenobjectselected},
    {"sim_ui_property_systemblockcanmovetofront",sim_ui_property_systemblockcanmovetofront},
    {"sim_ui_property_pauseactive",sim_ui_property_pauseactive},

    // for backward compatibility:
    {"sim_2delement_menu_title",sim_ui_menu_title},
    {"sim_2delement_menu_minimize",sim_ui_menu_minimize},
    {"sim_2delement_menu_close",sim_ui_menu_close},

    // for backward compatibility:
    {"sim_2delement_property_visible",sim_ui_property_visible},
    {"sim_2delement_property_visibleduringsimulationonly",sim_ui_property_visibleduringsimulationonly},
    {"sim_2delement_property_moveable",sim_ui_property_moveable},
    {"sim_2delement_property_relativetoleftborder",sim_ui_property_relativetoleftborder},
    {"sim_2delement_property_relativetotopborder",sim_ui_property_relativetotopborder},
    {"sim_2delement_property_fixedwidthfont",sim_ui_property_fixedwidthfont},
    {"sim_2delement_property_systemblock",sim_ui_property_systemblock},
    {"sim_2delement_property_settocenter",sim_ui_property_settocenter},
    {"sim_2delement_property_rolledup",sim_ui_property_rolledup},
    {"sim_2delement_property_selectassociatedobject",sim_ui_property_selectassociatedobject},
    {"sim_2delement_property_visiblewhenobjectselected",sim_ui_property_visiblewhenobjectselected},

    // for backward compatibility:
    {"sim_pathproperty_invert_velocity",sim_pathproperty_invert_velocity_deprecated},
    {"sim_pathproperty_infinite_acceleration",sim_pathproperty_infinite_acceleration_deprecated},
    {"sim_pathproperty_auto_velocity_profile_translation",sim_pathproperty_auto_velocity_profile_translation_deprecated},
    {"sim_pathproperty_auto_velocity_profile_rotation",sim_pathproperty_auto_velocity_profile_rotation_deprecated},
    {"sim_pathproperty_endpoints_at_zero",sim_pathproperty_endpoints_at_zero_deprecated},
    {"sim_boolparam_joint_motion_handling_enabled",sim_boolparam_joint_motion_handling_enabled_deprecated},
    {"sim_boolparam_path_motion_handling_enabled",sim_boolparam_path_motion_handling_enabled_deprecated},
    {"sim_jointmode_motion",sim_jointmode_motion_deprecated},


    {"",-1}
};




