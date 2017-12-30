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
#include "scintillaEditor.h"
#include "scintillaDlg.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscistyle.h>

#include "vMessageBox.h"
#include "v_rep_internal.h"
#include "v_repStrings.h"
#include "app.h"

CScintillaEditor::CScintillaEditor()
{
    _dlg=NULL;
    _scriptID=-1;
    _closeRequest=false;
    _currentWindowTitle="";
    _hideLevel=0;
}

CScintillaEditor::~CScintillaEditor() 
{
    _closeDlgWindow(true);
}

int CScintillaEditor::getScriptID()
{
    return(_scriptID);
}

void CScintillaEditor::showEditorButDontOpenIt()
{
    if (_hideLevel>0)
        _hideLevel--;
    if (_hideLevel==0)
    {
        if (_dlg!=NULL)
            _dlg->setShowWindow(true);
    }
}

void CScintillaEditor::hideEditorButDontCloseIt()
{
    _hideLevel++;
    if (_hideLevel==1)
    {
        if (_dlg!=NULL)
            _dlg->setShowWindow(false);
    }
}


bool CScintillaEditor::initialize(int scriptID)
{
    if ((App::operationalUIParts&sim_gui_scripteditor)==0)
        return(false);
    // Make sure that scripts cannot be viewed/edited with locked scenes:
    if (App::ct->environment->getSceneLocked())
    {
        App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Script"),strTranslate(IDS_SCENE_IS_LOCKED_WARNING_NO_SCRIPT_VIEWING),VMESSAGEBOX_OKELI);
        return(false);
    }

    if (App::userSettings->useBuiltInScriptEditor())
    {
        _dlg=new CScintillaDlg(App::mainWindow);
        if (_dlg==NULL)
            return(false);
    }
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptID);
    if (it==NULL)
    {
        if (App::userSettings->useBuiltInScriptEditor())
            _closeDlgWindow(true);
        return(false);
    }
    if (App::userSettings->useBuiltInScriptEditor())
        _dlg->setScriptID(scriptID);
    if (it->getScriptType()==sim_scripttype_mainscript)
    { 
        if (it->isDefaultMainScript())
        { // Display warning
            if (VMESSAGEBOX_REPLY_YES!=App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Main script"),strTranslate(IDS_MAINSCRIPT_EDITION_WARNING),VMESSAGEBOX_YES_NO))
            {
                if (App::userSettings->useBuiltInScriptEditor())
                    _closeDlgWindow(false);
                return(false);
            }
            // Have have to make the main script a default main script!
            it->setCustomizedMainScript(true);
            POST_SCENE_CHANGED_ANNOUNCEMENT(""); // **************** UNDO THINGY ****************
        }
    }
    if (App::userSettings->useBuiltInScriptEditor())
    {
        int posAndSize[4];
        it->getPreviousEditionWindowPosAndSize(posAndSize);
        _dlg->initPhase1(posAndSize);
        _scriptID=scriptID;
        updateWindowExceptContentText();
        if (it->getScriptType()==sim_scripttype_mainscript)
            _dlg->setColorsAndMainStyles(0);
        if (it->getScriptType()==sim_scripttype_childscript)
        {
            if (it->getThreadedExecution())
                _dlg->setColorsAndMainStyles(2);
            else
                _dlg->setColorsAndMainStyles(1);
        }
        if (it->getScriptType()==sim_scripttype_jointctrlcallback)
            _dlg->setColorsAndMainStyles(3);
        if (it->getScriptType()==sim_scripttype_generalcallback)
            _dlg->setColorsAndMainStyles(5);
        std::vector<int> foldingThing;
        _dlg->initPhase2(it->getScriptText(&foldingThing),&foldingThing);
    }
    return(true);
}

void CScintillaEditor::getWindowPosAndSize(int posAndSize[4])
{
    FUNCTION_DEBUG;
    _dlg->getWindowPosAndSize(posAndSize);
}

bool CScintillaEditor::updateWindowExceptContentText()
{ // return value means true means we need to close the editor
    FUNCTION_DEBUG;
    if (_dlg==NULL)
        return(true);
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(_scriptID);
    if ( (it==NULL)||_closeRequest )
        return(true);

    std::string tmp(it->getDescriptiveName());

    if (tmp.compare(_currentWindowTitle)!=0)
    {
        _dlg->setWindowTitleText(tmp);
        _currentWindowTitle=tmp;
    }
    return(false);
}

void CScintillaEditor::newSceneProcedure()
{

}

void CScintillaEditor::requestClosing(void* dialogPointer)
{
    FUNCTION_DEBUG;
    if (dialogPointer==(void*)_dlg)
        _closeRequest=true;
}

void CScintillaEditor::_closeDlgWindow(bool announceChange)
{ // announceChange is true by default!
    if (_dlg==NULL)
        return;
    bool changed=applyChanges(true);
    delete _dlg;
    _dlg=NULL;
    _scriptID=-1;
    _currentWindowTitle="";
    if (announceChange&&changed)
    {
        POST_SCENE_CHANGED_ANNOUNCEMENT(""); // ************************** UNDO thingy **************************
    }
}

bool CScintillaEditor::applyChanges(bool getFoldingInfo)
{
    FUNCTION_DEBUG;
    if (_dlg==NULL)
        return(false);
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(_scriptID);
    if (it==NULL)
        return(false);
    int l=_dlg->_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETLENGTH);
    char* buff=new char[l+1];
    _dlg->_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETTEXT,(unsigned long)l+1,buff);

    bool changed=false;
    char* originalBuff=it->getScriptText(NULL);
    int originalL=it->getScriptTextLength();
    if (originalL==l+1)
    {
        for (int i=0;i<l;i++)
        {
            if (originalBuff[i]!=buff[i])
            {
                changed=true;
                break;
            }
        }
    }
    else
        changed=true;

    if (getFoldingInfo)
    {
        std::vector<int> foldingInfo(_dlg->getFoldingInfo());
        it->setScriptText(buff,&foldingInfo);
    }
    else
        it->setScriptText(buff,NULL);
    delete[] buff;
    return(changed);
}
