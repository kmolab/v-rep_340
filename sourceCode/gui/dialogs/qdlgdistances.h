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

#ifndef QDLGDISTANCES_H
#define QDLGDISTANCES_H

#include "dlgEx.h"
#include <QListWidgetItem>

namespace Ui {
    class CQDlgDistances;
}

class CQDlgDistances : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgDistances(QWidget *parent = 0);
    ~CQDlgDistances();

    void refresh();
    void dialogCallbackFunc(const SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut);

    void cancelEvent();

    void updateObjectsInList();
    int getSelectedObjectID();
    void selectObjectInList(int objectID);
    bool inSelectionRoutine;

private slots:
    void onDeletePressed();
    void on_qqAddNewObject_clicked();

    void on_qqDistanceList_itemSelectionChanged();

    void on_qqDistanceList_itemChanged(QListWidgetItem *item);

    void on_qqEnableAll_clicked();

    void on_qqExplicitHandling_clicked();

    void on_qqUseThreshold_clicked();

    void on_qqThreshold_editingFinished();

    void on_qqDisplaySegment_clicked();

    void on_qqSegmentWidth_editingFinished();

    void on_qqAdjustSegmentColor_clicked();

private:
    Ui::CQDlgDistances *ui;
};

#endif // QDLGDISTANCES_H
