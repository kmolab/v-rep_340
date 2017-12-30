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
#include "qdlguibuttons.h"
#include "ui_qdlguibuttons.h"
#include "app.h"
#include "tt.h"
#include "gV.h"
#include "qdlgtextures.h"
#include "qdlgcolor.h"
#include "v_repStrings.h"

CQDlgUiButtons::CQDlgUiButtons(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgUiButtons)
{
    _dlgType=BUTTON_DLG;
    ui->setupUi(this);
    inMainRefreshRoutine=false;
}

CQDlgUiButtons::~CQDlgUiButtons()
{
    delete ui;
}

void CQDlgUiButtons::cancelEvent()
{
    // we override this cancel event. The container window should close, not this one!!
    // We close it by ending the button edit mode:
    App::mainWindow->editModeContainer->endEditMode(true);
}

void CQDlgUiButtons::refresh()
{
    inMainRefreshRoutine=true;
    QLineEdit* lineEditToSelect=getSelectedLineEdit();

    int uiID=App::ct->buttonBlockContainer->getBlockInEdition();
    CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(uiID);
    CSoftButton* itButton=NULL;
    if (it==NULL)
        return; // shouldn't happen
    int buttonSelectedNb=0;
    int emptyButtonSelectedNb=0;
    if (App::ct->buttonBlockContainer->selectedButtons.size()>0)
    {
        int pos=App::ct->buttonBlockContainer->selectedButtons[App::ct->buttonBlockContainer->selectedButtons.size()-1];
        VPoint size;
        it->getBlockSize(size);
        itButton=it->getButtonAtPos(pos%size.x,pos/size.x);
        for (int i=0;i<int(App::ct->buttonBlockContainer->selectedButtons.size());i++)
        {
            pos=App::ct->buttonBlockContainer->selectedButtons[i];
            CSoftButton* anyButton=it->getButtonAtPos(pos%size.x,pos/size.x);
            if (anyButton!=NULL)
                buttonSelectedNb++;
            else
                emptyButtonSelectedNb++;
        }
    }
    else
        return; // shouldn't happen

    ui->qqInsertButtons->setEnabled(emptyButtonSelectedNb!=0);
    ui->qqInsertMergedButton->setEnabled(emptyButtonSelectedNb>1);
    bool lastSelIsButton=false;
    bool lastSelStaysDown=false;
    int bt=0;
    if (itButton!=NULL)
    {
        lastSelIsButton=itButton->getButtonType()==sim_buttonproperty_button;
        bt=itButton->getButtonType();
    }
    if (lastSelIsButton)
        lastSelStaysDown=(itButton->getAttributes()&sim_buttonproperty_staydown)!=0;


    ui->qqButtonHandle->setEnabled(itButton!=NULL);

    ui->qqTypeCombo->setEnabled(itButton!=NULL);
    ui->qqEnabled->setEnabled((itButton!=NULL)&&(bt!=sim_buttonproperty_label));
    ui->qqStayDown->setEnabled((itButton!=NULL)&&lastSelIsButton);
    ui->qqRollUp->setEnabled((itButton!=NULL)&&lastSelIsButton&&lastSelStaysDown);
    ui->qqCenteredH->setEnabled((itButton!=NULL)&&(bt!=sim_buttonproperty_slider));
    ui->qqUpDownEvent->setEnabled((itButton!=NULL)&&lastSelIsButton&&(!lastSelStaysDown));
    ui->qqCloseAction->setEnabled((itButton!=NULL)&&lastSelIsButton);
    ui->qqCenteredV->setEnabled((itButton!=NULL)&&(itButton->getButtonType()!=sim_buttonproperty_slider));
    ui->qqBorderless->setEnabled(itButton!=NULL);
    ui->qqIgnoreMouse->setEnabled(itButton!=NULL);
    ui->qqApplyType->setEnabled((itButton!=NULL)&&(buttonSelectedNb>1));

    ui->qqLabelUp->setEnabled((itButton!=NULL)&&(bt!=sim_buttonproperty_slider));
    ui->qqLabelDown->setEnabled((itButton!=NULL)&&lastSelIsButton&&lastSelStaysDown);
    ui->qqApplyLabel->setEnabled((itButton!=NULL)&&(buttonSelectedNb>1));

    ui->qqColorUp->setEnabled(itButton!=NULL);
    ui->qqColorDown->setEnabled((itButton!=NULL)&&lastSelIsButton);
    ui->qqColorLabel->setEnabled((itButton!=NULL)&&(bt!=sim_buttonproperty_slider));
    ui->qqApplyColor->setEnabled((itButton!=NULL)&&(buttonSelectedNb>1));

    ui->qqTransparent->setEnabled(itButton!=NULL);
    ui->qqNoBackground->setEnabled(itButton!=NULL);
    ui->qqSetTexture->setEnabled((itButton!=NULL)&&(bt!=sim_buttonproperty_editbox));
    ui->qqApplyOtherProperties->setEnabled((itButton!=NULL)&&(buttonSelectedNb>1));


    ui->qqTypeCombo->clear();
    ui->qqEnabled->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_enabled)!=0));
    ui->qqStayDown->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_staydown)!=0));
    ui->qqRollUp->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_rollupaction)!=0));
    ui->qqCenteredH->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_horizontallycentered)!=0));
    ui->qqUpDownEvent->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_downupevent)!=0));
    ui->qqCloseAction->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_closeaction)!=0));
    ui->qqCenteredV->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_verticallycentered)!=0));
    ui->qqBorderless->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_borderless)!=0));
    ui->qqIgnoreMouse->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_ignoremouse)!=0));


    ui->qqTransparent->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_transparent)!=0));
    ui->qqNoBackground->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_nobackgroundcolor)!=0));

    if (itButton!=NULL)
    {
        ui->qqButtonHandle->setText(tt::getIString(false,itButton->buttonID).c_str());
        ui->qqLabelUp->setText(itButton->label.c_str());
        ui->qqLabelDown->setText(itButton->downLabel.c_str());

        ui->qqTypeCombo->addItem(strTranslate(IDSN_BUTTON),QVariant(sim_buttonproperty_button));
        ui->qqTypeCombo->addItem(strTranslate(IDSN_LABEL),QVariant(sim_buttonproperty_label));
        ui->qqTypeCombo->addItem(strTranslate(IDSN_EDIT_BOX),QVariant(sim_buttonproperty_editbox));
        ui->qqTypeCombo->addItem(strTranslate(IDSN_SLIDER),QVariant(sim_buttonproperty_slider));
        for (int i=0;i<ui->qqTypeCombo->count();i++)
        {
            if (ui->qqTypeCombo->itemData(i).toInt()==(bt&7))
            {
                ui->qqTypeCombo->setCurrentIndex(i);
                break;
            }
        }
    }
    else
    {
        ui->qqButtonHandle->setText("");
        ui->qqLabelUp->setText("");
        ui->qqLabelDown->setText("");
    }

    selectLineEdit(lineEditToSelect);
    inMainRefreshRoutine=false;
}

CSoftButton* CQDlgUiButtons::getLastSelectedButton()
{
    CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
    if (itBlock==NULL)
        return(NULL);
    if (App::ct->buttonBlockContainer->selectedButtons.size()==0)
        return(NULL);
    int butt=App::ct->buttonBlockContainer->selectedButtons[App::ct->buttonBlockContainer->selectedButtons.size()-1];
    VPoint size;
    itBlock->getBlockSize(size);
    CSoftButton* itButton=itBlock->getButtonAtPos(butt%size.x,butt/size.x);
    if (itButton==NULL)
        return(NULL);
    return(itButton);
}

int CQDlgUiButtons::getLastSelectedButtonId()
{
    CSoftButton* itButton=getLastSelectedButton();
    if (itButton==NULL)
        return(-1);
    return(itButton->buttonID);
}

int CQDlgUiButtons::getSelectedButtonId(int index)
{
    CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
    if (itBlock==NULL)
        return(-1);
    if (App::ct->buttonBlockContainer->selectedButtons.size()==0)
        return(-1);
    if (index>=int(App::ct->buttonBlockContainer->selectedButtons.size()))
        return(-1);
    int butt=App::ct->buttonBlockContainer->selectedButtons[index];
    VPoint size;
    itBlock->getBlockSize(size);
    CSoftButton* itButton=itBlock->getButtonAtPos(butt%size.x,butt/size.x);
    if (itButton==NULL)
        return(-1);
    return(itButton->buttonID);
}

void CQDlgUiButtons::on_qqInsertButtons_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=INSERT_BUTTONS_OPENGLUIBUTTONGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
        for (size_t i=0;i<App::ct->buttonBlockContainer->selectedButtons.size();i++)
            cmd.intParams.push_back(App::ct->buttonBlockContainer->selectedButtons[i]);
        App::appendSimulationThreadCommand(cmd);
        //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    }
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgUiButtons::on_qqInsertMergedButton_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=INSERT_MERGEDBUTTON_OPENGLUIBUTTONGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
        for (size_t i=0;i<App::ct->buttonBlockContainer->selectedButtons.size();i++)
            cmd.intParams.push_back(App::ct->buttonBlockContainer->selectedButtons[i]);
        App::appendSimulationThreadCommand(cmd);
        //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    }
    App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
}

void CQDlgUiButtons::on_qqButtonHandle_editingFinished()
{
    if (!ui->qqButtonHandle->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        int newVal=ui->qqButtonHandle->text().toInt(&ok);
        if (ok)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_BUTTONHANDLE_OPENGLUIBUTTONGUITRIGGEREDCMD;
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(getLastSelectedButtonId());
            cmd.intParams.push_back(newVal);
            App::appendSimulationThreadCommand(cmd);
            //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqTypeCombo_currentIndexChanged(int index)
{
    if (!inMainRefreshRoutine)
    {
        IF_UI_EVENT_CAN_READ_DATA
        {
            CSoftButton* itButton=getLastSelectedButton();
            if (itButton!=NULL)
            {
                SSimulationThreadCommand cmd;
                cmd.cmdId=SET_ATTRIBUTES_OPENGLUIBUTTONGUITRIGGEREDCMD;
                cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
                cmd.intParams.push_back(getLastSelectedButtonId());
                cmd.intParams.push_back((itButton->getAttributes()&0xfff8)|(ui->qqTypeCombo->itemData(ui->qqTypeCombo->currentIndex()).toInt()));
                App::appendSimulationThreadCommand(cmd);
                //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            }
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgUiButtons::on_qqEnabled_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ATTRIBUTES_OPENGLUIBUTTONGUITRIGGEREDCMD;
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(getLastSelectedButtonId());
            cmd.intParams.push_back(itButton->getAttributes()^sim_buttonproperty_enabled);
            App::appendSimulationThreadCommand(cmd);
            //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqStayDown_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ATTRIBUTES_OPENGLUIBUTTONGUITRIGGEREDCMD;
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(getLastSelectedButtonId());
            cmd.intParams.push_back(itButton->getAttributes()^sim_buttonproperty_staydown);
            App::appendSimulationThreadCommand(cmd);
            //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqRollUp_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ATTRIBUTES_OPENGLUIBUTTONGUITRIGGEREDCMD;
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(getLastSelectedButtonId());
            cmd.intParams.push_back(itButton->getAttributes()^sim_buttonproperty_rollupaction);
            App::appendSimulationThreadCommand(cmd);
            //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqCenteredH_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ATTRIBUTES_OPENGLUIBUTTONGUITRIGGEREDCMD;
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(getLastSelectedButtonId());
            cmd.intParams.push_back(itButton->getAttributes()^sim_buttonproperty_horizontallycentered);
            App::appendSimulationThreadCommand(cmd);
            //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqUpDownEvent_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ATTRIBUTES_OPENGLUIBUTTONGUITRIGGEREDCMD;
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(getLastSelectedButtonId());
            cmd.intParams.push_back(itButton->getAttributes()^sim_buttonproperty_downupevent);
            App::appendSimulationThreadCommand(cmd);
            //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqCloseAction_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ATTRIBUTES_OPENGLUIBUTTONGUITRIGGEREDCMD;
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(getLastSelectedButtonId());
            cmd.intParams.push_back(itButton->getAttributes()^sim_buttonproperty_closeaction);
            App::appendSimulationThreadCommand(cmd);
            //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqCenteredV_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ATTRIBUTES_OPENGLUIBUTTONGUITRIGGEREDCMD;
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(getLastSelectedButtonId());
            cmd.intParams.push_back(itButton->getAttributes()^sim_buttonproperty_verticallycentered);
            App::appendSimulationThreadCommand(cmd);
            //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqBorderless_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ATTRIBUTES_OPENGLUIBUTTONGUITRIGGEREDCMD;
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(getLastSelectedButtonId());
            cmd.intParams.push_back(itButton->getAttributes()^sim_buttonproperty_borderless);
            App::appendSimulationThreadCommand(cmd);
            //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqIgnoreMouse_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ATTRIBUTES_OPENGLUIBUTTONGUITRIGGEREDCMD;
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(getLastSelectedButtonId());
            cmd.intParams.push_back(itButton->getAttributes()^sim_buttonproperty_ignoremouse);
            App::appendSimulationThreadCommand(cmd);
            //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqApplyType_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=APPLY_BUTTONTYPE_OPENGLUIBUTTONGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
        cmd.intParams.push_back(getLastSelectedButtonId());
        for (size_t i=0;i<App::ct->buttonBlockContainer->selectedButtons.size()-1;i++)
            cmd.intParams.push_back(getSelectedButtonId(i));
        App::appendSimulationThreadCommand(cmd);
        //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqLabelUp_editingFinished()
{
    if (!ui->qqLabelUp->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=SET_LABEL_OPENGLUIBUTTONGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
        cmd.intParams.push_back(getLastSelectedButtonId());
        cmd.boolParams.push_back(true);
        cmd.stringParams.push_back(ui->qqLabelUp->text().toStdString());
        App::appendSimulationThreadCommand(cmd);
        //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqLabelDown_editingFinished()
{
    if (!ui->qqLabelDown->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=SET_LABEL_OPENGLUIBUTTONGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
        cmd.intParams.push_back(getLastSelectedButtonId());
        cmd.boolParams.push_back(false);
        cmd.stringParams.push_back(ui->qqLabelDown->text().toStdString());
        App::appendSimulationThreadCommand(cmd);
        //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqApplyLabel_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=APPLY_BUTTONLABELS_OPENGLUIBUTTONGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
        cmd.intParams.push_back(getLastSelectedButtonId());
        for (size_t i=0;i<App::ct->buttonBlockContainer->selectedButtons.size()-1;i++)
            cmd.intParams.push_back(getSelectedButtonId(i));
        App::appendSimulationThreadCommand(cmd);
        //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqColorUp_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton==NULL)
            return;
        CQDlgColor::displayDlgModal(COLOR_ID_OPENGLBUTTON_UP,App::ct->buttonBlockContainer->getBlockInEdition(),itButton->buttonID,sim_colorcomponent_ambient_diffuse,App::mainWindow);
        float* col=App::getRGBPointerFromItem(COLOR_ID_OPENGLBUTTON_UP,App::ct->buttonBlockContainer->getBlockInEdition(),itButton->buttonID,sim_colorcomponent_ambient_diffuse,NULL);
        if (col!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ITEMRGB_COLORGUITRIGGEREDCMD;
            cmd.intParams.push_back(COLOR_ID_OPENGLBUTTON_UP);
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(itButton->buttonID);
            cmd.intParams.push_back(sim_colorcomponent_ambient_diffuse);
            cmd.floatParams.push_back(col[0]);
            cmd.floatParams.push_back(col[1]);
            cmd.floatParams.push_back(col[2]);
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgUiButtons::on_qqColorDown_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton==NULL)
            return;
        CQDlgColor::displayDlgModal(COLOR_ID_OPENGLBUTTON_DOWN,App::ct->buttonBlockContainer->getBlockInEdition(),itButton->buttonID,sim_colorcomponent_ambient_diffuse,App::mainWindow);
        float* col=App::getRGBPointerFromItem(COLOR_ID_OPENGLBUTTON_DOWN,App::ct->buttonBlockContainer->getBlockInEdition(),itButton->buttonID,sim_colorcomponent_ambient_diffuse,NULL);
        if (col!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ITEMRGB_COLORGUITRIGGEREDCMD;
            cmd.intParams.push_back(COLOR_ID_OPENGLBUTTON_DOWN);
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(itButton->buttonID);
            cmd.intParams.push_back(sim_colorcomponent_ambient_diffuse);
            cmd.floatParams.push_back(col[0]);
            cmd.floatParams.push_back(col[1]);
            cmd.floatParams.push_back(col[2]);
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgUiButtons::on_qqColorLabel_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton==NULL)
            return;
        CQDlgColor::displayDlgModal(COLOR_ID_OPENGLBUTTON_TEXT,App::ct->buttonBlockContainer->getBlockInEdition(),itButton->buttonID,sim_colorcomponent_ambient_diffuse,App::mainWindow);
        float* col=App::getRGBPointerFromItem(COLOR_ID_OPENGLBUTTON_TEXT,App::ct->buttonBlockContainer->getBlockInEdition(),itButton->buttonID,sim_colorcomponent_ambient_diffuse,NULL);
        if (col!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ITEMRGB_COLORGUITRIGGEREDCMD;
            cmd.intParams.push_back(COLOR_ID_OPENGLBUTTON_TEXT);
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(itButton->buttonID);
            cmd.intParams.push_back(sim_colorcomponent_ambient_diffuse);
            cmd.floatParams.push_back(col[0]);
            cmd.floatParams.push_back(col[1]);
            cmd.floatParams.push_back(col[2]);
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
    }
}

void CQDlgUiButtons::on_qqApplyColor_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=APPLY_BUTTONCOLORS_OPENGLUIBUTTONGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
        cmd.intParams.push_back(getLastSelectedButtonId());
        for (size_t i=0;i<App::ct->buttonBlockContainer->selectedButtons.size()-1;i++)
            cmd.intParams.push_back(getSelectedButtonId(i));
        App::appendSimulationThreadCommand(cmd);
        //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqTransparent_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ATTRIBUTES_OPENGLUIBUTTONGUITRIGGEREDCMD;
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(getLastSelectedButtonId());
            int atr=itButton->getAttributes();
            if ((atr&sim_buttonproperty_transparent)==0)
            {
                atr=atr&(0xffff-sim_buttonproperty_nobackgroundcolor);
                atr=atr^sim_buttonproperty_transparent;
            }
            else
                atr=atr^sim_buttonproperty_transparent;
            cmd.intParams.push_back(atr);
            App::appendSimulationThreadCommand(cmd);
            //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqNoBackground_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_ATTRIBUTES_OPENGLUIBUTTONGUITRIGGEREDCMD;
            cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
            cmd.intParams.push_back(getLastSelectedButtonId());
            int atr=itButton->getAttributes();
            if ((atr&sim_buttonproperty_nobackgroundcolor)==0)
            {
                atr=atr&(0xffff-sim_buttonproperty_transparent);
                atr=atr^sim_buttonproperty_nobackgroundcolor;
            }
            else
                atr=atr^sim_buttonproperty_nobackgroundcolor;
            cmd.intParams.push_back(atr);
            App::appendSimulationThreadCommand(cmd);
            //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgUiButtons::on_qqSetTexture_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CSoftButton* itButton=getLastSelectedButton();
        if (itButton!=NULL)
            CQDlgTextures::displayDlg(TEXTURE_ID_OPENGL_GUI_BUTTON,App::ct->buttonBlockContainer->getBlockInEdition(),itButton->buttonID,App::mainWindow);
    }
}

void CQDlgUiButtons::on_qqApplyOtherProperties_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=APPLY_BUTTONOTHERPROP_OPENGLUIBUTTONGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->buttonBlockContainer->getBlockInEdition());
        cmd.intParams.push_back(getLastSelectedButtonId());
        for (size_t i=0;i<App::ct->buttonBlockContainer->selectedButtons.size()-1;i++)
            cmd.intParams.push_back(getSelectedButtonId(i));
        App::appendSimulationThreadCommand(cmd);
        //App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    }
}
