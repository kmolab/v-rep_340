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

#include "viewRendering.h"

#ifdef SIM_WITH_OPENGL

void displayView(CSView* view,C3DObject* it,int mainWindowXPos,bool clipWithMainWindowXPos,bool drawText,bool passiveSubView)
{
    int _viewPosition[2];
    int _viewSize[2];
    view->getViewPosition(_viewPosition);
    view->getViewSize(_viewSize);
    if (it!=NULL)
    {
        glEnable(GL_SCISSOR_TEST);
        glViewport(_viewPosition[0],_viewPosition[1],_viewSize[0],_viewSize[1]);
        int xMax=_viewPosition[0];
        int xSize=_viewSize[0];
        if ( (xMax<mainWindowXPos)&&clipWithMainWindowXPos )
        {
            xSize=xSize-(mainWindowXPos-xMax);
            xMax=mainWindowXPos;
        }
        glScissor(xMax,_viewPosition[1],xSize,_viewSize[1]);
        if (xSize>0) // to avoid strange effects!
        {
            if (it->getObjectType()==sim_object_camera_type)
                ((CCamera*)it)->lookIn(NULL,view,drawText,passiveSubView);
            if (it->getObjectType()==sim_object_graph_type)
                ((CGraph*)it)->lookAt(NULL,view,view->getTimeGraph(),drawText,passiveSubView,true);
            if (it->getObjectType()==sim_object_visionsensor_type)
                ((CVisionSensor*)it)->lookAt(view);
        }
        glDisable(GL_SCISSOR_TEST);
    }
    else
    { // We draw a white sub-view:
        glEnable(GL_SCISSOR_TEST);
        glViewport(_viewPosition[0],_viewPosition[1],_viewSize[0],_viewSize[1]);
        int xMax=_viewPosition[0];
        if ( (xMax<mainWindowXPos)&&clipWithMainWindowXPos )
            xMax=mainWindowXPos;
        glScissor(xMax,_viewPosition[1],_viewSize[0],_viewSize[1]);
        glClearColor(0.75f,0.75f,0.75f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);
    }
}


#else

void displayView(CSView* view,C3DObject* it,int mainWindowXPos,bool clipWithMainWindowXPos,bool drawText,bool passiveSubView)
{

}

#endif



