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

class CSharedIntVector_data; // forward declaration

class CSharedIntVector
{
public:

    CSharedIntVector(); // create new data
    CSharedIntVector(CSharedIntVector* dataToShare); // shares the data
    CSharedIntVector(CSharedIntVector_data* dataToShare); // shares the data
    virtual ~CSharedIntVector();

    const std::vector<int>& getRead(); // get the data for reading
    std::vector<int>& getWrite(); // get the data for writing
    CSharedIntVector* getDuplicate(); // returns a copy that shares the data
    void replaceWithCopyOf(CSharedIntVector* replacementToCopyFrom); // replaces the data with the provided data that will be shared

    static int releaseUnusedData();

private:
    CSharedIntVector_data* _data;

    static std::vector<CSharedIntVector_data*> _allData;
};

//---------------------------------------------------------
class CSharedIntVector_data
{
public:
    CSharedIntVector_data()
    {
        _refCnt=0;
    };

    CSharedIntVector_data(std::vector<int>* toCopyFrom)
    {
        _refCnt=0;
        _data.resize(toCopyFrom->size());
        std::copy(toCopyFrom->begin(),toCopyFrom->end(),_data.begin());
    };

    ~CSharedIntVector_data()
    {
    };

    CSharedIntVector_data* copyYourself()
    {
        CSharedIntVector_data* retVal=new CSharedIntVector_data(&_data);
        return(retVal);
    };

    std::vector<int>& getData() { return(_data); };

    int getRefCounter() { return(_refCnt); };

    void increaseRefCounter() { _refCnt++; };

    void decreaseRefCounter() { _refCnt--; };

private:
    int _refCnt;
    std::vector<int> _data;
};
//---------------------------------------------------------
