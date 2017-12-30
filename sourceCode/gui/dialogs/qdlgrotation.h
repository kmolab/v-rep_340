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

#ifndef QDLGROTATION_H
#define QDLGROTATION_H

#include "dlgEx.h"
#include "7Vector.h"

namespace Ui {
    class CQDlgRotation;
}

class QComboBox;

class CQDlgRotation : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgRotation(QWidget *parent = 0);
    ~CQDlgRotation();

    void refresh();

    void cancelEvent();


protected:
    void _enableCoordinatePart(bool enableState,bool enableButtons,bool alsoRadioButtons);
    void _enableTransformationPart(bool enableState,bool enableButtons,bool alsoRadioButtons);
    void _setDefaultValuesCoordinatePart(bool alsoRadioButtons);
    void _setDefaultValuesTransformationPart(bool alsoRadioButtons);
    void _setValuesTransformationPart(bool alsoRadioButtons);

    // Coord part
    bool _setCoord_userUnit(float newValueInUserUnit,int index);
    C7Vector _getNewTransf(const C7Vector& transf,float newValueInUserUnit,int index);
    bool _applyCoord();

    // Transf part
    bool _applyTransformation(int axis);
    void _transform(C7Vector& tr,bool self,int axis);

    static int coordMode; //0=abs,1=rel to parent
    static float rotAngles[3];
    static int transfMode; //0=abs,1=rel to parent,2=rel to self
    static int currentTab; //0=mouse transl., 1=pos, 2=transl., 3=scaling

    static int manipulationModePermission;
    static int manipulationRotationRelativeTo;
    static float manipulationRotationStepSize;

    int lastLastSelectionID;

private slots:
    void on_qqCoordWorld_clicked();

    void on_qqCoordParent_clicked();

    void on_qqCoordAlpha_editingFinished();

    void on_qqCoordBeta_editingFinished();

    void on_qqCoordGamma_editingFinished();

    void on_qqCoordApplyOr_clicked();

    void on_qqTransfWorld_clicked();

    void on_qqTransfParent_clicked();

    void on_qqTransfOwn_clicked();

    void on_qqTransfAlpha_editingFinished();

    void on_qqTransfBeta_editingFinished();

    void on_qqTransfGamma_editingFinished();

    void on_qqTransfApplyOr_clicked();

    void on_qqTransfApplyOrX_clicked();

    void on_qqTransfApplyOrY_clicked();

    void on_qqTransfApplyOrZ_clicked();

    void on_tabWidget_currentChanged(int index);

    // Mouse manip
    void on_qqOrWorld_clicked();
    void on_qqOrParent_clicked();
    void on_qqOrOwn_clicked();
    void on_qqOrA_clicked();
    void on_qqOrB_clicked();
    void on_qqOrG_clicked();
    void on_qqOrCombo_activated(int index);


private:
    void _selectItemOfCombobox(QComboBox* theBox,int itemData);
    int _getIndexOfComboboxItemWithData(QComboBox* theBox,int itemData);

    Ui::CQDlgRotation *ui;
};

#endif // QDLGROTATION_H
