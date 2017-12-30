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
#include "qdlgikavoidance.h"
#include "ui_qdlgikavoidance.h"
#include "app.h"
#include "tt.h"
#include "gV.h"
#include "qdlgentityvsentityselection.h"
#include "v_repStrings.h"

CQDlgIkAvoidance::CQDlgIkAvoidance(QWidget *parent) :
    VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgIkAvoidance)
{
    ui->setupUi(this);
    first=true;
}

CQDlgIkAvoidance::~CQDlgIkAvoidance()
{
    delete ui;
}

void CQDlgIkAvoidance::cancelEvent()
{
//  defaultModalDialogEndRoutine(false);
}

void CQDlgIkAvoidance::okEvent()
{
//  defaultModalDialogEndRoutine(true);
}

void CQDlgIkAvoidance::refresh()
{
    if (first)
    {
        avoidanceConstraintEnabled=(ikGroup->getConstraints()&sim_ik_avoidance_constraint);
        avoidanceDistance=ikGroup->getAvoidanceThreshold();
        avoidanceEntity1=ikGroup->getAvoidanceRobot();
        avoidanceEntity2=ikGroup->getAvoidanceObstacle();
    }
    first=false;

    if (avoidanceConstraintEnabled)
    {
        ui->qqDistance->setEnabled(true);
        ui->qqDistance->setText(tt::getFString(false,avoidanceDistance,3).c_str());
        std::string tmp;
        if (avoidanceEntity1<SIM_IDSTART_COLLECTION)
        {
            C3DObject* it=App::ct->objCont->getObject(avoidanceEntity1);
            if (it!=NULL)
            {
                tmp=it->getName();
                tmp+=tt::decorateString(" [",strTranslate(IDSN_SHAPE),"]");
            }
        }
        else
        {
            CRegCollection* it=App::ct->collections->getCollection(avoidanceEntity1);
            if (it!=NULL)
            {
                tmp=it->getCollectionName();
                tmp+=tt::decorateString(" [",strTranslate(IDSN_COLLECTION),"]");
            }
        }
        ui->qqEntity1->setText(tmp.c_str());

        if (avoidanceEntity2<SIM_IDSTART_COLLECTION)
        {
            C3DObject* it=App::ct->objCont->getObject(avoidanceEntity2);
            if (it!=NULL)
            {
                tmp=it->getName();
                tmp+=tt::decorateString(" [",strTranslate(IDSN_SHAPE),"]");
            }
            else
                tmp=strTranslate(IDS_ALL_OTHER_MEASURABLE_OBJECTS_IN_SCENE);
        }
        else
        {
            CRegCollection* it=App::ct->collections->getCollection(avoidanceEntity2);
            if (it!=NULL)
            {
                tmp=it->getCollectionName();
                tmp+=tt::decorateString(" [",strTranslate(IDSN_COLLECTION),"]");
            }
        }

        ui->qqEntity2->setText(tmp.c_str());
        ui->qqSelectEntities->setText(strTranslate("Remove avoidance constraint"));
    }
    else
    {
        ui->qqDistance->setEnabled(false);
        ui->qqDistance->setText("");
        ui->qqEntity1->setText("");
        ui->qqEntity2->setText("");
        ui->qqSelectEntities->setText(strTranslate("Select avoidance entities"));
    }
}

void CQDlgIkAvoidance::on_qqSelectEntities_clicked()
{
    if (avoidanceConstraintEnabled)
    {
        avoidanceEntity1=-1;
        avoidanceEntity2=-1;
        avoidanceConstraintEnabled=false;
//      ikGroup->setAvoidanceRobot(-1);
//      ikGroup->setAvoidanceObstacle(-1);
//      ikGroup->setConstraints((ikGroup->getConstraints()|sim_ik_avoidance_constraint)-sim_ik_avoidance_constraint);
    }
    else
    {
        CQDlgEntityVsEntitySelection theDialog(this);
        theDialog.initialize(2);
        if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
        {
            avoidanceEntity1=theDialog.entity1;
            avoidanceEntity2=theDialog.entity2;
            avoidanceConstraintEnabled=true;
//          ikGroup->setConstraints(ikGroup->getConstraints()|sim_ik_avoidance_constraint);
//          ikGroup->setAvoidanceRobot(theDialog.entity1);
//          ikGroup->setAvoidanceObstacle(theDialog.entity2);
        }
    }
    refresh();
}

void CQDlgIkAvoidance::on_qqDistance_editingFinished()
{
    if (!ui->qqDistance->isModified())
        return;
    bool ok;
    float newVal=ui->qqDistance->text().toFloat(&ok);
    if (ok)
    {
        tt::limitValue(0.001f,1.0f,newVal);
        avoidanceDistance=newVal;
//      ikGroup->setAvoidanceThreshold(newVal);
    }
    refresh();
}

void CQDlgIkAvoidance::on_qqClose_clicked(QAbstractButton *button)
{
    defaultModalDialogEndRoutine(true);
}
