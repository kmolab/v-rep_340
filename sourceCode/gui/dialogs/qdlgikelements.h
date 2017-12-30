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

#ifndef QDLGIKELEMENTS_H
#define QDLGIKELEMENTS_H

#include "dlgEx.h"
#include "ikGroup.h"
#include <QAbstractButton>

namespace Ui {
    class CQDlgIkElements;
}

class CQDlgIkElements : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgIkElements(QWidget *parent = 0);
    ~CQDlgIkElements();

    void cancelEvent();
    void refresh();
    void dialogCallbackFunc(const SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut);
    bool needsDestruction();
    bool isLinkedDataValid();
    bool doesInstanceSwitchRequireDestruction();

    void _initialize(int ikGroupHandle);
    static void display(int ikGroupHandle,QWidget* theParentWindow);

    void updateObjectsInList();
    int getSelectedObjectID();
    void selectObjectInList(int objectID);

    bool inMainRefreshRoutine;
    bool inListSelectionRoutine;
    bool noListSelectionAllowed;

    static int _ikGroupHandle;
    static bool _invalid;

private slots:
    void onDeletePressed();

    void on_qqAddNewElement_clicked();

    void on_qqList_itemSelectionChanged();

    void on_qqActive_clicked();

    void on_qqBaseCombo_currentIndexChanged(int index);

    void on_qqX_clicked();

    void on_qqY_clicked();

    void on_qqZ_clicked();

    void on_qqAlphaBeta_clicked();

    void on_qqGamma_clicked();

    void on_qqRelativeCombo_currentIndexChanged(int index);

    void on_qqPrecisionLinear_editingFinished();

    void on_qqPrecisionAngular_editingFinished();

    void on_qqWeightLinear_editingFinished();

    void on_qqWeightAngular_editingFinished();

private:
    Ui::CQDlgIkElements *ui;
};

#endif // QDLGIKELEMENTS_H
