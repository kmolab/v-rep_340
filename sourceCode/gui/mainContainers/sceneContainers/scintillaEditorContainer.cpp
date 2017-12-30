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
#include "scintillaEditorContainer.h"
#include "v_rep_internal.h"


#include "vVarious.h"
#include "app.h"

CScintillaEditorContainer::CScintillaEditorContainer()
{

}

CScintillaEditorContainer::~CScintillaEditorContainer()
{
    closeAllEditors();
}

void CScintillaEditorContainer::executeCommand(SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut)
{
    if (cmdIn->cmdId==SCINTILLA_EDITOR_OPEN_FOR_SCRIPT_SEUITHREADCMD)
        cmdOut->boolParams.push_back(openEditorForScript(cmdIn->intParams[0]));
    if (cmdIn->cmdId==SCINTILLA_EDITOR_CLOSE_ALL_SEUITHREADCMD)
        closeAllEditors();
    if (cmdIn->cmdId==SCINTILLA_EDITOR_CLOSE_OR_SHOW_ALL_SEUITHREADCMD)
        showOrHideAllEditors(cmdIn->boolParams[0]);
    if (cmdIn->cmdId==SCINTILLA_EDITOR_CLOSE_SEUITHREADCMD)
        cmdOut->boolParams.push_back(closeEditor(cmdIn->intParams[0]));
    if (cmdIn->cmdId==SCINTILLA_EDITOR_UPDATE_SEUITHREADCMD)
        updateWindowsExceptContentText();
    if (cmdIn->cmdId==SCINTILLA_EDITOR_APPLY_CHANGES_SEUITHREADCMD)
        applyChanges(cmdIn->boolParams[0]);
}

bool CScintillaEditorContainer::openEditorForScript(int scriptID)
{
    if (VThread::isCurrentThreadTheUiThread())
    { // we are in the UI thread. We execute the command now:
        if (App::userSettings->useBuiltInScriptEditor())
        {
            for (size_t i=0;i<_allEditors.size();i++)
            {
                if (_allEditors[i]->getScriptID()==scriptID)
                    return(true); // already open!
            }
        }
        CScintillaEditor* editor=new CScintillaEditor();
        if (editor->initialize(scriptID))
        {
            if (App::userSettings->useBuiltInScriptEditor())
                _allEditors.push_back(editor);
            else
            {
                delete editor;
                VVarious::executeExternalApplication(App::userSettings->externalScriptEditor,App::ct->luaScriptContainer->getScriptFromID(scriptID)->getFilenameForExternalScriptEditor().c_str(),App::directories->executableDirectory,VVARIOUS_SHOWNORMAL); // executable directory needed because otherwise the shellExecute command might switch directories!
            }
            return(true);
        }
        delete editor;
        return(false);
    }
    else
    { // We are NOT in the UI thread. We execute the command via the UI thread:
        SUIThreadCommand cmdIn;
        SUIThreadCommand cmdOut;
        cmdIn.cmdId=SCINTILLA_EDITOR_OPEN_FOR_SCRIPT_SEUITHREADCMD;
        cmdIn.intParams.push_back(scriptID);
        App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
        return(cmdOut.boolParams[0]);
    }
}

void CScintillaEditorContainer::closeAllEditors()
{
    if (VThread::isCurrentThreadTheUiThread())
    { // we are in the UI thread. We execute the command now:
        for (int i=0;i<int(_allEditors.size());i++)
            delete _allEditors[i];
        _allEditors.clear();
    }
    else
    { // We are NOT in the UI thread. We execute the command via the UI thread:
        SUIThreadCommand cmdIn;
        SUIThreadCommand cmdOut;
        cmdIn.cmdId=SCINTILLA_EDITOR_CLOSE_ALL_SEUITHREADCMD;
        App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
    }
}

bool CScintillaEditorContainer::closeEditor(int scriptID)
{ // false means the script was not opened nor hidden
    if (VThread::isCurrentThreadTheUiThread())
    { // we are in the UI thread. We execute the command now:
        for (int i=0;i<int(_allEditors.size());i++)
        {
            if (_allEditors[i]->getScriptID()==scriptID)
            {
                CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptID);
                if (it!=NULL)
                {
                    int posAndSize[4];
                    _allEditors[i]->getWindowPosAndSize(posAndSize);
                    it->setPreviousEditionWindowPosAndSize(posAndSize);
                }
                delete _allEditors[i];
                _allEditors.erase(_allEditors.begin()+i);
                return(true);
            }
        }
        return(false);
    }
    else
    { // We are NOT in the UI thread. We execute the command via the UI thread:
        SUIThreadCommand cmdIn;
        SUIThreadCommand cmdOut;
        cmdIn.cmdId=SCINTILLA_EDITOR_CLOSE_SEUITHREADCMD;
        cmdIn.intParams.push_back(scriptID);
        App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
        return(cmdOut.boolParams[0]);
    }
}

void CScintillaEditorContainer::requestClosing(void* dialogPointer)
{
    for (int i=0;i<int(_allEditors.size());i++)
        _allEditors[i]->requestClosing(dialogPointer);
}

void CScintillaEditorContainer::applyChanges(bool getFoldingInfo)
{
    if (VThread::isCurrentThreadTheUiThread())
    { // we are in the UI thread. We execute the command now:
        if (App::userSettings->useBuiltInScriptEditor())
        {
            for (int i=0;i<int(_allEditors.size());i++)
                _allEditors[i]->applyChanges(getFoldingInfo);
        }
        else
        {
            for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
                App::ct->luaScriptContainer->allScripts[i]->fromFileToBuffer();
        }
    }
    else
    { // We are NOT in the UI thread. We execute the command via the UI thread:
        SUIThreadCommand cmdIn;
        SUIThreadCommand cmdOut;
        cmdIn.cmdId=SCINTILLA_EDITOR_APPLY_CHANGES_SEUITHREADCMD;
        cmdIn.boolParams.push_back(getFoldingInfo);
        App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
    }
}

bool CScintillaEditorContainer::areWindowsOpen()
{
    return(_allEditors.size()!=0);
}

void CScintillaEditorContainer::showOrHideAllEditors(bool show)
{
    if (VThread::isCurrentThreadTheUiThread())
    { // we are in the UI thread. We execute the command now:
        for (int i=0;i<int(_allEditors.size());i++)
        {
            if (show)
                _allEditors[i]->showEditorButDontOpenIt();
            else
                _allEditors[i]->hideEditorButDontCloseIt();
        }
    }
    else
    { // We are NOT in the UI thread. We execute the command via the UI thread:
        SUIThreadCommand cmdIn;
        SUIThreadCommand cmdOut;
        cmdIn.cmdId=SCINTILLA_EDITOR_CLOSE_OR_SHOW_ALL_SEUITHREADCMD;
        cmdIn.boolParams.push_back(show);
        App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
    }
}

void CScintillaEditorContainer::updateWindowsExceptContentText()
{
    if (VThread::isCurrentThreadTheUiThread())
    { // we are in the UI thread. We execute the command now:
        for (int i=0;i<int(_allEditors.size());i++)
        {
            if (_allEditors[i]->updateWindowExceptContentText())
            {
                closeEditor(_allEditors[i]->getScriptID());
                i--; // We need to reprocess this position
            }
        }
    }
    else
    { // We are NOT in the UI thread. We execute the command via the UI thread:
        SUIThreadCommand cmdIn;
        SUIThreadCommand cmdOut;
        cmdIn.cmdId=SCINTILLA_EDITOR_UPDATE_SEUITHREADCMD;
        App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
    }
}
