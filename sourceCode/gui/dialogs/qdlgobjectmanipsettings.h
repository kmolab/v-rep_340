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

#ifndef QDLGOBJECTMANIPSETTINGS_H
#define QDLGOBJECTMANIPSETTINGS_H

#include "dlgEx.h"
#include "7Vector.h"

namespace Ui {
    class CQDlgObjectManipSettings;
}

class QComboBox;

class CQDlgObjectManipSettings : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgObjectManipSettings(QWidget *parent = 0);
    ~CQDlgObjectManipSettings();

    void refresh();

    void cancelEvent();

    static bool forbidManipWhenRunning;
    static bool forbidManipWhenNotRunning;
    static int manipulationModePermission;
    static int manipulationTranslationRelativeTo;
    static float manipulationTranslationStepSize;
    static int manipulationRotationRelativeTo;
    static float manipulationRotationStepSize;

protected:
    int lastLastSelectionID;

private slots:
    void on_qqDisabledWhenNotRunning_clicked();

    void on_qqDisabledWhenRunning_clicked();

    void on_qqPosWorld_clicked();

    void on_qqPosParent_clicked();

    void on_qqPosOwn_clicked();

    void on_qqPosX_clicked();

    void on_qqPosY_clicked();

    void on_qqPosZ_clicked();

    void on_qqPosCombo_activated(int index);

    void on_qqOrWorld_clicked();

    void on_qqOrParent_clicked();

    void on_qqOrOwn_clicked();

    void on_qqOrA_clicked();

    void on_qqOrB_clicked();

    void on_qqOrG_clicked();

    void on_qqOrCombo_activated(int index);

private:
    Ui::CQDlgObjectManipSettings *ui;

    void _selectItemOfCombobox(QComboBox* theBox,int itemData);
    int _getIndexOfComboboxItemWithData(QComboBox* theBox,int itemData);

};

#endif // QDLGOBJECTMANIPSETTINGS_H
