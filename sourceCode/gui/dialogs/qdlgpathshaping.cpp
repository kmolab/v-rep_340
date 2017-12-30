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
#include "qdlgpathshaping.h"
#include "ui_qdlgpathshaping.h"
#include "gV.h"
#include "tt.h"
#include "qdlgmaterial.h"
#include "app.h"
#include "v_repStrings.h"

bool CQDlgPathShaping::showWindow=false;

CQDlgPathShaping::CQDlgPathShaping(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgPathShaping)
{
    _dlgType=PATH_SHAPING_DLG;
    ui->setupUi(this);
    inMainRefreshRoutine=false;
}

CQDlgPathShaping::~CQDlgPathShaping()
{
    delete ui;
}

void CQDlgPathShaping::cancelEvent()
{ // no cancel event allowed
    showWindow=false;
    CDlgEx::cancelEvent();
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgPathShaping::refresh()
{
    inMainRefreshRoutine=true;
    QLineEdit* lineEditToSelect=getSelectedLineEdit();
    bool noEditModeNoSim=(App::getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped();

    int sectionType=0;
    CPath* it=App::ct->objCont->getLastSelection_path();
    bool en=false;
    if (it!=NULL)
    {
        it=App::ct->objCont->getLastSelection_path();
        en=it->getShapingEnabled();
        sectionType=it->getShapingType();
    }

    ui->qqEnabled->setEnabled(noEditModeNoSim&&(it!=NULL));

    ui->qqFollowOrientation->setEnabled(noEditModeNoSim&&(it!=NULL)&&en);
//  ui->qqConvexHull->setEnabled(noEditModeNoSim&&(it!=NULL)&&en);
    ui->qqAdjustColor->setEnabled(noEditModeNoSim&&(it!=NULL)&&en);
    ui->qqGenerateShape->setEnabled(noEditModeNoSim&&(it!=NULL)&&en);
    ui->qqMaxLength->setEnabled(noEditModeNoSim&&(it!=NULL)&&en);

    ui->qqTypeCombo->setEnabled(noEditModeNoSim&&(it!=NULL)&&en);
    ui->qqCyclic->setEnabled(noEditModeNoSim&&(it!=NULL)&&en&&(sectionType==0));
    ui->qqScalingFactor->setEnabled(noEditModeNoSim&&(it!=NULL)&&en);
    ui->qqCoordinates->setEnabled(noEditModeNoSim&&(it!=NULL)&&en&&(sectionType==0));

    ui->qqEnabled->setChecked(noEditModeNoSim&&(it!=NULL)&&it->getShapingEnabled());
    ui->qqFollowOrientation->setChecked((it!=NULL)&&it->getShapingFollowFullOrientation());
//  ui->qqConvexHull->setChecked((it!=NULL)&&it->getShapingThroughConvexHull());

    ui->qqTypeCombo->clear();
    ui->qqCyclic->setChecked((it!=NULL)&&it->getShapingSectionClosed());

    if (it!=NULL)
    {
        ui->qqMaxLength->setText(tt::getFString(false,it->getShapingElementMaxLength(),3).c_str());
        ui->qqScalingFactor->setText(tt::getFString(false,it->getShapingScaling(),3).c_str());

        ui->qqTypeCombo->addItem(strTranslate(IDS_SHAPING_CIRCLE),QVariant(1));
        ui->qqTypeCombo->addItem(strTranslate(IDS_SHAPING_SQUARE),QVariant(2));
        ui->qqTypeCombo->addItem(strTranslate(IDS_SHAPING_HORIZONTAL_SEGMENT),QVariant(3));
        ui->qqTypeCombo->addItem(strTranslate(IDS_SHAPING_VERTICAL_SEGMENT),QVariant(4));
        ui->qqTypeCombo->addItem(strTranslate(IDS_SHAPING_USER_DEFINED),QVariant(0));
        for (int i=0;i<ui->qqTypeCombo->count();i++)
        {
            if (ui->qqTypeCombo->itemData(i).toInt()==it->getShapingType())
            {
                ui->qqTypeCombo->setCurrentIndex(i);
                break;
            }
        }
        ui->qqCoordinates->clear();
        ui->qqCoordinates->setPlainText("");

        std::string tmp;
        for (int i=0;i<int(it->shapingCoordinates.size())/2;i++)
        {
            tmp+=tt::getFString(true,it->shapingCoordinates[2*i+0],3)+",";
            if (i<int(it->shapingCoordinates.size())/2-1)
            {
                tmp+=tt::getFString(true,it->shapingCoordinates[2*i+1],3)+",";
                tmp+=char(13);
                tmp+=char(10);
            }
            else
                tmp+=tt::getFString(true,it->shapingCoordinates[2*i+1],3);
        }
        ui->qqCoordinates->setPlainText(tmp.c_str());

    }
    else
    {
        ui->qqMaxLength->setText("");
        ui->qqScalingFactor->setText("");
        ui->qqCoordinates->setPlainText("");
        ui->qqCoordinates->clear();
    }

    inMainRefreshRoutine=false;
}

void CQDlgPathShaping::on_qqEnabled_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_SHAPINGENABLED_PATHSHAPINGGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgPathShaping::on_qqFollowOrientation_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_FOLLOWORIENTATION_PATHSHAPINGGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}
/*
void CQDlgPathShaping::on_qqConvexHull_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CPath* it=App::ct->objCont->getLastSelection_path();
        if (it!=NULL)
        {
            if (!it->getShapingThroughConvexHull())
                App::appendSimulationThreadCommand(SHOW_PROGRESSDLGGUITRIGGEREDCMD,-1,-1,0.0,0.0,"Recomputing the path geometry...");
            App::appendSimulationThreadCommand(TOGGLE_CONVEXHULLS_PATHSHAPINGGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(HIDE_PROGRESSDLGGUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}
*/
void CQDlgPathShaping::on_qqAdjustColor_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CQDlgMaterial::displayMaterialDlg(COLOR_ID_PATH_SHAPING,App::ct->objCont->getLastSelectionID(),-1,App::mainWindow);
    }
}

void CQDlgPathShaping::on_qqGenerateShape_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(GENERATE_SHAPE_PATHSHAPINGGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgPathShaping::on_qqMaxLength_editingFinished()
{
    if (!ui->qqMaxLength->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqMaxLength->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_ELEMENTMAXLENGTH_PATHSHAPINGGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgPathShaping::on_qqTypeCombo_currentIndexChanged(int index)
{
    if (!inMainRefreshRoutine)
    {
        IF_UI_EVENT_CAN_READ_DATA
        {
            App::appendSimulationThreadCommand(SET_TYPE_PATHSHAPINGGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),ui->qqTypeCombo->itemData(ui->qqTypeCombo->currentIndex()).toInt());
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgPathShaping::on_qqCyclic_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_LASTCOORDLINKSTOFIRST_PATHSHAPINGGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgPathShaping::on_qqScalingFactor_editingFinished()
{
    if (!ui->qqScalingFactor->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqScalingFactor->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_SCALINGFACTOR_PATHSHAPINGGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgPathShaping::on_qqCoordinates_textChanged()
{
    if (!inMainRefreshRoutine)
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=SET_COORDINATES_PATHSHAPINGGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->objCont->getLastSelectionID());
        std::string str(ui->qqCoordinates->toPlainText().toStdString());
        float tmp;
        while (_extractOneFloat(str,tmp))
            cmd.floatParams.push_back(tmp);
        if (cmd.floatParams.size()&1)
            cmd.floatParams.push_back(0.0f); // no odd number!
        App::appendSimulationThreadCommand(cmd);
        //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        //App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

bool CQDlgPathShaping::_extractOneFloat(std::string& txt,float& val)
{
    if (txt=="")
        return(false);
    std::string nb;
    bool brokenOut=false;
    for (int i=0;i<int(txt.length());i++)
    {
        if (txt[i]!=',')
        {
            if ((txt[i]!=10)&&(txt[i]!=13))
                nb+=txt[i];
        }
        else
        {
            txt.erase(txt.begin(),txt.begin()+i+1);
            brokenOut=true;
            break;
        }
    }
    if (!brokenOut)
        txt="";
    if (!tt::getValidFloat(nb,val))
        val=0.0f;
    return(true);
}
