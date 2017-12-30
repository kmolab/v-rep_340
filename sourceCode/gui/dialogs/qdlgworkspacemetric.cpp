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
#include "qdlgworkspacemetric.h"
#include "ui_qdlgworkspacemetric.h"
#include "gV.h"
#include "tt.h"
#include "app.h"
#include "v_repStringTable.h"

CQDlgWorkSpaceMetric::CQDlgWorkSpaceMetric(QWidget *parent) :
    VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgWorkSpaceMetric)
{
    ui->setupUi(this);
}

CQDlgWorkSpaceMetric::~CQDlgWorkSpaceMetric()
{
    delete ui;
}

void CQDlgWorkSpaceMetric::cancelEvent()
{
//  defaultModalDialogEndRoutine(false);
}

void CQDlgWorkSpaceMetric::okEvent()
{
//  defaultModalDialogEndRoutine(true);
}

void CQDlgWorkSpaceMetric::refresh()
{
    ui->qqX->setText(tt::getFString(false,data[0],3).c_str());
    ui->qqY->setText(tt::getFString(false,data[1],3).c_str());
    ui->qqZ->setText(tt::getFString(false,data[2],3).c_str());
    ui->qqAbg->setText(tt::getFString(false,data[3],3).c_str());
}

void CQDlgWorkSpaceMetric::on_qqClose_clicked(QAbstractButton *button)
{
    defaultModalDialogEndRoutine(true);
}

void CQDlgWorkSpaceMetric::on_qqX_editingFinished()
{
    if (!ui->qqX->isModified())
        return;
    bool ok;
    float newVal=ui->qqX->text().toFloat(&ok);
    if (ok)
        data[0]=tt::getLimitedFloat(0.0f,1000.0f,newVal);
    refresh();
}

void CQDlgWorkSpaceMetric::on_qqY_editingFinished()
{
    if (!ui->qqY->isModified())
        return;
    bool ok;
    float newVal=ui->qqY->text().toFloat(&ok);
    if (ok)
        data[1]=tt::getLimitedFloat(0.0f,1000.0f,newVal);
    refresh();
}

void CQDlgWorkSpaceMetric::on_qqZ_editingFinished()
{
    if (!ui->qqZ->isModified())
        return;
    bool ok;
    float newVal=ui->qqZ->text().toFloat(&ok);
    if (ok)
        data[2]=tt::getLimitedFloat(0.0f,1000.0f,newVal);
    refresh();
}

void CQDlgWorkSpaceMetric::on_qqAbg_editingFinished()
{
    if (!ui->qqAbg->isModified())
        return;
    bool ok;
    float newVal=ui->qqAbg->text().toFloat(&ok);
    if (ok)
        data[3]=tt::getLimitedFloat(0.0f,1000.0f,newVal);
    refresh();
}
