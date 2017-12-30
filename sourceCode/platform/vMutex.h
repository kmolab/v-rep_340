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
#ifndef SIM_WITHOUT_QT_AT_ALL
#include <QMutex>
#include <QWaitCondition>
typedef QMutex WMutex;
typedef QWaitCondition WWaitCondition;
#else // SIM_WITHOUT_QT_AT_ALL
#ifdef WIN_VREP
#include <Windows.h>
typedef CRITICAL_SECTION WMutex;
typedef std::vector<int> WWaitCondition;
#else // WIN_VREP
#include <pthread.h>
typedef pthread_mutex_t WMutex;
typedef pthread_cond_t WWaitCondition;
#endif // WIN_VREP
#endif // SIM_WITHOUT_QT_AT_ALL

#define VTHREAD_ID_DEAD 0

class VMutex
{
public:
    VMutex();
    virtual ~VMutex();

    // When using recursive mutexes:
    void lock();
    bool tryLock();
    void unlock();

    // When using non-recursive mutexes:
    void lock_simple();
    bool tryLock_simple();
    void unlock_simple();

    // Wait conditions:
    void wait_simple();
    void wakeAll_simple();

private:
#ifdef SIM_WITHOUT_QT_AT_ALL
    bool _areThreadIDsSame(VTHREAD_ID_TYPE threadA,VTHREAD_ID_TYPE threadB);
    VTHREAD_ID_TYPE _getCurrentThreadId();
    void _switchThread();
    void __sl(WMutex mutex);
    void __su(WMutex mutex);

    int _lockLevel;
    VTHREAD_ID_TYPE _lockThreadId;
#ifdef WIN_VREP
    int _nextWaitConditionId;
#endif // WIN_VREP
#endif // SIM_WITHOUT_QT_AT_ALL

    WMutex _recursiveMutex;
    WMutex _simpleMutex;
    WWaitCondition _simpleWaitCondition;
};

