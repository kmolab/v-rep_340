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
#include "qdlgtranslations.h"
#include "ui_qdlgtranslations.h"
#include "gV.h"
#include "tt.h"
#include "app.h"
#include "v_repStrings.h"

int CQDlgTranslations::coordMode=0; //0=abs,1=rel to parent
float CQDlgTranslations::rotAngles[3]={0.0f,0.0f,0.0f};
float CQDlgTranslations::translationValues[3]={0.0f,0.0f,0.0f};
float CQDlgTranslations::scalingValues[3]={1.0f,1.0f,1.0f};
int CQDlgTranslations::transfMode=0; //0=abs,1=rel to parent,2=rel to self

CQDlgTranslations::CQDlgTranslations(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgTranslations)
{
    ui->setupUi(this);
    lastLastSelectionID=-1;
}

CQDlgTranslations::~CQDlgTranslations()
{
    delete ui;
}

void CQDlgTranslations::cancelEvent()
{
    // we override this cancel event. The container window should close, not this one!!
    App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgTranslations::refresh()
{
    QLineEdit* lineEditToSelect=getSelectedLineEdit();
    int editMode=App::getEditModeType();
    int lastSelID=App::ct->objCont->getLastSelectionID();
    lastLastSelectionID=lastSelID;

    if (editMode==NO_EDIT_MODE)
    {
        bool sel=(App::ct->objCont->getSelSize()!=0);
        bool bigSel=(App::ct->objCont->getSelSize()>1);
        _enableCoordinatePart(sel,true,bigSel,true);
        _enableCoordinatePart(sel,false,bigSel,false);
        _enableTransformationPart(sel,0,sel,true);
        _enableTransformationPart(sel,1,sel,false);
        _enableTransformationPart(sel&&(transfMode!=2),2,sel&&(transfMode!=2),false);
        C3DObject* object=App::ct->objCont->getLastSelection_object();
        if (sel&&(object!=NULL))
        {
            // Coordinate part:
            C3Vector euler,pos;
            if (coordMode==0)
            {
                euler=object->getCumulativeTransformationPart1().Q.getEulerAngles();
                pos=object->getCumulativeTransformationPart1().X;
            }
            else
            {
                euler=object->getLocalTransformationPart1().Q.getEulerAngles();
                pos=object->getLocalTransformationPart1().X;
            }
            ui->qqCoordX->setText(tt::getEString(true,pos(0),4).c_str());
            ui->qqCoordY->setText(tt::getEString(true,pos(1),4).c_str());
            ui->qqCoordZ->setText(tt::getEString(true,pos(2),4).c_str());
            ui->qqCoordWorld->setChecked(coordMode==0);
            ui->qqCoordParent->setChecked(coordMode==1);
            // Transformation part:
            _setValuesTransformationPart(0,true);
            _setValuesTransformationPart(1,false);
            _setValuesTransformationPart(2,false);
        }
        else
        {
            _setDefaultValuesCoordinatePart(true,true);
            _setDefaultValuesCoordinatePart(false,false);
            _setDefaultValuesTransformationPart(0,true);
            _setDefaultValuesTransformationPart(1,false);
            _setDefaultValuesTransformationPart(2,false);
        }
    }
    else
    { // We are in an edit mode
        if (editMode&(BUTTON_EDIT_MODE|TRIANGLE_EDIT_MODE|EDGE_EDIT_MODE))
        {
            _enableCoordinatePart(false,true,false,true);
            _enableCoordinatePart(false,false,false,false);
            _enableTransformationPart(false,0,false,true);
            _enableTransformationPart(false,1,false,false);
            _enableTransformationPart(false,2,false,false);
            _setDefaultValuesCoordinatePart(true,true);
            _setDefaultValuesCoordinatePart(false,false);
            _setDefaultValuesTransformationPart(0,true);
            _setDefaultValuesTransformationPart(1,false);
            _setDefaultValuesTransformationPart(2,false);
        }
        else
        { // Vertex or path edit mode
            if (editMode&VERTEX_EDIT_MODE)
            {
                bool sel=(App::mainWindow->editModeContainer->getEditModeBufferSize()!=0);
                bool bigSel=(App::mainWindow->editModeContainer->getEditModeBufferSize()>1);
                if (transfMode==2)
                    transfMode=1;

                _setDefaultValuesCoordinatePart(true,true);
                _enableCoordinatePart(sel,false,bigSel,true);
                _enableTransformationPart(sel,0,sel,false);
                _enableTransformationPart(sel,1,sel,false);
                _enableTransformationPart(sel&&(transfMode!=2),2,sel&&(transfMode!=2),false);
                ui->qqTransfWorld->setEnabled(sel);
                ui->qqTransfParent->setEnabled(sel);
                ui->qqTransfOwn->setEnabled(false);
                CShape* shape=App::mainWindow->editModeContainer->getEditModeShape();
                if (sel&&(shape!=NULL))
                {
                    // Coordinate part:
                    int ind=App::mainWindow->editModeContainer->getLastEditModeBufferValue();
                    C3Vector pos(App::mainWindow->editModeContainer->getShapeEditMode()->getEditionVertex(ind));
                    if (coordMode==0)
                        pos=shape->getCumulativeTransformation()*pos;
                    ui->qqCoordX->setText(tt::getEString(true,pos(0),4).c_str());
                    ui->qqCoordY->setText(tt::getEString(true,pos(1),4).c_str());
                    ui->qqCoordZ->setText(tt::getEString(true,pos(2),4).c_str());
                    ui->qqCoordWorld->setChecked(coordMode==0);
                    ui->qqCoordParent->setChecked(coordMode==1);
                    // Transformation part:
                    _setValuesTransformationPart(0,true);
                    _setValuesTransformationPart(1,false);
                    _setValuesTransformationPart(2,false);
                }
                else
                {
                    _setDefaultValuesCoordinatePart(false,false);
                    _setDefaultValuesTransformationPart(0,true);
                    _setDefaultValuesTransformationPart(1,false);
                    _setDefaultValuesTransformationPart(2,false);
                }
            }
            if (editMode&PATH_EDIT_MODE)
            {
                bool sel=(App::mainWindow->editModeContainer->getEditModeBufferSize()!=0);
                bool bigSel=(App::mainWindow->editModeContainer->getEditModeBufferSize()>1);

                _enableCoordinatePart(sel,true,bigSel,true);
                _enableCoordinatePart(sel,false,bigSel,false);
                _enableTransformationPart(sel,0,sel,true);
                _enableTransformationPart(sel,1,sel,false);
                _enableTransformationPart(sel&&(transfMode!=2),2,sel&&(transfMode!=2),false);
                if (sel)
                {
                    CPath* path=App::mainWindow->editModeContainer->getEditModePath();
                    int ind=App::mainWindow->editModeContainer->getLastEditModeBufferValue();
                    CSimplePathPoint* pp=App::mainWindow->editModeContainer->getEditModePathContainer()->getSimplePathPoint(ind);
                    if (sel&&(path!=NULL)&&(pp!=NULL))
                    {
                        // Coordinate part:
                        C7Vector tr(pp->getTransformation());
                        if (coordMode==0)
                            tr=path->getCumulativeTransformation()*tr;
                        C3Vector euler(tr.Q.getEulerAngles());
                        ui->qqCoordX->setText(tt::getEString(true,tr.X(0),4).c_str());
                        ui->qqCoordY->setText(tt::getEString(true,tr.X(1),4).c_str());
                        ui->qqCoordZ->setText(tt::getEString(true,tr.X(2),4).c_str());
                        ui->qqCoordWorld->setChecked(coordMode==0);
                        ui->qqCoordParent->setChecked(coordMode==1);
                        // Transformation part:
                        _setValuesTransformationPart(0,true);
                        _setValuesTransformationPart(1,false);
                        _setValuesTransformationPart(2,false);
                    }
                    else
                    {
                        _setDefaultValuesCoordinatePart(false,false);
                        _setDefaultValuesTransformationPart(0,true);
                        _setDefaultValuesTransformationPart(1,false);
                        _setDefaultValuesTransformationPart(2,false);
                    }
                }
                else
                {
                    _setDefaultValuesCoordinatePart(true,true);
                    _setDefaultValuesCoordinatePart(false,false);
                    _setDefaultValuesTransformationPart(0,true);
                    _setDefaultValuesTransformationPart(1,false);
                    _setDefaultValuesTransformationPart(2,false);
                }
            }
        }
    }
    selectLineEdit(lineEditToSelect);
}

void CQDlgTranslations::_enableCoordinatePart(bool enableState,bool anglePart,bool enableButtons,bool alsoRadioButtons)
{
    if (!anglePart)
    {
        ui->qqCoordX->setEnabled(enableState);
        ui->qqCoordY->setEnabled(enableState);
        ui->qqCoordZ->setEnabled(enableState);
        ui->qqCoordApplyPos->setEnabled(enableButtons);
        ui->qqCoordApplyX->setEnabled(enableButtons);
        ui->qqCoordApplyY->setEnabled(enableButtons);
        ui->qqCoordApplyZ->setEnabled(enableButtons);
    }
    if (alsoRadioButtons)
    {
        ui->qqCoordWorld->setEnabled(enableState);
        ui->qqCoordParent->setEnabled(enableState);
    }
}

void CQDlgTranslations::_enableTransformationPart(bool enableState,int partIndex,bool enableButtons,bool alsoRadioButtons)
{
    if (partIndex==1)
    {
        ui->qqTransfX->setEnabled(enableState);
        ui->qqTransfY->setEnabled(enableState);
        ui->qqTransfZ->setEnabled(enableState);
        ui->qqTransfApplyPos->setEnabled(enableButtons);
    }
    if (partIndex==2)
    {
        ui->qqTransfSX->setEnabled(enableState);
        ui->qqTransfSY->setEnabled(enableState);
        ui->qqTransfSZ->setEnabled(enableState);
        ui->qqTransfApplyScale->setEnabled(enableButtons);
    }
    if (alsoRadioButtons)
    {
        ui->qqTransfWorld->setEnabled(enableState);
        ui->qqTransfParent->setEnabled(enableState);
        ui->qqTransfOwn->setEnabled(enableState);
    }
}

void CQDlgTranslations::_setDefaultValuesCoordinatePart(bool anglePart,bool alsoRadioButtons)
{
    if (!anglePart)
    {
        ui->qqCoordX->setText("");
        ui->qqCoordY->setText("");
        ui->qqCoordZ->setText("");
    }
    if (alsoRadioButtons)
    {
        ui->qqCoordWorld->setChecked(false);
        ui->qqCoordParent->setChecked(false);
    }
}

void CQDlgTranslations::_setDefaultValuesTransformationPart(int partIndex,bool alsoRadioButtons)
{
    if (partIndex==1)
    {
        ui->qqTransfX->setText("");
        ui->qqTransfY->setText("");
        ui->qqTransfZ->setText("");
    }
    if (partIndex==2)
    {
        ui->qqTransfSX->setText("");
        ui->qqTransfSY->setText("");
        ui->qqTransfSZ->setText("");
    }
    if (alsoRadioButtons)
    {
        ui->qqTransfWorld->setChecked(false);
        ui->qqTransfParent->setChecked(false);
        ui->qqTransfOwn->setChecked(false);
    }
}

void CQDlgTranslations::_setValuesTransformationPart(int partIndex,bool alsoRadioButtons)
{
    if (partIndex==1)
    {
        ui->qqTransfX->setText(tt::getEString(true,translationValues[0],4).c_str());
        ui->qqTransfY->setText(tt::getEString(true,translationValues[1],4).c_str());
        ui->qqTransfZ->setText(tt::getEString(true,translationValues[2],4).c_str());
    }
    if (partIndex==2)
    {
        ui->qqTransfSX->setText(tt::getEString(true,scalingValues[0],3).c_str());
        ui->qqTransfSY->setText(tt::getEString(true,scalingValues[1],3).c_str());
        ui->qqTransfSZ->setText(tt::getEString(true,scalingValues[2],3).c_str());
    }
    if (alsoRadioButtons)
    {
        ui->qqTransfWorld->setChecked(transfMode==0);
        ui->qqTransfParent->setChecked(transfMode==1);
        ui->qqTransfOwn->setChecked(transfMode==2);
    }
}

bool CQDlgTranslations::_setCoord_userUnit(float newValueInUserUnit,int index)
{
    bool retVal=false;
    int editMode=App::getEditModeType();
    C3DObject* object=App::ct->objCont->getLastSelection_object();
    if ( (editMode==NO_EDIT_MODE)&&(object!=NULL) )
    {
        C7Vector tr;
        if (coordMode==0)
            tr=object->getCumulativeTransformationPart1();
        else
            tr=object->getLocalTransformationPart1();
        tr=_getNewTransf(tr,newValueInUserUnit,index);

        SSimulationThreadCommand cmd;
        cmd.cmdId=SET_TRANSF_POSITIONTRANSLATIONGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->objCont->getLastSelectionID());
        cmd.intParams.push_back(coordMode);
        cmd.transfParams.push_back(tr);
        App::appendSimulationThreadCommand(cmd);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
    if ( (editMode&PATH_EDIT_MODE)&&(App::mainWindow->editModeContainer->getEditModeBufferSize()!=0)&&(App::mainWindow->editModeContainer->getEditModePathContainer()!=NULL) )
    {
        CPathCont* pathCont=App::mainWindow->editModeContainer->getEditModePathContainer();
        int ind=App::mainWindow->editModeContainer->getLastEditModeBufferValue();
        CSimplePathPoint* pp=pathCont->getSimplePathPoint(ind);
        CPath* path=App::mainWindow->editModeContainer->getEditModePath();
        if ( (pp!=NULL)&&(path!=NULL) )
        {
            C7Vector tr(pp->getTransformation());
            if (coordMode==0)
                tr=path->getCumulativeTransformationPart1()*tr;
            tr=_getNewTransf(tr,newValueInUserUnit,index);
            if (coordMode==0)
                pp->setTransformation(path->getCumulativeTransformation().getInverse()*tr,pathCont->getAttributes());
            else
                pp->setTransformation(tr,pathCont->getAttributes());
            pathCont->actualizePath();
        }
        retVal=true;
    }
    if ( (editMode&VERTEX_EDIT_MODE)&&(App::mainWindow->editModeContainer->getEditModeBufferSize()!=0) )
    {
        int ind=App::mainWindow->editModeContainer->getLastEditModeBufferValue();
        C3Vector v(App::mainWindow->editModeContainer->getShapeEditMode()->getEditionVertex(ind));
        CShape* shape=App::mainWindow->editModeContainer->getEditModeShape();
        if (shape!=NULL)
        {
            C7Vector tr;
            tr.setIdentity();
            tr.X=v;
            if (coordMode==0)
                tr=shape->getCumulativeTransformationPart1()*tr;
            tr=_getNewTransf(tr,newValueInUserUnit,index);
            if (coordMode==0)
                tr=shape->getCumulativeTransformation().getInverse()*tr;
            App::mainWindow->editModeContainer->getShapeEditMode()->setEditionVertex(ind,tr.X);
        }
        retVal=true;
    }
    return(retVal);
}

C7Vector CQDlgTranslations::_getNewTransf(const C7Vector& transf,float newValueInUserUnit,int index)
{
    C7Vector retVal(transf);
    retVal.X(index)=newValueInUserUnit;
    return(retVal);
}

bool CQDlgTranslations::_applyCoord(int mask)
{
    bool retVal=false;
    int editMode=App::getEditModeType();
    C3DObject* object=App::ct->objCont->getLastSelection_object();
    int objSelSize=App::ct->objCont->getSelSize();
    int editObjSelSize=App::mainWindow->editModeContainer->getEditModeBufferSize();
    if ( (editMode==NO_EDIT_MODE)&&(object!=NULL)&&(objSelSize>1) )
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=APPLY_POS_POSITIONTRANSLATIONGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->objCont->getLastSelectionID());
        for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
            cmd.intParams.push_back(App::ct->objCont->getSelID(i));
        cmd.intParams.push_back(coordMode);
        cmd.intParams.push_back(mask);
        App::appendSimulationThreadCommand(cmd);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
    if ( (editMode&PATH_EDIT_MODE)&&(editObjSelSize>1)&&(App::mainWindow->editModeContainer->getEditModePathContainer()!=NULL) )
    {
        CPathCont* pathCont=App::mainWindow->editModeContainer->getEditModePathContainer();
        int ind=App::mainWindow->editModeContainer->getLastEditModeBufferValue();
        CSimplePathPoint* pp=pathCont->getSimplePathPoint(ind);
        CPath* path=App::mainWindow->editModeContainer->getEditModePath();
        if ( (pp!=NULL)&&(path!=NULL) )
        {
            C7Vector tr(pp->getTransformation());
            if (coordMode==0)
                tr=path->getCumulativeTransformationPart1()*tr;
            for (int i=0;i<editObjSelSize-1;i++)
            {
                CSimplePathPoint* ppIt=App::mainWindow->editModeContainer->getPathEditMode()->getSimplePathPoint(i);
                if (ppIt!=NULL)
                {
                    C7Vector trIt(ppIt->getTransformation());
                    if (coordMode==0)
                        trIt=path->getCumulativeTransformationPart1()*trIt;
                    _copyTransf(tr,trIt,mask);
                    if (coordMode==0)
                        trIt=path->getCumulativeTransformationPart1().getInverse()*trIt;
                    ppIt->setTransformation(trIt,pathCont->getAttributes());
                }
            }
            pathCont->actualizePath();
        }
        retVal=true;
    }
    if ( (editMode&VERTEX_EDIT_MODE)&&(editObjSelSize>1) )
    {
        int ind=App::mainWindow->editModeContainer->getLastEditModeBufferValue();
        C3Vector v(App::mainWindow->editModeContainer->getShapeEditMode()->getEditionVertex(ind));
        CShape* shape=App::mainWindow->editModeContainer->getEditModeShape();
        if (shape!=NULL)
        {
            C7Vector tr;
            tr.setIdentity();
            tr.X=v;
            if (coordMode==0)
                tr=shape->getCumulativeTransformationPart1()*tr;
            for (int i=0;i<editObjSelSize-1;i++)
            {
                ind=App::mainWindow->editModeContainer->getEditModeBufferValue(i);
                C7Vector trIt;
                trIt.setIdentity();
                trIt.X=App::mainWindow->editModeContainer->getShapeEditMode()->getEditionVertex(ind);
                if (coordMode==0)
                    trIt=shape->getCumulativeTransformationPart1()*trIt;
                _copyTransf(tr,trIt,mask);
                if (coordMode==0)
                    trIt=shape->getCumulativeTransformationPart1().getInverse()*trIt;
                App::mainWindow->editModeContainer->getShapeEditMode()->setEditionVertex(ind,trIt.X);
            }
        }
        retVal=true;
    }
    return(retVal);
}

void CQDlgTranslations::_copyTransf(const C7Vector& tr,C7Vector& trIt,int mask)
{
    if (mask&1)
        trIt.X(0)=tr.X(0);
    if (mask&2)
        trIt.X(1)=tr.X(1);
    if (mask&4)
        trIt.X(2)=tr.X(2);
}

bool CQDlgTranslations::_applyTransformation(int t)
{ // t==0: rotation, t==1: translation, t==2: scaling
    bool retVal=false;
    int editMode=App::getEditModeType();
    int objSelSize=App::ct->objCont->getSelSize();
    int editObjSelSize=App::mainWindow->editModeContainer->getEditModeBufferSize();
    if ( (editMode==NO_EDIT_MODE)&&(objSelSize>0) )
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=TRANSLATESCALE_SELECTION_POSITIONTRANSLATIONGUITRIGGEREDCMD;
        for (int i=0;i<App::ct->objCont->getSelSize();i++)
            cmd.intParams.push_back(App::ct->objCont->getSelID(i));
        cmd.intParams.push_back(transfMode);
        cmd.intParams.push_back(t);
        cmd.floatParams.push_back(scalingValues[0]);
        cmd.floatParams.push_back(scalingValues[1]);
        cmd.floatParams.push_back(scalingValues[2]);
        cmd.floatParams.push_back(translationValues[0]);
        cmd.floatParams.push_back(translationValues[1]);
        cmd.floatParams.push_back(translationValues[2]);
        App::appendSimulationThreadCommand(cmd);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
    if ( (editMode&PATH_EDIT_MODE)&&(editObjSelSize>0)&&(App::mainWindow->editModeContainer->getEditModePathContainer()!=NULL) )
    {
        CPathCont* pathCont=App::mainWindow->editModeContainer->getEditModePathContainer();
        CPath* path=App::mainWindow->editModeContainer->getEditModePath();
        for (int i=0;i<editObjSelSize;i++)
        {
            CSimplePathPoint* pp=App::mainWindow->editModeContainer->getPathEditMode()->getSimplePathPoint(i);
            if ( (pp!=NULL)&&(path!=NULL) )
            {
                C7Vector tr(pp->getTransformation());
                if (transfMode==0)
                    tr=path->getCumulativeTransformationPart1()*tr;
                _transform(tr,t,transfMode==2);
                if (transfMode==0)
                    tr=path->getCumulativeTransformationPart1().getInverse()*tr;
                pp->setTransformation(tr,pathCont->getAttributes());
            }
        }
        pathCont->actualizePath();
        retVal=true;
    }
    if ( (editMode&VERTEX_EDIT_MODE)&&(editObjSelSize>0) )
    {
        CShape* shape=App::mainWindow->editModeContainer->getEditModeShape();
        if (shape!=NULL)
        {
            for (int i=0;i<editObjSelSize;i++)
            {
                C7Vector tr;
                tr.setIdentity();
                int ind=App::mainWindow->editModeContainer->getEditModeBufferValue(i);
                tr.X=App::mainWindow->editModeContainer->getShapeEditMode()->getEditionVertex(ind);
                if (transfMode==0)
                    tr=shape->getCumulativeTransformationPart1()*tr;
                _transform(tr,t,transfMode==2);
                if (transfMode==0)
                    tr=shape->getCumulativeTransformationPart1().getInverse()*tr;
                App::mainWindow->editModeContainer->getShapeEditMode()->setEditionVertex(ind,tr.X);
            }
        }
        retVal=true;
    }
    return(retVal);
}

void CQDlgTranslations::_transform(C7Vector& tr,int t,bool self)
{ // t==0: rotation, t==1: translation, t==2: scaling
    if (t==2)
    {
        tr.X(0)=tr.X(0)*scalingValues[0];
        tr.X(1)=tr.X(1)*scalingValues[1];
        tr.X(2)=tr.X(2)*scalingValues[2];
    }
    else
    {
        C7Vector m;
        m.setIdentity();
        if (t==0)
            m.Q.setEulerAngles(rotAngles[0],rotAngles[1],rotAngles[2]);
        if (t==1)
            m.X.set(translationValues);
        if (self)
            tr=tr*m;
        else
            tr=m*tr;
    }
}


void CQDlgTranslations::on_qqCoordWorld_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        coordMode=0;
        refresh();
    }
}

void CQDlgTranslations::on_qqCoordParent_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        coordMode=1;
        refresh();
    }
}

void CQDlgTranslations::on_qqCoordX_editingFinished()
{
    if (!ui->qqCoordX->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqCoordX->text().toFloat(&ok);
        if (ok)
        {
            if (_setCoord_userUnit(newVal,0))
                refresh();
        }
    }
}

void CQDlgTranslations::on_qqCoordY_editingFinished()
{
    if (!ui->qqCoordY->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqCoordY->text().toFloat(&ok);
        if (ok)
        {
            if (_setCoord_userUnit(newVal,1))
                refresh();
        }
    }
}

void CQDlgTranslations::on_qqCoordZ_editingFinished()
{
    if (!ui->qqCoordZ->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqCoordZ->text().toFloat(&ok);
        if (ok)
        {
            if (_setCoord_userUnit(newVal,2))
                refresh();
        }
    }
}

void CQDlgTranslations::on_qqCoordApplyPos_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (_applyCoord(1+2+4))
            refresh();
    }
}

void CQDlgTranslations::on_qqCoordApplyX_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (_applyCoord(1))
            refresh();
    }
}

void CQDlgTranslations::on_qqCoordApplyY_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (_applyCoord(2))
            refresh();
    }
}

void CQDlgTranslations::on_qqCoordApplyZ_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (_applyCoord(4))
            refresh();
    }
}

void CQDlgTranslations::on_qqTransfWorld_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        transfMode=0;
        refresh();
    }
}

void CQDlgTranslations::on_qqTransfParent_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        transfMode=1;
        refresh();
    }
}

void CQDlgTranslations::on_qqTransfOwn_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        transfMode=2;
        refresh();
    }
}


void CQDlgTranslations::on_qqTransfX_editingFinished()
{
    if (!ui->qqTransfX->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqTransfX->text().toFloat(&ok);
        if (ok)
        {
            tt::limitValue(-1000000.0f,+1000000.0f,newVal);
            translationValues[0]=newVal;
        }
        refresh();
    }
}

void CQDlgTranslations::on_qqTransfY_editingFinished()
{
    if (!ui->qqTransfY->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqTransfY->text().toFloat(&ok);
        if (ok)
        {
            tt::limitValue(-1000000.0f,+1000000.0f,newVal);
            translationValues[1]=newVal;
        }
        refresh();
    }
}

void CQDlgTranslations::on_qqTransfZ_editingFinished()
{
    if (!ui->qqTransfZ->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqTransfZ->text().toFloat(&ok);
        if (ok)
        {
            tt::limitValue(-1000000.0f,+1000000.0f,newVal);
            translationValues[2]=newVal;
        }
        refresh();
    }
}

void CQDlgTranslations::on_qqTransfApplyPos_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (_applyTransformation(1))
            refresh();
    }
}

void CQDlgTranslations::on_qqTransfSX_editingFinished()
{
    if (!ui->qqTransfSX->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqTransfSX->text().toFloat(&ok);
        if (ok)
        {
            tt::limitValue(-1000000.0f,1000000.0f,newVal);
            scalingValues[0]=newVal;
        }
        refresh();
    }
}

void CQDlgTranslations::on_qqTransfSY_editingFinished()
{
    if (!ui->qqTransfSY->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqTransfSY->text().toFloat(&ok);
        if (ok)
        {
            tt::limitValue(-1000000.0f,1000000.0f,newVal);
            scalingValues[1]=newVal;
        }
        refresh();
    }
}

void CQDlgTranslations::on_qqTransfSZ_editingFinished()
{
    if (!ui->qqTransfSZ->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqTransfSZ->text().toFloat(&ok);
        if (ok)
        {
            tt::limitValue(-1000000.0f,1000000.0f,newVal);
            scalingValues[2]=newVal;
        }
        refresh();
    }
}

void CQDlgTranslations::on_qqTransfApplyScale_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (_applyTransformation(2))
            refresh();
    }
}





