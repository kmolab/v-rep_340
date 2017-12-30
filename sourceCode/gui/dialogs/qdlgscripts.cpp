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
#include "qdlgscripts.h"
#include "ui_qdlgscripts.h"
#include "tt.h"
#include <QShortcut>
#include "qdlginsertscript.h"
#include "app.h"
#include "v_repStrings.h"
#include "jointObject.h"
#include "scintillaModalDlg.h"
#include "vMessageBox.h"

int CQDlgScripts::scriptViewMode=0;

CQDlgScripts::CQDlgScripts(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgScripts)
{
    _dlgType=LUA_SCRIPT_DLG;
    ui->setupUi(this);
    inSelectionRoutine=false;
    rebuildingRoutine=false;
    inMainRefreshRoutine=false;
    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(shortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
    QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
    connect(shortcut2,SIGNAL(activated()), this, SLOT(onDeletePressed()));
}

CQDlgScripts::~CQDlgScripts()
{
    delete ui;
}

void CQDlgScripts::dialogCallbackFunc(const SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut)
{
    if ( (cmdIn!=NULL)&&(cmdIn->intParams[0]==_dlgType) )
    {
        if (cmdIn->intParams[1]==0)
            selectObjectInList(cmdIn->intParams[2]);
    }
}

void CQDlgScripts::refresh()
{
    inMainRefreshRoutine=true;
    bool noEditMode=(App::getEditModeType()==NO_EDIT_MODE);
    bool noEditModeAndNoSim=noEditMode&&App::ct->simulation->isSimulationStopped();

    ui->qqCombo->setEnabled(noEditMode);
    ui->qqCombo->clear();
    ui->qqCombo->addItem(strTranslate(IDS_MAIN_AND_CHILD_SCRIPTS),QVariant(0));
    ui->qqCombo->addItem(strTranslate(IDS_CALLBACK_SCRIPTS),QVariant(1));
    ui->qqCombo->addItem(strTranslate(IDS_CUSTOMIZATION_SCRIPTS),QVariant(2));
    ui->qqCombo->addItem(strTranslate(IDS_ADD_ON_SCRIPTs),QVariant(3));
    ui->qqCombo->setCurrentIndex(scriptViewMode);

    int selectedObjectID=getSelectedObjectID();
    updateObjectsInList();
    selectObjectInList(selectedObjectID);

    ui->qqAddNewScript->setEnabled(noEditModeAndNoSim);
    ui->qqScriptList->setEnabled(noEditMode);

    ui->qqExecutionOrder->clear();
    ui->qqAssociatedObjectCombo->clear();

    CLuaScriptObject* theScript=App::ct->luaScriptContainer->getScriptFromID(getSelectedObjectID());
    ui->qqExecutionOrder->setEnabled((theScript!=NULL)&&noEditModeAndNoSim&&( (theScript->getScriptType()==sim_scripttype_childscript)||(theScript->getScriptType()==sim_scripttype_jointctrlcallback)||(theScript->getScriptType()==sim_scripttype_customizationscript) ));
    ui->qqAssociatedObjectCombo->setEnabled((theScript!=NULL)&&noEditModeAndNoSim&&( (theScript->getScriptType()==sim_scripttype_childscript)||(theScript->getScriptType()==sim_scripttype_jointctrlcallback)||(theScript->getScriptType()==sim_scripttype_customizationscript) ));
    ui->qqDisabled->setEnabled((theScript!=NULL)&&noEditMode&&( (theScript->getScriptType()==sim_scripttype_childscript)||(theScript->getScriptType()==sim_scripttype_customizationscript) ));
    ui->qqExecuteOnce->setEnabled((theScript!=NULL)&&noEditModeAndNoSim&&(theScript->getScriptType()==sim_scripttype_childscript)&&theScript->getThreadedExecution());

    if (theScript!=NULL)
    {
        if ( (theScript->getScriptType()==sim_scripttype_childscript)||(theScript->getScriptType()==sim_scripttype_jointctrlcallback)||(theScript->getScriptType()==sim_scripttype_customizationscript) )
        {
            ui->qqExecutionOrder->addItem(strTranslate(IDSN_FIRST),QVariant(sim_scriptexecorder_first));
            ui->qqExecutionOrder->addItem(strTranslate(IDSN_NORMAL),QVariant(sim_scriptexecorder_normal));
            ui->qqExecutionOrder->addItem(strTranslate(IDSN_LAST),QVariant(sim_scriptexecorder_last));
            ui->qqExecutionOrder->setCurrentIndex(theScript->getExecutionOrder());

            ui->qqAssociatedObjectCombo->addItem(strTranslate(IDSN_NONE),QVariant(-1));
            std::vector<std::string> names;
            std::vector<int> ids;
            for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
            {
                C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
                CLuaScriptObject* so=NULL;
                if (theScript->getScriptType()==sim_scripttype_childscript)
                    so=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_child(it->getID());
                if (theScript->getScriptType()==sim_scripttype_jointctrlcallback)
                    so=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_callback(it->getID());
                if (theScript->getScriptType()==sim_scripttype_customizationscript)
                    so=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_customization(it->getID());
                if ( (so==NULL)||(so==theScript) )
                {
                    if (theScript->getScriptType()!=sim_scripttype_jointctrlcallback)
                    {
                        names.push_back(it->getName());
                        ids.push_back(it->getID());
                    }
                    else
                    { // joint control callbacks. The object needs to be a joint (non-spherical):
                        if (it->getObjectType()==sim_object_joint_type)
                        {
                            CJoint* itj=(CJoint*)it;
                            if ( (itj->getJointType()==sim_joint_revolute_subtype)||(itj->getJointType()==sim_joint_prismatic_subtype) )
                            {
                                names.push_back(it->getName());
                                ids.push_back(it->getID());
                            }
                        }
                    }
                }
            }
            tt::orderStrings(names,ids);
            for (int i=0;i<int(names.size());i++)
                ui->qqAssociatedObjectCombo->addItem(names[i].c_str(),QVariant(ids[i]));
            int objIdAttached=-1;
            if (theScript->getScriptType()==sim_scripttype_childscript)
                objIdAttached=theScript->getObjectIDThatScriptIsAttachedTo_child();
            if (theScript->getScriptType()==sim_scripttype_jointctrlcallback)
                objIdAttached=theScript->getObjectIDThatScriptIsAttachedTo_callback();
            if (theScript->getScriptType()==sim_scripttype_customizationscript)
                objIdAttached=theScript->getObjectIDThatScriptIsAttachedTo_customization();
            for (int i=0;i<ui->qqAssociatedObjectCombo->count();i++)
            {
                if (ui->qqAssociatedObjectCombo->itemData(i).toInt()==objIdAttached)
                {
                    ui->qqAssociatedObjectCombo->setCurrentIndex(i);
                    break;
                }
            }
        }

        if (theScript->getScriptType()==sim_scripttype_customizationscript)
            ui->qqDisabled->setChecked(theScript->getScriptIsDisabled()||theScript->getCustomizationScriptIsTemporarilyDisabled());
        else
            ui->qqDisabled->setChecked(theScript->getScriptIsDisabled());


        ui->qqExecuteOnce->setChecked(theScript->getExecuteJustOnce());
    }
    else
    {
        ui->qqDisabled->setChecked(false);
        ui->qqExecuteOnce->setChecked(false);
    }
    inMainRefreshRoutine=false;
}

void CQDlgScripts::updateObjectsInList()
{
    rebuildingRoutine=true;
    ui->qqScriptList->clear();

    if (scriptViewMode==0)
    { // Main and child scripts
        CLuaScriptObject* it=App::ct->luaScriptContainer->getMainScript();
        if (it!=NULL)
        {
            std::string tmp=it->getDescriptiveName();
            int id=it->getScriptID();
            QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
            itm->setData(Qt::UserRole,QVariant(id));
            itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            itm->setForeground(QColor(255,128,128)); // RED
            ui->qqScriptList->addItem(itm);
        }
        for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
        {
            it=App::ct->luaScriptContainer->allScripts[i];
            int t=it->getScriptType();
            if (t==sim_scripttype_childscript)
            {
                std::string tmp=it->getDescriptiveName();
                int id=it->getScriptID();
                QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
                itm->setData(Qt::UserRole,QVariant(id));
                itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                if (it->getThreadedExecution())
                    itm->setForeground(QColor(128,205,205)); // CYAN
                else
                    itm->setForeground(QColor(128,128,128)); // GREY
                ui->qqScriptList->addItem(itm);
            }
        }
    }

    if (scriptViewMode==1)
    { // Callback scripts
        CLuaScriptObject* it=App::ct->luaScriptContainer->getCustomContactHandlingScript_callback();
        if (it!=NULL)
        {
            std::string tmp=it->getDescriptiveName();
            int id=it->getScriptID();
            QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
            itm->setData(Qt::UserRole,QVariant(id));
            itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            itm->setForeground(QColor(128,128,255)); // BLUE
            ui->qqScriptList->addItem(itm);
        }
        it=App::ct->luaScriptContainer->getGeneralCallbackHandlingScript_callback();
        if (it!=NULL)
        {
            std::string tmp=it->getDescriptiveName();
            int id=it->getScriptID();
            QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
            itm->setData(Qt::UserRole,QVariant(id));
            itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            itm->setForeground(QColor(128,255,128)); // GREEN
            ui->qqScriptList->addItem(itm);
        }
        for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
        {
            it=App::ct->luaScriptContainer->allScripts[i];
            int t=it->getScriptType();
            if (t==sim_scripttype_jointctrlcallback)
            {
                std::string tmp=it->getDescriptiveName();
                int id=it->getScriptID();
                QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
                itm->setData(Qt::UserRole,QVariant(id));
                itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                itm->setForeground(QColor(128,128,128)); // GREY
                ui->qqScriptList->addItem(itm);
            }
        }

    }

    if (scriptViewMode==2)
    { // Customization scripts
        for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
        {
            CLuaScriptObject* it=App::ct->luaScriptContainer->allScripts[i];
            int t=it->getScriptType();
            if (t==sim_scripttype_customizationscript)
            {
                std::string tmp=it->getDescriptiveName();
                int id=it->getScriptID();
                QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
                itm->setData(Qt::UserRole,QVariant(id));
                itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                itm->setForeground(QColor(128,128,128)); // GREY
                ui->qqScriptList->addItem(itm);
            }
        }
    }

    if (scriptViewMode==3)
    { // Add-on scripts
        for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
        {
            CLuaScriptObject* it=App::ct->luaScriptContainer->allScripts[i];
            int t=it->getScriptType();
            if (t==sim_scripttype_addonscript)
            {
                std::string tmp=it->getDescriptiveName();
                int id=it->getScriptID();
                QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
                itm->setData(Qt::UserRole,QVariant(id));
                itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                itm->setForeground(QColor(128,128,128)); // GREY
                ui->qqScriptList->addItem(itm);
            }
        }
    }
    rebuildingRoutine=false;
}

int CQDlgScripts::getSelectedObjectID()
{
    QList<QListWidgetItem*> sel=ui->qqScriptList->selectedItems();
    if (sel.size()>0)
        return(sel.at(0)->data(Qt::UserRole).toInt());
    return(-1);
}

void CQDlgScripts::selectObjectInList(int objectID)
{
    for (int i=0;i<ui->qqScriptList->count();i++)
    {
        QListWidgetItem* it=ui->qqScriptList->item(i);
        if (it!=NULL)
        {
            if (it->data(Qt::UserRole).toInt()==objectID)
            {
                inSelectionRoutine=true;
                it->setSelected(true);
                inSelectionRoutine=false;
                break;
            }
        }
    }
}

void CQDlgScripts::onDeletePressed()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if ( (focusWidget()==ui->qqScriptList)&&App::ct->simulation->isSimulationStopped() )
        {
            int scriptID=getSelectedObjectID();
            App::appendSimulationThreadCommand(DELETE_SCRIPT_SCRIPTGUITRIGGEREDCMD,scriptID);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}


void CQDlgScripts::on_qqAddNewScript_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CQDlgInsertScript theDialog(this);
        theDialog.initialize();
        if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
        {
            if (theDialog.scriptType==sim_scripttype_mainscript)
            {
                scriptViewMode=0;
                CLuaScriptObject* it=App::ct->luaScriptContainer->getMainScript();
                if (it!=NULL)
                {
                    if (VMESSAGEBOX_REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDS_MAIN_SCRIPT),strTranslate(IDS_INFO_NO_MORE_THAN_ONE_MAIN_SCRIPT),VMESSAGEBOX_YES_NO))
                    {
                        App::appendSimulationThreadCommand(DELETE_SCRIPT_SCRIPTGUITRIGGEREDCMD,it->getScriptID());
                        App::appendSimulationThreadCommand(INSERT_SCRIPT_SCRIPTGUITRIGGEREDCMD,sim_scripttype_mainscript,0);
                    }
                }
                else
                    App::appendSimulationThreadCommand(INSERT_SCRIPT_SCRIPTGUITRIGGEREDCMD,sim_scripttype_mainscript,0);
            }
            if (theDialog.scriptType==sim_scripttype_childscript)
            {
                scriptViewMode=0;
                App::appendSimulationThreadCommand(INSERT_SCRIPT_SCRIPTGUITRIGGEREDCMD,sim_scripttype_childscript,0);
            }
            if (theDialog.scriptType==(sim_scripttype_childscript|sim_scripttype_threaded))
            {
                scriptViewMode=0;
                App::appendSimulationThreadCommand(INSERT_SCRIPT_SCRIPTGUITRIGGEREDCMD,sim_scripttype_childscript,1);
            }
            if (theDialog.scriptType==sim_scripttype_customizationscript)
            {
                scriptViewMode=2;
                App::appendSimulationThreadCommand(INSERT_SCRIPT_SCRIPTGUITRIGGEREDCMD,sim_scripttype_customizationscript,0);
            }
            if (theDialog.scriptType==sim_scripttype_contactcallback)
            {
                scriptViewMode=1;
                bool goOn=true;
                bool doNotShowWarning=false;
                if (App::ct->environment->getEnableCustomContactHandlingViaScript())
                { // we already have a handler!
                    goOn=false;
                    doNotShowWarning=true;
                    if (VMESSAGEBOX_REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Custom collision/contact response"),strTranslate(IDS_INFO_NO_MORE_THAN_ONE_CONTACT_CALLBACK_SCRIPT),VMESSAGEBOX_YES_NO))
                    { // let's first remove the existing callback:
                        CLuaScriptObject* it=App::ct->luaScriptContainer->getCustomContactHandlingScript_callback();
                        if (it!=NULL)
                            App::appendSimulationThreadCommand(DELETE_SCRIPT_SCRIPTGUITRIGGEREDCMD,it->getScriptID());
                        goOn=true;
                    }
                }
                if (goOn)
                { // enable it
                    if (!doNotShowWarning)
                        App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Custom collision/contact response"),strTranslate(IDSN_CUSTOM_CONTACT_IS_SLOW_AND_NOT_RECOMENDED_WARNING),VMESSAGEBOX_OKELI);
                    App::appendSimulationThreadCommand(INSERT_SCRIPT_SCRIPTGUITRIGGEREDCMD,sim_scripttype_contactcallback,0);
                }
            }
            if (theDialog.scriptType==sim_scripttype_generalcallback)
            {
                scriptViewMode=1;
                bool goOn=true;
                if (App::ct->environment->getEnableGeneralCallbackScript())
                { // we already have a handler!
                    goOn=false;
                    if (VMESSAGEBOX_REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate("General callback script"),strTranslate(IDS_INFO_NO_MORE_THAN_ONE_GENERAL_CALLBACK_SCRIPT),VMESSAGEBOX_YES_NO))
                    { // let's first remove the existing callback:
                        CLuaScriptObject* it=App::ct->luaScriptContainer->getGeneralCallbackHandlingScript_callback();
                        if (it!=NULL)
                            App::appendSimulationThreadCommand(DELETE_SCRIPT_SCRIPTGUITRIGGEREDCMD,it->getScriptID());
                        goOn=true;
                    }
                }
                if (goOn)
                { // enable it
                    App::appendSimulationThreadCommand(INSERT_SCRIPT_SCRIPTGUITRIGGEREDCMD,sim_scripttype_generalcallback,0);
                }
            }
            if (theDialog.scriptType==sim_scripttype_jointctrlcallback)
            {
                scriptViewMode=1;
                App::appendSimulationThreadCommand(INSERT_SCRIPT_SCRIPTGUITRIGGEREDCMD,sim_scripttype_jointctrlcallback,0);
            }
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
//      selectObjectInList(newScriptID);
    }
}

void CQDlgScripts::on_qqScriptList_itemSelectionChanged()
{ // react only if the user clicked there!
    IF_UI_EVENT_CAN_READ_DATA
    {
        if ((!inSelectionRoutine)&&(!rebuildingRoutine))
            refresh();
    }
}

void CQDlgScripts::on_qqScriptList_itemDoubleClicked(QListWidgetItem *item)
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        if ( (item!=NULL)&&App::ct->simulation->isSimulationStopped() )
        {
            CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(item->data(Qt::UserRole).toInt());
            if (it!=NULL)
            {
                if ((it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript)||(it->getScriptType()==sim_scripttype_jointctrlcallback)||(it->getScriptType()==sim_scripttype_contactcallback)||(it->getScriptType()==sim_scripttype_generalcallback))
                    App::mainWindow->scintillaEditorContainer->openEditorForScript(it->getScriptID());
                if (it->getScriptType()==sim_scripttype_customizationscript)
                { // needs to be modal
                    // Process the command via the simulation thread (delayed):
                    C3DObject* theObj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo_customization());
                    std::string dlgTitle("Customization script '");
                    if (theObj!=NULL)
                        dlgTitle+=theObj->getName();
                    dlgTitle+="'";
                    SSimulationThreadCommand cmd;
                    cmd.cmdId=OPEN_MODAL_CUSTOMIZATION_SCRIPT_EDITOR_CMD;
                    cmd.intParams.push_back(it->getScriptID());
                    cmd.stringParams.push_back(dlgTitle);
                    App::appendSimulationThreadCommand(cmd);
                }
                if (it->getScriptType()==sim_scripttype_addonscript)
                { // needs to be modal
                    CScintillaModalDlg dlg(sim_scripttype_addonscript,App::mainWindow);
                    std::string dlgTitle("Add-on script '");
                    dlgTitle+=it->getAddOnName();
                    dlgTitle+="' (READ ONLY)";
                    if (dlg.initialize(it->getScriptID(),dlgTitle.c_str(),true,true))
                        dlg.makeDialogModal();
                }
            }
        }
    }
}

void CQDlgScripts::on_qqDisabled_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        int scriptID=getSelectedObjectID();
        App::appendSimulationThreadCommand(TOGGLE_DISABLED_SCRIPTGUITRIGGEREDCMD,scriptID);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_HIERARCHY_GUITRIGGEREDCMD);
    }
}

void CQDlgScripts::on_qqExecuteOnce_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        int scriptID=getSelectedObjectID();
        App::appendSimulationThreadCommand(TOGGLE_EXECUTEONCE_SCRIPTGUITRIGGEREDCMD,scriptID);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgScripts::on_qqAssociatedObjectCombo_currentIndexChanged(int index)
{
    if (!inMainRefreshRoutine)
    {
        IF_UI_EVENT_CAN_READ_DATA
        {
            int scriptID=getSelectedObjectID();
            int objID=ui->qqAssociatedObjectCombo->itemData(ui->qqAssociatedObjectCombo->currentIndex()).toInt();
            App::appendSimulationThreadCommand(SET_ASSOCIATEDOBJECT_SCRIPTGUITRIGGEREDCMD,scriptID,objID);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_HIERARCHY_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgScripts::on_qqExecutionOrder_currentIndexChanged(int index)
{
    if (!inMainRefreshRoutine)
    {
        IF_UI_EVENT_CAN_READ_DATA
        {
            int scriptID=getSelectedObjectID();
            int executionOrder=ui->qqExecutionOrder->itemData(ui->qqExecutionOrder->currentIndex()).toInt();
            App::appendSimulationThreadCommand(SET_EXECORDER_SCRIPTGUITRIGGEREDCMD,scriptID,executionOrder);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgScripts::on_qqCombo_currentIndexChanged(int index)
{
    if (!inMainRefreshRoutine)
    {
        IF_UI_EVENT_CAN_READ_DATA
        {
            scriptViewMode=ui->qqCombo->itemData(ui->qqCombo->currentIndex()).toInt();
            refresh();
        }
    }
}
