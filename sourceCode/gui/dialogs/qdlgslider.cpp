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

#include "vrepMainHeader.h"
#include "qdlgslider.h"
#include "ui_qdlgslider.h"
#include "tt.h"

CQDlgSlider::CQDlgSlider(QWidget *parent) : VDialog(parent,QT_MODAL_DLG_STYLE), ui(new Ui::CQDlgSlider)
{
    ui->setupUi(this);
}

CQDlgSlider::~CQDlgSlider()
{
    delete ui;
}

void CQDlgSlider::cancelEvent()
{
    defaultModalDialogEndRoutine(false);
}

void CQDlgSlider::okEvent()
{
//  defaultModalDialogEndRoutine(true);
}

void CQDlgSlider::refresh()
{
    if (opMode==0)
    {
        setWindowTitle("Mesh Decimation");
        ui->qqSlider->setMinimum(100);
        ui->qqSlider->setMaximum(900);
        std::string txt("Shape contains currently ");
        txt+=tt::getIString(false,triCnt);
        txt+=" triangles.";
        ui->qqText1->setText(txt.c_str());
        decimationPercent=tt::getLimitedFloat(0.1f,0.9f,decimationPercent);
        ui->qqSlider->setSliderPosition(int(decimationPercent*1000.0f));
        _displayDecimationText();
    }
}


void CQDlgSlider::on_qqOkCancelButtons_accepted()
{
    defaultModalDialogEndRoutine(true);
}

void CQDlgSlider::on_qqOkCancelButtons_rejected()
{
    defaultModalDialogEndRoutine(false);
}

void CQDlgSlider::on_qqSlider_sliderMoved(int position)
{
    if (opMode==0)
    {
        decimationPercent=float(position)/1000.0f;
        _displayDecimationText();
    }
}

void CQDlgSlider::_displayDecimationText()
{
    std::string txt="Decimate by ";
    txt+=tt::getIString(false,int(decimationPercent*100.0f));
    txt+="% (resulting shape will contain about ";
    txt+=tt::getIString(false,int(float(triCnt)*decimationPercent));
    txt+=" triangles)";
    ui->qqText2->setText(txt.c_str());
}
