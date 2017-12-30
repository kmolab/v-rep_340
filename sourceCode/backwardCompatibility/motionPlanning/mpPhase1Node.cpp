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

#include "mpPhase1Node.h"
#include "v_repLib.h"

CmpPhase1Node::CmpPhase1Node(int jointCount,const float* _jointPositions,const C7Vector& _tipPose)
{
    jointPositions=new float[jointCount];
// KEEP!!   neighbourIndices=new int[jointCount*2];
    tipPose=_tipPose;
    attributes=0;
    for (int i=0;i<jointCount;i++)
        jointPositions[i]=_jointPositions[i];
// KEEP!!   for (int i=0;i<jointCount*2;i++)
// KEEP!!       neighbourIndices[i]=-1;
}

CmpPhase1Node::~CmpPhase1Node()
{
// KEEP!!   delete[] neighbourIndices;
    delete[] jointPositions;
}

CmpPhase1Node* CmpPhase1Node::copyYourself(int jointCount)
{
    CmpPhase1Node* newNode=new CmpPhase1Node(jointCount,jointPositions,tipPose);
// KEEP!!   for (int i=0;i<jointCount*2;i++)
// KEEP!!       newNode->neighbourIndices[i]=neighbourIndices[i];
    newNode->attributes=attributes;
    return(newNode);
}

/* KEEP!!
void CmpPhase1Node::setNeighbours(int index,int theNeighbourIndex)
{
    neighbourIndices[index]=theNeighbourIndex;
}
*/

/*
void CmpPhase1Node::setColliding(bool colliding)
{
    if (colliding)
        attributes&=254;
    else
        attributes|=1;
}
*/
