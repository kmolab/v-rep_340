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

#include "3DObject.h"
#include "pathCont.h"
#include "geomProxy.h"

class CPath : public C3DObject  
{
public:

    CPath();
    virtual ~CPath();

    // Following functions are inherited from 3DObject
    void display(CViewableBase* renderingObject,int displayAttrib);
    C3DObject* copyYourself();
    void scaleObject(float scalingFactor);
    void scaleObjectNonIsometrically(float x,float y,float z);
    void serialize(CSer& ar);
    void serializeWExtIk(CExtIkSer& ar);
    bool announceObjectWillBeErased(int objID,bool copyBuffer);
    void announceCollectionWillBeErased(int groupID,bool copyBuffer);
    void announceCollisionWillBeErased(int collisionID,bool copyBuffer);
    void announceDistanceWillBeErased(int distanceID,bool copyBuffer);
    void announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer);
    void announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer);
    void performObjectLoadingMapping(std::vector<int>* map);
    void performGroupLoadingMapping(std::vector<int>* map);
    void performCollisionLoadingMapping(std::vector<int>* map);
    void performDistanceLoadingMapping(std::vector<int>* map);
    void performIkLoadingMapping(std::vector<int>* map);
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
    bool isPotentiallyCollidable() const;
    bool isPotentiallyMeasurable() const;
    bool isPotentiallyDetectable() const;
    bool isPotentiallyRenderable() const;
    bool isPotentiallyCuttable() const;
    std::string getObjectTypeInfo() const;
    std::string getObjectTypeInfoExtended() const;

    void setExplicitHandling(bool explicitHandl);
    bool getExplicitHandling();
    void resetPath();
    void handlePath(float deltaTime);

    void setShapingEnabled(bool isEnabled);
    bool getShapingEnabled();

    void setShapingFollowFullOrientation(bool full);
    bool getShapingFollowFullOrientation();
    void setShapingSectionClosed(bool closed);
    bool getShapingSectionClosed();
    void setShapingElementMaxLength(float l);
    float getShapingElementMaxLength();
    void setShapingType(int theType);
    int getShapingType();
    void setShapingThroughConvexHull(bool c);
    bool getShapingThroughConvexHull();
    bool getShape(CGeomProxy* geomObj[1],CShape* shapeObj[1]);

    void setShapingScaling(float s);
    float getShapingScaling() const;
    CVisualParam* getShapingColor();

    // Variables which need to be copied and serialized:
    CPathCont* pathContainer;
    std::vector<float> shapingCoordinates;

    // Other variables:
    std::vector<float> _pathShapeVertices;
    std::vector<int> _pathShapeIndices;
    std::vector<float> _pathShapeNormals;

protected:
    void _generatePathShape();

    // Variables which need to be copied and serialized:
    CVisualParam shapingColor;
    bool _explicitHandling;
    bool _shapingEnabled;

    bool _shapingFollowFullOrientation;
    bool _shapingSectionClosed;
    bool _shapingConvexHull;
    float _shapingElementMaxLength;
    int _shapingType;
    float _shapingScaling;

    // Other variables:
    unsigned short _pathModifID;

    bool _initialValuesInitialized;
    bool _initialExplicitHandling;

#ifdef SIM_WITH_GUI
public:
    bool transformSelectedPathPoints(const C4X4Matrix& cameraAbsConf,const C3Vector& clicked3DPoint,float prevPos[2],float pos[2],float screenHalfSizes[2],float halfSizes[2],bool perspective,int eventID);
#endif
};
