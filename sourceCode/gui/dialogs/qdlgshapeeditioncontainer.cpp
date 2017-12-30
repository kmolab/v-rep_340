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
#include "qdlgshapeeditioncontainer.h"
#include "ui_qdlgshapeeditioncontainer.h"
#include "app.h"
#include "qdlgtriangleedit.h"
#include "qdlgvertexedit.h"
#include "qdlgedgeedit.h"
#include "sceneObjectOperations.h"

#define TOP_BORDER_WIDTH 30

CQDlgShapeEditionContainer::CQDlgShapeEditionContainer(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgShapeEditionContainer)
{
    _dlgType=SHAPE_EDITION_DLG;
    ui->setupUi(this);
    pageDlgs[0]=new CQDlgTriangleEdit();
    originalHeights[0]=pageDlgs[0]->size().height();
    pageDlgs[1]=new CQDlgVertexEdit();
    originalHeights[1]=pageDlgs[1]->size().height();
    pageDlgs[2]=new CQDlgEdgeEdit();
    originalHeights[2]=pageDlgs[2]->size().height();

    currentPage=0;
    desiredPage=0;
    bl=new QVBoxLayout();
    bl->setContentsMargins(0,TOP_BORDER_WIDTH,0,0);
    setLayout(bl);
    bl->addWidget(pageDlgs[0]);
    bl->addWidget(pageDlgs[1]);
    pageDlgs[1]->setVisible(false);
    bl->addWidget(pageDlgs[2]);
    pageDlgs[2]->setVisible(false);


    QSize s(pageDlgs[currentPage]->size());
    s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);
    setFixedSize(s);
/*
#ifndef WIN_VREP
    // Since Qt5, we have problems on Linux (resizing-->dlg shifts in position) and Mac (resising-->ugly glitch)
    // In that case we keep constant-size dialogs.
    if (QT_VERSION>=0x050000)
    {
        int dlgMaxHeight=0;
        for (int i=0;i<3;i++)
        {
            int si=pageDlgs[i]->size().height();
            if (si>dlgMaxHeight)
                dlgMaxHeight=si;
        }
        s.setHeight(dlgMaxHeight+TOP_BORDER_WIDTH);
        setFixedSize(s);
    }
#endif
*/
}

CQDlgShapeEditionContainer::~CQDlgShapeEditionContainer()
{
    delete ui;
}

void CQDlgShapeEditionContainer::cancelEvent()
{
    App::appendSimulationThreadCommand(ANY_EDIT_MODE_FINISH_WITH_QUESTION_DLG_EMCMD);
//  defaultModalDialogEndRoutine(false);
}

void CQDlgShapeEditionContainer::okEvent()
{
//  defaultModalDialogEndRoutine(true);
}

void CQDlgShapeEditionContainer::refresh()
{
    ui->qqTriangleMode->setChecked(desiredPage==0);
    ui->qqVertexMode->setChecked(desiredPage==1);
    ui->qqEdgeMode->setChecked(desiredPage==2);

    if (desiredPage!=currentPage)
    {
        pageDlgs[currentPage]->setVisible(false);
        currentPage=desiredPage;
        pageDlgs[currentPage]->setVisible(true);

        QSize s(pageDlgs[currentPage]->size());
        s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);

#ifdef MAC_VREP
        if (QT_VERSION>=0x050000)
        { // Since Qt5, we have problems on Mac (resising-->ugly glitch)
            setVisible(false);
            setFixedSize(s);
            setVisible(true);
        }
        else
            setFixedSize(s);
#else
        setFixedSize(s);
#endif

/*
#ifdef WIN_VREP
        // Since Qt5, we have problems on Linux (resizing-->dlg shifts in position) and Mac (resising-->ugly glitch)
        // In that case we keep constant-size dialogs.
        QSize s(pageDlgs[currentPage]->size());
        s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);
        setFixedSize(s);
#else
        if (QT_VERSION<0x050000)
        {
            QSize s(pageDlgs[currentPage]->size());
            s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);
            setFixedSize(s);
        }
#endif
*/
    }

    pageDlgs[currentPage]->refresh();
}

void CQDlgShapeEditionContainer::on_qqTriangleMode_clicked()
{   // Following executed by the main simulation thread:
    App::appendSimulationThreadCommand(SHAPE_EDIT_MODE_SWAP_TO_TRIANGLE_EDIT_MODE_EMCMD);
}

void CQDlgShapeEditionContainer::on_qqVertexMode_clicked()
{   // Following executed by the main simulation thread:
    App::appendSimulationThreadCommand(SHAPE_EDIT_MODE_SWAP_TO_VERTEX_EDIT_MODE_EMCMD);
}

void CQDlgShapeEditionContainer::on_qqEdgeMode_clicked()
{   // Following executed by the main simulation thread:
    App::appendSimulationThreadCommand(SHAPE_EDIT_MODE_SWAP_TO_EDGE_EDIT_MODE_EMCMD);
}
