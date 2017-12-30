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

#include "vMenubar.h"
#include "app.h"

VMenu::VMenu()
{
    _qMenu=new QMenu(App::mainWindow);
}

VMenu::~VMenu()
{
    clear();
}

void VMenu::appendMenuAndDetach(VMenu* childMenu,bool enabled,const std::string& label)
{
    childMenu->_qMenu->setTitle(label.c_str());
    childMenu->_qMenu->setEnabled(enabled);
    _qMenu->addMenu(childMenu->_qMenu);
    _children.push_back(childMenu);
}

void VMenu::appendMenuItem(bool enabled,bool checkMark,int commandID,const std::string& label,bool showCheckmarkSpot)
{
    QAction* action=_qMenu->addAction(label.c_str());
    action->setEnabled(enabled);
    action->setCheckable(showCheckmarkSpot);
    action->setChecked(checkMark);
    App::mainWindow->connect(action,SIGNAL(triggered()),App::mainWindow->getPopupSignalMapper(),SLOT(map()));
    App::mainWindow->getPopupSignalMapper()->setMapping(action,commandID);
}

void VMenu::appendMenuSeparator()
{
    _qMenu->addSeparator();
}

int VMenu::trackPopupMenu()
{
    VMenubar::doNotExecuteCommandButMemorizeIt=true;
    _qMenu->exec(QCursor::pos());
    if (!VMenubar::doNotExecuteCommandButMemorizeIt)
        return(VMenubar::memorizedCommand);
    VMenubar::doNotExecuteCommandButMemorizeIt=false;
    return(-1);
}

QMenu* VMenu::getQMenu()
{
    return(_qMenu);
}

void VMenu::clear()
{
    for (size_t i=0;i<_children.size();i++)
        delete _children[i];
    _children.clear();

    _qMenu->clear();
}


int VMenubar::memorizedCommand=-1;
bool VMenubar::doNotExecuteCommandButMemorizeIt=false;

VMenubar::VMenubar()
{
    _qMenubar=App::mainWindow->menuBar();
    #ifdef MAC_VREP
        _qMenubar->setNativeMenuBar(false); // otherwise we have plenty of crashes!!! The result with this instruction is that the menu bar is not in the Mac top bar, but in the main window as in Windows!
    #endif
}

VMenubar::~VMenubar()
{
    clear();
}

void VMenubar::appendMenuAndDetach(VMenu* childMenu,bool enabled,const std::string& label)
{
    childMenu->getQMenu()->setTitle(label.c_str());
    childMenu->getQMenu()->setEnabled(enabled);
    _qMenubar->addMenu(childMenu->getQMenu());
    _children.push_back(childMenu);
}

void VMenubar::appendMenuItem(bool enabled,bool checkMark,int commandID,const std::string& label,bool showCheckmarkSpot)
{
    QAction* action=_qMenubar->addAction(label.c_str());
    action->setEnabled(enabled);
    action->setCheckable(showCheckmarkSpot);
    action->setChecked(checkMark);
    App::mainWindow->connect(action,SIGNAL(triggered()),App::mainWindow->getPopupSignalMapper(),SLOT(map()));
    App::mainWindow->getPopupSignalMapper()->setMapping(action,commandID);
}

void VMenubar::appendMenuSeparator()
{
    _qMenubar->addSeparator();
}

void VMenubar::clear()
{
    for (size_t i=0;i<_children.size();i++)
        delete _children[i];
    _children.clear();

    _qMenubar->clear();
}

