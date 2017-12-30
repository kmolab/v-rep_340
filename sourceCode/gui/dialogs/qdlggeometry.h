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

#ifndef QDLGGEOMETRY_H
#define QDLGGEOMETRY_H

#include "dlgEx.h"

namespace Ui {
    class CQDlgGeometry;
}

class CQDlgGeometry : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgGeometry(QWidget *parent = 0);
    ~CQDlgGeometry();

    void refresh();

    void cancelEvent();
    bool needsDestruction();
    bool isLinkedDataValid();
    bool doesInstanceSwitchRequireDestruction();

    void _initialize(int shapeHandle);
    void _setCurrentSizes();
    void _readSize(int index);
    void _readScaling(int index);
    void _readRotation(int index);

    static void display(int shapeHandle,QWidget* theParentWindow);

    int _shapeHandle;
    float scaleVal[3];
    float sizeVal[3];
    float rotationVal[3];
    bool keepProp;
    bool isPureShape;
    bool isConvex;
    bool isGroup;
    bool insideRefreshTriggered;
    std::string titleText;
    int vertexCount;
    int triangleCount;

private slots:
    void on_qqKeepProp_clicked();

    void on_qqSizeX_editingFinished();

    void on_qqSizeY_editingFinished();

    void on_qqSizeZ_editingFinished();

    void on_qqScaleX_editingFinished();

    void on_qqScaleY_editingFinished();

    void on_qqScaleZ_editingFinished();

    void on_qqApplySize_clicked();

    void on_qqApplyScale_clicked();

    void on_qqAlpha_editingFinished();

    void on_qqBeta_editingFinished();

    void on_qqGamma_editingFinished();

    void on_qqApplyEuler_clicked();

private:
    Ui::CQDlgGeometry *ui;
};

#endif // QDLGGEOMETRY_H
