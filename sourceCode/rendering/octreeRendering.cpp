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

#include "octreeRendering.h"

#ifdef SIM_WITH_OPENGL
#include "pluginContainer.h"

const int _cubeIndices[]={
    0,1,2,0,2,3,
    4,5,6,4,6,7,
    8,9,10,8,10,11,
    12,13,14,12,14,15,
    16,17,18,16,18,19,
    20,21,22,20,22,23
};

const float _cubeNormals[]={
    -1.0f,0.0f,0.0f,
    -1.0f,0.0f,0.0f,
    -1.0f,0.0f,0.0f,
    -1.0f,0.0f,0.0f,
    -1.0f,0.0f,0.0f,
    -1.0f,0.0f,0.0f,
    0.0f,0.0f,-1.0f,
    0.0f,0.0f,-1.0f,
    0.0f,0.0f,-1.0f,
    0.0f,0.0f,-1.0f,
    0.0f,0.0f,-1.0f,
    0.0f,0.0f,-1.0f,
    1.0f,0.0f,0.0f,
    1.0f,0.0f,0.0f,
    1.0f,0.0f,0.0f,
    1.0f,0.0f,0.0f,
    1.0f,0.0f,0.0f,
    1.0f,0.0f,0.0f,
    0.0f,0.0f,1.0f,
    0.0f,0.0f,1.0f,
    0.0f,0.0f,1.0f,
    0.0f,0.0f,1.0f,
    0.0f,0.0f,1.0f,
    0.0f,0.0f,1.0f,
    0.0f,-1.0f,0.0f,
    0.0f,-1.0f,0.0f,
    0.0f,-1.0f,0.0f,
    0.0f,-1.0f,0.0f,
    0.0f,-1.0f,0.0f,
    0.0f,-1.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,1.0f,0.0f
};

void displayOctree(COctree* octree,CViewableBase* renderingObject,int displayAttrib)
{
    // At the beginning of every 3DObject display routine:
    _commonStart(octree,renderingObject,displayAttrib);

    // Bounding box display:
    C3Vector mmaDim,mmiDim;
    octree->getMaxMinDims(mmaDim,mmiDim);
    C3Vector d(mmaDim-mmiDim);
    if (displayAttrib&sim_displayattribute_renderpass)
        _displayBoundingBox(octree,displayAttrib,true,cbrt(d(0)*d(1)*d(2))*0.6f);

    C3Vector normalVectorForLinesAndPoints(octree->getCumulativeTransformation().Q.getInverse()*C3Vector::unitZVector);

    // Object display:
    if (octree->getShouldObjectBeDisplayed(renderingObject->getID(),displayAttrib))
    {
        if ((App::getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
        {
            if (octree->getLocalObjectProperty()&sim_objectproperty_selectmodelbaseinstead)
                glLoadName(octree->getModelSelectionID());
            else
                glLoadName(octree->getID());
        }
        else
            glLoadName(-1);

        if ( (displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass) )
            glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);
        if ( (displayAttrib&sim_displayattribute_forcewireframe)==0)
            glEnable(GL_CULL_FACE);

        _enableAuxClippingPlanes(octree->getID());

        if (octree->getOctreeInfo()!=NULL)
        {
            std::vector<float>& _voxelPositions=octree->getCubePositions()[0];
            float* _cubeVertices=octree->getCubeVertices();
            bool setOtherColor=(App::ct->collisions->getCollisionColor(octree->getID())!=0);
            for (size_t i=0;i<App::ct->collections->allCollections.size();i++)
            {
                if (App::ct->collections->allCollections[i]->isObjectInCollection(octree->getID()))
                    setOtherColor|=(App::ct->collisions->getCollisionColor(App::ct->collections->allCollections[i]->getCollectionID())!=0);
            }
            if (displayAttrib&sim_displayattribute_forvisionsensor)
                setOtherColor=false;
            if (!setOtherColor)
                octree->getColor()->makeCurrentColor(false);
            else
                App::ct->mainSettings->collisionColor.makeCurrentColor(false);

            if (octree->getShowOctree()&&((displayAttrib&sim_displayattribute_forvisionsensor)==0))
            {
                std::vector<float> corners;
                CPluginContainer::mesh_getOctreeDebugCorners(octree->getOctreeInfo(),corners);

                glBegin(GL_LINES);
                glNormal3fv(normalVectorForLinesAndPoints.data);

                for (size_t i=0;i<corners.size()/24;i++)
                {
                    glVertex3fv(&corners[0]+i*8*3+0);
                    glVertex3fv(&corners[0]+i*8*3+3);
                    glVertex3fv(&corners[0]+i*8*3+3);
                    glVertex3fv(&corners[0]+i*8*3+9);
                    glVertex3fv(&corners[0]+i*8*3+0);
                    glVertex3fv(&corners[0]+i*8*3+6);
                    glVertex3fv(&corners[0]+i*8*3+6);
                    glVertex3fv(&corners[0]+i*8*3+9);

                    glVertex3fv(&corners[0]+i*8*3+12);
                    glVertex3fv(&corners[0]+i*8*3+15);
                    glVertex3fv(&corners[0]+i*8*3+15);
                    glVertex3fv(&corners[0]+i*8*3+21);
                    glVertex3fv(&corners[0]+i*8*3+12);
                    glVertex3fv(&corners[0]+i*8*3+18);
                    glVertex3fv(&corners[0]+i*8*3+18);
                    glVertex3fv(&corners[0]+i*8*3+21);

                    glVertex3fv(&corners[0]+i*8*3+0);
                    glVertex3fv(&corners[0]+i*8*3+12);

                    glVertex3fv(&corners[0]+i*8*3+3);
                    glVertex3fv(&corners[0]+i*8*3+15);

                    glVertex3fv(&corners[0]+i*8*3+6);
                    glVertex3fv(&corners[0]+i*8*3+18);

                    glVertex3fv(&corners[0]+i*8*3+9);
                    glVertex3fv(&corners[0]+i*8*3+21);
                }
                glEnd();
            }

            if (octree->getCellSizeForDisplay()!=octree->getCellSize())
            { // we need to reconstruct the buffer cube:
                octree->setCellSizeForDisplay(octree->getCellSize());

                float ss=octree->getCellSize()*0.5f;
                int ind=0;
                _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=ss;
                _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=ss; _cubeVertices[ind++]=ss;
                _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=ss; _cubeVertices[ind++]=-ss;
                _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=-ss;

                _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=-ss;
                _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=ss; _cubeVertices[ind++]=-ss;
                _cubeVertices[ind++]=ss; _cubeVertices[ind++]=ss; _cubeVertices[ind++]=-ss;
                _cubeVertices[ind++]=ss; _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=-ss;

                _cubeVertices[ind++]=ss; _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=-ss;
                _cubeVertices[ind++]=ss; _cubeVertices[ind++]=ss; _cubeVertices[ind++]=-ss;
                _cubeVertices[ind++]=ss; _cubeVertices[ind++]=ss; _cubeVertices[ind++]=ss;
                _cubeVertices[ind++]=ss; _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=ss;

                _cubeVertices[ind++]=ss; _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=ss;
                _cubeVertices[ind++]=ss; _cubeVertices[ind++]=ss; _cubeVertices[ind++]=ss;
                _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=ss; _cubeVertices[ind++]=ss;
                _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=ss;

                _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=ss;
                _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=-ss;
                _cubeVertices[ind++]=ss; _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=-ss;
                _cubeVertices[ind++]=ss; _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=ss;

                _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=ss; _cubeVertices[ind++]=ss;
                _cubeVertices[ind++]=ss; _cubeVertices[ind++]=ss; _cubeVertices[ind++]=ss;
                _cubeVertices[ind++]=ss; _cubeVertices[ind++]=ss; _cubeVertices[ind++]=-ss;
                _cubeVertices[ind++]=-ss; _cubeVertices[ind++]=ss; _cubeVertices[ind++]=-ss;

                octree->setVertexBufferId(-1);
                octree->setNormalBufferId(-1);
            }

            if (setOtherColor)
            {
                if (octree->getUsePointsInsteadOfCubes())
                {
                    glPointSize(float(octree->getPointSize()));
                    ogl::drawRandom3dPoints(&_voxelPositions[0],_voxelPositions.size()/3,normalVectorForLinesAndPoints.data);
                    glPointSize(1.0);
                }
                else
                {
                    for (size_t i=0;i<_voxelPositions.size()/3;i++)
                    {
                        glPushMatrix();
                        glTranslatef(_voxelPositions[3*i+0],_voxelPositions[3*i+1],_voxelPositions[3*i+2]);
                        int _vertexBufferId=octree->getVertexBufferId();
                        int _normalBufferId=octree->getNormalBufferId();
                        _drawTriangles(_cubeVertices,24,_cubeIndices,36,_cubeNormals,NULL,&_vertexBufferId,&_normalBufferId,NULL);
                        octree->setVertexBufferId(_vertexBufferId);
                        octree->setNormalBufferId(_normalBufferId);
                        glPopMatrix();
                    }
                }
            }
            else
            {
                const float blk[4]={0.0,0.0,0.0,0.0};
                glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,blk);
                if (octree->getUsePointsInsteadOfCubes())
                {
                    glPointSize(float(octree->getPointSize()));
                    ogl::drawRandom3dPointsEx(&_voxelPositions[0],_voxelPositions.size()/3,NULL,octree->getColors(),NULL,octree->getColorIsEmissive(),normalVectorForLinesAndPoints.data);
                    glPointSize(1.0);
                }
                else
                {
                    if (octree->getColorIsEmissive())
                    {
                        for (size_t i=0;i<_voxelPositions.size()/3;i++)
                        {
                            glPushMatrix();
                            glTranslatef(_voxelPositions[3*i+0],_voxelPositions[3*i+1],_voxelPositions[3*i+2]);
                            glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,octree->getColors()+4*i);
                            int _vertexBufferId=octree->getVertexBufferId();
                            int _normalBufferId=octree->getNormalBufferId();
                            _drawTriangles(_cubeVertices,24,_cubeIndices,36,_cubeNormals,NULL,&_vertexBufferId,&_normalBufferId,NULL);
                            octree->setVertexBufferId(_vertexBufferId);
                            octree->setNormalBufferId(_normalBufferId);
                            glEnd();
                            glPopMatrix();
                        }
                    }
                    else
                    {
                        for (size_t i=0;i<_voxelPositions.size()/3;i++)
                        {
                            glPushMatrix();
                            glTranslatef(_voxelPositions[3*i+0],_voxelPositions[3*i+1],_voxelPositions[3*i+2]);
                            glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,octree->getColors()+4*i);
                            int _vertexBufferId=octree->getVertexBufferId();
                            int _normalBufferId=octree->getNormalBufferId();
                            _drawTriangles(_cubeVertices,24,_cubeIndices,36,_cubeNormals,NULL,&_vertexBufferId,&_normalBufferId,NULL);
                            octree->setVertexBufferId(_vertexBufferId);
                            octree->setNormalBufferId(_normalBufferId);
                            glEnd();
                            glPopMatrix();
                        }
                    }
                }
            }
        }


        glDisable(GL_CULL_FACE);
        _disableAuxClippingPlanes();
    }

    // At the end of every 3DObject display routine:
    _commonFinish(octree,renderingObject);
}

#else

void displayOctree(COctree* octree,CViewableBase* renderingObject,int displayAttrib)
{

}

#endif



