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

#include "uiEditMode.h"
#include "buttonBlockContainer.h"
#include "vMenubar.h"
#include "global.h"
#include "v_repStringTable.h"

CUiEditMode::CUiEditMode(CButtonBlockContainer* buttonBlockContainer)
{
    _buttonBlockContainer=buttonBlockContainer;
    _buttonBlockContainer->setButtonEditMode_editMode(true);
}

CUiEditMode::~CUiEditMode()
{
    _buttonBlockContainer->setButtonEditMode_editMode(false);
}

void CUiEditMode::addMenu(VMenu* menu)
{
    CButtonBlock* it=_buttonBlockContainer->getBlockWithID(_buttonBlockContainer->getBlockInEdition());
    menu->appendMenuItem(it!=NULL,false,UI_EDIT_MODE_COPY_UI_EMCMD,IDS_COPY_SELECTED_2D_ELEMENT_MENU_ITEM);
    menu->appendMenuItem(_buttonBlockContainer->getCopyOfBlock_forEditMode()!=NULL,false,UI_EDIT_MODE_PASTE_UI_EMCMD,IDS_PASTE_BUFFER_MENU_ITEM);
    menu->appendMenuItem(it!=NULL,false,UI_EDIT_MODE_DELETE_UI_EMCMD,IDS_DELETE_SELECTED_2D_ELEMENT_MENU_ITEM);
    menu->appendMenuItem(it!=NULL,false,UI_EDIT_MODE_CUT_UI_EMCMD,IDS_CUT_SELECTED_2D_ELEMENT_MENU_ITEM);

    menu->appendMenuSeparator();

    menu->appendMenuItem(true,false,ANY_EDIT_MODE_FINISH_AND_APPLY_CHANGES_EMCMD,IDS_LEAVE_2D_ELEMENT_EDIT_MODE_MENU_ITEM);
}

bool CUiEditMode::processCommand(int commandID)
{ // Return value is true if the command was successful
    bool retVal=true;

    if (commandID==UI_EDIT_MODE_COPY_UI_EMCMD)
    {
        _buttonBlockContainer->copyBlockInEdition_editMode();
        return(retVal);
    }

    if (commandID==UI_EDIT_MODE_PASTE_UI_EMCMD)
    {
        _buttonBlockContainer->pasteCopiedBlock_editMode();
        return(retVal);
    }

    if (commandID==UI_EDIT_MODE_DELETE_UI_EMCMD)
    {
        _buttonBlockContainer->deleteBlockInEdition_editMode();
        return(retVal);
    }

    if (commandID==UI_EDIT_MODE_CUT_UI_EMCMD)
    {
        int savedSel=_buttonBlockContainer->getBlockInEdition();
        _buttonBlockContainer->copyBlockInEdition_editMode();
        _buttonBlockContainer->setBlockInEdition(savedSel);
        _buttonBlockContainer->deleteBlockInEdition_editMode();
        return(retVal);
    }

    if (commandID==UI_EDIT_MODE_DELETE_SELECTED_BUTTONS_EMCMD)
    {
        retVal=_buttonBlockContainer->deleteSelectedButtons_editMode();
        return(retVal);
    }

    return(false);
}
