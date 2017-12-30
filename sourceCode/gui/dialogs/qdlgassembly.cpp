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
#include "qdlgassembly.h"
#include "ui_qdlgassembly.h"
#include "v_rep_internal.h"
#include "tt.h"

#include "app.h"
#include "v_repStrings.h"
#include "vMessageBox.h"

CQDlgAssembly::CQDlgAssembly(QWidget *parent) :
    VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgAssembly)
{
    obj=NULL;
    ui->setupUi(this);
}

CQDlgAssembly::~CQDlgAssembly()
{
    delete ui;
}

void CQDlgAssembly::cancelEvent()
{
//  defaultModalDialogEndRoutine(false);
}

void CQDlgAssembly::okEvent()
{
//  defaultModalDialogEndRoutine(true);
}

void CQDlgAssembly::refresh()
{
    ui->qqSetLocalMatrix->setEnabled((obj->getAssemblyMatchValues(true).length()!=0)&&obj->getAssemblingLocalTransformationIsUsed());
    ui->qqChildMatchValue->setText(obj->getAssemblyMatchValues(true).c_str());
    ui->qqParentMatchValue->setText(obj->getAssemblyMatchValues(false).c_str());
    ui->qqHasMatrix->setChecked(obj->getAssemblingLocalTransformationIsUsed());
}

void CQDlgAssembly::on_qqChildMatchValue_editingFinished()
{
    if (!ui->qqChildMatchValue->isModified())
        return;
    obj->setAssemblyMatchValues(true,ui->qqChildMatchValue->text().toStdString().c_str());
    refresh();
}

void CQDlgAssembly::on_qqParentMatchValue_editingFinished()
{
    if (!ui->qqParentMatchValue->isModified())
        return;
    obj->setAssemblyMatchValues(false,ui->qqParentMatchValue->text().toStdString().c_str());
    refresh();
}

void CQDlgAssembly::on_qqSetLocalMatrix_clicked()
{
    unsigned short res=App::uiThread->messageBox_question(App::mainWindow,strTranslate(IDSN_ASSEMBLING_METHOD),strTranslate(IDSN_ASSEMBLING_METHOD_QUESTION),VMESSAGEBOX_YES_NO);
    if (res==VMESSAGEBOX_REPLY_YES)
    {
        C7Vector transform(obj->getLocalTransformationPart1());
        obj->setAssemblingLocalTransformation(transform);
        refresh();
    }
}

void CQDlgAssembly::on_qqClose_clicked(QAbstractButton *button)
{
    App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    defaultModalDialogEndRoutine(true);
}

void CQDlgAssembly::on_qqHasMatrix_clicked()
{
    obj->setAssemblingLocalTransformationIsUsed(!obj->getAssemblingLocalTransformationIsUsed());
    refresh();
}
