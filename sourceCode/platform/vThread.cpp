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
#include "vThread.h"
#ifdef WIN_VREP
#include "auxLib.h"
#endif
#ifndef SIM_WITHOUT_QT_AT_ALL
#include "wThread.h"
#endif

VMutex VThread::_lock;
bool VThread::_simulationMainThreadIdSet=false;
bool VThread::_uiThreadIdSet=false;
VTHREAD_ID_TYPE VThread::_simulationMainThreadId=VTHREAD_ID_DEAD;
VTHREAD_ID_TYPE VThread::_uiThreadId=VTHREAD_ID_DEAD;
std::vector<VTHREAD_ID_TYPE> VThread::_apiQueriedThreadIds;
unsigned short VThread::VTHREAD_LOWEST_PRIORITY=0;
unsigned short VThread::VTHREAD_NORMAL_PRIORITY=1;
unsigned short VThread::VTHREAD_HIGHEST_PRIORITY=2;
#ifdef WIN_VREP
unsigned long VThread::_mainThreadAffinityMask=0; // Means we use the OS default
#else
quint32 VThread::_mainThreadAffinityMask=0; // Means we use the OS default
#endif

void VThread::setProcessorCoreAffinity(int mode)
{
#ifdef WIN_VREP
    if (mode<0)
    {
        _mainThreadAffinityMask=0; // Means: we keep what the os choose
        return;
    }
    if (mode>0)
    { // we (try to) use the mask given in "mode":
        _mainThreadAffinityMask=mode;

        unsigned long oldAffinity=CAuxLib::vrepWin_setThreadAffinity(_mainThreadAffinityMask);
        if (oldAffinity==0)
            _mainThreadAffinityMask=0; // we failed setting what we wanted... we use the os default
        return;
    }
    else
    {
        bool found=false;
        for (int i=0;i<64;i++)
        {
            _mainThreadAffinityMask=(1<<i);
            unsigned long oldAffinity=CAuxLib::vrepWin_setThreadAffinity(_mainThreadAffinityMask);
            if (oldAffinity!=0)
            { // Means we could set the new affinity!
                found=true;
                break; 
            }
        }
        if (!found)
            _mainThreadAffinityMask=0;
    }
#else
    // not yet implemented
#endif
}

void VThread::launchThread(VTHREAD_START_ADDRESS startAddress,unsigned short priority,bool followMainThreadAffinity)
{
    int prio=1; // normal
    if (priority==VTHREAD_LOWEST_PRIORITY)
        prio=0;
    if (priority==VTHREAD_HIGHEST_PRIORITY)
        prio=2;
#ifdef WIN_VREP
    unsigned long aff=0;
    if ( (_mainThreadAffinityMask!=0)&&followMainThreadAffinity )
        aff=_mainThreadAffinityMask;
    CAuxLib::vrepWin_launchThread(startAddress,prio,aff);
#else
    quint32 aff=0;
    if ( (_mainThreadAffinityMask!=0)&&followMainThreadAffinity )
        aff=_mainThreadAffinityMask;
    pthread_t th;
    pthread_create(&th,NULL,startAddress,NULL);
#endif
}

#ifndef SIM_WITHOUT_QT_AT_ALL
void VThread::launchSimpleThread(SIMPLE_VTHREAD_START_ADDRESS startAddress,unsigned short priority)
{
    _lock.lock_simple();
    Thread* it=new Thread();
    Thread::startAddress=startAddress;
    Thread::startAddressIsFree=false;
    if (priority==VTHREAD_LOWEST_PRIORITY)
        it->start(QThread::LowestPriority);
    if (priority==VTHREAD_NORMAL_PRIORITY)
        it->start(QThread::NormalPriority);
    if (priority==VTHREAD_HIGHEST_PRIORITY)
        it->start(QThread::HighestPriority);
    while (!Thread::startAddressIsFree)
        QThread::yieldCurrentThread();
    _lock.unlock_simple();
}
#endif

int VThread::getThreadId_apiQueried()
{ // this is an artificial ID, just needed externally. 0=GUI thread, 1=main sim thread.
    if ((_apiQueriedThreadIds.size()==0)&&_simulationMainThreadIdSet&&_uiThreadIdSet)
    {
        _apiQueriedThreadIds.push_back(_uiThreadId);
        _apiQueriedThreadIds.push_back(_simulationMainThreadId);
    }
    if (_apiQueriedThreadIds.size()==0)
        return(-1);
    VTHREAD_ID_TYPE t=getCurrentThreadId();
    for (int i=0;i<int(_apiQueriedThreadIds.size());i++)
    {
        if (areThreadIDsSame(_apiQueriedThreadIds[i],t))
            return(i);
    }
    _apiQueriedThreadIds.push_back(t);
    return(_apiQueriedThreadIds.size()-1);
}

int VThread::getCoreCount()
{
    int retVal=0;
#ifdef SIM_WITHOUT_QT_AT_ALL
    retVal=1; // TODO_SIM_WITHOUT_QT_AT_ALL
#else
    retVal=QThread::idealThreadCount();
#endif
    if (retVal<1)
        retVal=1;
    return(retVal);
}

bool VThread::isSimulationMainThreadIdSet()
{
    return(_simulationMainThreadIdSet);
}

void VThread::setSimulationMainThreadId()
{
    _simulationMainThreadId=getCurrentThreadId();
    _simulationMainThreadIdSet=true;
}

bool VThread::isUiThreadIdSet()
{
    return(_uiThreadIdSet);
}

void VThread::setUiThreadId()
{
    _uiThreadId=getCurrentThreadId();
    _uiThreadIdSet=true;
}

bool VThread::isCurrentThreadTheUiThread()
{
    if (!_uiThreadIdSet)
        return(true); // the main thread is the UI thread
    return(areThreadIDsSame(_uiThreadId,getCurrentThreadId()));
}


bool VThread::isCurrentThreadTheMainSimulationThread()
{
    if (!_simulationMainThreadIdSet)
        return(false); // the initial thread is the one that will become the UI thread
    return(areThreadIDsSame(_simulationMainThreadId,getCurrentThreadId()));
}

bool VThread::areThreadIDsSame(VTHREAD_ID_TYPE threadA,VTHREAD_ID_TYPE threadB)
{
#ifdef WIN_VREP
    return(threadA==threadB); // normally this is the only thread ID comparison place!! Except for another one in VMutex
#else
    return(pthread_equal(threadA,threadB)!=0); // normally this is the only thread ID comparison place!! Except for another one in VMutex (THREAD_ID_COMPARISON)
#endif
}

VTHREAD_ID_TYPE VThread::getCurrentThreadId()
{
#ifdef WIN_VREP
    if (CAuxLib::vrepWin_getCurrentThreadId!=0)
        return(CAuxLib::vrepWin_getCurrentThreadId());
    return(0);
#else
    return(pthread_self());
#endif
}

void VThread::switchThread()
{
#ifdef WIN_VREP
    CAuxLib::vrepWin_switchThread();
#endif
#ifdef MAC_VREP
    pthread_yield_np();
#endif
#ifdef LIN_VREP
    pthread_yield();
#endif
}

void VThread::sleep(int ms)
{
#ifdef WIN_VREP
    if (CAuxLib::vrepWin_sleep!=NULL)
        CAuxLib::vrepWin_sleep(ms);
#else
    usleep(ms*1000);
#endif
}
