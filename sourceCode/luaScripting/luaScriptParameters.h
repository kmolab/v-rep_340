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
#include "ser.h"

struct SScriptParamEntry {
    std::string name;
    std::string unit;
    std::string value; // paramValue can contain embedded 0's since 2011/01/23
    int properties; // bit0 set: value not visible during simulation, bit1 set: value is not reset at simulation end, bit2 set: contains embedded 0's (since 2011/01/23)
};

class CLuaScriptParameters
{
public:
    CLuaScriptParameters();
    virtual ~CLuaScriptParameters();

    void initializeInitialValues(bool simulationIsRunning);
    void simulationEnded();

    void clearInitialParameters();

    // For following 2 functions, the paramName must be present for the function to return true:
    bool setParameterValue(const char* paramName,const char* paramValue,int paramValueLength); // paramValue can contain embedded 0's since 2011/01/23
    bool getParameterValue(const char* paramName,std::string& paramValue);

    // For following 3 functions, the paramName does not need to be present:
    void addParameterValue(const char* paramName,const char* unitText,const char* paramValue,int paramValueLength); // paramValue can contain embedded 0's since 2011/01/23
    bool removeParameterValue(const char* paramName);
    bool removeParameterValue(int index);
    int getParameterIndex(const char* paramName);
    void moveItem(int index,int newIndex);

    CLuaScriptParameters* copyYourself();
    void serialize(CSer& ar);

    std::vector<SScriptParamEntry> scriptParamEntries;
protected:

    std::vector<SScriptParamEntry> _initialScriptParamEntries;
    bool _initialValuesInitialized;
};
