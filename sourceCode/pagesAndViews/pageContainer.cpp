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

#include "vrepMainHeader.h"
#include "funcDebug.h"
#include "v_rep_internal.h"
#include "pageContainer.h"
#include "tt.h"
#include "sceneObjectOperations.h"
#include "simulation.h"
#include "3DObject.h"
#include "gV.h"
#include "pluginContainer.h"
#include "fileOperations.h"
#include "v_repStrings.h"
#include "app.h"
#include "pageRendering.h"
#ifdef SIM_WITH_GUI
#include "toolBarCommand.h"
#endif

CPageContainer::CPageContainer()
{
    _activePageIndex=0;
    _numberOfPages=0;
    rightMouseCaughtBy=-1;
    leftMouseCaughtBy=-1;
    prepareForPopupMenu=-1;
    rightMouseCaughtSoftDialog=false;
    leftMouseCaughtSoftDialog=false;
    _caughtElements=0;
#ifdef SIM_WITH_GUI
    setFocusObject(FOCUS_ON_PAGE);
#endif
    setUpDefaultPages();
    removeAllPages();
    _initialValuesInitialized=false;
}

CPageContainer::~CPageContainer()
{
    removeAllPages();
}

void CPageContainer::emptySceneProcedure()
{
    removeAllPages();
}

int CPageContainer::getPageCount() const
{ // YOU ARE ONLY ALLOWED TO MODIFY SIMPLE TYPES. NO OBJECT CREATION/DESTRUCTION HERE!!
    return(_numberOfPages);
}

void CPageContainer::initializeInitialValues(bool simulationIsRunning,int initializeOnlyForThisNewObject)
{
    _initialValuesInitialized=simulationIsRunning;
    if (simulationIsRunning)
    {
        _initialActivePageIndex=_activePageIndex;
    }
    for (int i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
            _allPages[i]->initializeInitialValues(simulationIsRunning,initializeOnlyForThisNewObject);
    }
}

void CPageContainer::simulationAboutToStart()
{
    initializeInitialValues(true,-1);
}

void CPageContainer::simulationEnded()
{
    if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
    {
        _activePageIndex=_initialActivePageIndex;
    }
    for (int i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
            _allPages[i]->simulationEnded();
    }
    _initialValuesInitialized=false;
}


void CPageContainer::removeAllPages()
{
    for (int i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
            delete _allPages[i];
        _allPages[i]=NULL;
    }
}
void CPageContainer::setUpDefaultPages(bool createASingleView)
{ // createASingleView is false by default
    if (_numberOfPages==0)
    {
        _numberOfPages=MAX_NUMBER_OF_PAGES;
        for (int i=0;i<_numberOfPages;i++)
            _allPages[i]=NULL;
    }
    for (int i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
        {
            delete _allPages[i];
            _allPages[i]=NULL;
        }
    }
    if (createASingleView)
    {
        CSPage* it=new CSPage(SINGLE_VIEW);
        _allPages[0]=it;
    }
    _activePageIndex=0;
}

CSPage* CPageContainer::getPage(int pageIndex) const
{ // YOU ARE ONLY ALLOWED TO MODIFY SIMPLE TYPES. NO OBJECT CREATION/DESTRUCTION HERE!!
    if ( (pageIndex<0)||(pageIndex>=_numberOfPages) )
        return(NULL); // Invalid view number
    return(_allPages[pageIndex]);   
}

void CPageContainer::announceObjectWillBeErased(int objectID)
{ // Never called from copy buffer!
    for (int i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
            _allPages[i]->announceObjectWillBeErased(objectID);
    }
}

void CPageContainer::setPageSizeAndPosition(int sizeX,int sizeY,int posX,int posY)
{
    _pageSize[0]=sizeX;
    _pageSize[1]=sizeY;
    _pagePosition[0]=posX;
    _pagePosition[1]=posY;
    // We set the view position and size for all soft dialogs:
    if (App::ct->buttonBlockContainer!=NULL)
        App::ct->buttonBlockContainer->setViewSizeAndPosition(sizeX,sizeY,posX,posY);
    // We set the view position and size for all views:
    for (int i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
            _allPages[i]->setPageSizeAndPosition(sizeX,sizeY,posX,posY);
    }
}

void CPageContainer::removePage(int pageIndex)
{
    if ( (pageIndex<0)||(pageIndex>=_numberOfPages) )
        return;
    if (_allPages[pageIndex]!=NULL)
    {
        delete _allPages[pageIndex];
        _allPages[pageIndex]=NULL;
    }
}

void CPageContainer::serialize(CSer& ar)
{
    if (ar.isStoring())
    { // Storing
        for (int i=0;i<_numberOfPages;i++)
        {
            if (_allPages[i]!=NULL)
            {
                ar.storeDataName("Vwo");
                ar.setCountingMode();
                _allPages[i]->serialize(ar);
                if (ar.setWritingMode())
                    _allPages[i]->serialize(ar);
            }
            else
            {
                ar.storeDataName("Nul");
                ar << (int)0;
                ar.flush();
            }
        }

        ar.storeDataName("Avi");
        ar << _activePageIndex;
        ar.flush();

        ar.storeDataName(SER_END_OF_OBJECT);
    }
    else
    {       // Loading
        removeAllPages();
        int viewCounter=0;
        int byteQuantity;
        std::string theName="";
        while (theName.compare(SER_END_OF_OBJECT)!=0)
        {
            theName=ar.readDataName();
            if (theName.compare(SER_END_OF_OBJECT)!=0)
            {
                bool noHit=true;
                if (theName.compare("Avi")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> _activePageIndex;
                    App::setToolbarRefreshFlag();
                }
                if (theName.compare("Vwo")==0)
                {
                    noHit=false;
                    ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
                    CSPage* theView=new CSPage(0);
                    theView->serialize(ar);
                    _allPages[viewCounter]=theView;
                    viewCounter++;
                }
                if (theName.compare("Nul")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    int tmp;
                    ar >> tmp;
                    viewCounter++;
                }
                if (noHit)
                    ar.loadUnknownData();
            }
        }
    }
}

void CPageContainer::performObjectLoadingMapping(std::vector<int>* map)
{
    for (int i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
            _allPages[i]->performObjectLoadingMapping(map);
    }
}

bool CPageContainer::processCommand(int commandID,int viewIndex)
{ // Return value is true if the command belonged to hierarchy menu and was executed
    if ( (viewIndex<0)||(viewIndex>=_numberOfPages) )
        return(false);
    if (_allPages[viewIndex]!=NULL)
        return(false);

    if ( (commandID==PAGE_CONT_FUNCTIONS_CREATE_SINGLE_VIEW_TYPE_PCCMD)||(commandID==PAGE_CONT_FUNCTIONS_CREATE_FOUR_VIEW_TYPE_PCCMD)||(commandID==PAGE_CONT_FUNCTIONS_CREATE_SIX_VIEW_TYPE_PCCMD)||
         (commandID==PAGE_CONT_FUNCTIONS_CREATE_EIGHT_VIEW_TYPE_PCCMD)||(commandID==PAGE_CONT_FUNCTIONS_CREATE_HORIZONTAL_VIEW_TYPE_PCCMD)||(commandID==PAGE_CONT_FUNCTIONS_CREATE_VERTICAL_VIEW_TYPE_PCCMD)||
         (commandID==PAGE_CONT_FUNCTIONS_CREATE_HORIZONTAL_3_VIEW_TYPE_PCCMD)||(commandID==PAGE_CONT_FUNCTIONS_CREATE_VERTICAL_3_VIEW_TYPE_PCCMD)||(commandID==PAGE_CONT_FUNCTIONS_CREATE_HORIZONTAL_1_PLUS_3_VIEW_TYPE_PCCMD)||
         (commandID==PAGE_CONT_FUNCTIONS_CREATE_VERTICAL_1_PLUS_3_VIEW_TYPE_PCCMD)||(commandID==PAGE_CONT_FUNCTIONS_CREATE_HORIZONTAL_1_PLUS_4_VIEW_TYPE_PCCMD)||(commandID==PAGE_CONT_FUNCTIONS_CREATE_VERTICAL_1_PLUS_4_VIEW_TYPE_PCCMD) )
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            CSPage* it=NULL;
            if (commandID==PAGE_CONT_FUNCTIONS_CREATE_SINGLE_VIEW_TYPE_PCCMD)
                it=new CSPage(SINGLE_VIEW);
            if (commandID==PAGE_CONT_FUNCTIONS_CREATE_FOUR_VIEW_TYPE_PCCMD)
                it=new CSPage(FOUR_VIEWS);
            if (commandID==PAGE_CONT_FUNCTIONS_CREATE_SIX_VIEW_TYPE_PCCMD)
                it=new CSPage(SIX_VIEWS);
            if (commandID==PAGE_CONT_FUNCTIONS_CREATE_EIGHT_VIEW_TYPE_PCCMD)
                it=new CSPage(EIGHT_VIEWS);
            if (commandID==PAGE_CONT_FUNCTIONS_CREATE_HORIZONTAL_VIEW_TYPE_PCCMD)
                it=new CSPage(HORIZONTALLY_DIVIDED);
            if (commandID==PAGE_CONT_FUNCTIONS_CREATE_VERTICAL_VIEW_TYPE_PCCMD)
                it=new CSPage(VERTICALLY_DIVIDED);
            if (commandID==PAGE_CONT_FUNCTIONS_CREATE_HORIZONTAL_3_VIEW_TYPE_PCCMD)
                it=new CSPage(HORIZONTALLY_DIVIDED_3);
            if (commandID==PAGE_CONT_FUNCTIONS_CREATE_VERTICAL_3_VIEW_TYPE_PCCMD)
                it=new CSPage(VERTICALLY_DIVIDED_3);
            if (commandID==PAGE_CONT_FUNCTIONS_CREATE_HORIZONTAL_1_PLUS_3_VIEW_TYPE_PCCMD)
                it=new CSPage(HORIZONTAL_1_PLUS_3_VIEWS);
            if (commandID==PAGE_CONT_FUNCTIONS_CREATE_VERTICAL_1_PLUS_3_VIEW_TYPE_PCCMD)
                it=new CSPage(VERTICAL_1_PLUS_3_VIEWS);
            if (commandID==PAGE_CONT_FUNCTIONS_CREATE_HORIZONTAL_1_PLUS_4_VIEW_TYPE_PCCMD)
                it=new CSPage(HORIZONTAL_1_PLUS_4_VIEWS);
            if (commandID==PAGE_CONT_FUNCTIONS_CREATE_VERTICAL_1_PLUS_4_VIEW_TYPE_PCCMD)
                it=new CSPage(VERTICAL_1_PLUS_4_VIEWS);
            App::addStatusbarMessage(IDSNS_CREATED_VIEWS);
            if (it!=NULL)
            {
                it->setPageSizeAndPosition(_pageSize[0],_pageSize[1],_pagePosition[0],_pagePosition[1]);
                _allPages[viewIndex]=it;
                POST_SCENE_CHANGED_ANNOUNCEMENT(""); // ************************** UNDO thingy **************************
            }
        }
        else
        { // We are in the UI thread. Execute the command via the main thread:
            SSimulationThreadCommand cmd;
            cmd.cmdId=commandID;
            cmd.intParams.push_back(viewIndex);
            App::appendSimulationThreadCommand(cmd);
        }
        return(true);
    }
    return(false);
}

void CPageContainer::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{
}

#ifdef SIM_WITH_GUI
void CPageContainer::renderCurrentPage(bool hideWatermark)
{
    FUNCTION_DEBUG;
    if (App::getEditModeType()!=BUTTON_EDIT_MODE)
    {
        CSPage* it=getPage(_activePageIndex);
        displayContainerPage(it,_pagePosition,_pageSize);
    }

    // Now we have to clear all mouseJustWentDown and mouseJustWentUp flag in case
    // it was not processed
    for (int  i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
            _allPages[i]->clearAllMouseJustWentDownAndUpFlags();
    }

    displayContainerPageOverlay(_pagePosition,_pageSize,_activePageIndex,focusObject);

    int tagId=-1;
    // If you are using an EDU license for the DYNAMICS plugin, MESH CALCULATION plugin or PATH PLANNING plugin, you have to keep the V-REP tag in place!!
    if ((!hideWatermark)&&(App::mainWindow!=NULL)&&(!App::mainWindow->simulationRecorder->getIsRecording()))
    {
        // If you are using V-REP in conjunction with the DYNAMICS PLUGIN or the MESH CALCULATION PLUGIN, you have to keep the EDU tag!! See the license agreements for details.
        if (CPluginContainer::isMeshPluginAvailable())
            tagId=EDU_TAG;
        else
            tagId=VREP_TAG;
    }
    if (tagId!=-1)
        displayContainerPageWatermark(_pagePosition,_pageSize,tagId);
}

int CPageContainer::getActivePageIndex() const
{
    return(_activePageIndex);
}

void CPageContainer::setActivePage(int pageIndex)
{
    if ( (pageIndex<_numberOfPages)&&(pageIndex>=0) )
    {
        _activePageIndex=pageIndex;
        clearAllLastMouseDownViewIndex(); // foc
        App::setToolbarRefreshFlag();
    }
}

void CPageContainer::addPageMenu(VMenu* menu)
{
    menu->appendMenuItem(true,false,PAGE_CONT_FUNCTIONS_CREATE_SINGLE_VIEW_TYPE_PCCMD,IDS_VIEW_TYPE_SINGLE_MENU_ITEM);
    menu->appendMenuItem(true,false,PAGE_CONT_FUNCTIONS_CREATE_HORIZONTAL_VIEW_TYPE_PCCMD,IDS_VIEW_TYPE_TWO_HORIZ_MENU_ITEM);
    menu->appendMenuItem(true,false,PAGE_CONT_FUNCTIONS_CREATE_VERTICAL_VIEW_TYPE_PCCMD,IDS_VIEW_TYPE_TWO_VERT_MENU_ITEM);
    menu->appendMenuItem(true,false,PAGE_CONT_FUNCTIONS_CREATE_HORIZONTAL_3_VIEW_TYPE_PCCMD,IDS_VIEW_TYPE_THREE_HORIZ_MENU_ITEM);
    menu->appendMenuItem(true,false,PAGE_CONT_FUNCTIONS_CREATE_VERTICAL_3_VIEW_TYPE_PCCMD,IDS_VIEW_TYPE_THREE_VERT_MENU_ITEM);
    menu->appendMenuItem(true,false,PAGE_CONT_FUNCTIONS_CREATE_HORIZONTAL_1_PLUS_3_VIEW_TYPE_PCCMD,IDS_VIEW_TYPE_HORIZONTAL_ONE_PLUS_THREE_MENU_ITEM);
    menu->appendMenuItem(true,false,PAGE_CONT_FUNCTIONS_CREATE_VERTICAL_1_PLUS_3_VIEW_TYPE_PCCMD,IDS_VIEW_TYPE_VERTICAL_ONE_PLUS_THREE_MENU_ITEM);
    menu->appendMenuItem(true,false,PAGE_CONT_FUNCTIONS_CREATE_HORIZONTAL_1_PLUS_4_VIEW_TYPE_PCCMD,IDS_VIEW_TYPE_HORIZONTAL_ONE_PLUS_FOUR_MENU_ITEM);
    menu->appendMenuItem(true,false,PAGE_CONT_FUNCTIONS_CREATE_VERTICAL_1_PLUS_4_VIEW_TYPE_PCCMD,IDS_VIEW_TYPE_VERTICAL_ONE_PLUS_FOUR_MENU_ITEM);
    menu->appendMenuItem(true,false,PAGE_CONT_FUNCTIONS_CREATE_FOUR_VIEW_TYPE_PCCMD,IDS_VIEW_TYPE_FOUR_MENU_ITEM);
    menu->appendMenuItem(true,false,PAGE_CONT_FUNCTIONS_CREATE_SIX_VIEW_TYPE_PCCMD,IDS_VIEW_TYPE_SIX_MENU_ITEM);
    menu->appendMenuItem(true,false,PAGE_CONT_FUNCTIONS_CREATE_EIGHT_VIEW_TYPE_PCCMD,IDS_VIEW_TYPE_EIGHT_MENU_ITEM);
}

bool CPageContainer::getMouseRelPosObjectAndViewSize(int x,int y,int relPos[2],int& objType,int& objID,int vSize[2],bool& viewIsPerspective) const
{ // NOT FULLY IMPLEMENTED! objType=-1 --> not supported, 0 --> hierarchy, 1 --> 3DViewable
    if ( (x<0)||(x>_pageSize[0])||(y<0)||(y>_pageSize[1]) )
        return(false);
    // The position is in this window zone
    if ((App::ct->buttonBlockContainer!=NULL)&&App::ct->buttonBlockContainer->mouseDownTest(x,y,_activePageIndex))
    { // not yet supported
        objType=-1;
        return(true);
    }
    const CSPage* it=getPage(_activePageIndex);
    if (it!=NULL)
    {
        if (it->getMouseRelPosObjectAndViewSize(x,y,relPos,objType,objID,vSize,viewIsPerspective))
            return(true); // we are in this view
    }
    return(false);
}

bool CPageContainer::leftMouseButtonDown(int x,int y,int selectionStatus)
{
    if ( (x<0)||(x>_pageSize[0])||(y<0)||(y>_pageSize[1]) )
        return(false);
    // The mouse went down in this window zone
    for (int i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
            _allPages[i]->clearCaughtElements(0xffff-sim_left_button);
    }
    if (App::ct->buttonBlockContainer!=NULL)
        App::ct->buttonBlockContainer->clearCaughtElements(0xffff-sim_left_button);

    mouseRelativePosition[0]=x;
    mouseRelativePosition[1]=y;
    leftMouseCaughtBy=-1;
    leftMouseCaughtSoftDialog=false;
    if ((App::ct->buttonBlockContainer!=NULL)&&App::ct->buttonBlockContainer->mouseDown(mouseRelativePosition[0],mouseRelativePosition[1],_activePageIndex,selectionStatus))
    {
        setFocusObject(FOCUS_ON_SOFT_DIALOG);
        return(true);
    }
    setFocusObject(FOCUS_ON_PAGE);
    CSPage* it=getPage(_activePageIndex);
    if (it!=NULL)
    {
        if (it->leftMouseButtonDown(x,y,selectionStatus))
        { // The mouse was caught by this view:
            return(true);
        }
    }
    // Here we could handle mouse catch for the view container (not handled now)
    return(false);
}

void CPageContainer::clearAllLastMouseDownViewIndex()
{ // YOU ARE ONLY ALLOWED TO MODIFY SIMPLE TYPES. NO OBJECT CREATION/DESTRUCTION HERE!!
    for (int i=0;i<_numberOfPages;i++)
    {
        CSPage* p=_allPages[i];
        if (p!=NULL)
            p->clearLastMouseDownViewIndex();
    }
}

int CPageContainer::getCursor(int x,int y) const
{
    if ( (x<0)||(x>_pageSize[0])||(y<0)||(y>_pageSize[1]) )
        return(-1);
    if ((App::ct->buttonBlockContainer!=NULL)&&App::ct->buttonBlockContainer->mouseDownTest(x,y,_activePageIndex))
        return(-1);
    CSPage* it=getPage(_activePageIndex);
    if (it==NULL)
        return(-1); // The active view doesn't exist!
    return(it->getCursor(x,y));
}

void CPageContainer::leftMouseButtonUp(int x,int y)
{ // YOU ARE ONLY ALLOWED TO MODIFY SIMPLE TYPES. NO OBJECT CREATION/DESTRUCTION HERE!!
    mouseRelativePosition[0]=x;
    mouseRelativePosition[1]=y;
    if (App::ct->buttonBlockContainer!=NULL)
    {
        if (App::ct->buttonBlockContainer->getCaughtElements()&sim_left_button)
            App::ct->buttonBlockContainer->mouseUp(mouseRelativePosition[0],mouseRelativePosition[1],_activePageIndex);
    }

    CSPage* it=getPage(_activePageIndex);
    if ( (it!=NULL)&&(it->getCaughtElements()&sim_left_button) )
        it->leftMouseButtonUp(mouseRelativePosition[0],mouseRelativePosition[1]);
}

void CPageContainer::mouseMove(int x,int y,bool passiveAndFocused)
{ // YOU ARE ONLY ALLOWED TO MODIFY SIMPLE TYPES. NO OBJECT CREATION/DESTRUCTION HERE!!
    int bts=sim_right_button|sim_middle_button|sim_left_button;
    if (App::userSettings->navigationBackwardCompatibility)
        bts=sim_right_button|sim_left_button;

    mouseRelativePosition[0]=x;
    mouseRelativePosition[1]=y;

    if (App::ct->buttonBlockContainer!=NULL)
    {
        if (!passiveAndFocused)
        {
            if (App::ct->buttonBlockContainer->getCaughtElements()&bts)
                App::ct->buttonBlockContainer->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1]);
        }
        else
            App::ct->buttonBlockContainer->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1]);
    }
    for (int i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
        {
            if (!passiveAndFocused)
            {
                if (_allPages[i]->getCaughtElements()&bts)
                    _allPages[i]->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1],passiveAndFocused);
            }
            else
            {
                if (i==_activePageIndex)
                    _allPages[i]->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1],passiveAndFocused);
            }
        }
    }
}

int CPageContainer::modelDragMoveEvent(int x,int y,C3Vector* desiredModelPosition)
{
    mouseRelativePosition[0]=x;
    mouseRelativePosition[1]=y;

    CSPage* it=getPage(_activePageIndex);
    if (it!=NULL)
        return(it->modelDragMoveEvent(mouseRelativePosition[0],mouseRelativePosition[1],desiredModelPosition));
    return(0);
}

void CPageContainer::mouseWheel(int deltaZ,int x,int y)
{
    // We sent this event only to the active view if no view was caught:
    CSPage* it=getPage(_activePageIndex);
    if (it!=NULL)
        it->mouseWheel(deltaZ,x,y);
}

void CPageContainer::looseFocus()
{
    if (App::ct->buttonBlockContainer!=NULL)
        App::ct->buttonBlockContainer->looseFocus();
    setFocusObject(FOCUS_ON_UNKNOWN_OBJECT);
}

int CPageContainer::getFocusObject() const
{
    return(focusObject);
}

void CPageContainer::setFocusObject(int obj)
{ // YOU ARE ONLY ALLOWED TO MODIFY SIMPLE TYPES. NO OBJECT CREATION/DESTRUCTION HERE!!
    focusObject=obj;
    if (focusObject==FOCUS_ON_PAGE)
    {
        if (App::ct->buttonBlockContainer!=NULL)
        {
            App::ct->buttonBlockContainer->setEditBoxEdition(-1,-1,false);
            App::ct->buttonBlockContainer->looseFocus();
        }
    }
}

void CPageContainer::keyPress(int key,QWidget* mainWindow)
{ // YOU ARE ONLY ALLOWED TO MODIFY SIMPLE TYPES. NO OBJECT CREATION/DESTRUCTION HERE!!
    if ((focusObject==FOCUS_ON_SOFT_DIALOG)&&(App::ct->buttonBlockContainer!=NULL))
    {
        App::ct->buttonBlockContainer->onKeyDown(key);
        return;
    }

    if ( (key==CTRL_V_KEY)||(key==DELETE_KEY)||(key==BACKSPACE_KEY)||(key==CTRL_X_KEY)||(key==CTRL_C_KEY)||(key==ESC_KEY)||(key==CTRL_A_KEY)||(key==CTRL_Y_KEY)||(key==CTRL_Z_KEY) )
    {
        if ( (App::mainWindow!=NULL)&&(!App::mainWindow->editModeContainer->keyPress(key)) )
            CSceneObjectOperations::keyPress(key); // the key press was not for the edit mode
        return;
    }

    if ( (key==CTRL_S_KEY)||(key==CTRL_O_KEY)||(key==CTRL_W_KEY)||(key==CTRL_Q_KEY)||(key==CTRL_N_KEY) )
    {
        CFileOperations::keyPress(key);
        return;
    }

    if (key==CTRL_SPACE_KEY)
    {
        App::ct->simulation->keyPress(key);
        return;
    }

    if (key==CTRL_E_KEY)
    {
        App::ct->keyPress(key);
        return;
    }
    if ( (key==CTRL_D_KEY)||(key==CTRL_G_KEY) )
    {
        App::mainWindow->dlgCont->keyPress(key);
        return;
    }

    int flags=0;
    if (App::mainWindow!=NULL)
    {
        if (App::mainWindow->getKeyDownState()&1)
            flags|=1;
        if (App::mainWindow->getKeyDownState()&2)
            flags|=2;
    }
    int data[4]={key,flags,0,0};
    void* retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_keypress,data,NULL,NULL);
    delete[] (char*)retVal;
    data[0]=key;
    data[1]=flags;
    retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_keypress,data,NULL,NULL); // for backward compatibility
    delete[] (char*)retVal;
    App::ct->outsideCommandQueue->addCommand(sim_message_keypress,key,flags,0,0,NULL,0);

    App::ct->setModificationFlag(1024); // key was pressed
}

bool CPageContainer::rightMouseButtonDown(int x,int y)
{ // YOU ARE ONLY ALLOWED TO MODIFY SIMPLE TYPES. NO OBJECT CREATION/DESTRUCTION HERE!!
    if ( (x<0)||(y<0)||(x>_pageSize[0])||(y>_pageSize[1]) )
        return(false);
    // The mouse went down in this window zone
    mouseRelativePosition[0]=x;
    mouseRelativePosition[1]=y;

    if (App::ct->buttonBlockContainer!=NULL)
        App::ct->buttonBlockContainer->clearCaughtElements(0xffff-sim_right_button);
    for (int i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
            _allPages[i]->clearCaughtElements(0xffff-sim_right_button);
    }
    prepareForPopupMenu=-1;
    // First we check the soft dialogs:

    if (App::ct->buttonBlockContainer!=NULL)
    {
        // Following 2 lines should be replaced with the right button down handling routine (when it exists)! (and then return true!)
        if (App::ct->buttonBlockContainer->mouseDownTest(mouseRelativePosition[0],mouseRelativePosition[1],_activePageIndex))
            return(false);
    }

    setFocusObject(FOCUS_ON_PAGE);
    CSPage* it=getPage(_activePageIndex);
    if (it!=NULL)
    {
        if (it->rightMouseButtonDown(mouseRelativePosition[0],mouseRelativePosition[1]))
            return(true); // The active view caught that event!
    }
    else
    { // Here we prepare info for the popup menu in the case no view exists for that index:
        _caughtElements|=sim_right_button; // we catch the right button
        prepareForPopupMenu=_activePageIndex;
        return(true); // We might display a popup menu to allow the user to create a view for that _activePageIndex
    }
    return(false); // Nothing caught that action
}

void CPageContainer::rightMouseButtonUp(int x,int y,int absX,int absY,QWidget* mainWindow)
{
    mouseRelativePosition[0]=x;
    mouseRelativePosition[1]=y;


    if (App::ct->buttonBlockContainer!=NULL)
    {
        // ROUTINE DOES NOT YET EXIST!
    }
    CSPage* it=getPage(_activePageIndex);
    if (it!=NULL)
    {
        if (it->getCaughtElements()&sim_right_button)
        {
            if (it->rightMouseButtonUp(mouseRelativePosition[0],mouseRelativePosition[1],absX,absY,mainWindow))
            {
                SSimulationThreadCommand cmd;
                cmd.cmdId=REMOVE_CURRENT_PAGE_CMD;
                App::appendSimulationThreadCommand(cmd);
            }
        }
    }
    else
    {
        if (_caughtElements&sim_right_button)
        {
            if ( (prepareForPopupMenu>=0)&&(prepareForPopupMenu<_numberOfPages)&&(prepareForPopupMenu==_activePageIndex) )
            {
                // Did the mouse go up in this zone?
                if ( (x>=0)&&(y>=0)&&(x<=_pageSize[0])&&(y<=_pageSize[1]) )
                { // Yes! We display a popup menu to allow the user to create a new view at that index:
                    if (App::operationalUIParts&sim_gui_popups)
                    { // Default popups
                        VMenu mainMenu=VMenu();
                        addPageMenu(&mainMenu);
                        int command=mainMenu.trackPopupMenu();
                        processCommand(command,_activePageIndex);
                    }
                }
            }
        }
    }
    prepareForPopupMenu=-1;
}

bool CPageContainer::middleMouseButtonDown(int x,int y)
{
    if ( (x<0)||(y<0)||(x>_pageSize[0])||(y>_pageSize[1]) )
        return(false);

    // The mouse went down in this window zone
    mouseRelativePosition[0]=x;
    mouseRelativePosition[1]=y;

    if (App::ct->buttonBlockContainer!=NULL)
        App::ct->buttonBlockContainer->clearCaughtElements(0xffff-sim_middle_button);

    for (int i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
            _allPages[i]->clearCaughtElements(0xffff-sim_middle_button);
    }

    // First we check the soft dialogs:
    if (App::ct->buttonBlockContainer!=NULL)
    {
        // Following 2 lines should be replaced with the right button down handling routine (when it exists)! (and then return true!)
        if (App::ct->buttonBlockContainer->mouseDownTest(mouseRelativePosition[0],mouseRelativePosition[1],_activePageIndex))
            return(false);
    }

    setFocusObject(FOCUS_ON_PAGE);
    CSPage* it=getPage(_activePageIndex);
    if (it!=NULL)
    {
        if (it->middleMouseButtonDown(mouseRelativePosition[0],mouseRelativePosition[1]))
            return(true); // The active view caught that event!
    }
    return(false); // Nothing caught that action
}

void CPageContainer::middleMouseButtonUp(int x,int y)
{
    mouseRelativePosition[0]=x;
    mouseRelativePosition[1]=y;


    if (App::ct->buttonBlockContainer!=NULL)
    {
        // ROUTINE DOES NOT YET EXIST!
    }

    CSPage* it=getPage(_activePageIndex);
    if (it!=NULL)
    {
        if (it->getCaughtElements()&sim_middle_button)
            it->middleMouseButtonUp(mouseRelativePosition[0],mouseRelativePosition[1]);
    }
}

bool CPageContainer::leftMouseButtonDoubleClick(int x,int y,int selectionStatus)
{ // YOU ARE ONLY ALLOWED TO MODIFY SIMPLE TYPES. NO OBJECT CREATION/DESTRUCTION HERE!!
    if ( (x<0)||(y<0)||(x>_pageSize[0])||(y>_pageSize[1]) )
        return(false);
    // The mouse went down in this window zone
    mouseRelativePosition[0]=x;
    mouseRelativePosition[1]=y;
    // Soft dialogs:
    if (App::ct->buttonBlockContainer!=NULL)
    {
        if (App::ct->buttonBlockContainer->leftMouseButtonDoubleClick(x,y,_activePageIndex))
        {
            setFocusObject(FOCUS_ON_SOFT_DIALOG);
            return(true);
        }
    }
    setFocusObject(FOCUS_ON_PAGE);
    { // If dialog focus could be lost:
        CSPage* it=getPage(_activePageIndex);
        if (it!=NULL)
            return(it->leftMouseButtonDoubleClick(x,y,selectionStatus));
    }
    return(false); // Not processed
}

int CPageContainer::getCaughtElements() const
{ // YOU ARE ONLY ALLOWED TO MODIFY SIMPLE TYPES. NO OBJECT CREATION/DESTRUCTION HERE!!
    int retVal=0;
    for (int i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
            retVal|=_allPages[i]->getCaughtElements();
    }
    if (App::ct->buttonBlockContainer!=NULL)
        retVal|=App::ct->buttonBlockContainer->getCaughtElements();
    retVal|=_caughtElements;
    return(retVal);
}

void CPageContainer::clearCaughtElements(int keepMask)
{ // YOU ARE ONLY ALLOWED TO MODIFY SIMPLE TYPES. NO OBJECT CREATION/DESTRUCTION HERE!!
    for (int i=0;i<_numberOfPages;i++)
    {
        if (_allPages[i]!=NULL)
            _allPages[i]->clearCaughtElements(keepMask);
    }
    if (App::ct->buttonBlockContainer!=NULL)
        App::ct->buttonBlockContainer->clearCaughtElements(keepMask);
    _caughtElements&=keepMask;
}

#endif
