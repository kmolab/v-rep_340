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
#include "v_rep_internal.h"
#include "globalGuiTextureContainer.h"
#include "global.h"
#include "imgLoaderSaver.h"
#include "oGL.h"
#include "rendering.h"

CGlobalGuiTextureContainer::CGlobalGuiTextureContainer()
{
    int xres,yres;
    bool rgba;
    unsigned char* dat;

    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/world.tga",xres,yres,rgba,NULL);
    addObject(WORLD_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/s_shape.tga",xres,yres,rgba,NULL);
    addObject(SIMPLE_SHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/p_shape.tga",xres,yres,rgba,NULL);
    addObject(PURE_SHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/h_shape.tga",xres,yres,rgba,NULL);
    addObject(HEIGHTFIELD_SHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/mp_shape.tga",xres,yres,rgba,NULL);
    addObject(PURE_MULTISHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/m_shape.tga",xres,yres,rgba,NULL);
    addObject(MULTI_SHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/r_joint.tga",xres,yres,rgba,NULL);
    addObject(REVOLUTE_JOINT_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/p_joint.tga",xres,yres,rgba,NULL);
    addObject(PRISMATIC_JOINT_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/spheric.tga",xres,yres,rgba,NULL);
    addObject(SPHERICAL_JOINT_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/camera.tga",xres,yres,rgba,NULL);
    addObject(CAMERA_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/o_lighti.tga",xres,yres,rgba,NULL);
    addObject(OMNI_LIGHT_OFF_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/o_lighta.tga",xres,yres,rgba,NULL);
    addObject(OMNI_LIGHT_ON_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/s_lighti.tga",xres,yres,rgba,NULL);
    addObject(SPOT_LIGHT_OFF_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/s_lighta.tga",xres,yres,rgba,NULL);
    addObject(SPOT_LIGHT_ON_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/d_lighti.tga",xres,yres,rgba,NULL);
    addObject(DIR_LIGHT_OFF_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/d_lighta.tga",xres,yres,rgba,NULL);
    addObject(DIR_LIGHT_ON_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/graph.tga",xres,yres,rgba,NULL);
    addObject(GRAPH_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/dummy.tga",xres,yres,rgba,NULL);
    addObject(DUMMY_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/octree.tga",xres,yres,rgba,NULL);
    addObject(OCTREE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/pointcloud.tga",xres,yres,rgba,NULL);
    addObject(POINTCLOUD_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/model.tga",xres,yres,rgba,NULL);
    addObject(MODEL_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/sensor.tga",xres,yres,rgba,NULL);
    addObject(PROXIMITYSENSOR_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/rendsens.tga",xres,yres,rgba,NULL);
    addObject(VISIONSENSOR_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/path.tga",xres,yres,rgba,NULL);
    addObject(PATH_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;

    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/mirror.tga",xres,yres,rgba,NULL);
    addObject(MIRROR_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/clipplane.tga",xres,yres,rgba,NULL);
    addObject(CLIPPLANE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;

    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/convexs.tga",xres,yres,rgba,NULL);
    addObject(CONVEX_SHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/convexm.tga",xres,yres,rgba,NULL);
    addObject(CONVEX_MULTISHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;

    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/minus.tga",xres,yres,rgba,NULL);
    addObject(MINUS_SIGN_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/plus.tga",xres,yres,rgba,NULL);
    addObject(PLUS_SIGN_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/mscript.tga",xres,yres,rgba,NULL);
    addObject(MAIN_SCRIPT_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/mcscript.tga",xres,yres,rgba,NULL);
    addObject(MAIN_SCRIPT_CUSTOMIZED_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/script.tga",xres,yres,rgba,NULL);
    addObject(SCRIPT_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/scriptDisabled.tga",xres,yres,rgba,NULL);
    addObject(SCRIPTDISABLED_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/scriptthreaded.tga",xres,yres,rgba,NULL);
    addObject(SCRIPT_THREADED_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/scriptthreadedDisabled.tga",xres,yres,rgba,NULL);
    addObject(SCRIPTDISABLED_THREADED_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/scriptparameters.tga",xres,yres,rgba,NULL);
    addObject(SCRIPT_PARAMETERS_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/scriptparametersactive.tga",xres,yres,rgba,NULL);
    addObject(SCRIPT_PARAMETERS_ACTIVE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/pathpoint.tga",xres,yres,rgba,NULL);
    addObject(PATH_POINT_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/vertex.tga",xres,yres,rgba,NULL);
    addObject(VERTEX_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/triangle.tga",xres,yres,rgba,NULL);
    addObject(TRIANGLE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/edge.tga",xres,yres,rgba,NULL);
    addObject(EDGE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/2delement.tga",xres,yres,rgba,NULL);
    addObject(BUTTON_BLOCK_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/mill.tga",xres,yres,rgba,NULL);
    addObject(MILL_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/ftsensor.tga",xres,yres,rgba,NULL);
    addObject(FORCE_SENSOR_TREE_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/warning.tga",xres,yres,rgba,NULL);
    addObject(WARNING_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/dynamics.tga",xres,yres,rgba,NULL);
    addObject(DYNAMICS_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/folder.tga",xres,yres,rgba,NULL);
    addObject(FOLDER_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/jointCtrlScript.tga",xres,yres,rgba,NULL);
    addObject(JOINTCTRLSCRIPT_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/customizationScript.tga",xres,yres,rgba,NULL);
    addObject(CUSTOMIZATIONSCRIPT_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/customizationScriptDisabled.tga",xres,yres,rgba,NULL);
    addObject(CUSTOMIZATIONSCRIPTDISABLED_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/collRespScript.tga",xres,yres,rgba,NULL);
    addObject(COLLISIONRESPONSESCRIPT_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;

    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/generalCallbackScript.tga",xres,yres,rgba,NULL);
    addObject(GENERALCALLBACKSCRIPT_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;


    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/checkedbgd.tga",xres,yres,rgba,NULL);
    addObject(CHECKED_BACKGROUND_PICTURE,xres,yres,rgba,false,true,dat);
    delete[] dat;
    

    // Cursors:
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_arrow.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_ARROW,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_finger.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_FINGER,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_alldir.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_ALL_DIR,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_horizdir.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_HORIZONTAL_DIR,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_vertdir.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_VERTICAL_DIR,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_slash.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_SLASH_DIR,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_backslash.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_BACKSLASH_DIR,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_nobutt.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_NO_BUTTON,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_leftbutt.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_LEFT_BUTTON,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_middlebutt.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_MIDDLE_BUTTON,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_wheel.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_WHEEL_BUTTON,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_rightbutt.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_RIGHT_BUTTON,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_shift.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_SHIFT_BUTTON,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/cur_ctrl.tga",xres,yres,rgba,NULL);
    addObject(CURSOR_CTRL_BUTTON,xres,yres,rgba,false,true,dat);
    delete[] dat;

    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/vrepWaterMark.tga",xres,yres,rgba,NULL);
    addObject(VREP_TAG,xres,yres,rgba,false,true,dat);
    delete[] dat;
    dat=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/eduWaterMark.tga",xres,yres,rgba,NULL);
    addObject(EDU_TAG,xres,yres,rgba,false,true,dat);
    delete[] dat;
}

CGlobalGuiTextureContainer::~CGlobalGuiTextureContainer()
{
    removeAllObjects();
}

void CGlobalGuiTextureContainer::addObject(int pictureIndex,int sizeX,int sizeY,bool rgba,bool horizFlip,bool vertFlip,unsigned char* data)
{
    CTextureObject* it=new CTextureObject(sizeX,sizeY);
    it->setImage(rgba,horizFlip,vertFlip,data);
    _allTextureObjects.push_back(it);
    _allPictureIndices.push_back(pictureIndex);
}

void CGlobalGuiTextureContainer::removeAllObjects()
{
    for (int i=0;i<int(_allTextureObjects.size());i++)
        delete _allTextureObjects[i];
    _allTextureObjects.clear();
    _allPictureIndices.clear();
}

void CGlobalGuiTextureContainer::startTextureDisplay(int pictureIndex)
{
    for (size_t i=0;i<_allPictureIndices.size();i++)
    {
        if (_allPictureIndices[i]==pictureIndex)
        {
            _startTextureDisplay(_allTextureObjects[i],true,0,false,false);
            break;
        }
    }
}

void CGlobalGuiTextureContainer::endTextureDisplay()
{
    _endTextureDisplay();
}
