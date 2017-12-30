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
#include "v_rep_internal.h"
#include "regDist.h"
#include "3DObject.h"
#include "global.h"
#include "distanceRoutine.h"
#include "tt.h"
#include "app.h"
#include "gV.h"
#include "v_repStrings.h"
#include "vDateTime.h"
#include "pluginContainer.h"
#include "distanceRendering.h"

CRegDist::CRegDist(int obj1ID,int obj2ID,std::string objName,int objID)
{
    object1ID=obj1ID;
    object2ID=obj2ID;
    objectName=objName;
    objectID=objID;
    displaySegment=true;
    explicitHandling=false;
    for (int i=0;i<7;i++)
        distanceResult[i]=0.0f;
    distanceBuffer[0]=-1;
    distanceBuffer[1]=-1;
    distanceBuffer[2]=-1;
    distanceBuffer[3]=-1;
    treshhold=0.5f;
    treshholdActive=false;
    distanceIsValid=false;
    _segmentWidth=2;
    segmentColor.setColorsAllBlack();
    _initialValuesInitialized=false;

    _calcTimeInMs=0;
}

void CRegDist::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
    _initialValuesInitialized=simulationIsRunning;
    if (simulationIsRunning)
    {
        _initialExplicitHandling=explicitHandling;
    }
}

void CRegDist::simulationAboutToStart()
{
    initializeInitialValues(true);
}

void CRegDist::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
    if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
    {
        explicitHandling=_initialExplicitHandling;
    }
    _initialValuesInitialized=false;
}

CRegDist::~CRegDist()
{
}

bool CRegDist::isSame(int obj1ID,int obj2ID) const
{
    if ( (obj1ID==object1ID)&&(obj2ID==object2ID) )
        return(true);
    if ( (obj2ID==object1ID)&&(obj1ID==object2ID) )
        return(true);
    return(false);
}

int CRegDist::getObjectID() const
{
    return(objectID);
}

std::string CRegDist::getObjectName() const
{
    return(objectName);
}

std::string CRegDist::getObjectPartnersName() const
{
    std::string theName=getObjectName();
    theName=theName.append(" (");
    if (object1ID<SIM_IDSTART_COLLECTION)
    {
        C3DObject* it=App::ct->objCont->getObject(object1ID);
        int t=it->getObjectType();
        if (t==sim_object_shape_type)
            theName+=IDSN_SHAPE;
        if (t==sim_object_octree_type)
            theName+=IDSN_OCTREE;
        if (t==sim_object_pointcloud_type)
            theName+=IDSN_POINTCLOUD;
        if (t==sim_object_dummy_type)
            theName+=IDSN_DUMMY;
        theName+=":";
        theName+=it->getName();
    }
    else
    {
        CRegCollection* it=App::ct->collections->getCollection(object1ID);
        if (it!=NULL)
        {
            theName+=strTranslate(IDSN_COLLECTION);
            theName+=":";
            theName+=it->getCollectionName();
        }
    }
    theName=theName.append(" - ");
    if (object2ID>=SIM_IDSTART_COLLECTION)
    {
        CRegCollection* it=App::ct->collections->getCollection(object2ID);
        if (it!=NULL)
        {
            theName+=strTranslate(IDSN_COLLECTION);
            theName+=":";
            theName+=it->getCollectionName();
        }
    }
    else
    {
        if (object2ID!=-1)
        {
            C3DObject* it=App::ct->objCont->getObject(object2ID);
            int t=it->getObjectType();
            if (t==sim_object_shape_type)
                theName+=IDSN_SHAPE;
            if (t==sim_object_octree_type)
                theName+=IDSN_OCTREE;
            if (t==sim_object_pointcloud_type)
                theName+=IDSN_POINTCLOUD;
            if (t==sim_object_dummy_type)
                theName+=IDSN_DUMMY;
            theName+=":";
            theName+=it->getName();
        }
        else
            theName+=strTranslate(IDS_ALL_OTHER_ENTITIES);
    }
    theName=theName.append(")");
    return(theName);
}

void CRegDist::setObjectName(std::string newName)
{
    objectName=newName;
}

void CRegDist::setObjectID(int newID)
{
    objectID=newID;
}

int CRegDist::getObject1ID() const
{
    return(object1ID);
}

int CRegDist::getObject2ID() const
{
    return(object2ID);
}

void CRegDist::setTreshhold(float tr)
{
    tt::limitValue(0.0001f,10000.0f,tr);
    treshhold=tr;
}

float CRegDist::getTreshhold() const
{
    return(treshhold);
}

void CRegDist::setTreshholdActive(bool active)
{
    treshholdActive=active;
}

bool CRegDist::getTreshholdActive() const
{
    return(treshholdActive);
}

void CRegDist::clearDistanceResult()
{
    distanceIsValid=false;
    _calcTimeInMs=0;
}

void CRegDist::setDisplaySegment(bool display)
{
    displaySegment=display;
}

void CRegDist::setExplicitHandling(bool explicitHandl)
{
    explicitHandling=explicitHandl;
}

bool CRegDist::getExplicitHandling() const
{
    return(explicitHandling);
}

bool CRegDist::getDisplaySegment() const
{
    return(displaySegment);
}

bool CRegDist::getDistanceResult(float dist[7]) const
{ // Return value false means the distance was not measured or above the specified treshhold.
    if (distanceIsValid)
    {
        dist[0]=distanceResult[0];
        dist[1]=distanceResult[1];
        dist[2]=distanceResult[2];
        dist[3]=distanceResult[3];
        dist[4]=distanceResult[4];
        dist[5]=distanceResult[5];
        dist[6]=distanceResult[6];
    }
    return(distanceIsValid);
}

float CRegDist::getCalculationTime() const
{
    return(float(_calcTimeInMs)*0.001f);
}

float CRegDist::handleDistance()
{   
    distanceIsValid=false;
    _calcTimeInMs=0;
    if (!App::ct->mainSettings->distanceCalculationEnabled)
        return(-1.0);
    if (!CPluginContainer::isMeshPluginAvailable())
        return(-1.0);
    int stTime=VDateTime::getTimeInMs();
    _distance=SIM_MAX_FLOAT;
    if (treshholdActive)
        _distance=treshhold;
    distanceIsValid=CDistanceRoutine::getDistanceBetweenEntitiesIfSmaller(object1ID,object2ID,_distance,distanceResult,distanceBuffer,distanceBuffer+2,false,false);

    _calcTimeInMs=VDateTime::getTimeDiffInMs(stTime);
    return(readDistance());
}

float CRegDist::readDistance() const
{
    if (distanceIsValid)
        return(_distance);
    return(-1.0); // keep this negative. Indicates an invalid distance
}

bool CRegDist::announceObjectWillBeErased(int objID,bool copyBuffer)
{ // Return value true means that this distance object should be destroyed
    if (object1ID==objID)
        return(true);
    if (object2ID==objID)
        return(true);
    return(false);
}

bool CRegDist::announceCollectionWillBeErased(int groupID,bool copyBuffer)
{ // Return value true means that this distance object should be destroyed
    if (object1ID==groupID)
        return(true);
    if (object2ID==groupID)
        return(true);
    return(false);
}

void CRegDist::performObjectLoadingMapping(std::vector<int>* map)
{
    if (object1ID<SIM_IDSTART_COLLECTION)
        object1ID=App::ct->objCont->getLoadingMapping(map,object1ID);
    if ( (object2ID<SIM_IDSTART_COLLECTION)&&(object2ID!=-1) )
        object2ID=App::ct->objCont->getLoadingMapping(map,object2ID);
}

void CRegDist::performGroupLoadingMapping(std::vector<int>* map)
{
    if (object1ID>=SIM_IDSTART_COLLECTION)
        object1ID=App::ct->objCont->getLoadingMapping(map,object1ID);
    if (object2ID>=SIM_IDSTART_COLLECTION)
        object2ID=App::ct->objCont->getLoadingMapping(map,object2ID);
}

CRegDist* CRegDist::copyYourself()
{
    CRegDist* newDistObj=new CRegDist(object1ID,object2ID,objectName,objectID);
    newDistObj->treshhold=treshhold;
    newDistObj->treshholdActive=treshholdActive;
    newDistObj->displaySegment=displaySegment;
    newDistObj->explicitHandling=explicitHandling;

    newDistObj->_segmentWidth=_segmentWidth;
    segmentColor.copyYourselfInto(&newDistObj->segmentColor);

    newDistObj->_initialValuesInitialized=_initialValuesInitialized;
    newDistObj->_initialExplicitHandling=_initialExplicitHandling;

    return(newDistObj);
}

void CRegDist::setSegmentWidth(int w)
{
    w=tt::getLimitedInt(1,4,w);
    _segmentWidth=w;
}

int CRegDist::getSegmentWidth() const
{
    return(_segmentWidth);
}

void CRegDist::serialize(CSer& ar)
{
    if (ar.isStoring())
    {       // Storing
        ar.storeDataName("Inx");
        ar << object1ID << object2ID << objectID;
        ar.flush();

        ar.storeDataName("Trh");
        ar << treshhold;
        ar.flush();

        ar.storeDataName("Swt");
        ar << _segmentWidth;
        ar.flush();

        ar.storeDataName("Col");
        ar.setCountingMode();
        segmentColor.serialize(ar,1);
        if (ar.setWritingMode())
            segmentColor.serialize(ar,1);

        ar.storeDataName("Var");
        unsigned char nothing=0;
        SIM_SET_CLEAR_BIT(nothing,0,treshholdActive);
        SIM_SET_CLEAR_BIT(nothing,1,!displaySegment);
        SIM_SET_CLEAR_BIT(nothing,2,explicitHandling);
        ar << nothing;
        ar.flush();

        ar.storeDataName("Nme");
        ar << objectName;
        ar.flush();

        ar.storeDataName(SER_END_OF_OBJECT);
    }
    else
    {       // Loading
        int byteQuantity;
        std::string theName="";
        while (theName.compare(SER_END_OF_OBJECT)!=0)
        {
            theName=ar.readDataName();
            if (theName.compare(SER_END_OF_OBJECT)!=0)
            {
                bool noHit=true;
                if (theName.compare("Inx")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> object1ID >> object2ID >> objectID;
                }
                if (theName.compare("Trh")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> treshhold;
                }
                if (theName.compare("Swt")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> _segmentWidth;
                }
                if (theName.compare("Col")==0)
                {
                    noHit=false;
                    ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                    segmentColor.serialize(ar,1);
                }
                if (theName.compare("Var")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    unsigned char nothing;
                    ar >> nothing;
                    treshholdActive=SIM_IS_BIT_SET(nothing,0);
                    displaySegment=!SIM_IS_BIT_SET(nothing,1);
                    explicitHandling=SIM_IS_BIT_SET(nothing,2);
                }
                if (theName.compare("Nme")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> objectName;
                }
                if (noHit)
                    ar.loadUnknownData();
            }
        }
    }
}

void CRegDist::displayDistanceSegment()
{
    if ( (!distanceIsValid)||(!displaySegment) )
        return;
    displayDistance(this,_segmentWidth,distanceResult);
}
