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
#include "3DObject.h"
#include "3Vector.h"
#include "7Vector.h"

class CDummy : public C3DObject  
{
public:

    CDummy();
    virtual ~CDummy();

    // Following functions are inherited from 3DObject
    void display(CViewableBase* renderingObject,int displayAttrib);
    C3DObject* copyYourself();
    void scaleObject(float scalingFactor);
    void scaleObjectNonIsometrically(float x,float y,float z);
    void serialize(CSer& ar);
    void serializeWExtIk(CExtIkSer& ar);
    static void serializeWExtIkStatic(CExtIkSer& ar);
    void announceCollectionWillBeErased(int groupID,bool copyBuffer);
    void announceCollisionWillBeErased(int collisionID,bool copyBuffer);
    void announceDistanceWillBeErased(int distanceID,bool copyBuffer);
    void announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer);
    void performIkLoadingMapping(std::vector<int>* map);
    void performGroupLoadingMapping(std::vector<int>* map);
    void performCollisionLoadingMapping(std::vector<int>* map);
    void performDistanceLoadingMapping(std::vector<int>* map);
    void performGcsLoadingMapping(std::vector<int>* map);
    void performTextureObjectLoadingMapping(std::vector<int>* map);
    void performDynMaterialObjectLoadingMapping(std::vector<int>* map);
    void bufferMainDisplayStateVariables();
    void bufferedMainDisplayStateVariablesToDisplay();
    void simulationAboutToStart();
    void simulationEnded();
    void initializeInitialValues(bool simulationIsRunning);
    bool getFullBoundingBox(C3Vector& minV,C3Vector& maxV) const;
    bool getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV) const;
    bool getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices) const;
    std::string getObjectTypeInfo() const;
    std::string getObjectTypeInfoExtended() const;
    bool isPotentiallyCollidable() const;
    bool isPotentiallyMeasurable() const;
    bool isPotentiallyDetectable() const;
    bool isPotentiallyRenderable() const;
    bool isPotentiallyCuttable() const;
    bool announceObjectWillBeErased(int objID,bool copyBuffer);
    void announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer);
    void performObjectLoadingMapping(std::vector<int>* map);

    // SPECIAL FUNCTION TO GUARANTEE FORWARD COMPATIBILITY WHEN LOADING OBJECT TYPES THAT DON'T EXIST YET!
    void loadUnknownObjectType(CSer& ar);

    // Various functions
    void setSize(float theNewSize);
    float getSize() const;

    void setAssignedToParentPath(bool assigned);
    bool getAssignedToParentPath() const;
    void setAssignedToParentPathOrientation(bool assigned);
    bool getAssignedToParentPathOrientation() const;
    void setFreeOnPathTrajectory(bool isFree);
    bool getFreeOnPathTrajectory() const;
    void setVirtualDistanceOffsetOnPath(float off);
    float getVirtualDistanceOffsetOnPath() const;

    void setVirtualDistanceOffsetOnPath_variationWhenCopy(float off);
    float getVirtualDistanceOffsetOnPath_variationWhenCopy() const;

    void setLinkedDummyID(int id,bool setDirectly);
    void setLinkType(int lt,bool setDirectly);
    int getLinkedDummyID() const;
    int getLinkType() const;
    C7Vector getTempLocalTransformation() const;
    void setTempLocalTransformation(const C7Vector& tr);

    CVisualParam* getColor();

protected:
    CVisualParam color;
    float size;
    int _linkedDummyID;
    int _linkType;
    bool _assignedToParentPath;
    bool _assignedToParentPathOrientation;
    bool _freeOnPathTrajectory;
    float _virtualDistanceOffsetOnPath;
    float _virtualDistanceOffsetOnPath_variationWhenCopy;
    C7Vector _localTransformation_temp; // used for IK (needed when the dummy is freely sliding on a path object!)
};
