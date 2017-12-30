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

#include "interfaceStackObject.h"
#include <vector>

class CInterfaceStackTable : public CInterfaceStackObject
{
public:
    CInterfaceStackTable();
    virtual ~CInterfaceStackTable();

    CInterfaceStackObject* copyYourself();
    void printContent(int spaces) const;
    std::string getObjectData() const;
    unsigned int createFromData(const char* data);
    static bool checkCreateFromData(const char* data,unsigned int& w,unsigned int l);

    bool isTableArray();
    int getArraySize();
    int getMapEntryCount();

    CInterfaceStackObject* getArrayItemAtIndex(int ind);
    CInterfaceStackObject* getMapItemAtIndex(int ind,std::string& stringKey,double& numberKey,bool& isStringKey);
    void getAllObjectsAndClearTable(std::vector<CInterfaceStackObject*>& allObjs);

    void setUCharArray(const unsigned char* array,int l);
    void setIntArray(const int* array,int l);
    void setFloatArray(const float* array,int l);
    void setDoubleArray(const double* array,int l);

    void appendArrayObject(CInterfaceStackObject* obj);
    void appendMapObject(CInterfaceStackObject* obj,const char* key);
    void appendMapObject(CInterfaceStackObject* obj,double key);
    void appendArrayOrMapObject(CInterfaceStackObject* obj,CInterfaceStackObject* key);

    bool getUCharArray(unsigned char* array,int count);
    bool getIntArray(int* array,int count);
    bool getFloatArray(float* array,int count);
    bool getDoubleArray(double* array,int count);

    int getTableInfo(int infoType);

protected:
    bool _areAllValueThis(int what);

    std::vector<CInterfaceStackObject*> _tableObjects;
    bool _isTableArray;
};
