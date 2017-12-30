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
#include "ghostObject.h"
#include "app.h"
#include "ghostRendering.h"

CGhostObject::CGhostObject()
{ // only for serialization or copy
    transparencyFactor=0;
}

CGhostObject::CGhostObject(int theGroupId,int theObjectHandle,C7Vector theTr,int theOptions,float theStartTime,float theEndTime,const float theColor[12])
{
    // options: bit0 set (1): model instead of object
    // options: bit1 set (2): real-time playback
    // options: bit2 set (4): original colors
    // options: bit3 set (8): force invisible objects to appear too
    // options: bit4 set (16): ghost is invisible (maybe temporarily)
    // options: bit5 set (32): backface culling (when using custom colors only)

    groupId=theGroupId;
    objectHandle=theObjectHandle;
    options=theOptions;
    startTime=theStartTime;
    endTime=theEndTime;
    tr=theTr;
    transparencyFactor=0;
    if (theColor!=NULL)
    {
        for (int i=0;i<12;i++)
            color[i]=theColor[i];
    }
    else
    {
        for (int i=0;i<12;i++)
            color[i]=0.5f;
    }
}

CGhostObject::~CGhostObject()
{
}

void CGhostObject::modifyAttributes(int theOptions,int theOptionsMask)
{
    // options: bit0 set (1): model instead of object
    // options: bit1 set (2): real-time playback
    // options: bit2 set (4): original colors
    // options: bit3 set (8): force invisible objects to appear too
    // options: bit4 set (16): ghost is invisible (maybe temporarily)
    options=((options|theOptionsMask)-theOptionsMask)|(theOptions&theOptionsMask);
}

CGhostObject* CGhostObject::copyYourself()
{
    CGhostObject* newGhost=new CGhostObject();
    newGhost->groupId=groupId;
    newGhost->ghostId=ghostId;
    newGhost->objectHandle=objectHandle;
    newGhost->options=options;
    newGhost->startTime=startTime;
    newGhost->endTime=endTime;
    newGhost->transparencyFactor=transparencyFactor;
    for (int i=0;i<12;i++)
        newGhost->color[i]=color[i];
    newGhost->tr=tr;
    return(newGhost);
}

void CGhostObject::render(int displayAttributes,float simulationTime,float realTime)
{
    if ((options&16)!=0)
        return; // invisible
    bool doIt=false;
    if ((options&2)!=0)
        doIt=(realTime>=startTime)&&(realTime<endTime);
    else
        doIt=(simulationTime>=startTime-0.00005f)&&(simulationTime<endTime-0.00005f);
    if (doIt)
    {
        CShape* shape=App::ct->objCont->getShape(objectHandle);
        if (shape!=NULL)
            displayGhost(shape,tr,displayAttributes,options,float(transparencyFactor)/255.0f,color);
    }
}
