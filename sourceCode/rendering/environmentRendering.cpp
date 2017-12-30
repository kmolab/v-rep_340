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

#include "environmentRendering.h"

#ifdef SIM_WITH_OPENGL

void displayBackground(const int* viewSize,bool fogEnabled,const float* fogBackgroundColor,const float* backGroundColorDown,const float* backGroundColor)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,viewSize[0],0,viewSize[1],-100,100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ogl::setMaterialColor(ogl::colorBlack,ogl::colorBlack,ogl::colorBlack);
    glDisable(GL_DEPTH_TEST);
    glColorMaterial(GL_FRONT_AND_BACK,GL_EMISSION);
    glEnable(GL_COLOR_MATERIAL);
    glBegin(GL_QUADS);
    if (fogEnabled)
        glColor3fv(fogBackgroundColor);
    else
        glColor3fv(backGroundColorDown);
    glVertex3i(0,0,0);
    glVertex3i(viewSize[0],0,0);
    if (fogEnabled)
        glColor3fv(fogBackgroundColor);
    else
        glColor3fv(backGroundColor);
    glVertex3i(viewSize[0],viewSize[1],0);
    glVertex3i(0,viewSize[1],0);
    glEnd();
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void enableAmbientLight(bool on,const float* ambientLightColor)
{
    if (on)
    {
        GLfloat ambient[]={ambientLightColor[0],ambientLightColor[1],ambientLightColor[2],1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
    }
    else
    {
        GLfloat ambient[]={0.0f,0.0f,0.0f,1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
    }
}

void activateFog(const float* fogBackgroundColor,int fogType,float dd,float farClipp,float fogStart,float fogEnd,float fogDensity)
{
    float fog_color[4]={fogBackgroundColor[0],fogBackgroundColor[1],fogBackgroundColor[2],1.0f};
    GLenum fogTypeEnum[3]={GL_LINEAR,GL_EXP,GL_EXP2};
    glFogfv(GL_FOG_COLOR,fog_color);
    glFogi(GL_FOG_MODE,fogTypeEnum[fogType]);
    glFogf(GL_FOG_START,fogStart*dd+(1.0f-dd)*farClipp);
    glFogf(GL_FOG_END,fogEnd*dd+(1.0f-dd)*farClipp);
    glFogf(GL_FOG_DENSITY,fogDensity*dd);
    glEnable(GL_FOG);
}

void enableFog(bool on)
{
    if (on)
        glEnable(GL_FOG);
    else
        glDisable(GL_FOG);
}

#else

void displayBackground(const int* viewSize,bool fogEnabled,const float* fogBackgroundColor,const float* backGroundColorDown,const float* backGroundColor)
{

}

void enableAmbientLight(bool on,const float* ambientLightColor)
{

}

void activateFog(const float* fogBackgroundColor,int fogType,float dd,float farClipp,float fogStart,float fogEnd,float fogDensity)
{

}

void enableFog(bool on)
{

}

#endif



