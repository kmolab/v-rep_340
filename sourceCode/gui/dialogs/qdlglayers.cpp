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
#include "qdlglayers.h"
#include "ui_qdlglayers.h"
#include "app.h"

CQDlgLayers::CQDlgLayers(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgLayers)
{
    _dlgType=LAYERS_DLG;
    ui->setupUi(this);
}

CQDlgLayers::~CQDlgLayers()
{
    delete ui;
}

void CQDlgLayers::refresh()
{
    ui->a_1->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_2->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_3->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_4->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_5->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_6->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_7->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_8->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_9->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_10->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_11->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_12->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_13->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_14->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_15->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());
    ui->a_16->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());

    ui->invert->setEnabled(!App::ct->simulation->getDynamicContentVisualizationOnly());

    ui->qqDynamicContentOnly->setEnabled(!App::ct->simulation->isSimulationStopped());


    ui->a_1->setChecked(App::ct->mainSettings->getActiveLayers()&1);
    ui->a_2->setChecked(App::ct->mainSettings->getActiveLayers()&2);
    ui->a_3->setChecked(App::ct->mainSettings->getActiveLayers()&4);
    ui->a_4->setChecked(App::ct->mainSettings->getActiveLayers()&8);
    ui->a_5->setChecked(App::ct->mainSettings->getActiveLayers()&16);
    ui->a_6->setChecked(App::ct->mainSettings->getActiveLayers()&32);
    ui->a_7->setChecked(App::ct->mainSettings->getActiveLayers()&64);
    ui->a_8->setChecked(App::ct->mainSettings->getActiveLayers()&128);
    ui->a_9->setChecked(App::ct->mainSettings->getActiveLayers()&256);
    ui->a_10->setChecked(App::ct->mainSettings->getActiveLayers()&512);
    ui->a_11->setChecked(App::ct->mainSettings->getActiveLayers()&1024);
    ui->a_12->setChecked(App::ct->mainSettings->getActiveLayers()&2048);
    ui->a_13->setChecked(App::ct->mainSettings->getActiveLayers()&4096);
    ui->a_14->setChecked(App::ct->mainSettings->getActiveLayers()&8192);
    ui->a_15->setChecked(App::ct->mainSettings->getActiveLayers()&16384);
    ui->a_16->setChecked(App::ct->mainSettings->getActiveLayers()&32768);

    ui->qqDynamicContentOnly->setChecked(App::ct->simulation->getDynamicContentVisualizationOnly());

}

void CQDlgLayers::clickedLayer(int bit)
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_LAYER_LAYERGUITRIGGEREDCMD,bit);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgLayers::on_a_1_clicked()
{
    clickedLayer(1);
}

void CQDlgLayers::on_a_2_clicked()
{
    clickedLayer(2);
}

void CQDlgLayers::on_a_3_clicked()
{
    clickedLayer(4);
}

void CQDlgLayers::on_a_4_clicked()
{
    clickedLayer(8);
}

void CQDlgLayers::on_a_5_clicked()
{
    clickedLayer(16);
}

void CQDlgLayers::on_a_6_clicked()
{
    clickedLayer(32);
}

void CQDlgLayers::on_a_7_clicked()
{
    clickedLayer(64);
}

void CQDlgLayers::on_a_8_clicked()
{
    clickedLayer(128);
}

void CQDlgLayers::on_a_9_clicked()
{
    clickedLayer(256);
}

void CQDlgLayers::on_a_10_clicked()
{
    clickedLayer(512);
}

void CQDlgLayers::on_a_11_clicked()
{
    clickedLayer(1024);
}

void CQDlgLayers::on_a_12_clicked()
{
    clickedLayer(2048);
}

void CQDlgLayers::on_a_13_clicked()
{
    clickedLayer(4096);
}

void CQDlgLayers::on_a_14_clicked()
{
    clickedLayer(8192);
}

void CQDlgLayers::on_a_15_clicked()
{
    clickedLayer(16384);
}

void CQDlgLayers::on_a_16_clicked()
{
    clickedLayer(32768);
}

void CQDlgLayers::on_invert_clicked()
{
    clickedLayer(65535);
}

void CQDlgLayers::on_qqDynamicContentOnly_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_SHOWDYNCONTENT_LAYERGUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}
