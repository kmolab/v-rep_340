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
#include "gV.h"
#include "global.h"
#include "tt.h"
#include "MyMath.h"
#include <boost/lexical_cast.hpp>

std::string gv::getAngleUnitStr()
{
    return("Degrees");
}

std::string gv::getSizeUnitStr()
{
    return("Meters");
}

std::string gv::getSizeUnitStrShort()
{
    return("m");
}

std::string gv::getMassUnitStr()
{
    return("Kilograms");
}

std::string gv::getMassUnitStrShort()
{
    return("kg");
}

std::string gv::getSurfaceUnitStr()
{
    return("Square meters");
}

std::string gv::getVolumeUnitStr()
{
    return("Cubic meters");
}

std::string gv::getTimeUnitStr()
{
    return("Seconds");
}

std::string gv::getTimeUnitStrShort()
{
    return("s");
}

std::string gv::getIntUnitStr()
{
    return("Integer");
}

std::string gv::getFloatUnitStr()
{
    return("Floating point");
}

std::string gv::getBoolUnitStr()
{
    return("Boolean");
}

std::string gv::getCountUnitStr()
{
    return("Count");
}

std::string gv::getBoolStateUnitStr()
{
    return("Boolean state");
}

std::string gv::getUserUnitStr()
{
    return("User unit");
}

std::string gv::getNullUnitStr()
{
    return("Null");
}

std::string gv::formatUnitStr(const std::string& txt,const std::string& s,bool inParenthesis,bool startWithSpace)
{
    std::string str(txt);
    if (startWithSpace)
        str+=" ";
    if (inParenthesis)
        str+="[";
    for (int i=0;i<int(s.length());i++)
    {
        if (s[i]=='m')
            str+="m";
        else if (s[i]=='s')
            str+="s";
        else if (s[i]=='k')
            str+="kg";
        else if (s[i]=='r')
            str+="deg";
        else
            str+=s[i];
    }
    if (inParenthesis)
        str+="]";
    return(str);
}

std::string gv::getTimeStr(float timeInSeconds,int additionalDecimals)
{
    return(tt::FNb(0,timeInSeconds,3+additionalDecimals,false));
}

std::string gv::getHourMinuteSecondMilisecondStr(float timeInSeconds)
{
    std::string tmp(tt::FNb(2,int(timeInSeconds/3600.0f),false));
    tmp+=":";
    tmp+=tt::FNb(2,int(CMath::robustFmod(timeInSeconds/60.0f,60.0f)),false)+":";
    tmp+=tt::FNb(2,int(CMath::robustFmod(timeInSeconds,60.0f)),false)+".";
    tmp+=tt::FNb(2,int(CMath::robustFmod(timeInSeconds*100.0f,100.0f)),false);
    return(tmp);
}


std::string gv::getAngleStr(bool sign,float angleInRad,int leadingZeros)
{ // if leadingZeros==-1, the default leading zeros are used
    if (leadingZeros==-1)
        leadingZeros=3;
    return(tt::FNb(leadingZeros,angleInRad*radToUser,2,sign));
}

std::string gv::getSizeStr(bool sign,float sizeInMeter,int leadingZeros)
{ // if leadingZeros==-1, the default leading zeros are used
    if (leadingZeros==-1)
        leadingZeros=1;
    return(tt::FNb(leadingZeros,sizeInMeter,4,sign));
}

float gv::radToUser=radToDeg;
float gv::userToRad=degToRad;
float gv::angularVelToUser=radToDeg;
float gv::userToAngularVel=degToRad;
float gv::angularAccelToUser=radToDeg;
float gv::userToAngularAccel=degToRad;


