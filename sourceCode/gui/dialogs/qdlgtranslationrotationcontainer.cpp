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
#include "qdlgtranslationrotationcontainer.h"
#include "ui_qdlgtranslationrotationcontainer.h"
#include "app.h"

#include "toolBarCommand.h"
#include "qdlgtranslations.h"
#include "qdlgrotations.h"

#define TOP_BORDER_WIDTH 30

CQDlgTranslationRotationContainer::CQDlgTranslationRotationContainer(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgTranslationRotationContainer)
{
    _dlgType=TRANSLATION_ROTATION_DLG;
    ui->setupUi(this);
    pageDlgs[0]=new CQDlgTranslations();
    originalHeights[0]=pageDlgs[0]->size().height();
    pageDlgs[1]=new CQDlgRotations();
    originalHeights[1]=pageDlgs[1]->size().height();

    currentPage=0;
    desiredPage=0;
    bl=new QVBoxLayout();
    bl->setContentsMargins(0,TOP_BORDER_WIDTH,0,0);
    setLayout(bl);
    bl->addWidget(pageDlgs[0]);
    bl->addWidget(pageDlgs[1]);
    pageDlgs[1]->setVisible(false);

    QSize s(pageDlgs[currentPage]->size());
    s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);
    setFixedSize(s);
}

CQDlgTranslationRotationContainer::~CQDlgTranslationRotationContainer()
{
    delete ui;
}

void CQDlgTranslationRotationContainer::cancelEvent()
{
    App::setMouseMode((App::getMouseMode()&0xff00)|sim_navigation_camerashift);
//  defaultModalDialogEndRoutine(false);
}

void CQDlgTranslationRotationContainer::okEvent()
{
//  defaultModalDialogEndRoutine(true);
}

void CQDlgTranslationRotationContainer::refresh()
{
    if ((App::getMouseMode()&0x0f)==sim_navigation_objectshift)
        desiredPage=0;
    if ((App::getMouseMode()&0x0f)==sim_navigation_objectrotate)
        desiredPage=1;
    ui->qqTranslationMode->setChecked(desiredPage==0);
    ui->qqRotationMode->setChecked(desiredPage==1);

    ui->qqTranslationMode->setEnabled((!App::mainWindow->oglSurface->isSceneSelectionActive())&&App::mainWindow->getObjectShiftToggleViaGuiEnabled());
    bool rot=true;
    if (App::ct->objCont!=NULL)
        rot=App::mainWindow->editModeContainer->pathPointManipulation->getSelectedPathPointIndicesSize_nonEditMode()==0;
    ui->qqRotationMode->setEnabled(rot&&(!App::mainWindow->oglSurface->isSceneSelectionActive())&&App::mainWindow->getObjectRotateToggleViaGuiEnabled());

    if (desiredPage!=currentPage)
    {
        pageDlgs[currentPage]->setVisible(false);
        currentPage=desiredPage;
        pageDlgs[currentPage]->setVisible(true);
        QSize s(pageDlgs[currentPage]->size());
        s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);
        setFixedSize(s);
    }
    pageDlgs[currentPage]->refresh();
}

void CQDlgTranslationRotationContainer::on_qqTranslationMode_clicked()
{ // Following executed by the main simulation thread:
    CToolBarCommand::processCommand(OBJECT_SHIFT_NAVIGATION_CMD);
}

void CQDlgTranslationRotationContainer::on_qqRotationMode_clicked()
{ // Following executed by the main simulation thread:
    CToolBarCommand::processCommand(OBJECT_ROTATE_NAVIGATION_CMD);
}
