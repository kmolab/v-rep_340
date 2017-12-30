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

#ifndef QDLGSETTINGS_H
#define QDLGSETTINGS_H

#include "dlgEx.h"

namespace Ui {
    class CQDlgSettings;
}


class QComboBox;

class CQDlgSettings : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgSettings(QWidget *parent = 0);
    ~CQDlgSettings();

    void refresh();

private slots:
    void on_translationStepSize_activated(int index);

    void on_rotationStepSize_activated(int index);

    void on_removeIdenticalVerticesCheckbox_clicked();

    void on_removeIdenticalVerticesTolerance_editingFinished();

    void on_removeIdenticalTriangles_clicked();

    void on_ignoreTriangleWinding_clicked();

    void on_antialiasing_clicked();

    void on_worldReference_clicked();

    void on_boundingBoxDisplay_clicked();

    void on_undoRedo_clicked();

    void on_hideConsole_clicked();

    void on_qqTransparentDlgs_clicked();

    void on_qqAutoSave_clicked();

    void on_qqExternalScriptEditor_clicked();

    void on_qqSelectScriptEditor_clicked();

    void on_qqThreadedRendering_clicked();

    void on_qqAdjustOpenGl_clicked();

    void on_qqAskWithIncludedScriptFiles_clicked();

private:
    Ui::CQDlgSettings *ui;

    void _selectItemOfCombobox(QComboBox* theBox,int itemData);
    int _getIndexOfComboboxItemWithData(QComboBox* theBox,int itemData);
};

#endif // QDLGSETTINGS_H
