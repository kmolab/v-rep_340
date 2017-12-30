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
#include "qdlgsettings.h"
#include "ui_qdlgsettings.h"
#include "tt.h"
#include "gV.h"
#include "v_repStrings.h"
#include "vFileDialog.h"
#include "qdlgopenglsettings.h"
#include "app.h"
#include "vMessageBox.h"

CQDlgSettings::CQDlgSettings(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgSettings)
{
    _dlgType=SETTINGS_DLG;
    ui->setupUi(this);
}

CQDlgSettings::~CQDlgSettings()
{
    delete ui;
}

void CQDlgSettings::refresh()
{
    bool noEditMode=(App::getEditModeType()==NO_EDIT_MODE);
    bool noSim=App::ct->simulation->isSimulationStopped();
    bool noEditModeAndNoSim=noEditMode&&noSim;

#ifndef WIN_VREP
    ui->hideConsole->setVisible(false);
#endif

    ui->removeIdenticalVerticesCheckbox->setEnabled(noSim);
    ui->removeIdenticalVerticesTolerance->setEnabled(noSim);
    ui->removeIdenticalTriangles->setEnabled(noSim);
    ui->ignoreTriangleWinding->setEnabled(App::userSettings->identicalTrianglesCheck&&noSim);
    ui->antialiasing->setEnabled(noSim);
    ui->undoRedo->setEnabled(noEditModeAndNoSim);
    ui->qqAutoSave->setEnabled(noEditModeAndNoSim);
    ui->qqThreadedRendering->setEnabled(noEditModeAndNoSim&&(App::userSettings->threadedRenderingDuringSimulation>=0));
    ui->qqSelectScriptEditor->setEnabled(!App::userSettings->getUseBuiltInScriptEditorToSave()&&noEditModeAndNoSim);
    ui->qqAdjustOpenGl->setEnabled(noEditModeAndNoSim);
    ui->qqExternalScriptEditor->setEnabled(noEditModeAndNoSim);
    ui->qqScriptEditor->setEnabled(noEditModeAndNoSim);
    ui->qqAskWithIncludedScriptFiles->setEnabled(noEditModeAndNoSim);

    ui->translationStepSize->clear();
    ui->translationStepSize->addItem(tt::getFString(false,0.001f,3).c_str(),QVariant(1));
    ui->translationStepSize->addItem(tt::getFString(false,0.002f,3).c_str(),QVariant(2));
    ui->translationStepSize->addItem(tt::getFString(false,0.005f,3).c_str(),QVariant(5));
    ui->translationStepSize->addItem(tt::getFString(false,0.01f,3).c_str(),QVariant(10));
    ui->translationStepSize->addItem(tt::getFString(false,0.025f,3).c_str(),QVariant(25));
    ui->translationStepSize->addItem(tt::getFString(false,0.05f,3).c_str(),QVariant(50));
    ui->translationStepSize->addItem(tt::getFString(false,0.1f,3).c_str(),QVariant(100));
    ui->translationStepSize->addItem(tt::getFString(false,0.25f,3).c_str(),QVariant(250));
    ui->translationStepSize->addItem(tt::getFString(false,0.5f,3).c_str(),QVariant(500));
    _selectItemOfCombobox(ui->translationStepSize,int((App::userSettings->getTranslationStepSize()+0.0005f)*1000.0f));

    ui->rotationStepSize->clear();
    ui->rotationStepSize->addItem(tt::getAngleFString(false,1.0f*degToRad,1).c_str(),QVariant(1));
    ui->rotationStepSize->addItem(tt::getAngleFString(false,2.0f*degToRad,1).c_str(),QVariant(2));
    ui->rotationStepSize->addItem(tt::getAngleFString(false,5.0f*degToRad,1).c_str(),QVariant(5));
    ui->rotationStepSize->addItem(tt::getAngleFString(false,10.0f*degToRad,1).c_str(),QVariant(10));
    ui->rotationStepSize->addItem(tt::getAngleFString(false,15.0f*degToRad,1).c_str(),QVariant(15));
    ui->rotationStepSize->addItem(tt::getAngleFString(false,30.0f*degToRad,1).c_str(),QVariant(30));
    ui->rotationStepSize->addItem(tt::getAngleFString(false,45.0f*degToRad,1).c_str(),QVariant(45));
    _selectItemOfCombobox(ui->rotationStepSize,int(App::userSettings->getRotationStepSize()*radToDeg+0.5f));

    ui->removeIdenticalVerticesCheckbox->setChecked(App::userSettings->identicalVerticesCheck);
    ui->removeIdenticalVerticesTolerance->setText(tt::getEString(false,App::userSettings->identicalVerticesTolerance,2).c_str());

    ui->removeIdenticalTriangles->setChecked(App::userSettings->identicalTrianglesCheck);
    ui->ignoreTriangleWinding->setChecked(App::userSettings->identicalTrianglesWindingCheck);



    ui->antialiasing->setChecked(App::userSettings->antiAliasing);
    ui->worldReference->setChecked(App::userSettings->displayWorldReference);
    ui->boundingBoxDisplay->setChecked(App::userSettings->displayBoundingBoxeWhenObjectSelected);
    ui->undoRedo->setChecked(App::userSettings->getUndoRedoEnabled());
    ui->hideConsole->setChecked(!App::userSettings->alwaysShowConsole);
    ui->qqTransparentDlgs->setChecked(App::userSettings->allowTransparentDialogs);
    ui->qqAutoSave->setChecked(App::userSettings->autoSaveDelay!=0);

    ui->qqThreadedRendering->setChecked(App::userSettings->threadedRenderingDuringSimulation==1);
    ui->qqAskWithIncludedScriptFiles->setChecked(App::userSettings->askToIncludeScriptFiles);

    ui->qqExternalScriptEditor->setChecked(!App::userSettings->getUseBuiltInScriptEditorToSave());
    ui->qqScriptEditor->setText(App::userSettings->externalScriptEditorToSave.c_str());

    ui->qqAdjustOpenGl->setEnabled(App::ct->simulation->isSimulationStopped());
}

void CQDlgSettings::on_translationStepSize_activated(int index)
{
    App::appendSimulationThreadCommand(SET_TRANSLATIONSTEPSIZE_USERSETTINGSGUITRIGGEREDCMD,-1,-1,float(ui->translationStepSize->itemData(index).toInt())/1000.0f);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_rotationStepSize_activated(int index)
{
    App::appendSimulationThreadCommand(SET_ROTATIONSTEPSIZE_USERSETTINGSGUITRIGGEREDCMD,-1,-1,float(ui->rotationStepSize->itemData(index).toInt())*degToRad);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_removeIdenticalVerticesCheckbox_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_REMOVEIDENTICALVERTICES_USERSETTINGSGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_removeIdenticalVerticesTolerance_editingFinished()
{
    if (!ui->removeIdenticalVerticesTolerance->isModified())
        return;
    float newVal;
    bool ok;
    newVal=ui->removeIdenticalVerticesTolerance->text().toFloat(&ok);
    if (ok)
    {
        if (newVal>=0.0f)
        {
            if (newVal>0.5f)
                newVal=0.5f;
            App::appendSimulationThreadCommand(SET_IDENTICALVERTICESTOLERANCE_USERSETTINGSGUITRIGGEREDCMD,-1,-1,newVal);
        }
    }
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_removeIdenticalTriangles_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_REMOVEIDENTICALTRIANGLES_USERSETTINGSGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_ignoreTriangleWinding_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_IGNORETRIANGLEWINDING_USERSETTINGSGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_antialiasing_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_ANTIALIASING_USERSETTINGSGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_worldReference_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_SHOWWORLDREF_USERSETTINGSGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_boundingBoxDisplay_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_SHOWBOUNDINGBOX_USERSETTINGSGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_undoRedo_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_UNDOREDO_USERSETTINGSGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(REFRESH_TOOLBARS_GUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_hideConsole_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_HIDECONSOLE_USERSETTINGSGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_qqTransparentDlgs_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_TOGGLETRANSPARENTDLGS_USERSETTINGSGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_qqAutoSave_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_AUTOSAVE_USERSETTINGSGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_qqExternalScriptEditor_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_EXTERNALSCRIPTEDITOR_USERSETTINGSGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_EXTERNAL_SCRIPT_EDITOR),strTranslate(IDSN_EXTERNAL_EDITOR_INFO),VMESSAGEBOX_OKELI);
}

void CQDlgSettings::on_qqSelectScriptEditor_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        std::string initPath=App::directories->getPathFromFull(App::userSettings->externalScriptEditorToSave);
        std::string filenameAndPath=App::uiThread->getOpenFileName(App::mainWindow,0,strTranslate(IDSN_EXTERNAL_SCRIPT_EDITOR),initPath,App::userSettings->externalScriptEditorToSave,false,"all","*");
        if (filenameAndPath.length()!=0)
            App::appendSimulationThreadCommand(SET_EXTERNALSCRIPTEDITOR_USERSETTINGSGUITRIGGEREDCMD,-1,-1,0,0,filenameAndPath.c_str());
        App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_EXTERNAL_SCRIPT_EDITOR),strTranslate(IDSN_EXTERNAL_EDITOR_INFO),VMESSAGEBOX_OKELI);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgSettings::on_qqThreadedRendering_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_THREADEDRENDERING_USERSETTINGSGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgSettings::on_qqAdjustOpenGl_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CQDlgOpenGlSettings theDialog(this);

        theDialog.offscreenContextType=App::userSettings->offscreenContextType;
        theDialog.fboType=App::userSettings->fboType;
        theDialog.vboOperation=App::userSettings->vboOperation;
//        theDialog.exclusiveGuiRendering=(App::userSettings->visionSensorsUseGuiThread_windowed!=0);
//        theDialog.compatibilityTweak1=App::userSettings->oglCompatibilityTweak1;
//        theDialog.glFinish_normal=App::userSettings->useGlFinish;
//        theDialog.glFinish_visionSensors=App::userSettings->useGlFinish_visionSensors;
        theDialog.idleFps=App::userSettings->getIdleFps();
//        theDialog.forceExt=App::userSettings->forceFboViaExt;
//        theDialog.glVersionMajor=App::userSettings->desiredOpenGlMajor;
//        theDialog.glVersionMinor=App::userSettings->desiredOpenGlMinor;

        theDialog.refresh();
        if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_OPENGLSETTINGS_USERSETTINGSGUITRIGGEREDCMD;
            cmd.intParams.push_back(theDialog.offscreenContextType);
            cmd.intParams.push_back(theDialog.fboType);
            cmd.intParams.push_back(theDialog.vboOperation);

            cmd.intParams.push_back(App::userSettings->visionSensorsUseGuiThread_windowed);
            cmd.boolParams.push_back(App::userSettings->oglCompatibilityTweak1);
            cmd.boolParams.push_back(App::userSettings->useGlFinish);
            cmd.boolParams.push_back(App::userSettings->useGlFinish_visionSensors);
            cmd.intParams.push_back(theDialog.idleFps);
            cmd.boolParams.push_back(App::userSettings->forceFboViaExt);
            cmd.intParams.push_back(App::userSettings->desiredOpenGlMajor);
            cmd.intParams.push_back(App::userSettings->desiredOpenGlMinor);

//            int tmp=-1;
//            if (!theDialog.exclusiveGuiRendering)
//                tmp=0;
//            cmd.intParams.push_back(tmp);
//            cmd.boolParams.push_back(theDialog.compatibilityTweak1);
//            cmd.boolParams.push_back(theDialog.glFinish_normal);
//            cmd.boolParams.push_back(theDialog.glFinish_visionSensors);
//            cmd.intParams.push_back(theDialog.idleFps);
//            cmd.boolParams.push_back(theDialog.forceExt);
//            cmd.intParams.push_back(theDialog.glVersionMajor);
//            cmd.intParams.push_back(theDialog.glVersionMinor);
            App::appendSimulationThreadCommand(cmd);
        }
    }
}

void CQDlgSettings::_selectItemOfCombobox(QComboBox* theBox,int itemData)
{
    theBox->setCurrentIndex(_getIndexOfComboboxItemWithData(theBox,itemData));
}

int CQDlgSettings::_getIndexOfComboboxItemWithData(QComboBox* theBox,int itemData)
{
    for (int i=0;i<theBox->count();i++)
    {
        if (theBox->itemData(i).toInt()==itemData)
            return(i);
    }
    return(-1);
}

void CQDlgSettings::on_qqAskWithIncludedScriptFiles_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_ASKTOINCLUDESCRIPTFILES_USERSETTINGSGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}
