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

#include "millRendering.h"

#ifdef SIM_WITH_OPENGL

void displayMill(CMill* mill,CViewableBase* renderingObject,int displayAttrib)
{
    // At the beginning of every 3DObject display routine:
    _commonStart(mill,renderingObject,displayAttrib);

    // Display the bounding box:
    if (displayAttrib&sim_displayattribute_renderpass)
        _displayBoundingBox(mill,displayAttrib,true,0.0);

    C3Vector normalVectorForLinesAndPoints(mill->getCumulativeTransformation().Q.getInverse()*C3Vector::unitZVector);

    // Display the object:
    if (mill->getShouldObjectBeDisplayed(renderingObject->getID(),displayAttrib))
    {
        if ((App::getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
        {
            if (mill->getLocalObjectProperty()&sim_objectproperty_selectmodelbaseinstead)
                glLoadName(mill->getModelSelectionID());
            else
                glLoadName(mill->getID());
        }
        else
            glLoadName(-1);

        _enableAuxClippingPlanes(mill->getID());
        bool wire=false;
        if ( (displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass) )
        {
            wire=true;
            glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);
        }

        int _milledObjectCount;
        if (mill->getMilledCount(_milledObjectCount)&&(_milledObjectCount>0))
            mill->getColor(true)->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
        else
            mill->getColor(false)->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
        float _size=mill->getSize();
        ogl::drawBox(_size/2.0f,_size/2.0f,_size/2.0f,!wire,normalVectorForLinesAndPoints.data);

        if (mill->convexVolume->volumeEdges.size()!=0)
            ogl::drawRandom3dLines(&mill->convexVolume->volumeEdges[0],mill->convexVolume->volumeEdges.size()/3,false,normalVectorForLinesAndPoints.data);

        ogl::setMaterialColor(ogl::colorBlack,ogl::colorBlack,ogl::colorRed);

        if (mill->convexVolume->normalsInside.size()!=0)
            ogl::drawRandom3dLines(&mill->convexVolume->normalsInside[0],mill->convexVolume->normalsInside.size()/3,false,normalVectorForLinesAndPoints.data);

        _disableAuxClippingPlanes();
    }

    // At the end of every 3DObject display routine:
    _commonFinish(mill,renderingObject);
}

#else

void displayMill(CMill* mill,CViewableBase* renderingObject,int displayAttrib)
{

}

#endif



