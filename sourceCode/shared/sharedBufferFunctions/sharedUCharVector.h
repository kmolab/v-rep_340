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

class CSharedUCharVector_data; // forward declaration

class CSharedUCharVector
{
public:

    CSharedUCharVector(); // create new data
    CSharedUCharVector(CSharedUCharVector* dataToShare); // shares the data
    CSharedUCharVector(CSharedUCharVector_data* dataToShare); // shares the data
    virtual ~CSharedUCharVector();

    const std::vector<unsigned char>& getRead(); // get the data for reading
    std::vector<unsigned char>& getWrite(); // get the data for writing
    CSharedUCharVector* getDuplicate(); // returns a copy that shares the data
    void replaceWithCopyOf(CSharedUCharVector* replacementToCopyFrom); // replaces the data with the provided data that will be shared

    static int releaseUnusedData();

private:
    CSharedUCharVector_data* _data;

    static std::vector<CSharedUCharVector_data*> _allData;
};

//---------------------------------------------------------
class CSharedUCharVector_data
{
public:
    CSharedUCharVector_data()
    {
        _refCnt=0;
    };

    CSharedUCharVector_data(std::vector<unsigned char>* toCopyFrom)
    {
        _refCnt=0;
        _data.resize(toCopyFrom->size());
        std::copy(toCopyFrom->begin(),toCopyFrom->end(),_data.begin());
    };

    ~CSharedUCharVector_data()
    {
    };

    CSharedUCharVector_data* copyYourself()
    {
        CSharedUCharVector_data* retVal=new CSharedUCharVector_data(&_data);
        return(retVal);
    };

    std::vector<unsigned char>& getData() { return(_data); };

    int getRefCounter() { return(_refCnt); };

    void increaseRefCounter() { _refCnt++; };

    void decreaseRefCounter() { _refCnt--; };

private:
    int _refCnt;
    std::vector<unsigned char> _data;
};
//---------------------------------------------------------
