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
#include "qdlgshapes.h"
#include "ui_qdlgshapes.h"
#include "tt.h"
#include "gV.h"
#include "qdlgmaterial.h"
#include "geometric.h"
#include "qdlgtextures.h"
#include "qdlggeometry.h"
#include "qdlgshapedyn.h"
#include "vFileDialog.h"
#include "app.h"
#include "imgLoaderSaver.h"
#include "v_repStrings.h"
#include "vMessageBox.h"

CQDlgShapes::CQDlgShapes(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgShapes)
{
    _dlgType=SHAPE_DLG;
    ui->setupUi(this);
}

CQDlgShapes::~CQDlgShapes()
{
    delete ui;
}

void CQDlgShapes::cancelEvent()
{
    // we override this cancel event. The container window should close, not this one!!
    App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgShapes::refresh()
{
    QLineEdit* lineEditToSelect=getSelectedLineEdit();
    bool noEditModeAndNoSim=(App::getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped();

    bool sel=App::ct->objCont->isLastSelectionAShape();
    bool ssel=App::ct->objCont->isLastSelectionASimpleShape();
    int sc=App::ct->objCont->getShapeNumberInSelection();
    int ssc=App::ct->objCont->getSimpleShapeNumberInSelection();
    bool compoundShapeDisplay=(sel&&(!ssel));
    CShape* it=App::ct->objCont->getLastSelection_shape();

    ui->qqEditMultishape->setEnabled(compoundShapeDisplay&&noEditModeAndNoSim);
    ui->qqEditMultishape->setVisible(compoundShapeDisplay);
    ui->qqColorGroup->setVisible(!compoundShapeDisplay);
    ui->qqOtherPropGroup->setVisible(!compoundShapeDisplay);
    ui->qqApplyColors->setVisible(!compoundShapeDisplay);
    ui->qqApplyMain->setVisible(!compoundShapeDisplay);

    ui->qqShadingAngle->setEnabled(ssel&&noEditModeAndNoSim);
    ui->qqEdgesAngle->setEnabled(ssel&&noEditModeAndNoSim);
    ui->qqBackfaceCulling->setEnabled(ssel&&noEditModeAndNoSim);
    ui->qqWireframe->setEnabled(ssel&&noEditModeAndNoSim);
    ui->qqShowEdges->setEnabled(ssel&&noEditModeAndNoSim);
    ui->qqHiddenBorder->setEnabled(ssel&&noEditModeAndNoSim);

    ui->qqApplyColors->setEnabled(ssel&&(ssc>1)&&noEditModeAndNoSim);
    ui->qqAdjustOutsideColor->setEnabled(ssel&&noEditModeAndNoSim);
    ui->qqInvertFaces->setEnabled(sel&&noEditModeAndNoSim);

    ui->qqGeometry->setEnabled(sel&&noEditModeAndNoSim);
    ui->qqTexture->setEnabled(ssel&&noEditModeAndNoSim);
    ui->qqDirtTexture->setEnabled((sc>0)&&noEditModeAndNoSim);
    ui->qqClearTextures->setEnabled((sc>0)&&noEditModeAndNoSim);

    ui->qqApplyMain->setEnabled(sel&&(ssc>1)&&noEditModeAndNoSim);

    ui->qqEditDynamics->setEnabled(sel&&noEditModeAndNoSim);
    ui->qqEditDynamics->setChecked(CQDlgShapeDyn::showDynamicWindow);


    if (ssel)
    {
        ui->qqShadingAngle->setText(tt::getAngleFString(false,((CGeometric*)it->geomData->geomInfo)->getGouraudShadingAngle(),1).c_str());
        ui->qqEdgesAngle->setText(tt::getAngleFString(false,((CGeometric*)it->geomData->geomInfo)->getEdgeThresholdAngle(),1).c_str());
        ui->qqBackfaceCulling->setChecked(((CGeometric*)it->geomData->geomInfo)->getCulling());
        ui->qqWireframe->setChecked(((CGeometric*)it->geomData->geomInfo)->getWireframe());
        ui->qqShowEdges->setChecked(((CGeometric*)it->geomData->geomInfo)->getVisibleEdges());
        ui->qqHiddenBorder->setChecked(((CGeometric*)it->geomData->geomInfo)->getHideEdgeBorders());
    }
    else
    {
        ui->qqShadingAngle->setText("");
        ui->qqEdgesAngle->setText("");
        ui->qqBackfaceCulling->setChecked(false);
        ui->qqWireframe->setChecked(false);
        ui->qqShowEdges->setChecked(false);
        ui->qqHiddenBorder->setChecked(false);
    }
    selectLineEdit(lineEditToSelect);
}

void CQDlgShapes::on_qqBackfaceCulling_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_BACKFACECULLING_SHAPEGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgShapes::on_qqWireframe_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_WIREFRAME_SHAPEGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgShapes::on_qqInvertFaces_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        App::appendSimulationThreadCommand(INVERT_FACES_SHAPEGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgShapes::on_qqShowEdges_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_SHOWEDGES_SHAPEGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgShapes::on_qqShadingAngle_editingFinished()
{
    if (!ui->qqShadingAngle->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqShadingAngle->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_SHADINGANGLE_SHAPEGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,gv::userToRad*newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgShapes::on_qqApplyMain_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=APPLY_OTHERPROP_SHAPEGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->objCont->getLastSelectionID());
        for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
            cmd.intParams.push_back(App::ct->objCont->getSelID(i));
        App::appendSimulationThreadCommand(cmd);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgShapes::on_qqEditDynamics_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CQDlgShapeDyn::showDynamicWindow=!CQDlgShapeDyn::showDynamicWindow;
        if (App::mainWindow->dlgCont->isVisible(SHAPE_DYN_DLG)!=CQDlgShapeDyn::showDynamicWindow)
            App::mainWindow->dlgCont->toggle(SHAPE_DYN_DLG);
    }
}

void CQDlgShapes::on_qqAdjustOutsideColor_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CQDlgMaterial::displayMaterialDlg(COLOR_ID_SHAPE,App::ct->objCont->getLastSelectionID(),-1,App::mainWindow);
    }
}

void CQDlgShapes::on_qqApplyColors_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=APPLY_VISUALPROP_SHAPEGUITRIGGEREDCMD;
        cmd.intParams.push_back(App::ct->objCont->getLastSelectionID());
        for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
            cmd.intParams.push_back(App::ct->objCont->getSelID(i));
        App::appendSimulationThreadCommand(cmd);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    }
}

void CQDlgShapes::on_qqTexture_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CQDlgTextures::displayDlg(TEXTURE_ID_SIMPLE_SHAPE,App::ct->objCont->getLastSelectionID(),-1,App::mainWindow);
    }
}

void CQDlgShapes::on_qqGeometry_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        CQDlgGeometry::display(App::ct->objCont->getLastSelectionID(),App::mainWindow);
    }
}

void CQDlgShapes::on_qqDirtTexture_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        std::string tst(App::directories->textureDirectory);
        std::string filenameAndPath=App::uiThread->getOpenFileName(this,0,"Loading texture...",tst,"",true,"Image files","tga","jpg","jpeg","png","gif","bmp","tiff");
        if (filenameAndPath.length()!=0)
        {
            if (VFile::doesFileExist(filenameAndPath))
            {
                App::directories->textureDirectory=App::directories->getPathFromFull(filenameAndPath);
                int resX,resY,n;
                unsigned char* data=CImageLoaderSaver::load(filenameAndPath.c_str(),&resX,&resY,&n,0);
                bool rgba=(n==4);
                if (n<3)
                {
                    delete[] data;
                    data=NULL;
                }
                if (data==NULL)
                    App::uiThread->messageBox_critical(App::mainWindow,strTranslate("Texture"),strTranslate(IDS_TEXTURE_FILE_COULD_NOT_BE_LOADED),VMESSAGEBOX_OKELI);
                else
                {
                    SSimulationThreadCommand cmd;
                    cmd.cmdId=SET_QUICKTEXTURES_SHAPEGUITRIGGEREDCMD;
                    for (int i=0;i<App::ct->objCont->getSelSize();i++)
                        cmd.intParams.push_back(App::ct->objCont->getSelID(i));
                    cmd.uint8Params.assign(data,data+resX*resY*n);
                    delete[] data;
                    cmd.boolParams.push_back(rgba);
                    cmd.intParams.push_back(resX);
                    cmd.intParams.push_back(resY);
                    cmd.stringParams.push_back(App::directories->getNameFromFull(filenameAndPath));
                    App::appendSimulationThreadCommand(cmd);
                    App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
                }
            }
        }
    }
}

void CQDlgShapes::on_qqClearTextures_clicked()
{
    IF_UI_EVENT_CAN_WRITE_DATA
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=CLEAR_TEXTURES_SHAPEGUITRIGGEREDCMD;
        for (int i=0;i<App::ct->objCont->getSelSize();i++)
            cmd.intParams.push_back(App::ct->objCont->getSelID(i));
        App::appendSimulationThreadCommand(cmd);
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
    }
}

void CQDlgShapes::on_qqEdgesAngle_editingFinished()
{
    if (!ui->qqEdgesAngle->isModified())
        return;
    IF_UI_EVENT_CAN_READ_DATA
    {
        bool ok;
        float newVal=ui->qqEdgesAngle->text().toFloat(&ok);
        if (ok)
        {
            App::appendSimulationThreadCommand(SET_EDGEANGLE_SHAPEGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID(),-1,gv::userToRad*newVal);
            App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        }
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}

void CQDlgShapes::on_qqEditMultishape_clicked()
{
    SSimulationThreadCommand cmd;
    cmd.cmdId=SHAPE_EDIT_MODE_START_EMCMD;
    App::appendSimulationThreadCommand(cmd);
}

void CQDlgShapes::on_qqHiddenBorder_clicked()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        App::appendSimulationThreadCommand(TOGGLE_HIDEEDGEBORDERS_SHAPEGUITRIGGEREDCMD,App::ct->objCont->getLastSelectionID());
        App::appendSimulationThreadCommand(POST_SCENE_CHANGED_ANNOUNCEMENT_GUITRIGGEREDCMD);
        App::appendSimulationThreadCommand(FULLREFRESH_ALL_DIALOGS_GUITRIGGEREDCMD);
    }
}
