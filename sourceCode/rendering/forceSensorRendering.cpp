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

#include "forceSensorRendering.h"

#ifdef SIM_WITH_OPENGL

void displayForceSensor(CForceSensor* forceSensor,CViewableBase* renderingObject,int displayAttrib)
{
    // At the beginning of every 3DObject display routine:
    _commonStart(forceSensor,renderingObject,displayAttrib);

    // Bounding box display:
    if (displayAttrib&sim_displayattribute_renderpass)
        _displayBoundingBox(forceSensor,displayAttrib,true,forceSensor->getSize());

    // Object display:
    if (forceSensor->getShouldObjectBeDisplayed(renderingObject->getID(),displayAttrib))
    {
        _enableAuxClippingPlanes(forceSensor->getID());
        if (displayAttrib&sim_displayattribute_dynamiccontentonly)
            ogl::setMaterialColor(0.0f,0.6f,0.0f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f);
        else
            forceSensor->getColor(false)->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
        _displayForceSensor(forceSensor,displayAttrib,true,forceSensor->getSize());
        if (displayAttrib&sim_displayattribute_dynamiccontentonly)
            ogl::setMaterialColor(0.2f,0.2f,0.2f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f);
        else
            forceSensor->getColor(true)->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
        _displayForceSensor(forceSensor,displayAttrib,false,forceSensor->getSize());
        _disableAuxClippingPlanes();
    }

    // At the end of every 3DObject display routine:
    _commonFinish(forceSensor,renderingObject);
}

void _displayForceSensor(CForceSensor* forceSensor,int displayAttrib,bool partOne,float sizeParam)
{
    if ((App::getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
    {
        if (forceSensor->getLocalObjectProperty()&sim_objectproperty_selectmodelbaseinstead)
            glLoadName(forceSensor->getModelSelectionID());
        else
            glLoadName(forceSensor->getID());
    }
    else
        glLoadName(-1);

    glPushAttrib(GL_POLYGON_BIT);
    if ((displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass))
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    glPushMatrix();
    if (partOne)
        ogl::drawCylinder(forceSensor->getSize(),forceSensor->getSize()*0.5f,16,0,true);
    else
    {
        if (forceSensor->getDynamicSecondPartIsValid()&&(!App::ct->simulation->isSimulationStopped()))
        { // for dynamic mode
            C7Vector tr(forceSensor->getDynamicSecondPartLocalTransform());
            glTranslatef(tr.X(0),tr.X(1),tr.X(2));
            C4Vector axis=tr.Q.getAngleAndAxisNoChecking();
            glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
        }

        ogl::drawCylinder(forceSensor->getSize()/5.0f,forceSensor->getSize(),16,0,true);
        if (displayAttrib&sim_displayattribute_selected)
            _drawReference(forceSensor,sizeParam);
    }
    glPopAttrib();
    glPopMatrix();
}

#else

void displayForceSensor(CForceSensor* forceSensor,CViewableBase* renderingObject,int displayAttrib)
{

}

#endif



