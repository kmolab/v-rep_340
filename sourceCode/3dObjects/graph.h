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
#include "graphData.h"
#include "graphDataComb.h"
#include "staticGraphCurve.h"
#include "sView.h"
#include "VPoint.h"

class CGraph : public C3DObject  
{
public:

    CGraph();
    virtual ~CGraph();

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
    std::string getObjectTypeInfo() const;
    std::string getObjectTypeInfoExtended() const;
    bool isPotentiallyCollidable() const;
    bool isPotentiallyMeasurable() const;
    bool isPotentiallyDetectable() const;
    bool isPotentiallyRenderable() const;
    bool isPotentiallyCuttable() const;

    // Various
    bool getGraphCurve(int graphType,int index,std::string& label,std::vector<float>& xVals,std::vector<float>& yVals,std::vector<float>& zVals,int& curveType,float col[3],float minMax[6]) const;
    void curveToClipboard(int graphType,const std::string& curveName) const;
    void curveToStatic(int graphType,const std::string& curveName);
    void removeStaticCurve(int graphType,const std::string& curveName);

    void setSize(float theNewSize);
    float getSize() const;
    int addNewGraphData(CGraphData* graphData);
    void removeGraphData(int id);
    int getDataStreamCount() const;
    int get2DCurveCount() const;
    int get3DCurveCount() const;
    CGraphData* getGraphData(int id) const;
    CGraphData* getGraphData(std::string theName) const;
    CGraphDataComb* getGraphData2D(int id) const;
    CGraphDataComb* getGraphData2D(std::string theName) const;
    CGraphDataComb* getGraphData3D(int id) const;
    CGraphDataComb* getGraphData3D(std::string theName) const;

    void add2DPartners(CGraphDataComb* it);
    void add3DPartners(CGraphDataComb* it);
    void remove2DPartners(int id);
    void remove3DPartners(int id);
    bool set3DDataName(int identifier,std::string newName);
    bool set2DDataName(int identifier,std::string newName);
    void setBufferSize(int buffSize);
    int getBufferSize() const;
    void setCyclic(bool isCyclic);
    bool getCyclic() const;
    void resetGraph();
    void addNextPoint(float time);
    bool getAbsIndexOfPosition(int pos,int& absIndex) const;
    int getNumberOfPoints() const;
    void setJustDrawCurves(bool justCurves);
    bool getJustDrawCurves() const;
    bool getData(const CGraphData* it,int pos,float& outputValue,bool cyclic,float range,bool doUnitConversion) const;

    void exportGraphData(VArchive &ar);

    void setExplicitHandling(bool explicitHandl);
    bool getExplicitHandling() const;

    int getTrackingValueIndex() const;
    void makeCurveStatic(int curveIndex,int dimensionIndex);
    void removeAllStatics();
    void copyCurveToClipboard(int curveIndex,int dimensionIndex);


    void announceGraphDataObjectWillBeDestroyed(int graphDataID);

    CVisualParam* getColor();

    // Variables which need to be serialized & copied
    std::vector <CGraphData*> daten;
    std::vector <CGraphDataComb*> threeDPartners;
    std::vector <CGraphDataComb*> twoDPartners;
    std::vector <CStaticGraphCurve*> _staticCurves;
    bool xYZPlanesDisplay;
    bool graphGrid;
    bool graphValues;
    float backgroundColor[3];
    float textColor[3];

protected:
    CStaticGraphCurve* getStaticCurveFromName(int type,const std::string& name);

    // Variables which need to be serialized & copied
    CVisualParam color;
    float size;
    int bufferSize;
    bool cyclic;
    bool _explicitHandling;
    int numberOfPoints;
    int startingPoint;
    std::vector <float> times;

    bool justDrawCurves; // no need to serialize. used to display just the 3D curves on top of everything
    int trackingValueIndex;
    bool trackingValueIsStatic;
    float trackingValue[2];
    float squareDistFromTrackingValue;
    // Various
    static VPoint currentWinSize;

    bool _initialValuesInitialized;
    bool _initialExplicitHandling;


#ifdef SIM_WITH_GUI
public:
    void lookAt(int windowSize[2],CSView* subView,bool timeGraph,bool drawText,bool passiveSubView,bool oneOneProportionForXYGraph);
    void validateViewValues(int windowSize[2],float graphPosition[2],float graphSize[2],
                bool timeGraph,bool shiftOnly=false,bool keepProp=false,bool autoModeForTimeGraphXaxis=true);

private:
    void drawGrid(int windowSize[2],float graphPosition[2],float graphSize[2]);
    void drawOverlay(int windowSize[2],float graphPosition[2],float graphSize[2],
                    int mouseMode,CSView* subView,bool passiveSubView);
    void drawValues(int windowSize[2],float graphPosition[2],float graphSize[2],
                    int mousePosition[2],bool mouseIsDown,bool dontRender,bool autoMode,bool timeGraphYaxisAutoMode,
                    bool drawText,bool passiveSubView,bool timeGraph,CSView* subView);
#endif
};
