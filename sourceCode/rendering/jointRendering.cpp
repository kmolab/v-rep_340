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

#include "jointRendering.h"

#ifdef SIM_WITH_OPENGL

void displayJoint(CJoint* joint,CViewableBase* renderingObject,int displayAttrib)
{
    // At the beginning of every 3DObject display routine:
    _commonStart(joint,renderingObject,displayAttrib);

    bool guiIsRendering=((displayAttrib&sim_displayattribute_forvisionsensor)==0);

    // Bounding box display:
    float sizeParam=joint->getLength()/4.0f;
    if (sizeParam<joint->getDiameter())
        sizeParam=joint->getDiameter();
    if (sizeParam>10.0f*joint->getDiameter())
        sizeParam=10.0f*joint->getDiameter();
    if (displayAttrib&sim_displayattribute_renderpass)
        _displayBoundingBox(joint,displayAttrib,true,sizeParam);

    // Object display:
    if (joint->getShouldObjectBeDisplayed(renderingObject->getID(),displayAttrib))
    {
        _enableAuxClippingPlanes(joint->getID());
        if (displayAttrib&sim_displayattribute_dynamiccontentonly)
        {
            if (joint->getDynamicObjectFlag_forVisualization_forDisplay(guiIsRendering)==4)
                ogl::setMaterialColor(0.0f,0.3f,1.0f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f);
            if (joint->getDynamicObjectFlag_forVisualization_forDisplay(guiIsRendering)==8)
                ogl::setMaterialColor(1.0f,0.0f,0.0f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f);
            if (joint->getDynamicObjectFlag_forVisualization_forDisplay(guiIsRendering)==16)
                ogl::setMaterialColor(0.9f,0.9f,0.0f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f);
        }
        else
            joint->getColor(false)->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);

        _displayJoint(joint,displayAttrib,true,sizeParam);

        if (displayAttrib&sim_displayattribute_dynamiccontentonly)
            ogl::setMaterialColor(0.1f,0.1f,0.1f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f);
        else
            joint->getColor(true)->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
        _displayJoint(joint,displayAttrib,false,sizeParam);
        _disableAuxClippingPlanes();
    }

    // At the end of every 3DObject display routine:
    _commonFinish(joint,renderingObject);
}

void _displayJoint(CJoint* joint,int displayAttrib,bool partOne,float sizeParam)
{
    if ((App::getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
    {
        if (joint->getLocalObjectProperty()&sim_objectproperty_selectmodelbaseinstead)
            glLoadName(joint->getModelSelectionID());
        else
            glLoadName(joint->getID());
    }
    else
        glLoadName(-1);

    glPushAttrib(GL_POLYGON_BIT);
    if ((displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass))
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    glPushMatrix();
    if (joint->getJointType()==sim_joint_spherical_subtype)
    {
        if (partOne)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            ogl::drawSphere(joint->getDiameter(),16,8,true);
            glDisable(GL_CULL_FACE);
        }
        else
        {
            if (joint->getDynamicSecondPartIsValid()&&(!App::ct->simulation->isSimulationStopped()))
            { // for dynamic mode
                C7Vector tr(joint->getDynamicSecondPartLocalTransform());
                glTranslatef(tr.X(0),tr.X(1),tr.X(2));
                C4Vector axis=tr.Q.getAngleAndAxisNoChecking();
                glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
            }
            else
            {
                C4Vector tr(joint->getSphericalTransformation().getAngleAndAxis());
                glRotatef(tr(0)*radToDeg,tr(1),tr(2),tr(3));
            }

            ogl::drawSphere(joint->getDiameter()/1.5f,16,8,true);
            if (displayAttrib&sim_displayattribute_selected)
                _drawReference(joint,sizeParam);
        }
    }
    if (joint->getJointType()==sim_joint_revolute_subtype)
    {
        if (partOne)
            ogl::drawCylinder(joint->getDiameter(),joint->getLength(),8,0,true);
        else
        {
            if (joint->getDynamicSecondPartIsValid()&&(!App::ct->simulation->isSimulationStopped()))
            { // for dynamic mode
                C7Vector tr(joint->getDynamicSecondPartLocalTransform());
                glTranslatef(tr.X(0),tr.X(1),tr.X(2));
                C4Vector axis=tr.Q.getAngleAndAxisNoChecking();
                glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
            }
            else
            {
                glRotatef(joint->getPosition()*radToDeg,0.0f,0.0f,1.0f);
                glTranslatef(0.0f,0.0f,joint->getPosition()*joint->getScrewPitch());
            }

            ogl::drawCylinder(joint->getDiameter()/2.0f,joint->getLength()*1.2f,8,0,true);
            if (displayAttrib&sim_displayattribute_selected)
                _drawReference(joint,sizeParam);
        }
    }
    if (joint->getJointType()==sim_joint_prismatic_subtype)
    {
        if (partOne)
            ogl::drawBox(joint->getDiameter(),joint->getDiameter(),joint->getLength(),true,NULL);
        else
        {
            if (joint->getDynamicSecondPartIsValid()&&(!App::ct->simulation->isSimulationStopped()))
            { // for dynamic mode
                C7Vector tr(joint->getDynamicSecondPartLocalTransform());
                glTranslatef(tr.X(0),tr.X(1),tr.X(2));
                C4Vector axis=tr.Q.getAngleAndAxisNoChecking();
                glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
            }
            else
                glTranslatef(0.0f,0.0f,joint->getPosition());

            ogl::drawBox(joint->getDiameter()/2.0f,joint->getDiameter()/2.0f,joint->getLength()*1.2f,true,NULL);
            if (displayAttrib&sim_displayattribute_selected)
                _drawReference(joint,sizeParam);
        }
    }
    glPopAttrib();
    glPopMatrix();
}

#else

void displayJoint(CJoint* joint,CViewableBase* renderingObject,int displayAttrib)
{

}

#endif



