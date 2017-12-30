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
#include "qdlgsimpleSearch.h"
#include "ui_qdlgsimpleSearch.h"

std::string CQDlgSimpleSearch::textToSearch="";
bool CQDlgSimpleSearch::matchCase=false;


CQDlgSimpleSearch::CQDlgSimpleSearch(QWidget *parent) :
    VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgSimpleSearch)
{
    ui->setupUi(this);
}

CQDlgSimpleSearch::~CQDlgSimpleSearch()
{
    delete ui;
}

void CQDlgSimpleSearch::cancelEvent()
{
//  defaultModalDialogEndRoutine(false);
}

void CQDlgSimpleSearch::okEvent()
{
//  defaultModalDialogEndRoutine(true);
}

void CQDlgSimpleSearch::init()
{
    ui->qqMatchCase->setChecked(matchCase);
    ui->qqText->setText(textToSearch.c_str());
    ui->qqText->selectAll();
}

void CQDlgSimpleSearch::on_qqMatchCase_clicked()
{
   matchCase=!matchCase;
}

void CQDlgSimpleSearch::on_qqFind_clicked()
{
    textToSearch=ui->qqText->text().toStdString();
    defaultModalDialogEndRoutine(true);
}

void CQDlgSimpleSearch::on_qqText_returnPressed()
{
    textToSearch=ui->qqText->text().toStdString();
    defaultModalDialogEndRoutine(true);
}
