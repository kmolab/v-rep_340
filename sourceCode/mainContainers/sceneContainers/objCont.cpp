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

// This file needs serious refactoring!!

#include "vrepMainHeader.h"
#include "funcDebug.h"
#include "tt.h"
#include "objCont.h"
#include "global.h"
#include "v_rep_internal.h"
#include "shapeComponent.h"
#include "geometricConstraintSolverInt.h"
#include "pluginContainer.h"
#include "geometric.h"
#include "meshManip.h"
#include "v_repStrings.h"
#include "app.h"
#include "vDateTime.h"

CObjCont::CObjCont()
{
    _objectActualizationEnabled=true;
    _nextObjectHandle=SIM_IDSTART_3DOBJECT;

    removeAllObjects();
}

CObjCont::~CObjCont()
{
    removeAllObjects();
}

float CObjCont::_defaultSceneID=0.0f;

void CObjCont::setDefaultSceneID(float id)
{
    _defaultSceneID=id;
}

float CObjCont::getDefaultSceneID()
{
    return(_defaultSceneID);
}

void CObjCont::appendLoadOperationIssue(const char* text,int objectId)
{
    _loadOperationIssuesToBeDisplayed+=text;
    _loadOperationIssuesToBeDisplayed_objectHandles.push_back(objectId);
}

float CObjCont::computeSceneID()
{ // there is no guarantee at all that two same ids really represent the same scene! Should just be used as an indication!
    float id=0.0f;
    for (int i=0;i<int(objectList.size());i++)
        id+=float(objectList[i]);
    if (objectList.size()!=0)
        id/=float(objectList.size());
    return(id);
}

void CObjCont::simulationAboutToStart()
{
    C3DObject::incrementModelPropertyValidityNumber();
    for (int i=0;i<int(objectList.size());i++)
        getObject(objectList[i])->simulationAboutToStart();
}

void CObjCont::simulationEnded()
{
    for (int i=0;i<int(objectList.size());i++)
        getObject(objectList[i])->simulationEnded();
    C3DObject::incrementModelPropertyValidityNumber();
}

bool CObjCont::loadModelOrScene(CSer& ar,bool selectLoaded,bool isScene,bool justLoadThumbnail,C7Vector* optionalModelTr,C3Vector* optionalModelBoundingBoxSize,float* optionalModelNonDefaultTranslationStepSize)
{   // There is a similar routine for XML files further down

    // Should always be called through 'loadModel' or 'loadScene'!!!!

    _loadOperationIssuesToBeDisplayed.clear();
    _loadOperationIssuesToBeDisplayed_objectHandles.clear();

    CGeometric::clearTempVerticesIndicesNormalsAndEdges();
    deselectObjects();

    std::vector<C3DObject*> loadedObjectList;
    std::vector<CTextureObject*> loadedTextureList;
    std::vector<CDynMaterialObject*> loadedDynMaterialList;
    std::vector<CRegCollection*> loadedGroupList;
    std::vector<CRegCollision*> loadedCollisionList;
    std::vector<CRegDist*> loadedDistanceList;
    std::vector<CikGroup*> loadedIkGroupList;
    std::vector<CPathPlanningTask*> pathPlanningTaskList;
    std::vector<CMotionPlanningTask*> motionPlanningTaskList;
    std::vector<CButtonBlock*> loadedButtonBlockList;
    std::vector<CLuaScriptObject*> loadedLuaScriptList;
    std::vector<CConstraintSolverObject*> loadedConstraintSolverObjectList;


    int byteQuantity;
    std::string theName="";
    bool hasThumbnail=false;
    while (theName.compare(SER_END_OF_FILE)!=0)
    {
        theName=ar.readDataName();
        if (theName.compare(SER_END_OF_FILE)!=0)
        {
            bool noHit=true;
            if (theName.compare(SER_MODEL_THUMBNAIL_INFO)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                C7Vector tr;
                C3Vector bbs;
                float ndss;
                App::ct->environment->modelThumbnail_notSerializedHere.serializeAdditionalModelInfos(ar,tr,bbs,ndss);
                if (optionalModelTr!=NULL)
                    optionalModelTr[0]=tr;
                if (optionalModelBoundingBoxSize!=NULL)
                    optionalModelBoundingBoxSize[0]=bbs;
                if (optionalModelNonDefaultTranslationStepSize!=NULL)
                    optionalModelNonDefaultTranslationStepSize[0]=ndss;
                noHit=false;
            }

            if (theName.compare(SER_MODEL_THUMBNAIL)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                App::ct->environment->modelThumbnail_notSerializedHere.serialize(ar);
                noHit=false;
                if (justLoadThumbnail)
                    return(true);
                hasThumbnail=true;
            }

            if (theName.compare(SER_MODEL_THUMBNAIL_OLD_COMPATIBILITY_2012_03_06)==0)
            { // For backward compatibility (6/3/2012) Takes care of files reaching back to 25/7/2010
                ar >> byteQuantity;
                int l;
                ar >> l;
                char t;
                std::vector<char> modelThumbnailBuffer;
                for (int i=0;i<l;i++)
                {
                    ar >> t;
                    modelThumbnailBuffer.push_back(t);
                }
                if (modelThumbnailBuffer.size()!=0)
                    App::ct->environment->modelThumbnail_notSerializedHere.setCompressedThumbnailImage(&modelThumbnailBuffer[0]);
                else
                    App::ct->environment->modelThumbnail_notSerializedHere.clearThumbnailImage();
                noHit=false;
                if (justLoadThumbnail)
                    return(true);
                hasThumbnail=true;
            }

            // Handle the heavy data here so we don't have duplicates (vertices, indices, normals and edges):
            //------------------------------------------------------------
            if (theName.compare(SER_VERTICESINDICESNORMALSEDGES)==0)
            {
                CGeometric::clearTempVerticesIndicesNormalsAndEdges();
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                CGeometric::serializeTempVerticesIndicesNormalsAndEdges(ar);
                noHit=false;
            }
            //------------------------------------------------------------

            C3DObject* it=load3DObject(ar,theName,noHit);
            if (it!=NULL) 
            {
                loadedObjectList.push_back(it);
                noHit=false;
            }

            CTextureObject* theTextureData=App::ct->textureCont->loadTextureObject(ar,theName,noHit);
            if (theTextureData!=NULL)
            {
                loadedTextureList.push_back(theTextureData);
                noHit=false;
            }
            if (theName.compare(SER_DYNMATERIAL)==0)
            { // Following for backward compatibility (i.e. files written prior V-REP 3.4.0) (30/10/2016)
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                CDynMaterialObject* myNewObject=new CDynMaterialObject();
                myNewObject->serialize(ar);
                loadedDynMaterialList.push_back(myNewObject);
                noHit=false;
            }
            if (theName.compare(SER_GHOSTS)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                App::ct->ghostObjectCont->serialize(ar);
                noHit=false;
            }
            if (theName.compare(SER_ENVIRONMENT)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                App::ct->environment->serialize(ar,true);
                noHit=false;
            }
            if (theName.compare(SER_SETTINGS)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                App::ct->mainSettings->serialize(ar);
                noHit=false;
            }
            if (theName.compare(SER_DYNAMICS)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                App::ct->dynamicsContainer->serialize(ar);
                noHit=false;
            }
            if (theName.compare(SER_SIMULATION)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                App::ct->simulation->serialize(ar);
                noHit=false;
                
                // For backward compatibility (3/1/2012):
                //************************************************
                if (App::ct->mainSettings->forBackwardCompatibility_03_01_2012_stillUsingStepSizeDividers)
                { // This needs to be done AFTER simulation settings are loaded!
                    float step=float(App::ct->simulation->getSimulationTimeStep_speedModified_ns())/1000000.0f;
                    float bulletStepSize=step/float(App::ct->mainSettings->dynamicsBULLETStepSizeDivider_forBackwardCompatibility_03_01_2012);
                    float odeStepSize=step/float(App::ct->mainSettings->dynamicsODEStepSizeDivider_forBackwardCompatibility_03_01_2012);
                    if (fabs(step-0.05f)>0.002f)
                        App::ct->dynamicsContainer->setUseDynamicDefaultCalculationParameters(4); // use custom settings
                    // Following has an effect only when using custom parameters (custom parameters might already be enabled before above line!):

                    App::ct->dynamicsContainer->setEngineFloatParam(sim_bullet_global_stepsize,bulletStepSize,false);
                    App::ct->dynamicsContainer->setEngineFloatParam(sim_ode_global_stepsize,odeStepSize,false);
                }
                //************************************************
            }
            if (theName.compare(SER_VIEWS)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                App::ct->pageContainer->serialize(ar);
                noHit=false;
            }
            if (theName.compare(SER_GROUP)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                CRegCollection* it=new CRegCollection("Default");
                it->serialize(ar);
                loadedGroupList.push_back(it);
                noHit=false;
            }
            if (theName.compare(SER_BUTTON_BLOCK)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                CButtonBlock* it=new CButtonBlock(1,1,10,10,0);
                it->serialize(ar);
                loadedButtonBlockList.push_back(it);
                noHit=false;
            }
            if (theName.compare(SER_LUA_SCRIPT)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                CLuaScriptObject* it=new CLuaScriptObject(-1);
                it->serialize(ar);
                loadedLuaScriptList.push_back(it);
                noHit=false;
            }
            if (theName.compare(SER_GEOMETRIC_CONSTRAINT_OBJECT)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                CConstraintSolverObject* it=new CConstraintSolverObject();
                it->serialize(ar);
                loadedConstraintSolverObjectList.push_back(it);
                noHit=false;
            }
            if (theName.compare(SER_SCENE_CUSTOM_DATA)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                App::ct->customSceneData->serializeData(ar);
                noHit=false;
            }
            if (theName.compare(SER_COLLISION)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                CRegCollision* it=new CRegCollision(0,0,"",0);
                it->serialize(ar);
                loadedCollisionList.push_back(it);
                noHit=false;
            }
            if (theName.compare(SER_DISTANCE)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                CRegDist* it=new CRegDist(0,0,"",0);
                it->serialize(ar);
                loadedDistanceList.push_back(it);
                noHit=false;
            }
            if (theName.compare(SER_IK)==0)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                CikGroup* it=new CikGroup();
                it->serialize(ar);
                loadedIkGroupList.push_back(it);
                noHit=false;
            }
            if (theName==SER_PATH_PLANNING)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                CPathPlanningTask* it=new CPathPlanningTask();
                it->serialize(ar);
                pathPlanningTaskList.push_back(it);
                noHit=false;
            }
            if (theName==SER_MOTION_PLANNING)
            {
                ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                CMotionPlanningTask* it=new CMotionPlanningTask();
                it->serialize(ar);
                motionPlanningTaskList.push_back(it);
                noHit=false;
            }
            if (noHit)
                ar.loadUnknownData();
        }
    }

    CGeometric::clearTempVerticesIndicesNormalsAndEdges();

    int fileVrepVersion=ar.getVrepVersionThatWroteThisFile();

    // All object have been loaded and are in:
    // loadedObjectList
    // loadedGroupList
    // ...

    addObjectsToSceneAndPerformMappings(&loadedObjectList,
                                        &loadedGroupList,
                                        &loadedCollisionList,
                                        &loadedDistanceList,
                                        &loadedIkGroupList,
                                        &pathPlanningTaskList,
                                        &motionPlanningTaskList,
                                        &loadedButtonBlockList,
                                        &loadedLuaScriptList,
                                        &loadedConstraintSolverObjectList,
                                        loadedTextureList,
                                        loadedDynMaterialList,
                                        !isScene,fileVrepVersion);

    // Following to avoid the flickering when loading something (also during undo/redo):
    for (size_t i=0;i<App::ct->objCont->objectList.size();i++)
    {
        C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
        it->bufferMainDisplayStateVariables();
        it->bufferedMainDisplayStateVariablesToDisplay();
    }

    CGeometric::clearTempVerticesIndicesNormalsAndEdges();

    _loadOperationIssuesToBeDisplayed.clear();
    _loadOperationIssuesToBeDisplayed_objectHandles.clear();

    return(true);
}

bool CObjCont::loadModelOrScene(const CXmlSer& xmlDoc,const xmlNode* parentNode,bool selectLoaded,bool isScene)
{   // There is a similar routine for binary files further up

    // Should always be called through 'loadModel' or 'loadScene'!!!!

    _loadOperationIssuesToBeDisplayed.clear();
    _loadOperationIssuesToBeDisplayed_objectHandles.clear();

    CGeometric::clearTempVerticesIndicesNormalsAndEdges();
    deselectObjects();

    CGeometric::clearTempVerticesIndicesNormalsAndEdges();

    _loadOperationIssuesToBeDisplayed.clear();
    _loadOperationIssuesToBeDisplayed_objectHandles.clear();

    return(true);
}

void CObjCont::addObjectsToSceneAndPerformMappings(std::vector<C3DObject*>* loadedObjectList,
                                                    std::vector<CRegCollection*>* loadedGroupList,
                                                    std::vector<CRegCollision*>* loadedCollisionList,
                                                    std::vector<CRegDist*>* loadedDistanceList,
                                                    std::vector<CikGroup*>* loadedIkGroupList,
                                                    std::vector<CPathPlanningTask*>* loadedPathPlanningTaskList,
                                                    std::vector<CMotionPlanningTask*>* loadedMotionPlanningTaskList,
                                                    std::vector<CButtonBlock*>* loadedButtonBlockList,
                                                    std::vector<CLuaScriptObject*>* loadedLuaScriptList,
                                                    std::vector<CConstraintSolverObject*>* loadedConstraintSolverObjectList,
                                                    std::vector<CTextureObject*>& loadedTextureObjectList,
                                                    std::vector<CDynMaterialObject*>& loadedDynMaterialObjectList,
                                                    bool model,int fileVrepVersion)
{
    FUNCTION_DEBUG;
    // We check what suffix offset is needed for this model (in case of a scene, the offset is ignored since we won't introduce the objects as copies!):
    int suffixOffset=getSuffixOffsetForObjectToAdd(loadedObjectList,
        loadedGroupList,
        loadedCollisionList,
        loadedDistanceList,
        loadedIkGroupList,
        loadedPathPlanningTaskList,
        loadedMotionPlanningTaskList,
        loadedButtonBlockList,
        loadedLuaScriptList,
        loadedConstraintSolverObjectList);

    // We have 3 cases:
    // 1. We are loading a scene, 2. We are loading a model, 3. We are pasting objects
    // We add objects to the scene as copies only if we also add at least one child script and we don't have a scene. Otherwise objects are added
    // and no '#' (or no modified suffix) will appear in their names.
    // Following line summarizes this:
    bool objectIsACopy=((loadedLuaScriptList->size()!=0)&&model); // scenes are not treated like copies!

    std::vector<int> allInitAndEndSuffix;
    std::vector<std::string> allNamesWithTypes;

    // Texture data:
    std::vector<int> textureMapping;
    for (size_t i=0;i<loadedTextureObjectList.size();i++)
    {
        textureMapping.push_back(loadedTextureObjectList[i]->getObjectID());
        CTextureObject* handler=App::ct->textureCont->getObject(App::ct->textureCont->addObjectWithSuffixOffset(loadedTextureObjectList[i],objectIsACopy,suffixOffset)); // if a same object is found, the object is destroyed in addObject!
        if (handler!=loadedTextureObjectList[i])
            loadedTextureObjectList[i]=handler; // this happens when a similar object is already present
        textureMapping.push_back(handler->getObjectID());
    }
    prepareFastLoadingMapping(textureMapping);

    // We add all 3DObjects:
    enableObjectActualization(false);
    std::vector<int> objectMapping;
    for (size_t i=0;i<loadedObjectList->size();i++)
    {
        objectMapping.push_back(loadedObjectList->at(i)->getID()); // Old ID
        std::string initName(loadedObjectList->at(i)->getName());
        addObjectToSceneWithSuffixOffset(loadedObjectList->at(i),objectIsACopy,suffixOffset);
        objectMapping.push_back(loadedObjectList->at(i)->getID()); // New ID

        if (loadedObjectList->at(i)->getObjectType()==sim_object_shape_type)
        {
            CShape* shape=(CShape*)loadedObjectList->at(i);
            int matId=shape->geomData->geomInfo->getDynMaterialId_OLD();
            if ((fileVrepVersion<30303)&&(matId>=0))
            { // for backward compatibility(29/10/2016), when the dyn material was stored separaterly and shared among shapes
                for (size_t j=0;j<loadedDynMaterialObjectList.size();j++)
                {
                    if (loadedDynMaterialObjectList[j]->getObjectID()==matId)
                    {
                        CDynMaterialObject* mat=loadedDynMaterialObjectList[j]->copyYourself();
                        shape->setDynMaterial(mat);
                        break;
                    }
                }
            }
            if (matId==-2)
            { // Backward compatibility (12/9/2013): before V-REP 3.0.5, each shape had individual material properties. Then until V3.3.2, they had shared mat. prop. Then again individual prop.!!!
                CDynMaterialObject* mat=shape->geomData->geomInfo->createDynMaterialObject_OLD();
                shape->setDynMaterial(mat);
            }
            if (fileVrepVersion<30301)
            { // Following for backward compatibility (09/03/2016)
                CDynMaterialObject* mat=shape->getDynMaterial();
                if (mat->getEngineBoolParam(sim_bullet_body_sticky,NULL))
                { // Formely sticky contact objects need to be adjusted for the new Bullet:
                    if (shape->getShapeIsDynamicallyStatic())
                        mat->setEngineFloatParam(sim_bullet_body_friction,mat->getEngineFloatParam(sim_bullet_body_oldfriction,NULL)); // the new Bullet friction
                    else
                        mat->setEngineFloatParam(sim_bullet_body_friction,0.25f); // the new Bullet friction
                }
            }
            shape->geomData->geomInfo->setDynMaterialId_OLD(-1);
        }
    }
    prepareFastLoadingMapping(objectMapping);
    enableObjectActualization(true);
    actualizeObjectInformation();

    // Remove any material that was loaded from a previous file version, where materials were still shared (until V3.3.2)
    for (size_t i=0;i<loadedDynMaterialObjectList.size();i++)
        delete loadedDynMaterialObjectList[i];

    // We add all the collections:
    std::vector<int> groupMapping;
    for (size_t i=0;i<loadedGroupList->size();i++)
    {
        groupMapping.push_back(loadedGroupList->at(i)->getCollectionID()); // Old ID
        std::string initName(loadedGroupList->at(i)->getCollectionName());
        App::ct->collections->addCollectionWithSuffixOffset(loadedGroupList->at(i),objectIsACopy,suffixOffset);
        groupMapping.push_back(loadedGroupList->at(i)->getCollectionID()); // New ID
    }
    prepareFastLoadingMapping(groupMapping);

    // We add all the collisions:
    std::vector<int> collisionMapping;
    for (size_t i=0;i<loadedCollisionList->size();i++)
    {
        collisionMapping.push_back(loadedCollisionList->at(i)->getObjectID()); // Old ID
        std::string initName(loadedCollisionList->at(i)->getObjectName());
        App::ct->collisions->addObjectWithSuffixOffset(loadedCollisionList->at(i),objectIsACopy,suffixOffset);
        collisionMapping.push_back(loadedCollisionList->at(i)->getObjectID()); // New ID
    }
    prepareFastLoadingMapping(collisionMapping);

    // We add all the distances:
    std::vector<int> distanceMapping;
    for (size_t i=0;i<loadedDistanceList->size();i++)
    {
        distanceMapping.push_back(loadedDistanceList->at(i)->getObjectID()); // Old ID
        std::string initName(loadedDistanceList->at(i)->getObjectName());
        App::ct->distances->addObjectWithSuffixOffset(loadedDistanceList->at(i),objectIsACopy,suffixOffset);
        distanceMapping.push_back(loadedDistanceList->at(i)->getObjectID()); // New ID
    }
    prepareFastLoadingMapping(distanceMapping);

    // We add all the ik groups:
    std::vector<int> ikGroupMapping;
    for (size_t i=0;i<loadedIkGroupList->size();i++)
    {
        ikGroupMapping.push_back(loadedIkGroupList->at(i)->getObjectID()); // Old ID
        std::string initName(loadedIkGroupList->at(i)->getObjectName());
        App::ct->ikGroups->addIkGroupWithSuffixOffset(loadedIkGroupList->at(i),objectIsACopy,suffixOffset);
        ikGroupMapping.push_back(loadedIkGroupList->at(i)->getObjectID()); // New ID
    }
    prepareFastLoadingMapping(ikGroupMapping);

    // We add all the path planning tasks:
    std::vector<int> pathPlanningTaskMapping;
    for (size_t i=0;i<loadedPathPlanningTaskList->size();i++)
    {
        pathPlanningTaskMapping.push_back(loadedPathPlanningTaskList->at(i)->getObjectID()); // Old ID
        std::string initName(loadedPathPlanningTaskList->at(i)->getObjectName());
        App::ct->pathPlanning->addObjectWithSuffixOffset(loadedPathPlanningTaskList->at(i),objectIsACopy,suffixOffset);
        pathPlanningTaskMapping.push_back(loadedPathPlanningTaskList->at(i)->getObjectID()); // New ID
    }
    prepareFastLoadingMapping(pathPlanningTaskMapping);

    // We add all the motion planning tasks:
    std::vector<int> motionPlanningTaskMapping;
    for (size_t i=0;i<loadedMotionPlanningTaskList->size();i++)
    {
        motionPlanningTaskMapping.push_back(loadedMotionPlanningTaskList->at(i)->getObjectID()); // Old ID
        std::string initName(loadedMotionPlanningTaskList->at(i)->getObjectName());
        App::ct->motionPlanning->addObjectWithSuffixOffset(loadedMotionPlanningTaskList->at(i),objectIsACopy,suffixOffset);
        motionPlanningTaskMapping.push_back(loadedMotionPlanningTaskList->at(i)->getObjectID()); // New ID
    }
    prepareFastLoadingMapping(motionPlanningTaskMapping);

    // We add all the button blocks:
    std::vector<int> buttonBlockMapping;
    for (size_t i=0;i<loadedButtonBlockList->size();i++)
    {
        buttonBlockMapping.push_back(loadedButtonBlockList->at(i)->getBlockID()); // Old ID
        std::string initName(loadedButtonBlockList->at(i)->getBlockName());
        App::ct->buttonBlockContainer->insertBlockWithSuffixOffset(loadedButtonBlockList->at(i),objectIsACopy,suffixOffset);
        buttonBlockMapping.push_back(loadedButtonBlockList->at(i)->getBlockID()); // New ID
    }
    prepareFastLoadingMapping(buttonBlockMapping);

    // We add all the scripts:
    std::vector<int> luaScriptMapping;
    for (size_t i=0;i<loadedLuaScriptList->size();i++)
    {
        luaScriptMapping.push_back(loadedLuaScriptList->at(i)->getScriptID()); // Old ID
        App::ct->luaScriptContainer->insertScript(loadedLuaScriptList->at(i));
        luaScriptMapping.push_back(loadedLuaScriptList->at(i)->getScriptID()); // New ID
    }
    prepareFastLoadingMapping(luaScriptMapping);

    // We add all the GCS objects:
    std::vector<int> constraintSolverObjectMapping;
    for (size_t i=0;i<loadedConstraintSolverObjectList->size();i++)
    {
        constraintSolverObjectMapping.push_back(loadedConstraintSolverObjectList->at(i)->getObjectID()); // Old ID
        std::string initName(loadedConstraintSolverObjectList->at(i)->getObjectName());
        App::ct->constraintSolver->addObjectWithSuffixOffset(loadedConstraintSolverObjectList->at(i),objectIsACopy,suffixOffset);
        constraintSolverObjectMapping.push_back(loadedConstraintSolverObjectList->at(i)->getObjectID()); // New ID
    }
    prepareFastLoadingMapping(constraintSolverObjectMapping);


    enableObjectActualization(false);

    // We do the mapping for the 3DObjects:
    for (size_t i=0;i<loadedObjectList->size();i++)
    {
        C3DObject* it=loadedObjectList->at(i);
        it->performObjectLoadingMapping(&objectMapping);
        it->performGroupLoadingMapping(&groupMapping);
        it->performCollisionLoadingMapping(&collisionMapping);
        it->performDistanceLoadingMapping(&distanceMapping);
        it->performIkLoadingMapping(&ikGroupMapping);
        it->performGcsLoadingMapping(&constraintSolverObjectMapping);
        it->performTextureObjectLoadingMapping(&textureMapping);
//        it->performDynMaterialObjectLoadingMapping(&dynMaterialMapping);
    }
    // We do the mapping for the collections:
    for (size_t i=0;i<loadedGroupList->size();i++)
    {
        CRegCollection* it=loadedGroupList->at(i);
        it->performObjectLoadingMapping(&objectMapping);
    }
    // We do the mapping for the collisions:
    for (size_t i=0;i<loadedCollisionList->size();i++)
    {
        CRegCollision* it=loadedCollisionList->at(i);
        it->performObjectLoadingMapping(&objectMapping);
        it->performGroupLoadingMapping(&groupMapping);
    }
    // We do the mapping for the distances:
    for (size_t i=0;i<loadedDistanceList->size();i++)
    {
        CRegDist* it=loadedDistanceList->at(i);
        it->performObjectLoadingMapping(&objectMapping);
        it->performGroupLoadingMapping(&groupMapping);
    }
    // We do the mapping for the ik groups:
    for (size_t i=0;i<loadedIkGroupList->size();i++)
    {
        CikGroup* it=loadedIkGroupList->at(i);
        it->performObjectLoadingMapping(&objectMapping);
        it->performGroupLoadingMapping(&groupMapping);
        it->performIkGroupLoadingMapping(&ikGroupMapping);
    }
    // We do the mapping for the path planning tasks:
    for (size_t i=0;i<loadedPathPlanningTaskList->size();i++)
    {
        CPathPlanningTask* it=loadedPathPlanningTaskList->at(i);
        it->performObjectLoadingMapping(&objectMapping);
        it->performGroupLoadingMapping(&groupMapping);
    }
    // We do the mapping for the motion planning tasks:
    for (size_t i=0;i<loadedMotionPlanningTaskList->size();i++)
    {
        CMotionPlanningTask* it=loadedMotionPlanningTaskList->at(i);
        it->performObjectLoadingMapping(&objectMapping);
        it->performGroupLoadingMapping(&groupMapping);
        it->performIkGroupLoadingMapping(&ikGroupMapping);
    }
    // We do the mapping for the 2D Elements:
    for (size_t i=0;i<loadedButtonBlockList->size();i++)
    {
        CButtonBlock* it=loadedButtonBlockList->at(i);
        it->perform3DObjectLoadingMapping(&objectMapping);
        it->performTextureObjectLoadingMapping(&textureMapping);
    }
    // We do the mapping for the Lua scripts:
    for (size_t i=0;i<loadedLuaScriptList->size();i++)
    {
        CLuaScriptObject* it=loadedLuaScriptList->at(i);
        it->perform3DObjectLoadingMapping(&objectMapping);
    }
    // We do the mapping for the GCS objects:
    for (size_t i=0;i<loadedConstraintSolverObjectList->size();i++)
    {
        CConstraintSolverObject* it=loadedConstraintSolverObjectList->at(i);
        it->performObjectLoadingMapping(&objectMapping);
    }

    // We do the mapping for the ghost objects:
    if (!model)
        App::ct->ghostObjectCont->performObjectLoadingMapping(&objectMapping);

    // We set ALL texture object dependencies (not just for loaded objects):
    // We cannot use App::ct->textureCont->updateAllDependencies, since the shape list is not yet actualized!
    App::ct->textureCont->clearAllDependencies();
    App::ct->buttonBlockContainer->setTextureDependencies();
    for (size_t i=0;i<_objectIndex.size();i++) // here we cannot use shapeList, because not yet actualized!!
    {
        if ( (_objectIndex[i]!=NULL)&&(_objectIndex[i]->getObjectType()==sim_object_shape_type) )
            ((CShape*)_objectIndex[i])->geomData->setTextureDependencies(_objectIndex[i]->getID());
    }


    enableObjectActualization(true);
    actualizeObjectInformation();

    if (!model)
        App::ct->pageContainer->performObjectLoadingMapping(&objectMapping);

    actualizeObjectInformation();

    // Following for backward compatibility (6/8/2014) (not that important if left out):
    for (size_t i=0;i<loadedObjectList->size();i++)
    {
        if (loadedObjectList->at(i)->getObjectType()==sim_object_joint_type)
        {
            CJoint* joint=(CJoint*)loadedObjectList->at(i);
            if (joint->getJointCallbackCallOrder_backwardCompatibility()!=0)
            {
                CLuaScriptObject* script=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_callback(joint->getID());
                if (script!=NULL)
                    script->setExecutionOrder(joint->getJointCallbackCallOrder_backwardCompatibility()+1);
            }
        }
    }

    // Now clean-up suffixes equal or above those added, but only for models or objects copied into the scene (global suffix clean-up can be done in the environment dialog):
    if (model) // condition was added on 29/9/2014
        cleanupDashNames(suffixOffset-1);

/* Until 4/10/2013. Global suffix name clean-up. This was confusing!
    if (App::ct->simulation->isSimulationStopped()) // added on 2010/02/20 (otherwise objects can get automatically renamed during simulation!!)
        cleanupDashNames(-1);
*/

//************ We need to initialize all object types (also because an object copied during simulation hasn't the simulationEnded routine called!)
    bool simulationRunning=!App::ct->simulation->isSimulationStopped();
    for (size_t i=0;i<loadedObjectList->size();i++)
        loadedObjectList->at(i)->initializeInitialValues(simulationRunning);
    for (size_t i=0;i<loadedButtonBlockList->size();i++)
        loadedButtonBlockList->at(i)->initializeInitialValues(simulationRunning);
    for (size_t i=0;i<loadedCollisionList->size();i++)
        loadedCollisionList->at(i)->initializeInitialValues(simulationRunning);
    for (size_t i=0;i<loadedDistanceList->size();i++)
        loadedDistanceList->at(i)->initializeInitialValues(simulationRunning);
    for (size_t i=0;i<loadedGroupList->size();i++)
        loadedGroupList->at(i)->initializeInitialValues(simulationRunning);
    for (size_t i=0;i<loadedIkGroupList->size();i++)
        loadedIkGroupList->at(i)->initializeInitialValues(simulationRunning);
    for (size_t i=0;i<loadedPathPlanningTaskList->size();i++)
        loadedPathPlanningTaskList->at(i)->initializeInitialValues(simulationRunning);
    for (size_t i=0;i<loadedMotionPlanningTaskList->size();i++)
        loadedMotionPlanningTaskList->at(i)->initializeInitialValues(simulationRunning);
    for (size_t i=0;i<loadedLuaScriptList->size();i++)
        loadedLuaScriptList->at(i)->initializeInitialValues(simulationRunning);
    for (size_t i=0;i<loadedConstraintSolverObjectList->size();i++)
        loadedConstraintSolverObjectList->at(i)->initializeInitialValues(simulationRunning);

    // Here we call the initializeInitialValues for all pages & views
    for (size_t i=0;i<loadedObjectList->size();i++)
        App::ct->pageContainer->initializeInitialValues(simulationRunning,loadedObjectList->at(i)->getID());
//**************************************************************************************

    // We select what was loaded if we have a model loaded through the GUI:
    deselectObjects();
    if (model)
    {
        for (size_t i=0;i<loadedObjectList->size();i++)
            addObjectToSelection(loadedObjectList->at(i)->getID());
    }

    // Now display the load operation issues:
    if (_loadOperationIssuesToBeDisplayed.length()!=0)
    {
        size_t startPos=_loadOperationIssuesToBeDisplayed.find("@@REPLACE@@");
        while (startPos!=std::string::npos)
        {
            int handle=_loadOperationIssuesToBeDisplayed_objectHandles[0];
            std::string newTxt("NAME_NOT_FOUND");
            int handle2=getLoadingMapping(&luaScriptMapping,handle);
            CLuaScriptObject* script=App::ct->luaScriptContainer->getScriptFromID(handle2);
            if (script!=NULL)
                newTxt=script->getShortDescriptiveName();
            _loadOperationIssuesToBeDisplayed.replace(startPos,strlen("@@REPLACE@@"),newTxt);
            _loadOperationIssuesToBeDisplayed_objectHandles.erase(_loadOperationIssuesToBeDisplayed_objectHandles.begin(),_loadOperationIssuesToBeDisplayed_objectHandles.begin()+1);
            startPos=_loadOperationIssuesToBeDisplayed.find("@@REPLACE@@");
        }
        int h=simAuxiliaryConsoleOpen_internal("Load operation issues",30,4,NULL,NULL,NULL,NULL);
        simAuxiliaryConsolePrint_internal(h,_loadOperationIssuesToBeDisplayed.c_str());

    }
    _loadOperationIssuesToBeDisplayed.clear();
    _loadOperationIssuesToBeDisplayed_objectHandles.clear();
}

int CObjCont::getSuffixOffsetForObjectToAdd(std::vector<C3DObject*>* loadedObjectList,
                                                 std::vector<CRegCollection*>* loadedGroupList,
                                                 std::vector<CRegCollision*>* loadedCollisionList,
                                                 std::vector<CRegDist*>* loadedDistanceList,
                                                 std::vector<CikGroup*>* loadedIkGroupList,
                                                 std::vector<CPathPlanningTask*>* loadedPathPlanningTaskList,
                                                 std::vector<CMotionPlanningTask*>* loadedMotionPlanningTaskList,
                                                 std::vector<CButtonBlock*>* loadedButtonBlockList,
                                                 std::vector<CLuaScriptObject*>* loadedLuaScriptList,
                                                 std::vector<CConstraintSolverObject*>* loadedConstraintSolverObjectList)
{
    // 1. We find out about the smallest suffix to paste:
    int smallestSuffix=SIM_MAX_INT;
    // 3DObjects:
    for (size_t i=0;i<loadedObjectList->size();i++)
    {
        int s=tt::getNameSuffixNumber(loadedObjectList->at(i)->getName().c_str(),true);
        if (i==0)
            smallestSuffix=s;
        else
        {
            if (s<smallestSuffix)
                smallestSuffix=s;
        }
    }
    // Collections:
    for (size_t i=0;i<loadedGroupList->size();i++)
    {
        int s=tt::getNameSuffixNumber(loadedGroupList->at(i)->getCollectionName().c_str(),true);
        if (s<smallestSuffix)
            smallestSuffix=s;
    }
    // Collisions:
    for (size_t i=0;i<loadedCollisionList->size();i++)
    {
        int s=tt::getNameSuffixNumber(loadedCollisionList->at(i)->getObjectName().c_str(),true);
        if (s<smallestSuffix)
            smallestSuffix=s;
    }
    // Distances:
    for (size_t i=0;i<loadedDistanceList->size();i++)
    {
        int s=tt::getNameSuffixNumber(loadedDistanceList->at(i)->getObjectName().c_str(),true);
        if (s<smallestSuffix)
            smallestSuffix=s;
    }
    // IK Groups:
    for (size_t i=0;i<loadedIkGroupList->size();i++)
    {
        int s=tt::getNameSuffixNumber(loadedIkGroupList->at(i)->getObjectName().c_str(),true);
        if (s<smallestSuffix)
            smallestSuffix=s;
    }
    // Path planning tasks:
    for (size_t i=0;i<loadedPathPlanningTaskList->size();i++)
    {
        int s=tt::getNameSuffixNumber(loadedPathPlanningTaskList->at(i)->getObjectName().c_str(),true);
        if (s<smallestSuffix)
            smallestSuffix=s;
    }
    // Motion planning tasks:
    for (size_t i=0;i<loadedMotionPlanningTaskList->size();i++)
    {
        int s=tt::getNameSuffixNumber(loadedMotionPlanningTaskList->at(i)->getObjectName().c_str(),true);
        if (s<smallestSuffix)
            smallestSuffix=s;
    }
    // 2D Elements:
    for (size_t i=0;i<loadedButtonBlockList->size();i++)
    {
        int s=tt::getNameSuffixNumber(loadedButtonBlockList->at(i)->getBlockName().c_str(),true);
        if (s<smallestSuffix)
            smallestSuffix=s;
    }
    // GCS objects:
    for (size_t i=0;i<loadedConstraintSolverObjectList->size();i++)
    {
        int s=tt::getNameSuffixNumber(loadedConstraintSolverObjectList->at(i)->getObjectName().c_str(),true);
        if (s<smallestSuffix)
            smallestSuffix=s;
    }

    // 2. Now we find out about the highest suffix among existing objects (already in the scene):
    int biggestSuffix,smallestSuffixDummy;

    _getAllObjectsSmallestAndBiggestSuffix(smallestSuffixDummy,biggestSuffix);
    return(biggestSuffix-smallestSuffix+1);
}

bool CObjCont::loadModel(CSer& ar,bool justLoadThumbnail,C7Vector* optionalModelTr,C3Vector* optionalModelBoundingBoxSize,float* optionalModelNonDefaultTranslationStepSize)
{   // There is a similar routine for XML files further down

    bool retVal=loadModelOrScene(ar,true,false,justLoadThumbnail,optionalModelTr,optionalModelBoundingBoxSize,optionalModelNonDefaultTranslationStepSize);
    if (!justLoadThumbnail)
    {
        void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_modelloaded,NULL,NULL,NULL);
        delete[] (char*)returnVal;
        returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_model_loaded,NULL,NULL,NULL); // for backward compatibility
        delete[] (char*)returnVal;

        App::ct->outsideCommandQueue->addCommand(sim_message_model_loaded,0,0,0,0,NULL,0); // only for Lua
        App::ct->setModificationFlag(4); // model loaded
    }
    return(retVal);
}

bool CObjCont::loadModel(const CXmlSer& xmlDoc,const xmlNode* parentNode)
{   // There is a similar routine for binary files further up

    bool retVal=loadModelOrScene(xmlDoc,parentNode,true,false);

    void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_modelloaded,NULL,NULL,NULL);
    delete[] (char*)returnVal;
    returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_model_loaded,NULL,NULL,NULL); // for backward compatibility
    delete[] (char*)returnVal;

    App::ct->outsideCommandQueue->addCommand(sim_message_model_loaded,0,0,0,0,NULL,0); // only for Lua
    App::ct->setModificationFlag(4); // model loaded
    return(retVal);
}

C3DObject* CObjCont::load3DObject(CSer& ar,std::string theName,bool &noHit)
{
    int byteQuantity;
    if (theName.compare(SER_SHAPE)==0)
    {
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        CShape* myNewObject=new CShape();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    if (theName.compare(SER_JOINT)==0)
    {
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        CJoint* myNewObject=new CJoint();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    if (theName.compare(SER_GRAPH)==0)
    {
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        CGraph* myNewObject=new CGraph();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    if (theName.compare(SER_CAMERA)==0)
    {
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        CCamera* myNewObject=new CCamera();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    if (theName.compare(SER_LIGHT)==0)
    {
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        CLight* myNewObject=new CLight();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    if (theName.compare(SER_MIRROR)==0)
    {
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        CMirror* myNewObject=new CMirror();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    if (theName.compare(SER_OCTREE)==0)
    {
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        COctree* myNewObject=new COctree();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    if (theName.compare(SER_POINTCLOUD)==0)
    {
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        CPointCloud* myNewObject=new CPointCloud();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    if (theName.compare(SER_DUMMY)==0)
    {
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        CDummy* myNewObject=new CDummy();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    if (theName.compare(SER_PROXIMITYSENSOR)==0)
    {
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        CProxSensor* myNewObject=new CProxSensor();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    if (theName.compare(SER_VISIONSENSOR)==0)
    {
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        CVisionSensor* myNewObject=new CVisionSensor();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    if (theName.compare(SER_PATH)==0)
    {
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        CPath* myNewObject=new CPath();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    if (theName.compare(SER_MILL)==0)
    {
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        CMill* myNewObject=new CMill();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    if (theName.compare(SER_FORCESENSOR)==0)
    { 
        ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
        CForceSensor* myNewObject=new CForceSensor();
        myNewObject->serialize(ar);
        noHit=false;
        return(myNewObject);
    }
    // If we arrived here it means that maybe we have a new 3DObject type that this V-REP doesn't understand yet. 
    // We try to replace it with a dummy (2009/12/09):
    unsigned char dat[14];
    if (ar.readBytesButKeepPointerUnchanged(dat,14)!=14)
        return(NULL); // No, this is not a 3DObject! (not enough to read)
    if ((dat[4]=='3')&&(dat[5]=='d')&&(dat[6]=='o')&&(dat[11]==57)&&(dat[12]==58)&&(dat[13]==59))
    { // yes we have a 3DObject of an unknown type!
        ar >> byteQuantity; // Undo/redo will never arrive here
        CDummy* newUnknownType=new CDummy();
        newUnknownType->loadUnknownObjectType(ar);
        noHit=false;
        return(newUnknownType);
    }
    return(NULL); // No, this is not a 3DObject!
}

bool CObjCont::loadScene(CSer& ar,bool forUndoRedoOperation)
{   // We have another similar routine for XML files further down
    removeAllObjects();
    bool retVal=loadModelOrScene(ar,false,true,false,NULL,NULL,NULL);
    if (!forUndoRedoOperation)
    {
        void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_sceneloaded,NULL,NULL,NULL);
        delete[] (char*)returnVal;
        App::ct->setModificationFlag(8); // scene loaded
        App::ct->outsideCommandQueue->addCommand(sim_message_scene_loaded,0,0,0,0,NULL,0);
    }
    return(retVal);
}

bool CObjCont::loadScene(const CXmlSer& xmlDoc,const xmlNode* parentNode)
{   // We have another similar routine for binary files further up
    removeAllObjects();
    bool retVal=loadModelOrScene(xmlDoc,parentNode,false,true);
    void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_sceneloaded,NULL,NULL,NULL);
    delete[] (char*)returnVal;
    App::ct->setModificationFlag(8); // scene loaded
    App::ct->outsideCommandQueue->addCommand(sim_message_scene_loaded,0,0,0,0,NULL,0);
    return(retVal);
}

void CObjCont::store3DObject(CSer& ar,C3DObject* it)
{
    if (it->getObjectType()==sim_object_shape_type)
        ar.storeDataName(SER_SHAPE);
    if (it->getObjectType()==sim_object_joint_type)
        ar.storeDataName(SER_JOINT);
    if (it->getObjectType()==sim_object_graph_type)
        ar.storeDataName(SER_GRAPH);
    if (it->getObjectType()==sim_object_camera_type)
        ar.storeDataName(SER_CAMERA);
    if (it->getObjectType()==sim_object_light_type)
        ar.storeDataName(SER_LIGHT);
    if (it->getObjectType()==sim_object_mirror_type)
        ar.storeDataName(SER_MIRROR);
    if (it->getObjectType()==sim_object_octree_type)
        ar.storeDataName(SER_OCTREE);
    if (it->getObjectType()==sim_object_pointcloud_type)
        ar.storeDataName(SER_POINTCLOUD);
    if (it->getObjectType()==sim_object_dummy_type)
        ar.storeDataName(SER_DUMMY);
    if (it->getObjectType()==sim_object_proximitysensor_type)
        ar.storeDataName(SER_PROXIMITYSENSOR);
    if (it->getObjectType()==sim_object_visionsensor_type)
        ar.storeDataName(SER_VISIONSENSOR);
    if (it->getObjectType()==sim_object_path_type)
        ar.storeDataName(SER_PATH);
    if (it->getObjectType()==sim_object_mill_type)
        ar.storeDataName(SER_MILL);
    if (it->getObjectType()==sim_object_forcesensor_type)
        ar.storeDataName(SER_FORCESENSOR);
    ar.setCountingMode();
    it->serialize(ar);
    if (ar.setWritingMode())
        it->serialize(ar);
}

C3DObject* CObjCont::getObjectWithUniqueID(int uniqueID)
{
    for (int i=0;i<int(objectList.size());i++)
    {
        C3DObject* it=getObject(objectList[i]);
        if (it!=NULL)
        {
            if (it->getUniqueID()==uniqueID)
                return(it);
        }
    }
    return(NULL);
}

std::string CObjCont::getSimilarNameWithHighestSuffix(std::string objectName,bool dash)
{
    std::string nameWithoutSuffix(tt::getNameWithoutSuffixNumber(objectName.c_str(),dash));
    int highestSuffixNb=-2;
    std::string highestSuffixName;
    for (int i=0;i<int(objectList.size());i++)
    {
        C3DObject* it=getObject(objectList[i]);
        if (it!=NULL)
        {
            std::string nm(it->getName());
            if (tt::getNameWithoutSuffixNumber(nm.c_str(),dash).compare(nameWithoutSuffix)==0)
            {
                int suffixNb=tt::getNameSuffixNumber(nm.c_str(),dash);
                if (suffixNb>highestSuffixNb)
                {
                    highestSuffixNb=suffixNb;
                    highestSuffixName=nm;
                }
            }
        }
    }
    if (highestSuffixNb==-2)
        return(""); // none
    return(highestSuffixName);
}

void CObjCont::saveScene(CSer& ar)
{ // There is another similar routine for XML files further down

    // **** Following needed to save existing calculation structures:
    App::ct->environment->setSaveExistingCalculationStructuresTemp(false);
    if (!App::ct->undoBufferContainer->isUndoSavingOrRestoringUnderWay())
    { // real saving!
        if (App::ct->environment->getSaveExistingCalculationStructures())
        {
            // removed on 10/9/2014 App::ct->environment->setSaveExistingCalculationStructures(false); // we clear that flag
            App::ct->environment->setSaveExistingCalculationStructuresTemp(true);
        }
    }
    // ************************************************************

    selectedObjectsWhenSaving.clear();

    //***************************************************
    ar.storeDataName(SER_MODEL_THUMBNAIL);
    ar.setCountingMode();
    App::ct->environment->modelThumbnail_notSerializedHere.serialize(ar,false);
    if (ar.setWritingMode())
        App::ct->environment->modelThumbnail_notSerializedHere.serialize(ar,false);
    //****************************************************

    // Textures:
    int textCnt=0;
    while (App::ct->textureCont->getObjectAtIndex(textCnt)!=NULL)
    {
        CTextureObject* it=App::ct->textureCont->getObjectAtIndex(textCnt);
        App::ct->textureCont->storeTextureObject(ar,it);
        textCnt++;
    }

    // DynMaterial objects:
    // We only save this for backward compatibility, but not needed for V-REP's from 3.4.0 on:
    //------------------------------------------------------------
    int dynObjId=SIM_IDSTART_DYNMATERIAL_OLD;
    for (size_t i=0;i<shapeList.size();i++)
    {
        CShape* it=getShape(shapeList[i]);
        CDynMaterialObject* mat=it->getDynMaterial();
        it->geomData->geomInfo->setDynMaterialId_OLD(dynObjId);
        mat->setObjectID(dynObjId++);
        ar.storeDataName(SER_DYNMATERIAL);
        ar.setCountingMode();
        mat->serialize(ar);
        if (ar.setWritingMode())
            mat->serialize(ar);
    }
    //------------------------------------------------------------

    // Handle the heavy data here so we don't have duplicates (vertices, indices, normals and edges):
    //------------------------------------------------------------
    CGeometric::clearTempVerticesIndicesNormalsAndEdges();
    for (size_t i=0;i<shapeList.size();i++)
    {
        CShape* it=getShape(shapeList[i]);
        it->prepareVerticesIndicesNormalsAndEdgesForSerialization();
    }
    ar.storeDataName(SER_VERTICESINDICESNORMALSEDGES);
    ar.setCountingMode();
    CGeometric::serializeTempVerticesIndicesNormalsAndEdges(ar);
    if (ar.setWritingMode())
        CGeometric::serializeTempVerticesIndicesNormalsAndEdges(ar);
    CGeometric::clearTempVerticesIndicesNormalsAndEdges();
    //------------------------------------------------------------

    for (int i=0;i<int(objectList.size());i++)
    {
        C3DObject* it=getObject(objectList[i]);
        store3DObject(ar,it);
    }

    ar.storeDataName(SER_GHOSTS);
    ar.setCountingMode();
    App::ct->ghostObjectCont->serialize(ar);
    if (ar.setWritingMode())
        App::ct->ghostObjectCont->serialize(ar);

    ar.storeDataName(SER_ENVIRONMENT);
    ar.setCountingMode();
    App::ct->environment->serialize(ar,true);
    if (ar.setWritingMode())
        App::ct->environment->serialize(ar,true);

    for (int i=0;i<int(App::ct->collisions->collisionObjects.size());i++)
    {
        ar.storeDataName(SER_COLLISION);
        ar.setCountingMode();
        App::ct->collisions->collisionObjects[i]->serialize(ar);
        if (ar.setWritingMode())
            App::ct->collisions->collisionObjects[i]->serialize(ar);
    }
    for (int i=0;i<int(App::ct->distances->distanceObjects.size());i++)
    {
        ar.storeDataName(SER_DISTANCE);
        ar.setCountingMode();
        App::ct->distances->distanceObjects[i]->serialize(ar);
        if (ar.setWritingMode())
            App::ct->distances->distanceObjects[i]->serialize(ar);
    }
    for (int i=0;i<int(App::ct->ikGroups->ikGroups.size());i++)
    {
        ar.storeDataName(SER_IK);
        ar.setCountingMode();
        App::ct->ikGroups->ikGroups[i]->serialize(ar);
        if (ar.setWritingMode())
            App::ct->ikGroups->ikGroups[i]->serialize(ar);
    }
    for (int i=0;i<int(App::ct->pathPlanning->allObjects.size());i++)
    {
        ar.storeDataName(SER_PATH_PLANNING);
        ar.setCountingMode();
        App::ct->pathPlanning->allObjects[i]->serialize(ar);
        if (ar.setWritingMode())
            App::ct->pathPlanning->allObjects[i]->serialize(ar);
    }
    for (int i=0;i<int(App::ct->motionPlanning->allObjects.size());i++)
    {
        ar.storeDataName(SER_MOTION_PLANNING);
        ar.setCountingMode();
        App::ct->motionPlanning->allObjects[i]->serialize(ar);
        if (ar.setWritingMode())
            App::ct->motionPlanning->allObjects[i]->serialize(ar);
    }

    ar.storeDataName(SER_SETTINGS);
    ar.setCountingMode();
    App::ct->mainSettings->serialize(ar);
    if (ar.setWritingMode())
        App::ct->mainSettings->serialize(ar);

    ar.storeDataName(SER_DYNAMICS);
    ar.setCountingMode();
    App::ct->dynamicsContainer->serialize(ar);
    if (ar.setWritingMode())
        App::ct->dynamicsContainer->serialize(ar);

    ar.storeDataName(SER_SIMULATION);
    ar.setCountingMode();
    App::ct->simulation->serialize(ar);
    if (ar.setWritingMode())
        App::ct->simulation->serialize(ar);

    ar.storeDataName(SER_SCENE_CUSTOM_DATA);
    ar.setCountingMode();
    App::ct->customSceneData->serializeData(ar);
    if (ar.setWritingMode())
        App::ct->customSceneData->serializeData(ar);

    ar.storeDataName(SER_VIEWS);
    ar.setCountingMode();
    App::ct->pageContainer->serialize(ar);
    if (ar.setWritingMode())
        App::ct->pageContainer->serialize(ar);

    // ******* FOLLOWING FOR FORWARD COMPATIBILITY, SO THAT OLD V_REP VERSIONS CAN READ NEWER FILES (2010/07/14) *******************
    // We serialize the rendering order:
    ar.storeDataName(SER_ORDER_OLD_FORWARD_COMPATIBILITY_2010_07_14);
    for (int i=0;i<int(objectList.size());i++)
        ar << objectList[i];
    ar.flush();
    // ******************************************************************************************************************************
    
    // We serialize all groups:
    for (int i=0;i<int(App::ct->collections->allCollections.size());i++)
    {
        ar.storeDataName(SER_GROUP);
        ar.setCountingMode();
        App::ct->collections->allCollections[i]->serialize(ar);
        if (ar.setWritingMode())
            App::ct->collections->allCollections[i]->serialize(ar);
    }

    // We serialize the buttonBlocks (non-system):
    for (int i=0;i<int(App::ct->buttonBlockContainer->allBlocks.size());i++)
    {
        CButtonBlock* bblk=App::ct->buttonBlockContainer->allBlocks[i];
        if ((bblk->getAttributes()&sim_ui_property_systemblock)==0)
        {
            ar.storeDataName(SER_BUTTON_BLOCK);
            ar.setCountingMode();
            bblk->serialize(ar);
            if (ar.setWritingMode())
                bblk->serialize(ar);
        }
    }

    // We serialize the lua script objects (not the add-on scripts!):
    for (size_t i=0;i<App::ct->luaScriptContainer->allScripts.size();i++)
    {
        CLuaScriptObject* it=App::ct->luaScriptContainer->allScripts[i];
        if (it->isSceneScript())
        {
            ar.storeDataName(SER_LUA_SCRIPT);
            ar.setCountingMode();
            it->serialize(ar);
            if (ar.setWritingMode())
                it->serialize(ar);
        }
    }

    // We serialize the GCS objects (all of them):
    for (int i=0;i<int(App::ct->constraintSolver->allGcsObjects.size());i++)
    {
        CConstraintSolverObject* it=App::ct->constraintSolver->allGcsObjects[i];
        ar.storeDataName(SER_GEOMETRIC_CONSTRAINT_OBJECT);
        ar.setCountingMode();
        it->serialize(ar);
        if (ar.setWritingMode())
            it->serialize(ar);
    }

    ar.storeDataName(SER_END_OF_FILE);
    CGeometric::clearTempVerticesIndicesNormalsAndEdges();
}

void CObjCont::saveScene(CXmlSer& xmlDoc,xmlNode* parentNode)
{ // There is another similar routine for binary files further up

    // **** Following needed to save existing calculation structures:
    App::ct->environment->setSaveExistingCalculationStructuresTemp(false);

    if (App::ct->environment->getSaveExistingCalculationStructures())
    {
        // removed on 10/9/2014 App::ct->environment->setSaveExistingCalculationStructures(false); // we clear that flag
        App::ct->environment->setSaveExistingCalculationStructuresTemp(true);
    }
    // ************************************************************

    selectedObjectsWhenSaving.clear();

    CGeometric::clearTempVerticesIndicesNormalsAndEdges();
}

void CObjCont::exportIkContent(CExtIkSer& ar)
{
    ar.writeInt(0); // this is the ext IK serialization version. Not forward nor backward compatible!

    ar.writeInt(int(objectList.size()));

    for (int i=0;i<int(objectList.size());i++)
    {
        C3DObject* it=getObject(objectList[i]);

        ar.writeInt(it->getObjectType());

        if (it->getObjectType()==sim_object_shape_type)
            ((CShape*)it)->serializeWExtIk(ar);
        if (it->getObjectType()==sim_object_joint_type)
            ((CJoint*)it)->serializeWExtIk(ar);
        if (it->getObjectType()==sim_object_graph_type)
            ((CGraph*)it)->serializeWExtIk(ar);
        if (it->getObjectType()==sim_object_camera_type)
            ((CCamera*)it)->serializeWExtIk(ar);
        if (it->getObjectType()==sim_object_dummy_type)
            ((CDummy*)it)->serializeWExtIk(ar);
        if (it->getObjectType()==sim_object_octree_type)
            ((COctree*)it)->serializeWExtIk(ar);
        if (it->getObjectType()==sim_object_pointcloud_type)
            ((CPointCloud*)it)->serializeWExtIk(ar);
        if (it->getObjectType()==sim_object_proximitysensor_type)
            ((CProxSensor*)it)->serializeWExtIk(ar);
        if (it->getObjectType()==sim_object_path_type)
            ((CPath*)it)->serializeWExtIk(ar);
        if (it->getObjectType()==sim_object_visionsensor_type)
            ((CVisionSensor*)it)->serializeWExtIk(ar);
        if (it->getObjectType()==sim_object_mill_type)
            ((CMill*)it)->serializeWExtIk(ar);
        if (it->getObjectType()==sim_object_forcesensor_type)
            ((CForceSensor*)it)->serializeWExtIk(ar);
        if (it->getObjectType()==sim_object_light_type)
            ((CLight*)it)->serializeWExtIk(ar);
        if (it->getObjectType()==sim_object_mirror_type)
            ((CMirror*)it)->serializeWExtIk(ar);
    }

    ar.writeInt(int(App::ct->ikGroups->ikGroups.size()));

    for (int i=0;i<int(App::ct->ikGroups->ikGroups.size());i++)
        App::ct->ikGroups->ikGroups[i]->serializeWExtIk(ar);

}

bool CObjCont::getAllShapesFromScene(std::vector<C3DObject*>& objects,int propMask)
{
    for (int i=0;i<int(shapeList.size());i++)
    {
        CShape* it=getShape(shapeList[i]);
        if ( (it->getCumulativeObjectSpecialProperty()&propMask)||(propMask==-1) )
            objects.push_back(it);
    }
    return(true);
}

bool CObjCont::getAllShapesAndDummiesFromScene(std::vector<C3DObject*>& objects,int propMask)
{
    for (int i=0;i<int(dummyList.size());i++)
    {
        CDummy* it=getDummy(dummyList[i]);
        if ( (it->getCumulativeObjectSpecialProperty()&propMask)||(propMask==-1) )
            objects.push_back(it);
    }
    for (int i=0;i<int(shapeList.size());i++)
    {
        CShape* it=getShape(shapeList[i]);
        if ( (it->getCumulativeObjectSpecialProperty()&propMask)||(propMask==-1) )
            objects.push_back(it);
    }
    return(true);
}

void CObjCont::getAllCollidableObjectsFromSceneExcept(const std::vector<C3DObject*>* exceptionObjects,std::vector<C3DObject*>& objects)
{
    for (size_t i=0;i<objectList.size();i++)
    {
        C3DObject* it=getObject(objectList[i]);
        if (it->isPotentiallyCollidable())
        {
            if (it->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_collidable)
            { // Make sure we don't have it in the exception list:
                bool okToAdd=true;
                if (exceptionObjects!=NULL)
                {
                    for (size_t j=0;j<exceptionObjects->size();j++)
                    {
                        if (exceptionObjects->at(j)==it)
                        {
                            okToAdd=false;
                            break;
                        }
                    }
                }
                if (okToAdd)
                    objects.push_back(it);
            }
        }
    }
}

void CObjCont::getAllMeasurableObjectsFromSceneExcept(const std::vector<C3DObject*>* exceptionObjects,std::vector<C3DObject*>& objects)
{
    for (size_t i=0;i<objectList.size();i++)
    {
        C3DObject* it=getObject(objectList[i]);
        if (it->isPotentiallyMeasurable())
        {
            if (it->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_measurable)
            { // Make sure we don't have it in the exception list:
                bool okToAdd=true;
                if (exceptionObjects!=NULL)
                {
                    for (size_t j=0;j<exceptionObjects->size();j++)
                    {
                        if (exceptionObjects->at(j)==it)
                        {
                            okToAdd=false;
                            break;
                        }
                    }
                }
                if (okToAdd)
                    objects.push_back(it);
            }
        }
    }
}

void CObjCont::getAllDetectableObjectsFromSceneExcept(const std::vector<C3DObject*>* exceptionObjects,std::vector<C3DObject*>& objects,int detectableMask)
{
    for (size_t i=0;i<objectList.size();i++)
    {
        C3DObject* it=getObject(objectList[i]);
        if (it->isPotentiallyDetectable())
        {
            if ( (it->getCumulativeObjectSpecialProperty()&detectableMask)||(detectableMask==-1) )
            { // Make sure we don't have it in the exception list:
                bool okToAdd=true;
                if (exceptionObjects!=NULL)
                {
                    for (size_t j=0;j<exceptionObjects->size();j++)
                    {
                        if (exceptionObjects->at(j)==it)
                        {
                            okToAdd=false;
                            break;
                        }
                    }
                }
                if (okToAdd)
                    objects.push_back(it);
            }
        }
    }
}

bool CObjCont::getAllShapesAndDummiesFromSceneExcept(std::vector<C3DObject*>& exceptionObjects,std::vector<C3DObject*>& objects,int propMask)
{
    for (int i=0;i<int(dummyList.size());i++)
    {
        CDummy* it=getDummy(dummyList[i]);
        if ( (it->getCumulativeObjectSpecialProperty()&propMask)||(propMask==-1) )
        { // Make sure we don't have it in the exception list:
            bool okToAdd=true;
            for (int j=0;j<int(exceptionObjects.size());j++)
            {
                if (exceptionObjects[j]==it)
                {
                    okToAdd=false;
                    break;
                }
            }
            if (okToAdd)
                objects.push_back(it);
        }
    }
    for (int i=0;i<int(shapeList.size());i++)
    {
        CShape* it=getShape(shapeList[i]);
        if ( (it->getCumulativeObjectSpecialProperty()&propMask)||(propMask==-1) )
        { // Make sure we don't have it in the exception list:
            bool okToAdd=true;
            for (int j=0;j<int(exceptionObjects.size());j++)
            {
                if (exceptionObjects[j]==it)
                {
                    okToAdd=false;
                    break;
                }
            }
            if (okToAdd)
                objects.push_back(it);
        }
    }
    return(true);
}

void CObjCont::getSelectedObjects(std::vector<C3DObject*>& selection)
{
    selection.clear();
    for (int i=0;i<int(_selectedObjectIDs.size());i++)
    {
        C3DObject* it=getObject(_selectedObjectIDs[i]);
        if (it!=NULL)
            selection.push_back(it);
    }
}

void CObjCont::getSelectedObjects(std::vector<int>& selection)
{
    selection.clear();
    selection.assign(_selectedObjectIDs.begin(),_selectedObjectIDs.end());
}

void CObjCont::setSelectedObjects(const std::vector<int>& selection)
{
    deselectObjects();
    for (size_t i=0;i<selection.size();i++)
        addObjectToSelection(selection[i]);
}

C3DObject* CObjCont::getSelectedObject()
{
    if (getSelSize()!=0)
        return(_objectIndex[getSelID(getSelSize()-1)]);
    return(NULL);
}

void CObjCont::xorAddObjectToSelection(int identifier)
{
    if (identifier!=-1)
    {
        if (identifier>=NON_OBJECT_PICKING_ID_PATH_PTS_START) // individual path points!
        {
#ifdef SIM_WITH_GUI
            if (App::mainWindow!=NULL)
                App::mainWindow->editModeContainer->pathPointManipulation->xorAddPathPointToSelection_nonEditMode(identifier);
#endif
        }
        else
        {
            if (!isObjectSelected(identifier))
            {
                _selectedObjectIDs.push_back(identifier);
                _selectedObjectsBool[identifier>>3]=_selectedObjectsBool[identifier>>3]|(1<<(identifier&7));
                App::ct->collections->mark(identifier); //For groups
                App::ct->buttonBlockContainer->a3DObjectWasSelected(identifier);
            }
            else
            {
                int i;
                for (i=0;i<getSelSize();i++)
                {
                    if (getSelID(i)==identifier)
                        break;
                }
                _selectedObjectIDs.erase(_selectedObjectIDs.begin()+i);
                _selectedObjectsBool[identifier>>3]=_selectedObjectsBool[identifier>>3]&(255-(1<<(identifier&7)));
                App::ct->collections->unmark(identifier);   //For groups    
            }
#ifdef SIM_WITH_GUI
            if (App::mainWindow!=NULL)
                App::mainWindow->editModeContainer->announceObjectSelectionChanged();
#endif
        }
    }
    else
    {
        _selectedObjectIDs.clear();
        for (int i=0;i<int(_selectedObjectsBool.size());i++)
            _selectedObjectsBool[i]=0;
        App::ct->collections->unmarkAll();      //For groups
#ifdef SIM_WITH_GUI
        if (App::mainWindow!=NULL)
            App::mainWindow->editModeContainer->announceObjectSelectionChanged();
#endif
    }
    App::setLightDialogRefreshFlag();
}

void CObjCont::addObjectToSelection(int identifier)
{
    if (identifier>=0)
    {
        if (identifier>=NON_OBJECT_PICKING_ID_PATH_PTS_START) // individual path points!
        {
#ifdef SIM_WITH_GUI
            if (App::mainWindow!=NULL)
                App::mainWindow->editModeContainer->pathPointManipulation->addPathPointToSelection_nonEditMode(identifier);
#endif
        }
        else
        {
            if (getObject(identifier)!=NULL)
            {
                if (!isObjectSelected(identifier))
                {
                    _selectedObjectIDs.push_back(identifier);
                    _selectedObjectsBool[identifier>>3]=_selectedObjectsBool[identifier>>3]|(1<<(identifier&7));
                    App::ct->collections->mark(identifier); //For groups
                    App::ct->buttonBlockContainer->a3DObjectWasSelected(identifier);
#ifdef SIM_WITH_GUI
                    if (App::mainWindow!=NULL)
                        App::mainWindow->editModeContainer->announceObjectSelectionChanged();
#endif
                }
            }
        }
    }
    App::setLightDialogRefreshFlag();
}



void CObjCont::removeObjectFromSelection(int identifier)
{
    for (int i=0;i<getSelSize();i++)
    {
        if (getSelID(i)==identifier)
        {
            _selectedObjectIDs.erase(_selectedObjectIDs.begin()+i);
            _selectedObjectsBool[identifier>>3]=_selectedObjectsBool[identifier>>3]&(255-(1<<(identifier&7)));
            App::ct->collections->unmark(identifier);   //For groups
#ifdef SIM_WITH_GUI
            if (App::mainWindow!=NULL)
                App::mainWindow->editModeContainer->announceObjectSelectionChanged();
#endif
            break;
        }
    }
    App::setLightDialogRefreshFlag();
}

int CObjCont::getSelSize()
{
    return(_selectedObjectIDs.size());
}
int CObjCont::getSelID(int i)
{
    return(_selectedObjectIDs[i]);
}


bool CObjCont::selectObject(int objectIdentifier)
// No checking if object really exists.
// -1 is a valid parameter (no object is selected)
{
    _selectedObjectIDs.clear();
    for (int i=0;i<int(_selectedObjectsBool.size());i++)
        _selectedObjectsBool[i]=0;
    App::ct->collections->unmarkAll();          //For groups
    if (objectIdentifier!=-1)
    {
        _selectedObjectIDs.push_back(objectIdentifier);
        _selectedObjectsBool[objectIdentifier>>3]=_selectedObjectsBool[objectIdentifier>>3]|(1<<(objectIdentifier&7));
        App::ct->collections->mark(objectIdentifier);       //For groups
    }
#ifdef SIM_WITH_GUI
    if (App::mainWindow!=NULL)
        App::mainWindow->editModeContainer->announceObjectSelectionChanged();
#endif
    App::setLightDialogRefreshFlag();
    return(true);
}

void CObjCont::selectAllObjects()
{
    _selectedObjectIDs.clear();
    for (int i=0;i<int(_selectedObjectsBool.size());i++)
        _selectedObjectsBool[i]=0;
    App::ct->collections->unmarkAll();
    for (int i=0;i<int(objectList.size());i++)
    {
        _selectedObjectIDs.push_back(objectList[i]);
        _selectedObjectsBool[objectList[i]>>3]=_selectedObjectsBool[objectList[i]>>3]|(1<<(objectList[i]&7));
        App::ct->collections->mark(objectList[i]);
    }
#ifdef SIM_WITH_GUI
    if (App::mainWindow!=NULL)
        App::mainWindow->editModeContainer->announceObjectSelectionChanged();
#endif
    App::setLightDialogRefreshFlag();
}

void CObjCont::deselectObjects()
{
    _selectedObjectIDs.clear();
    for (int i=0;i<int(_selectedObjectsBool.size());i++)
        _selectedObjectsBool[i]=0;
    App::ct->collections->unmarkAll();
#ifdef SIM_WITH_GUI
    if (App::mainWindow!=NULL)
        App::mainWindow->editModeContainer->announceObjectSelectionChanged();
#endif
    App::setLightDialogRefreshFlag();
}

bool CObjCont::isSelectionSame(std::vector<int>& sel,bool actualize)
{
    if (_selectedObjectIDs.size()!=sel.size())
    {
        if (actualize)
        {
            sel.clear();
            sel.assign(_selectedObjectIDs.begin(),_selectedObjectIDs.end());
        }
        return(false);
    }
    for (int i=0;i<int(_selectedObjectIDs.size());i++)
    {
        if (_selectedObjectIDs[i]!=sel[i])
        {
            if (actualize)
            {
                sel.clear();
                sel.assign(_selectedObjectIDs.begin(),_selectedObjectIDs.end());
            }
            return(false);
        }
    }
    return(true);
}

CShape* CObjCont::getShape(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_shape_type)
        return(NULL);
    return((CShape*)_objectIndex[identifier]);
}

CMirror* CObjCont::getMirror(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_mirror_type)
        return(NULL);
    return((CMirror*)_objectIndex[identifier]);
}

COctree* CObjCont::getOctree(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_octree_type)
        return(NULL);
    return((COctree*)_objectIndex[identifier]);
}

CPointCloud* CObjCont::getPointCloud(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_pointcloud_type)
        return(NULL);
    return((CPointCloud*)_objectIndex[identifier]);
}

CProxSensor* CObjCont::getProximitySensor(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_proximitysensor_type)
        return(NULL);
    return((CProxSensor*)_objectIndex[identifier]);
}
CVisionSensor* CObjCont::getVisionSensor(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_visionsensor_type)
        return(NULL);
    return((CVisionSensor*)_objectIndex[identifier]);
}
CPath* CObjCont::getPath(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_path_type)
        return(NULL);
    return((CPath*)_objectIndex[identifier]);
}
CMill* CObjCont::getMill(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_mill_type)

        return(NULL);
    return((CMill*)_objectIndex[identifier]);
}
CForceSensor* CObjCont::getForceSensor(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_forcesensor_type)
        return(NULL);
    return((CForceSensor*)_objectIndex[identifier]);
}
CCamera* CObjCont::getCamera(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_camera_type)
        return(NULL);
    return((CCamera*)_objectIndex[identifier]);
}
CLight* CObjCont::getLight(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_light_type)
        return(NULL);
    return((CLight*)_objectIndex[identifier]);
}

CGraph* CObjCont::getGraph(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_graph_type)
        return(NULL);
    return((CGraph*)_objectIndex[identifier]);
}

//------------------ Object destruction announcement -------------------------

void CObjCont::announceCollectionWillBeErased(int groupID)
{
    for (int i=0;i<int(objectList.size());i++)
        getObject(objectList[i])->announceCollectionWillBeErased(groupID,false); // this never triggers 3DObject destruction!
    App::ct->collisions->announceCollectionWillBeErased(groupID); // This can trigger a collision destruction!
    App::ct->distances->announceCollectionWillBeErased(groupID); // This can trigger a distance destruction!
    App::ct->ikGroups->announceCollectionWillBeErased(groupID); // This will never trigger an IK group destruction! (at least for now!)
    App::ct->pathPlanning->announceCollectionWillBeErased(groupID); // This can trigger a path planning object destruction!
    App::ct->motionPlanning->announceCollectionWillBeErased(groupID); // This can trigger a motion planning object destruction!
}

void CObjCont::announceGcsObjectWillBeErased(int gcsObjectID)
{
    for (int i=0;i<int(objectList.size());i++)
        getObject(objectList[i])->announceGcsObjectWillBeErased(gcsObjectID,false); // this never triggers 3DObject destruction!
}

void CObjCont::announceCollisionWillBeErased(int collisionID)
{
    for (int i=0;i<int(objectList.size());i++)
        getObject(objectList[i])->announceCollisionWillBeErased(collisionID,false); // this never triggers 3DObject destruction!
}
void CObjCont::announceDistanceWillBeErased(int distanceID)
{
    for (int i=0;i<int(objectList.size());i++)
        getObject(objectList[i])->announceDistanceWillBeErased(distanceID,false); // this never triggers 3DObject destruction!
}

void CObjCont::announce2DElementWillBeErased(int elementID)
{
    if (App::ct->textureCont!=NULL)
        App::ct->textureCont->announceGeneralObjectWillBeErased(elementID,-1);
}

void CObjCont::announce2DElementButtonWillBeErased(int elementID,int buttonID)
{
    if (App::ct->textureCont!=NULL)
        App::ct->textureCont->announceGeneralObjectWillBeErased(elementID,buttonID);
}

void CObjCont::actualizeMechanismIDs()
{
    for (int i=0;i<int(objectList.size());i++)
    {
        C3DObject* it=getObject(objectList[i]);
        if (it->getParent()==NULL)
        {
            CGeometricConstraintSolverInt work(it->getID(),-1); 
            int mechanismID=work.getMechanismID();
            std::vector<C3DObject*> childrenAndSelf;
            it->getAllObjectsRecursive(&childrenAndSelf,true);
            for (int j=0;j<int(childrenAndSelf.size());j++)
                childrenAndSelf[j]->setMechanismID(mechanismID);
        }
    }
}

//----------------------------------------------------------------------------
void CObjCont::setAbsoluteAngle(int identifier,float angle,int index)
{ // Angle is specified in radian!
    C3DObject* it=getObject(identifier);
    if (it==NULL)
        return;
    C7Vector cumul(it->getCumulativeTransformationPart1());
    C7Vector parentInverse(it->getParentCumulativeTransformation().getInverse());
    C3Vector euler=cumul.Q.getEulerAngles();
    euler(index)=angle;
    cumul.Q.setEulerAngles(euler);
    it->setLocalTransformation(parentInverse*cumul);
}

void CObjCont::setAbsolutePosition(int identifier,float pos,int index)
{
    C3DObject* it=getObject(identifier);
    if (it==NULL)
        return;
    C7Vector cumul(it->getCumulativeTransformationPart1());
    C7Vector parentInverse(it->getParentCumulativeTransformation().getInverse());
    cumul.X(index)=pos;
    it->setLocalTransformation(parentInverse*cumul);
}

void CObjCont::setAbsolutePosition(int identifier,const C3Vector& p)
{
    C3DObject* it=getObject(identifier);
    if (it==NULL)
        return;
    C7Vector cumul(it->getCumulativeTransformationPart1());
    C7Vector parentInverse(it->getParentCumulativeTransformation().getInverse());
    cumul.X=p;
    it->setLocalTransformation(parentInverse*cumul);
}

void CObjCont::setAbsoluteAngles(int identifier,const C3Vector& euler)
{   // Here euler elements are in radians!!!
    C3DObject* it=getObject(identifier);
    if (it==NULL)
        return;
    C7Vector cumul(it->getCumulativeTransformationPart1());
    C7Vector parentInverse(it->getParentCumulativeTransformation().getInverse());
    cumul.Q.setEulerAngles(euler);
    it->setLocalTransformation(parentInverse*cumul);
}

int CObjCont::getSimpleShapeNumberInSelection()
{
    return(getSimpleShapeNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getShapeNumberInSelection()
{
    return(getShapeNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getMirrorNumberInSelection()
{
    return(getMirrorNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getOctreeNumberInSelection()
{
    return(getOctreeNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getPointCloudNumberInSelection()
{
    return(getPointCloudNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getJointNumberInSelection()
{
    return(getJointNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getCompoundNumberInSelection()
{
    return(getCompoundNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getGraphNumberInSelection()
{
    return(getGraphNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getCameraNumberInSelection()
{
    return(getCameraNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getLightNumberInSelection()
{
    return(getLightNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getDummyNumberInSelection()
{
    return(getDummyNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getProxSensorNumberInSelection()
{
    return(getProxSensorNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getVisionSensorNumberInSelection()
{
    return(getVisionSensorNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getPathNumberInSelection()
{
    return(getPathNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getMillNumberInSelection()
{
    return(getMillNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getForceSensorNumberInSelection()
{
    return(getForceSensorNumberInSelection(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAShape()
{
    return(isLastSelectionAShape(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAMirror()
{
    return(isLastSelectionAMirror(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAnOctree()
{
    return(isLastSelectionAnOctree(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAPointCloud()
{
    return(isLastSelectionAPointCloud(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionASimpleShape()
{
    return(isLastSelectionASimpleShape(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAJoint()
{
    return(isLastSelectionAJoint(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionACompound()
{
    return(isLastSelectionACompound(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAGraph()
{
    return(isLastSelectionAGraph(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionACamera()
{
    return(isLastSelectionACamera(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionALight()
{
    return(isLastSelectionALight(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionADummy()
{
    return(isLastSelectionADummy(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAProxSensor()
{
    return(isLastSelectionAProxSensor(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAVisionSensor()
{
    return(isLastSelectionAVisionSensor(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAPath()
{
    return(isLastSelectionAPath(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAMill()
{
    return(isLastSelectionAMill(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAForceSensor()
{
    return(isLastSelectionAForceSensor(&_selectedObjectIDs));
}

C3DObject* CObjCont::getLastSelection_object()
{
    return(getLastSelection(&_selectedObjectIDs));
}

CMirror* CObjCont::getLastSelection_mirror()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_mirror_type)
            return((CMirror*)it);
    }
    return(NULL);
}

COctree* CObjCont::getLastSelection_octree()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_octree_type)
            return((COctree*)it);
    }
    return(NULL);
}

CPointCloud* CObjCont::getLastSelection_pointCloud()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_pointcloud_type)
            return((CPointCloud*)it);
    }
    return(NULL);
}

CShape* CObjCont::getLastSelection_shape()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_shape_type)
            return((CShape*)it);
    }
    return(NULL);
}

CJoint* CObjCont::getLastSelection_joint()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_joint_type)
            return((CJoint*)it);
    }
    return(NULL);
}

CGraph* CObjCont::getLastSelection_graph()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_graph_type)
            return((CGraph*)it);
    }
    return(NULL);
}

CCamera* CObjCont::getLastSelection_camera()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_camera_type)
            return((CCamera*)it);
    }
    return(NULL);
}

CLight* CObjCont::getLastSelection_light()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_light_type)
            return((CLight*)it);
    }
    return(NULL);
}

CDummy* CObjCont::getLastSelection_dummy()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_dummy_type)
            return((CDummy*)it);
    }
    return(NULL);
}

CProxSensor* CObjCont::getLastSelection_proxSensor()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_proximitysensor_type)
            return((CProxSensor*)it);
    }
    return(NULL);
}

CVisionSensor* CObjCont::getLastSelection_visionSensor()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_visionsensor_type)
            return((CVisionSensor*)it);
    }
    return(NULL);
}

CPath* CObjCont::getLastSelection_path()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_path_type)
            return((CPath*)it);
    }
    return(NULL);
}

CMill* CObjCont::getLastSelection_mill()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_mill_type)
            return((CMill*)it);
    }
    return(NULL);
}

CForceSensor* CObjCont::getLastSelection_forceSensor()
{
    C3DObject* it=getLastSelection(&_selectedObjectIDs);
    if (it!=NULL)
    {
        if (it->getObjectType()==sim_object_forcesensor_type)
            return((CForceSensor*)it);
    }
    return(NULL);
}

int CObjCont::getLastSelectionID()
{
    return(getLastSelectionID(&_selectedObjectIDs));
}

int CObjCont::getSimpleShapeNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CShape* it=getShape(selection->at(i));
        if (it!=NULL)
        {
            if (!it->isCompound()) 
                counter++;
        }
    }
    return (counter);
}
int CObjCont::getShapeNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CShape* it=getShape(selection->at(i));
        if (it!=NULL)
            counter++;
    }
    return (counter);
}
int CObjCont::getMirrorNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CMirror* it=getMirror(selection->at(i));
        if (it!=NULL)
            counter++;
    }
    return (counter);
}
int CObjCont::getOctreeNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        COctree* it=getOctree(selection->at(i));
        if (it!=NULL)
            counter++;
    }
    return (counter);
}
int CObjCont::getPointCloudNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CPointCloud* it=getPointCloud(selection->at(i));
        if (it!=NULL)
            counter++;
    }
    return (counter);
}
int CObjCont::getJointNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CJoint* it=getJoint(selection->at(i));
        if (it!=NULL) 
            counter++;
    }
    return (counter);
}
int CObjCont::getGraphNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CGraph* it=getGraph(selection->at(i));
        if (it!=NULL) 
            counter++;
    }
    return (counter);
}
int CObjCont::getCompoundNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CShape* it=getShape(selection->at(i));
        if (it!=NULL)
        {
            if (it->isCompound())
                counter++;
        }
    }
    return (counter);
}
int CObjCont::getCameraNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CCamera* it=getCamera(selection->at(i));
        if (it!=NULL) 
            counter++;  
    }
    return (counter);
}
int CObjCont::getLightNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CLight* it=getLight(selection->at(i));
        if (it!=NULL) 
            counter++;  
    }
    return (counter);
}
int CObjCont::getDummyNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CDummy* it=getDummy(selection->at(i));
        if (it!=NULL) 
            counter++;
    }
    return (counter);
}
int CObjCont::getProxSensorNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CProxSensor* it=getProximitySensor(selection->at(i));
        if (it!=NULL) 
            counter++;
    }
    return (counter);
}
int CObjCont::getVisionSensorNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CVisionSensor* it=getVisionSensor(selection->at(i));
        if (it!=NULL) 
            counter++;
    }
    return (counter);
}
int CObjCont::getPathNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CPath* it=getPath(selection->at(i));
        if (it!=NULL) 
            counter++;
    }
    return (counter);
}
int CObjCont::getMillNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CMill* it=getMill(selection->at(i));
        if (it!=NULL) 
            counter++;
    }
    return (counter);
}
int CObjCont::getForceSensorNumberInSelection(std::vector<int>* selection)
{
    int counter=0;
    for (int i=0;i<int(selection->size());i++)
    {
        CForceSensor* it=getForceSensor(selection->at(i));
        if (it!=NULL) 
            counter++;
    }
    return (counter);
}
bool CObjCont::isLastSelectionAShape(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(false);
    CShape* it=getShape(selection->at(selection->size()-1));
    if (it!=NULL)
        return(true);
    return(false);
}
bool CObjCont::isLastSelectionASimpleShape(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(false);
    CShape* it=getShape(selection->at(selection->size()-1));
    if (it!=NULL)
    {
        if (!it->isCompound()) 
            return(true);
    }
    return(false);
}
bool CObjCont::isLastSelectionAJoint(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(false);
    CJoint* it=getJoint(selection->at(selection->size()-1));
    if (it!=NULL) 
        return(true);
    return(false);
}
bool CObjCont::isLastSelectionAMirror(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(false);
    CMirror* it=getMirror(selection->at(selection->size()-1));
    if (it!=NULL) 
        return(true);
    return(false);
}
bool CObjCont::isLastSelectionAnOctree(std::vector<int>* selection)
{
    if (selection->size()==0)
        return(false);
    COctree* it=getOctree(selection->at(selection->size()-1));
    if (it!=NULL)
        return(true);
    return(false);
}
bool CObjCont::isLastSelectionAPointCloud(std::vector<int>* selection)
{
    if (selection->size()==0)
        return(false);
    CPointCloud* it=getPointCloud(selection->at(selection->size()-1));
    if (it!=NULL)
        return(true);
    return(false);
}
bool CObjCont::isLastSelectionACompound(std::vector<int>* selection)
{
    if (selection->size()==0)
        return(false);
    CShape* it=getShape(selection->at(selection->size()-1));
    if (it!=NULL)
    {
        if (it->isCompound()) 
            return(true);
    }
    return(false);
}
bool CObjCont::isLastSelectionAGraph(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(false);
    CGraph* it=getGraph(selection->at(selection->size()-1));
    if (it!=NULL) 
        return(true);
    return(false);
}
bool CObjCont::isLastSelectionACamera(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(false);
    CCamera* it=getCamera(selection->at(selection->size()-1));
    if (it!=NULL)
        return(true);
    return(false);
}
bool CObjCont::isLastSelectionALight(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(false);
    CLight* it=getLight(selection->at(selection->size()-1));
    if (it!=NULL)
        return(true);
    return(false);
}
bool CObjCont::isLastSelectionADummy(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(false);
    CDummy* it=getDummy(selection->at(selection->size()-1));
    if (it!=NULL) 
        return(true);
    return(false);
}
bool CObjCont::isLastSelectionAProxSensor(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(false);
    CProxSensor* it=getProximitySensor(selection->at(selection->size()-1));
    if (it!=NULL) 
        return(true);
    return(false);
}
bool CObjCont::isLastSelectionAVisionSensor(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(false);
    CVisionSensor* it=getVisionSensor(selection->at(selection->size()-1));
    if (it!=NULL) 
        return(true);
    return(false);
}
bool CObjCont::isLastSelectionAPath(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(false);
    CPath* it=getPath(selection->at(selection->size()-1));
    if (it!=NULL) 
        return(true);
    return(false);
}
bool CObjCont::isLastSelectionAMill(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(false);
    CMill* it=getMill(selection->at(selection->size()-1));
    if (it!=NULL) 
        return(true);
    return(false);
}
bool CObjCont::isLastSelectionAForceSensor(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(false);
    CForceSensor* it=getForceSensor(selection->at(selection->size()-1));
    if (it!=NULL) 
        return(true);
    return(false);
}
C3DObject* CObjCont::getLastSelection(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(NULL);
    return(_objectIndex[selection->at(selection->size()-1)]);
}

int CObjCont::getLastSelectionID(std::vector<int>* selection)
{
    if (selection->size()==0) 
        return(-1);
    return((*selection)[(selection->size()-1)]);
}

bool CObjCont::doesObjectExist(C3DObject* obj)
{
    for (int i=0;i<int(objectList.size());i++)
    {
        C3DObject* obj2=getObject(objectList[i]);
        if (obj==obj2)
            return(true);
    }
    return(false);
}

bool CObjCont::isObjectInSelection(int objectID)
{
    return(isObjectInSelection(objectID,&_selectedObjectIDs));
}
bool CObjCont::isObjectInSelection(int objectID,std::vector<int>* selection)
{
    for (int i=0;i<int(selection->size());i++)
    {
        if (selection->at(i)==objectID)
            return(true);
    }
    return(false);
}

int CObjCont::getRenderingPosition(int objID)
{ // The rendering order is the reversed objectList!!
    for (int i=0;i<int(objectList.size());i++)
    {
        if (objectList[i]==objID)
            return(objectList.size()-i-1);
    }
    return(-1);
}

void CObjCont::changePoseAndKeepRestInPlace(int identifier,C7Vector& m,bool changePositionX,
        bool changePositionY,bool changePositionZ,bool changeOrientation)
{   // Changes the orientation/position of an object (identifier) with
    // keeping all its children in place. m is the new pose.
    C3DObject* it=CObjCont::getObject(identifier);
    if (it==NULL) 
        return;
    C7Vector oldRel(it->getLocalTransformationPart1());
    C7Vector oldRelActuatorPartIncl(it->getLocalTransformation());
    C7Vector oldAbs(it->getCumulativeTransformationPart1());
    C7Vector newAbs(oldAbs);
    if (changePositionX)
        newAbs.X(0)=m.X(0);
    if (changePositionY)
        newAbs.X(1)=m.X(1);
    if (changePositionZ)
        newAbs.X(2)=m.X(2);
    if (changeOrientation)
        newAbs.Q=m.Q;
    C7Vector parentInv(it->getParentCumulativeTransformation().getInverse());
    it->setLocalTransformation(parentInv*newAbs);
    // from here we have to compensate the children
    C7Vector newRelActuatorPartIncl(it->getLocalTransformation().getInverse());
    C7Vector coeff(newRelActuatorPartIncl*oldRelActuatorPartIncl);
    for (int i=0;i<int(it->childList.size());i++)
    {
        C7Vector oldChild(it->childList[i]->getLocalTransformationPart1());
        it->childList[i]->setLocalTransformation(coeff*oldChild);
    }
}

void CObjCont::removeFromSelectionAllExceptModelBase(bool keepObjectsSelectedThatAreNotBuiltOnAModelBase)
{ // One model base per hierarchy tree!
    FUNCTION_DEBUG;
    std::vector<int> sel(_selectedObjectIDs);
    deselectObjects();
    std::vector<C3DObject*> modelBases;
    std::vector<C3DObject*> nonModelBasesBuildOnNothing;
    for (int i=0;i<int(sel.size());i++)
    {
        C3DObject* it=getObject(sel[i]);
        if (it->getModelBase())
            modelBases.push_back(it);
        else
        {
            C3DObject* objIt=it;
            bool addIt=true;
            while (objIt->getParent()!=NULL)
            {
                objIt=objIt->getParent();
                if (objIt->getModelBase())
                {
                    addIt=false;
                    break;
                }
            }
            if (addIt)
                nonModelBasesBuildOnNothing.push_back(it);
        }
    }
    for (int i=0;i<int(modelBases.size());i++)
    {
        C3DObject* it=modelBases[i];
        bool isIndependentBase=true;
        while (it->getParent()!=NULL)
        {
            it=it->getParent();
            for (int j=0;j<int(modelBases.size());j++)
            {
                if (modelBases[j]==it)
                {
                    isIndependentBase=false;
                    break;
                }
            }
        }
        if (isIndependentBase)
            addObjectToSelection(modelBases[i]->getID());
    }
    if (keepObjectsSelectedThatAreNotBuiltOnAModelBase)
    {
        for (int i=0;i<int(nonModelBasesBuildOnNothing.size());i++)
            addObjectToSelection(nonModelBasesBuildOnNothing[i]->getID());
    }
}


void CObjCont::getMinAndMaxNameSuffixes(int& minSuffix,int& maxSuffix)
{
    minSuffix=-1;
    maxSuffix=-1;
    for (size_t i=0;i<objectList.size();i++)
    {
        int s=tt::getNameSuffixNumber(_objectIndex[objectList[i]]->getName().c_str(),true);
        if (i==0)
        {
            minSuffix=s;
            maxSuffix=s;
        }
        else
        {
            if (s<minSuffix)
                minSuffix=s;
            if (s>maxSuffix)
                maxSuffix=s;
        }
    }
}

bool CObjCont::canSuffix1BeSetToSuffix2(int suffix1,int suffix2)
{
    for (int i=0;i<int(objectList.size());i++)
    {
        int s1=tt::getNameSuffixNumber(_objectIndex[objectList[i]]->getName().c_str(),true);
        if (s1==suffix1)
        {
            std::string name1(tt::getNameWithoutSuffixNumber(_objectIndex[objectList[i]]->getName().c_str(),true));
            for (int j=0;j<int(objectList.size());j++)
            {
                int s2=tt::getNameSuffixNumber(_objectIndex[objectList[j]]->getName().c_str(),true);
                if (s2==suffix2)
                {
                    std::string name2(tt::getNameWithoutSuffixNumber(_objectIndex[objectList[j]]->getName().c_str(),true));
                    if (name1==name2)
                        return(false); // NO! We would have a name clash!
                }
            }
        }
    }
    return(true);
}

void CObjCont::setSuffix1ToSuffix2(int suffix1,int suffix2)
{
    for (int i=0;i<int(objectList.size());i++)
    {
        int s1=tt::getNameSuffixNumber(_objectIndex[objectList[i]]->getName().c_str(),true);
        if (s1==suffix1)
        {
            std::string name1(tt::getNameWithoutSuffixNumber(_objectIndex[objectList[i]]->getName().c_str(),true));
            _objectIndex[objectList[i]]->setName(tt::generateNewName_dash(name1,suffix2+1));
        }
    }
}

bool CObjCont::addObjectToScene(C3DObject* newObject,bool objectIsACopy)
{
    return(addObjectToSceneWithSuffixOffset(newObject,objectIsACopy,1));
}

bool CObjCont::addObjectToSceneWithSuffixOffset(C3DObject* newObject,bool objectIsACopy,int suffixOffset)
{
    App::ct->environment->setSceneCanBeDiscardedWhenNewSceneOpened(false); // 4/3/2012

    // Handle cases where two object names are the same, 
    // and replace all spaces and illegal chars:
    std::string newObjName=newObject->getName();
    if (objectIsACopy)
        newObjName=tt::generateNewName_dash(newObjName,suffixOffset);
    else
    {
        if (getObject(newObjName)!=NULL)
        {
            // Following faster with many objects:
            std::string baseName(tt::getNameWithoutSuffixNumber(newObjName.c_str(),false));
            int initialSuffix=tt::getNameSuffixNumber(newObjName.c_str(),false);
            std::vector<int> suffixes;
            std::vector<int> dummyValues;
            for (int i=0;i<int(objectList.size());i++)
            {
                std::string baseNameIt(tt::getNameWithoutSuffixNumber(_objectIndex[objectList[i]]->getName().c_str(),false));
                if (baseName.compare(baseNameIt)==0)
                {
                    suffixes.push_back(tt::getNameSuffixNumber(_objectIndex[objectList[i]]->getName().c_str(),false));
                    dummyValues.push_back(0);
                }
            }
            tt::orderAscending(suffixes,dummyValues);
            int lastS=-1;
            for (int i=0;i<int(suffixes.size());i++)
            {
                if ( (suffixes[i]>initialSuffix)&&(suffixes[i]>lastS+1) )
                    break;
                lastS=suffixes[i];
            }
            newObjName=tt::generateNewName_noDash(baseName,lastS+1+1);
        }
        // Following was too slow with many objects:
        //      while (getObject(newObjName)!=NULL)
        //          newObjName=tt::generateNewName_noDash(newObjName);
    }
    newObject->setName(newObjName);
    // Give the object a new identifier
    int i=_nextObjectHandle;
    while (_objectIndex[i]!=NULL)
    {
        i++;
        if (i>=(SIM_IDEND_3DOBJECT-SIM_IDSTART_3DOBJECT))
            i=SIM_IDSTART_3DOBJECT;
    }
    // i now points to a free spot. Actualize _nextObjectHandle now:
    _nextObjectHandle=i+1;
    if (_nextObjectHandle>=(SIM_IDEND_3DOBJECT-SIM_IDSTART_3DOBJECT))
        _nextObjectHandle=SIM_IDSTART_3DOBJECT;
    // set the new handle to the object:
    newObject->setID(i);
    objectList.push_back(i); // Added at the end of the list --> rendered first
    // Set the object in the index list:
    _objectIndex[i]=newObject;

    if (newObject->getObjectType()==sim_object_graph_type)
    { // If the simulation is running, we have to empty the buffer!!! (otherwise we might have old and new data mixed together (e.g. old data in future, new data in present!)
        if ((App::ct->simulation!=NULL)&&(!App::ct->simulation->isSimulationStopped()))
        {
            CGraph* graph=(CGraph*)newObject;
            graph->resetGraph();
        }
    }

    // Actualize the object information
    actualizeObjectInformation();

    App::ct->setModificationFlag(2); // object created
    return(true);
}

void CObjCont::_getAllObjectsSmallestAndBiggestSuffix(int& smallestSuffix,int& biggestSuffix)
{
    smallestSuffix=SIM_MAX_INT;
    biggestSuffix=-1;
    int minS,maxS;
    App::ct->constraintSolver->getMinAndMaxNameSuffixes(minS,maxS);
    if (minS<smallestSuffix)
        smallestSuffix=minS;
    if (maxS>biggestSuffix)
        biggestSuffix=maxS;
    App::ct->buttonBlockContainer->getMinAndMaxNameSuffixes(minS,maxS);
    if (minS<smallestSuffix)
        smallestSuffix=minS;
    if (maxS>biggestSuffix)
        biggestSuffix=maxS;
    App::ct->objCont->getMinAndMaxNameSuffixes(minS,maxS);
    if (minS<smallestSuffix)
        smallestSuffix=minS;
    if (maxS>biggestSuffix)
        biggestSuffix=maxS;
    App::ct->collisions->getMinAndMaxNameSuffixes(minS,maxS);
    if (minS<smallestSuffix)
        smallestSuffix=minS;
    if (maxS>biggestSuffix)
        biggestSuffix=maxS;
    App::ct->distances->getMinAndMaxNameSuffixes(minS,maxS);
    if (minS<smallestSuffix)
        smallestSuffix=minS;
    if (maxS>biggestSuffix)
        biggestSuffix=maxS;
    App::ct->collections->getMinAndMaxNameSuffixes(minS,maxS);
    if (minS<smallestSuffix)
        smallestSuffix=minS;
    if (maxS>biggestSuffix)
        biggestSuffix=maxS;
    App::ct->ikGroups->getMinAndMaxNameSuffixes(minS,maxS);
    if (minS<smallestSuffix)
        smallestSuffix=minS;
    if (maxS>biggestSuffix)
        biggestSuffix=maxS;
    App::ct->pathPlanning->getMinAndMaxNameSuffixes(minS,maxS);
    if (minS<smallestSuffix)
        smallestSuffix=minS;
    if (maxS>biggestSuffix)
        biggestSuffix=maxS;
    App::ct->motionPlanning->getMinAndMaxNameSuffixes(minS,maxS);
    if (minS<smallestSuffix)
        smallestSuffix=minS;
    if (maxS>biggestSuffix)
        biggestSuffix=maxS;
}

bool CObjCont::_canSuffix1BeSetToSuffix2(int suffix1,int suffix2)
{
    if (!App::ct->objCont->canSuffix1BeSetToSuffix2(suffix1,suffix2))
        return(false);
    if (!App::ct->constraintSolver->canSuffix1BeSetToSuffix2(suffix1,suffix2))
        return(false);
    if (!App::ct->buttonBlockContainer->canSuffix1BeSetToSuffix2(suffix1,suffix2))
        return(false);
    if (!App::ct->collisions->canSuffix1BeSetToSuffix2(suffix1,suffix2))
        return(false);
    if (!App::ct->distances->canSuffix1BeSetToSuffix2(suffix1,suffix2))
        return(false);
    if (!App::ct->collections->canSuffix1BeSetToSuffix2(suffix1,suffix2))
        return(false);
    if (!App::ct->ikGroups->canSuffix1BeSetToSuffix2(suffix1,suffix2))
        return(false);
    if (!App::ct->pathPlanning->canSuffix1BeSetToSuffix2(suffix1,suffix2))
        return(false);
    if (!App::ct->motionPlanning->canSuffix1BeSetToSuffix2(suffix1,suffix2))
        return(false);
    return(true);
}

void CObjCont::_setSuffix1ToSuffix2(int suffix1,int suffix2)
{
    App::ct->objCont->setSuffix1ToSuffix2(suffix1,suffix2);
    App::ct->constraintSolver->setSuffix1ToSuffix2(suffix1,suffix2);
    App::ct->buttonBlockContainer->setSuffix1ToSuffix2(suffix1,suffix2);
    App::ct->collisions->setSuffix1ToSuffix2(suffix1,suffix2);
    App::ct->distances->setSuffix1ToSuffix2(suffix1,suffix2);
    App::ct->collections->setSuffix1ToSuffix2(suffix1,suffix2);
    App::ct->ikGroups->setSuffix1ToSuffix2(suffix1,suffix2);
    App::ct->pathPlanning->setSuffix1ToSuffix2(suffix1,suffix2);
    App::ct->motionPlanning->setSuffix1ToSuffix2(suffix1,suffix2);
}

void CObjCont::cleanupDashNames(int suffix)
{ // This function will try to use the lowest dash naming possible (e.g. model#59 --> model and model#67 --> model#0 if possible)
    // if suffix is -1, then all suffixes are cleaned, otherwise only those equal or above 'suffix'

    // 1. we get all object's smallest and biggest suffix: 
    int smallestSuffix,biggestSuffix;
    _getAllObjectsSmallestAndBiggestSuffix(smallestSuffix,biggestSuffix);
    if (suffix<=0)
        suffix=0;
    for (int i=suffix;i<=biggestSuffix;i++)
    {
        for (int j=-1;j<i;j++)
        {
            if (_canSuffix1BeSetToSuffix2(i,j))
            {
                _setSuffix1ToSuffix2(i,j);
                break;
            }
        }
    }
}

void CObjCont::prepareFastLoadingMapping(std::vector<int>& map)
{
    std::vector<int> mapC(map);
    map.clear();
    int minVal=0;
    int maxVal=0;
    for (int i=0;i<int(mapC.size())/2;i++)
    {
        int v=mapC[2*i+0];
        if ( (v<minVal)||(i==0) )
            minVal=v;
        if ( (v>maxVal)||(i==0) )
            maxVal=v;
    }
    map.push_back(minVal);
    if (mapC.size()!=0)
    {
        for (int i=0;i<maxVal-minVal+1;i++)
            map.push_back(-1);
        for (int i=0;i<int(mapC.size())/2;i++)
            map[1+mapC[2*i+0]-minVal]=mapC[2*i+1];
    }
}

int CObjCont::getLoadingMapping(std::vector<int>* map,int oldVal)
{
    // using a fast mapping (lots of RAM)
    if ( (oldVal<0)||((oldVal-map->at(0))>int(map->size())-2) )
        return(-1);
    return(map->at(oldVal+1-map->at(0)));
}

void CObjCont::removeAllObjects()
{
#ifdef SIM_WITH_GUI
    if (App::mainWindow!=NULL)
        App::mainWindow->editModeContainer->processCommand(ANY_EDIT_MODE_FINISH_AND_CANCEL_CHANGES_EMCMD,NULL);
#endif
    deselectObjects();

    while (objectList.size()!=0)
    {
        C3DObject* it=getObject(objectList[0]);
        if (it!=NULL) 
            eraseObject(it);
    }
    // The above loop destroys all 3DObjects, and normally automatically
    // all resources linked to them. So we don't have to destroy the resources

    deselectObjects();
    objectList.clear();
    _objectIndex.clear();
    _objectIndex.resize(SIM_IDEND_3DOBJECT-SIM_IDSTART_3DOBJECT,NULL);
    _selectedObjectsBool.clear();
    _selectedObjectsBool.resize((SIM_IDEND_3DOBJECT-SIM_IDSTART_3DOBJECT)/8,0);

    octreeList.clear();
    pointCloudList.clear();
    mirrorList.clear();
    lightList.clear();
    cameraList.clear();
    graphList.clear();
    proximitySensorList.clear();
    visionSensorList.clear();
    pathList.clear();
    millList.clear();
    forceSensorList.clear();
    shapeList.clear();
    selectedObjectsWhenSaving.clear();
    jointList.clear();
    dummyList.clear();
    orphanList.clear();

    // Following new since 30/7/2014:
    // ideally we want to always use different object handles so that if the user erases an object and
    // creates a new one just after, the erased object's handle is not reused. That's why we have
    // the _nextObjectHandle variable.
    // The undo/redo functionality however doesn't like that, since at each undo/redo, all objects
    // get a new handle, and so the scene memory print looks different, even if the scenes are
    // identical: the undo/redo then marks a new restore point, which is not correct.
    // So, finally, when the whole scene gets emptied at least we make sure that all handles
    // start from the beginning:
    _nextObjectHandle=SIM_IDSTART_3DOBJECT;
}

bool CObjCont::makeObjectChildOf(C3DObject* childObject,C3DObject* parentObject)
{   // This will trigger an actualization (important! orphanList needs also an update and other things too)
    // Careful: this routine should be able to be used for objects in the object
    // container, but also for objects in the copy buffer!!!! So don't make
    // use of any 'getObject(id)' or similar function!!!!! <-- this is a very old comment. Is it still true??
    if (childObject==NULL) 
        return(false);
    // Check if the child has already his desired parent (so that we don't have to call the actualization (heavy and will also refresh all dialogs) (added on 2009/12/15)
    if (childObject->getParent()==parentObject)
        return(true);
    if (parentObject==NULL)
    {
        C7Vector oldAbsoluteTransf(childObject->getCumulativeTransformationPart1());
        childObject->setParent(NULL);
        childObject->setLocalTransformation(oldAbsoluteTransf);
        if (childObject->getObjectType()==sim_object_shape_type)
            ((CShape*)childObject)->setParentFollowsDynamic(false);
        actualizeObjectInformation();
        C3DObject::incrementModelPropertyValidityNumber();
        return(true);
    }
    // Illegal loop checking:
    if (parentObject->isObjectParentedWith(childObject)) 
        return(false);
    C7Vector oldAbsoluteTransf(childObject->getCumulativeTransformationPart1());
    C7Vector parentInverse(parentObject->getCumulativeTransformation().getInverse());
    childObject->setLocalTransformation(parentInverse*oldAbsoluteTransf);
    childObject->setParent(parentObject);

    if ( (childObject->getObjectType()==sim_object_shape_type)&&((CShape*)childObject)->getSetAutomaticallyToNonStaticIfGetsParent() )
    {
        ((CShape*)childObject)->setShapeIsDynamicallyStatic(false);
    }
    
    if ( (childObject->getObjectType()==sim_object_shape_type)&&((CShape*)childObject)->getParentFollowsDynamic() )
    { // Make sure we don't have more than one child that has the parentFollow tag :
        for (int i=0;i<int(parentObject->childList.size());i++)
        {
            if ( (parentObject->childList[i]->getObjectType()==sim_object_shape_type)&&(parentObject->childList[i]!=childObject) )
            {
                CShape* otherChild=(CShape*)parentObject->childList[i];
                if (otherChild->getParentFollowsDynamic())
                {
                    ((CShape*)childObject)->setParentFollowsDynamic(false);
                    break;
                }
            }
        }
    }

    actualizeObjectInformation();
    C3DObject::incrementModelPropertyValidityNumber();
    return(true);
}

void CObjCont::setAbsoluteConfiguration(int identifier,const C7Vector& v,bool keepChildrenInPlace)
{
    C3DObject* it=getObject(identifier);
    if (it==NULL)
        return;
    C7Vector childPreTr(it->getLocalTransformation());
    C7Vector parentInverse(it->getParentCumulativeTransformation().getInverse());
    it->setLocalTransformation(parentInverse*v);
    if (keepChildrenInPlace)
    {
        childPreTr=it->getLocalTransformation().getInverse()*childPreTr;
        for (int i=0;i<int(it->childList.size());i++)
            it->childList[i]->setLocalTransformation(childPreTr*it->childList[i]->getLocalTransformationPart1());
    }
}


void CObjCont::enableObjectActualization(bool e)
{
    _objectActualizationEnabled=e;
}

void CObjCont::actualizeObjectInformation()
{
    std::vector<int> initialSel;
    getSelectedObjects(initialSel);
    deselectObjects();

    if (_objectActualizationEnabled) 
    {
        App::setRebuildHierarchyFlag();

        // Actualize each object's child list
        // Following rewritten on 2009/03/15 to make it faster:
        for (size_t i=0;i<objectList.size();i++)
        {
            C3DObject* it=_objectIndex[objectList[i]];
            it->childList.clear();
        }
        for (size_t i=0;i<objectList.size();i++)
        {
            C3DObject* it=_objectIndex[objectList[i]];
            C3DObject* parent=it->getParent();
            if (parent!=NULL)
                parent->childList.push_back(it);
        }

        // Actualize the light, graph, sensor & joint list
        jointList.clear();
        dummyList.clear();
        orphanList.clear();
        octreeList.clear();
        pointCloudList.clear();
        mirrorList.clear();
        lightList.clear();
        cameraList.clear();
        graphList.clear();
        proximitySensorList.clear();
        visionSensorList.clear();
        pathList.clear();
        millList.clear();
        forceSensorList.clear();
        shapeList.clear();

        for (size_t i=0;i<objectList.size();i++)
        {
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_joint_type)
                jointList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_dummy_type)
                dummyList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getParent()==NULL)
                orphanList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_camera_type)
                cameraList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_light_type)
                lightList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_mirror_type)
                mirrorList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_octree_type)
                octreeList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_pointcloud_type)
                pointCloudList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_graph_type)
                graphList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_proximitysensor_type)
                proximitySensorList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_visionsensor_type)
                visionSensorList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_path_type)
                pathList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_shape_type)
                shapeList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_mill_type)
                millList.push_back(objectList[i]);
            if (_objectIndex[objectList[i]]->getObjectType()==sim_object_forcesensor_type)
                forceSensorList.push_back(objectList[i]);
        }
        // We actualize the direct linked joint list of each joint: (2009-01-27)
        for (size_t i=0;i<jointList.size();i++)
        {
            CJoint* it=getJoint(jointList[i]);
            it->directDependentJoints.clear();
            for (size_t j=0;j<jointList.size();j++)
            {
                CJoint* anAct=getJoint(App::ct->objCont->jointList[j]);
                if (anAct!=it)
                {
                    if ((anAct->getJointMode()==sim_jointmode_dependent)||(anAct->getJointMode()==sim_jointmode_reserved_previously_ikdependent))
                    {
                        if (anAct->getDependencyJointID()==it->getID())
                            it->directDependentJoints.push_back(anAct);
                    }
                }
            }
        }

        // We rebuild the group information
        App::ct->collections->actualizeAllGroups();

        // Keep this close to the end (added on 2010/02/09):
        for (size_t i=0;i<shapeList.size();i++)
            getShape(shapeList[i])->clearLastParentForLocalGlobalCollidable();
    }

    setSelectedObjects(initialSel);
    App::setFullDialogRefreshFlag();
}


int CObjCont::getObjectIdentifier(const std::string& objectName)
{
    for (size_t i=0;i<objectList.size();i++)
    {
        if (_objectIndex[objectList[i]]->getName().compare(objectName)==0)
            return(objectList[i]);
    }
    return(-1);
}

int CObjCont::getHighestObjectID()
{
    int highest=-1;
    for (int i=0;i<int(objectList.size());i++)
    {
        if (objectList[i]>highest)
            highest=objectList[i];
    }
    return(highest);
}

C3DObject* CObjCont::getObject(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    return(_objectIndex[identifier]);
}

CDummy* CObjCont::getDummy(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_dummy_type)
        return(NULL);
    return((CDummy*)_objectIndex[identifier]);
}

CJoint* CObjCont::getJoint(int identifier)
{
    if (identifier<0)
        return(NULL);
    if (identifier>=int(_objectIndex.size()))
        return(NULL);
    if (_objectIndex[identifier]==NULL)
        return(NULL);
    if (_objectIndex[identifier]->getObjectType()!=sim_object_joint_type)
        return(NULL);
    return((CJoint*)_objectIndex[identifier]);
}

C3DObject* CObjCont::getObject(const std::string& name)
{
    return(getObject(getObjectIdentifier(name)));
}

bool CObjCont::eraseObject(C3DObject* it)
{
    deselectObjects();

    if (it==NULL) 
        return(false);

    // We announce the object will be erased:
    announceObjectWillBeErased(it->getID()); // this may trigger other "interesting" things, such as customization script runs, etc.
    deselectObjects(); // to make sure, since above might have changed selection again

    // We remove the object from the object list
    size_t i;
    for (i=0;i<objectList.size();i++)
    {
        if (objectList[i]==it->getID()) 
            break;
    }
    objectList.erase(objectList.begin()+i);
    // Now remove the object from the index
    _objectIndex[it->getID()]=NULL;
    delete it;
    C3DObject::incrementModelPropertyValidityNumber();
    actualizeObjectInformation();
    App::ct->setModificationFlag(1); // object erased
    return(true);
}

//------------------ Object destruction announcement -------------------------
void CObjCont::announceObjectWillBeErased(int objectID)
{
    FUNCTION_DEBUG;

    // Moved following line up on 26/8/2016, because it can itself
    // trigger other destructions:
    App::ct->luaScriptContainer->announceObjectWillBeErased(objectID);

    for (int i=0;i<int(objectList.size());i++)
    {
        C3DObject* it=getObject(objectList[i]);
        if (it->getID()!=objectID)
        {
            if (it->announceObjectWillBeErased(objectID,false))
            { // We should never enter here since one obj destruction cannot trigger another obj destruction (anymore, no more versatiles!) 
#ifdef SIM_WITH_GUI
                App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSNOTR_APPLICATION_ERROR),strTranslate(IDSNOTR_STRANGE_ERROR5),VMESSAGEBOX_OKELI);
#endif
                eraseObject(it);
                i=-1; // ordering might have changed!
            }
        }
    }
    // First objects that won't trigger any more destructions:
    App::ct->pageContainer->announceObjectWillBeErased(objectID); // might trigger a view destruction!
    App::ct->constraintSolver->announceObjectWillBeErased(objectID);        // Triggers an announceGcsObjectWillBeErased for 3DObjects!
    App::ct->buttonBlockContainer->announceObjectWillBeErased(objectID);
//    App::ct->luaScriptContainer->announceObjectWillBeErased(objectID);
    App::ct->pathPlanning->announceObjectWillBeErased(objectID);
    App::ct->motionPlanning->announceObjectWillBeErased(objectID);
    App::ct->collisions->announceObjectWillBeErased(objectID);
    App::ct->distances->announceObjectWillBeErased(objectID);
    App::ct->drawingCont->announceObjectWillBeErased(objectID);
    App::ct->pointCloudCont->announceObjectWillBeErased(objectID);
    App::ct->ghostObjectCont->announceObjectWillBeErased(objectID);
    App::ct->bannerCont->announceObjectWillBeErased(objectID);
    App::ct->textureCont->announceGeneralObjectWillBeErased(objectID,-1);

    // Now objects that could trigger other destructions:
    App::ct->collections->announceObjectWillBeErased(objectID); // can trigger distance, collision and pathPlanning destructions
    App::ct->ikGroups->announceObjectWillBeErased(objectID); // can trigger pathPlanning object destruction
}

void CObjCont::announceIkGroupWillBeErased(int ikGroupID)
{
    for (int i=0;i<int(objectList.size());i++)
        getObject(objectList[i])->announceIkObjectWillBeErased(ikGroupID,false); // this never triggers 3DObject destruction!
    App::ct->ikGroups->announceIkGroupWillBeErased(ikGroupID); // This will never trigger an Ik group destruction
    App::ct->motionPlanning->announceIkGroupWillBeErased(ikGroupID); // This can trigger a motion planning object destruction!
}

void CObjCont::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{
}
