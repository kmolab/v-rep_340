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

#include "vrepMainHeader.h"
#include "collisionRoutine.h"
#include "distanceRoutine.h"
#include "pluginContainer.h"
#include "app.h"

//---------------------------- GENERAL COLLISION QUERIES ---------------------------

bool CCollisionRoutine::doEntitiesCollide(int entity1ID,int entity2ID,std::vector<float>* intersections,bool overrideCollidableFlagIfObject1,bool overrideCollidableFlagIfObject2,int collidingObjectIDs[2])
{   // if entity2ID==-1, then all collidable objects are tested against entity1
    // We first check if objects are valid:
    C3DObject* object1=App::ct->objCont->getObject(entity1ID);
    C3DObject* object2=App::ct->objCont->getObject(entity2ID);
    CRegCollection* collection1=NULL;
    CRegCollection* collection2=NULL;
    if (object1==NULL)
    {
        collection1=App::ct->collections->getCollection(entity1ID);
        if (collection1==NULL)
            return(false);
    }
    if (object2==NULL)
    {
        collection2=App::ct->collections->getCollection(entity2ID);
        if ( (collection2==NULL)&&(entity2ID!=-1) )
            return(false);
    }
    App::ct->calcInfo->collisionDetectionStart();
    bool collisionResult=false;
    if (object1!=NULL)
    { // Here we have an object against..
        if (object2!=NULL)
        { //...another object
            collisionResult=_doesObjectCollideWithObject(object1,object2,overrideCollidableFlagIfObject1,overrideCollidableFlagIfObject2,intersections);
            if (collisionResult&&(collidingObjectIDs!=NULL))
            {
                collidingObjectIDs[0]=object1->getID();
                collidingObjectIDs[1]=object2->getID();
            }
        }
        else
        { // an objects VS a collection or all other objects
            std::vector<C3DObject*> group;
            if (entity2ID==-1)
            { // Special group here (all objects except the shape):
                std::vector<C3DObject*> exception;
                exception.push_back(object1);
                App::ct->objCont->getAllCollidableObjectsFromSceneExcept(&exception,group);
            }
            else
            { // Regular group here:
                App::ct->collections->getCollidableObjectsFromCollection(entity2ID,group);
            }

            if (group.size()!=0)
            {
                int collidingGroupObject=-1;
                if (object1->getObjectType()==sim_object_shape_type)
                    collisionResult=_doesGroupCollideWithShape(group,(CShape*)object1,intersections,overrideCollidableFlagIfObject1,collidingGroupObject);
                if (object1->getObjectType()==sim_object_octree_type)
                    collisionResult=_doesGroupCollideWithOctree(group,(COctree*)object1,overrideCollidableFlagIfObject1,collidingGroupObject);
                if (object1->getObjectType()==sim_object_dummy_type)
                    collisionResult=_doesGroupCollideWithDummy(group,(CDummy*)object1,overrideCollidableFlagIfObject1,collidingGroupObject);
                if (object1->getObjectType()==sim_object_pointcloud_type)
                    collisionResult=_doesGroupCollideWithPointCloud(group,(CPointCloud*)object1,overrideCollidableFlagIfObject1,collidingGroupObject);

                if (collisionResult&&(collidingObjectIDs!=NULL))
                {
                    collidingObjectIDs[0]=object1->getID();
                    collidingObjectIDs[1]=collidingGroupObject;
                }
            }
        }
    }
    else
    { // Here we have a group against...
        std::vector<C3DObject*> group1;
        App::ct->collections->getCollidableObjectsFromCollection(entity1ID,group1);
        if (group1.size()!=0)
        {
            if (object2!=NULL)
            { // ...an object
                int collidingGroupObject=-1;
                if (object2->getObjectType()==sim_object_shape_type)
                    collisionResult=_doesGroupCollideWithShape(group1,(CShape*)object2,intersections,overrideCollidableFlagIfObject2,collidingGroupObject);
                if (object2->getObjectType()==sim_object_octree_type)
                    collisionResult=_doesGroupCollideWithOctree(group1,(COctree*)object2,overrideCollidableFlagIfObject2,collidingGroupObject);
                if (object2->getObjectType()==sim_object_dummy_type)
                    collisionResult=_doesGroupCollideWithDummy(group1,(CDummy*)object2,overrideCollidableFlagIfObject2,collidingGroupObject);
                if (object2->getObjectType()==sim_object_pointcloud_type)
                    collisionResult=_doesGroupCollideWithPointCloud(group1,(CPointCloud*)object2,overrideCollidableFlagIfObject2,collidingGroupObject);

                if (collisionResult&&(collidingObjectIDs!=NULL))
                {
                    collidingObjectIDs[0]=collidingGroupObject;
                    collidingObjectIDs[1]=object2->getID();
                }
            }
            else
            { // ...another group (or all other objects) (entity2ID could be -1)
                std::vector<C3DObject*> group2;
                if (entity2ID==-1)
                { // Special group here
                    App::ct->objCont->getAllCollidableObjectsFromSceneExcept(&group1,group2);
                }
                else
                { // Regular group here:
                    App::ct->collections->getCollidableObjectsFromCollection(entity2ID,group2);
                }
                if (group2.size()!=0)
                {
                    int collidingGroupObjects[2]={-1,-1};

                    if (entity1ID!=entity2ID)
                        collisionResult=_doesGroupCollideWithGroup(group1,group2,intersections,collidingGroupObjects);
                    else
                        collisionResult=_doesGroupCollideWithItself(group1,intersections,collidingGroupObjects);

                    if (collisionResult&&(collidingObjectIDs!=NULL))
                    {
                        collidingObjectIDs[0]=collidingGroupObjects[0];
                        collidingObjectIDs[1]=collidingGroupObjects[1];
                    }
                }
            }
        }
    }
    App::ct->calcInfo->collisionDetectionEnd(collisionResult);
    return(collisionResult);
}

//----------------------------------------------------------------------------------

bool CCollisionRoutine::_doesShapeCollideWithShape(CShape* shape1,CShape* shape2,std::vector<float>* intersections,bool overrideShape1CollidableFlag,bool overrideShape2CollidableFlag)
{   // if intersections is different from NULL we check for all collisions and
    // append intersection segments to the vector.
    // We never check a shape against itself!!
    if (shape1==shape2)
        return(false);
    if ( ( (shape1->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_collidable)==0 )&&(!overrideShape1CollidableFlag) )
        return(false);
    if ( ( (shape2->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_collidable)==0 )&&(!overrideShape2CollidableFlag) )
        return(false);

    // Before building collision nodes, check if the shape's bounding boxes collide (new since 9/7/2014):
    if ( (!shape1->isCollisionInformationInitialized())||(!shape2->isCollisionInformationInitialized()) )
    {
        if (!CPluginContainer::mesh_getBoxBoxCollision(shape1->getCumulativeTransformation().getMatrix(),shape1->geomData->getBoundingBoxHalfSizes(),shape2->getCumulativeTransformation().getMatrix(),shape2->geomData->getBoundingBoxHalfSizes()))
            return(false);
    }

    // Build the collision nodes only when needed. So do it right here!
    shape1->initializeCalculationStructureIfNeeded();
    shape2->initializeCalculationStructureIfNeeded();

    return(shape1->doesShapeCollideWithShape(shape2,intersections));
}

bool CCollisionRoutine::_doesGroupCollideWithShape(const std::vector<C3DObject*>& group, CShape* shape,std::vector<float>* intersections,bool overrideShapeCollidableFlag,int &collidingGroupObject)
{   // if intersections is different from NULL we check for ALL shape-shape collisions and
    // append intersection segments to the vector.
    bool returnValue=false;
    for (size_t i=0;i<group.size();i++)
    {
        if (group[i]->getObjectType()==sim_object_shape_type)
        {
            if (shape!=group[i])
            { // never self-collision
                if (_doesShapeCollideWithShape(shape,(CShape*)group[i],intersections,overrideShapeCollidableFlag,true))
                {
                    returnValue=true;
                    collidingGroupObject=group[i]->getID();
                    if (intersections==NULL)
                        return(true);
                }
            }
        }
        if (group[i]->getObjectType()==sim_object_octree_type)
        {
            if (!returnValue)
            {
                if (_doesOctreeCollideWithShape((COctree*)group[i],shape,true,overrideShapeCollidableFlag))
                {
                    returnValue=true;
                    collidingGroupObject=group[i]->getID();
                    if (intersections==NULL)
                        return(true);
                }
            }
        }
    }
    return(returnValue);
}

bool CCollisionRoutine::_doesGroupCollideWithGroup(const std::vector<C3DObject*>& group1,const std::vector<C3DObject*>& group2,std::vector<float>* intersections,int collidingGroupObjects[2])
{   // if intersections is different from NULL we check for all collisions and
    // append intersection segments to the vector.
    bool returnValue=false;
    std::vector<C3DObject*> checkedPairs;
    for (size_t i=0;i<group1.size();i++)
    {
        for (size_t j=0;j<group2.size();j++)
        {
            if (group1[i]!=group2[j])
            { // never check an object against itself
                // Never check twice the same pair:
                bool present=false;
                for (size_t k=0;k<checkedPairs.size()/2;k++)
                {
                    if ((group2[j]==checkedPairs[2*k+0])&&(group1[i]==checkedPairs[2*k+1]))
                    {
                        present=true;
                        break;
                    }
                }
                if (!present)
                {
                    checkedPairs.push_back(group1[i]);
                    checkedPairs.push_back(group2[j]);

                    bool doIt=(!returnValue);
                    if ( (!doIt)&&(intersections!=NULL) )
                    { // we still might have to do it if we have shape-shape colldetection (for the contour)
                        doIt=(group1[i]->getObjectType()==sim_object_shape_type)&&(group2[j]->getObjectType()==sim_object_shape_type);
                    }

                    if (doIt)
                    {
                        if (_doesObjectCollideWithObject(group1[i],group2[j],true,true,intersections))
                        {
                            collidingGroupObjects[0]=group1[i]->getID();
                            collidingGroupObjects[1]=group2[j]->getID();
                            if (intersections==NULL)
                                return(true);
                            returnValue=true;
                        }
                    }
                }
            }
        }
    }
    return(returnValue);
}

bool CCollisionRoutine::_areObjectBoundingBoxesOverlapping(C3DObject* obj1,C3DObject* obj2)
{
    C3DObject* objs[2]={obj1,obj2};
    C3Vector halfSizes[2];
    C4X4Matrix m[2];
    for (size_t cnt=0;cnt<2;cnt++)
    {
        C3DObject* obj=objs[cnt];
        if (obj->getObjectType()==sim_object_shape_type)
        {
            halfSizes[cnt]=((CShape*)obj)->geomData->getBoundingBoxHalfSizes();
            m[cnt]=obj->getCumulativeTransformation().getMatrix();
        }
        if (obj->getObjectType()==sim_object_dummy_type)
        {
            halfSizes[cnt]=C3Vector(0.0001f,0.0001f,0.0001f);
            m[cnt]=obj->getCumulativeTransformation().getMatrix();
        }
        if (obj->getObjectType()==sim_object_octree_type)
            ((COctree*)obj)->getMatrixAndHalfSizeOfBoundingBox(m[cnt],halfSizes[cnt]);
        if (obj->getObjectType()==sim_object_pointcloud_type)
            ((CPointCloud*)obj)->getMatrixAndHalfSizeOfBoundingBox(m[cnt],halfSizes[cnt]);
    }
    return (CPluginContainer::mesh_getBoxBoxCollision(m[0],halfSizes[0],m[1],halfSizes[1]));
}

bool CCollisionRoutine::_doesOctreeCollideWithShape(COctree* octree,CShape* shape,bool overrideOctreeCollidableFlag,bool overrideShapeCollidableFlag)
{
    if (octree->getOctreeInfo()==NULL)
        return(false); // Octree is empty
    if ( ( (octree->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_collidable)==0 )&&(!overrideOctreeCollidableFlag) )
        return(false);
    if ( ( (shape->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_collidable)==0 )&&(!overrideShapeCollidableFlag) )
        return(false);

    // Before building collision nodes, check if the shape's bounding boxes collide (new since 9/7/2014):
    if (!shape->isCollisionInformationInitialized())
    {
        if (!_areObjectBoundingBoxesOverlapping(octree,shape))
            return(false);
    }

    // Build the collision nodes only when needed. So do it right here!
    shape->initializeCalculationStructureIfNeeded();

    return(CPluginContainer::mesh_checkOctreeCollisionWithShape(octree->getOctreeInfo(),octree->getCumulativeTransformation().getMatrix(),shape->geomData->collInfo,shape->getCumulativeTransformation().getMatrix()));
}

bool CCollisionRoutine::_doesOctreeCollideWithOctree(COctree* octree1,COctree* octree2,bool overrideOctree1CollidableFlag,bool overrideOctree2CollidableFlag)
{
    if (octree1==octree2)
        return(false); // never self-collision
    if (octree1->getOctreeInfo()==NULL)
        return(false); // Octree is empty
    if (octree2->getOctreeInfo()==NULL)
        return(false); // Octree is empty
    if ( ( (octree1->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_collidable)==0 )&&(!overrideOctree1CollidableFlag) )
        return(false);
    if ( ( (octree2->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_collidable)==0 )&&(!overrideOctree2CollidableFlag) )
        return(false);
    if (!_areObjectBoundingBoxesOverlapping(octree1,octree2))
        return(false);
    return(CPluginContainer::mesh_checkOctreeCollisionWithOctree(octree1->getOctreeInfo(),octree1->getCumulativeTransformation().getMatrix(),octree2->getOctreeInfo(),octree2->getCumulativeTransformation().getMatrix()));
}

bool CCollisionRoutine::_doesOctreeCollideWithPointCloud(COctree* octree,CPointCloud* pointCloud,bool overrideOctreeCollidableFlag,bool overridePointCloudCollidableFlag)
{
    if (octree->getOctreeInfo()==NULL)
        return(false); // Octree is empty
    if (pointCloud->getPointCloudInfo()==NULL)
        return(false); // PointCloud is empty
    if ( ( (octree->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_collidable)==0 )&&(!overrideOctreeCollidableFlag) )
        return(false);
    if ( ( (pointCloud->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_collidable)==0 )&&(!overridePointCloudCollidableFlag) )
        return(false);

    if (!_areObjectBoundingBoxesOverlapping(octree,pointCloud))
        return(false);

    return(CPluginContainer::mesh_checkOctreeCollisionWithPointCloud(octree->getOctreeInfo(),octree->getCumulativeTransformation().getMatrix(),pointCloud->getPointCloudInfo(),pointCloud->getCumulativeTransformation().getMatrix()));
}

bool CCollisionRoutine::_doesOctreeCollideWithDummy(COctree* octree,CDummy* dummy,bool overrideOctreeCollidableFlag,bool overrideDummyCollidableFlag)
{
    if (octree->getOctreeInfo()==NULL)
        return(false); // Octree is empty
    if ( ( (octree->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_collidable)==0 )&&(!overrideOctreeCollidableFlag) )
        return(false);
    if ( ( (dummy->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_collidable)==0 )&&(!overrideDummyCollidableFlag) )
        return(false);

    return(CPluginContainer::mesh_checkOctreeCollisionWithSinglePoint(octree->getOctreeInfo(),octree->getCumulativeTransformation().getMatrix(),dummy->getCumulativeTransformation().X,NULL,NULL));
}

bool CCollisionRoutine::_doesObjectCollideWithObject(C3DObject* object1,C3DObject* object2,bool overrideObject1CollidableFlag,bool overrideObject2CollidableFlag,std::vector<float>* intersections)
{
    if (object1->getObjectType()==sim_object_shape_type)
    {
        if (object2->getObjectType()==sim_object_shape_type)
            return(_doesShapeCollideWithShape((CShape*)object1,(CShape*)object2,intersections,overrideObject1CollidableFlag,overrideObject2CollidableFlag));
        if (object2->getObjectType()==sim_object_octree_type)
            return(_doesOctreeCollideWithShape((COctree*)object2,(CShape*)object1,overrideObject2CollidableFlag,overrideObject1CollidableFlag));
    }
    if (object1->getObjectType()==sim_object_octree_type)
    {
        if (object2->getObjectType()==sim_object_shape_type)
            return(_doesOctreeCollideWithShape((COctree*)object1,(CShape*)object2,overrideObject1CollidableFlag,overrideObject2CollidableFlag));
        if (object2->getObjectType()==sim_object_octree_type)
            return(_doesOctreeCollideWithOctree((COctree*)object1,(COctree*)object2,overrideObject1CollidableFlag,overrideObject2CollidableFlag));
        if (object2->getObjectType()==sim_object_dummy_type)
            return(_doesOctreeCollideWithDummy((COctree*)object1,(CDummy*)object2,overrideObject1CollidableFlag,overrideObject2CollidableFlag));
        if (object2->getObjectType()==sim_object_pointcloud_type)
            return(_doesOctreeCollideWithPointCloud((COctree*)object1,(CPointCloud*)object2,overrideObject1CollidableFlag,overrideObject2CollidableFlag));
    }
    if (object1->getObjectType()==sim_object_dummy_type)
    {
        if (object2->getObjectType()==sim_object_octree_type)
            return(_doesOctreeCollideWithDummy((COctree*)object2,(CDummy*)object1,overrideObject2CollidableFlag,overrideObject1CollidableFlag));
    }
    if (object1->getObjectType()==sim_object_pointcloud_type)
    {
        if (object2->getObjectType()==sim_object_octree_type)
            return(_doesOctreeCollideWithPointCloud((COctree*)object2,(CPointCloud*)object1,overrideObject2CollidableFlag,overrideObject1CollidableFlag));
    }
    return(false);
}


bool CCollisionRoutine::_doesGroupCollideWithOctree(const std::vector<C3DObject*>& group,COctree* octree,bool overrideOctreeCollidableFlag,int& collidingGroupObject)
{
    for (size_t i=0;i<group.size();i++)
    {
        if (group[i]->getObjectType()==sim_object_shape_type)
        {
            if (_doesOctreeCollideWithShape(octree,(CShape*)group[i],overrideOctreeCollidableFlag,true))
            {
                collidingGroupObject=group[i]->getID();
                return(true);
            }
        }
        if (group[i]->getObjectType()==sim_object_dummy_type)
        {
            if (_doesOctreeCollideWithDummy(octree,(CDummy*)group[i],overrideOctreeCollidableFlag,true))
            {
                collidingGroupObject=group[i]->getID();
                return(true);
            }
        }
        if (group[i]->getObjectType()==sim_object_octree_type)
        {
            if (octree!=group[i])
            { // never self-collision
                if (_doesOctreeCollideWithOctree(octree,(COctree*)group[i],overrideOctreeCollidableFlag,true))
                {
                    collidingGroupObject=group[i]->getID();
                    return(true);
                }
            }
        }
        if (group[i]->getObjectType()==sim_object_pointcloud_type)
        {
            if (_doesOctreeCollideWithPointCloud(octree,(CPointCloud*)group[i],overrideOctreeCollidableFlag,true))
            {
                collidingGroupObject=group[i]->getID();
                return(true);
            }
        }
    }
    return(false);
}

bool CCollisionRoutine::_doesGroupCollideWithDummy(const std::vector<C3DObject*>& group,CDummy* dummy,bool overrideDummyCollidableFlag,int& collidingGroupObject)
{
    for (size_t i=0;i<group.size();i++)
    {
        if (group[i]->getObjectType()==sim_object_octree_type)
        {
            if (_doesOctreeCollideWithDummy((COctree*)group[i],dummy,overrideDummyCollidableFlag,true))
            {
                collidingGroupObject=group[i]->getID();
                return(true);
            }
        }
    }
    return(false);
}

bool CCollisionRoutine::_doesGroupCollideWithPointCloud(const std::vector<C3DObject*>& group,CPointCloud* pointCloud,bool overridePointCloudCollidableFlag,int& collidingGroupObject)
{
    for (size_t i=0;i<group.size();i++)
    {
        if (group[i]->getObjectType()==sim_object_octree_type)
        {
            if (_doesOctreeCollideWithPointCloud((COctree*)group[i],pointCloud,overridePointCloudCollidableFlag,true))
            {
                collidingGroupObject=group[i]->getID();
                return(true);
            }
        }
    }
    return(false);
}

bool CCollisionRoutine::_doesGroupCollideWithItself(const std::vector<C3DObject*>& group,std::vector<float>* intersections,int collidingGroupObjects[2])
{   // if intersections is different from NULL we check for all collisions and
    // append intersection segments to the vector.

    std::vector<C3DObject*> objPairs; // Object pairs we need to check
    for (size_t i=0;i<group.size();i++)
    {
        C3DObject* obj1=group[i];
        int csci1=obj1->getCollectionSelfCollisionIndicator();
        for (size_t j=0;j<group.size();j++)
        {
            C3DObject* obj2=group[j];
            int csci2=obj2->getCollectionSelfCollisionIndicator();
            if (obj1!=obj2)
            { // We never check an object against itself!
                if (abs(csci1-csci2)!=1)
                { // the collection self collision indicators differences is not 1
                    // We now check if these partners are already present in objPairs
                    bool absent=true;
                    for (size_t k=0;k<objPairs.size()/2;k++)
                    {
                        if ( ( (objPairs[2*k+0]==obj1)&&(objPairs[2*k+1]==obj2) ) || ( (objPairs[2*k+0]==obj2)&&(objPairs[2*k+1]==obj1) ) )
                        {
                            absent=false;
                            break;
                        }
                    }
                    if (absent)
                    {
                        objPairs.push_back(obj1);
                        objPairs.push_back(obj2);
                    }
                }
            }
        }
    }

    // Here we check all objects from the two groups against each other
    bool returnValue=false;
    for (size_t i=0;i<objPairs.size()/2;i++)
    {
        C3DObject* obj1=objPairs[2*i+0];
        C3DObject* obj2=objPairs[2*i+1];
        if (obj1!=NULL) // to take into account a removed pair
        {
            bool doIt=(!returnValue);
            if ( (!doIt)&&(intersections!=NULL) )
            { // we still might have to do it if we have shape-shape colldetection (for the contour)
                doIt=(obj1->getObjectType()==sim_object_shape_type)&&(obj2->getObjectType()==sim_object_shape_type);
            }
            if (doIt)
            {
                if (_doesObjectCollideWithObject(obj1,obj2,true,true,intersections))
                {
                    collidingGroupObjects[0]=obj1->getID();
                    collidingGroupObjects[1]=obj2->getID();
                    if (intersections==NULL)
                        return(true);
                    returnValue=true;
                }
            }
        }
    }
    return(returnValue);
}
