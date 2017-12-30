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

#include "cameraRendering.h"

#ifdef SIM_WITH_OPENGL
#include "pluginContainer.h"

void displayCamera(CCamera* camera,CViewableBase* renderingObject,int displayAttrib)
{
    // At the beginning of every 3DObject display routine:
    _commonStart(camera,renderingObject,displayAttrib);

    // Bounding box display:
    if (displayAttrib&sim_displayattribute_renderpass)
        _displayBoundingBox(camera,displayAttrib,true,0.0);

    // Object display:
    if (camera->getShouldObjectBeDisplayed(renderingObject->getID(),displayAttrib))
    {
        if ((App::getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
        {
            if (camera->getLocalObjectProperty()&sim_objectproperty_selectmodelbaseinstead)
                glLoadName(camera->getModelSelectionID());
            else
                glLoadName(camera->getID());
        }
        else
            glLoadName(-1);

        if ( (displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass) )
            glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);

        _enableAuxClippingPlanes(camera->getID());
        camera->getColor(false)->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
        float cameraSize=camera->getCameraSize();
        glPushMatrix();
        glTranslatef(0.0f,0.0f,-cameraSize);
        ogl::drawBox(0.4f*cameraSize,cameraSize,2.0f*cameraSize,true,NULL);
        glPopMatrix();
        glPushMatrix();
        camera->getColor(true)->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
        glTranslatef(0.0f,1.3f*cameraSize,-0.25f*cameraSize);
        glRotatef(90.0f,0,1,0);
        ogl::drawCylinder(2.0f*cameraSize,cameraSize/2.0f,20,0,true);
        glTranslatef(1.5f*cameraSize,0.0f,0.0f);
        ogl::drawCylinder(2.0f*cameraSize,cameraSize/2.0f,20,0,true);
        glPopMatrix();
        glTranslatef(0.0f,0.0f,cameraSize/6.0f);
        ogl::drawCone(cameraSize,5.0f*cameraSize/3.0f,20,true,true);
        _disableAuxClippingPlanes();
    }

    // At the end of every 3DObject display routine:
    _commonFinish(camera,renderingObject);
}

#else

void displayCamera(CCamera* camera,CViewableBase* renderingObject,int displayAttrib)
{

}

#endif
