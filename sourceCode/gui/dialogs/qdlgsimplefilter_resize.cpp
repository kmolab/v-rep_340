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
#include "qdlgsimplefilter_resize.h"
#include "ui_qdlgsimplefilter_resize.h"
#include "gV.h"
#include "tt.h"

CQDlgSimpleFilter_resize::CQDlgSimpleFilter_resize(QWidget *parent) :
    CQDlgSimpleFilter(parent),
    ui(new Ui::CQDlgSimpleFilter_resize)
{
    ui->setupUi(this);
}

CQDlgSimpleFilter_resize::~CQDlgSimpleFilter_resize()
{
    delete ui;
}

void CQDlgSimpleFilter_resize::refresh()
{
    ui->qqX->setText(tt::getFString(false,_floatParameters[0],3).c_str());
    ui->qqY->setText(tt::getFString(false,_floatParameters[1],3).c_str());
}

void CQDlgSimpleFilter_resize::on_qqOkCancel_accepted()
{
    defaultModalDialogEndRoutine(true);
}

void CQDlgSimpleFilter_resize::on_qqOkCancel_rejected()
{
    defaultModalDialogEndRoutine(false);
}

void CQDlgSimpleFilter_resize::on_qqX_editingFinished()
{
    if (!ui->qqX->isModified())
        return;
    bool ok;
    float newVal=ui->qqX->text().toFloat(&ok);
    if (ok)
    {
        tt::limitValue(0.01f,100.0f,newVal);
        _floatParameters[0]=newVal;
    }
    refresh();
}

void CQDlgSimpleFilter_resize::on_qqY_editingFinished()
{
    if (!ui->qqY->isModified())
        return;
    bool ok;
    float newVal=ui->qqY->text().toFloat(&ok);
    if (ok)
    {
        tt::limitValue(0.01f,100.0f,newVal);
        _floatParameters[1]=newVal;
    }
    refresh();
}
