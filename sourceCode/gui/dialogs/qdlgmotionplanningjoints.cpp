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
#include "qdlgmotionplanningjoints.h"
#include "ui_qdlgmotionplanningjoints.h"
#include "gV.h"
#include "tt.h"
#include "app.h"
#include "v_repStringTable.h"

CQDlgMotionPlanningJoints::CQDlgMotionPlanningJoints(QWidget *parent) :
    VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgMotionPlanningJoints)
{
    ui->setupUi(this);
    inMainRefreshRoutine=false;
    noTableSelectionAllowed=false;
    first=true;
}

CQDlgMotionPlanningJoints::~CQDlgMotionPlanningJoints()
{
    delete ui;
}

void CQDlgMotionPlanningJoints::cancelEvent()
{
//  defaultModalDialogEndRoutine(false);
}

void CQDlgMotionPlanningJoints::okEvent()
{
//  defaultModalDialogEndRoutine(true);
}

void CQDlgMotionPlanningJoints::refresh()
{
    inMainRefreshRoutine=true;

    if (first)
        updateObjectsInTable();

    int retV=1;
    for (size_t i=0;i<jointSubdiv.size();i++)
        retV*=jointSubdiv[i];

    ui->qqNodeCnt->setText(tt::FNb(0,retV,false).c_str());

    inMainRefreshRoutine=false;
    first=false;
}


void CQDlgMotionPlanningJoints::updateObjectsInTable()
{
    noTableSelectionAllowed=true;
    ui->qqTable->clear();
    ui->qqTable->insertColumn(0);
    ui->qqTable->insertColumn(1);
    ui->qqTable->insertColumn(2);
    ui->qqTable->setColumnWidth(0,140);
    ui->qqTable->setColumnWidth(1,100);
    ui->qqTable->setColumnWidth(2,75);
    QStringList labels;
    labels << IDSN_JOINT_NAME << IDSN_RANGE_SUBDIVISIONS << IDSN_METRIC_WEIGHT;
    ui->qqTable->setHorizontalHeaderLabels(labels);

    for (size_t i=0;i<jointNames.size();i++)
    {
        ui->qqTable->insertRow(i);
        QTableWidgetItem* item=new QTableWidgetItem();
        ui->qqTable->setItem(i,0,item);

        item->setFlags(Qt::ItemIsEnabled);
        item->setText(jointNames[i].c_str());

        item=new QTableWidgetItem();
        ui->qqTable->setItem(i,1,item);
        item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
        item->setText(tt::FNb(0,jointSubdiv[i]-1,false).c_str());

        item=new QTableWidgetItem();
        ui->qqTable->setItem(i,2,item);
        item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
        item->setText(tt::FNb(0,jointMetricWeight[i],2,false).c_str());
    }
    noTableSelectionAllowed=false;
}


void CQDlgMotionPlanningJoints::on_qqClose_clicked(QAbstractButton *button)
{
    defaultModalDialogEndRoutine(true);
}


void CQDlgMotionPlanningJoints::on_qqTable_itemChanged(QTableWidgetItem *item)
{
    if (inMainRefreshRoutine)
        return;
    if (item!=NULL)
    {
        int jointIndex=item->row();
        if (item->column()==1)
        { // subdivisions changed
            bool ok;
            int newVal=item->text().toInt(&ok);
            if (ok)
            {
                newVal=tt::getLimitedInt(2,49,newVal);
                jointSubdiv[jointIndex]=newVal+1;
            }
            item->setText(tt::FNb(0,jointSubdiv[jointIndex]-1,false).c_str());
        }
        if (item->column()==2)
        { // weight changed
            bool ok;
            float newVal=item->text().toFloat(&ok);
            if (ok)
            {
                newVal=tt::getLimitedFloat(0.0f,1000.0f,newVal);
                jointMetricWeight[jointIndex]=newVal;
            }
            item->setText(tt::FNb(0,jointMetricWeight[jointIndex],2,false).c_str());
        }
        refresh();
    }
}
