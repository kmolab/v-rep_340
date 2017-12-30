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

#include "mirrorRendering.h"

#ifdef SIM_WITH_OPENGL

void displayMirror(CMirror* mirror,CViewableBase* renderingObject,int displayAttrib)
{
    // At the beginning of every 3DObject display routine:
    _commonStart(mirror,renderingObject,displayAttrib);

    // Bounding box display:
    if (displayAttrib&sim_displayattribute_renderpass)
        _displayBoundingBox(mirror,displayAttrib,true,0.0);

    // Object display:
    if (mirror->getShouldObjectBeDisplayed(renderingObject->getID(),displayAttrib))
    {
        if ((App::getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
        {
            if (mirror->getLocalObjectProperty()&sim_objectproperty_selectmodelbaseinstead)
                glLoadName(mirror->getModelSelectionID());
            else
                glLoadName(mirror->getID());
        }
        else
            glLoadName(-1);

        if ( (displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass) )
            glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);

        _enableAuxClippingPlanes(mirror->getID());
        if (((displayAttrib&sim_displayattribute_pickpass)||((mirror->currentMirrorContentBeingRendered!=mirror->getID())&&(mirror->currentMirrorContentBeingRendered!=-1)))&&mirror->getIsMirror())
        {
            ogl::disableLighting_useWithCare(); // only temporarily
            ogl::setMaterialColor(mirror->mirrorColor,ogl::colorBlack,ogl::colorBlack);
            ogl::drawPlane(mirror->getMirrorWidth(),mirror->getMirrorHeight());
            ogl::enableLighting_useWithCare();
        }
        _disableAuxClippingPlanes();
        // For this part we want the clipping planes disabled:
        if (!mirror->getIsMirror())
        {
            mirror->getClipPlaneColor()->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
            ogl::drawPlane(mirror->getMirrorWidth(),mirror->getMirrorHeight());
        }
    }

    // At the end of every 3DObject display routine:
    _commonFinish(mirror,renderingObject);
}

#else

void displayMirror(CMirror* mirror,CViewableBase* renderingObject,int displayAttrib)
{

}

#endif



