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

#include "vrepMainHeader.h"
#include "broadcastData.h"
#include "vMutex.h"
#include "broadcastDataVisual.h"

class CBroadcastDataContainer
{
public:
    CBroadcastDataContainer();
    virtual ~CBroadcastDataContainer();

    void visualizeCommunications(int pcTimeInMs);
    void removeAllObjects();
    void removeObject(int index);
    void simulationAboutToStart();
    void simulationEnded();
    void removeTimedOutObjects(float simulationTime);
    void broadcastData(int emitterID,int targetID,int dataHeader,std::string& dataName,float timeOutSimulationTime,float actionRadius,int antennaHandle,float emissionAngle1,float emissionAngle2,const char* data,int dataLength);
    char* receiveData(int receiverID,float simulationTime,int dataHeader,std::string& dataName,int antennaHandle,int& dataLength,int index,int& senderID,int& dataHeaderR,std::string& dataNameR);

    static bool getWirelessForceShow_emission();
    static void setWirelessForceShow_emission(bool f);
    static bool getWirelessForceShow_reception();
    static void setWirelessForceShow_reception(bool f);

private:
    VMutex _objectMutex;

    std::vector<CBroadcastData*> _allObjects;
    std::vector<CBroadcastDataVisual*> _allVisualObjects;
    static bool _wirelessForceShow_emission;
    static bool _wirelessForceShow_reception;
};
