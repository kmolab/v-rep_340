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

#ifndef QDLGSCRIPTS_H
#define QDLGSCRIPTS_H

#include "dlgEx.h"
#include <QListWidgetItem>

namespace Ui {
    class CQDlgScripts;
}

class CQDlgScripts : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgScripts(QWidget *parent = 0);
    ~CQDlgScripts();

    void refresh();
    void dialogCallbackFunc(const SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut);

    void updateObjectsInList();
    int getSelectedObjectID();
    void selectObjectInList(int objectID);
    bool inSelectionRoutine;
    bool rebuildingRoutine;
    bool inMainRefreshRoutine;

    static int scriptViewMode;

private slots:
    void onDeletePressed();
    void on_qqAddNewScript_clicked();

    void on_qqScriptList_itemSelectionChanged();

    void on_qqScriptList_itemDoubleClicked(QListWidgetItem *item);

    void on_qqDisabled_clicked();

    void on_qqExecuteOnce_clicked();

    void on_qqAssociatedObjectCombo_currentIndexChanged(int index);

    void on_qqExecutionOrder_currentIndexChanged(int index);

    void on_qqCombo_currentIndexChanged(int index);

private:
    Ui::CQDlgScripts *ui;
};

#endif // QDLGSCRIPTS_H
