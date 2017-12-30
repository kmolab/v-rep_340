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
#include "qdlglightmaterial.h"
#include "ui_qdlglightmaterial.h"
#include "gV.h"
#include "tt.h"
#include "app.h"

CQDlgLightMaterial::CQDlgLightMaterial(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgLightMaterial)
{
    _dlgType=LIGHTMATERIAL_DLG;
    ui->setupUi(this);
    _lastSelectedObjectID=App::ct->objCont->getLastSelectionID();
    _objectSelectionSize=App::ct->objCont->getSelSize();
    if (App::mainWindow!=NULL)
        App::mainWindow->dlgCont->close(COLOR_DLG);
}

CQDlgLightMaterial::~CQDlgLightMaterial()
{
    delete ui;
    if (App::mainWindow!=NULL)
        App::mainWindow->dlgCont->close(COLOR_DLG);
}

void CQDlgLightMaterial::refresh()
{
    int allowedParts=0; // Bit-coded: 1=ambient/difuse, 2=spec, 4=emiss., 8=aux channels, 16=pulsation, 32=shininess, 64=opacity, 128=colorName, 256=ext. string
    App::getVisualParamPointerFromItem(_objType,_objID1,_objID2,NULL,&allowedParts);
    bool simStopped=App::ct->simulation->isSimulationStopped();
    ui->qqDiffuseAdjust->setEnabled(simStopped&&(allowedParts&2));
    ui->qqSpecularAdjust->setEnabled(simStopped&&(allowedParts&4));
}

bool CQDlgLightMaterial::needsDestruction()
{
    if (!isLinkedDataValid())
        return(true);
    return(CDlgEx::needsDestruction());
}

bool CQDlgLightMaterial::isLinkedDataValid()
{
    if (_lastSelectedObjectID!=App::ct->objCont->getLastSelectionID())
        return(false);
    if (_objectSelectionSize!=App::ct->objCont->getSelSize())
        return(false);
    if (!App::ct->simulation->isSimulationStopped())
        return(false);
    return(App::getVisualParamPointerFromItem(_objType,_objID1,_objID2,NULL,NULL)!=NULL);
}


void CQDlgLightMaterial::displayMaterialDlg(int objType,int objID1,int objID2,QWidget* theParentWindow)
{
    if (App::mainWindow==NULL)
        return;

    App::mainWindow->dlgCont->close(LIGHTMATERIAL_DLG);
    App::mainWindow->dlgCont->close(COLOR_DLG);
    if (App::mainWindow->dlgCont->openOrBringToFront(LIGHTMATERIAL_DLG))
    {
        CQDlgLightMaterial* mat=(CQDlgLightMaterial*)App::mainWindow->dlgCont->getDialog(LIGHTMATERIAL_DLG);
        if (mat!=NULL)
            mat->_initializeDlg(objType,objID1,objID2);
    }
}

void CQDlgLightMaterial::_initializeDlg(int objType,int objID1,int objID2)
{
    _objType=objType;
    _objID1=objID1;
    _objID2=objID2;
    std::string str;
    App::getRGBPointerFromItem(_objType,_objID1,_objID2,-1,&str);
    setWindowTitle(str.c_str());
    refresh();
}

void CQDlgLightMaterial::_adjustCol(int colComponent)
{
    CQDlgColor::displayDlg(_objType,_objID1,_objID2,colComponent,App::mainWindow,false,true,true);
}

void CQDlgLightMaterial::cancelEvent()
{ // We just hide the dialog and destroy it at next rendering pass
    if (((QDialog*)this)->isModal()) // this condition and next line on 31/3/2013: on Linux the dlg couldn't be closed!
        defaultModalDialogEndRoutine(false);
    else
        CDlgEx::cancelEvent();
}

bool CQDlgLightMaterial::doesInstanceSwitchRequireDestruction()
{
    return(true);
}

void CQDlgLightMaterial::on_qqDiffuseAdjust_clicked()
{
    _adjustCol(sim_colorcomponent_diffuse);
}

void CQDlgLightMaterial::on_qqSpecularAdjust_clicked()
{
    _adjustCol(sim_colorcomponent_specular);
}

