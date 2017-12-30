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

#include "pathRendering.h"

#ifdef SIM_WITH_OPENGL

void displayPath(CPath* path,CViewableBase* renderingObject,int displayAttrib)
{
    // At the beginning of every 3DObject display routine:
    _commonStart(path,renderingObject,displayAttrib);

    // Bounding box display:
    if (displayAttrib&sim_displayattribute_renderpass)
        _displayBoundingBox(path,displayAttrib,true,path->pathContainer->getSquareSize()*2.0f);

    C3Vector normalVectorForLinesAndPoints(path->getCumulativeTransformation().Q.getInverse()*C3Vector::unitZVector);

    // Object display:
#ifdef SIM_WITH_GUI
    if ( path->getShouldObjectBeDisplayed(renderingObject->getID(),displayAttrib)||( (App::mainWindow!=NULL)&&(App::mainWindow->editModeContainer->getEditModePath()==path) ) )
#else
    if (path->getShouldObjectBeDisplayed(renderingObject->getID(),displayAttrib))
#endif
    {
        if ((App::getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
        {
            if (path->getLocalObjectProperty()&sim_objectproperty_selectmodelbaseinstead)
                glLoadName(path->getModelSelectionID());
            else
                glLoadName(path->getID());
        }
        else
            glLoadName(-1);

        if ( (displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass) )
            glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);

#ifdef SIM_WITH_GUI
        if ( (App::mainWindow!=NULL)&&(App::mainWindow->editModeContainer->getEditModePath()==path) )
            App::mainWindow->editModeContainer->getEditModePathContainer()->render(true,0,false,path->getID());
        else
#endif
        {
            _enableAuxClippingPlanes(path->getID());
            if ((displayAttrib&sim_displayattribute_forvisionsensor)==0)
            {
                bool isUniqueSelectedPath=false;
#ifdef SIM_WITH_GUI
                if (App::mainWindow!=NULL)
                    isUniqueSelectedPath=App::mainWindow->editModeContainer->pathPointManipulation->getUniqueSelectedPathId_nonEditMode()!=-1;
#endif
                path->pathContainer->render(false,displayAttrib,isUniqueSelectedPath,path->getID());
            }

            if (path->getShapingEnabled())
            {
                path->getShapingColor()->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
                glBegin(GL_TRIANGLES);
                for (int i=0;i<int(path->_pathShapeIndices.size());i++)
                {
                    glNormal3fv(&path->_pathShapeNormals[3*i]);
                    glVertex3fv(&path->_pathShapeVertices[3*(path->_pathShapeIndices[i])]);
                }
                glEnd();
            }
            _disableAuxClippingPlanes();
        }

        glDisable(GL_CULL_FACE);
    }

    // At the end of every 3DObject display routine:
    _commonFinish(path,renderingObject);
}

#else

void displayPath(CPath* path,CViewableBase* renderingObject,int displayAttrib)
{

}

#endif
