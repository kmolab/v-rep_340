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
#include "ser.h"
#include "dummyClasses.h"
#include "jointObject.h"
#include "ikGroup.h"

class CMotionPlanningTask
{
public:
    CMotionPlanningTask(const std::vector<int>& jointHandles);
    CMotionPlanningTask();
    virtual ~CMotionPlanningTask();

    void renderYour3DStuff();
    void initializeInitialValues(bool simulationIsRunning);
    void simulationAboutToStart();
    void simulationEnded();

    CMotionPlanningTask* copyYourself();
    void setDefaultValues();
    void serialize(CSer& ar);
    void performObjectLoadingMapping(std::vector<int>* map);
    void performGroupLoadingMapping(std::vector<int>* map);
    void performIkGroupLoadingMapping(std::vector<int>* map);
    bool announceObjectWillBeErased(int objID,bool copyBuffer);
    bool announceCollectionWillBeErased(int groupID,bool copyBuffer);
    bool announceIkGroupWillBeErased(int ikGroupID,bool copyBuffer);
    int getObjectID();
    void setObjectID(int newID);
    std::string getObjectName();
    void setObjectName(std::string newName);
    void getJointHandles(std::vector<int>& handles);
    int getJointCount();
    int getIkGroup();
    void setIkGroup(int ikGroupId);
    int getJointStepCount(int jointHandle);
    void setJointStepCount(int jointHandle,int stepCount);
    void setRobotEntity(int entityID);
    int getRobotEntity();
    void setObstacleEntity(int entityID);
    int getObstacleEntity();
    void setRobotSelfCollEntity1(int entityID);
    int getRobotSelfCollEntity1();
    void setRobotSelfCollEntity2(int entityID);
    int getRobotSelfCollEntity2();
    void setDistanceThreshold(float d);
    float getDistanceThreshold();
    void setSelfCollDistanceThreshold(float d);
    float getSelfCollDistanceThreshold();
    float getRobotMetric(int jointHandle);
    void setRobotMetric(int jointHandle,float w);
    float getTipMetric(int index);
    void setTipMetric(int index,float w);
    void getCurrentJointPositions(std::vector<float>& jointPositions);
    void setPhase1NodeVisualization(bool show);
    bool getPhase1NodeVisualization();
    bool getPhase1NodeAreReady();


    void clearPhase2NodeVisualizationAndPathVisualization();

    C3DObject* getBaseObject();
    C3DObject* getTipObject();
    C3DObject* getTargetObject();
    CikGroup* getIkGroupObject();

    int calculateResultingNodeCount();
    void clearDataStructure();
    bool calculateDataStructureIfNeeded(int* collidingNodeCnt,int* totalNodeCnt,bool outputActivityToConsole);
    int getRobotConfigFromTipPose(const C7Vector& tipPose,int options,float* robotJoints,int trialCount,float tipCloseDistance,const float* configConstraints,const float* referenceConfigs,int refConfigCount,const int* jointBehaviour,int correctionPasses,int maxTimeInMs);

    float* findPath(const float* startConfig,const float* goalConfig,int options,float stepSize,int* outputConfigsCnt,int maxTimeInMs,const int* auxIntParams,const float* auxFloatParams);
    float* findIkPath(const float* startConfig,const C7Vector& goalPose,int options,float stepSize,int* outputConfigsCnt,const int* auxIntParams,const float* auxFloatParams);
    float* simplifyPath(const float* pathBuffer,int configCnt,int options,float stepSize,int increment,int* outputConfigsCnt,int maxTimeInMs,const int* auxIntParams,const float* auxFloatParams);
    float* getConfigTransition(const float* startConfig,const float* goalConfig,int options,const int* select,float calcStepSize,float maxOutStepSize,int wayPointCnt,const float* wayPoints,int* outputConfigsCnt,const int* auxIntParams,const float* auxFloatParams);

    static int randomSeed;

    void _saveAllSceneJointPositionsAndModes(std::vector<CJoint*>& joints,std::vector<float>& jointPositions,std::vector<int>& jointModes,std::vector<float>* jointIkResolutionWeights=NULL);
    void _restoreAllSceneJointPositionsAndModes(const std::vector<CJoint*>& joints,const std::vector<float>& jointPositions,const std::vector<int>& jointModes,const std::vector<float>* jointIkResolutionWeights=NULL);

    bool _showPhase1Nodes;
    int _objectID;
    std::string _objectName;
    std::vector<int> _jointHandles;
    std::vector<int> _jointStepCount;
    int _ikGroupId;
    int _robotEntity;
    int _obstacleEntity;
    float _distanceThreshold;
    int _robotSelfCollEntity1;
    int _robotSelfCollEntity2;
    float _selfCollDistanceThreshold;

    float _tipMetric[4]; // the weight each Cart. dim. has for min. dist. calc. x,y,z,angle
    std::vector<float> _robotMetric; // the weight each Dof of the robot config. has for min. dist. calc. j1,j2,j3,..,jn

    CDummyMpObject* _data;

    char* _nodesSerializationData;
    int _nodesSerializationDataSize;

    // following 3 not serialized!
    bool _showPhase2FromStartConnections;
    bool _showPhase2FromGoalConnections;
    bool _showPhase2Path;
};
