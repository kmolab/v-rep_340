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
#include "qdlgmirrors.h"
#include "ui_qdlgmirrors.h"
#include "tt.h"
#include "gV.h"
#include "qdlgmaterial.h"
#include "qdlgcolor.h"
#include "app.h"
#include "v_repStrings.h"

CQDlgMirrors::CQDlgMirrors(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgMirrors)
{
    _dlgType=MIRROR_DLG;
    ui->setupUi(this);
    inMainRefreshRoutine=false;
}

CQDlgMirrors::~CQDlgMirrors()
{
    delete ui;
}

void CQDlgMirrors::cancelEvent()
{
    // we override this cancel event. The container window should close, not this one!!
    App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgMirrors::refresh()
{
    inMainRefreshRoutine=true;
    QLineEdit* lineEditToSelect=getSelectedLineEdit();
    bool noEditMode=App::getEditModeType()==NO_EDIT_MODE;
    bool noEditModeAndNoSim=noEditMode&&App::ct->simulation->isSimulationStopped();

    CMirror* it=App::ct->objCont->getLastSelection_mirror();

    ui->qqDisableAllMirrors->setEnabled(noEditMode);
    ui->qqDisableAllClippingPlanes->setEnabled(noEditMode);

    ui->qqDisableAllMirrors->setChecked(App::ct->mainSettings->mirrorsDisabled);
    ui->qqDisableAllClippingPlanes->setChecked(App::ct->mainSettings->clippingPlanesDisabled);


    ui->qqEnabled->setEnabled((it!=NULL)&&noEditModeAndNoSim);
    ui->qqWidth->setEnabled((it!=NULL)&&noEditModeAndNoSim);
    ui->qqHeight->setEnabled((it!=NULL)&&noEditModeAndNoSim);
    ui->qqReflectance->setEnabled((it!=NULL)&&it->getIsMirror()&&noEditModeAndNoSim);
    ui->qqColor->setEnabled((it!=NULL)&&noEditModeAndNoSim);

    ui->qqIsMirror->setEnabled((it!=NULL)&&noEditModeAndNoSim);
    ui->qqIsClippingPlane->setEnabled((it!=NULL)&&noEditModeAndNoSim);

    ui->qqEntityCombo->setEnabled((it!=NULL)&&noEditModeAndNoSim);
    ui->qqEntityCombo->clear();

    if (it!=NULL)
    {
        ui->qqEnabled->setChecked(it->getActive()&&noEditModeAndNoSim);

        ui->qqWidth->setText(tt::getFString(false,it->getMirrorWidth(),3).c_str());
        ui->qqHeight->setText(tt::getFString(false,it->getMirrorHeight(),3).c_str());
        ui->qqReflectance->setText(tt::getFString(false,it->getReflectance(),2).c_str());
        ui->qqIsMirror->setChecked(it->getIsMirror());
        ui->qqIsClippingPlane->setChecked(!it->getIsMirror());

        if (!it->getIsMirror())
        {

            ui->qqEntityCombo->addItem(strTranslate(IDS_ALL_VISIBLE_OBJECTS_IN_SCENE),QVariant(-1));

            std::vector<std::string> names;
            std::vector<int> ids;

            // Now collections:
            for (int i=0;i<int(App::ct->collections->allCollections.size());i++)
            {
                CRegCollection* it=App::ct->collections->allCollections[i];
                std::string name(tt::decorateString("[",strTranslate(IDSN_COLLECTION),"] "));
                name+=it->getCollectionName();
                names.push_back(name);
                ids.push_back(it->getCollectionID());
            }
            tt::orderStrings(names,ids);
            for (int i=0;i<int(names.size());i++)
                ui->qqEntityCombo->addItem(names[i].c_str(),QVariant(ids[i]));

            names.clear();
            ids.clear();

            // Now objects:
            for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
            {
                C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
                std::string name(tt::decorateString("[",strTranslate(IDS_OBJECT),"] "));
                name+=it->getName();
                names.push_back(name);
                ids.push_back(it->getID());
            }
            tt::orderStrings(names,ids);
            for (int i=0;i<int(names.size());i++)
                ui->qqEntityCombo->addItem(names[i].c_str(),QVariant(ids[i]));

            // Select current item:
            for (int i=0;i<ui->qqEntityCombo->count();i++)
            {
                if (ui->qqEntityCombo->itemData(i).toInt()==it->getClippingObjectOrCollection())
                {
                    ui->qqEntityCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
    }
    else
    {
        ui->qqEnabled->setChecked(false);
        ui->qqIsMirror->setChecked(true);
        ui->qqIsClippingPlane->setChecked(false);
        ui->qqWidth->setText("");
        ui->qqHeight->setText("");
        ui->qqReflectance->setText("");
    }

    selectLineEdit(lineEditToSelect);
    inMainRefreshRoutine=false;
}

void CQDlgMirrors::on_qqEnabled_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_ENABLED_MIRRORGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgMirrors::on_qqWidth_editingFinished()
{
    if (!ui->qqWidth->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqWidth->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_WIDTH_MIRRORGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgMirrors::on_qqHeight_editingFinished()
{
    if (!ui->qqHeight->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqHeight->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_HEIGHT_MIRRORGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgMirrors::on_qqColor_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CMirror* it=App::ct->objCont->getLastSelection_mirror();
        if (it!=NULL)
        {
            if (it->getIsMirror())
                CQDlgColor::displayDlg(COLOR_ID_MIRROR,App::ct->objCont->getLastSelectionID(),-1,0,App::mainWindow);
            else
                CQDlgMaterial::displayMaterialDlg(COLOR_ID_CLIPPINGPLANE,App::ct->objCont->getLastSelectionID(),-1,App::mainWindow);
        }
    }
}


void CQDlgMirrors::on_qqReflectance_editingFinished()
{
    if (!ui->qqReflectance->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqReflectance->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_REFLECTANCE_MIRRORGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgMirrors::on_qqIsMirror_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(SET_MIRRORFUNC_MIRRORGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgMirrors::on_qqIsClippingPlane_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(SET_CLIPPINGFUNC_MIRRORGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgMirrors::on_qqEntityCombo_currentIndexChanged(int index)
{
    if (!inMainRefreshRoutine)
    {
        IF_UI_EVENT_CAN_READ_DATA
        {
            int objID=ui->qqEntityCombo->itemData(ui->qqEntityCombo->currentIndex()).toInt();
            App::appendSimulationThreadCommand(SET_CLIPPINGENTITY_MIRRORGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),objID);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgMirrors::on_qqDisableAllMirrors_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_DISABLEALLMIRRORS_MIRRORGUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgMirrors::on_qqDisableAllClippingPlanes_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_DISABLEALLCLIPPING_MIRRORGUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}
