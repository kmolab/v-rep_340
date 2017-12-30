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
#include "regCollectionEl.h"

class CRegCollection
{
public:
    CRegCollection(std::string grName);
    virtual ~CRegCollection();

    void initializeInitialValues(bool simulationIsRunning);
    void simulationAboutToStart();
    void simulationEnded();

    bool actualizeCollection(bool full=true);
    bool isObjectInCollection(int objID);
    CRegCollectionEl* getSubCollection(int subGroupID);
    void addSubCollection(CRegCollectionEl* subGr);
    bool removeSubCollection(int subGroupID);
    void unmarkAll();
    void mark(int objID);
    void unmark(int objID);
    bool areAllMarked();
    void serialize(CSer& ar);
    void performObjectLoadingMapping(std::vector<int>* map);
    bool announceObjectWillBeErased(int objID,bool copyBuffer);
    int getCollectionID();
    void setCollectionID(int newID);
    std::string getCollectionName();
    void setCollectionName(std::string newName);
    bool getOverridesObjectMainProperties();
    void setOverridesObjectMainProperties(bool o);
    CRegCollection* copyYourself();
    void emptyCollection();

    // Variables which need to be serialized
    std::vector<CRegCollectionEl*> subCollectionList;
    // Other
    std::vector<int> collectionObjects;

private:
    // Variables which need to be serialized
    std::string groupName;
    int groupID;
    bool _overridesObjectMainProperties;
    // Other
    std::vector<int> markedObjects;
};
