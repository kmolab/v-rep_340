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
#include "qdlgtranslationrotationcont.h"
#include "ui_qdlgtranslationrotationcont.h"
#include "app.h"
#include "toolBarCommand.h"
#include "qdlgtranslation.h"
#include "qdlgrotation.h"

CQDlgTranslationRotationCont::CQDlgTranslationRotationCont(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgTranslationRotationCont)
{
    _dlgType=TRANSLATION_ROTATION_DLG;
    ui->setupUi(this);
    pageDlgs[0]=new CQDlgTranslation();
    originalHeights[0]=pageDlgs[0]->size().height();
    pageDlgs[1]=new CQDlgRotation();
    originalHeights[1]=pageDlgs[1]->size().height();

    currentPage=0;
    desiredPage=0;
    bl=new QVBoxLayout();
    bl->setContentsMargins(0,0,0,0);
    setLayout(bl);
    bl->addWidget(pageDlgs[0]);
    bl->addWidget(pageDlgs[1]);
    pageDlgs[1]->setVisible(false);

    QSize s(pageDlgs[currentPage]->size());
    s.setHeight(originalHeights[currentPage]);
    setFixedSize(s);
}

CQDlgTranslationRotationCont::~CQDlgTranslationRotationCont()
{
    delete ui;
}

void CQDlgTranslationRotationCont::cancelEvent()
{
    App::setMouseMode((App::getMouseMode()&0xff00)|sim_navigation_camerashift);
//  defaultModalDialogEndRoutine(false);
}

void CQDlgTranslationRotationCont::okEvent()
{
//  defaultModalDialogEndRoutine(true);
}

void CQDlgTranslationRotationCont::refresh()
{
    if ((App::getMouseMode()&0x0f)==sim_navigation_objectshift)
        desiredPage=0;
    if ((App::getMouseMode()&0x0f)==sim_navigation_objectrotate)
        desiredPage=1;

    if (desiredPage==0)
        setWindowTitle("Object/Item Translation/Position");
    else
        setWindowTitle("Object/Item Rotation/Orientation");

    if (desiredPage!=currentPage)
    {
        pageDlgs[currentPage]->setVisible(false);
        currentPage=desiredPage;
        pageDlgs[currentPage]->setVisible(true);
        QSize s(pageDlgs[currentPage]->size());
        s.setHeight(originalHeights[currentPage]);
        setFixedSize(s);
    }
    pageDlgs[currentPage]->refresh();
}
