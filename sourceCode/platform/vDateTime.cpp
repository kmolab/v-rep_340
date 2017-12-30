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
#include "vDateTime.h"
#ifndef SIM_WITHOUT_QT_AT_ALL
#include <QDateTime>
#include <QElapsedTimer>
#endif
#include <ctime>
#ifdef WIN_VREP
#include <Windows.h>
#else
#include <sys/time.h>
#endif

unsigned int VDateTime::getOSTimeInMs()
{
#ifdef WIN_VREP
    return(timeGetTime());
#else
    struct timeval now;
    gettimeofday(&now,NULL);
    return(now.tv_sec*1000+now.tv_usec/1000);
#endif
}

int VDateTime::getTimeInMs()
{
    bool ok;
    int retVal=0;
#ifdef WIN_VREP
    retVal=_getTimeWithStartInMs_viaPerformanceCounter(ok);
    if (ok)
        return(retVal);
#endif // WIN_VREP
    retVal=_getTimeWithStartInMs_viaQt(ok);
    if (ok)
        return(retVal);
#ifdef WIN_VREP
    retVal=_getTimeWithStartInMs_viaTimeGetTime(ok);
#else // WIN_VREP
    retVal=_getTimeWithStartInMs_viaGetTimeOfDay(ok);
#endif // WIN_VREP
    return(retVal);
}

int VDateTime::getTimeDiffInMs(int lastTime)
{
    return(getTimeDiffInMs(lastTime,getTimeInMs()));
}

int VDateTime::getTimeDiffInMs(int oldTime,int newTime)
{
    return(newTime-oldTime);
}

quint64 VDateTime::getSecondsSince1970()
{
#ifdef SIM_WITHOUT_QT_AT_ALL
    return(std::time(0));
#else
    QDateTime now=QDateTime::currentDateTime();
    return(now.toTime_t());
#endif
}

void VDateTime::getYearMonthDayHourMinuteSecond(int* year,int* month,int* day,int* hour,int* minute,int* second)
{
#ifdef SIM_WITHOUT_QT_AT_ALL
    time_t now=std::time(0);
    tm *ltm=std::localtime(&now);
    if (year!=NULL)
        year[0]=1900+ltm->tm_year;
    if (month!=NULL)
        month[0]=1+ltm->tm_mon;
    if (day!=NULL)
        day[0]=ltm->tm_mday;
    if (hour!=NULL)
        hour[0]=ltm->tm_hour;
    if (minute!=NULL)
        minute[0]=ltm->tm_min;
    if (second!=NULL)
        second[0]=ltm->tm_sec;
#else
    QDate now=QDate::currentDate();
    QTime nowTime=QTime::currentTime();
    if (year!=NULL)
        year[0]=now.year();
    if (month!=NULL)
        month[0]=now.month();
    if (day!=NULL)
        day[0]=now.day();
    if (hour!=NULL)
        hour[0]=nowTime.hour();
    if (minute!=NULL)
        minute[0]=nowTime.minute();
    if (second!=NULL)
        second[0]=nowTime.second();
#endif
}

int VDateTime::getDaysTo(int year_before,int month_before,int day_before,int year_after,int month_after,int day_after)
{
#ifdef SIM_WITHOUT_QT_AT_ALL
    tm tmp;
    tmp.tm_year=year_before-1900;
    tmp.tm_mon=month_before-1;
    tmp.tm_mday=day_before;
    tmp.tm_hour=0;
    tmp.tm_min=0;
    tmp.tm_sec=0;
    time_t tbefore=mktime(&tmp);
    tmp.tm_year=year_after-1900;
    tmp.tm_mon=month_after-1;
    tmp.tm_mday=day_after;
    time_t tafter=mktime(&tmp);
    return(int(difftime(tafter,tbefore)/86400.0f));
#else
    QDate before(year_before,month_before,day_before);
    return(before.daysTo(QDate(year_after,month_after,day_after)));
#endif
}

int VDateTime::_getTimeWithStartInMs_viaQt(bool& success)
{
#ifdef SIM_WITHOUT_QT_AT_ALL
    success=false;
    return(0);
#else
    static bool first=true;
    static QTime firstTime;
    if (first)
    {
        first=false;
        firstTime.start();
    }
    success=true;
    return(firstTime.elapsed());
#endif
}

#ifdef WIN_VREP
int VDateTime::_getTimeWithStartInMs_viaPerformanceCounter(bool& success)
{
    static bool works=true;
    static bool first=true;
    static double pcFreq=0.0;
    static __int64 cntStart=0;
    LARGE_INTEGER highResFreq;
    if (first&&works)
    {
        first=false;
        works=(QueryPerformanceFrequency(&highResFreq)!=0);
        pcFreq=double(highResFreq.QuadPart)/1000.0;
        QueryPerformanceCounter(&highResFreq);
        cntStart=highResFreq.QuadPart;
    }
    if (!works)
    {
        success=false;
        return(0);
    }
    success=true;
    QueryPerformanceCounter(&highResFreq);
    return(int(double(highResFreq.QuadPart-cntStart)/pcFreq));
}

int VDateTime::_getTimeWithStartInMs_viaTimeGetTime(bool& success)
{
    success=true;
    unsigned int t=timeGetTime();
    static unsigned int startT=t;
    if (t>=startT)
        return((t-startT)&0x8fffffff);
    return((t+(0xffffffff-startT))&0x8fffffff);
}
#else // WIN_VREP
int VDateTime::_getTimeWithStartInMs_viaGetTimeOfDay(bool& success)
{
    struct timeval now;
    success=(gettimeofday(&now,NULL)==0);
    static time_t initSec=now.tv_sec;
    static suseconds_t initUSec=now.tv_usec;
    return((now.tv_sec-initSec)*1000+(now.tv_usec-initUSec)/1000);
}
#endif // WIN_VREP

