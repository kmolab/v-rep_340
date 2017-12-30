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
#include "pointCloudContainer_old.h"
#include "viewableBase.h"
#include "easyLock.h"

CPointCloudContainer_old::CPointCloudContainer_old()
{
}

CPointCloudContainer_old::~CPointCloudContainer_old()
{
    removeAllObjects(false);
}

void CPointCloudContainer_old::simulationAboutToStart()
{

}

void CPointCloudContainer_old::simulationEnded()
{
    removeAllObjects(true);
//  if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
//  {
//  }
}

void CPointCloudContainer_old::emptySceneProcedure()
{ // don't do anything here! (plugin or add-on might be using that functionality too) 
}

CPtCloud_old* CPointCloudContainer_old::getObject(int objectID)
{
    for (int i=0;i<int(_allObjects.size());i++)
    {
        if (_allObjects[i]->getObjectID()==objectID)
            return(_allObjects[i]);
    }
    return(NULL);
}

int CPointCloudContainer_old::addObject(CPtCloud_old* it)
{
    EASYLOCK(_objectMutex);
    int newID=0;
    newID++;
    while (getObject(newID)!=NULL)
        newID++;
    it->setObjectID(newID);
    _allObjects.push_back(it);
    return(newID);
}

void CPointCloudContainer_old::removeAllObjects(bool onlyNonPersistentOnes)
{
    EASYLOCK(_objectMutex);
    for (int i=0;i<int(_allObjects.size());i++)
    {
        if ( (!onlyNonPersistentOnes)||(!_allObjects[i]->isPersistent()) )
        {
            delete _allObjects[i];
            _allObjects.erase(_allObjects.begin()+i);
            i--; // reprocess this position
        }
    }
}

bool CPointCloudContainer_old::removeObject(int objectID)
{
    EASYLOCK(_objectMutex);
    for (int i=0;i<int(_allObjects.size());i++)
    {
        if (_allObjects[i]->getObjectID()==objectID)
        {
            delete _allObjects[i];
            _allObjects.erase(_allObjects.begin()+i);
            return(true);
        }
    }
    return(false);
}

void CPointCloudContainer_old::announceObjectWillBeErased(int objID)
{ // Never called from copy buffer!
    int i=0;
    while (i<int(_allObjects.size()))
    {
        if (_allObjects[i]->announceObjectWillBeErased(objID))
        {
            delete _allObjects[i];
            _allObjects.erase(_allObjects.begin()+i);
        }
        else
            i++;
    }
}

void CPointCloudContainer_old::renderYour3DStuff_nonTransparent(CViewableBase* renderingObject,int displayAttrib)
{
    if ((displayAttrib&sim_displayattribute_nopointclouds)==0)
        drawAll(displayAttrib);
}

void CPointCloudContainer_old::renderYour3DStuff_transparent(CViewableBase* renderingObject,int displayAttrib)
{
}

void CPointCloudContainer_old::renderYour3DStuff_overlay(CViewableBase* renderingObject,int displayAttrib)
{
}

void CPointCloudContainer_old::drawAll(int displayAttrib)
{
    EASYLOCK(_objectMutex);
    for (size_t i=0;i<_allObjects.size();i++)
        _allObjects[i]->draw(displayAttrib);
}
