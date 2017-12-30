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

#include "interfaceStackString.h"

CInterfaceStackString::CInterfaceStackString(const char* str,int l)
{
    _objectType=STACK_OBJECT_STRING;
    if (l==0)
    {
        if (str!=NULL)
            _value.assign(str);
    }
    else
        _value.assign(str,str+l);
}

CInterfaceStackString::~CInterfaceStackString()
{
}

const char* CInterfaceStackString::getValue(int* l)
{
    if (l!=NULL)
        l[0]=_value.size();
    return(_value.c_str());
}

void CInterfaceStackString::setValue(const char* str,int l)
{
    _value.assign(str,str+l);
}

CInterfaceStackObject* CInterfaceStackString::copyYourself()
{
    CInterfaceStackString* retVal=new CInterfaceStackString(_value.c_str(),_value.length());
    return(retVal);
}

void CInterfaceStackString::printContent(int spaces) const
{
    for (int i=0;i<spaces;i++)
        printf(" ");
    if (std::string(_value.c_str()).size()==_value.size())
        printf("STRING: %s\n",_value.c_str());
    else
        printf("STRING: <buffer data>\n");
}

std::string CInterfaceStackString::getObjectData() const
{
    std::string retVal;
    unsigned int l=_value.size();
    char* tmp=(char*)(&l);
    for (size_t i=0;i<sizeof(l);i++)
        retVal.push_back(tmp[i]);
    for (size_t i=0;i<l;i++)
        retVal.push_back(_value[i]);
    return(retVal);
}

unsigned int CInterfaceStackString::createFromData(const char* data)
{
    unsigned int l;
    char* tmp=(char*)(&l);
    for (size_t i=0;i<sizeof(l);i++)
        tmp[i]=data[i];
    for (size_t i=0;i<l;i++)
        _value.push_back(data[sizeof(l)+i]);
    return(sizeof(l)+l);
}

bool CInterfaceStackString::checkCreateFromData(const char* data,unsigned int& w,unsigned int l)
{
    unsigned int m;
    if (l<sizeof(m))
        return(false);
    char* tmp=(char*)(&m);
    for (size_t i=0;i<sizeof(m);i++)
        tmp[i]=data[i];
    if (l<sizeof(m)+m)
        return(false);
    w=sizeof(m)+m;
    return(true);
}
