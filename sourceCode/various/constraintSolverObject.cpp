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
#include "constraintSolverObject.h"
#include "tt.h"

#include "app.h"
#include "geometricConstraintSolverInt.h"
#include "v_repStrings.h"
#include "vDateTime.h"

CConstraintSolverObject::CConstraintSolverObject()
{
    _maxIterations=20;
    _interpolation=1.0f;
    _generalDamping=0.0f;
    _maxAngleVariation=10.0f*degToRad;
    _maxLinearVariation=0.05f;
    _loopClosurePositionTolerance=0.001f;
    _loopClosureOrientationTolerance=1.0f*degToRad;
    _base3DObject=-1;
    _objectID=SIM_IDSTART_GCSOBJECT;
    _explicitHandling=false;
    _calcTimeInMs=0;
    _initialValuesInitialized=false;
    _objectName=IDSOGL_MECHANISM;
}

CConstraintSolverObject::~CConstraintSolverObject()
{
}

void CConstraintSolverObject::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
    _initialValuesInitialized=simulationIsRunning;
    if (simulationIsRunning)
    {
        _initialExplicitHandling=_explicitHandling;
    }
}

void CConstraintSolverObject::simulationAboutToStart()
{
    initializeInitialValues(true);
}

void CConstraintSolverObject::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
    if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
    {
        _explicitHandling=_initialExplicitHandling;
    }
    _initialValuesInitialized=false;
}

void CConstraintSolverObject::setObjectID(int newID)
{
    _objectID=newID;
}

int CConstraintSolverObject::getObjectID()
{
    return(_objectID);
}

float CConstraintSolverObject::getCalculationTime()
{
    return(float(_calcTimeInMs)*0.001f);
}

void CConstraintSolverObject::setObjectName(const char* newName)
{
    _objectName=newName;
}

std::string CConstraintSolverObject::getObjectName()
{
    return(_objectName);
}

void CConstraintSolverObject::setBase3DObject(int objID)
{
    _base3DObject=objID;
}

int CConstraintSolverObject::getBase3DObject()
{
    return(_base3DObject);
}

void CConstraintSolverObject::setExplicitHandling(bool explHandl)
{
    _explicitHandling=explHandl;
}

bool CConstraintSolverObject::getExplicitHandling()
{
    return(_explicitHandling);
}

void CConstraintSolverObject::setMaximumIterations(int maxIt)
{
    tt::limitValue(1,200,maxIt);
    _maxIterations=maxIt;
}

int CConstraintSolverObject::getMaximumIterations()
{
    return(_maxIterations);
}

void CConstraintSolverObject::setInterpolation(float interpol)
{
    tt::limitValue(0.01f,1.0f,interpol);
    _interpolation=interpol;
}

float CConstraintSolverObject::getInterpolation()
{
    return(_interpolation);
}

void CConstraintSolverObject::setGeneralDamping(float damping)
{
    tt::limitValue(0.0f,1.0f,damping);
    _generalDamping=damping;
}

float CConstraintSolverObject::getGeneralDamping()
{
    return(_generalDamping);
}

void CConstraintSolverObject::setMaxAngleVariation(float maxVar)
{
    tt::limitValue(0.1f*degToRad,180.0f*degToRad,maxVar);
    _maxAngleVariation=maxVar;
}

float CConstraintSolverObject::getMaxAngleVariation()
{
    return(_maxAngleVariation);
}

void CConstraintSolverObject::setMaxLinearVariation(float maxVar)
{
    tt::limitValue(0.001f,100.0f,maxVar);
    _maxLinearVariation=maxVar;
}

float CConstraintSolverObject::getMaxLinearVariation()
{
    return(_maxLinearVariation);
}

void CConstraintSolverObject::setLoopClosurePositionTolerance(float tol)
{
    tt::limitValue(0.000001f,1.0f,tol);
    _loopClosurePositionTolerance=tol;
}

float CConstraintSolverObject::getLoopClosurePositionTolerance()
{
    return(_loopClosurePositionTolerance);
}

void CConstraintSolverObject::setLoopClosureOrientationTolerance(float tol)
{
    tt::limitValue(0.0001f*degToRad,45.0f*degToRad,tol);
    _loopClosureOrientationTolerance=tol;
}

float CConstraintSolverObject::getLoopClosureOrientationTolerance()
{
    return(_loopClosureOrientationTolerance);
}

void CConstraintSolverObject::selectAllObjectsInMechanism()
{
    App::ct->objCont->deselectObjects();
    App::ct->objCont->actualizeMechanismIDs();
    C3DObject* it=App::ct->objCont->getObject(_base3DObject);
    if (it!=NULL)
    {
        int mechanismID=it->getMechanismID();
        for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
        {
            int theID=App::ct->objCont->objectList[i];
            C3DObject* it2=App::ct->objCont->getObject(theID);
            if ( (theID!=_base3DObject)&&(it2!=NULL) )
            {
                if (it2->getMechanismID()==mechanismID)
                    App::ct->objCont->addObjectToSelection(theID);
            }
        }
        App::ct->objCont->addObjectToSelection(_base3DObject);
    }
}

void CConstraintSolverObject::performObjectLoadingMapping(std::vector<int>* map)
{
    _base3DObject=App::ct->objCont->getLoadingMapping(map,_base3DObject);
}

bool CConstraintSolverObject::announceObjectWillBeErased(int objID,bool copyBuffer)
{
    return (objID==_base3DObject); // this object maybe needs destruction
}

CConstraintSolverObject* CConstraintSolverObject::copyYourself()
{
    CConstraintSolverObject* newObj=new CConstraintSolverObject();
    newObj->setObjectID(getObjectID());
    newObj->setObjectName(getObjectName().c_str());
    newObj->setBase3DObject(getBase3DObject());
    newObj->setExplicitHandling(getExplicitHandling());
    newObj->setMaximumIterations(getMaximumIterations());
    newObj->setInterpolation(getInterpolation());
    newObj->setGeneralDamping(getGeneralDamping());
    newObj->setMaxAngleVariation(getMaxAngleVariation());
    newObj->setMaxLinearVariation(getMaxLinearVariation());
    newObj->setLoopClosurePositionTolerance(getLoopClosurePositionTolerance());
    newObj->setLoopClosureOrientationTolerance(getLoopClosureOrientationTolerance());

    newObj->_initialValuesInitialized=_initialValuesInitialized;
    newObj->_initialExplicitHandling=_initialExplicitHandling;

    return(newObj);
}

bool CConstraintSolverObject::computeGcs()
{
    C3DObject* it=App::ct->objCont->getObject(_base3DObject);
    if (it==NULL)
        return(false);
    CGeometricConstraintSolverInt work(_base3DObject,_objectID);
    App::ct->calcInfo->gcsStart();
    int stTime=VDateTime::getTimeInMs();
    work.solve();
    _calcTimeInMs=VDateTime::getTimeDiffInMs(stTime);
    App::ct->calcInfo->gcsEnd();
    return(true);
}

void CConstraintSolverObject::serialize(CSer& ar)
{
    if (ar.isStoring())
    {       // Storing
        ar.storeDataName("Ipa");
        ar << _objectID << _base3DObject << _maxIterations;
        ar.flush();

        ar.storeDataName("Gon");
        ar << _objectName;
        ar.flush();

        ar.storeDataName("Fpa");
        ar << _interpolation << _generalDamping << _maxAngleVariation;
        ar << _maxLinearVariation << _loopClosurePositionTolerance << _loopClosureOrientationTolerance;
        ar.flush();

        ar.storeDataName("Var");
        unsigned char dummy=0;
        SIM_SET_CLEAR_BIT(dummy,0,_explicitHandling);
        ar << dummy;
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
                if (theName.compare("Ipa")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> _objectID >> _base3DObject >> _maxIterations;
                }
                if (theName.compare("Gon")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> _objectName;
                }
                if (theName.compare("Fpa")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> _interpolation >> _generalDamping >> _maxAngleVariation;
                    ar >> _maxLinearVariation >> _loopClosurePositionTolerance >> _loopClosureOrientationTolerance;
                }
                if (theName.compare("Var")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    unsigned char dummy;
                    ar >> dummy;
                    _explicitHandling=SIM_IS_BIT_SET(dummy,0);
                }
                if (noHit)
                    ar.loadUnknownData();
            }
        }
    }
}
