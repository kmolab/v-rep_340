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

class CBannerObject  
{
public:
    CBannerObject(const char* label,int options,int sceneObjID,const float relConfig[6],const float labelCol[12],const float backCol[12],float height);
    virtual ~CBannerObject();

    void draw3DStuff(bool overlay,bool transparentObject,int displayAttrib,const C4X4Matrix& cameraCTM,const int windowSize[2],float verticalViewSizeOrAngle,bool perspective);
    void setObjectID(int newID);
    int getObjectID();

    bool announceObjectWillBeErased(int objID);
    void adjustForFrameChange(const C7Vector& preCorrection);
    void adjustForScaling(float xScale,float yScale,float zScale);

    int getSceneObjectID();
    bool getCreatedFromScript();
    void setCreatedFromScript(bool c);
    int getOptions();
    int getParentObjectHandle();

    bool isVisible();
    bool toggleVisibility();

    CVisualParam color;
    CVisualParam backColor;

protected:
    std::string _label;
    int _objectID;
    int _sceneObjectID;
    int _options;
    bool _createdFromScript;
    float _height;
    bool _visible;
    C7Vector _relativeConfig;
};
