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

#include "shape.h"
#include "proximitySensor.h"
#include "octree.h"
#include "pointCloud.h"

typedef bool (*OCCLUSION_CHECK_CALLBACK)(const float pt[3]);

struct SOcclusionCheckDat
{
    bool occlusionCheck;
    float sensorClose;
    bool frontFace;
    bool backFace;
    std::vector<C3DObject*> objects;
    std::vector<C4X4Matrix> objectRelToSensorM;
};

//FULLY STATIC CLASS
class CProxSensorRoutine  
{
public:
    // The main general routine:
    static bool detectEntity(int sensorID,int entityID,bool closestFeatureMode,bool angleLimitation,float maxAngle,C3Vector& detectedPt,float& dist,bool frontFace,bool backFace,int& detectedObject,float minThreshold,C3Vector& triNormal,bool overrideDetectableFlagIfNonCollection,bool checkOcclusions);

    static bool detectPrimitive(int sensorID,float* vertexPointer,int itemType,int itemCount,bool closestFeatureMode,bool angleLimitation,float maxAngle,C3Vector& detectedPt,float& dist,bool frontFace,bool backFace,float minThreshold,C3Vector& triNormal);


private:
    static SOcclusionCheckDat _occlusionCheckDat;

    static bool _checkForOcclusion(const float _detectedPt[3]);

    static int _detectDummy(CProxSensor* sensor,CDummy* dummy,C3Vector& detectedPt,float& dist,C3Vector& triNormalNotNormalized,bool closestFeatureMode,bool angleLimitation,float maxAngle,bool frontFace,bool backFace,float minThreshold,OCCLUSION_CHECK_CALLBACK occlusionCheckCallback);
    static int _detectShape(CProxSensor* sensor,CShape* shape,C3Vector& detectedPt,float& dist,C3Vector& triNormalNotNormalized,bool closestFeatureMode,bool angleLimitation,float maxAngle,bool frontFace,bool backFace,float minThreshold,OCCLUSION_CHECK_CALLBACK occlusionCheckCallback);
    static int _detectOctree(CProxSensor* sensor,COctree* octree,C3Vector& detectedPt,float& dist,C3Vector& triNormalNotNormalized,bool closestFeatureMode,bool angleLimitation,float maxAngle,bool frontFace,bool backFace,float minThreshold,OCCLUSION_CHECK_CALLBACK occlusionCheckCallback);
    static int _detectPointCloud(CProxSensor* sensor,CPointCloud* pointCloud,C3Vector& detectedPt,float& dist,C3Vector& triNormalNotNormalized,bool closestFeatureMode,bool angleLimitation,float maxAngle,bool frontFace,bool backFace,float minThreshold,OCCLUSION_CHECK_CALLBACK occlusionCheckCallback);

    static int _detectObject(CProxSensor* sensor,C3DObject* object,C3Vector& detectedPt,float& dist,C3Vector& triNormalNotNormalized,bool closestFeatureMode,bool angleLimitation,float maxAngle,bool frontFace,bool backFace,float minThreshold,OCCLUSION_CHECK_CALLBACK occlusionCheckCallback);

    static OCCLUSION_CHECK_CALLBACK _prepareOcclusionCheck(const CProxSensor* sensor,const std::vector<C3DObject*>& objectsToDetect,bool frontFace,bool backFace,bool checkOcclusions);
    static void _cleanUpOcclusionCheck();
    static void _orderGroupAccordingToApproxDistanceToSensingPoint(const CProxSensor* sensor,std::vector<C3DObject*>& group);
    static float _getApproxPointObjectBoundingBoxDistance(const C3Vector& point,C3DObject* obj);
    static bool _doesSensorVolumeOverlapWithObjectBoundingBox(CProxSensor* sensor,C3DObject* obj);
};
