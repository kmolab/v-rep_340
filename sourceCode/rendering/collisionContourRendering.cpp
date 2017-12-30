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

#include "collisionContourRendering.h"

#ifdef SIM_WITH_OPENGL

void displayContour(CRegCollision* coll,int countourWidth)
{
    std::vector<float>& intersections=coll->getIntersectionsPtr()[0];

    glDisable(GL_DEPTH_TEST);
    coll->contourColor.makeCurrentColor(false);
    for (size_t i=0;i<intersections.size()/6;i++)
    {
        if ( (intersections[6*i+0]==intersections[6*i+3])&&
            (intersections[6*i+1]==intersections[6*i+4])&&
            (intersections[6*i+2]==intersections[6*i+5]) )
        {
            glPointSize(3.0f);
            ogl::buffer.clear();
            ogl::addBuffer3DPoints(&intersections[6*i]);
            ogl::drawRandom3dPoints(&ogl::buffer[0],1,NULL);
            ogl::buffer.clear();
            glPointSize(1.0f);
        }
        else
        {
            glLineWidth(float(countourWidth));
            ogl::buffer.clear();
            ogl::addBuffer3DPoints(&intersections[6*i+0]);
            ogl::addBuffer3DPoints(&intersections[6*i+3]);
            ogl::drawRandom3dLines(&ogl::buffer[0],2,false,NULL);
            ogl::buffer.clear();
            glLineWidth(1.0f);
        }
    }
    glEnable(GL_DEPTH_TEST);
}

#else

void displayContour(CRegCollision* coll,int countourWidth)
{

}

#endif



