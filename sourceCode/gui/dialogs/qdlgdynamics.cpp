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
#include "qdlgdynamics.h"
#include "ui_qdlgdynamics.h"
#include "tt.h"
#include "app.h"
#include "gV.h"
#include "propBrowser_engineProp_general.h"
#include "v_repStrings.h"

CQDlgDynamics::CQDlgDynamics(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgDynamics)
{
    _dlgType=DYNAMICS_DLG;
    ui->setupUi(this);
    inMainRefreshRoutine=false;
}

CQDlgDynamics::~CQDlgDynamics()
{
    delete ui;
}

void CQDlgDynamics::cancelEvent()
{
    // we override this cancel event. The container window should close, not this one!!
    App::mainWindow->dlgCont->close(CALCULATION_DLG);
}

void CQDlgDynamics::refresh()
{
    inMainRefreshRoutine=true;
    QLineEdit* lineEditToSelect=getSelectedLineEdit();

    bool noEditModeNoSim=(App::getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped();
    bool noEditMode=(App::getEditModeType()==NO_EDIT_MODE);
    bool en=App::ct->dynamicsContainer->getDynamicsEnabled();

    ui->qqEngineCombo->setEnabled(noEditModeNoSim&&en);
    ui->qqContactPoints->setEnabled(noEditMode&&en);
    ui->qqAdjustEngine->setEnabled(noEditModeNoSim&&en);
    ui->qqGravityX->setEnabled(noEditMode&&en);
    ui->qqGravityY->setEnabled(noEditMode&&en);
    ui->qqGravityZ->setEnabled(noEditMode&&en);

    ui->qqEnabled->setChecked(en);
    ui->qqEngineCombo->clear();
    ui->qqEngineCombo->addItem(strTranslate(IDS_BULLET_2_78),0);
    ui->qqEngineCombo->addItem(strTranslate(IDS_BULLET_2_83),1);
    ui->qqEngineCombo->addItem(strTranslate(IDS_ODE),2);
    ui->qqEngineCombo->addItem(strTranslate(IDS_VORTEX),3);
    ui->qqEngineCombo->addItem(strTranslate(IDS_NEWTON),4);
    int ver;
    int eng=App::ct->dynamicsContainer->getDynamicEngineType(&ver);
    if ( (eng==sim_physics_bullet)&&(ver==0) )
        ui->qqEngineCombo->setCurrentIndex(0);
    if ( (eng==sim_physics_bullet)&&(ver==283) )
        ui->qqEngineCombo->setCurrentIndex(1);
    if (eng==sim_physics_ode)
        ui->qqEngineCombo->setCurrentIndex(2);
    if (eng==sim_physics_vortex)
        ui->qqEngineCombo->setCurrentIndex(3);
    if (eng==sim_physics_newton)
        ui->qqEngineCombo->setCurrentIndex(4);

    ui->qqContactPoints->setChecked(App::ct->dynamicsContainer->getDisplayContactPoints());

    C3Vector accel(App::ct->dynamicsContainer->getGravity());
    ui->qqGravityX->setText(tt::getEString(true,accel(0),2).c_str());
    ui->qqGravityY->setText(tt::getEString(true,accel(1),2).c_str());
    ui->qqGravityZ->setText(tt::getEString(true,accel(2),2).c_str());

    selectLineEdit(lineEditToSelect);
    inMainRefreshRoutine=false;
}

void CQDlgDynamics::on_qqEnabled_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_DYNAMICS_DYNAMICSGUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgDynamics::on_qqEngineCombo_currentIndexChanged(int index)
{
    if (!inMainRefreshRoutine)
    {
        IF_UI_EVENT_CAN_READ_DATA
        {
            int index=ui->qqEngineCombo->itemData(ui->qqEngineCombo->currentIndex()).toInt();
            int ver=0;
            int eng=0;
            if ((index==0)||(index==1))
                eng=sim_physics_bullet;
            if (index==1)
                ver=283;
            if (index==2)
                eng=sim_physics_ode;
            if (index==3)
                eng=sim_physics_vortex;
            if (index==4)
                eng=sim_physics_newton;

            App::appendSimulationThreadCommand(SET_ENGINE_DYNAMICSGUITRIGGEREDCMD,eng,ver);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(REFRESH_TOOLBARS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgDynamics::on_qqContactPoints_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_DISPLAYCONTACTS_DYNAMICSGUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgDynamics::on_qqAdjustEngine_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        CPropBrowserEngineGeneral dlg(this);
        dlg.setModal(true);
        dlg.exec(); // items are set in here
        SSimulationThreadCommand cmd;
        cmd.cmdId=SET_ALLGLOBALPARAMS_DYNAMICSGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->dynamicsContainer->getUseDynamicDefaultCalculationParameters());
        std::vector<int> iParams;
        std::vector<float> fParams;
        App::ct->dynamicsContainer->getBulletIntParams(iParams);
        App::ct->dynamicsContainer->getBulletFloatParams(fParams);
        cmd.intVectorParams.push_back(iParams);
        cmd.floatVectorParams.push_back(fParams);
        iParams.clear();
        fParams.clear();
        App::ct->dynamicsContainer->getOdeIntParams(iParams);
        App::ct->dynamicsContainer->getOdeFloatParams(fParams);
        cmd.intVectorParams.push_back(iParams);
        cmd.floatVectorParams.push_back(fParams);
        iParams.clear();
        fParams.clear();
        App::ct->dynamicsContainer->getVortexIntParams(iParams);
        App::ct->dynamicsContainer->getVortexFloatParams(fParams);
        cmd.intVectorParams.push_back(iParams);
        cmd.floatVectorParams.push_back(fParams);
        iParams.clear();
        fParams.clear();
        App::ct->dynamicsContainer->getNewtonIntParams(iParams);
        App::ct->dynamicsContainer->getNewtonFloatParams(fParams);
        cmd.intVectorParams.push_back(iParams);
        cmd.floatVectorParams.push_back(fParams);
        iParams.clear();
        fParams.clear();
        App::appendSimulationThreadCommand(cmd);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgDynamics::on_qqGravityX_editingFinished()
{
    if (!ui->qqGravityX->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqGravityX->text().toFloat(&ok);
        if (ok)
        {
            C3Vector vect=App::ct->dynamicsContainer->getGravity();
            vect(0)=newVal;
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_GRAVITY_DYNAMICSGUITRIGGEREDCMD;
            cmd.posParams.push_back(vect);
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgDynamics::on_qqGravityY_editingFinished()
{
    if (!ui->qqGravityY->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqGravityY->text().toFloat(&ok);
        if (ok)
        {
            C3Vector vect=App::ct->dynamicsContainer->getGravity();
            vect(1)=newVal;
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_GRAVITY_DYNAMICSGUITRIGGEREDCMD;
            cmd.posParams.push_back(vect);
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgDynamics::on_qqGravityZ_editingFinished()
{
    if (!ui->qqGravityZ->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqGravityZ->text().toFloat(&ok);
        if (ok)
        {
            C3Vector vect=App::ct->dynamicsContainer->getGravity();
            vect(2)=newVal;
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_GRAVITY_DYNAMICSGUITRIGGEREDCMD;
            cmd.posParams.push_back(vect);
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}
