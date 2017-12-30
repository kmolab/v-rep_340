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

#pragma once

#include "visualParam.h"
#include "4X4Matrix.h"
#include "vMutex.h"

class CDrawingObject  
{
public:
    CDrawingObject(int theObjectType,float size,float duplicateTolerance,int sceneObjID,int maxItemCount,bool createdFromScript);
    virtual ~CDrawingObject();

    void draw(bool overlay,bool transparentObject,int displayAttrib,const C4X4Matrix& cameraCTM);
    void setObjectID(int newID);
    int getObjectID() const;
    bool addItem(const float* itemData);
    int getObjectType() const;
    bool announceObjectWillBeErased(int objID);
    void adjustForFrameChange(const C7Vector& preCorrection);
    void adjustForScaling(float xScale,float yScale,float zScale);

    bool canMeshBeExported() const;
    void getExportableMesh(std::vector<float>& vertices,std::vector<int>& indices) const;

    int getSceneObjectID() const;
    bool getCreatedFromScript() const;
    void setCreatedFromScript(bool c);
    bool getPersistent() const;
    void setPersistent(bool c);

    float getSize() const;
    int getMaxItemCount() const;
    int getStartItem() const;

    std::vector<float>* getDataPtr();

    CVisualParam color;

    int verticesPerItem;
    int normalsPerItem;
    int otherFloatsPerItem;
    int floatsPerItem;

protected:
    void _exportTrianglePoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices) const;
    void _exportQuadPoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices) const;
    void _exportDiscPoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices) const;
    void _exportCubePoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices) const;
    void _exportSpherePoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices) const;
    void _exportTriangles(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices) const;
    void _exportTriOrQuad(C7Vector& tr,C3Vector* v0,C3Vector* v1,C3Vector* v2,C3Vector* v3,std::vector<float>& vertices,std::vector<int>& indices,int& nextIndex) const;

    void _setItemSizes();

    int _objectID;
    int _sceneObjectID;
    int _objectType;
    float _size;
    int _maxItemCount;
    int _startItem;
    float _duplicateTolerance;
    bool _createdFromScript;
    bool _persistent;

    VMutex _objectMutex;

    std::vector<float> _data;
};
