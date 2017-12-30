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
#include "qdlgabout.h"
#include "ui_qdlgabout.h"
#include "ttUtil.h"
#include "tt.h"
#include "v_repStrings.h"
#include "v_repConst.h"
#include "ser.h"
#include "app.h"
#include "pluginContainer.h"

CQDlgAbout::CQDlgAbout(QWidget *parent) :
    VDialog(parent,QT_MODAL_DLG_STYLE|Qt::CustomizeWindowHint|Qt::WindowTitleHint), // since Qt5.1: Tool --> Dialog
    ui(new Ui::CQDlgAbout)
{
    ui->setupUi(this);
    initializationEvent();
}

CQDlgAbout::~CQDlgAbout()
{
    delete ui;
}

void CQDlgAbout::cancelEvent()
{
    defaultModalDialogEndRoutine(false);
}

void CQDlgAbout::okEvent()
{
    defaultModalDialogEndRoutine(true);
}

void CQDlgAbout::initializationEvent()
{
    defaultDialogInitializationRoutine();
    QPixmap img;

    if (CPluginContainer::isMeshPluginAvailable())
        img.load(":/imageFiles_vrep/splashEdu.png"); // you may change this splash screen only if you use the commercial version of the plugins!
    else
        img.load(":/imageFiles_vrep/splashBasic.png");


    ui->splashImage->setPixmap(img);

#ifdef QT_VREP_64BIT
    std::string bits(" 64bit");
#else
    std::string bits(" 32bit");
#endif





    std::string txt;
    setWindowTitle(IDS_ABOUT_V_REP);
    txt=CTTUtil::getFormattedString("V-REP custom compilation, Version ",VREP_PROGRAM_VERSION," ",VREP_PROGRAM_REVISION," ",bits.c_str()," (serialization version ");
    txt+=CTTUtil::dwordToString(CSer::SER_SERIALIZATION_VERSION);
    txt+=")";
    txt+=CTTUtil::getFormattedString("\n(Qt Version ",QT_VERSION_STR,")");
    txt+="\n";
    txt+=CTTUtil::getFormattedString("\nBuilt ",__DATE__);
    txt+="\nCopyright (c) 2006-";
    std::string date(__DATE__);
    for (int i=0;i<4;i++)
        txt+=date[date.length()-4+i];
    txt+=CTTUtil::getFormattedString(" ",VREP_COMPANY_NAME_TXT,"\n",VREP_WEBPAGE_ADDRESS_TXT);


    ui->info->setText(txt.c_str());
}

void CQDlgAbout::on_qqClose_clicked(QAbstractButton *button)
{
    defaultModalDialogEndRoutine(true);
}
