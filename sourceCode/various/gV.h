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

//FULLY STATIC CLASS
class gv  
{
public:
    static std::string getAngleStr(bool sign,float angleInRad,int leadingZeros=-1);
    static std::string getSizeStr(bool sign,float sizeInMeter,int leadingZeros=-1);

    static std::string getTimeStr(float timeInSeconds,int additionalDecimals=0);
    static std::string getHourMinuteSecondMilisecondStr(float timeInSeconds);


    static std::string getAngleUnitStr();
    static std::string getSizeUnitStr();
    static std::string getSizeUnitStrShort();
    static std::string getMassUnitStr();
    static std::string getMassUnitStrShort();
    static std::string getSurfaceUnitStr();
    static std::string getVolumeUnitStr();
    static std::string getTimeUnitStr();
    static std::string getTimeUnitStrShort();
    static std::string getIntUnitStr();
    static std::string getFloatUnitStr();
    static std::string getBoolUnitStr();
    static std::string getCountUnitStr();
    static std::string getBoolStateUnitStr();
    static std::string getUserUnitStr();
    static std::string getNullUnitStr();
    static std::string formatUnitStr(const std::string& txt,const std::string& s,bool inParenthesis=true,bool startWithSpace=true);

    static float radToUser;
    static float userToRad;
    static float angularVelToUser;
    static float userToAngularVel;
    static float angularAccelToUser;
    static float userToAngularAccel;
};
