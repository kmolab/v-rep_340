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

#ifndef QDLGTEXTURES_H
#define QDLGTEXTURES_H

#include "dlgEx.h"

namespace Ui {
    class CQDlgTextures;
}

class CQDlgTextures : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgTextures(QWidget *parent = 0);
    ~CQDlgTextures();

    void refresh();

    void cancelEvent();
    bool needsDestruction();
    bool isLinkedDataValid();
    bool doesInstanceSwitchRequireDestruction();

    int _objType;
    int _objID1;
    int _objID2;

    void _setTextureConfig(int index);
    void _setTextureScaling(int index);
    void _setTextureBooleanProperty(int index);

    static void displayDlg(int objType,int objID1,int objID2,QWidget* theParentWindow);
    void _initializeDlg(int objType,int objID1,int objID2);

private slots:
    void on_qqAlpha_editingFinished();

    void on_qqBeta_editingFinished();

    void on_qqGamma_editingFinished();

    void on_qqX_editingFinished();

    void on_qqY_editingFinished();

    void on_qqZ_editingFinished();

    void on_qqU_editingFinished();

    void on_qqV_editingFinished();

    void on_qqInterpolate_clicked();

    void on_qqRepeatU_clicked();

    void on_qqRepeatV_clicked();

    void on_qqRemoveSelect_clicked();

    void on_qqLoad_clicked();

    void on_qqMapMode_currentIndexChanged(int index);

    void on_qqApplyMode_currentIndexChanged(int index);

private:
    Ui::CQDlgTextures *ui;

    bool inMainRefreshRoutine;
};

#endif // QDLGTEXTURES_H
