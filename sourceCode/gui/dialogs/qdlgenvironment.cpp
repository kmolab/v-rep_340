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
#include "qdlgenvironment.h"
#include "ui_qdlgenvironment.h"
#include "gV.h"
#include "qdlgcolor.h"
#include "qdlgmaterial.h"
#include "tt.h"
#include "app.h"
#include "v_repStrings.h"
#include "scintillaModalDlg.h"
#include "vMessageBox.h"

CQDlgEnvironment::CQDlgEnvironment(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgEnvironment)
{
    _dlgType=ENVIRONMENT_DLG;
    ui->setupUi(this);
}

CQDlgEnvironment::~CQDlgEnvironment()
{
    delete ui;
}

void CQDlgEnvironment::refresh()
{
    QLineEdit* lineEditToSelect=getSelectedLineEdit();
    bool noEditModeNoSim=(App::getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped();

    ui->qqShowWirelessEmissions->setEnabled(noEditModeNoSim);
    ui->qqShowWirelessReceptions->setEnabled(noEditModeNoSim);
    ui->qqWirelessEmissionColor->setEnabled(App::ct->environment->getVisualizeWirelessEmitters()&&noEditModeNoSim);
    ui->qqWirelessReceptionColor->setEnabled(App::ct->environment->getVisualizeWirelessReceivers()&&noEditModeNoSim);
    ui->qqNextSaveIsDefinitive->setEnabled((!App::ct->environment->getSceneLocked())&&noEditModeNoSim);
    ui->qqCustomContactHandling->setEnabled(noEditModeNoSim);
    ui->qqEditCustomContact->setEnabled(noEditModeNoSim&&App::ct->environment->getEnableCustomContactHandlingViaScript());
    ui->qqGeneralCallback->setEnabled(noEditModeNoSim);
    ui->qqEditGeneralCallback->setEnabled(noEditModeNoSim&&App::ct->environment->getEnableGeneralCallbackScript());
    ui->qqCleanUpHashNames->setEnabled(noEditModeNoSim);
    ui->qqExtensionString->setEnabled(noEditModeNoSim);
    ui->qqBackgroundColorUp->setEnabled(noEditModeNoSim);
    ui->qqBackgroundColorDown->setEnabled(noEditModeNoSim);
    ui->qqAmbientLightColor->setEnabled(noEditModeNoSim);
    ui->qqFogAdjust->setEnabled(noEditModeNoSim);
    ui->qqCleanUpGhosts->setEnabled(noEditModeNoSim);
    ui->qqMaxTriangleSize->setEnabled(noEditModeNoSim);
    ui->qqMinRelTriangleSize->setEnabled(noEditModeNoSim);
    ui->qqSaveCalcStruct->setEnabled(noEditModeNoSim);
    ui->qqShapeTexturesDisabled->setEnabled(noEditModeNoSim);
//    ui->qqUserInterfaceTexturesDisabled->setEnabled(noEditModeNoSim);
    ui->qqAcknowledgments->setEnabled(noEditModeNoSim);

    ui->qqShowWirelessEmissions->setChecked(App::ct->environment->getVisualizeWirelessEmitters());
    ui->qqShowWirelessReceptions->setChecked(App::ct->environment->getVisualizeWirelessReceivers());

    ui->qqMaxTriangleSize->setText(tt::getEString(false,App::ct->environment->getCalculationMaxTriangleSize(),2).c_str());
    ui->qqMinRelTriangleSize->setText(tt::getFString(false,App::ct->environment->getCalculationMinRelTriangleSize(),3).c_str());
    ui->qqSaveCalcStruct->setChecked(App::ct->environment->getSaveExistingCalculationStructures());
    ui->qqShapeTexturesDisabled->setChecked(!App::ct->environment->getShapeTexturesEnabled());
//    ui->qqUserInterfaceTexturesDisabled->setChecked(!App::ct->environment->get2DElementTexturesEnabled());

    ui->qqNextSaveIsDefinitive->setChecked(App::ct->environment->getRequestFinalSave());

    ui->qqCustomContactHandling->setChecked(App::ct->environment->getEnableCustomContactHandlingViaScript());
    ui->qqGeneralCallback->setChecked(App::ct->environment->getEnableGeneralCallbackScript());

    ui->qqExtensionString->setText(App::ct->environment->getExtensionString().c_str());
    ui->qqAcknowledgments->setPlainText(App::ct->environment->getAcknowledgement().c_str());

    selectLineEdit(lineEditToSelect);
}

void CQDlgEnvironment::on_qqBackgroundColorUp_clicked()
{
    CQDlgColor::displayDlg(COLOR_ID_BACKGROUND_UP,-1,-1,0,App::mainWindow);
}

void CQDlgEnvironment::on_qqBackgroundColorDown_clicked()
{
    CQDlgColor::displayDlg(COLOR_ID_BACKGROUND_DOWN,-1,-1,0,App::mainWindow);
}

void CQDlgEnvironment::on_qqAmbientLightColor_clicked()
{
    CQDlgColor::displayDlg(COLOR_ID_AMBIENT_LIGHT,-1,-1,0,App::mainWindow);
}

void CQDlgEnvironment::on_qqFogAdjust_clicked()
{
    App::mainWindow->dlgCont->processCommand(OPEN_FOG_DLG_CMD);
}

void CQDlgEnvironment::on_qqShowWirelessEmissions_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_VISUALIZEEMMISIONS_ENVIRONMENTGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgEnvironment::on_qqShowWirelessReceptions_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_VISUALIZERECEPTIONS_ENVIRONMENTGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgEnvironment::on_qqWirelessEmissionColor_clicked()
{
    CQDlgMaterial::displayMaterialDlg(COLOR_ID_WIRELESS_EMISSIONS,-1,-1,App::mainWindow);
}

void CQDlgEnvironment::on_qqWirelessReceptionColor_clicked()
{
    CQDlgMaterial::displayMaterialDlg(COLOR_ID_WIRELESS_RECEPTIONS,-1,-1,App::mainWindow);
}

void CQDlgEnvironment::on_qqMaxTriangleSize_editingFinished()
{
    if (!ui->qqMaxTriangleSize->isModified())
        return;
    bool ok;
    float newVal=ui->qqMaxTriangleSize->text().toFloat(&ok);
    if (ok)
    {
        App::appendSimulationThreadCommand(SET_MAXTRIANGLESIZE_ENVIRONMENTGUITRIGGEREDCMD,-1,-1,newVal);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    }
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgEnvironment::on_qqSaveCalcStruct_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (!App::ct->environment->getSaveExistingCalculationStructures())
            App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_CALCULATION_STRUCTURE),strTranslate(IDS_SAVING_CALCULATION_STRUCTURE),VMESSAGEBOX_OKELI);
        App::appendSimulationThreadCommand(TOGGLE_SAVECALCSTRUCT_ENVIRONMENTGUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgEnvironment::on_qqShapeTexturesDisabled_clicked()
{
    App::appendSimulationThreadCommand(TOGGLE_SHAPETEXTURES_ENVIRONMENTGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgEnvironment::on_qqNextSaveIsDefinitive_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (!App::ct->environment->getRequestFinalSave())
            App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_SCENE_LOCKING),strTranslate(IDS_SCENE_LOCKING_INFO),VMESSAGEBOX_OKELI);
        App::appendSimulationThreadCommand(TOGGLE_LOCKSCENE_ENVIRONMENTGUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgEnvironment::on_qqAcknowledgments_textChanged()
{
    std::string txt=ui->qqAcknowledgments->toPlainText().toStdString();
    tt::removeSpacesAndEmptyLinesAtBeginningAndEnd(txt);
    // No refresh here!! (otherwise we can't edit the item properly)
    App::appendSimulationThreadCommand(SET_ACKNOWLEDGMENT_ENVIRONMENTGUITRIGGEREDCMD,-1,-1,0.0,0.0,txt.c_str());
}

void CQDlgEnvironment::on_qqCustomContactHandling_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool doIt=false;
        bool disableIt=false;
        std::string scriptTxt;
        if (App::ct->environment->getEnableCustomContactHandlingViaScript())
        { // disable it
            if (VMESSAGEBOX_REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Custom collision/contact response"),strTranslate(IDSN_SURE_TO_REMOVE_CUSTOM_CONTACT_WARNING),VMESSAGEBOX_YES_NO))
            {
                doIt=true;
                disableIt=true;
            }
        }
        else
        { // enable it
            App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Custom collision/contact response"),strTranslate(IDSN_CUSTOM_CONTACT_IS_SLOW_AND_NOT_RECOMENDED_WARNING),VMESSAGEBOX_OKELI);
            std::string filenameAndPath(App::directories->systemDirectory+VREP_SLASH);
            filenameAndPath+=DEFAULT_CONTACTCALLBACKSCRIPT_NAME;
            if (VFile::doesFileExist(filenameAndPath))
            {
                try
                {
                    VFile file(filenameAndPath,VFile::READ|VFile::SHARE_DENY_NONE);
                    VArchive archive(&file,VArchive::LOAD);
                    unsigned int archiveLength=(unsigned int)file.getLength();
                    char* defaultScript=new char[archiveLength+1];
                    for (int i=0;i<int(archiveLength);i++)
                        archive >> defaultScript[i];
                    defaultScript[archiveLength]=0;
                    scriptTxt=defaultScript;
                    doIt=true;
                    disableIt=false;
                    delete[] defaultScript;
                    archive.close();
                    file.close();
                }
                catch(VFILE_EXCEPTION_TYPE e)
                {
                    VFile::reportAndHandleFileExceptionError(e);
                    scriptTxt="Default script file could not be found!"; // do not use comments ("--"), we want to cause an execution error!
                    doIt=true;
                    disableIt=false;
                }
            }
            else
            {
                scriptTxt="Default collision/contact response script file could not be found!"; // do not use comments ("--"), we want to cause an execution error!
                doIt=true;
                disableIt=false;
            }
        }
        if (doIt)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_CUSTOMCOLLRESPONSE_ENVIRONMENTGUITRIGGEREDCMD;
            cmd.boolParams.push_back(!disableIt);
            cmd.stringParams.push_back(scriptTxt);
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgEnvironment::on_qqEditCustomContact_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (App::ct->environment->getEnableCustomContactHandlingViaScript())
        {
            CLuaScriptObject* script=App::ct->luaScriptContainer->getCustomContactHandlingScript_callback();
            if (script)
                App::mainWindow->scintillaEditorContainer->openEditorForScript(script->getScriptID());
        }
    }
}

void CQDlgEnvironment::on_qqCleanUpHashNames_clicked()
{
    App::appendSimulationThreadCommand(CLEANUP_OBJNAMES_ENVIRONMENTGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgEnvironment::on_qqCleanUpGhosts_clicked()
{
    App::appendSimulationThreadCommand(CLEANUP_GHOSTS_ENVIRONMENTGUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
}

void CQDlgEnvironment::on_qqMinRelTriangleSize_editingFinished()
{
    if (!ui->qqMinRelTriangleSize->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqMinRelTriangleSize->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_MINTRIANGLESIZE_ENVIRONMENTGUITRIGGEREDCMD,-1,-1,newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgEnvironment::on_qqGeneralCallback_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool doIt=false;
        bool enableIt=false;
        std::string scriptTxt;
        if (App::ct->environment->getEnableGeneralCallbackScript())
        { // disable it
            if (VMESSAGEBOX_REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate("General callback script"),strTranslate(IDSN_SURE_TO_REMOVE_GENERAL_CALLBACK_WARNING),VMESSAGEBOX_YES_NO))
            {
                doIt=true;
                enableIt=false;
            }
        }
        else
        { // enable it
            std::string filenameAndPath(App::directories->systemDirectory+VREP_SLASH);
            filenameAndPath+=DEFAULT_GENERALCALLBACKSCRIPT_NAME;
            if (VFile::doesFileExist(filenameAndPath))
            {
                try
                {
                    VFile file(filenameAndPath,VFile::READ|VFile::SHARE_DENY_NONE);
                    VArchive archive(&file,VArchive::LOAD);
                    unsigned int archiveLength=(unsigned int)file.getLength();
                    char* defaultScript=new char[archiveLength+1];
                    for (int i=0;i<int(archiveLength);i++)
                        archive >> defaultScript[i];
                    defaultScript[archiveLength]=0;
                    doIt=true;
                    enableIt=true;
                    scriptTxt=defaultScript;
                    delete[] defaultScript;
                    archive.close();
                    file.close();
                }
                catch(VFILE_EXCEPTION_TYPE e)
                {
                    VFile::reportAndHandleFileExceptionError(e);
                    doIt=true;
                    enableIt=true;
                    scriptTxt="Default script file could not be found!"; // do not use comments ("--"), we want to cause an execution error!
                }
            }
            else
            {
                doIt=true;
                enableIt=true;
                scriptTxt="Default general callback script file could not be found!"; // do not use comments ("--"), we want to cause an execution error!
            }
        }
        if (doIt)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_GENERALCALLBACK_ENVIRONMENTGUITRIGGEREDCMD;
            cmd.boolParams.push_back(enableIt);
            cmd.stringParams.push_back(scriptTxt);
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgEnvironment::on_qqEditGeneralCallback_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (App::ct->environment->getEnableGeneralCallbackScript())
        {
            CLuaScriptObject* script=App::ct->luaScriptContainer->getGeneralCallbackHandlingScript_callback();
            if (script)
                App::mainWindow->scintillaEditorContainer->openEditorForScript(script->getScriptID());
        }
    }
}

void CQDlgEnvironment::on_qqExtensionString_editingFinished()
{
    if (!ui->qqExtensionString->isModified())
        return;
    App::appendSimulationThreadCommand(SET_EXTSTRING_ENVIRONMENTGUITRIGGEREDCMD,-1,-1,0.0,0.0,ui->qqExtensionString->text().toStdString().c_str());
    App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}
