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

#include "viewSelector.h"
#include "pageSelector.h"
#include "sceneSelector.h"
#include "buttonBlock.h"
#include "hierarchy.h"

class COglSurface
{
public:
    COglSurface();
    virtual ~COglSurface();

    void adjustBrowserAndHierarchySizesToDefault();
    void setSurfaceSizeAndPosition(int sizeX,int sizeY,int posX,int posY);
    bool leftMouseButtonDown(int x,int y,int selectionStatus);
    bool getMouseRelPosObjectAndViewSize(int x,int y,int relPos[2],int& objType,int& objID,int vSize[2],bool& viewIsPerspective);
    void leftMouseButtonUp(int x,int y);
    void mouseMove(int x,int y,bool passiveAndFocused);
    int modelDragMoveEvent(int xPos,int yPos,C3Vector* desiredModelPosition);
    int getCursor(int x,int y);
    bool _hierarchyResizingMousePosition(int x,int y);
    void mouseWheel(int deltaZ,int x,int y);
    bool rightMouseButtonDown(int x,int y);
    void rightMouseButtonUp(int x,int y,int absX,int absY,QWidget* mainWindow);
    bool middleMouseButtonDown(int x,int y);
    void middleMouseButtonUp(int x,int y);
    bool leftMouseButtonDoubleClick(int x,int y,int selectionStatus);
    unsigned char* render(int currentCursor,int mouseButtonState,int mousePos[2],int* frameResol);
    void setViewSelectionActive(bool isActive);
    void setPageSelectionActive(bool isActive);
    void setSceneSelectionActive(bool isActive);
    void setHierarchySurfaceSizeAndPosition();
    void setViewSurfaceSizeAndPosition();
    void actualizeAllSurfacesSizeAndPosition();
    void setUpDefaultValues();
    void startViewSelection(int objectType,int subViewIndex);
    void keyPress(int key,QWidget* mainWindow);
    void setHierarchyEnabled(bool isEnabled);
    bool isHierarchyEnabled();
    int getHierarchyWidth();
    void setHierarchyWidth(int w);
    void setFocusObject(int obj);
    int getFocusObject();
    int getCaughtElements();
    void clearCaughtElements(int keepMask);
    bool isViewSelectionActive();
    bool isPageSelectionActive();
    bool isSceneSelectionActive();
    bool isScenePageOrViewSelectionActive();

    CViewSelector* viewSelector;
    CPageSelector* pageSelector;
    CSceneSelector* sceneSelector;

    CHierarchy* hierarchy;

    static bool _hierarchyEnabled;

private:

    bool sceneSelectionActive;
    bool viewSelectionActive;
    bool pageSelectionActive;
    bool _hierarchyClosingButtonDown;
    bool _hierarchyResizingAction;

    int _caughtElements; // This refers to the hierarchy closing button and the hierarchy/browser resizing bar only!

    static int _hierarchyWidth;
    static int _hierarchyMinWidth;

    bool _readyToRender;
    int surfaceSize[2];
    int surfacePosition[2];

    int mouseRelativePosition[2];
    int mousePreviousRelativePosition[2];

    int focusObject;
};
