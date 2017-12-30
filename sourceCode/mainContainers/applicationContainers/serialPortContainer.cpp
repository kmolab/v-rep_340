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
#include "serialPortContainer.h"
#include "v_rep_internal.h"
#include "tt.h"
#include "app.h"

CSerialPortContainer::CSerialPortContainer()
{

}

CSerialPortContainer::~CSerialPortContainer()
{
    _closeAllPorts();
}

void CSerialPortContainer::_closeAllPorts()
{ // closes all ports of this instance
    while (_openedSerialPorts.size()!=0)
    {
        if (App::userSettings->useAlternateSerialPortRoutines)
            serialPortClose_alternativeRoutine(_openedSerialPorts[0]->getPortHandle());
        else
            serialPortClose(_openedSerialPorts[0]->getPortHandle());
    }
}

int CSerialPortContainer::_addOpenedPort(CSerialPort* port)
{
    _openedSerialPorts.push_back(port);
    return(port->getPortHandle());
}

CSerialPort* CSerialPortContainer::_getPortFromPortNumber_backwardCompatibility(int portNb)
{ // for backward compatibility (10/04/2012)
    for (int i=0;i<int(_openedSerialPorts.size());i++)
    {
        if (_openedSerialPorts[i]->getPortNumber_backwardCompatibility()==portNb)
            return(_openedSerialPorts[i]);
    }
    return(NULL);
}


CSerialPort* CSerialPortContainer::_getPortFromHandle(int portHandle)
{
    for (int i=0;i<int(_openedSerialPorts.size());i++)
    {
        if (_openedSerialPorts[i]->getPortHandle()==portHandle)
            return(_openedSerialPorts[i]);
    }
    return(NULL);
}

void CSerialPortContainer::simulationEnded()
{ // Close only ports that were opened through a script:
    for (int i=0;i<int(_openedSerialPorts.size());i++)
    {
        if (_openedSerialPorts[i]->wasOpenedFromAScript())
        {
            if (App::userSettings->useAlternateSerialPortRoutines)
                serialPortClose_alternativeRoutine(_openedSerialPorts[i]->getPortHandle());
            else
                serialPortClose(_openedSerialPorts[i]->getPortHandle());
            i=-1; // We have to restart the loop from the beginning!
        }
    }
}

void CSerialPortContainer::executeCommand(SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut)
{
    if (cmdIn->cmdId==SERIAL_PORT_ALT_OPEN_SPUITHREADCMD)
        cmdOut->intParams.push_back(serialPortOpen_alternativeRoutine(cmdIn->boolParams[0],cmdIn->stringParams[0].c_str(),cmdIn->intParams[0]));
    if (cmdIn->cmdId==SERIAL_PORT_ALT_CLOSE_SPUITHREADCMD)
        cmdOut->boolParams.push_back(serialPortClose_alternativeRoutine(cmdIn->intParams[0]));
    if (cmdIn->cmdId==SERIAL_PORT_ALT_CHECK_SPUITHREADCMD)
        cmdOut->intParams.push_back(serialPortCheck_alternativeRoutine(cmdIn->intParams[0]));
    if (cmdIn->cmdId==SERIAL_PORT_ALT_SEND_SPUITHREADCMD)
        cmdOut->intParams.push_back(serialPortSend_alternativeRoutine(cmdIn->intParams[0],cmdIn->stringParams[0]));
    if (cmdIn->cmdId==SERIAL_PORT_ALT_RECEIVE_SPUITHREADCMD)
    {
        std::string data;
        cmdOut->intParams.push_back(serialPortReceive_alternativeRoutine(cmdIn->intParams[0],data,cmdIn->intParams[1]));
        cmdOut->stringParams.push_back(data);
    }
}

int CSerialPortContainer::serialPortOpen_alternativeRoutine(bool fromScript,const char* name,int baudrate)
{
    if (VThread::isCurrentThreadTheUiThread())
    { // we are in the UI thread. We execute the command now:
        CSerialPort* port=new CSerialPort(fromScript,true);
        int result;
        if (port->open(name,baudrate)==0)
        {
            delete port;
            result=-1;
        }
        else
            result=_addOpenedPort(port);
        return(result);
    }
    else
    { // We are NOT in the UI thread. We execute the command via the UI thread:
        SUIThreadCommand cmdIn;
        SUIThreadCommand cmdOut;
        cmdIn.cmdId=SERIAL_PORT_ALT_OPEN_SPUITHREADCMD;
        cmdIn.boolParams.push_back(fromScript);
        cmdIn.stringParams.push_back(name);
        cmdIn.intParams.push_back(baudrate);
        App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
        return(cmdOut.intParams[0]);
    }
}

bool CSerialPortContainer::serialPortClose_alternativeRoutine(int portHandle)
{
    if (VThread::isCurrentThreadTheUiThread())
    { // we are in the UI thread. We execute the command now:
        CSerialPort* port=_getPortFromHandle(portHandle);
        if (port!=NULL)
        {
            for (int i=0;i<int(_openedSerialPorts.size());i++)
            {
                if (_openedSerialPorts[i]->getPortHandle()==portHandle)
                {
                    delete _openedSerialPorts[i];
                    _openedSerialPorts.erase(_openedSerialPorts.begin()+i);
                    break;
                }
            }
        }
        return(port!=NULL);
    }
    else
    { // We are NOT in the UI thread. We execute the command via the UI thread:
        SUIThreadCommand cmdIn;
        SUIThreadCommand cmdOut;
        cmdIn.cmdId=SERIAL_PORT_ALT_CLOSE_SPUITHREADCMD;
        cmdIn.intParams.push_back(portHandle);
        App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
        return(cmdOut.boolParams[0]);
    }
}

int CSerialPortContainer::serialPortCheck_alternativeRoutine(int portHandle)
{
    if (VThread::isCurrentThreadTheUiThread())
    { // we are in the UI thread. We execute the command now:
        CSerialPort* port=_getPortFromHandle(portHandle);
        int result=-1;
        if (port!=NULL)
            result=port->readDataWaiting();
        return(result);
    }
    else
    { // We are NOT in the UI thread. We execute the command via the UI thread:
        SUIThreadCommand cmdIn;
        SUIThreadCommand cmdOut;
        cmdIn.cmdId=SERIAL_PORT_ALT_CHECK_SPUITHREADCMD;
        cmdIn.intParams.push_back(portHandle);
        App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
        return(cmdOut.intParams[0]);
    }
}

int CSerialPortContainer::serialPortSend_alternativeRoutine(int portHandle,const std::string& data)
{
    if (VThread::isCurrentThreadTheUiThread())
    { // we are in the UI thread. We execute the command now:
        CSerialPort* port=_getPortFromHandle(portHandle);
        int result=-1;
        if (port!=NULL)
        {
            result=0;
            if (data.size()>0)
                result=port->sendData(&data[0],data.size());
        }
        return(result);
    }
    else
    { // We are NOT in the UI thread. We execute the command via the UI thread:
        SUIThreadCommand cmdIn;
        SUIThreadCommand cmdOut;
        cmdIn.cmdId=SERIAL_PORT_ALT_SEND_SPUITHREADCMD;
        cmdIn.intParams.push_back(portHandle);
        cmdIn.stringParams.push_back(data);
        App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
        return(cmdOut.intParams[0]);
    }
}

int CSerialPortContainer::serialPortReceive_alternativeRoutine(int portHandle,std::string& data,int dataLengthToRead)
{
    if (VThread::isCurrentThreadTheUiThread())
    { // we are in the UI thread. We execute the command now:
        data.clear();
        CSerialPort* port=_getPortFromHandle(portHandle);
        int result=-1;
        if (port!=NULL)
        {
            result=0;
            int availableData=port->readDataWaiting();
            if (availableData>0)
            {
                data.resize(SIM_MIN(availableData,dataLengthToRead));
                result=port->readData(&data[0],SIM_MIN(availableData,dataLengthToRead));
            }
        }
        return(result);
    }
    else
    { // We are NOT in the UI thread. We execute the command via the UI thread:
        SUIThreadCommand cmdIn;
        SUIThreadCommand cmdOut;
        cmdIn.cmdId=SERIAL_PORT_ALT_RECEIVE_SPUITHREADCMD;
        cmdIn.intParams.push_back(portHandle);
        cmdIn.intParams.push_back(dataLengthToRead);
        App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
        data=cmdOut.stringParams[0];
        return(cmdOut.intParams[0]);
    }
}

int CSerialPortContainer::serialPortOpen(bool fromScript,const char* name,int baudrate)
{
    if (App::userSettings->useAlternateSerialPortRoutines)
        return(serialPortOpen_alternativeRoutine(fromScript,name,baudrate));
    else
    {
        CSerialPort* port=new CSerialPort(fromScript,false);
        if (port->open(name,baudrate)==0)
        {
            delete port;
            return(-1);
        }
        return(_addOpenedPort(port));
    }
}

bool CSerialPortContainer::serialPortClose(int portHandle)
{
    if (App::userSettings->useAlternateSerialPortRoutines)
        return(serialPortClose_alternativeRoutine(portHandle));
    else
    {
        bool retVal=false;
        for (int i=0;i<int(_openedSerialPorts.size());i++)
        {
            if (_openedSerialPorts[i]->getPortHandle()==portHandle)
            {
                delete _openedSerialPorts[i];
                _openedSerialPorts.erase(_openedSerialPorts.begin()+i);
                retVal=true;
                break;
            }
        }
        return(retVal);
    }
}

int CSerialPortContainer::serialPortCheck(int portHandle)
{
    if (App::userSettings->useAlternateSerialPortRoutines)
        return(serialPortCheck_alternativeRoutine(portHandle));
    else
    {
        int retVal=-1;
        CSerialPort* port=_getPortFromHandle(portHandle);
        if (port!=NULL)
            retVal=port->readDataWaiting();
        return(retVal);
    }
}

int CSerialPortContainer::serialPortSend(int portHandle,const std::string& data)
{
    if (App::userSettings->useAlternateSerialPortRoutines)
        return(serialPortSend_alternativeRoutine(portHandle,data));
    else
    {
        CSerialPort* port=_getPortFromHandle(portHandle);
        int retVal=-1;
        if (port!=NULL)
            retVal=port->sendData(&data[0],data.size());
        return(retVal);
    }
}

int CSerialPortContainer::serialPortReceive(int portHandle,std::string& data,int dataLengthToRead)
{
    if (App::userSettings->useAlternateSerialPortRoutines)
        return(serialPortReceive_alternativeRoutine(portHandle,data,dataLengthToRead));
    else
    {
        CSerialPort* port=_getPortFromHandle(portHandle);
        int retVal=0;
        if (port!=NULL)
        {
            int availableData=port->readDataWaiting();
            if (availableData>0)
                retVal=port->readData(&data[0],SIM_MIN(availableData,dataLengthToRead));
        }
        return(retVal);
    }
}

bool CSerialPortContainer::serialPortOpen_old(bool fromScript,int portNumber,int baudRate)
{ // old, for backward compatibility
    CSerialPort* port=new CSerialPort(fromScript,false);
    std::string theString("\\\\.\\COM");
    theString+=tt::FNb(portNumber);
    if (port->open(theString.c_str(),baudRate)==0)
        return(false);
    port->setPortNumber_backwardCompatibility(portNumber);
    _addOpenedPort(port);
    return(true);
}

bool CSerialPortContainer::serialPortClose_old(int portNumber)
{ // old, for backward compatibility
    CSerialPort* port=_getPortFromPortNumber_backwardCompatibility(portNumber);
    if (port==NULL)
        return(false);
    serialPortClose(port->getPortHandle());
    return(true);
}

int CSerialPortContainer::serialPortSend_old(int portNumber,const char* data,int dataLength)
{ // old, for backward compatibility
    CSerialPort* port=_getPortFromPortNumber_backwardCompatibility(portNumber);
    if (port==NULL)
        return(-1);
    return(port->sendData(data,dataLength));
}

int CSerialPortContainer::serialPortReceive_old(int portNumber,char* buffer,int dataLengthToRead)
{ // old, for backward compatibility
    CSerialPort* port=_getPortFromPortNumber_backwardCompatibility(portNumber);
    if (port==NULL)
        return(-1);
    int retVal=0;
    int availableData=port->readDataWaiting();
    if (availableData!=0)
        retVal=port->readData(buffer,SIM_MIN(availableData,dataLengthToRead));
    return(retVal);
}
