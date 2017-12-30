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
#include "qdlgmultishapeedition.h"
#include "ui_qdlgmultishapeedition.h"
#include "tt.h"
#include "gV.h"
#include "geometric.h"
#include "qdlgtextures.h"
#include "app.h"
#include "v_repStrings.h"
#include "qdlgmaterial.h"

CGeometric* theGeom=NULL;

CQDlgMultishapeEdition::CQDlgMultishapeEdition(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgMultishapeEdition)
{
    _dlgType=MULTISHAPE_EDITION_DLG;
    ui->setupUi(this);
}

CQDlgMultishapeEdition::~CQDlgMultishapeEdition()
{
    delete ui;
}

void CQDlgMultishapeEdition::cancelEvent()
{
    // we override this cancel event.
    SSimulationThreadCommand cmd;
    cmd.cmdId=ANY_EDIT_MODE_FINISH_AND_APPLY_CHANGES_EMCMD;
    App::appendSimulationThreadCommand(cmd);
}

void CQDlgMultishapeEdition::refresh()
{
    QLineEdit* lineEditToSelect=getSelectedLineEdit();
    theGeom=NULL;
    if ( App::mainWindow->editModeContainer->getMultishapeEditMode()->isCurrentMultishapeGeometricComponentValid() )
        theGeom=App::mainWindow->editModeContainer->getMultishapeEditMode()->getCurrentMultishapeGeometricComponent();

    ui->qqShadingAngle->setEnabled(theGeom!=NULL);
    ui->qqEdgesAngle->setEnabled(theGeom!=NULL);
    ui->qqBackfaceCulling->setEnabled(theGeom!=NULL);
    ui->qqWireframe->setEnabled(theGeom!=NULL);
    ui->qqShowEdges->setEnabled(theGeom!=NULL);
    ui->qqHiddenBorder->setEnabled(theGeom!=NULL);
    ui->qqAdjustColor->setEnabled(theGeom!=NULL);
    ui->qqTexture->setEnabled(theGeom!=NULL);

    if (theGeom!=NULL)
    {
        ui->qqShadingAngle->setText(tt::getAngleFString(false,theGeom->getGouraudShadingAngle(),1).c_str());
        ui->qqEdgesAngle->setText(tt::getAngleFString(false,theGeom->getEdgeThresholdAngle(),1).c_str());
        ui->qqBackfaceCulling->setChecked(theGeom->getCulling());
        ui->qqWireframe->setChecked(theGeom->getWireframe());
        ui->qqShowEdges->setChecked(theGeom->getVisibleEdges());
        ui->qqHiddenBorder->setChecked(theGeom->getHideEdgeBorders());
    }
    else
    {
        ui->qqShadingAngle->setText("");
        ui->qqEdgesAngle->setText("");
        ui->qqBackfaceCulling->setChecked(false);
        ui->qqWireframe->setChecked(false);
        ui->qqShowEdges->setChecked(false);
        ui->qqHiddenBorder->setChecked(false);
    }
    selectLineEdit(lineEditToSelect);
}

void CQDlgMultishapeEdition::on_qqBackfaceCulling_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (theGeom==NULL)
            return;
        CShape* shape=App::mainWindow->editModeContainer->getMultishapeEditMode()->getEditModeMultishape();
        int index=App::mainWindow->editModeContainer->getMultishapeEditMode()->getMultishapeGeometricComponentIndex();
        if (shape!=NULL)
        {
            App::appendSimulationThreadCommand(TOGGLE_BACKFACECULLING_MULTISHAPEEDITIONGUITRIGGEREDCMD,shape->getID(),index);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgMultishapeEdition::on_qqWireframe_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (theGeom==NULL)
            return;
        CShape* shape=App::mainWindow->editModeContainer->getMultishapeEditMode()->getEditModeMultishape();
        int index=App::mainWindow->editModeContainer->getMultishapeEditMode()->getMultishapeGeometricComponentIndex();
        if (shape!=NULL)
        {
            App::appendSimulationThreadCommand(TOGGLE_WIREFRAME_MULTISHAPEEDITIONGUITRIGGEREDCMD,shape->getID(),index);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgMultishapeEdition::on_qqShowEdges_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (theGeom==NULL)
            return;
        CShape* shape=App::mainWindow->editModeContainer->getMultishapeEditMode()->getEditModeMultishape();
        int index=App::mainWindow->editModeContainer->getMultishapeEditMode()->getMultishapeGeometricComponentIndex();
        if (shape!=NULL)
        {
            App::appendSimulationThreadCommand(TOGGLE_SHOWEDGES_MULTISHAPEEDITIONGUITRIGGEREDCMD,shape->getID(),index);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgMultishapeEdition::on_qqShadingAngle_editingFinished()
{
    if (!ui->qqShadingAngle->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (theGeom==NULL)
            return;
        bool ok;
        float newVal=ui->qqShadingAngle->text().toFloat(&ok);
        CShape* shape=App::mainWindow->editModeContainer->getMultishapeEditMode()->getEditModeMultishape();
        int index=App::mainWindow->editModeContainer->getMultishapeEditMode()->getMultishapeGeometricComponentIndex();
        if ((shape!=NULL)&&ok)
        {
            App::appendSimulationThreadCommand(SET_SHADINGANGLE_MULTISHAPEEDITIONGUITRIGGEREDCMD,shape->getID(),index,gv::userToRad*newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgMultishapeEdition::on_qqAdjustColor_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (theGeom==NULL)
            return;
        CQDlgMaterial::displayMaterialDlg(COLOR_ID_SHAPE_GEOMETRY,App::mainWindow->editModeContainer->getEditModeObjectID(),App::mainWindow->editModeContainer->getMultishapeEditMode()->getMultishapeGeometricComponentIndex(),App::mainWindow);
    }
}

void CQDlgMultishapeEdition::on_qqTexture_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (theGeom==NULL)
            return;
        CQDlgTextures::displayDlg(TEXTURE_ID_COMPOUND_SHAPE,App::mainWindow->editModeContainer->getEditModeObjectID(),App::mainWindow->editModeContainer->getMultishapeEditMode()->getMultishapeGeometricComponentIndex(),App::mainWindow);
    }
}

void CQDlgMultishapeEdition::on_qqEdgesAngle_editingFinished()
{
    if (!ui->qqEdgesAngle->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (theGeom==NULL)
            return;
        bool ok;
        float newVal=ui->qqEdgesAngle->text().toFloat(&ok);
        CShape* shape=App::mainWindow->editModeContainer->getMultishapeEditMode()->getEditModeMultishape();
        int index=App::mainWindow->editModeContainer->getMultishapeEditMode()->getMultishapeGeometricComponentIndex();
        if ((shape!=NULL)&&ok)
        {
            App::appendSimulationThreadCommand(SET_EDGEANGLE_MULTISHAPEEDITIONGUITRIGGEREDCMD,shape->getID(),index,gv::userToRad*newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgMultishapeEdition::on_qqHiddenBorder_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (theGeom==NULL)
            return;
        CShape* shape=App::mainWindow->editModeContainer->getMultishapeEditMode()->getEditModeMultishape();
        int index=App::mainWindow->editModeContainer->getMultishapeEditMode()->getMultishapeGeometricComponentIndex();
        if (shape!=NULL)
        {
            App::appendSimulationThreadCommand(TOGGLE_HIDDENBORDER_MULTISHAPEEDITIONGUITRIGGEREDCMD,shape->getID(),index);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}
