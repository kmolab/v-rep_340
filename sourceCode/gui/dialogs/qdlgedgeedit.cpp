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

#include "qdlgedgeedit.h"
#include "ui_qdlgedgeedit.h"
#include "app.h"
#include "gV.h"
#include "tt.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>

CQDlgEdgeEdit::CQDlgEdgeEdit(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgEdgeEdit)
{
    ui->setupUi(this);
}

CQDlgEdgeEdit::~CQDlgEdgeEdit()
{
    delete ui;
}

void CQDlgEdgeEdit::cancelEvent()
{
    // we override this cancel event. The container window should close, not this one!!
    App::mainWindow->dlgCont->close(SHAPE_EDITION_DLG);
}

void CQDlgEdgeEdit::refresh()
{
    ui->qqShowHiddenEdges->setChecked(App::mainWindow->editModeContainer->getShapeEditMode()->getShowHiddenVerticeAndEdges());
    ui->qqAutoFollowing->setChecked(App::mainWindow->editModeContainer->getShapeEditMode()->getAutomaticallyFollowEdges());
    ui->qqEdgeAngle->setEnabled(App::mainWindow->editModeContainer->getShapeEditMode()->getAutomaticallyFollowEdges());
    ui->qqDirectionChangeAngle->setEnabled(App::mainWindow->editModeContainer->getShapeEditMode()->getAutomaticallyFollowEdges());
    ui->qqEdgeAngle->setText(tt::getAngleFString(false,App::mainWindow->editModeContainer->getShapeEditMode()->getEdgeMaxAngle(),1).c_str());
    ui->qqDirectionChangeAngle->setText(tt::getAngleFString(false,App::mainWindow->editModeContainer->getShapeEditMode()->getEdgeDirectionChangeMaxAngle(),1).c_str());

    std::string tmp=std::string(IDS_TOTAL_EDGES)+": "+boost::lexical_cast<std::string>(App::mainWindow->editModeContainer->getEditModeBufferSize())+"/"+
        boost::lexical_cast<std::string>(App::mainWindow->editModeContainer->getShapeEditMode()->getEditionEdgesSize()/2);
    ui->qqInfo->setText(tmp.c_str());

    ui->qqMakePath->setEnabled(App::mainWindow->editModeContainer->getEditModeBufferSize()!=0);
}

void CQDlgEdgeEdit::on_qqShowHiddenEdges_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->setShowHiddenVerticeAndEdges(!App::mainWindow->editModeContainer->getShapeEditMode()->getShowHiddenVerticeAndEdges());
        refresh();
    }
}

void CQDlgEdgeEdit::on_qqAutoFollowing_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->setAutomaticallyFollowEdges(!App::mainWindow->editModeContainer->getShapeEditMode()->getAutomaticallyFollowEdges());
        refresh();
    }
}

void CQDlgEdgeEdit::on_qqEdgeAngle_editingFinished()
{
    if (!ui->qqEdgeAngle->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqEdgeAngle->text().toFloat(&ok);
        if (ok)
        {
            newVal=tt::getLimitedFloat(1.0f*degToRad,180.0f*degToRad,newVal*gv::userToRad);
            App::mainWindow->editModeContainer->getShapeEditMode()->setEdgeMaxAngle(newVal);
        }
        refresh();
    }
}

void CQDlgEdgeEdit::on_qqDirectionChangeAngle_editingFinished()
{
    if (!ui->qqDirectionChangeAngle->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqDirectionChangeAngle->text().toFloat(&ok);
        if (ok)
        {
            newVal=tt::getLimitedFloat(0.0f*degToRad,180.0f*degToRad,newVal*gv::userToRad);
            App::mainWindow->editModeContainer->getShapeEditMode()->setEdgeDirectionChangeMaxAngle(newVal);
        }
        refresh();
    }
}

void CQDlgEdgeEdit::on_qqMakePath_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->makePath();
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgEdgeEdit::on_qqClearSelection_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->deselectEditModeBuffer();
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgEdgeEdit::on_qqInvertSelection_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        for (int i=0;i<App::mainWindow->editModeContainer->getShapeEditMode()->getEditionEdgesSize()/2;i++)
            App::mainWindow->editModeContainer->getShapeEditMode()->xorAddItemToEditModeBuffer(i,true);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}
