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
#include "qdlgobjectmanipsettings.h"
#include "ui_qdlgobjectmanipsettings.h"
#include "gV.h"
#include "tt.h"
#include "app.h"
#include "v_repStrings.h"

bool CQDlgObjectManipSettings::forbidManipWhenRunning;
bool CQDlgObjectManipSettings::forbidManipWhenNotRunning;
int CQDlgObjectManipSettings::manipulationModePermission;
int CQDlgObjectManipSettings::manipulationTranslationRelativeTo;
float CQDlgObjectManipSettings::manipulationTranslationStepSize;
int CQDlgObjectManipSettings::manipulationRotationRelativeTo;
float CQDlgObjectManipSettings::manipulationRotationStepSize;

CQDlgObjectManipSettings::CQDlgObjectManipSettings(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgObjectManipSettings)
{
    _dlgType=OBJECT_MANIP_SETTINGS_DLG;
    ui->setupUi(this);
    lastLastSelectionID=-1;
}

CQDlgObjectManipSettings::~CQDlgObjectManipSettings()
{
    delete ui;
}

void CQDlgObjectManipSettings::cancelEvent()
{
    // we override this cancel event. The container window should close, not this one!!
    App::mainWindow->dlgCont->close(OBJECT_MANIP_SETTINGS_DLG);
}

void CQDlgObjectManipSettings::refresh()
{
    bool noEditModeAndNoSim=(App::getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped();
    int lastSelID=App::ct->objCont->getLastSelectionID();
    if (lastSelID!=-1)
    {
        C3DObject* object=App::ct->objCont->getLastSelection_object();
        forbidManipWhenRunning=object->getObjectManipulationDisabledDuringSimulation();
        forbidManipWhenNotRunning=object->getObjectManipulationDisabledDuringNonSimulation();
        manipulationModePermission=object->getObjectManipulationModePermissions();
        manipulationTranslationRelativeTo=object->getObjectManipulationTranslationRelativeTo();
        manipulationTranslationStepSize=object->getNonDefaultTranslationStepSize();
        manipulationRotationRelativeTo=object->getObjectManipulationRotationRelativeTo();
        manipulationRotationStepSize=object->getNonDefaultRotationStepSize();
    }
    lastLastSelectionID=lastSelID;
    bool mNotDisabled=(lastSelID!=-1);

    ui->qqDisabledWhenRunning->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqDisabledWhenNotRunning->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqPosWorld->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqPosParent->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqPosOwn->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqPosX->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqPosY->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqPosZ->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqPosCombo->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqOrWorld->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqOrParent->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqOrOwn->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqOrA->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqOrB->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqOrG->setEnabled(mNotDisabled&&noEditModeAndNoSim);
    ui->qqOrCombo->setEnabled(mNotDisabled&&noEditModeAndNoSim);

    ui->qqDisabledWhenRunning->setChecked(mNotDisabled&&forbidManipWhenRunning);
    ui->qqDisabledWhenNotRunning->setChecked(mNotDisabled&&forbidManipWhenNotRunning);

    ui->qqPosWorld->setChecked(mNotDisabled&&(manipulationTranslationRelativeTo==0));
    ui->qqPosParent->setChecked(mNotDisabled&&(manipulationTranslationRelativeTo==1));
    ui->qqPosOwn->setChecked(mNotDisabled&&(manipulationTranslationRelativeTo==2));

    ui->qqPosX->setChecked(mNotDisabled&&(manipulationModePermission&0x01));
    ui->qqPosY->setChecked(mNotDisabled&&(manipulationModePermission&0x02));
    ui->qqPosZ->setChecked(mNotDisabled&&(manipulationModePermission&0x04));

    ui->qqPosCombo->clear();
    if (mNotDisabled)
    {
        ui->qqPosCombo->addItem(IDS_DEFAULT,QVariant(0));

        ui->qqPosCombo->addItem(tt::getFString(false,0.001f,3).c_str(),QVariant(1));
        ui->qqPosCombo->addItem(tt::getFString(false,0.002f,3).c_str(),QVariant(2));
        ui->qqPosCombo->addItem(tt::getFString(false,0.005f,3).c_str(),QVariant(5));
        ui->qqPosCombo->addItem(tt::getFString(false,0.01f,3).c_str(),QVariant(10));
        ui->qqPosCombo->addItem(tt::getFString(false,0.025f,3).c_str(),QVariant(25));
        ui->qqPosCombo->addItem(tt::getFString(false,0.05f,3).c_str(),QVariant(50));
        ui->qqPosCombo->addItem(tt::getFString(false,0.1f,3).c_str(),QVariant(100));
        ui->qqPosCombo->addItem(tt::getFString(false,0.25f,3).c_str(),QVariant(250));
        ui->qqPosCombo->addItem(tt::getFString(false,0.5f,3).c_str(),QVariant(500));
        _selectItemOfCombobox(ui->qqPosCombo,int((manipulationTranslationStepSize+0.0005f)*1000.0f));
    }

    ui->qqOrWorld->setChecked(mNotDisabled&&(manipulationRotationRelativeTo==0));
    ui->qqOrParent->setChecked(mNotDisabled&&(manipulationRotationRelativeTo==1));
    ui->qqOrOwn->setChecked(mNotDisabled&&(manipulationRotationRelativeTo==2));

    ui->qqOrA->setChecked(mNotDisabled&&(manipulationModePermission&0x008));
    ui->qqOrB->setChecked(mNotDisabled&&(manipulationModePermission&0x010));
    ui->qqOrG->setChecked(mNotDisabled&&(manipulationModePermission&0x020));

    ui->qqOrCombo->clear();
    if (mNotDisabled)
    {
        ui->qqOrCombo->addItem(IDS_DEFAULT,QVariant(0));
        ui->qqOrCombo->addItem(tt::getAngleFString(false,1.0f*degToRad,1).c_str(),QVariant(1000));
        ui->qqOrCombo->addItem(tt::getAngleFString(false,2.0f*degToRad,1).c_str(),QVariant(2000));
        ui->qqOrCombo->addItem(tt::getAngleFString(false,5.0f*degToRad,1).c_str(),QVariant(5000));
        ui->qqOrCombo->addItem(tt::getAngleFString(false,10.0f*degToRad,1).c_str(),QVariant(10000));
        ui->qqOrCombo->addItem(tt::getAngleFString(false,15.0f*degToRad,1).c_str(),QVariant(15000));
        ui->qqOrCombo->addItem(tt::getAngleFString(false,30.0f*degToRad,1).c_str(),QVariant(30000));
        ui->qqOrCombo->addItem(tt::getAngleFString(false,45.0f*degToRad,1).c_str(),QVariant(45000));
        _selectItemOfCombobox(ui->qqOrCombo,int((manipulationRotationStepSize*radToDeg+0.0005f)*1000.0f));
    }
}

void CQDlgObjectManipSettings::on_qqDisabledWhenNotRunning_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_DISABLEWHENNOTRUNNING_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqDisabledWhenRunning_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_DISABLEWHENRUNNING_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqPosWorld_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(SET_POSRELATIVETO_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),0);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqPosParent_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(SET_POSRELATIVETO_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),1);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqPosOwn_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(SET_POSRELATIVETO_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),2);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqPosX_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        C3DObject* object=App::ct->objCont->getLastSelection_object();
        if (object!=NULL)
        {
            int permission=object->getObjectManipulationModePermissions();
            permission=permission^0x01;
            int low=permission&0x07;
            int high=permission&56;
            if (low==0x07)
                low=3;
            if (low==0)
                low=1;
            permission=low+high;
            App::appendSimulationThreadCommand(SET_PERMISSIONS_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),permission);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqPosY_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        C3DObject* object=App::ct->objCont->getLastSelection_object();
        if (object!=NULL)
        {
            int permission=object->getObjectManipulationModePermissions();
            permission=permission^0x02;
            int low=permission&0x07;
            int high=permission&56;
            if (low==0x07)
                low=3;
            if (low==0)
                low=2;
            permission=low+high;
            App::appendSimulationThreadCommand(SET_PERMISSIONS_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),permission);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqPosZ_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        C3DObject* object=App::ct->objCont->getLastSelection_object();
        if (object!=NULL)
        {
            int permission=object->getObjectManipulationModePermissions();
            permission=permission^0x04;
            int low=permission&0x07;
            int high=permission&56;
            if (low==0x07)
                low=6;
            if (low==0)
                low=4;
            permission=low+high;
            App::appendSimulationThreadCommand(SET_PERMISSIONS_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),permission);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqPosCombo_activated(int index)
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(SET_POSSTEPSIZE_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,float(ui->qqPosCombo->itemData(index).toInt())/1000.0f);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqOrWorld_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(SET_ORRELATIVETO_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),0);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqOrParent_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(SET_ORRELATIVETO_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),1);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqOrOwn_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(SET_ORRELATIVETO_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),2);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqOrA_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        C3DObject* object=App::ct->objCont->getLastSelection_object();
        if (object!=NULL)
        {
            int permission=object->getObjectManipulationModePermissions();
            permission=(permission&0x07)|0x08;
            App::appendSimulationThreadCommand(SET_PERMISSIONS_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),permission);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqOrB_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        C3DObject* object=App::ct->objCont->getLastSelection_object();
        if (object!=NULL)
        {
            int permission=object->getObjectManipulationModePermissions();
            permission=(permission&0x07)|0x10;
            App::appendSimulationThreadCommand(SET_PERMISSIONS_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),permission);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqOrG_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        C3DObject* object=App::ct->objCont->getLastSelection_object();
        if (object!=NULL)
        {
            int permission=object->getObjectManipulationModePermissions();
            permission=(permission&0x07)|0x20;
            App::appendSimulationThreadCommand(SET_PERMISSIONS_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),permission);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::on_qqOrCombo_activated(int index)
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(SET_ORSTEPSIZE_OBJECTMANIPGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,float(ui->qqOrCombo->itemData(index).toInt())*degToRad/1000.0f);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgObjectManipSettings::_selectItemOfCombobox(QComboBox* theBox,int itemData)
{
    theBox->setCurrentIndex(_getIndexOfComboboxItemWithData(theBox,itemData));
}

int CQDlgObjectManipSettings::_getIndexOfComboboxItemWithData(QComboBox* theBox,int itemData)
{
    for (int i=0;i<theBox->count();i++)
    {
        if (theBox->itemData(i).toInt()==itemData)
            return(i);
    }
    return(-1);
}
