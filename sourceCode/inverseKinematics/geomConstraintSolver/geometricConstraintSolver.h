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

#include "iKObjCont.h"
#include "iKChain.h"
#include "iKGraphObjCont.h"
#include "iKChainCont.h"

struct SGeomConstrSolverParam
{
    int maxIterations;
    float interpolation;
    float generalDamping;
    float maxAngularVariation;
    float maxLinearVariation;
    float loopClosurePositionTolerance;
    float loopClosureOrientationTolerance;
};

//FULLY STATIC CLASS
class CGeometricConstraintSolver  
{
public:
    static bool solve(CIKGraphObjCont& graphContainer,SGeomConstrSolverParam& parameters);

private:
    static bool solveHierarchy(CIKObjCont* objects,SGeomConstrSolverParam& parameters);
    static bool performMainIterationLoop(std::vector<CIKDummy*>& tipContainer,int& maxIterations,float interpolation,SGeomConstrSolverParam& parameters);
    static bool tryIncrementalLoopSolve(std::vector<CIKDummy*>& tipContainer,SGeomConstrSolverParam& parameters);
    static int performOnePass(CIKChainCont& chainCont,bool& limitOrAvoidanceNeedMoreCalculation,float interpolFact,float& nextInterpol,SGeomConstrSolverParam& parameters);
    
    static void orderLoopsFromSmallestToBiggest(std::vector<CIKDummy*>& tipContainer);
    static void getSmallestLoops(std::vector<CIKDummy*>& tipContainer,std::vector<CIKDummy*>& returnContainer,int numberOfLoops);
    static bool areLoopsClosedWithinTolerance(std::vector<CIKDummy*>& tipContainer,SGeomConstrSolverParam& parameters);
};
