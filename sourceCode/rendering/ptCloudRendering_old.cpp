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

#include "ptCloudRendering_old.h"

#ifdef SIM_WITH_OPENGL
void displayPtCloud_old(CPtCloud_old* ptCloud,C3DObject* it)
{
    glPushMatrix();

    C3Vector normalVectorForLinesAndPoints=C3Vector::unitZVector;

    if (it!=NULL)
    {
        C7Vector tr=it->getCumulativeTransformationPart1_forDisplay(true);
        glTranslatef(tr.X(0),tr.X(1),tr.X(2));
        C4Vector axis=tr.Q.getAngleAndAxisNoChecking();
        glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
        normalVectorForLinesAndPoints=(tr.Q.getInverse()*C3Vector::unitZVector);
    }

    glPointSize(ptCloud->_pointSize);

    if (ptCloud->_normals.size()==0)
        ogl::setMaterialColor(ptCloud->_defaultColors,ogl::colorBlack,ptCloud->_defaultColors+12);
    else
        ogl::setMaterialColor(ptCloud->_defaultColors,ptCloud->_defaultColors+8,ptCloud->_defaultColors+12);

    if (ptCloud->_colors.size()==0)
    {
        if (ptCloud->_normals.size()==0)
            ogl::drawRandom3dPointsEx(&ptCloud->_vertices[0],ptCloud->_vertices.size()/3,NULL,NULL,NULL,false,normalVectorForLinesAndPoints.data);
        else
            ogl::drawRandom3dPointsEx(&ptCloud->_vertices[0],ptCloud->_vertices.size()/3,&ptCloud->_normals[0],NULL,NULL,false,normalVectorForLinesAndPoints.data);
    }
    else
    {
        if (ptCloud->_normals.size()==0)
            ogl::drawRandom3dPointsEx(&ptCloud->_vertices[0],ptCloud->_vertices.size()/3,NULL,&ptCloud->_colors[0],NULL,(ptCloud->_options&4)!=0,normalVectorForLinesAndPoints.data);
        else
            ogl::drawRandom3dPointsEx(&ptCloud->_vertices[0],ptCloud->_vertices.size()/3,&ptCloud->_normals[0],&ptCloud->_colors[0],NULL,(ptCloud->_options&4)!=0,normalVectorForLinesAndPoints.data);
    }
    glPointSize(1.0f);

    glPopMatrix();
}

#else

void displayPtCloud_old(CPtCloud_old* ptCloud,C3DObject* it)
{

}

#endif



