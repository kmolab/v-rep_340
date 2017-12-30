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

#ifndef QDLGCOLOR_H
#define QDLGCOLOR_H

#include "dlgEx.h"

namespace Ui {
    class CQDlgColor;
}

class CQDlgColor : public CDlgEx 
{
    Q_OBJECT

public:
    explicit CQDlgColor(QWidget *parent = 0);
    ~CQDlgColor();

    void refresh();

    void initializationEvent();
    void cancelEvent();
    bool needsDestruction();
    bool isLinkedDataValid();
    bool doesInstanceSwitchRequireDestruction();

    void computeRGBValues();
    void computeHSLValues();

    static void displayDlg(int objType,int objID1,int objID2,int colComponent,QWidget* theParentWindow,bool doNotCloseMaterialDlg=false,bool doNotCloseLightMaterialDlg=false,bool appendColorComponentInName=false);
    static void displayDlgModal(int objType,int objID1,int objID2,int colComponent,QWidget* theParentWindow,bool doNotCloseMaterialDlg=false,bool doNotCloseLightMaterialDlg=false,bool appendColorComponentInName=false);
    void initializeDlg(int objType,int objID1,int objID2,int colComponent,bool appendColorComponentInName);

    float redState;
    float greenState;
    float blueState;
    float hueState;
    float saturationState;
    float luminosityState;
    int _objType;
    int _objID1;
    int _objID2;
    int _colComponent;
    bool inRefreshPart;
    int _validityCheck1;
    int _validityCheck2;

private slots:
    void on_qqRedValue_editingFinished();

    void on_qqGreenValue_editingFinished();

    void on_qqBlueValue_editingFinished();

    void on_qqHueValue_editingFinished();

    void on_qqSaturationValue_editingFinished();

    void on_qqLuminosityValue_editingFinished();

    void on_qqRedSlider_sliderMoved(int position);

    void on_qqGreenSlider_sliderMoved(int position);

    void on_qqBlueSlider_sliderMoved(int position);

    void on_qqHueSlider_sliderMoved(int position);

    void on_qqSaturationSlider_sliderMoved(int position);

    void on_qqLuminositySlider_sliderMoved(int position);

    void on_qqRedSlider_sliderReleased();

    void on_qqGreenSlider_sliderReleased();

    void on_qqBlueSlider_sliderReleased();

    void on_qqHueSlider_sliderReleased();

    void on_qqSaturationSlider_sliderReleased();

    void on_qqLuminositySlider_sliderReleased();

private:
    void _getRGBFromItem();
    void _setRGBToItem();

    Ui::CQDlgColor *ui;
};

#endif // QDLGCOLOR_H
