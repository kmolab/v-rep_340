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

#include "motionPlanningTaskRendering_old.h"

#ifdef SIM_WITH_OPENGL
#include "pathPlanningInterface.h"

void displayMotionPlanningTask(CMotionPlanningTask* task)
{
    C3DObject* basef=task->getBaseObject();
    if ((task->_data!=NULL)&&(basef!=NULL))
    {
        float* p;
        int ind=0;
        C7Vector tr(basef->getCumulativeTransformationPart1());
        glPushMatrix();
        glTranslatef(tr.X(0),tr.X(1),tr.X(2));
        C4Vector axis=tr.Q.getAngleAndAxis();
        glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
        glLoadName(-1);

        ogl::buffer.clear();

        ogl::setMaterialColor(ogl::colorBlack,ogl::colorBlack,ogl::colorBlack);
        if (task->_showPhase1Nodes)
        {
            ogl::setMaterialColor(sim_colorcomponent_emission,ogl::colorGreen);
            int r;
            while (true)
            {
                r=CPathPlanningInterface::getMpPhase1NodesRenderData(task->_data,ind++,&p);
                if (r==-1)
                    break;
                if (r>0)
                {
                    ogl::buffer.push_back(p[0]);
                    ogl::buffer.push_back(p[1]);
                    ogl::buffer.push_back(p[2]);
                }
            }
            if (ogl::buffer.size()!=0)
                ogl::drawRandom3dPoints(&ogl::buffer[0],int(ogl::buffer.size())/3,NULL);
            ogl::buffer.clear();
/* KEEP!!
            ind=0;
            float* p2;
            ogl::buffer.clear();
            while (false)
            { // edges visualization
                int r=task->_data->getPhase1NodesRenderData(ind,&p);
                if (r==-1)
                    break;
                if (r==1)
                {
                    for (int i=0;i<6;i++)
                    {
                        if (task->_data->getPhase1ConnectionData(ind,i,&p2)==1)
                        {
                            ogl::addBuffer3DPoints(p);
                            ogl::addBuffer3DPoints(p2);
                        }
                    }
                }
                ind++;
            }
            if (ogl::buffer.size()!=0)
                ogl::drawRandom3dLines(&ogl::buffer[0],ogl::buffer.size()/3,false,NULL);
            ogl::buffer.clear();
*/
        }

        if (task->_showPhase2FromStartConnections)
        {
            ogl::setMaterialColor(sim_colorcomponent_emission,1.0f,0.5f,0.0f);
            _renderPhase2data(task,0);
        }

        if (task->_showPhase2FromGoalConnections)
        {
            ogl::setMaterialColor(sim_colorcomponent_emission,0.0f,0.5f,1.0f);
            _renderPhase2data(task,1);
        }

        if (task->_showPhase2Path)
        {
            ogl::setMaterialColor(sim_colorcomponent_emission,ogl::colorYellow);
            glLineWidth(3.0f);
            _renderPhase2data(task,2);
            glLineWidth(1.0f);
        }

        glPopMatrix();
    }
}

void _renderPhase2data(CMotionPlanningTask* task,unsigned int what)
{
    int ind=0;
    float* p1;
    float* p2;
    ogl::buffer.clear();
    while (CPathPlanningInterface::getMpPhase2NodesRenderData(task->_data,what,ind++,&p1,&p2)!=-1)
    {
        ogl::buffer.push_back(p1[0]);
        ogl::buffer.push_back(p1[1]);
        ogl::buffer.push_back(p1[2]);
        ogl::buffer.push_back(p2[0]);
        ogl::buffer.push_back(p2[1]);
        ogl::buffer.push_back(p2[2]);
    }
    if (ogl::buffer.size()!=0)
        ogl::drawRandom3dLines(&ogl::buffer[0],ogl::buffer.size()/3,false,NULL);
    ogl::buffer.clear();
}

#else

void displayMotionPlanningTask(CMotionPlanningTask* task)
{

}

#endif



