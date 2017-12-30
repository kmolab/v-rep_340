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

#include "visualParam.h"
#include "extIkSer.h"
#include "3Vector.h"
#include "4Vector.h"
#include "7Vector.h"
#include "vMutex.h"

struct SCustomRefs
{
    int generalObjectType; // e.g. sim_appobj_object_type, sim_appobj_collision_type
    int generalObjectHandle;
    int option; // bit 0: destroy object if referenced object is destroyed
};

class CShape;
class CCustomData;
class CViewableBase;

class C3DObject  
{
public:

    C3DObject();
    virtual ~C3DObject();

    // Following functions need to be implemented in each class derived from 3DObject
    virtual void display(CViewableBase* renderingObject,int displayAttrib);
    virtual C3DObject* copyYourself();
    virtual void scaleObject(float scalingFactor);
    virtual void scaleObjectNonIsometrically(float x,float y,float z);
    virtual void serialize(CSer& ar);
    virtual void serializeWExtIk(CExtIkSer& ar);
    virtual bool announceObjectWillBeErased(int objID,bool copyBuffer);
    virtual void announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer);
    virtual void announceCollectionWillBeErased(int groupID,bool copyBuffer);
    virtual void announceCollisionWillBeErased(int collisionID,bool copyBuffer);
    virtual void announceDistanceWillBeErased(int distanceID,bool copyBuffer);
    virtual void announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer);
    virtual void performObjectLoadingMapping(std::vector<int>* map);
    virtual void performGroupLoadingMapping(std::vector<int>* map);
    virtual void performCollisionLoadingMapping(std::vector<int>* map);
    virtual void performDistanceLoadingMapping(std::vector<int>* map);
    virtual void performGcsLoadingMapping(std::vector<int>* map);
    virtual void performTextureObjectLoadingMapping(std::vector<int>* map);
    virtual void performDynMaterialObjectLoadingMapping(std::vector<int>* map);
    virtual void performIkLoadingMapping(std::vector<int>* map);
    virtual void bufferMainDisplayStateVariables();
    virtual void bufferedMainDisplayStateVariablesToDisplay();

    virtual void simulationAboutToStart();
    virtual void simulationEnded();
    virtual void initializeInitialValues(bool simulationIsRunning);
    virtual bool getFullBoundingBox(C3Vector& minV,C3Vector& maxV) const;
    virtual bool getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV) const;
    virtual bool getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices) const;

    virtual std::string getObjectTypeInfo() const;
    virtual std::string getObjectTypeInfoExtended() const;
    virtual bool isPotentiallyCollidable() const;
    virtual bool isPotentiallyMeasurable() const;
    virtual bool isPotentiallyDetectable() const;
    virtual bool isPotentiallyRenderable() const;
    virtual bool isPotentiallyCuttable() const;


    // Following functions need to be called from the derived classes
    C3DObject* copyYourselfMain();
    void scaleObjectMain(float scalingFactor);
    void scaleObjectNonIsometricallyMain(float x,float y,float z);
    void serializeMain(CSer& ar);
    void serializeWExtIkMain(CExtIkSer& ar);
    void simulationEndedMain();
    void initializeInitialValuesMain(bool simulationIsRunning);

    bool announceObjectWillBeErasedMain(int objID,bool copyBuffer);
    void announceIkObjectWillBeErasedMain(int ikGroupID,bool copyBuffer);
    void announceGroupWillBeErasedMain(int groupID,bool copyBuffer);
    void announceCollisionWillBeErasedMain(int collisionID,bool copyBuffer);
    void announceDistanceWillBeErasedMain(int distanceID,bool copyBuffer);
    void announceGcsObjectWillBeErasedMain(int gcsObjectID,bool copyBuffer);

    void performObjectLoadingMappingMain(std::vector<int>* map);
    void performGroupLoadingMappingMain(std::vector<int>* map);
    void performCollisionLoadingMappingMain(std::vector<int>* map);
    void performDistanceLoadingMappingMain(std::vector<int>* map);
    void performGcsLoadingMappingMain(std::vector<int>* map);
    void performTextureObjectLoadingMappingMain(std::vector<int>* map);
    void performDynMaterialObjectLoadingMappingMain(std::vector<int>* map);
    void performIkLoadingMappingMain(std::vector<int>* map);

    void bufferMainDisplayStateVariablesMain();
    void bufferedMainDisplayStateVariablesToDisplayMain();
    void setRestoreToDefaultLights(bool s);
    bool getRestoreToDefaultLights() const;
    
    // Various functions
    void scalePosition(float scalingFactor);
    void getAllShapeObjectsRecursive(std::vector<CShape*>* shapeList,bool baseIncluded=true,bool start=true) const;
    void getAllObjectsRecursive(std::vector<C3DObject*>* objectList,bool baseIncluded=true,bool start=true) const;
    void getChain(std::vector<C3DObject*>& objectList,bool tipIncluded=true,bool start=true) const;
    int getUniqueID() const;
    int getAllowedObjectSpecialProperties();

    void setReferencedHandles(int cnt,const int* handles);
    int getReferencedHandlesCount();
    int getReferencedHandles(int* handles);

    C3DObject* getFirstParentInSelection(const std::vector<C3DObject*>* sel) const;
    C3DObject* getLastParentInSelection(const std::vector<C3DObject*>* sel) const;
    void setObjectType(int theType);
    void setDynamicObjectFlag_forVisualization(int isDynamicallySimulatedFlag);
    int getDynamicObjectFlag_forVisualization() const;
    int getDynamicObjectFlag_forVisualization_forDisplay(bool guiIsRendering) const;

    void setSpecificLight(int h);
    int getSpecificLight() const;

    bool getGlobalMarkingBoundingBox(const C7Vector& baseCoordInv,C3Vector& min,C3Vector& max,bool& minMaxNotYetDefined,bool first,bool guiIsRendering);
    void getBoundingBoxEncompassingBoundingBox(const C7Vector& baseCoordInv,C3Vector& min,C3Vector& max,bool guiIsRendering);

    int getModelSelectionID(bool firstObject=true);

    void setLocalObjectProperty(int p);
    int getLocalObjectProperty();
    int getCumulativeObjectProperty();
    void setModelBase(bool m);
    bool getModelBase() const;

    void getChildScriptsToRun(std::vector<int>& childScriptIDs);
    void setObjectManipulationModePermissions(int p);
    int getObjectManipulationModePermissions() const;
    void setObjectManipulationDisabledDuringSimulation(bool d);
    bool getObjectManipulationDisabledDuringSimulation() const;
    void setObjectManipulationDisabledDuringNonSimulation(bool d);
    bool getObjectManipulationDisabledDuringNonSimulation() const;

    void setObjectManipulationTranslationRelativeTo(int p);
    int getObjectManipulationTranslationRelativeTo() const;
    void setObjectManipulationRotationRelativeTo(int p);
    int getObjectManipulationRotationRelativeTo() const;
    void setNonDefaultTranslationStepSize(float s);
    float getNonDefaultTranslationStepSize() const;
    void setNonDefaultRotationStepSize(float s);
    float getNonDefaultRotationStepSize() const;

    void setObjectCustomData(int header,const char* data,int dataLength);
    int getObjectCustomDataLength(int header);
    void getObjectCustomData(int header,char* data);
    bool getObjectCustomDataHeader(int index,int& header);

    // Following same as above, but not serialized:
    void setObjectCustomData_tempData(int header,const char* data,int dataLength);
    int getObjectCustomDataLength_tempData(int header);
    void getObjectCustomData_tempData(int header,char* data);
    bool getObjectCustomDataHeader_tempData(int index,int& header);

    int getParentCount() const;

    void setLocalObjectSpecialProperty(int prop);
    int getLocalObjectSpecialProperty() const;
    void setLocalModelProperty(int prop);
    int getLocalModelProperty() const;
    int getCumulativeModelProperty();
    int getCumulativeObjectSpecialProperty();

    int getTreeDynamicProperty(); // combination of sim_objdynprop_dynamic and sim_objdynprop_respondable

    void disableDynamicTreeForManipulation(bool d);

    void setTransparentObjectDistanceOffset(float d);
    float getTransparentObjectDistanceOffset() const;

    void setAuthorizedViewableObjects(int objOrCollHandle);
    int getAuthorizedViewableObjects() const;

    void setMechanismID(int id);
    int getMechanismID() const;

    // Following two needed when a shape is edited during simulation
    void incrementMemorizedConfigurationValidCounter();
    int getMemorizedConfigurationValidCounter();

    void setEnableCustomizationScript(bool c,const char* scriptContent);
    bool getEnableCustomizationScript();

    void setSizeFactor(float f);
    float getSizeFactor() const;
    void setSizeValues(const float s[3]);
    void getSizeValues(float s[3]) const;


    void setDynamicsFullRefreshFlag(bool refresh);
    bool getDynamicsFullRefreshFlag() const;

    void setDynamicSimulationIconCode(int c);
    int getDynamicSimulationIconCode() const;

    void clearManipulationModeOverlayGridFlag();

    bool isObjectPartOfInvisibleModel();

    void setModelAcknowledgement(const std::string& a);
    std::string getModelAcknowledgement() const;

    void setIgnoredByViewFitting(bool ignored);
    bool getIgnoredByViewFitting() const;

    bool getShouldObjectBeDisplayed(int viewableHandle,int displayAttrib);

    void setAssemblingLocalTransformation(const C7Vector& tr);
    C7Vector getAssemblingLocalTransformation() const;
    void setAssemblingLocalTransformationIsUsed(bool u);
    bool getAssemblingLocalTransformationIsUsed();
    void setAssemblyMatchValues(bool asChild,const char* str);
    std::string getAssemblyMatchValues(bool asChild) const;
    const std::vector<std::string>* getChildAssemblyMatchValuesPointer() const;
    int getAllChildrenThatMayBecomeAssemblyParent(const std::vector<std::string>* assemblingChildMatchValues,std::vector<C3DObject*>& objects) const;
    bool doesParentAssemblingMatchValuesMatchWithChild(const std::vector<std::string>* assemblingChildMatchValues) const;

    C7Vector getParentCumulativeTransformation_forDisplay(bool guiIsRendering) const;
    C7Vector getCumulativeTransformation_forDisplay(bool guiIsRendering) const;
    C7Vector getLocalTransformation_forDisplay(bool guiIsRendering) const;
    C7Vector getCumulativeTransformationPart1_forDisplay(bool guiIsRendering) const;
    C7Vector getLocalTransformationPart1_forDisplay(bool guiIsRendering) const;


    void generateUniqueUpdatableString();
    std::string getUniqueUpdatableString() const;

    std::string getExtensionString() const;
    void setExtensionString(const char* str);

    void setHierarchyColorIndex(int c);
    int getHierarchyColorIndex() const;

    void setCollectionSelfCollisionIndicator(int c);
    int getCollectionSelfCollisionIndicator() const;

    void measureVelocity(float dt); // should be called just after the main script was called!!!
    float getMeasuredAngularVelocity() const;
    C3Vector getMeasuredLinearVelocity() const;
    C3Vector getMeasuredAngularVelocity3() const;

    void setForceAlwaysVisible_tmp(bool force);

    static void incrementModelPropertyValidityNumber();


    void getCumulativeTransformationMatrix(float m[4][4],bool useTempValues=false) const;
    void getCumulativeTransformationMatrixPart1(float m[4][4],bool useTempValues=false) const;

    void setLocalTransformation(const C7Vector& v);
    void setLocalTransformation(const C4Vector& q);
    void setLocalTransformation(const C3Vector& x);

    void setAbsoluteTransformation(const C7Vector& v);
    void setAbsoluteTransformation(const C4Vector& q);
    void setAbsoluteTransformation(const C3Vector& x);
    C7Vector getParentCumulativeTransformation(bool useTempValues=false) const;
    C7Vector getCumulativeTransformation(bool useTempValues=false) const;
    C7Vector getLocalTransformation(bool useTempValues=false) const;
    C7Vector getCumulativeTransformationPart1(bool useTempValues=false) const;
    C7Vector getLocalTransformationPart1(bool useTempValues=false) const;

    int getID() const;
    void setID(int newID);
    void setName(std::string newName);
    std::string getName() const;
    C3DObject* getParent() const;
    bool isObjectParentedWith(C3DObject* thePotentialParent) const;
    int getParentIdentifierLoading() const;
    void setParentIdentifierLoading(int pID);
    void setParent(C3DObject* newParent,bool actualizeInfo=true);
    int getObjectType() const;
    int getFirstModelRelatives(std::vector<C3DObject*>& firstModelRelatives,bool visibleModelsOnly) const;
    int countFirstModelRelatives(bool visibleModelsOnly) const;

    std::vector<C3DObject*> childList;
    unsigned short layer;

protected:
    int _dynamicSimulationIconCode;
    static int _modelPropertyValidityNumber;

    bool _restoreToDefaultLights;
    bool _forceAlwaysVisible_tmp;
    int _specificLight; // -1=default (i.e. all), -2=none, otherwise a light ID or collection ID. Is not serialized!
    std::string _extensionString;

    C7Vector _assemblingLocalTransformation; // When assembling this object
    bool _assemblingLocalTransformationIsUsed;

    int _uniqueID; // valid for a given scene instance (non-persistent)

    std::vector<std::string> _assemblyMatchValuesChild;
    std::vector<std::string> _assemblyMatchValuesParent;
    std::string _uniqueUpdatableString;

    C7Vector _transformation; // Quaternion and position
    int _objectID;
    std::string _objectName;
    // Keep parent stored as pointer. Very important!!! If we would use a simple
    // parent identifier, an object and its copy in the copy buffer would have
    // the exact same parent (which would be retrieved via 'CObjCont::getObject()')
    C3DObject* _parentObject;
    int _parentID;
    int _objectType;

    int _localObjectProperty;
    bool _modelBase;
    bool _ignoredByViewFitting;
    int _hierarchyColorIndex;
    int _collectionSelfCollisionIndicator;

    int _localObjectSpecialProperty;
    int _localModelProperty;
    int _cumulativeModelProperty;
    int _cumulativeModelPropertyValidityNumber;

    int _authorizedViewableObjects; // -1 --> all, -2 --> none, otherwise object or collection handle

    int _objectManipulationModePermissions;
    bool _objectManipulationDisabledDuringSimulation;
    bool _objectManipulationDisabledDuringNonSimulation;
    bool _objectManipulationModePermissionsPreviousCtrlKeyDown;
    float _objectTranslationNonDefaultStepSize;
    float _objectRotationNonDefaultStepSize;
    int _objectManipulationTranslationRelativeTo; //0=world, 1=parent, 2=own frame
    int _objectManipulationRotationRelativeTo; //0=world, 1=parent, 2=own frame
    int _memorizedConfigurationValidCounter;
    float _sizeFactor; // just used so that scripts can also adjust for scaling
    float _sizeValues[3];
    int _dynamicProperty; // modified in the dynamics routines. Not saved nor copied!
    bool _dynamicsTemporarilyDisabled;
    bool _dynamicsFullRefreshFlag;

    float _transparentObjectDistanceOffset;

    VMutex _objectMutex;
    C3Vector _objectManipulationModeSubTranslation;
    float _objectManipulationModeSubRotation;
    int _objectManipulationModeEventId;
    int _objectManipulationModeAxisIndex;
    C3Vector _objectManipulationModeRelativePositionOfClickedPoint;
    C3Vector _objectManipulationModeTotalTranslation;
    float _objectManipulationModeTotalRotation;
    unsigned char _objectManipulationMode_flaggedForGridOverlay; // is the rotation or translation axis index + 8 if it is a rotation, or +16 if it is a translation
    CCustomData* _customObjectData;
    CCustomData* _customObjectData_tempData; // this one is not serialized (but copied)!
    std::vector<SCustomRefs> _customReferencedHandles;
    std::string _modelAcknowledgement;

    C7Vector _transformation_buffered;
    C7Vector _transformation_forDisplay;
    int _dynamicObjectFlag_forVisualization_buffered;
    int _dynamicObjectFlag_forVisualization_forDisplay;


    // Other variables
    int _mechanismID; // don't serialize!
    bool _initialValuesInitializedMain;
    bool _initialConfigurationMemorized;
    int _initialParentUniqueID;
    int _initialMainPropertyOverride;
    int _initialMemorizedConfigurationValidCounter;
    C7Vector _initialLocalTransformationPart1;
    static int _uniqueIDCounter;

    int _dynamicObjectFlag_forVisualization;

    float _measuredAngularVelocity_velocityMeasurement;
    C3Vector _measuredAngularVelocity3_velocityMeasurement;
    C3Vector _measuredLinearVelocity_velocityMeasurement;
    C7Vector _previousAbsTransf_velocityMeasurement;
    bool _previousPositionOrientationIsValid;


#ifdef SIM_WITH_GUI
public:
    void displayManipulationModeOverlayGrid(bool transparentAndOverlay);
    bool setLocalTransformationFromObjectRotationMode(const C4X4Matrix& cameraAbsConf,float rotationAmount,bool perspective,int eventID);
    bool setLocalTransformationFromObjectTranslationMode(const C4X4Matrix& cameraAbsConf,const C3Vector& clicked3DPoint,float prevPos[2],float pos[2],float screenHalfSizes[2],float halfSizes[2],bool perspective,int eventID);
#endif
};
