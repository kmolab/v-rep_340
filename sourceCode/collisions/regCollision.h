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
#include "visualParam.h"

class CRegCollision
{
public:

    CRegCollision(int obj1ID,int obj2ID,std::string objName,int objID);
    virtual ~CRegCollision();

    void displayCollisionContour();
    void initializeInitialValues(bool simulationIsRunning);
    void simulationAboutToStart();
    void simulationEnded();

    bool isSame(int obj1ID,int obj2ID) const;
    int getObjectID() const;
    std::string getObjectName() const;
    std::string getObjectPartnersName() const;
    void setObjectName(std::string newName);
    void setObjectID(int newID);
    int getObject1ID() const;
    int getObject2ID() const;
    void clearCollisionResult();
    bool getCollisionResult() const;
    bool isCollisionResultValid() const;
    float getCalculationTime() const;
    bool handleCollision();
    int readCollision(int collObjHandles[2]) const;
    void serialize(CSer& ar);
    void performObjectLoadingMapping(std::vector<int>* map);
    void performGroupLoadingMapping(std::vector<int>* map);
    bool announceObjectWillBeErased(int objID,bool copyBuffer);
    bool announceCollectionWillBeErased(int groupID,bool copyBuffer);
    CRegCollision* copyYourself();
    void setColliderChangesColor(bool changes);
    bool getColliderChangesColor() const;
    void setCollideeChangesColor(bool changes);
    bool getCollideeChangesColor() const;
    void setExhaustiveDetection(bool exhaustive);
    bool getExhaustiveDetection() const;
    void setExplicitHandling(bool explicitHandl);
    bool getExplicitHandling() const;
    int getCollisionColor(int entityID) const;
    bool canComputeCollisionContour() const;

    void setContourWidth(int w);
    int getContourWidth() const;

    std::vector<float>* getIntersectionsPtr();

    CVisualParam contourColor;

protected:  
    // Variables which need to be serialized and copied:
    int object1ID;
    int object2ID;
    std::string objectName;
    int objectID;
    int _countourWidth;
    // Various:
    bool collisionResult;
    bool _collisionResultValid;
    int _collObjectHandles[2];

    int _calcTimeInMs;

    bool colliderChangesColor;
    bool collideeChangesColor;
    bool detectAllCollisions;
    bool explicitHandling;
    std::vector<float> intersections;

    bool _initialValuesInitialized;
    bool _initialExplicitHandling;
};
