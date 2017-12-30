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
#include "qdlgslider2.h"
#include "ui_qdlgslider2.h"
#include "tt.h"

CQDlgSlider2::CQDlgSlider2(QWidget *parent) : VDialog(parent,QT_MODAL_DLG_STYLE), ui(new Ui::CQDlgSlider2)
{
    ui->setupUi(this);
}

CQDlgSlider2::~CQDlgSlider2()
{
    delete ui;
}

void CQDlgSlider2::cancelEvent()
{
    defaultModalDialogEndRoutine(false);
}

void CQDlgSlider2::okEvent()
{
//  defaultModalDialogEndRoutine(true);
}

void CQDlgSlider2::refresh()
{
    if (opMode==0)
    {
        setWindowTitle("Shape Inside Extraction");
        ui->qqTextBefore1->setText("Resolution:");
        ui->qqTextBefore2->setText("Number of iterations:");
        ui->qqSlider1->setMinimum(0);
        ui->qqSlider1->setMaximum(11);
        ui->qqSlider2->setMinimum(100);
        ui->qqSlider2->setMaximum(5000);
        resolution=tt::getLimitedInt(2,4096,resolution);
        int bit=2;
        _resolutionLevel=-1;
        for (int i=0;i<11;i++)
        {
            if (resolution>=bit)
            {
                _resolutionLevel++;
                bit*=2;
            }
            else
                break;
        }
        resolution=bit/2;
        ui->qqSlider1->setSliderPosition(_resolutionLevel);
        iterationCnt=tt::getLimitedInt(100,10000,iterationCnt);
        ui->qqSlider2->setSliderPosition(iterationCnt);
        _displayMeshInsideExtractionTexts();
    }
}


void CQDlgSlider2::on_qqOkCancelButtons_accepted()
{
    defaultModalDialogEndRoutine(true);
}

void CQDlgSlider2::on_qqOkCancelButtons_rejected()
{
    defaultModalDialogEndRoutine(false);
}

void CQDlgSlider2::on_qqSlider1_sliderMoved(int position)
{
    if (opMode==0)
    {
        _resolutionLevel=position;
        resolution=2;
        for (int i=0;i<_resolutionLevel;i++)
            resolution*=2;
        _displayMeshInsideExtractionTexts();
    }
}

void CQDlgSlider2::on_qqSlider2_sliderMoved(int position)
{
    if (opMode==0)
    {
        iterationCnt=position;
        _displayMeshInsideExtractionTexts();
    }
}

void CQDlgSlider2::_displayMeshInsideExtractionTexts()
{
    std::string txt="Resolution ";
    txt+=tt::getIString(false,resolution);
    txt+="x";
    txt+=tt::getIString(false,resolution);
    if (resolution>1024)
        txt+=" (warning: requires a lot of memory!)";
    ui->qqTextAfter1->setText(txt.c_str());
    txt="Iteration count ";
    txt+=tt::getIString(false,iterationCnt);
    ui->qqTextAfter2->setText(txt.c_str());
}
