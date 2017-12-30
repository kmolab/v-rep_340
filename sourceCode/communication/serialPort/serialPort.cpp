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
#include "serialPort.h"

#ifdef WIN_VREP
#include "auxLib.h"
#else
#include "serial_mac_linux.h"
#endif

int CSerialPort::_qserialPort_nextHandle=0;

CSerialPort::CSerialPort(bool wasOpenedFromAScript,bool useQSerialPort)
{
    _useQSerialPort=useQSerialPort;
    _qserialPort=NULL;
    _portHandle=-1;
    _portNumber_backwardCompatibility=-1;
    _wasOpenedFromAScript=wasOpenedFromAScript;
}

CSerialPort::~CSerialPort()
{
    if (_portHandle>=0)
        close();
}

void CSerialPort::setPortNumber_backwardCompatibility(int nb)
{
    _portNumber_backwardCompatibility=nb;
}

int CSerialPort::getPortNumber_backwardCompatibility()
{
    return(_portNumber_backwardCompatibility);
}

int CSerialPort::getPortHandle()
{
    return(_portHandle);
}

bool CSerialPort::wasOpenedFromAScript()
{
    return(_wasOpenedFromAScript);
}

bool CSerialPort::open(const char* portString,int nBaud)
{
    if (!_useQSerialPort)
    {
#ifdef WIN_VREP
        _portHandle=CAuxLib::vrepWin_serialOpen((char*)portString,nBaud);
#else
        _portHandle=serialOpen(portString,nBaud);
#endif
    }
    else
    {
        _portHandle=-1; // means error
        _qserialPort=new QSerialPort();

        if (_qserialPort->setBaudRate(nBaud))
        {
//          _qserialPort->setFlowControl(QSerialPort::HardwareControl);
            _qserialPort->setPortName(portString);
            if (_qserialPort->open(QIODevice::ReadWrite))
                _portHandle=_qserialPort_nextHandle++;
        }
        if (_portHandle<0)
        {
            delete _qserialPort;
            _qserialPort=NULL;
        }
    }
    return (_portHandle>=0);
}

bool CSerialPort::close()
{
    if (_portHandle>=0)
    {
        if (!_useQSerialPort)
        {
#ifdef WIN_VREP
            return(CAuxLib::vrepWin_serialClose(_portHandle)!=0);
#else
            return(serialClose(_portHandle)==0);
#endif
        }
        else
        {
            _qserialPort->close();
            delete _qserialPort;
            _qserialPort=NULL;
            return(true);
        }
    }
    return(false);
}

int CSerialPort::sendData(const char *buffer,int size)
{
    if (_portHandle>=0)
    {
        if (!_useQSerialPort)
        {
#ifdef WIN_VREP
            return(CAuxLib::vrepWin_serialWrite(_portHandle,buffer,size));
#else
            return(serialWrite(_portHandle,buffer,size));
#endif
        }
        else
        {
            int ret=_qserialPort->write(buffer,size);
            _qserialPort->flush();
            return(ret);
        }
    }
    return(0);
}


int CSerialPort::readDataWaiting( void )
{
    if (_portHandle>=0)
    {
        if (!_useQSerialPort)
        {
#ifdef WIN_VREP
            return(CAuxLib::vrepWin_serialCheck(_portHandle));
#else
            return(serialCheck(_portHandle));
#endif
        }
        else
            return(_qserialPort->bytesAvailable());
    }
    return(0);
}

int CSerialPort::readData(void *buffer,int limit)
{
    if (_portHandle>=0)
    {
        if (!_useQSerialPort)
        {
#ifdef WIN_VREP
            return(CAuxLib::vrepWin_serialRead(_portHandle,(char*)buffer,limit));
#else
            return(serialRead(_portHandle,(char*)buffer,limit));
#endif
        }
        else
            return(_qserialPort->read((char*)buffer,limit));
    }
    return(0);
}

