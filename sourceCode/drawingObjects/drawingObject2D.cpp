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

#include "vrepMainHeader.h"
#include "drawingObject2D.h"
#include "tt.h"
#include "v_repConst.h"
#include "drawingObject2DRendering.h"

CDrawingObject2D::CDrawingObject2D(int theObjectType,float* vertices,float col[3],float size,float dir,char* txt)
{
    size=tt::getLimitedFloat(0.001f,10000.0f,size);
    _size=size;
    _dir=dir;
    _objectType=theObjectType;
    _col[0]=col[0];
    _col[1]=col[1];
    _col[2]=col[2];
    int t=_objectType&255;
    if ((t==sim_drawing2d_plus)||(t==sim_drawing2d_cross)||
        (t==sim_drawing2d_frame)||(t==sim_drawing2d_square)||
        (t==sim_drawing2d_arrow)||(t==sim_drawing2d_text)||
        (t==sim_drawing2d_circle)||(t==sim_drawing2d_disc))
    {
        for (int i=0;i<2;i++)
            _vertices[i]=vertices[i];
    }
    if ((t==sim_drawing2d_line)||(t==sim_drawing2d_rectangle))
    {
        for (int i=0;i<4;i++)
            _vertices[i]=vertices[i];
    }
    if (t==sim_drawing2d_triangle)
    {
        for (int i=0;i<6;i++)
            _vertices[i]=vertices[i];
    }
    if (t==sim_drawing2d_text)
    {
        if (txt!=NULL)
            _txt=txt;
    }
}

CDrawingObject2D::~CDrawingObject2D()
{
}

void CDrawingObject2D::draw(int minViewPos[2],int maxViewPos[2])
{
    displayDrawingObject2D(this,minViewPos,maxViewPos);
}
