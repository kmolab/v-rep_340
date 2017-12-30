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
#include "qdlgaddgraphcurve.h"
#include "ui_qdlgaddgraphcurve.h"
#include "gV.h"
#include "tt.h"
#include "graphingRoutines.h"
#include "app.h"


CQDlgAddGraphCurve::CQDlgAddGraphCurve(QWidget *parent) :
    VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgAddGraphCurve)
{
    ui->setupUi(this);
}

CQDlgAddGraphCurve::~CQDlgAddGraphCurve()
{
    delete ui;
}

void CQDlgAddGraphCurve::cancelEvent()
{
    defaultModalDialogEndRoutine(false);
}

void CQDlgAddGraphCurve::okEvent()
{
//  defaultModalDialogEndRoutine(true);
}

void CQDlgAddGraphCurve::refresh()
{
    CGraph* it=App::ct->objCont->getLastSelection_graph();
    ui->qqZValue->setVisible(!xyGraph);
    ui->qqComboZ->setVisible(!xyGraph);

    ui->qqComboX->clear();
    ui->qqComboY->clear();
    ui->qqComboZ->clear();
    ui->qqComboX->addItem("0.0",QVariant(-1));
    ui->qqComboY->addItem("0.0",QVariant(-1));
    ui->qqComboZ->addItem("0.0",QVariant(-1));
    for (size_t i=0;i<it->daten.size();i++)
    {
        int theID=it->daten[i]->getIdentifier();
        ui->qqComboX->addItem(it->daten[i]->getName().c_str(),QVariant(theID));
        ui->qqComboY->addItem(it->daten[i]->getName().c_str(),QVariant(theID));
        if (!xyGraph)
            ui->qqComboZ->addItem(it->daten[i]->getName().c_str(),QVariant(theID));
    }
}

void CQDlgAddGraphCurve::on_qqOkCancel_accepted()
{
    dataIDX=ui->qqComboX->itemData(ui->qqComboX->currentIndex()).toInt();
    dataIDY=ui->qqComboY->itemData(ui->qqComboY->currentIndex()).toInt();
    dataIDZ=ui->qqComboZ->itemData(ui->qqComboZ->currentIndex()).toInt();
    defaultModalDialogEndRoutine(true);
}

void CQDlgAddGraphCurve::on_qqOkCancel_rejected()
{
    defaultModalDialogEndRoutine(false);
}
