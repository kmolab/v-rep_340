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
#include "qdlgconstraintsolver.h"
#include "ui_qdlgconstraintsolver.h"
#include "tt.h"
#include "gV.h"
#include <QShortcut>
#include "editboxdelegate.h"
#include "geometricConstraintSolverInt.h"
#include "v_repStrings.h"
#include "app.h"
#include "vMessageBox.h"

CQDlgConstraintSolver::CQDlgConstraintSolver(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgConstraintSolver)
{
    _dlgType=CONSTRAINT_SOLVER_DLG;
    ui->setupUi(this);
    inSelectionRoutine=false;
    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(shortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
    QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
    connect(shortcut2,SIGNAL(activated()), this, SLOT(onDeletePressed()));
    CEditBoxDelegate* delegate=new CEditBoxDelegate();
    ui->qqObjectList->setItemDelegate(delegate);
}

CQDlgConstraintSolver::~CQDlgConstraintSolver()
{
    delete ui;
}

void CQDlgConstraintSolver::cancelEvent()
{
    // we override this cancel event. The container window should close, not this one!!
    App::mainWindow->dlgCont->close(CALCULATION_DLG);
}

void CQDlgConstraintSolver::dialogCallbackFunc(const SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut)
{
    if ( (cmdIn!=NULL)&&(cmdIn->intParams[0]==_dlgType) )
    {
        if (cmdIn->intParams[1]==0)
            selectObjectInList(cmdIn->intParams[2]);
    }
}

void CQDlgConstraintSolver::refresh()
{
    QLineEdit* lineEditToSelect=getSelectedLineEdit();
    bool noEditModeAndNoSim=(App::getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped();

    bool sel1=(App::ct->objCont->getSelSize()==1);
    ui->qqAllEnabled->setChecked(App::ct->mainSettings->gcsCalculationEnabled);
    ui->qqAddNew->setEnabled(sel1&&noEditModeAndNoSim);

    if (!inSelectionRoutine)
    {
        int selectedObjectID=getSelectedObjectID();
        updateObjectsInList();
        selectObjectInList(selectedObjectID);
    }

    CConstraintSolverObject* it=App::ct->constraintSolver->getObject(getSelectedObjectID());

    ui->qqObjectList->setEnabled(noEditModeAndNoSim);
    ui->qqIterations->setEnabled((it!=NULL)&&noEditModeAndNoSim);
    ui->qqInterpolation->setEnabled((it!=NULL)&&noEditModeAndNoSim);
    ui->qqDamping->setEnabled((it!=NULL)&&noEditModeAndNoSim);
    ui->qqAngularVariation->setEnabled((it!=NULL)&&noEditModeAndNoSim);
    ui->qqLinearVariation->setEnabled((it!=NULL)&&noEditModeAndNoSim);
    ui->qqLoopClosurePosTol->setEnabled((it!=NULL)&&noEditModeAndNoSim);
    ui->qqLoopClosureAngTol->setEnabled((it!=NULL)&&noEditModeAndNoSim);
    ui->qqExplicitHandling->setEnabled((it!=NULL)&&noEditModeAndNoSim);
    if (it!=NULL)
    {
        ui->qqIterations->setText(tt::getIString(false,it->getMaximumIterations()).c_str());
        ui->qqInterpolation->setText(tt::getFString(false,it->getInterpolation(),3).c_str());
        ui->qqDamping->setText(tt::getFString(false,it->getGeneralDamping(),4).c_str());
        ui->qqAngularVariation->setText(tt::getAngleFString(false,it->getMaxAngleVariation(),3).c_str());
        ui->qqLinearVariation->setText(tt::getFString(false,it->getMaxLinearVariation(),4).c_str());
        ui->qqLoopClosurePosTol->setText(tt::getFString(false,it->getLoopClosurePositionTolerance(),4).c_str());
        ui->qqLoopClosureAngTol->setText(tt::getAngleFString(false,it->getLoopClosureOrientationTolerance(),3).c_str());
        ui->qqExplicitHandling->setChecked(it->getExplicitHandling());
    }
    else
    {
        ui->qqIterations->setText("");
        ui->qqInterpolation->setText("");
        ui->qqDamping->setText("");
        ui->qqAngularVariation->setText("");
        ui->qqLinearVariation->setText("");
        ui->qqLoopClosurePosTol->setText("");
        ui->qqLoopClosureAngTol->setText("");
        ui->qqExplicitHandling->setChecked(false);
    }
    selectLineEdit(lineEditToSelect);
}

void CQDlgConstraintSolver::updateObjectsInList()
{
    ui->qqObjectList->clear();
    for (int i=0;i<int(App::ct->constraintSolver->allGcsObjects.size());i++)
    {
        CConstraintSolverObject* it=App::ct->constraintSolver->allGcsObjects[i];
        std::string tmp=it->getObjectName();
        int id=it->getObjectID();
        C3DObject* it2=App::ct->objCont->getObject(it->getBase3DObject());
        if (it2!=NULL)
        {
            tmp+=" (base: ";
            tmp+=it2->getName();
            tmp+=")";
        }
        QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
        itm->setData(Qt::UserRole,QVariant(id));
        itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
        ui->qqObjectList->addItem(itm);
    }
}

int CQDlgConstraintSolver::getSelectedObjectID()
{
    QList<QListWidgetItem*> sel=ui->qqObjectList->selectedItems();
    if (sel.size()>0)
        return(sel.at(0)->data(Qt::UserRole).toInt());
    return(-1);
}

void CQDlgConstraintSolver::selectObjectInList(int objectID)
{
    for (int i=0;i<ui->qqObjectList->count();i++)
    {
        QListWidgetItem* it=ui->qqObjectList->item(i);
        if (it!=NULL)
        {
            if (it->data(Qt::UserRole).toInt()==objectID)
            {
                it->setSelected(true);
                break;
            }
        }
    }
}

void CQDlgConstraintSolver::on_qqAllEnabled_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_ENABLEALL_GEOMCONSTRSOLVERGUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgConstraintSolver::on_qqAddNew_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(INSERT_GEOMCONSTRSOLVERGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgConstraintSolver::on_qqObjectList_itemSelectionChanged()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        int objID=getSelectedObjectID();
        CConstraintSolverObject* it=App::ct->constraintSolver->getObject(objID);
        if (it!=NULL)
            ((CEditBoxDelegate*)ui->qqObjectList->itemDelegate())->initialText=it->getObjectName();
        else
            ((CEditBoxDelegate*)ui->qqObjectList->itemDelegate())->initialText="";
        inSelectionRoutine=true;
        refresh();
        inSelectionRoutine=false;
    }
}

void CQDlgConstraintSolver::on_qqObjectList_itemChanged(QListWidgetItem *item)
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (item!=NULL)
        {
            std::string newName(item->text().toStdString());
            CConstraintSolverObject* it=App::ct->constraintSolver->getObject(item->data(Qt::UserRole).toInt());
            if ( (it!=NULL)&&(newName!="") )
            {
                App::appendSimulationThreadCommand(RENAME_GEOMCONSTRSOLVERGUITRIGGEREDCMD,it->getObjectID(),-1,0.0,0.0,newName.c_str());
                App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            }
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgConstraintSolver::onDeletePressed()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (focusWidget()==ui->qqObjectList)
        {
            int objID=getSelectedObjectID();
            if (objID!=-1)
            {
                App::appendSimulationThreadCommand(REMOVE_GEOMCONSTRSOLVERGUITRIGGEREDCMD,objID);
                App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
                App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
            }
        }
    }
}

void CQDlgConstraintSolver::on_qqExplicitHandling_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_EXPLICITHANDLING_GEOMCONSTRSOLVERGUITRIGGEREDCMD,getSelectedObjectID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgConstraintSolver::on_qqIterations_editingFinished()
{
    if (!ui->qqIterations->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        int newVal=ui->qqIterations->text().toInt(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_MAXITERATIONS_GEOMCONSTRSOLVERGUITRIGGEREDCMD,getSelectedObjectID(),newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgConstraintSolver::on_qqInterpolation_editingFinished()
{
    if (!ui->qqInterpolation->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqInterpolation->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_INTERPOLATION_GEOMCONSTRSOLVERGUITRIGGEREDCMD,getSelectedObjectID(),-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgConstraintSolver::on_qqDamping_editingFinished()
{
    if (!ui->qqDamping->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqDamping->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_DAMPING_GEOMCONSTRSOLVERGUITRIGGEREDCMD,getSelectedObjectID(),-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgConstraintSolver::on_qqLinearVariation_editingFinished()
{
    if (!ui->qqLinearVariation->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqLinearVariation->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_MAXLINVARIATION_GEOMCONSTRSOLVERGUITRIGGEREDCMD,getSelectedObjectID(),-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgConstraintSolver::on_qqAngularVariation_editingFinished()
{
    if (!ui->qqAngularVariation->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqAngularVariation->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_MAXANGVARIATION_GEOMCONSTRSOLVERGUITRIGGEREDCMD,getSelectedObjectID(),-1,newVal*gv::userToRad);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgConstraintSolver::on_qqLoopClosurePosTol_editingFinished()
{
    if (!ui->qqLoopClosurePosTol->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqLoopClosurePosTol->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_LOOPCLOSUREPOSTOL_GEOMCONSTRSOLVERGUITRIGGEREDCMD,getSelectedObjectID(),-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgConstraintSolver::on_qqLoopClosureAngTol_editingFinished()
{
    if (!ui->qqLoopClosureAngTol->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqLoopClosureAngTol->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_LOOPCLOSUREANGTOL_GEOMCONSTRSOLVERGUITRIGGEREDCMD,getSelectedObjectID(),-1,newVal*gv::userToRad);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}
