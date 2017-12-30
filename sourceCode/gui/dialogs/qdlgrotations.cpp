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
#include "qdlgrotations.h"
#include "ui_qdlgrotations.h"
#include "gV.h"
#include "tt.h"
#include "app.h"
#include "v_repStrings.h"

int CQDlgRotations::coordMode=0; //0=abs,1=rel to parent
float CQDlgRotations::rotAngles[3]={0.0f,0.0f,0.0f};
float CQDlgRotations::translationValues[3]={0.0f,0.0f,0.0f};
float CQDlgRotations::scalingValues[3]={1.0f,1.0f,1.0f};
int CQDlgRotations::transfMode=0; //0=abs,1=rel to parent,2=rel to self


CQDlgRotations::CQDlgRotations(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgRotations)
{
    ui->setupUi(this);
    lastLastSelectionID=-1;
}

CQDlgRotations::~CQDlgRotations()
{
    delete ui;
}

void CQDlgRotations::cancelEvent()
{
    // we override this cancel event. The container window should close, not this one!!
    App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgRotations::refresh()
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
            ui->qqCoordAlpha->setText(tt::getAngleEString(true,euler(0),4).c_str());
            ui->qqCoordBeta->setText(tt::getAngleEString(true,euler(1),4).c_str());
            ui->qqCoordGamma->setText(tt::getAngleEString(true,euler(2),4).c_str());
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
                if (transfMode==2)
                    transfMode=1;

                _enableCoordinatePart(false,true,false,true);
                _setDefaultValuesCoordinatePart(true,true);
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
                        ui->qqCoordAlpha->setText(tt::getAngleEString(true,euler(0),4).c_str());
                        ui->qqCoordBeta->setText(tt::getAngleEString(true,euler(1),4).c_str());
                        ui->qqCoordGamma->setText(tt::getAngleEString(true,euler(2),4).c_str());
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

void CQDlgRotations::_enableCoordinatePart(bool enableState,bool anglePart,bool enableButtons,bool alsoRadioButtons)
{
    if (anglePart)
    {
        ui->qqCoordAlpha->setEnabled(enableState);
        ui->qqCoordBeta->setEnabled(enableState);
        ui->qqCoordGamma->setEnabled(enableState);
        ui->qqCoordApplyOr->setEnabled(enableButtons);
    }
    if (alsoRadioButtons)
    {
        ui->qqCoordWorld->setEnabled(enableState);
        ui->qqCoordParent->setEnabled(enableState);
    }
}

void CQDlgRotations::_enableTransformationPart(bool enableState,int partIndex,bool enableButtons,bool alsoRadioButtons)
{
    if (partIndex==0)
    {
        ui->qqTransfAlpha->setEnabled(enableState);
        ui->qqTransfBeta->setEnabled(enableState);
        ui->qqTransfGamma->setEnabled(enableState);
        ui->qqTransfApplyOr->setEnabled(enableButtons);
    }
    if (alsoRadioButtons)
    {
        ui->qqTransfWorld->setEnabled(enableState);
        ui->qqTransfParent->setEnabled(enableState);
        ui->qqTransfOwn->setEnabled(enableState);
    }
}

void CQDlgRotations::_setDefaultValuesCoordinatePart(bool anglePart,bool alsoRadioButtons)
{
    if (anglePart)
    {
        ui->qqCoordAlpha->setText("");
        ui->qqCoordBeta->setText("");
        ui->qqCoordGamma->setText("");
    }
    if (alsoRadioButtons)
    {
        ui->qqCoordWorld->setChecked(false);
        ui->qqCoordParent->setChecked(false);
    }
}

void CQDlgRotations::_setDefaultValuesTransformationPart(int partIndex,bool alsoRadioButtons)
{
    if (partIndex==0)
    {
        ui->qqTransfAlpha->setText("");
        ui->qqTransfBeta->setText("");
        ui->qqTransfGamma->setText("");
    }
    if (alsoRadioButtons)
    {
        ui->qqTransfWorld->setChecked(false);
        ui->qqTransfParent->setChecked(false);
        ui->qqTransfOwn->setChecked(false);
    }
}

void CQDlgRotations::_setValuesTransformationPart(int partIndex,bool alsoRadioButtons)
{
    if (partIndex==0)
    {
        ui->qqTransfAlpha->setText(tt::getAngleEString(true,rotAngles[0],4).c_str());
        ui->qqTransfBeta->setText(tt::getAngleEString(true,rotAngles[1],4).c_str());
        ui->qqTransfGamma->setText(tt::getAngleEString(true,rotAngles[2],4).c_str());
    }
    if (alsoRadioButtons)
    {
        ui->qqTransfWorld->setChecked(transfMode==0);
        ui->qqTransfParent->setChecked(transfMode==1);
        ui->qqTransfOwn->setChecked(transfMode==2);
    }
}

bool CQDlgRotations::_setCoord_userUnit(float newValueInUserUnit,int index)
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

C7Vector CQDlgRotations::_getNewTransf(const C7Vector& transf,float newValueInUserUnit,int index)
{
    C7Vector retVal(transf);
    C3Vector euler(retVal.Q.getEulerAngles());
    euler(index)=newValueInUserUnit*gv::userToRad;
    retVal.Q.setEulerAngles(euler(0),euler(1),euler(2));
    return(retVal);
}

bool CQDlgRotations::_applyCoord()
{
    bool retVal=false;
    int editMode=App::getEditModeType();
    C3DObject* object=App::ct->objCont->getLastSelection_object();
    int objSelSize=App::ct->objCont->getSelSize();
    int editObjSelSize=App::mainWindow->editModeContainer->getEditModeBufferSize();
    if ( (editMode==NO_EDIT_MODE)&&(object!=NULL)&&(objSelSize>1) )
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=APPLY_OR_ORIENTATIONROTATIONGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->objCont->getLastSelectionID());
        for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
            cmd.intParams.push_back(App::ct->objCont->getSelID(i));
        cmd.intParams.push_back(coordMode);
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
                    trIt.Q=tr.Q;
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
                trIt.Q=tr.Q;
                if (coordMode==0)
                    trIt=shape->getCumulativeTransformationPart1().getInverse()*trIt;
                App::mainWindow->editModeContainer->getShapeEditMode()->setEditionVertex(ind,trIt.X);
            }
        }
        retVal=true;
    }
    return(retVal);
}

bool CQDlgRotations::_applyTransformation()
{
    bool retVal=false;
    int editMode=App::getEditModeType();
    int objSelSize=App::ct->objCont->getSelSize();
    int editObjSelSize=App::mainWindow->editModeContainer->getEditModeBufferSize();
    if ( (editMode==NO_EDIT_MODE)&&(objSelSize>0) )
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=ROTATE_SELECTION_ORIENTATIONROTATIONGUITRIGGEREDCMD;
        for (int i=0;i<App::ct->objCont->getSelSize();i++)
            cmd.intParams.push_back(App::ct->objCont->getSelID(i));
        cmd.intParams.push_back(transfMode);
        cmd.floatParams.push_back(rotAngles[0]);
        cmd.floatParams.push_back(rotAngles[1]);
        cmd.floatParams.push_back(rotAngles[2]);
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
                _transform(tr,transfMode==2);
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
                _transform(tr,transfMode==2);
                if (transfMode==0)
                    tr=shape->getCumulativeTransformationPart1().getInverse()*tr;
                App::mainWindow->editModeContainer->getShapeEditMode()->setEditionVertex(ind,tr.X);
            }
        }
        retVal=true;
    }
    return(retVal);
}

void CQDlgRotations::_transform(C7Vector& tr,bool self)
{
    C7Vector m;
    m.setIdentity();
    m.Q.setEulerAngles(rotAngles[0],rotAngles[1],rotAngles[2]);
    if (self)
        tr=tr*m;
    else
        tr=m*tr;
}


void CQDlgRotations::on_qqCoordWorld_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        coordMode=0;
        refresh();
    }
}

void CQDlgRotations::on_qqCoordParent_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        coordMode=1;
        refresh();
    }
}

void CQDlgRotations::on_qqCoordAlpha_editingFinished()
{
    if (!ui->qqCoordAlpha->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqCoordAlpha->text().toFloat(&ok);
        if (ok)
        {
            if (_setCoord_userUnit(newVal,0))
                refresh();
        }
    }
}

void CQDlgRotations::on_qqCoordBeta_editingFinished()
{
    if (!ui->qqCoordBeta->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqCoordBeta->text().toFloat(&ok);
        if (ok)
        {
            if (_setCoord_userUnit(newVal,1))
                refresh();
        }
    }
}

void CQDlgRotations::on_qqCoordGamma_editingFinished()
{
    if (!ui->qqCoordGamma->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqCoordGamma->text().toFloat(&ok);
        if (ok)
        {
            if (_setCoord_userUnit(newVal,2))
                refresh();
        }
    }
}

void CQDlgRotations::on_qqCoordApplyOr_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (_applyCoord())
            refresh();
    }
}

void CQDlgRotations::on_qqTransfWorld_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        transfMode=0;
        refresh();
    }
}

void CQDlgRotations::on_qqTransfParent_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        transfMode=1;
        refresh();
    }
}

void CQDlgRotations::on_qqTransfOwn_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        transfMode=2;
        refresh();
    }
}

void CQDlgRotations::on_qqTransfAlpha_editingFinished()
{
    if (!ui->qqTransfAlpha->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqTransfAlpha->text().toFloat(&ok);
        if (ok)
        {
            newVal*=gv::userToRad;
            newVal=fmod(newVal,piValTimes2);
            rotAngles[0]=newVal;
        }
        refresh();
    }
}

void CQDlgRotations::on_qqTransfBeta_editingFinished()
{
    if (!ui->qqTransfBeta->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqTransfBeta->text().toFloat(&ok);
        if (ok)
        {
            newVal*=gv::userToRad;
            newVal=fmod(newVal,piValTimes2);
            rotAngles[1]=newVal;
        }
        refresh();
    }
}

void CQDlgRotations::on_qqTransfGamma_editingFinished()
{
    if (!ui->qqTransfGamma->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqTransfGamma->text().toFloat(&ok);
        if (ok)
        {
            newVal*=gv::userToRad;
            newVal=fmod(newVal,piValTimes2);
            rotAngles[2]=newVal;
        }
        refresh();
    }
}

void CQDlgRotations::on_qqTransfApplyOr_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (_applyTransformation())
            refresh();
    }
}

