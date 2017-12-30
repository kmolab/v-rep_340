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
#include "qdlgscaling.h"
#include "ui_qdlgscaling.h"
#include "app.h"
#include "sceneObjectOperations.h"
#include "tt.h"

CQDlgScaling::CQDlgScaling(QWidget *parent) :
    VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgScaling)
{
    ui->setupUi(this);
    inPlace=false;
    factor=2.0f;
}

CQDlgScaling::~CQDlgScaling()
{
    delete ui;
}

void CQDlgScaling::cancelEvent()
{
    defaultModalDialogEndRoutine(false);
}

void CQDlgScaling::okEvent()
{
    _doTheScaling();
    defaultModalDialogEndRoutine(true);
}

void CQDlgScaling::refresh()
{
    ui->qqInPlace->setChecked(inPlace);
    ui->qqScalingFactor->setText(tt::getEString(false,factor,3).c_str());
}

void CQDlgScaling::_doTheScaling()
{
    std::vector<int> sel;
    for (int i=0;i<App::ct->objCont->getSelSize();i++)
        sel.push_back(App::ct->objCont->getSelID(i));
    SSimulationThreadCommand cmd;
    cmd.cmdId=SCALE_SCALINGGUITRIGGEREDCMD;
    cmd.intParams.assign(sel.begin(),sel.end());
    cmd.floatParams.push_back(factor);
    cmd.boolParams.push_back(inPlace);
    App::appendSimulationThreadCommand(cmd);
    App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgScaling::on_qqInPlace_clicked()
{
    inPlace=!inPlace;
    refresh();
}

void CQDlgScaling::on_qqScalingFactor_editingFinished()
{
    if (!ui->qqScalingFactor->isModified())
        return;
    bool ok;
    float newVal=ui->qqScalingFactor->text().toFloat(&ok);
    if (ok)
    {
        tt::limitValue(0.001f,1000.0f,newVal);
        factor=newVal;
    }
    refresh();
}

void CQDlgScaling::on_qqOkCancel_accepted()
{
    _doTheScaling();
    defaultModalDialogEndRoutine(true);
}

void CQDlgScaling::on_qqOkCancel_rejected()
{
    defaultModalDialogEndRoutine(false);
}
