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

#include "qdlgtriangleedit.h"
#include "ui_qdlgtriangleedit.h"
#include "app.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>

CQDlgTriangleEdit::CQDlgTriangleEdit(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgTriangleEdit)
{
    ui->setupUi(this);
}

CQDlgTriangleEdit::~CQDlgTriangleEdit()
{
    delete ui;
}

void CQDlgTriangleEdit::cancelEvent()
{
    // we override this cancel event. The container window should close, not this one!!
    App::mainWindow->dlgCont->close(SHAPE_EDITION_DLG);
}

void CQDlgTriangleEdit::refresh()
{
    std::string tmp=std::string(IDS_TOTAL_TRIANGLES)+": "+boost::lexical_cast<std::string>(App::mainWindow->editModeContainer->getEditModeBufferSize())+"/"+
        boost::lexical_cast<std::string>(App::mainWindow->editModeContainer->getShapeEditMode()->getEditionIndicesSize()/3);
    ui->qqInfo->setText(tmp.c_str());
    ui->qqMakeShape->setEnabled(App::mainWindow->editModeContainer->getEditModeBufferSize()!=0);
    ui->qqMakeCuboid->setEnabled(App::mainWindow->editModeContainer->getEditModeBufferSize()!=0);
    ui->qqMakeCylinder->setEnabled(App::mainWindow->editModeContainer->getEditModeBufferSize()!=0);
    ui->qqMakeSphere->setEnabled(App::mainWindow->editModeContainer->getEditModeBufferSize()!=0);
    ui->qqFlip->setEnabled(App::mainWindow->editModeContainer->getEditModeBufferSize()!=0);
    ui->qqSubdivide->setEnabled(true);
}

void CQDlgTriangleEdit::on_qqMakeShape_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->makeShape();
    }
}

void CQDlgTriangleEdit::on_qqMakeCuboid_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->makePrimitive(0);
    }
}

void CQDlgTriangleEdit::on_qqMakeCylinder_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->makePrimitive(3);
    }
}

void CQDlgTriangleEdit::on_qqMakeSphere_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->makePrimitive(1);
    }
}

void CQDlgTriangleEdit::on_qqFlip_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->flipTriangles();
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgTriangleEdit::on_qqSubdivide_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->subdivideTriangles();
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgTriangleEdit::on_qqClearSelection_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        App::mainWindow->editModeContainer->deselectEditModeBuffer();
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgTriangleEdit::on_qqInvertSelection_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        for (int i=0;i<App::mainWindow->editModeContainer->getShapeEditMode()->getEditionIndicesSize()/3;i++)
            App::mainWindow->editModeContainer->getShapeEditMode()->xorAddItemToEditModeBuffer(i,true);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}
