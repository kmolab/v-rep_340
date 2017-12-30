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

#include "vMessageBox.h"
#include <QMessageBox>

enum {
        VMESSAGEBOX_APP_MODAL       =8,
        VMESSAGEBOX_INFO_TYPE       =16,
        VMESSAGEBOX_QUESTION_TYPE   =32,
        VMESSAGEBOX_WARNING_TYPE    =64,
        VMESSAGEBOX_CRITICAL_TYPE   =128,
};

unsigned short VMessageBox::informationSystemModal(QWidget* parent,const std::string& title,const std::string& message,unsigned short flags)
{ // Don't forget: parent can be NULL at application start-up!
    flags|=VMESSAGEBOX_INFO_TYPE;
    return(_displayBox(parent,title,message,flags));
}

unsigned short VMessageBox::information(QWidget* parent,const std::string& title,const std::string& message,unsigned short flags)
{ // Don't forget: parent can be NULL at application start-up!
    flags|=VMESSAGEBOX_APP_MODAL|VMESSAGEBOX_INFO_TYPE;
    return(_displayBox(parent,title,message,flags));
}

unsigned short VMessageBox::question(QWidget* parent,const std::string& title,const std::string& message,unsigned short flags)
{ // Don't forget: parent can be NULL at application start-up!
    flags|=VMESSAGEBOX_APP_MODAL|VMESSAGEBOX_QUESTION_TYPE;
    return(_displayBox(parent,title,message,flags));
}

unsigned short VMessageBox::warning(QWidget* parent,const std::string& title,const std::string& message,unsigned short flags)
{ // Don't forget: parent can be NULL at application start-up!
    flags|=VMESSAGEBOX_APP_MODAL|VMESSAGEBOX_WARNING_TYPE;
    return(_displayBox(parent,title,message,flags));
}

unsigned short VMessageBox::critical(QWidget* parent,const std::string& title,const std::string& message,unsigned short flags)
{ // Don't forget: parent can be NULL at application start-up!
    flags|=VMESSAGEBOX_APP_MODAL|VMESSAGEBOX_CRITICAL_TYPE;
    return(_displayBox(parent,title,message,flags));
}

unsigned short VMessageBox::_displayBox(QWidget* parent,const std::string& title,const std::string& message,unsigned short flags)
{ // Don't forget: parent can be NULL at application start-up!
    unsigned short lower=(flags&7);
    unsigned short upper=flags-lower;

    QMessageBox msg(parent);
    msg.setWindowTitle(title.c_str());
    msg.setText(message.c_str());
    if (lower==VMESSAGEBOX_OKELI)
        msg.addButton(QMessageBox::Ok);
    if (lower==VMESSAGEBOX_YES_NO)
    {
        msg.addButton(QMessageBox::Yes);
        msg.addButton(QMessageBox::No);
    }
    if (lower==VMESSAGEBOX_YES_NO_CANCEL)
    {
        msg.addButton(QMessageBox::Yes);
        msg.addButton(QMessageBox::No);
        msg.addButton(QMessageBox::Cancel);
    }
    if (lower==VMESSAGEBOX_OK_CANCEL)
    {
        msg.addButton(QMessageBox::Ok);
        msg.addButton(QMessageBox::Cancel);
    }
    if (upper&VMESSAGEBOX_INFO_TYPE)
        msg.setIcon(QMessageBox::Information);
    if (upper&VMESSAGEBOX_QUESTION_TYPE)
        msg.setIcon(QMessageBox::Question);
    if (upper&VMESSAGEBOX_WARNING_TYPE)
        msg.setIcon(QMessageBox::Warning);
    if (upper&VMESSAGEBOX_CRITICAL_TYPE)
        msg.setIcon(QMessageBox::Critical);

// Following doesn't always work correctly (at least on Qt: the dialog sometimes simply doesn't display!!!!!):
//  if (alwaysOnTop)
//      msg.setWindowFlags(Qt::WindowStaysOnTopHint);

    int result=msg.exec();

    if (result==QMessageBox::Cancel)
        return(VMESSAGEBOX_REPLY_CANCEL);
    if (result==QMessageBox::No)
        return(VMESSAGEBOX_REPLY_NO);
    if (result==QMessageBox::Yes)
        return(VMESSAGEBOX_REPLY_YES);
    if (result==QMessageBox::Ok)
        return(VMESSAGEBOX_REPLY_OK);
    return(VMESSAGEBOX_REPLY_CANCEL); // Can happen when the close button was clicked!
}
