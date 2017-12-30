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
#include "path.h"
#include "objCont.h"
#include "vMenubar.h"

class CPathEditMode
{
public:
    CPathEditMode(CPath* path,CObjCont* objCont);
    virtual ~CPathEditMode();

    void endEditMode(bool cancelChanges);
    CPathCont* getEditModePathContainer();
    CPath* getEditModePath();

    int getEditModeBufferSize();
    int getLastEditModeBufferValue();
    int getEditModeBufferValue(int index);
    std::vector<int>* getEditModeBuffer();
    void deselectEditModeBuffer();
    void removeItemFromEditModeBuffer(int item);
    bool isEditModeItemAValidItem(int item);
    void xorAddItemToEditModeBuffer(int item);
    void addItemToEditModeBuffer(int item);
    bool alreadyInEditModeBuffer(int item);
    int getBezierPathPointCount();
    CSimplePathPoint* getSimplePathPoint(int editModeBufferIndex);

    void makeDummies();

    void addMenu(VMenu* menu,C3DObject* viewableObject);
    bool processCommand(int commandID,C3DObject* viewableObject);

private:
    void _copySelection(std::vector<int>* selection);
    void _paste(int insertPosition);
    void _insertNewPoint(int insertPosition);
    void _cutSelection(std::vector<int>* selection);
    void _deleteSelection(std::vector<int>* selection);
    void _keepXAxisAndAlignZAxis(std::vector<int>* selection);
    void _generatePath();

    std::vector<CSimplePathPoint*> editBufferPathPointsCopy;
    CPath* _path;
    CPathCont* _editionPathCont;
    CObjCont* _objCont;
    std::vector<int> editModeBuffer;
};
