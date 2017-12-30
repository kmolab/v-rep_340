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
#include "qdlgcameras.h"
#include "ui_qdlgcameras.h"
#include "tt.h"
#include "gV.h"
#include "qdlgmaterial.h"
#include "app.h"
#include "v_repStrings.h"

CQDlgCameras::CQDlgCameras(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgCameras)
{
    _dlgType=CAMERA_DLG;
    ui->setupUi(this);
    inMainRefreshRoutine=false;
}

CQDlgCameras::~CQDlgCameras()
{
    delete ui;
}

void CQDlgCameras::cancelEvent()
{
    // we override this cancel event. The container window should close, not this one!!
    App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgCameras::refresh()
{
    inMainRefreshRoutine=true;
    QLineEdit* lineEditToSelect=getSelectedLineEdit();

    bool noEditModeNoSim=(App::getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped();
    CCamera* it=App::ct->objCont->getLastSelection_camera();

    ui->qqAlongX->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqAlongY->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqAlongZ->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqColorA->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqColorB->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqFarClipping->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqNearClipping->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqFullRotation->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqKeepHeadUp->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqLocalLights->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqAllowPicking->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqManipProxy->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqPerspectiveProjectionAngle->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqOrthographicProjectionSize->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqShowFog->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqSize->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqTilting->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqTrackedCombo->setEnabled((it!=NULL)&&noEditModeNoSim);
    ui->qqRenderModeCombo->setEnabled((it!=NULL)&&noEditModeNoSim);

    ui->qqTrackedCombo->clear();
    ui->qqRenderModeCombo->clear();

    if (it!=NULL)
    {
        ui->qqManipProxy->setChecked(it->getUseParentObjectAsManipulationProxy());
        ui->qqKeepHeadUp->setChecked((it->getCameraManipulationModePermissions()&0x020)!=0);
        ui->qqFullRotation->setChecked((it->getCameraManipulationModePermissions()&0x008)!=0);
        ui->qqTilting->setChecked((it->getCameraManipulationModePermissions()&0x010)!=0);
        ui->qqAlongX->setChecked((it->getCameraManipulationModePermissions()&0x001)!=0);
        ui->qqAlongY->setChecked((it->getCameraManipulationModePermissions()&0x002)!=0);
        ui->qqAlongZ->setChecked((it->getCameraManipulationModePermissions()&0x004)!=0);
        ui->qqPerspectiveProjectionAngle->setText(tt::getAngleFString(false,it->getViewAngle(),1).c_str());
        ui->qqOrthographicProjectionSize->setText(tt::getEString(false,it->getOrthoViewSize(),2).c_str());
        ui->qqSize->setText(tt::getEString(false,it->getCameraSize(),2).c_str());
        ui->qqNearClipping->setText(tt::getEString(false,it->getNearClippingPlane(),2).c_str());
        ui->qqFarClipping->setText(tt::getEString(false,it->getFarClippingPlane(),2).c_str());
        ui->qqShowFog->setChecked(it->getShowFogIfAvailable());
        ui->qqLocalLights->setChecked(it->getuseLocalLights());
        ui->qqAllowPicking->setChecked(it->getAllowPicking());

        ui->qqTrackedCombo->addItem(strTranslate(IDSN_NONE),QVariant(-1));
        std::vector<std::string> names;
        std::vector<int> ids;
        for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
        {
            C3DObject* it2=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
            if (it2!=it)
            {
                names.push_back(it2->getName());
                ids.push_back(it2->getID());
            }
        }
        tt::orderStrings(names,ids);
        for (int i=0;i<int(names.size());i++)
            ui->qqTrackedCombo->addItem(names[i].c_str(),QVariant(ids[i]));
        for (int i=0;i<ui->qqTrackedCombo->count();i++)
        {
            if (ui->qqTrackedCombo->itemData(i).toInt()==it->getTrackedObjectID())
            {
                ui->qqTrackedCombo->setCurrentIndex(i);
                break;
            }
        }

        ui->qqRenderModeCombo->addItem(strTranslate(IDS_VISIBLE_COMPONENTS),QVariant(0));
        ui->qqRenderModeCombo->addItem(strTranslate(IDS_RAY_TRACING_DURING_SIMULATION_AND_RECORDING),QVariant(1));
        ui->qqRenderModeCombo->addItem(strTranslate(IDS_RAY_TRACING_DURING_SIMULATION),QVariant(2));
        ui->qqRenderModeCombo->addItem(strTranslate(IDS_RAY_TRACING2_DURING_SIMULATION_AND_RECORDING),QVariant(3));
        ui->qqRenderModeCombo->addItem(strTranslate(IDS_RAY_TRACING2_DURING_SIMULATION),QVariant(4));
        ui->qqRenderModeCombo->addItem(strTranslate(IDS_EXTERNAL_RENDERER),QVariant(5));

        // Select current item:
        for (int i=0;i<ui->qqRenderModeCombo->count();i++)
        {
            if (ui->qqRenderModeCombo->itemData(i).toInt()==it->getRenderMode())
            {
                ui->qqRenderModeCombo->setCurrentIndex(i);
                break;
            }
        }

    }
    else
    {
        ui->qqAlongX->setChecked(false);
        ui->qqAlongY->setChecked(false);
        ui->qqAlongZ->setChecked(false);
        ui->qqFullRotation->setChecked(false);
        ui->qqKeepHeadUp->setChecked(false);
        ui->qqLocalLights->setChecked(false);
        ui->qqManipProxy->setChecked(false);
        ui->qqShowFog->setChecked(false);
        ui->qqTilting->setChecked(false);
        ui->qqAllowPicking->setChecked(false);
        ui->qqFarClipping->setText("");
        ui->qqNearClipping->setText("");
        ui->qqPerspectiveProjectionAngle->setText("");
        ui->qqOrthographicProjectionSize->setText("");
        ui->qqSize->setText("");
    }

    selectLineEdit(lineEditToSelect);
    inMainRefreshRoutine=false;
}

void CQDlgCameras::on_qqPerspectiveProjectionAngle_editingFinished()
{
    if (!ui->qqPerspectiveProjectionAngle->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqPerspectiveProjectionAngle->text().toFloat(&ok);
        if (ok)
        {
            float v=newVal*gv::userToRad;
            App::appendSimulationThreadCommand(SET_VIEW_ANGLE_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,v);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqOrthographicProjectionSize_editingFinished()
{
    if (!ui->qqOrthographicProjectionSize->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqOrthographicProjectionSize->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_ORTHO_VIEW_SIZE_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqNearClipping_editingFinished()
{
    if (!ui->qqNearClipping->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqNearClipping->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_NEAR_CLIPPING_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqFarClipping_editingFinished()
{
    if (!ui->qqFarClipping->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqFarClipping->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_FAR_CLIPPING_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqTrackedCombo_currentIndexChanged(int index)
{
    if (!inMainRefreshRoutine)
    {
        IF_UI_EVENT_CAN_WRITE_DATA
        {
            int objID=ui->qqTrackedCombo->itemData(ui->qqTrackedCombo->currentIndex()).toInt();
            App::appendSimulationThreadCommand(SET_TRACKED_OBJECT_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),objID);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgCameras::on_qqShowFog_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_SHOWFOG_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqKeepHeadUp_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_KEEPHEADUP_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqManipProxy_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_USEPARENTASMANIPPROXY_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqAlongX_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_SHIFTALONGX_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqAlongY_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_SHIFTALONGY_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqAlongZ_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_SHIFTALONGZ_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqFullRotation_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_FULLROTATION_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqTilting_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_TILTING_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqColorA_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CQDlgMaterial::displayMaterialDlg(COLOR_ID_CAMERA_A,App::ct->objCont->getLastSelectionID(),-1,App::mainWindow);
    }
}

void CQDlgCameras::on_qqColorB_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CQDlgMaterial::displayMaterialDlg(COLOR_ID_CAMERA_B,App::ct->objCont->getLastSelectionID(),-1,App::mainWindow);
    }
}

void CQDlgCameras::on_qqSize_editingFinished()
{
    if (!ui->qqSize->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqSize->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_SIZE_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}


void CQDlgCameras::on_qqLocalLights_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_LOCALLIGHTS_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqAllowPicking_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_ALLOWPICKING_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgCameras::on_qqRenderModeCombo_currentIndexChanged(int index)
{
    if (!inMainRefreshRoutine)
    {
        IF_UI_EVENT_CAN_WRITE_DATA
        {
            int renderMode=ui->qqRenderModeCombo->itemData(ui->qqRenderModeCombo->currentIndex()).toInt();
            App::appendSimulationThreadCommand(SET_RENDERMODE_CAMERAGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),renderMode);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}
