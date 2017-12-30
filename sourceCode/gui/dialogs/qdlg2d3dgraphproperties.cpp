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
#include "qdlg2d3dgraphproperties.h"
#include "ui_qdlg2d3dgraphproperties.h"
#include "gV.h"
#include "tt.h"
#include "graphingRoutines.h"
#include <QShortcut>
#include "editboxdelegate.h"
#include "qdlgcolor.h"
#include "qdlgmaterial.h"
#include "qdlgaddgraphcurve.h"
#include "v_repStrings.h"
#include "app.h"
#include "vMessageBox.h"

CQDlg2D3DGraphProperties::CQDlg2D3DGraphProperties(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlg2D3DGraphProperties)
{
    _dlgType=GRAPH2DAND3DCURVES_DLG;
    ui->setupUi(this);
    _inListSelectionRoutine=false;
    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(shortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
    QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
    connect(shortcut2,SIGNAL(activated()), this, SLOT(onDeletePressed()));
    CEditBoxDelegate* delegate=new CEditBoxDelegate();
    ui->qqList->setItemDelegate(delegate);
    _graphHandle=-1;
    if (App::mainWindow!=NULL)
        App::mainWindow->dlgCont->close(GRAPH2DAND3DCURVES_DLG);
}

CQDlg2D3DGraphProperties::~CQDlg2D3DGraphProperties()
{
    delete ui;
}

void CQDlg2D3DGraphProperties::dialogCallbackFunc(const SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut)
{
    if ( (cmdIn!=NULL)&&(cmdIn->intParams[0]==_dlgType) )
    {
        if (cmdIn->intParams[1]==0)
            selectObjectInList(cmdIn->intParams[2]);
    }
}

void CQDlg2D3DGraphProperties::cancelEvent()
{ // We just hide the dialog and destroy it at next rendering pass
    _graphHandle=-1;
    CDlgEx::cancelEvent();
}

bool CQDlg2D3DGraphProperties::doesInstanceSwitchRequireDestruction()
{
    return(true);
}

bool CQDlg2D3DGraphProperties::needsDestruction()
{
    if (!isLinkedDataValid())
        return(true);
    return(CDlgEx::needsDestruction());
}

void CQDlg2D3DGraphProperties::_initialize(int graphHandle,bool xyGraph)
{
    _graphHandle=graphHandle;
    _xyGraph=xyGraph;
    CGraph* graph=App::ct->objCont->getGraph(_graphHandle);
    if (graph!=NULL)
    {
        std::string txt;
        if (_xyGraph)
        {
            ui->qqGroupBox->setTitle(strTranslate(IDSN_XY_CURVE_PROPERTIES));
            txt=IDSN_XY_CURVES;
        }
        else
        {
            ui->qqGroupBox->setTitle(strTranslate(IDSN_3D_CURVE_PROPERTIES));
            txt=IDSN_3D_CURVES;
        }
        txt+=" (";
        txt+=graph->getName();
        txt+=")";
        setWindowTitle(txt.c_str());
    }

    refresh();
}

bool CQDlg2D3DGraphProperties::isLinkedDataValid()
{
    if (!App::ct->simulation->isSimulationStopped())
        return(false);
    if (App::getEditModeType()!=NO_EDIT_MODE)
        return(false);
    if (App::ct->objCont->getGraph(_graphHandle)!=NULL)
        return(App::ct->objCont->getLastSelectionID()==_graphHandle);
    return(false);
}

void CQDlg2D3DGraphProperties::display(int graphHandle,bool xyGraph,QWidget* theParentWindow)
{
    if (App::mainWindow==NULL)
        return;
    App::mainWindow->dlgCont->close(GRAPH2DAND3DCURVES_DLG);
    if (App::mainWindow->dlgCont->openOrBringToFront(GRAPH2DAND3DCURVES_DLG))
    {
        CQDlg2D3DGraphProperties* dlg=(CQDlg2D3DGraphProperties*)App::mainWindow->dlgCont->getDialog(GRAPH2DAND3DCURVES_DLG);
        if (dlg!=NULL)
            dlg->_initialize(graphHandle,xyGraph);
    }
}

void CQDlg2D3DGraphProperties::onDeletePressed()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (focusWidget()==ui->qqList)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=DELETE_CURVE_GRAPHCURVEGUITRIGGEREDCMD;
            cmd.intParams.push_back(_graphHandle);
            cmd.intParams.push_back(getSelectedObjectID());
            cmd.boolParams.push_back(_xyGraph);
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlg2D3DGraphProperties::refresh()
{
    if (!isLinkedDataValid())
        return;

    ui->qqWidthLabel->setVisible(!_xyGraph);
    ui->qqWidth->setVisible(!_xyGraph);
    ui->qqRelativeToGraph->setVisible(!_xyGraph);
    ui->qqRelativeToWorld->setVisible(!_xyGraph);
    ui->qqAlwaysOnTop->setVisible(!_xyGraph);


    CGraph* it=App::ct->objCont->getLastSelection_graph();
    CGraphDataComb* graphData=NULL;
    int graphDataId=getSelectedObjectID();
    if (_xyGraph)
        graphData=it->getGraphData2D(graphDataId);
    else
        graphData=it->getGraphData3D(graphDataId);

    ui->qqVisible->setEnabled(graphData!=NULL);
    ui->qqDisplayLabel->setEnabled(graphData!=NULL);
    ui->qqLinkPoints->setEnabled(graphData!=NULL);
    ui->qqAdjustColor->setEnabled(graphData!=NULL);
    ui->qqDuplicate->setEnabled(graphData!=NULL);
    ui->qqWidth->setEnabled((graphData!=NULL)&&(!_xyGraph));
    ui->qqRelativeToGraph->setEnabled((graphData!=NULL)&&(!_xyGraph));
    ui->qqRelativeToWorld->setEnabled((graphData!=NULL)&&(!_xyGraph));
    ui->qqAlwaysOnTop->setEnabled((graphData!=NULL)&&(!_xyGraph));

    ui->qqVisible->setChecked((graphData!=NULL)&&graphData->getVisible());
    ui->qqDisplayLabel->setChecked((graphData!=NULL)&&graphData->getLabel());
    ui->qqLinkPoints->setChecked((graphData!=NULL)&&graphData->getLinkPoints());
    ui->qqRelativeToGraph->setChecked((graphData!=NULL)&&(!graphData->getCurveRelativeToWorld()));
    ui->qqRelativeToWorld->setChecked((graphData!=NULL)&&(graphData->getCurveRelativeToWorld()));
    ui->qqAlwaysOnTop->setChecked((graphData!=NULL)&&(graphData->getVisibleOnTopOfEverything()));

    if ((graphData!=NULL)&&(!_xyGraph))
        ui->qqWidth->setText(tt::getIString(false,int(graphData->get3DCurveWidth()+0.1f)).c_str());
    else
        ui->qqWidth->setText("");

    if (!_inListSelectionRoutine)
    {
        updateObjectsInList();
        selectObjectInList(graphDataId);
    }
}

void CQDlg2D3DGraphProperties::updateObjectsInList()
{
    ui->qqList->clear();
    if (!App::ct->objCont->isLastSelectionAGraph())
        return;
    CGraph* it=App::ct->objCont->getLastSelection_graph();
    if (_xyGraph)
    {
        for (size_t i=0;i<it->twoDPartners.size();i++)
        {
            CGraphDataComb* aa=it->twoDPartners[i];
            std::string tmp(aa->getName());
            tmp+=" [";
            if (aa->data[0]==-1)
                tmp+="0.0 - ";
            else
                tmp+=it->getGraphData(aa->data[0])->getName()+" - ";
            if (aa->data[1]==-1)
                tmp+="0.0]";
            else
                tmp+=it->getGraphData(aa->data[1])->getName()+"]";

            QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
            itm->setData(Qt::UserRole,QVariant(aa->getIdentifier()));
            itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
            ui->qqList->addItem(itm);
        }
    }
    else
    {
        for (size_t i=0;i<it->threeDPartners.size();i++)
        {
            CGraphDataComb* aa=it->threeDPartners[i];

            std::string tmp(aa->getName());
            tmp+=" [";
            if (aa->data[0]==-1)
                tmp+="0.0 - ";
            else
                tmp+=it->getGraphData(aa->data[0])->getName()+" - ";
            if (aa->data[1]==-1)
                tmp+="0.0 - ";
            else
                tmp+=it->getGraphData(aa->data[1])->getName()+" - ";
            if (aa->data[2]==-1)
                tmp+="0.0]";
            else
                tmp+=it->getGraphData(aa->data[2])->getName()+"]";

            QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
            itm->setData(Qt::UserRole,QVariant(aa->getIdentifier()));
            itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
            ui->qqList->addItem(itm);
        }
    }
}

int CQDlg2D3DGraphProperties::getSelectedObjectID()
{
    QList<QListWidgetItem*> sel=ui->qqList->selectedItems();
    if (sel.size()>0)
        return(sel.at(0)->data(Qt::UserRole).toInt());
    return(-1);
}

void CQDlg2D3DGraphProperties::selectObjectInList(int objectID)
{
    for (int i=0;i<ui->qqList->count();i++)
    {
        QListWidgetItem* it=ui->qqList->item(i);
        if (it!=NULL)
        {
            if (it->data(Qt::UserRole).toInt()==objectID)
            {
                it->setSelected(true);
                break;
            }
        }
    }
}

void CQDlg2D3DGraphProperties::on_qqAddNewCurve_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CQDlgAddGraphCurve theDialog(this);
        theDialog.xyGraph=_xyGraph;
        theDialog.refresh();
        if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=ADD_NEWCURVE_GRAPHCURVEGUITRIGGEREDCMD;
            cmd.intParams.push_back(_graphHandle);
            cmd.intParams.push_back(theDialog.dataIDX);
            cmd.intParams.push_back(theDialog.dataIDY);
            cmd.intParams.push_back(theDialog.dataIDZ);
            cmd.boolParams.push_back(_xyGraph);
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlg2D3DGraphProperties::on_qqList_itemSelectionChanged()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CGraph* it=App::ct->objCont->getLastSelection_graph();
        int objID=getSelectedObjectID();
        CGraphDataComb* grData;
        if (_xyGraph)
            grData=it->getGraphData2D(objID);
        else
            grData=it->getGraphData3D(objID);
        if (grData!=NULL)
            ((CEditBoxDelegate*)ui->qqList->itemDelegate())->initialText=grData->getName();
        else
            ((CEditBoxDelegate*)ui->qqList->itemDelegate())->initialText="";
        _inListSelectionRoutine=true;
        refresh();
        _inListSelectionRoutine=false;
    }
}

void CQDlg2D3DGraphProperties::on_qqList_itemChanged(QListWidgetItem *item)
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (item!=NULL)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=RENAME_CURVE_GRAPHCURVEGUITRIGGEREDCMD;
            cmd.intParams.push_back(_graphHandle);
            cmd.intParams.push_back(item->data(Qt::UserRole).toInt());
            cmd.boolParams.push_back(_xyGraph);
            cmd.stringParams.push_back(item->text().toStdString());
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlg2D3DGraphProperties::on_qqVisible_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=TOGGLE_VISIBLE_GRAPHCURVEGUITRIGGEREDCMD;
        cmd.intParams.push_back(_graphHandle);
        cmd.intParams.push_back(getSelectedObjectID());
        cmd.boolParams.push_back(_xyGraph);
        App::appendSimulationThreadCommand(cmd);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlg2D3DGraphProperties::on_qqDisplayLabel_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=TOGGLE_LABEL_GRAPHCURVEGUITRIGGEREDCMD;
        cmd.intParams.push_back(_graphHandle);
        cmd.intParams.push_back(getSelectedObjectID());
        cmd.boolParams.push_back(_xyGraph);
        App::appendSimulationThreadCommand(cmd);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlg2D3DGraphProperties::on_qqLinkPoints_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=TOGGLE_LINKPTS_GRAPHCURVEGUITRIGGEREDCMD;
        cmd.intParams.push_back(_graphHandle);
        cmd.intParams.push_back(getSelectedObjectID());
        cmd.boolParams.push_back(_xyGraph);
        App::appendSimulationThreadCommand(cmd);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlg2D3DGraphProperties::on_qqAdjustColor_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (_xyGraph)
            CQDlgColor::displayDlg(COLOR_ID_GRAPH_2DCURVE,App::ct->objCont->getLastSelectionID(),getSelectedObjectID(),sim_colorcomponent_ambient_diffuse,App::mainWindow);
        else
            CQDlgMaterial::displayMaterialDlg(COLOR_ID_GRAPH_3DCURVE,App::ct->objCont->getLastSelectionID(),getSelectedObjectID(),App::mainWindow);
    }
}

void CQDlg2D3DGraphProperties::on_qqDuplicate_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=DUPLICATE_TOSTATIC_GRAPHCURVEGUITRIGGEREDCMD;
        cmd.intParams.push_back(_graphHandle);
        cmd.intParams.push_back(getSelectedObjectID());
        cmd.boolParams.push_back(_xyGraph);
        App::appendSimulationThreadCommand(cmd);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlg2D3DGraphProperties::on_qqWidth_editingFinished()
{
    if (!ui->qqWidth->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        int newVal=ui->qqWidth->text().toInt(&ok);
        if (ok&&(!_xyGraph))
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_CURVEWIDTH_GRAPHCURVEGUITRIGGEREDCMD;
            cmd.intParams.push_back(_graphHandle);
            cmd.intParams.push_back(getSelectedObjectID());
            cmd.intParams.push_back(newVal);
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlg2D3DGraphProperties::on_qqRelativeToGraph_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (!_xyGraph)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_RELATIVETO_GRAPHCURVEGUITRIGGEREDCMD;
            cmd.intParams.push_back(_graphHandle);
            cmd.intParams.push_back(getSelectedObjectID());
            cmd.boolParams.push_back(false);
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlg2D3DGraphProperties::on_qqRelativeToWorld_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (!_xyGraph)
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=SET_RELATIVETO_GRAPHCURVEGUITRIGGEREDCMD;
            cmd.intParams.push_back(_graphHandle);
            cmd.intParams.push_back(getSelectedObjectID());
            cmd.boolParams.push_back(true);
            App::appendSimulationThreadCommand(cmd);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
            App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
        }
    }
}

void CQDlg2D3DGraphProperties::on_qqAlwaysOnTop_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=TOGGLE_ONTOP_GRAPHCURVEGUITRIGGEREDCMD;
        cmd.intParams.push_back(_graphHandle);
        cmd.intParams.push_back(getSelectedObjectID());
        cmd.boolParams.push_back(_xyGraph);
        App::appendSimulationThreadCommand(cmd);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}
