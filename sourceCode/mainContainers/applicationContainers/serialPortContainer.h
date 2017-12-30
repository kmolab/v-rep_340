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
#include "serialPort.h"
#include "uiThread.h"

class CSerialPortContainer
{
public:
    CSerialPortContainer();
    virtual ~CSerialPortContainer();

    void simulationEnded();

    // From UI thread:
    void executeCommand(SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut);

    // General routines, from UI or SIM thread:
    int serialPortOpen(bool fromScript,const char* name,int baudrate);
    bool serialPortClose(int portHandle);
    int serialPortCheck(int portHandle);
    int serialPortSend(int portHandle,const std::string& data);
    int serialPortReceive(int portHandle,std::string& data,int dataLengthToRead);

    // Alternative routines from any thread:
    int serialPortOpen_alternativeRoutine(bool fromScript,const char* name,int baudrate);
    bool serialPortClose_alternativeRoutine(int portHandle);
    int serialPortCheck_alternativeRoutine(int portHandle);
    int serialPortSend_alternativeRoutine(int portHandle,const std::string& data);
    int serialPortReceive_alternativeRoutine(int portHandle,std::string& data,int dataLengthToRead);

    // Old routines, for backward compatibility:
    bool serialPortOpen_old(bool fromScript,int portNumber,int baudRate);
    bool serialPortClose_old(int portNumber);
    int serialPortSend_old(int portNumber,const char* data,int dataLength);
    int serialPortReceive_old(int portNumber,char* buffer,int dataLengthToRead);

protected:
    void _closeAllPorts();
    CSerialPort* _getPortFromHandle(int portHandle);
    int _addOpenedPort(CSerialPort* port);
    CSerialPort* _getPortFromPortNumber_backwardCompatibility(int portNb);

    std::vector<CSerialPort*> _openedSerialPorts;
};
