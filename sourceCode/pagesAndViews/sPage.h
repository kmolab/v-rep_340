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

// This file requires some serious refactoring!

#pragma once

#include "vrepMainHeader.h"
#include "sView.h"
#include "ser.h"

class CSPage  
{
public:
    CSPage(int thePageType);
    virtual ~CSPage();
    void render();
    void initializeInitialValues(bool simulationIsRunning,int initializeOnlyForThisNewObject);
    void simulationEnded();
    void serialize(CSer& ar);
    bool setPageType(int newType);
    int getPageType() const;
    void announceObjectWillBeErased(int objectID);
    void performObjectLoadingMapping(std::vector<int>* map);
    void getPageSizeAndPosition(int& sizeX,int& sizeY,int& posX,int& posY) const;
    void setPageSizeAndPosition(int sizeX,int sizeY,int posX,int posY);
    CSView* getView(int viewIndex);
    void getViewSizeAndPosition(int sViewSize[2],int sViewPos[2],int subViewIndex) const;
    void getFloatingViewRelativeSizeAndPosition(float sViewSize[2],float sViewPos[2],int subViewIndex) const;
    void setViewSizesAndPositions();
    int getRegularViewCount() const;
    int getViewCount() const;
    void addFloatingView();
    void addFloatingView(CSView* theFloatingView,float relSize[2],float relPos[2]);
    void getBorderCorrectedFloatingViewPosition(int posX,int posY,int sizeX,int sizeY,int& newPosX,int& newPosY) const;
    bool removeFloatingView(int viewIndex);
    int getViewIndexFromViewUniqueID(int uniqueID) const;
    bool isViewValid(CSView* v) const;

private:
    // Variables which need to be serialized:
    int _pageType;
    int _lastMouseDownViewIndex;
    std::vector<CSView*> _allViews;
    // Positions and sizes are relative now (2009/05/22)
    std::vector<float> _allViewAuxSizesAndPos;

    std::vector<float> _initialAuxViewSizesAndPos;
    std::vector<int> _initialAuxViewUniqueIDs;
    bool _initialValuesInitialized;

    // Variables which don't need to be serialized:
    int _pagePosition[2];
    int _pageSize[2];
    int _caughtElements;
    int mouseRelativePosition[2];
    int previousMouseRelativePosition[2];

    int auxViewResizingAction;
    int viewIndexOfResizingAction;
    // Positions and sizes are relative now (2009/05/22)
    float floatingViewAddOffset;

#ifdef SIM_WITH_GUI
public:
    int getCaughtElements() const;
    void clearCaughtElements(int keepMask);
    bool viewIsPassive(int viewIndex) const;
    void swapViews(int index1,int index2,bool alsoSizeAndPosInfo);
    int bringViewToFrontIfPossible(int index);
    void getViewRelativeMousePosition(int mouseX,int mouseY,int& relMouseX,int& relMouseY,int index) const;
    int getViewIndexOfMousePosition(int mouseX,int mouseY) const;
    bool doubleClickActionForView(int viewIndex);
    void clearAllMouseJustWentDownAndUpFlags();
    int getMousePosRelativeToFloatingViewBorders(int mouseX,int mouseY,int index) const;
    void clearLastMouseDownViewIndex();
    int getLastMouseDownViewIndex() const;
    bool leftMouseButtonDown(int x,int y,int selectionStatus);
    bool getMouseRelPosObjectAndViewSize(int x,int y,int relPos[2],int& objType,int& objID,int vSize[2],bool& viewIsPerspective) const;
    void leftMouseButtonUp(int x,int y);
    int getCursor(int x,int y) const;
    void mouseMove(int x,int y,bool passiveAndFocused);
    int modelDragMoveEvent(int xPos,int yPos,C3Vector* desiredModelPosition);
    void mouseWheel(int deltaZ,int x,int y);
    bool middleMouseButtonDown(int x,int y);
    void middleMouseButtonUp(int x,int y);
    bool rightMouseButtonDown(int x,int y);
    bool rightMouseButtonUp(int x,int y,int absX,int absY,QWidget* mainWindow);
    bool leftMouseButtonDoubleClick(int x,int y,int selectionStatus);
#endif
};
