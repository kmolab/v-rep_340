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
#include "vArchive.h"

unsigned short VArchive::LOAD=0;
unsigned short VArchive::STORE=1;

VArchive::VArchive(VFile* file,unsigned short flag)
{
#ifdef SIM_WITHOUT_QT_AT_ALL
    _theFile=file;
    _loading=((flag&1)==0);
#else
    _theFile=file;
    _loading=((flag&1)==0);
    _theArchive=new QDataStream(file->getFile());
    // Following 2 important to be compatible with the files written with first V-REP versions:
    _theArchive->setFloatingPointPrecision(QDataStream::SinglePrecision);
    _theArchive->setByteOrder(QDataStream::LittleEndian);
#endif
}

VArchive::~VArchive()
{
#ifndef SIM_WITHOUT_QT_AT_ALL
    delete _theArchive;
#endif
}

void VArchive::writeString(const std::string& str)
{
    for (int i=0;i<int(str.length());i++)
        (*this) << str[i];
}

void VArchive::writeLine(const std::string& line)
{
    writeString(line);
    (*this) << char(13);
    (*this) << char(10);
}

bool VArchive::readSingleLine(unsigned int& actualPosition,std::string& line,bool doNotReplaceTabsWithOneSpace)
{
    unsigned int archiveLength=(unsigned int)_theFile->getLength();
    unsigned char oneByte;
    line="";
    while (actualPosition<archiveLength)
    {
        (*this) >> oneByte;
        actualPosition++;
        if (oneByte!=(unsigned char)13)
        {
            if (oneByte==(unsigned char)10)
                return(true);
            if ( (oneByte!=(unsigned char)9)||doNotReplaceTabsWithOneSpace )
                line.insert(line.end(),(char)oneByte);
            else
                line.insert(line.end(),' ');
        }
    }
    return(line.length()!=0);
}

bool VArchive::readMultiLine(unsigned int& actualPosition,std::string& line,bool doNotReplaceTabsWithOneSpace,const char* multilineSeparator)
{
    line="";
    while (true)
    {
        std::string l;
        bool stillReadMatter=readSingleLine(actualPosition,l,doNotReplaceTabsWithOneSpace);
        while ((l.length()!=0)&&(l[l.length()-1]==' '))
            l.erase(l.begin()+l.length()-1);
        if ((l.length()!=0)&&(l[l.length()-1]=='\\'))
        {
            l.erase(l.begin()+l.length()-1);
            line+=l+multilineSeparator;
            if (!stillReadMatter)
                return(false);
        }
        else
        {
            line+=l;
            return(stillReadMatter);
        }
    }
}

VFile* VArchive::getFile()
{
    return(_theFile);
}

bool VArchive::isStoring()
{
    return(!_loading);
}

bool VArchive::isLoading()
{
    return(_loading);
}

void VArchive::close()
{
}
