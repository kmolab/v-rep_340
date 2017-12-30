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
#include "v_rep_internal.h"
#include "broadcastDataVisual.h"
#include "app.h"
#include "vDateTime.h"
#include "broadcastDataVisualRendering.h"

CBroadcastDataVisual::CBroadcastDataVisual(float timeOutSimulationTime,float actionRadius,const C7Vector& emitterConf,float emissionAngle1,float emissionAngle2)
{ // displayed if not timed out
    _timeOutSimulationTime=timeOutSimulationTime;
    _actionRadius=actionRadius;
    _emitterConf=emitterConf;
    _emissionAngle1=emissionAngle1;
    _emissionAngle2=emissionAngle2;
    _emitter=true;
}

CBroadcastDataVisual::CBroadcastDataVisual(const C3Vector& emitterPos,const C3Vector& receiverPos)
{ // receiving. Displayed exactly once
    _timeOutSimulationTime=0.0f;
    _emitterConf.X=emitterPos;
    _receiverPos=receiverPos;
    _emitter=false;
}

bool CBroadcastDataVisual::doesRequireDestruction(float simulationTime)
{
    if (_emitter)
        return(simulationTime>_timeOutSimulationTime);
    return(_timeOutSimulationTime!=0.0f);
}

bool CBroadcastDataVisual::isReceiver()
{
    return(!_emitter);
}

CBroadcastDataVisual::~CBroadcastDataVisual()
{
}

void CBroadcastDataVisual::visualize()
{
    displayEmitterOrReceiver(this);
    if (!_emitter)
        _timeOutSimulationTime=1.0f; // indicates that this object will be destroyed
}
