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

#include "visionSensorRendering.h"

#ifdef SIM_WITH_OPENGL
#include "pluginContainer.h"

void displayVisionSensor(CVisionSensor* visionSensor,CViewableBase* renderingObject,int displayAttrib)
{
    // At the beginning of every 3DObject display routine:
    _commonStart(visionSensor,renderingObject,displayAttrib);

    // Bounding box display:
    if (displayAttrib&sim_displayattribute_renderpass)
        _displayBoundingBox(visionSensor,displayAttrib,true,0.0);

    C3Vector normalVectorForLinesAndPoints(visionSensor->getCumulativeTransformation().Q.getInverse()*C3Vector::unitZVector);

    // Object display:
    if (visionSensor->getShouldObjectBeDisplayed(renderingObject->getID(),displayAttrib))
    {
        if ((App::getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
        {
            if (visionSensor->getLocalObjectProperty()&sim_objectproperty_selectmodelbaseinstead)
                glLoadName(visionSensor->getModelSelectionID());
            else
                glLoadName(visionSensor->getID());
        }
        else
            glLoadName(-1);

        if ( (displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass) )
            glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);

        glPushMatrix();

        if ((visionSensor->sensorResult.sensorWasTriggered&&visionSensor->getShowVolumeWhenDetecting())||((!visionSensor->sensorResult.sensorWasTriggered)&&visionSensor->getShowVolumeWhenNotDetecting()))
        {
            if (visionSensor->sensorResult.sensorWasTriggered)
                visionSensor->getColor(true)->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
            else
                visionSensor->getColor(false)->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
            C3Vector c,f;
            visionSensor->getSensingVolumeCorners(c,f);
            ogl::buffer.clear();
            ogl::addBuffer3DPoints(-f(0),-f(1),f(2));
            ogl::addBuffer3DPoints(-f(0),+f(1),f(2));
            ogl::addBuffer3DPoints(+f(0),+f(1),f(2));
            ogl::addBuffer3DPoints(+f(0),-f(1),f(2));
            ogl::addBuffer3DPoints(-f(0),-f(1),f(2));
            ogl::drawRandom3dLines(&ogl::buffer[0],ogl::buffer.size()/3,true,normalVectorForLinesAndPoints.data);
            ogl::buffer.clear();
            ogl::addBuffer3DPoints(-c(0),-c(1),c(2));
            ogl::addBuffer3DPoints(-c(0),+c(1),c(2));
            ogl::addBuffer3DPoints(+c(0),+c(1),c(2));
            ogl::addBuffer3DPoints(+c(0),-c(1),c(2));
            ogl::addBuffer3DPoints(-c(0),-c(1),c(2));
            ogl::drawRandom3dLines(&ogl::buffer[0],ogl::buffer.size()/3,true,normalVectorForLinesAndPoints.data);
            ogl::buffer.clear();
            ogl::addBuffer3DPoints(-c(0),-c(1),c(2));
            ogl::addBuffer3DPoints(-f(0),-f(1),f(2));
            ogl::addBuffer3DPoints(-c(0),+c(1),c(2));
            ogl::addBuffer3DPoints(-f(0),+f(1),f(2));
            ogl::addBuffer3DPoints(+c(0),-c(1),c(2));
            ogl::addBuffer3DPoints(+f(0),-f(1),f(2));
            ogl::addBuffer3DPoints(+c(0),+c(1),c(2));
            ogl::addBuffer3DPoints(+f(0),+f(1),f(2));
            ogl::drawRandom3dLines(&ogl::buffer[0],ogl::buffer.size()/3,false,normalVectorForLinesAndPoints.data);
            ogl::buffer.clear();
        }

        if (visionSensor->sensorResult.sensorWasTriggered)
            visionSensor->getColor(true)->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
        else
            visionSensor->getColor(false)->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
        C3Vector _size(visionSensor->getSize());
        float dx=_size(0)*0.5f;
        float dy=_size(1)*0.5f;
        glBegin(GL_QUADS);
        glNormal3f(-1.0f,0.0f,0.0f);
        glVertex3f(-dx,-dy,0.0f);
        glVertex3f(-dx,dy,0.0f);
        glVertex3f(-dx,dy,-_size(2));
        glVertex3f(-dx,-dy,-_size(2));
        glNormal3f(0.0f,0.0f,-1.0f);
        glVertex3f(-dx,-dy,-_size(2));
        glVertex3f(-dx,dy,-_size(2));
        glVertex3f(dx,dy,-_size(2));
        glVertex3f(dx,-dy,-_size(2));
        glNormal3f(1.0f,0.0f,0.0f);
        glVertex3f(dx,-dy,-_size(2));
        glVertex3f(dx,dy,-_size(2));
        glVertex3f(dx,dy,0.0f);
        glVertex3f(dx,-dy,0.0f);
        glNormal3f(0.0f,-1.0f,0.0f);
        glVertex3f(-dx,-dy,0.0f);
        glVertex3f(-dx,-dy,-_size(2));
        glVertex3f(dx,-dy,-_size(2));
        glVertex3f(dx,-dy,0.0f);
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(-dx,dy,0.0f);
        glVertex3f(dx,dy,0.0f);
        glVertex3f(dx,dy,-_size(2));
        glVertex3f(-dx,dy,-_size(2));
        ogl::setMaterialColor(ogl::colorBlack,ogl::colorBlack,ogl::colorBlack);
        glNormal3f(0.0f,0.0f,1.0f);
        glVertex3f(dx,-dy,0.0f);
        glVertex3f(dx,dy,0.0f);
        glVertex3f(-dx,dy,0.0f);
        glVertex3f(-dx,-dy,0.0f);
        glEnd();
        glPopMatrix();

        _disableAuxClippingPlanes();
    }

    // At the end of every 3DObject display routine:
    _commonFinish(visionSensor,renderingObject);
}

#else

void displayVisionSensor(CVisionSensor* visionSensor,CViewableBase* renderingObject,int displayAttrib)
{

}

#endif



