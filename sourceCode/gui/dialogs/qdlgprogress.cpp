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

#include "qdlgprogress.h"
#include "ui_qdlgprogress.h"

CQDlgProgress::CQDlgProgress(QWidget *parent) : QDialog(parent,Qt::SplashScreen), ui(new Ui::CQDlgProgress)
{
    ui->setupUi(this);
    ui->qqBar->setMinimum(0);
    ui->qqBar->setMaximum(100);
    ui->qqBar->setValue(0);
}

CQDlgProgress::~CQDlgProgress()
{
    delete ui;
}

void CQDlgProgress::updateProgress(float p,const char* txt)
{
    if (p<0.0f)
        ui->qqBar->setMaximum(0);
    else
        ui->qqBar->setValue(int(p*100.0f));
    ui->qqText->setText(txt);
}


