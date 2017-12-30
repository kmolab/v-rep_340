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

#include "v_repConst.h"
#include "interfaceStackNull.h"
#include "interfaceStackBool.h"
#include "interfaceStackNumber.h"
#include "interfaceStackString.h"
#include "interfaceStackTable.h"

CInterfaceStackTable::CInterfaceStackTable()
{
    _objectType=STACK_OBJECT_TABLE;
    _isTableArray=true;
}

CInterfaceStackTable::~CInterfaceStackTable()
{
    for (size_t i=0;i<_tableObjects.size();i++)
        delete _tableObjects[i];
}

bool CInterfaceStackTable::isTableArray()
{
    return(_isTableArray);
}

int CInterfaceStackTable::getArraySize()
{
    if (!_isTableArray)
        return(0);
    return(_tableObjects.size());
}

int CInterfaceStackTable::getMapEntryCount()
{
    if (_isTableArray)
        return(0);
    return(_tableObjects.size()/2);
}


bool CInterfaceStackTable::getUCharArray(unsigned char* array,int count)
{
    if (!_isTableArray)
        return(false);
    bool retVal=true;
    size_t c=(size_t)count;
    if (c>_tableObjects.size())
        c=_tableObjects.size();
    for (size_t i=0;i<c;i++)
    {
        int t=_tableObjects[i]->getObjectType();
        if (t==STACK_OBJECT_NUMBER)
            array[i]=(unsigned char)((CInterfaceStackNumber*)_tableObjects[i])->getValue();
        else
        {
            array[i]=0;
            retVal=false;
        }
    }
    for (size_t i=c;i<(size_t)count;i++)
        array[i]=0; // fill with zeros
    return(retVal);
}

bool CInterfaceStackTable::getIntArray(int* array,int count)
{
    if (!_isTableArray)
        return(false);
    bool retVal=true;
    size_t c=(size_t)count;
    if (c>_tableObjects.size())
        c=_tableObjects.size();
    for (size_t i=0;i<c;i++)
    {
        int t=_tableObjects[i]->getObjectType();
        if (t==STACK_OBJECT_NUMBER)
            array[i]=(int)((CInterfaceStackNumber*)_tableObjects[i])->getValue();
        else
        {
            array[i]=0;
            retVal=false;
        }
    }
    for (size_t i=c;i<(size_t)count;i++)
        array[i]=0; // fill with zeros
    return(retVal);
}

bool CInterfaceStackTable::getFloatArray(float* array,int count)
{
    if (!_isTableArray)
        return(false);
    bool retVal=true;
    size_t c=(size_t)count;
    if (c>_tableObjects.size())
        c=_tableObjects.size();
    for (size_t i=0;i<c;i++)
    {
        int t=_tableObjects[i]->getObjectType();
        if (t==STACK_OBJECT_NUMBER)
            array[i]=(float)((CInterfaceStackNumber*)_tableObjects[i])->getValue();
        else
        {
            array[i]=0.0;
            retVal=false;
        }
    }
    for (size_t i=c;i<(size_t)count;i++)
        array[i]=0.0; // fill with zeros
    return(retVal);
}

bool CInterfaceStackTable::getDoubleArray(double* array,int count)
{
    if (!_isTableArray)
        return(false);
    bool retVal=true;
    size_t c=(size_t)count;
    if (c>_tableObjects.size())
        c=_tableObjects.size();
    for (size_t i=0;i<c;i++)
    {
        int t=_tableObjects[i]->getObjectType();
        if (t==STACK_OBJECT_NUMBER)
            array[i]=((CInterfaceStackNumber*)_tableObjects[i])->getValue();
        else
        {
            array[i]=0.0;
            retVal=false;
        }
    }
    for (size_t i=c;i<(size_t)count;i++)
        array[i]=0.0; // fill with zeros
    return(retVal);
}

void CInterfaceStackTable::appendArrayObject(CInterfaceStackObject* obj)
{
    _tableObjects.push_back(obj);
}

void CInterfaceStackTable::appendMapObject(CInterfaceStackObject* obj,const char* key)
{
    _isTableArray=false;
    _tableObjects.push_back(new CInterfaceStackString(key,0));
    _tableObjects.push_back(obj);
}

void CInterfaceStackTable::appendMapObject(CInterfaceStackObject* obj,double key)
{
    _isTableArray=false;
    _tableObjects.push_back(new CInterfaceStackNumber(key));
    _tableObjects.push_back(obj);
}

void CInterfaceStackTable::appendArrayOrMapObject(CInterfaceStackObject* obj,CInterfaceStackObject* key)
{   // here we basically treat this table as an array, until the key is:
    // 1) not a number, 2) not consecutive, 3) does not start at 1.
    // In that case, we then convert that table from array to map representation
    bool valueInserted=false;
    if (_isTableArray)
    {
        if (key->getObjectType()==STACK_OBJECT_NUMBER)
        {
            int ind=(int)((CInterfaceStackNumber*)key)->getValue();
            if (int(_tableObjects.size())+1==ind)
            {
                _tableObjects.push_back(obj);
                valueInserted=true;
            }
        }
    }
    if (!valueInserted)
    {
        if (_isTableArray)
        { // we need to convert the table to a map representation:
            int l=_tableObjects.size();
            for (int i=0;i<l;i++)
                _tableObjects.insert(_tableObjects.begin()+2*i,new CInterfaceStackNumber((double)(i+1)));
        }
        _isTableArray=false;
        _tableObjects.push_back(key);
        _tableObjects.push_back(obj);
    }
}

CInterfaceStackObject* CInterfaceStackTable::getArrayItemAtIndex(int ind)
{
    if ( (!_isTableArray)||(ind>=(int)_tableObjects.size()) )
        return(NULL);
    return(_tableObjects[ind]);
}

CInterfaceStackObject* CInterfaceStackTable::getMapItemAtIndex(int ind,std::string& stringKey,double& numberKey,bool& isStringKey)
{
    if ( (_isTableArray)||(ind>=(int)_tableObjects.size()/2) )
        return(NULL);
    if (_tableObjects[2*ind+0]->getObjectType()==STACK_OBJECT_NUMBER)
    {
        isStringKey=false;
        CInterfaceStackNumber* keyObj=(CInterfaceStackNumber*)_tableObjects[2*ind+0];
        numberKey=keyObj->getValue();
    }
    else
    {
        isStringKey=true;
        CInterfaceStackString* keyObj=(CInterfaceStackString*)_tableObjects[2*ind+0];
        stringKey=keyObj->getValue(NULL);
    }
    return(_tableObjects[2*ind+1]);
}

CInterfaceStackObject* CInterfaceStackTable::copyYourself()
{
    CInterfaceStackTable* retVal=new CInterfaceStackTable();
    for (size_t i=0;i<_tableObjects.size();i++)
        retVal->_tableObjects.push_back(_tableObjects[i]->copyYourself());
    retVal->_isTableArray=_isTableArray;
    return(retVal);
}

void CInterfaceStackTable::getAllObjectsAndClearTable(std::vector<CInterfaceStackObject*>& allObjs)
{
    allObjs.clear();
    allObjs.assign(_tableObjects.begin(),_tableObjects.end());
    _tableObjects.clear();
    _isTableArray=true;
}

void CInterfaceStackTable::setUCharArray(const unsigned char* array,int l)
{
    _tableObjects.clear();
    _isTableArray=true;
    for (int i=0;i<l;i++)
        _tableObjects.push_back(new CInterfaceStackNumber((double)array[i]));
}

void CInterfaceStackTable::setIntArray(const int* array,int l)
{
    _tableObjects.clear();
    _isTableArray=true;
    for (int i=0;i<l;i++)
        _tableObjects.push_back(new CInterfaceStackNumber((double)array[i]));
}

void CInterfaceStackTable::setFloatArray(const float* array,int l)
{
    _tableObjects.clear();
    _isTableArray=true;
    for (int i=0;i<l;i++)
        _tableObjects.push_back(new CInterfaceStackNumber((float)array[i]));
}

void CInterfaceStackTable::setDoubleArray(const double* array,int l)
{
    _tableObjects.clear();
    _isTableArray=true;
    for (int i=0;i<l;i++)
        _tableObjects.push_back(new CInterfaceStackNumber(array[i]));
}

int CInterfaceStackTable::getTableInfo(int infoType)
{
    if (infoType==0)
    { // array size or table type (array/map)
        if (_isTableArray)
            return(getArraySize());
        return(sim_stack_table_map);
    }
    int retVal=0;
    if ( (infoType==1)&&_areAllValueThis(STACK_OBJECT_NULL) )
        retVal=1;
    if ( (infoType==2)&&_areAllValueThis(STACK_OBJECT_NUMBER) )
        retVal=1;
    if ( (infoType==3)&&_areAllValueThis(STACK_OBJECT_BOOL) )
        retVal=1;
    if ( (infoType==4)&&_areAllValueThis(STACK_OBJECT_STRING) )
        retVal=1;
    if ( (infoType==5)&&_areAllValueThis(STACK_OBJECT_TABLE) )
        retVal=1;
    return(retVal);
}

bool CInterfaceStackTable::_areAllValueThis(int what)
{
    if (_tableObjects.size()==0)
        return(true);
    if (_isTableArray)
    {
        for (size_t i=0;i<_tableObjects.size();i++)
        {
            if (_tableObjects[i]->getObjectType()!=what)
                return(false);
        }
    }
    else
    {
        for (size_t i=0;i<_tableObjects.size()/2;i++)
        {
            if (_tableObjects[2*i+1]->getObjectType()!=what)
                return(false);
        }
    }
    return(true);
}

void CInterfaceStackTable::printContent(int spaces) const
{
    for (int i=0;i<spaces;i++)
        printf(" ");
    if (_tableObjects.size()==0)
        printf("TABLE: <empty>\n");
    else
    {
        if (_isTableArray)
        {
            printf("ARRAY TABLE (%i items, keys are omitted):\n",_tableObjects.size()*2);
            for (size_t i=0;i<_tableObjects.size();i++)
                _tableObjects[i]->printContent(spaces+4);
        }
        else
        {
            printf("MAP TABLE (%i items, key and value):\n",_tableObjects.size());
            for (size_t i=0;i<_tableObjects.size();i++)
                _tableObjects[i]->printContent(spaces+4);
        }
    }
}

std::string CInterfaceStackTable::getObjectData() const
{
    std::string retVal;

    if (_isTableArray)
        retVal=char(1);
    else
        retVal=char(0);
    unsigned int l=_tableObjects.size();
    char* tmp=(char*)(&l);
    for (size_t i=0;i<sizeof(l);i++)
        retVal.push_back(tmp[i]);
    for (size_t i=0;i<_tableObjects.size();i++)
    {
        retVal.push_back((char)_tableObjects[i]->getObjectType());
        retVal+=_tableObjects[i]->getObjectData();
    }
    return(retVal);
}

unsigned int CInterfaceStackTable::createFromData(const char* data)
{
    unsigned int retVal=0;
    _isTableArray=(data[retVal]!=0);
    retVal++;
    unsigned int l;
    char* tmp=(char*)(&l);
    for (size_t i=0;i<sizeof(l);i++)
        tmp[i]=data[retVal+i];
    retVal+=sizeof(l);
    for (size_t i=0;i<l;i++)
    {
        unsigned int r=0;
        CInterfaceStackObject* obj=CInterfaceStackObject::createFromDataStatic(data+retVal,r);
        _tableObjects.push_back(obj);
        retVal+=r;
    }
    return(retVal);
}

bool CInterfaceStackTable::checkCreateFromData(const char* data,unsigned int& w,unsigned int l)
{
    unsigned int m;
    if (l<sizeof(m)+1)
        return(false);
    char* tmp=(char*)(&m);
    for (size_t i=0;i<sizeof(m);i++)
        tmp[i]=data[1+i];
    w=1+sizeof(m);
    for (size_t i=0;i<m;i++)
    {
        if (l<w+1)
            return(false);
        unsigned char t=data[w++];
        bool res=false;
        unsigned int v=0;
        if (t==STACK_OBJECT_NULL)
            res=CInterfaceStackNull::checkCreateFromData(data+w,v,l-w);
        if (t==STACK_OBJECT_NUMBER)
            res=CInterfaceStackNumber::checkCreateFromData(data+w,v,l-w);
        if (t==STACK_OBJECT_BOOL)
            res=CInterfaceStackBool::checkCreateFromData(data+w,v,l-w);
        if (t==STACK_OBJECT_STRING)
            res=CInterfaceStackString::checkCreateFromData(data+w,v,l-w);
        if (t==STACK_OBJECT_TABLE)
            res=CInterfaceStackTable::checkCreateFromData(data+w,v,l-w);
        if (!res)
            return(false);
        w+=v;
    }
    return(true);
}
