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
#include "qdlgsimplefilter_velodyne.h"
#include "ui_qdlgsimplefilter_velodyne.h"
#include "gV.h"
#include "tt.h"

CQDlgSimpleFilter_velodyne::CQDlgSimpleFilter_velodyne(QWidget *parent) :
    CQDlgSimpleFilter(parent),
    ui(new Ui::CQDlgSimpleFilter_velodyne)
{
    ui->setupUi(this);
}

CQDlgSimpleFilter_velodyne::~CQDlgSimpleFilter_velodyne()
{
    delete ui;
}

void CQDlgSimpleFilter_velodyne::refresh()
{
    ui->qqX->setText(tt::getIString(false,_intParameters[0]).c_str());
    ui->qqY->setText(tt::getIString(false,_intParameters[1]).c_str());
    ui->qqV->setText(tt::getAngleFString(false,_floatParameters[0],2).c_str());
}

void CQDlgSimpleFilter_velodyne::on_qqOkCancel_accepted()
{
    defaultModalDialogEndRoutine(true);
}

void CQDlgSimpleFilter_velodyne::on_qqOkCancel_rejected()
{
    defaultModalDialogEndRoutine(false);
}

void CQDlgSimpleFilter_velodyne::on_qqX_editingFinished()
{
    if (!ui->qqX->isModified())
        return;
    bool ok;
    int newVal=ui->qqX->text().toInt(&ok);
    if (ok)
    {
        tt::limitValue(1,2048,newVal);
        _intParameters[0]=newVal;
    }
    refresh();
}

void CQDlgSimpleFilter_velodyne::on_qqY_editingFinished()
{
    if (!ui->qqY->isModified())
        return;
    bool ok;
    int newVal=ui->qqY->text().toInt(&ok);
    if (ok)
    {
        tt::limitValue(1,2048,newVal);
        _intParameters[1]=newVal;
    }
    refresh();
}


void CQDlgSimpleFilter_velodyne::on_qqV_editingFinished()
{
    if (!ui->qqV->isModified())
        return;
    bool ok;
    float newVal=ui->qqV->text().toFloat(&ok);
    if (ok)
    {
        tt::limitValue(1.0f,120.0f,newVal);
        _floatParameters[0]=newVal*gv::userToRad;
    }
    refresh();
}
