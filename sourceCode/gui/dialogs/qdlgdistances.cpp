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
#include "qdlgdistances.h"
#include "ui_qdlgdistances.h"
#include "tt.h"
#include <QShortcut>
#include "qdlgentityvsentityselection.h"
#include "editboxdelegate.h"
#include "qdlgmaterial.h"
#include "app.h"
#include "gV.h"

CQDlgDistances::CQDlgDistances(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgDistances)
{
    _dlgType=DISTANCE_DLG;
    ui->setupUi(this);
    inSelectionRoutine=false;
    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(shortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
    QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
    connect(shortcut2,SIGNAL(activated()), this, SLOT(onDeletePressed()));
    CEditBoxDelegate* delegate=new CEditBoxDelegate();
    ui->qqDistanceList->setItemDelegate(delegate);
}

CQDlgDistances::~CQDlgDistances()
{
    delete ui;
}

void CQDlgDistances::cancelEvent()
{
    // we override this cancel event. The container window should close, not this one!!
    App::mainWindow->dlgCont->close(CALCULATION_DLG);
}

void CQDlgDistances::dialogCallbackFunc(const SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut)
{
    if ( (cmdIn!=NULL)&&(cmdIn->intParams[0]==_dlgType) )
    {
        if (cmdIn->intParams[1]==0)
            selectObjectInList(cmdIn->intParams[2]);
    }
}

void CQDlgDistances::refresh()
{
    QLineEdit* lineEditToSelect=getSelectedLineEdit();
    bool noEditModeNoSim=(App::getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped();

    ui->qqEnableAll->setChecked(App::ct->mainSettings->distanceCalculationEnabled);

    if (!inSelectionRoutine)
    {
        int selectedObjectID=getSelectedObjectID();
        updateObjectsInList();
        selectObjectInList(selectedObjectID);
    }

    CRegDist* it=App::ct->distances->getObject(getSelectedObjectID());
    ui->qqAddNewObject->setEnabled(noEditModeNoSim);
    ui->qqExplicitHandling->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqUseThreshold->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqDisplaySegment->setEnabled((it!=NULL)&&noEditModeNoSim);

    ui->qqAdjustSegmentColor->setEnabled((it!=NULL)&&it->getDisplaySegment()&&noEditModeNoSim);
    ui->qqThreshold->setEnabled((it!=NULL)&&it->getTreshholdActive()&&noEditModeNoSim);
    ui->qqSegmentWidth->setEnabled((it!=NULL)&&it->getDisplaySegment()&&noEditModeNoSim);
    if (it!=NULL)
    {
        ui->qqExplicitHandling->setChecked(it->getExplicitHandling());
        ui->qqUseThreshold->setChecked(it->getTreshholdActive());
        ui->qqThreshold->setText(tt::getEString(false,it->getTreshhold(),2).c_str());
        ui->qqDisplaySegment->setChecked(it->getDisplaySegment());
        ui->qqSegmentWidth->setText(tt::getIString(false,it->getSegmentWidth()).c_str());
    }
    else
    {
        ui->qqExplicitHandling->setChecked(false);
        ui->qqUseThreshold->setChecked(false);
        ui->qqThreshold->setText("");
        ui->qqDisplaySegment->setChecked(false);
        ui->qqSegmentWidth->setText("");
    }
    selectLineEdit(lineEditToSelect);
}

void CQDlgDistances::updateObjectsInList()
{
    bool noEditModeNoSim=(App::getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped();
    ui->qqDistanceList->setEnabled(noEditModeNoSim);
    ui->qqDistanceList->clear();
    for (size_t i=0;i<App::ct->distances->distanceObjects.size();i++)
    {
        CRegDist* it=App::ct->distances->distanceObjects[i];
        std::string tmp=it->getObjectPartnersName();
        int id=it->getObjectID();
        QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
        itm->setData(Qt::UserRole,QVariant(id));
        itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
        ui->qqDistanceList->addItem(itm);
    }
}

void CQDlgDistances::on_qqAddNewObject_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        CQDlgEntityVsEntitySelection theDialog(this);
        theDialog.initialize(1);
        if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
        {
            App::appendSimulationThreadCommand(ADD_NEW_DISTANCEGUITRIGGEREDCMD,theDialog.entity1,theDialog.entity2);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

int CQDlgDistances::getSelectedObjectID()
{
    QList<QListWidgetItem*> sel=ui->qqDistanceList->selectedItems();
    if (sel.size()>0)
        return(sel.at(0)->data(Qt::UserRole).toInt());
    return(-1);
}

void CQDlgDistances::selectObjectInList(int objectID)
{
    for (int i=0;i<ui->qqDistanceList->count();i++)
    {
        QListWidgetItem* it=ui->qqDistanceList->item(i);
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

void CQDlgDistances::on_qqDistanceList_itemSelectionChanged()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        int objID=getSelectedObjectID();
        CRegDist* dist=App::ct->distances->getObject(objID);
        if (dist!=NULL)
            ((CEditBoxDelegate*)ui->qqDistanceList->itemDelegate())->initialText=dist->getObjectName();
        else
            ((CEditBoxDelegate*)ui->qqDistanceList->itemDelegate())->initialText="";
        inSelectionRoutine=true;
        refresh();
        inSelectionRoutine=false;
    }
}

void CQDlgDistances::on_qqDistanceList_itemChanged(QListWidgetItem *item)
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (item!=NULL)
        {
            std::string newName(item->text().toStdString());
            CRegDist* it=App::ct->distances->getObject(item->data(Qt::UserRole).toInt());
            if ( (it!=NULL)&&(newName!="") )
            {
                if (it->getObjectName()!=newName)
                {
                    tt::removeIllegalCharacters(newName,true);
                    if (App::ct->distances->getObject(newName)==NULL)
                    {
                        App::appendSimulationThreadCommand(SET_OBJECTNAME_DISTANCEGUITRIGGEREDCMD,it->getObjectID(),-1,0.0,0.0,newName.c_str());
                        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
                    }
                }
            }
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgDistances::onDeletePressed()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        if (focusWidget()==ui->qqDistanceList)
        {
            int objID=getSelectedObjectID();
            if (objID!=-1)
            {
                App::appendSimulationThreadCommand(DELETE_OBJECT_DISTANCEGUITRIGGEREDCMD,objID);
                App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
                App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
            }
        }
    }
}

void CQDlgDistances::on_qqEnableAll_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_ENABLE_ALL_DISTANCEGUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgDistances::on_qqExplicitHandling_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CRegDist* it=App::ct->distances->getObject(getSelectedObjectID());
        if (it!=NULL)
        {
            App::appendSimulationThreadCommand(TOGGLE_EXPLICITHANDLING_DISTANCEGUITRIGGEREDCMD,it->getObjectID());
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}


void CQDlgDistances::on_qqUseThreshold_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CRegDist* it=App::ct->distances->getObject(getSelectedObjectID());
        if (it!=NULL)
        {
            App::appendSimulationThreadCommand(TOGGLE_USETHRESHOLD_DISTANCEGUITRIGGEREDCMD,it->getObjectID());
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgDistances::on_qqThreshold_editingFinished()
{
    if (!ui->qqThreshold->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        CRegDist* it=App::ct->distances->getObject(getSelectedObjectID());
        if (it!=NULL)
        {
            bool ok;
            float newVal=ui->qqThreshold->text().toFloat(&ok);
            if (ok)
            {
                App::appendSimulationThreadCommand(SET_THRESHOLD_DISTANCEGUITRIGGEREDCMD,it->getObjectID(),-1,newVal);
                App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            }
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgDistances::on_qqDisplaySegment_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CRegDist* it=App::ct->distances->getObject(getSelectedObjectID());
        if (it!=NULL)
        {
            App::appendSimulationThreadCommand(TOGGLE_SEGMENTDISPLAY_DISTANCEGUITRIGGEREDCMD,it->getObjectID());
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgDistances::on_qqSegmentWidth_editingFinished()
{
    if (!ui->qqSegmentWidth->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        CRegDist* it=App::ct->distances->getObject(getSelectedObjectID());
        if (it!=NULL)
        {
            bool ok;
            int newVal=ui->qqSegmentWidth->text().toInt(&ok);
            if (ok)
            {
                App::appendSimulationThreadCommand(SET_SEGMENTWIDTH_DISTANCEGUITRIGGEREDCMD,it->getObjectID(),-1,newVal);
                App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            }
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgDistances::on_qqAdjustSegmentColor_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CQDlgMaterial::displayMaterialDlg(COLOR_ID_DISTANCESEGMENT,getSelectedObjectID(),-1,App::mainWindow);
    }
}
