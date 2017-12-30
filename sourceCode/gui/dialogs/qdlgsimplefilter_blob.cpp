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
#include "qdlgsimplefilter_blob.h"
#include "ui_qdlgsimplefilter_blob.h"
#include "gV.h"
#include "tt.h"

CQDlgSimpleFilter_blob::CQDlgSimpleFilter_blob(QWidget *parent) :
    CQDlgSimpleFilter(parent),
    ui(new Ui::CQDlgSimpleFilter_blob)
{
    ui->setupUi(this);
}

CQDlgSimpleFilter_blob::~CQDlgSimpleFilter_blob()
{
    delete ui;
}

void CQDlgSimpleFilter_blob::refresh()
{
    ui->qqThreshold->setText(tt::getFString(false,_floatParameters[0],3).c_str());
    ui->qqMinSize->setText(tt::getFString(false,_floatParameters[1],3).c_str());
    ui->qqDiffColor->setChecked(_byteParameters[0]&1);
}

void CQDlgSimpleFilter_blob::on_qqOkCancel_accepted()
{
    defaultModalDialogEndRoutine(true);
}

void CQDlgSimpleFilter_blob::on_qqOkCancel_rejected()
{
    defaultModalDialogEndRoutine(false);
}

void CQDlgSimpleFilter_blob::on_qqThreshold_editingFinished()
{
    if (!ui->qqThreshold->isModified())
        return;
    bool ok;
    float newVal=ui->qqThreshold->text().toFloat(&ok);
    if (ok)
    {
        tt::limitValue(0.01f,1.0f,newVal);
        _floatParameters[0]=newVal;
    }
    refresh();
}

void CQDlgSimpleFilter_blob::on_qqDiffColor_clicked()
{
    if (_byteParameters[0]&1)
        _byteParameters[0]-=1;
    else
        _byteParameters[0]|=1;
    refresh();
}

void CQDlgSimpleFilter_blob::on_qqMinSize_editingFinished()
{
    if (!ui->qqMinSize->isModified())
        return;
    bool ok;
    float newVal=ui->qqMinSize->text().toFloat(&ok);
    if (ok)
    {
        tt::limitValue(0.0f,1.0f,newVal);
        _floatParameters[1]=newVal;
    }
    refresh();
}
