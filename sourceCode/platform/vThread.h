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
#include "vMutex.h"
#ifndef SIM_WITHOUT_QT_AT_ALL
#include <QTime>
#include <QThread>
#endif
#ifdef WIN_VREP
#define VTHREAD_RETURN_TYPE quint32
#else
#include <unistd.h>
#define VTHREAD_RETURN_TYPE void*
#endif
#define VTHREAD_START_ADDRESS AFX_THREADPROC_TT
#define VTHREAD_ARGUMENT_TYPE void*
typedef unsigned int( __cdecl* SIMPLE_VTHREAD_START_ADDRESS )( void* );
#define SIMPLE_VTHREAD_RETURN_TYPE unsigned int
#define SIMPLE_VTHREAD_ARGUMENT_TYPE void*

// FULLY STATIC CLASS
class VThread  
{
public:
    static void launchThread(VTHREAD_START_ADDRESS startAddress,unsigned short priority,bool followMainThreadAffinity);
#ifndef SIM_WITHOUT_QT_AT_ALL
    static void launchSimpleThread(SIMPLE_VTHREAD_START_ADDRESS startAddress,unsigned short priority);
#endif
    static void setProcessorCoreAffinity(int mode); // negative=multicore, 0=any single core, otherwise affinity mask (bit1=core1, bit2=core2, etc.)
    static int getThreadId_apiQueried();
    static int getCoreCount();
    static bool isSimulationMainThreadIdSet();
    static void setSimulationMainThreadId();
    static bool isUiThreadIdSet();
    static void setUiThreadId();
    static bool isCurrentThreadTheMainSimulationThread();
    static bool isCurrentThreadTheUiThread();
    static bool areThreadIDsSame(VTHREAD_ID_TYPE threadA,VTHREAD_ID_TYPE threadB);
    static VTHREAD_ID_TYPE getCurrentThreadId();
    static void switchThread();
    static void sleep(int ms);

private:
    static bool _simulationMainThreadIdSet;
    static bool _uiThreadIdSet;
    static VTHREAD_ID_TYPE _simulationMainThreadId;
    static VTHREAD_ID_TYPE _uiThreadId;
    static VMutex _lock;
    static std::vector<VTHREAD_ID_TYPE> _apiQueriedThreadIds;
#ifdef WIN_VREP
    static unsigned long _mainThreadAffinityMask;
#else
    static quint32 _mainThreadAffinityMask;
#endif

public:
    static unsigned short VTHREAD_LOWEST_PRIORITY;
    static unsigned short VTHREAD_NORMAL_PRIORITY;
    static unsigned short VTHREAD_HIGHEST_PRIORITY;
};

