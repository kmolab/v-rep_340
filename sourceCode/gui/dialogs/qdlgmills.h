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

#ifndef QDLGMILLS_H
#define QDLGMILLS_H

#include "dlgEx.h"

namespace Ui {
    class CQDlgMills;
}

class CQDlgMills : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgMills(QWidget *parent = 0);
    ~CQDlgMills();

    void refresh();

    void cancelEvent();

    bool inMainRefreshRoutine;
private slots:
    void on_qqEnableAll_clicked();

    void on_qqExplicitHandling_clicked();

    void on_qqBaseSize_editingFinished();

    void on_qqAdjustVolume_clicked();

    void on_qqEntityToCut_currentIndexChanged(int index);

    void on_qqPassiveVolumeColor_clicked();

    void on_qqActiveVolumeColor_clicked();

private:
    Ui::CQDlgMills *ui;
};

#endif // QDLGMILLS_H
