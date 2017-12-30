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
#include "qdlgmodelproperties.h"
#include "ui_qdlgmodelproperties.h"
#include "v_rep_internal.h"
#include "qdlgmodelthumbnailvisu.h"
#include "qdlgmodelthumbnail.h"
#include "app.h"
#include "tt.h"

CQDlgModelProperties::CQDlgModelProperties(QWidget *parent) :
    VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgModelProperties)
{
    ui->setupUi(this);
}

CQDlgModelProperties::~CQDlgModelProperties()
{
    delete ui;
}

void CQDlgModelProperties::cancelEvent()
{
    //defaultModalDialogEndRoutine(false);
}

void CQDlgModelProperties::okEvent()
{
    //defaultModalDialogEndRoutine(true);
}

void CQDlgModelProperties::refresh()
{
    int ovProp=modelBaseObject->getLocalModelProperty();
    ui->qqNotVisible->setChecked((ovProp&sim_modelproperty_not_visible)!=0);
    ui->qqNotCollidable->setChecked((ovProp&sim_modelproperty_not_collidable)!=0);
    ui->qqNotMeasurable->setChecked((ovProp&sim_modelproperty_not_measurable)!=0);
    ui->qqNotRenderable->setChecked((ovProp&sim_modelproperty_not_renderable)!=0);
    ui->qqNotCuttable->setChecked((ovProp&sim_modelproperty_not_cuttable)!=0);
    ui->qqNotDetectable->setChecked((ovProp&sim_modelproperty_not_detectable)!=0);
    ui->qqNotDynamic->setChecked((ovProp&sim_modelproperty_not_dynamic)!=0);
    ui->qqNotRespondable->setChecked((ovProp&sim_modelproperty_not_respondable)!=0);
    ui->qqScriptsInactive->setChecked((ovProp&sim_modelproperty_scripts_inactive)!=0);
    ui->qqNotInsideModelBBox->setChecked((ovProp&sim_modelproperty_not_showasinsidemodel)!=0);

    ui->qqAcknowledgments->setPlainText(modelBaseObject->getModelAcknowledgement().c_str());
}

void CQDlgModelProperties::on_qqSelectThumbnail_clicked()
{ // We don't set an undo point here, it is set when we close the dialog
    bool keepCurrentThumbnail=false;
    int modelBase=modelBaseObject->getID();
    while (true)
    {
        if (App::ct->environment->modelThumbnail_notSerializedHere.hasImage())
        { // we already have a thumbnail!
            CQDlgModelThumbnailVisu dlg(this);
            dlg.applyThumbnail(&App::ct->environment->modelThumbnail_notSerializedHere);
            keepCurrentThumbnail=(dlg.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL);
        }
        if (!keepCurrentThumbnail)
        {
            CQDlgModelThumbnail dlg(this);
            dlg.modelBaseDummyID=modelBase;
            dlg.initialize();
            dlg.actualizeBitmap();
            if (dlg.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
            {
                // We first apply the thumbnail in the UI thread scene (needed), then post a message for the sim thread
                App::ct->environment->modelThumbnail_notSerializedHere.copyFrom(&dlg.thumbnail);
                SSimulationThreadCommand cmd;
                cmd.cmdId=SET_THUMBNAIL_GUITRIGGEREDCMD;
                unsigned char* img=(unsigned char*)dlg.thumbnail.getPointerToUncompressedImage();
                for (size_t i=0;i<128*128*4;i++)
                    cmd.uint8Params.push_back(img[i]);
                App::appendSimulationThreadCommand(cmd);
                if (!dlg.thumbnailIsFromFile)
                    break;
            }
            else
                break;
        }
        else
            break;
    }
}

void CQDlgModelProperties::on_qqNotVisible_clicked()
{
    modelBaseObject->setLocalModelProperty(modelBaseObject->getLocalModelProperty()^sim_modelproperty_not_visible);
    refresh();
}

void CQDlgModelProperties::on_qqNotCollidable_clicked()
{
    modelBaseObject->setLocalModelProperty(modelBaseObject->getLocalModelProperty()^sim_modelproperty_not_collidable);
    refresh();
}

void CQDlgModelProperties::on_qqNotMeasurable_clicked()
{
    modelBaseObject->setLocalModelProperty(modelBaseObject->getLocalModelProperty()^sim_modelproperty_not_measurable);
    refresh();
}

void CQDlgModelProperties::on_qqNotRenderable_clicked()
{
    modelBaseObject->setLocalModelProperty(modelBaseObject->getLocalModelProperty()^sim_modelproperty_not_renderable);
    refresh();
}

void CQDlgModelProperties::on_qqNotCuttable_clicked()
{
    modelBaseObject->setLocalModelProperty(modelBaseObject->getLocalModelProperty()^sim_modelproperty_not_cuttable);
    refresh();
}

void CQDlgModelProperties::on_qqNotDetectable_clicked()
{
    modelBaseObject->setLocalModelProperty(modelBaseObject->getLocalModelProperty()^sim_modelproperty_not_detectable);
    refresh();
}

void CQDlgModelProperties::on_qqNotDynamic_clicked()
{
    modelBaseObject->setLocalModelProperty(modelBaseObject->getLocalModelProperty()^sim_modelproperty_not_dynamic);
    refresh();
}

void CQDlgModelProperties::on_qqNotRespondable_clicked()
{
    modelBaseObject->setLocalModelProperty(modelBaseObject->getLocalModelProperty()^sim_modelproperty_not_respondable);
    refresh();
}

void CQDlgModelProperties::on_qqScriptsInactive_clicked()
{
    modelBaseObject->setLocalModelProperty(modelBaseObject->getLocalModelProperty()^sim_modelproperty_scripts_inactive);
    refresh();
}

void CQDlgModelProperties::on_qqNotInsideModelBBox_clicked(bool checked)
{
    modelBaseObject->setLocalModelProperty(modelBaseObject->getLocalModelProperty()^sim_modelproperty_not_showasinsidemodel);
    refresh();
}

void CQDlgModelProperties::on_qqClose_clicked(QAbstractButton *button)
{
    std::string acknowledgment(ui->qqAcknowledgments->toPlainText().toStdString());
    tt::removeSpacesAndEmptyLinesAtBeginningAndEnd(acknowledgment);
    SSimulationThreadCommand cmd;
    cmd.cmdId=SET_OVERRIDEPROPANDACKNOWLEDGMENT_MODELGUITRIGGEREDCMD;
    cmd.intParams.push_back(modelBaseObject->getID());
    cmd.intParams.push_back(modelBaseObject->getLocalModelProperty());
    cmd.stringParams.push_back(acknowledgment.c_str());
    App::appendSimulationThreadCommand(cmd);
    App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    defaultModalDialogEndRoutine(true);
}
