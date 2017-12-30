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

class CRegDist  
{
public:
    CRegDist(int obj1ID,int obj2ID,std::string objName,int objID);
    virtual ~CRegDist();

    void displayDistanceSegment();
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
    void clearDistanceResult();
    bool getDistanceResult(float dist[7]) const;
    float getCalculationTime() const;
    float handleDistance();
    float readDistance() const;

    void serialize(CSer& ar);
    void performObjectLoadingMapping(std::vector<int>* map);
    void performGroupLoadingMapping(std::vector<int>* map);
    bool announceObjectWillBeErased(int objID,bool copyBuffer);
    bool announceCollectionWillBeErased(int groupID,bool copyBuffer);
    void setTreshhold(float tr);
    float getTreshhold() const;
    void setTreshholdActive(bool active);
    bool getTreshholdActive() const;
    CRegDist* copyYourself();
    void setDisplaySegment(bool display);
    bool getDisplaySegment() const;
    void setExplicitHandling(bool explicitHandl);
    bool getExplicitHandling() const;

    void setSegmentWidth(int w);
    int getSegmentWidth() const;

    CVisualParam segmentColor;

protected:  
    // Variables which need to be serialized and copied:
    int object1ID;
    int object2ID;
    std::string objectName;
    int objectID;
    int _segmentWidth;
    float treshhold;
    bool treshholdActive;

    // Various:
    float distanceResult[7];
    float _distance;
    int distanceBuffer[4];
    bool distanceIsValid;
    int _calcTimeInMs;
    bool displaySegment;
    bool explicitHandling;

    bool _initialValuesInitialized;
    bool _initialExplicitHandling;
};
