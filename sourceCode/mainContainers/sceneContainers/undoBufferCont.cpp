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
#include "funcDebug.h"
#include "v_rep_internal.h"
#include "undoBufferCont.h"
#include "pluginContainer.h"
#include "vDateTime.h"
#include "app.h"
#include "v_repStrings.h"
//#include "vMessageBox.h"

CUndoBufferCont::CUndoBufferCont()
{
    _commonInit();
}

CUndoBufferCont::~CUndoBufferCont()
{
    for (int i=0;i<int(_buffers.size());i++)
        delete _buffers[i];
    undoBufferArrays.clearAll();
}

int CUndoBufferCont::getNextBufferId()
{
    return(_nextBufferId);
}

void CUndoBufferCont::emptySceneProcedure()
{
    _commonInit();
}

void CUndoBufferCont::simulationAboutToStart()
{
}

void CUndoBufferCont::simulationEnded()
{
}

void CUndoBufferCont::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{
}

void CUndoBufferCont::_commonInit()
{
    _currentStateIndex=-1;
    for (int i=0;i<int(_buffers.size());i++)
        delete _buffers[i];
    _buffers.clear();
    undoBufferArrays.clearAll();
    _announceChangeStartCalled=false;
    _announceChangeGradualCalledTime=-1;
    _sceneSaveMightBeNeeded=false;
    _undoPointSavingOrRestoringUnderWay=false;
    _inUndoRoutineNow=false;
    _nextBufferId=0;
}

bool CUndoBufferCont::isUndoSavingOrRestoringUnderWay()
{
    FUNCTION_DEBUG;
    return(_undoPointSavingOrRestoringUnderWay);
}

bool CUndoBufferCont::isSceneSaveMaybeNeededFlagSet()
{
    return(_sceneSaveMightBeNeeded);
}

void CUndoBufferCont::clearSceneSaveMaybeNeededFlag()
{
    _sceneSaveMightBeNeeded=false;
}

bool CUndoBufferCont::announceChange()
{
    FUNCTION_DEBUG;
    if (!_inUndoRoutineNow)
    {
        bool retVal=memorizeState();
        _announceChangeGradualCalledTime=-1;
        return(retVal);
    }
    return(false);
}

void CUndoBufferCont::announceChangeStart()
{
    FUNCTION_DEBUG;
    if (!_inUndoRoutineNow)
        _announceChangeStartCalled=true;
}

void CUndoBufferCont::announceChangeEnd()
{
    FUNCTION_DEBUG;
    if (!_inUndoRoutineNow)
    {
        if (_announceChangeStartCalled)
        {
            memorizeState();
            _announceChangeGradualCalledTime=-1;
        }
        _announceChangeStartCalled=false;
    }
}

void CUndoBufferCont::announceChangeGradual()
{
    FUNCTION_DEBUG;
    if (!_inUndoRoutineNow)
    {
        if (_announceChangeGradualCalledTime==-1)
            _announceChangeGradualCalledTime=VDateTime::getTimeInMs();
    }
}

void CUndoBufferCont::memorizeStateIfNeeded()
{
    FUNCTION_DEBUG;
    if (!_inUndoRoutineNow)
    {
        if (_announceChangeGradualCalledTime==-1)
            return;
        if (VDateTime::getTimeDiffInMs(_announceChangeGradualCalledTime)>5000)
        {
            memorizeState();
            _announceChangeGradualCalledTime=-1;
        }
    }
}

void CUndoBufferCont::emptyRedoBuffer()
{
    FUNCTION_DEBUG;
    while (int(_buffers.size())>_currentStateIndex+1)
    {
        undoBufferArrays.removeDependenciesFromUndoBufferId(_buffers[_buffers.size()-1]->getBufferId());
        delete _buffers[_buffers.size()-1];
        _buffers.pop_back();
        App::setToolbarRefreshFlag();
    }
}

bool CUndoBufferCont::memorizeState()
{
    FUNCTION_DEBUG;
#ifdef SIM_WITH_GUI
    if (_inUndoRoutineNow)
        return(false);
    if (!App::ct->simulation->isSimulationStopped())
        return(false);
    if (App::mainWindow==NULL)
        return(false); // we are in headless mode

    // If we arrived here, we might have something to save:

    if (!_isGoodToMemorizeUndoOrRedo())
    { // we cannot memorize anything now...
        _sceneSaveMightBeNeeded=true; // actually we don't know if this is really the case, but since we are not authorized to check right now, this is safer!
        return(false);
    }

    bool retVal=true; // means the scene changed.. we modify this variable accordingly later down

    static int tooLongExecutionCount=0;
    int startTime=VDateTime::getTimeInMs();

    if ( (App::mainWindow!=NULL)&&(App::mainWindow->scintillaEditorContainer!=NULL) )
        App::mainWindow->scintillaEditorContainer->applyChanges(false);

    std::vector<char> newBuff;
    CSer serObj(newBuff);
    serObj.disableCountingModeExceptForExceptions();

    serObj.writeOpen();
    _undoPointSavingOrRestoringUnderWay=true;
    CUndoBufferCameras* cameraBuffers=new CUndoBufferCameras();
    cameraBuffers->storeCameras();
    App::ct->objCont->saveScene(serObj); // This takes the 90% of time of the whole routine
    cameraBuffers->restoreCameras();
    _undoPointSavingOrRestoringUnderWay=false;
    serObj.writeClose(false); // We don't wanna compression

    CUndoBuffer* it=new CUndoBuffer(newBuff,_nextBufferId++,cameraBuffers);
    if (_currentStateIndex==-1)
    { // first buffer, we just add it
        _buffers.push_back(it);
        _currentStateIndex=0;
    }
    else
    { // We check with previous buffer:
        std::vector<char> prevBuff;
        _getFullBuffer(_currentStateIndex,prevBuff);
        if (it->finalize(prevBuff))
        { // different from previous, we remove forward buffers and add this one:
            while (int(_buffers.size())>_currentStateIndex+1)
            {
                undoBufferArrays.removeDependenciesFromUndoBufferId(_buffers[_buffers.size()-1]->getBufferId());
                delete _buffers[_buffers.size()-1];
                _buffers.pop_back();
            }
            _buffers.push_back(it);
            _currentStateIndex++;
        }
        else
        {
            undoBufferArrays.removeDependenciesFromUndoBufferId(it->getBufferId());
            delete it; // same as previous, we delete it
            retVal=false;
        }
    }

    std::vector<char> fullBuff;
    while ( (_getUsedMemory()+undoBufferArrays.getMemorySizeInBytes()>App::userSettings->undoRedoMaxBufferSize)||(int(_buffers.size())>App::userSettings->undoRedoLevelCount) )
    { // We have to remove a few states at the beginning
        if (int(_buffers.size())<3)
            break; // at least 3 states!
        _getFullBuffer(1,fullBuff);
        CUndoBuffer* it=_buffers[1];
        it->updateWithFullBuffer(fullBuff);
        undoBufferArrays.removeDependenciesFromUndoBufferId(_buffers[0]->getBufferId());
        delete _buffers[0];
        _buffers.erase(_buffers.begin());
        _currentStateIndex--;
    }
    if (VDateTime::getTimeDiffInMs(startTime)>App::userSettings->timeInMsForUndoRedoTooLongWarning)
        tooLongExecutionCount++;
    else
        tooLongExecutionCount=0;

    if (tooLongExecutionCount>=5)
    {
        static bool displayedMessage=false;
        if (!displayedMessage)
        {
            if (VMESSAGEBOX_REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Undo / redo"),strTranslate(IDS_INFO_UNDO_REDO_TAKES_TOO_LONG),VMESSAGEBOX_YES_NO))
                App::userSettings->setUndoRedoEnabled(false);
            displayedMessage=true;
        }
        tooLongExecutionCount=0;
    }

    App::setToolbarRefreshFlag();

    _sceneSaveMightBeNeeded=_sceneSaveMightBeNeeded||retVal;

    return(retVal);
#else
    return(false);
#endif
}

bool CUndoBufferCont::_isGoodToMemorizeUndoOrRedo()
{
    FUNCTION_DEBUG;
#ifdef SIM_WITH_GUI
    if (!App::ct->simulation->isSimulationStopped())
        return(false);
    if (App::mainWindow==NULL)
        return(false);
    if (App::getEditModeType()!=NO_EDIT_MODE)
        return(false);
    if (App::mainWindow->oglSurface->isPageSelectionActive())
        return(false);
    if (App::mainWindow->oglSurface->isViewSelectionActive())
        return(false);
    if (App::mainWindow->oglSurface->isSceneSelectionActive())
        return(false);
    if (!App::userSettings->getUndoRedoEnabled())
        return(false);
    return(true);
#else
    return(false);
#endif
}

bool CUndoBufferCont::canUndo()
{
    FUNCTION_DEBUG;
#ifdef SIM_WITH_GUI
    if (App::mainWindow==NULL)
        return(false); // we are in headless mode
    if (!_isGoodToMemorizeUndoOrRedo())
        return(false);
    return(_currentStateIndex>0);
#else
    return(false);
#endif
}

bool CUndoBufferCont::canRedo()
{
    FUNCTION_DEBUG;
#ifdef SIM_WITH_GUI
    if (App::mainWindow==NULL)
        return(false); // we are in headless mode
    if (!_isGoodToMemorizeUndoOrRedo())
        return(false);
    return(_currentStateIndex<=(int(_buffers.size())-2));
#else
    return(false);
#endif
}

void CUndoBufferCont::undo()
{
    FUNCTION_DEBUG;
#ifdef SIM_WITH_GUI
    if (_inUndoRoutineNow)
        return;
    if (App::mainWindow==NULL)
        return; // we are in headless mode
    if (App::mainWindow->scintillaEditorContainer->areWindowsOpen())
    {
        App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_UNDO_REDO),strTranslate(IDS_UNDO_REDO_WITH_OPEN_SCRIPT_EDITOR_MESSAGE),VMESSAGEBOX_OKELI);
        return;
    }

    _inUndoRoutineNow=true;
    // 1. We memorize this position: NOOOOOOO!!! Can cause subtle errors!! 
    if (_announceChangeGradualCalledTime!=-1)
    {
        //  memorizeState();
        _announceChangeGradualCalledTime=-1;
    }
    if (_currentStateIndex<1)
    {
        _inUndoRoutineNow=false;
        return; // nothing to undo
    }

    // 2. We go back:
    _currentStateIndex--;
    std::vector<char> theBuff;
    CUndoBufferCameras* cameraBuffers=_getFullBuffer(_currentStateIndex,theBuff);
    // 3. Load the buffer:

    void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_abouttoundo,NULL,NULL,NULL);
    delete[] (char*)returnVal;

    _rememberSelectionState();
    cameraBuffers->preRestoreCameras();

    App::ct->objCont->deselectObjects();
    App::ct->simulation->stopSimulation(); // should be anyway stopped!
    App::ct->emptyScene(false);

    CSer serObj(theBuff);
    int serializationVersion;
    unsigned short dum0;
    int dum1;
    char dum2;
    serObj.readOpen(serializationVersion,dum0,dum1,dum2);
    _undoPointSavingOrRestoringUnderWay=true;

    App::ct->objCont->loadScene(serObj,true);
    cameraBuffers->restoreCameras();

    _undoPointSavingOrRestoringUnderWay=false;
    serObj.readClose();
    if (App::mainWindow!=NULL)
        App::mainWindow->refreshDimensions(); // this is important so that the new pages and views are set to the correct dimensions

    // 4. We select previously selected objects:
    _restoreSelectionState();

    _inUndoRoutineNow=false;
    
    returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_undoperformed,NULL,NULL,NULL);
    delete[] (char*)returnVal;
    App::ct->setModificationFlag(16); // undo called

    // 5. Dialog refresh:
    App::setFullDialogRefreshFlag();
    App::setToolbarRefreshFlag();
#endif
}

void CUndoBufferCont::redo()
{
    FUNCTION_DEBUG;
#ifdef SIM_WITH_GUI
    if (_currentStateIndex>(int(_buffers.size())-2))
        return; // nothing to redo
    if (App::mainWindow==NULL)
        return; // we are in headless mode
    if (App::mainWindow->scintillaEditorContainer->areWindowsOpen())
    {
        App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_UNDO_REDO),strTranslate(IDS_UNDO_REDO_WITH_OPEN_SCRIPT_EDITOR_MESSAGE),VMESSAGEBOX_OKELI);
        return;
    }

    // 2. We go forward:
    _currentStateIndex++;
    std::vector<char> theBuff;
    CUndoBufferCameras* cameraBuffers=_getFullBuffer(_currentStateIndex,theBuff);

    // 3. Load the buffer:

    void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_abouttoredo,NULL,NULL,NULL);
    delete[] (char*)returnVal;

    _rememberSelectionState();
    cameraBuffers->preRestoreCameras();

    App::ct->objCont->deselectObjects();
    App::ct->simulation->stopSimulation(); // should be anyway stopped!
    App::ct->emptyScene(false);

    CSer serObj(theBuff);
    int serializationVersion;
    unsigned short dum0;
    int dum1;
    char dum2;
    serObj.readOpen(serializationVersion,dum0,dum1,dum2);
    _undoPointSavingOrRestoringUnderWay=true;

    App::ct->objCont->loadScene(serObj,true);
    cameraBuffers->restoreCameras();

    _undoPointSavingOrRestoringUnderWay=false;
    serObj.readClose();
    if (App::mainWindow!=NULL)
        App::mainWindow->refreshDimensions(); // this is important so that the new pages and views are set to the correct dimensions


    // 4. We select previously selected objects:
    _restoreSelectionState();

    returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_redoperformed,NULL,NULL,NULL);
    delete[] (char*)returnVal;
    App::ct->setModificationFlag(32); // redo called

    // 5. Dialog refresh:
    App::setFullDialogRefreshFlag();
    App::setToolbarRefreshFlag();
#endif
}

CUndoBufferCameras* CUndoBufferCont::_getFullBuffer(int index,std::vector<char>& fullBuff)
{
    FUNCTION_DEBUG;
    if ( (index>=int(_buffers.size()))||(index<0) )
    {
        fullBuff.clear();
        return(NULL);
    }
    CUndoBufferCameras* retVal=NULL;
    if (index==0)
    { // the first one
        retVal=_buffers[index]->getCameraBuffers();
        _buffers[index]->getRestored(NULL,fullBuff);
    }
    else
    {
        std::vector<char> prevBuff;
        _getFullBuffer(index-1,prevBuff);
        retVal=_buffers[index]->getCameraBuffers();
        _buffers[index]->getRestored(&prevBuff,fullBuff);
    }
    return(retVal);
}

int CUndoBufferCont::_getUsedMemory()
{
    FUNCTION_DEBUG;
    int retVal=0;
    for (int i=0;i<int(_buffers.size());i++)
        retVal+=int(_buffers[i]->buffer.size());
    return (retVal);
}

void CUndoBufferCont::_rememberSelectionState()
{
    FUNCTION_DEBUG;
    _selectionState.clear();
    std::vector<C3DObject*> sel;
    App::ct->objCont->getSelectedObjects(sel);
    for (int i=0;i<int(sel.size());i++)
        _selectionState.push_back(sel[i]->getName());
}

void CUndoBufferCont::_restoreSelectionState()
{
    FUNCTION_DEBUG;
    for (int i=0;i<int(_selectionState.size());i++)
    {
        C3DObject* obj=App::ct->objCont->getObject(_selectionState[i]);
        if (obj!=NULL)
            App::ct->objCont->addObjectToSelection(obj->getID());
    }
    _selectionState.clear();
}

