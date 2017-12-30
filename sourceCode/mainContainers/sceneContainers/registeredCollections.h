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
#include "regCollection.h"
#include "shape.h"
#include "dummy.h"
#include "mainCont.h"

class CRegisteredCollections : public CMainCont
{
public:
    CRegisteredCollections();
    virtual ~CRegisteredCollections();

    void simulationAboutToStart();
    void simulationEnded();
    void renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib);
    void newScene();
    void actualizeAllGroups();
    void announceObjectWillBeErased(int objID);
    void setUpDefaultValues();
    void removeAllCollections();
    CRegCollection* getCollection(int groupID);
    CRegCollection* getCollection(std::string groupName);
    void addCollection(CRegCollection* theGroup,bool objectIsACopy);
    void addCollectionWithSuffixOffset(CRegCollection* theGroup,bool objectIsACopy,int suffixOffset);
    void getMinAndMaxNameSuffixes(int& minSuffix,int& maxSuffix);
    bool canSuffix1BeSetToSuffix2(int suffix1,int suffix2);
    void setSuffix1ToSuffix2(int suffix1,int suffix2);
    bool removeCollection(int groupID);
    void unmarkAll();
    void mark(int objID);
    void unmark(int objID);
    bool isObjectInMarkedCollection(int objID);
    void repairMarking();
    bool getShapesAndDummiesFromCollection(int groupID,std::vector<C3DObject*>* objInGroup,int propMask,bool pathPlanningRoutineCalling);
    bool getShapesAndVolumesFromCollection(int groupID,std::vector<C3DObject*>* objInGroup,int propMask,bool pathPlanningRoutineCalling);

    void getCollidableObjectsFromCollection(int collectionID,std::vector<C3DObject*>& objects);
    void getMeasurableObjectsFromCollection(int collectionID,std::vector<C3DObject*>& objects);
    void getDetectableObjectsFromCollection(int collectionID,std::vector<C3DObject*>& objects,int detectableMask);



    void performObjectLoadingMapping(std::vector<int>* map);
    void addCollectionToSelection(int groupID);
    void removeCollectionFromSelection(int groupID);
    void deselectAllCollections();
    bool isCollectionInSelection(int groupID);

    // Variable that need to be serialized on an individual basis:
    std::vector<CRegCollection*> allCollections;
    // Various
    std::vector<int> selectedCollections;
};
