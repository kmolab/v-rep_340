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

#include "sharedFloatVector.h"

std::vector<CSharedFloatVector_data*> CSharedFloatVector::_allData;

CSharedFloatVector::CSharedFloatVector()
{
    _data=new CSharedFloatVector_data();
    _data->increaseRefCounter();
    _allData.push_back(_data);
}

CSharedFloatVector::CSharedFloatVector(CSharedFloatVector* dataToShare)
{
    _data=dataToShare->_data;
    _data->increaseRefCounter();
}

CSharedFloatVector::CSharedFloatVector(CSharedFloatVector_data* dataToShare)
{
    _data=dataToShare;
    _data->increaseRefCounter();
}

CSharedFloatVector::~CSharedFloatVector()
{
    _data->decreaseRefCounter();
//  if (notGui)
        releaseUnusedData();
}

const std::vector<float>& CSharedFloatVector::getRead()
{
    return(_data->getData());
}

std::vector<float>& CSharedFloatVector::getWrite()
{
    if (_data->getRefCounter()==1)
        return(_data->getData()); // data not shared right now

    // The data is shared. We duplicate and disconnect it:
    CSharedFloatVector_data* newData=_data->copyYourself();
    _data->decreaseRefCounter();
    _data=newData;
    _data->increaseRefCounter();
    _allData.push_back(_data);
    return(_data->getData());
}

CSharedFloatVector* CSharedFloatVector::getDuplicate()
{
    CSharedFloatVector* retVal=new CSharedFloatVector(_data);
    return(retVal);
}

void CSharedFloatVector::replaceWithCopyOf(CSharedFloatVector* replacementToCopyFrom)
{
    _data->decreaseRefCounter();
//  if (notGui)
        releaseUnusedData();
    _data=replacementToCopyFrom->_data;
    _data->increaseRefCounter();
}

int CSharedFloatVector::releaseUnusedData()
{
    size_t i=0;
    while (i<_allData.size())
    {
        if (_allData[i]->getRefCounter()==0)
            delete _allData[i];
        else
            i++;
    }
    return(int(_allData.size()));
}
