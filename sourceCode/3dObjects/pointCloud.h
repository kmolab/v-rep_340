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
class COctree;

class CPointCloud : public C3DObject
{
public:

    CPointCloud();
    virtual ~CPointCloud();

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
    void setMaxPointCountPerCell(int cnt);
    int getMaxPointCountPerCell() const;
    void insertPoints(const float* pts,int ptsCnt,bool ptsAreRelativeToPointCloud,const unsigned char* optionalColors3,bool colorsAreIndividual);
    void insertShape(const CShape* shape);
    void insertOctree(const COctree* octree);
    void insertDummy(const CDummy* dummy);
    void insertPointCloud(const CPointCloud* pointCloud);
    void insertObjects(const std::vector<int>& sel);
    void insertObject(const C3DObject* obj);

    int removePoints(const float* pts,int ptsCnt,bool ptsAreRelativeToPointCloud,float distanceTolerance);
    void subtractOctree(const COctree* octree);
    void subtractDummy(const CDummy* dummy,float distanceTolerance);
    void subtractPointCloud(const CPointCloud* pointCloud,float distanceTolerance);
    void subtractOctree(const void* octree2Info,const C4X4Matrix& octree2CTM);
    void subtractObjects(const std::vector<int>& sel);
    void subtractObject(const C3DObject* obj,float distanceTolerance);

    int intersectPoints(const float* pts,int ptsCnt,bool ptsAreRelativeToPointCloud,float distanceTolerance);

    void clear();
    bool getShowOctree() const;
    void setShowOctree(bool show);
    float getAveragePointCountInCell();
    int getPointSize() const;
    void setPointSize(int s);
    float getBuildResolution() const;
    void setBuildResolution(float r);
    float getRemovalDistanceTolerance() const;
    void setRemovalDistanceTolerance(float t);
    float getInsertionDistanceTolerance() const;
    void setInsertionDistanceTolerance(float t);
    bool getUseRandomColors() const;
    void setUseRandomColors(bool r);
    bool getColorIsEmissive() const;
    void setColorIsEmissive(bool e);
    bool getSaveCalculationStructure() const;
    void setSaveCalculationStructure(bool s);
    bool getDoNotUseCalculationStructure() const;
    void setDoNotUseCalculationStructure(bool s);
    float getPointDisplayRatio() const;
    void setPointDisplayRatio(float r);
    const std::vector<float>* getPoints() const;
    std::vector<float>* getPoints();
    const void* getPointCloudInfo() const;
    void* getPointCloudInfo();
    void getMatrixAndHalfSizeOfBoundingBox(C4X4Matrix& m,C3Vector& hs) const;

    CVisualParam* getColor();
    void getMaxMinDims(C3Vector& ma,C3Vector& mi) const;
    std::vector<float>* getColors();
    std::vector<float>* getDisplayPoints();
    std::vector<float>* getDisplayColors();

protected:
    void _readPositionsAndColorsAndSetDimensions();
    void _getCharRGB3Colors(const std::vector<float>& floatRGBA,std::vector<unsigned char>& charRGB);

    // Variables which need to be serialized & copied
    CVisualParam color;
    float _cellSize;
    int _maxPointCountPerCell;
    void* _pointCloudInfo;
    C3Vector _minDim;
    C3Vector _maxDim;
    std::vector<float> _points;
    std::vector<float> _colors;
    std::vector<float> _displayPoints;
    std::vector<float> _displayColors;
    bool _showOctreeStructure;
    bool _useRandomColors;
    bool _saveCalculationStructure;
    int _pointSize;
    int _nonEmptyCells;
    float _buildResolution;
    float _removalDistanceTolerance;
    float _insertionDistanceTolerance;
    float _pointDisplayRatio;
    bool _doNotUseOctreeStructure;
    bool _colorIsEmissive;
};
