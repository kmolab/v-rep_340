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
#include "3DObject.h"
#ifdef SIM_WITH_GUI
#include "vMenubar.h"
#endif

//FULLY STATIC CLASS
class CSceneObjectOperations
{
public:
    static void keyPress(int key);
    static bool processCommand(int commandID);

    static void copyObjects(std::vector<int>* selection,bool displayMessages);
    static void pasteCopyBuffer(bool displayMessages);
    static void cutObjects(std::vector<int>* selection,bool displayMessages);
    static void deleteObjects(std::vector<int>* selection,bool displayMessages);
    static void scaleObjects(const std::vector<int>& selection,float scalingFactor,bool scalePositionsToo);

    static void addRootObjectChildrenToSelection(std::vector<int>& selection);
    static int groupSelection(std::vector<int>* selection,bool showMessages);
    static void ungroupSelection(std::vector<int>* selection,bool showMessages);
    static bool mergeSelection(std::vector<int>* selection,bool showMessages);
    static void divideSelection(std::vector<int>* selection,bool showMessages);
    static void mergePathSelection(std::vector<int>* selection);
    static int generateConvexDecomposed(int shapeHandle,size_t nClusters,double maxConcavity,bool addExtraDistPoints,
                                        bool addFacesPoints,double maxConnectDist,size_t maxTrianglesInDecimatedMesh,
                                        size_t maxHullVertices,double smallClusterThreshold,
                                        bool individuallyConsiderMultishapeComponents,int maxIterations,
                                        bool useHACD,int resolution_VHACD,int depth_VHACD,float concavity_VHACD,
                                        int planeDownsampling_VHACD,int convexHullDownsampling_VHACD,
                                        float alpha_VHACD,float beta_VHACD,float gamma_VHACD,bool pca_VHACD,
                                        bool voxelBased_VHACD,int maxVerticesPerCH_VHACD,float minVolumePerCH_VHACD);
    static int generateConvexHull(int shapeHandle);
    static int convexDecompose_apiVersion(int shapeHandle,int options,const int* intParams,const float* floatParams);

#ifdef SIM_WITH_GUI
    static void addMenu(VMenu* menu);
#endif
};
