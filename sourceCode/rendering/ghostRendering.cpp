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

#include "ghostRendering.h"

#ifdef SIM_WITH_OPENGL

void displayGhost(CShape* shape,const C7Vector& tr,int displayAttributes,int options,float transparencyFactor,const float* color)
{
    glPushMatrix();
    glPushAttrib(GL_POLYGON_BIT);

    glTranslatef(tr.X(0),tr.X(1),tr.X(2));
    C4Vector axis=tr.Q.getAngleAndAxisNoChecking();
    glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));

    shape->geomData->geomInfo->displayGhost(shape->geomData,displayAttributes,(options&4)!=0,(options&32)!=0,transparencyFactor,color);

    glPopAttrib();
    glPopMatrix();
    ogl::setBlending(false);
    glDisable(GL_CULL_FACE);
}

#else

void displayGhost(CShape* shape,const C7Vector& tr,int displayAttributes,int options,float transparencyFactor,const float* color)
{

}

#endif



