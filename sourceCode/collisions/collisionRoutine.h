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

#include "shape.h"
#include "dummy.h"
#include "octree.h"
#include "pointCloud.h"
#include <vector>

//FULLY STATIC CLASS
class CCollisionRoutine  
{
public:
    CCollisionRoutine();
    virtual ~CCollisionRoutine();

    static bool doEntitiesCollide(int entity1ID,int entity2ID,std::vector<float>* intersections,bool overrideCollidableFlagIfObject1,bool overrideCollidableFlagIfObject2,int collidingObjectIDs[2]);

private:
    static bool _doesObjectCollideWithObject(C3DObject* object1,C3DObject* object2,bool overrideObject1CollidableFlag,bool overrideObject2CollidableFlag,std::vector<float>* intersections);
    static bool _doesShapeCollideWithShape(CShape* shape1,CShape* shape2,std::vector<float>* intersections,bool overrideShape1CollidableFlag,bool overrideShape2CollidableFlag);
    static bool _doesOctreeCollideWithShape(COctree* octree,CShape* shape,bool overrideOctreeCollidableFlag,bool overrideShapeCollidableFlag);
    static bool _doesOctreeCollideWithOctree(COctree* octree1,COctree* octree2,bool overrideOctree1CollidableFlag,bool overrideOctree2CollidableFlag);
    static bool _doesOctreeCollideWithPointCloud(COctree* octree,CPointCloud* pointCloud,bool overrideOctreeCollidableFlag,bool overridePointCloudCollidableFlag);
    static bool _doesOctreeCollideWithDummy(COctree* octree,CDummy* dummy,bool overrideOctreeCollidableFlag,bool overrideDummyCollidableFlag);

    static bool _doesGroupCollideWithShape(const std::vector<C3DObject*>& group,CShape* shape,std::vector<float>* intersections,bool overrideShapeCollidableFlag,int& collidingGroupObject);
    static bool _doesGroupCollideWithOctree(const std::vector<C3DObject*>& group,COctree* octree,bool overrideOctreeCollidableFlag,int& collidingGroupObject);
    static bool _doesGroupCollideWithDummy(const std::vector<C3DObject*>& group,CDummy* dummy,bool overrideDummyCollidableFlag,int& collidingGroupObject);
    static bool _doesGroupCollideWithPointCloud(const std::vector<C3DObject*>& group,CPointCloud* pointClout,bool overridePointCloudCollidableFlag,int& collidingGroupObject);

    static bool _doesGroupCollideWithItself(const std::vector<C3DObject*>& group,std::vector<float>* intersections,int collidingGroupObjects[2]);
    static bool _doesGroupCollideWithGroup(const std::vector<C3DObject*>& group1,const std::vector<C3DObject*>& group2,std::vector<float>* intersections,int collidingGroupObjects[2]);

    static bool _areObjectBoundingBoxesOverlapping(C3DObject* obj1,C3DObject* obj2);
};
