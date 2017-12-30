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
#include "qdlgfog.h"
#include "ui_qdlgfog.h"
#include "gV.h"
#include "tt.h"
#include "qdlgcolor.h"
#include "app.h"

CQDlgFog::CQDlgFog(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgFog)
{
    _dlgType=FOG_DLG;
    ui->setupUi(this);
}

CQDlgFog::~CQDlgFog()
{
    delete ui;
}

void CQDlgFog::refresh()
{
    QLineEdit* lineEditToSelect=getSelectedLineEdit();
    bool noEditModeNoSim=(App::getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped();

    ui->qqFogEnabled->setEnabled(noEditModeNoSim);
    ui->qqFogEnabled->setChecked(App::ct->environment->getFogEnabled());

    ui->qqLinear->setEnabled(App::ct->environment->getFogEnabled()&&noEditModeNoSim);
    ui->qqExp->setEnabled(App::ct->environment->getFogEnabled()&&noEditModeNoSim);
    ui->qqExp2->setEnabled(App::ct->environment->getFogEnabled()&&noEditModeNoSim);
    ui->qqStartDistance->setEnabled(App::ct->environment->getFogEnabled()&&(App::ct->environment->getFogType()==0)&&noEditModeNoSim);
    ui->qqEndDistance->setEnabled(App::ct->environment->getFogEnabled()&&(App::ct->environment->getFogType()==0)&&noEditModeNoSim);
    ui->qqDensity->setEnabled(App::ct->environment->getFogEnabled()&&(App::ct->environment->getFogType()!=0)&&noEditModeNoSim);
    ui->qqAdjustColor->setEnabled(App::ct->environment->getFogEnabled()&&noEditModeNoSim);

    ui->qqLinear->setChecked(App::ct->environment->getFogType()==0);
    ui->qqExp->setChecked(App::ct->environment->getFogType()==1);
    ui->qqExp2->setChecked(App::ct->environment->getFogType()==2);
    if (App::ct->environment->getFogType()!=0)
    {
        ui->qqStartDistance->setText("");
        ui->qqEndDistance->setText("");
        ui->qqDensity->setText(tt::getFString(false,App::ct->environment->getFogDensity(),2).c_str());
    }
    else
    {
        ui->qqStartDistance->setText(tt::getFString(false,App::ct->environment->getFogStart(),2).c_str());
        ui->qqEndDistance->setText(tt::getFString(false,App::ct->environment->getFogEnd(),2).c_str());
        ui->qqDensity->setText("");
    }

    selectLineEdit(lineEditToSelect);
}

void CQDlgFog::on_qqFogEnabled_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_ENABLED_FOGGUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgFog::on_qqLinear_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(SET_TYPE_FOGGUITRIGGEREDCMD,0);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgFog::on_qqExp_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(SET_TYPE_FOGGUITRIGGEREDCMD,1);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgFog::on_qqExp2_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(SET_TYPE_FOGGUITRIGGEREDCMD,2);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgFog::on_qqStartDistance_editingFinished()
{
    if (!ui->qqStartDistance->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqStartDistance->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_START_FOGGUITRIGGEREDCMD,-1,-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgFog::on_qqEndDistance_editingFinished()
{
    if (!ui->qqEndDistance->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqEndDistance->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_END_FOGGUITRIGGEREDCMD,-1,-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgFog::on_qqDensity_editingFinished()
{
    if (!ui->qqDensity->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqDensity->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_DENSITY_FOGGUITRIGGEREDCMD,-1,-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgFog::on_qqAdjustColor_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CQDlgColor::displayDlg(COLOR_ID_FOG,-1,-1,0,App::mainWindow);
    }
}
