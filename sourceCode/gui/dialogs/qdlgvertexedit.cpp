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

#include "qdlgvertexedit.h"
#include "ui_qdlgvertexedit.h"
#include "app.h"
#include "gV.h"
#include "tt.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>

CQDlgVertexEdit::CQDlgVertexEdit(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgVertexEdit)
{
    ui->setupUi(this);
}

CQDlgVertexEdit::~CQDlgVertexEdit()
{
    delete ui;
}

void CQDlgVertexEdit::cancelEvent()
{
    // we override this cancel event. The container window should close, not this one!!
    App::mainWindow->dlgCont->close(SHAPE_EDITION_DLG);
}

void CQDlgVertexEdit::refresh()
{
    ui->qqShowHiddenVertices->setChecked(App::mainWindow->editModeContainer->getShapeEditMode()->getShowHiddenVerticeAndEdges());

    std::string tmp=std::string(IDS_TOTAL_VERTICES)+": "+boost::lexical_cast<std::string>(App::mainWindow->editModeContainer->getEditModeBufferSize())+"/"+
        boost::lexical_cast<std::string>(App::mainWindow->editModeContainer->getShapeEditMode()->getEditionVerticesSize()/3);
    ui->qqInfo->setText(tmp.c_str());

    ui->qqInsertTriangles->setEnabled(App::mainWindow->editModeContainer->getEditModeBufferSize()>2);
    ui->qqInsertFan->setEnabled(App::mainWindow->editModeContainer->getEditModeBufferSize()>3);
    ui->qqMakeDummies->setEnabled(App::mainWindow->editModeContainer->getEditModeBufferSize()!=0);
}

void CQDlgVertexEdit::on_qqShowHiddenVertices_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->setShowHiddenVerticeAndEdges(!App::mainWindow->editModeContainer->getShapeEditMode()->getShowHiddenVerticeAndEdges());
        refresh();
    }
}

void CQDlgVertexEdit::on_qqInsertTriangles_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->insertTriangles();
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgVertexEdit::on_qqInsertFan_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->insertTriangleFan();
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgVertexEdit::on_qqMakeDummies_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::mainWindow->editModeContainer->getShapeEditMode()->makeDummies();
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgVertexEdit::on_qqClearSelection_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        App::mainWindow->editModeContainer->deselectEditModeBuffer();
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgVertexEdit::on_qqInvertSelection_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        for (int i=0;i<App::mainWindow->editModeContainer->getShapeEditMode()->getEditionVerticesSize()/3;i++)
            App::mainWindow->editModeContainer->getShapeEditMode()->xorAddItemToEditModeBuffer(i,true);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}
