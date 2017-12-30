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

#ifndef QDLGMATERIAL_H
#define QDLGMATERIAL_H

#include "dlgEx.h"
#include "qdlgcolor.h"
#include "visualParam.h"

namespace Ui {
    class CQDlgMaterial;
}

class CQDlgMaterial : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgMaterial(QWidget *parent = 0);
    ~CQDlgMaterial();

    void refresh();

    void cancelEvent();

    bool needsDestruction();
    bool isLinkedDataValid();
    bool doesInstanceSwitchRequireDestruction();

    bool inMainRefreshRoutine;

    int _objType;
    int _objID1;
    int _objID2;
    int _lastSelectedObjectID;
    int _objectSelectionSize;

    static void displayMaterialDlg(int objType,int objID1,int objID2,QWidget* theParentWindow);
    static void displayMaterialDlgModal(int objType,int objID1,int objID2,QWidget* theParentWindow);
    void _initializeDlg(int objType,int objID1,int objID2);
    void _adjustCol(int colComponent);

private slots:
    void on_qqAmbientAdjust_clicked();

    void on_qqSpecularAdjust_clicked();

    void on_qqEmissiveAdjust_clicked();

    void on_qqPulsationAdjust_clicked();

    void on_qqShininess_editingFinished();

    void on_qqOpacityEnable_clicked();

    void on_qqOpacity_editingFinished();

    void on_qqColorName_editingFinished();

    void on_qqAuxiliaryAdjust_clicked();

    void on_qqExtensionString_editingFinished();

private:
    Ui::CQDlgMaterial *ui;
};

#endif // QDLGMATERIAL_H
