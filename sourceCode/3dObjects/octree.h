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

class CDummy;
class CPointCloud;

class COctree : public C3DObject
{
public:

    COctree();
    virtual ~COctree();

    // Following functions are inherited from 3DObject
    void display(CViewableBase* renderingObject,int displayAttrib);
    C3DObject* copyYourself();
    void scaleObject(float scalingFactor);
    void scaleObjectNonIsometrically(float x,float y,float z);
    void serialize(CSer& ar);
    void serializeWExtIk(CExtIkSer& ar);
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

    // Various functions
    void setCellSize(float theNewSize);
    float getCellSize() const;
    void setCellSizeForDisplay(float theNewSizeForDisplay);
    float getCellSizeForDisplay() const;

    void insertPoints(const float* pts,int ptsCnt,bool ptsAreRelativeToOctree,const unsigned char* optionalColors3,bool colorsAreIndividual,const unsigned int* optionalTags,unsigned int theTagWhenOptionalTagsIsNull);
    void insertShape(const CShape* shape,unsigned int theTag);
    void insertOctree(const COctree* octree,unsigned int theTag);
    void insertDummy(const CDummy* dummy,unsigned int theTag);
    void insertPointCloud(const CPointCloud* pointCloud,unsigned int theTag);
    void insertOctree(const void* octree2Info,const C4X4Matrix& octree2CTM,unsigned int theTag);
    void insertObjects(const std::vector<int>& sel);
    void insertObject(const C3DObject* obj,unsigned int theTag);

    void subtractPoints(const float* pts,int ptsCnt,bool ptsAreRelativeToOctree);
    void subtractShape(const CShape* shape);
    void subtractOctree(const COctree* octree);
    void subtractDummy(const CDummy* dummy);
    void subtractPointCloud(const CPointCloud* pointCloud);
    void subtractOctree(const void* octree2Info,const C4X4Matrix& octree2CTM);
    void subtractObjects(const std::vector<int>& sel);
    void subtractObject(const C3DObject* obj);

    void clear();
    bool getShowOctree() const;
    void setShowOctree(bool show);
    bool getUseRandomColors() const;
    void setUseRandomColors(bool r);
    bool getColorIsEmissive() const;
    void setColorIsEmissive(bool e);
    bool getUsePointsInsteadOfCubes() const;
    void setUsePointsInsteadOfCubes(bool r);
    bool getSaveCalculationStructure() const;
    void setSaveCalculationStructure(bool s);
    int getPointSize() const;
    void setPointSize(int s);
    const std::vector<float>* getCubePositions() const;
    std::vector<float>* getCubePositions();
    const void* getOctreeInfo() const;
    void* getOctreeInfo();
    void getMatrixAndHalfSizeOfBoundingBox(C4X4Matrix& m,C3Vector& hs) const;
    CVisualParam* getColor();

    void setVertexBufferId(int id);
    int getVertexBufferId() const;
    void setNormalBufferId(int id);
    int getNormalBufferId() const;
    void getMaxMinDims(C3Vector& ma,C3Vector& mi) const;
    float* getCubeVertices();
    float* getColors();

protected:
    void _readPositionsAndColorsAndSetDimensions();

    // Variables which need to be serialized & copied
    CVisualParam color;
    float _cellSize;
    int _pointSize;
    void* _octreeInfo;
    C3Vector _minDim;
    C3Vector _maxDim;
    std::vector<float> _voxelPositions;
    std::vector<float> _colors;
    bool _showOctreeStructure;
    bool _useRandomColors;
    bool _usePointsInsteadOfCubes;
    bool _saveCalculationStructure;
    bool _colorIsEmissive;

    // following only for display:
    float _cubeVertices[24*3];
    float _cellSizeForDisplay;
    int _vertexBufferId;
    int _normalBufferId;
};
