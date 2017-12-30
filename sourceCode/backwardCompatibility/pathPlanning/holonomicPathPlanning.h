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

#include "pathPlanning.h"
#include "holonomicPathNode.h"
#include "dummyClasses.h"
#include <vector>
#include "4Vector.h"
#include "7Vector.h"

class CHolonomicPathPlanning : public CPathPlanning  
{
public:
    CHolonomicPathPlanning(int theStartDummyID,int theGoalDummyID,
                            int theRobotCollectionID,int theObstacleCollectionID,int ikGroupID,
                            int thePlanningType,float theAngularCoeff,
                            float theStepSize,
                            const float theSearchMinVal[4],const float theSearchRange[4],
                            const int theDirectionConstraints[4],const float clearanceAndMaxDistance[2],const C3Vector& gammaAxis);
    virtual ~CHolonomicPathPlanning();

    // Following functions are inherited from CPathPlanning:
    int searchPath(int maxTimePerPass);
    bool setPartialPath();
    int smoothFoundPath(int steps,int maxTimePerPass);
    void getPathData(std::vector<float>& data);

    void getSearchTreeData(std::vector<float>& data,bool fromStart);

    void setAngularCoefficient(float coeff);
    void setStepSize(float size);

    std::vector<CHolonomicPathNode*> fromStart;
    std::vector<CHolonomicPathNode*> fromGoal;
    std::vector<CHolonomicPathNode*> foundPath;

private:
    bool doCollide(float* dist);

    CHolonomicPathNode* getClosestNode(std::vector<CHolonomicPathNode*>& nodes,CHolonomicPathNode* sample);
    CHolonomicPathNode* extend(std::vector<CHolonomicPathNode*>* nodeList,CHolonomicPathNode* toBeExtended,CHolonomicPathNode* extention,bool connect,CDummyDummy* dummy);
    int getVector(CHolonomicPathNode* fromPoint,CHolonomicPathNode* toPoint,float vect[7],float e,float& artificialLength,bool dontDivide);
    bool addVector(C3Vector& pos,C4Vector& orient,float vect[7]);
    bool areDirectionConstraintsRespected(float vect[7]);
    bool areSomeValuesForbidden(float values[7]);


    int startDummyID;
    int goalDummyID;
    int planningType;
    float angularCoeff;
    float stepSize;
    float _searchMinVal[4];
    float _searchRange[4];
    int _directionConstraints[4];
    bool _directionConstraintsOn;

    C4Vector _gammaAxisRotation;
    C4Vector _gammaAxisRotationInv;

    float _startConfInterferenceState;

    C7Vector _startDummyCTM;
    C7Vector _startDummyLTM;

    int numberOfRandomConnectionTries_forSteppedSmoothing;
    int numberOfRandomConnectionTriesLeft_forSteppedSmoothing;
    std::vector<int> foundPathSameStraightLineID_forSteppedSmoothing;
    int sameStraightLineNextID_forSteppedSmoothing;
    int nextIteration_forSteppedSmoothing;
};
