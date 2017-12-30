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
#include "shapeEditMode.h"
#include "multishapeEditMode.h"
#include "pathEditMode.h"
#include "uiEditMode.h"
#include "pathPointManipulation.h"

class CEditModeContainer
{
public:
    CEditModeContainer();
    virtual ~CEditModeContainer();

    bool enterEditMode(int objID,int modeType);
    void endEditMode(bool cancelChanges);
    int getEditModeObjectID();
    CShape* getEditModeShape();
    CPath* getEditModePath();
    C3DObject* getEditModeObject();
    CPathCont* getEditModePathContainer();
    int getEditModeType();
    void swapShapeEditModeType(int theType);

    int getEditModeBufferSize();
    int getLastEditModeBufferValue();
    int getEditModeBufferValue(int index);
    std::vector<int>* getEditModeBuffer();
    void deselectEditModeBuffer();
    void removeItemFromEditModeBuffer(int item);
    void xorAddItemToEditModeBuffer(int item,bool disableEdgeFollowing);
    void addItemToEditModeBuffer(int item,bool disableEdgeFollowing);

    CShapeEditMode* getShapeEditMode();
    CMultishapeEditMode* getMultishapeEditMode();
    CPathEditMode* getPathEditMode();
    CUiEditMode* getUiEditMode();

    void addMenu(VMenu* menu,C3DObject* viewableObject);
    bool processCommand(int commandID,C3DObject* viewableObject);
    bool keyPress(int key);


    void announceObjectSelectionChanged();
    void announceSceneInstanceChanged();
    void simulationAboutToStart();
    void simulationEnded();

    CPathPointManipulation* pathPointManipulation;

private:
    bool _processShapeEditModeCommand(int commandID);
    bool _processMultishapeEditModeCommand(int commandID);
    bool _processPathEditModeCommand(int commandID,C3DObject* viewableObject);
    bool _processUiEditModeCommand(int commandID);

    CShapeEditMode* _shapeEditMode;
    CMultishapeEditMode* _multishapeEditMode;
    CPathEditMode* _pathEditMode;
    CUiEditMode* _uiEditMode;

    int _editModeObject;
    bool _editMode_hierarchyWasEnabledBeforeEditMode;
    bool _modelBrowserVisibilityBeforeEditMode;
    bool _simulationStopped;
};
