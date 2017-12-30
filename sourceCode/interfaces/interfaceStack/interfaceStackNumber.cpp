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

#include "interfaceStackNumber.h"

CInterfaceStackNumber::CInterfaceStackNumber(double theValue)
{
    _objectType=STACK_OBJECT_NUMBER;
    _value=theValue;
}

CInterfaceStackNumber::~CInterfaceStackNumber()
{
}

double CInterfaceStackNumber::getValue()
{
    return(_value);
}

void CInterfaceStackNumber::setValue(double theValue)
{
    _value=theValue;
}

CInterfaceStackObject* CInterfaceStackNumber::copyYourself()
{
    CInterfaceStackNumber* retVal=new CInterfaceStackNumber(_value);
    return(retVal);
}

void CInterfaceStackNumber::printContent(int spaces) const
{
    for (int i=0;i<spaces;i++)
        printf(" ");
    printf("NUMBER: %f\n",_value);
}

std::string CInterfaceStackNumber::getObjectData() const
{
    std::string retVal;
    char* tmp=(char*)(&_value);
    for (size_t i=0;i<sizeof(_value);i++)
        retVal.push_back(tmp[i]);
    return(retVal);
}

unsigned int CInterfaceStackNumber::createFromData(const char* data)
{
    char* tmp=(char*)(&_value);
    for (size_t i=0;i<sizeof(_value);i++)
        tmp[i]=data[i];
    return(sizeof(_value));
}

bool CInterfaceStackNumber::checkCreateFromData(const char* data,unsigned int& w,unsigned int l)
{
    if (l<sizeof(double))
        return(false);
    w=sizeof(double);
    return(true);
}
