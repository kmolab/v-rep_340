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
#include "vFile.h"
#ifndef SIM_WITHOUT_QT_AT_ALL
#include <QDataStream>
#endif

class VArchive  
{
public:
    VArchive(VFile* file,unsigned short flag);
    virtual ~VArchive();

    void writeLine(const std::string& line); // Will add char(10) and char(13)
    void writeString(const std::string& str); // Will not add char(10) or char(13)
    bool readSingleLine(unsigned int& actualPosition,std::string& line,bool doNotReplaceTabsWithOneSpace);
    bool readMultiLine(unsigned int& actualPosition,std::string& line,bool doNotReplaceTabsWithOneSpace,const char* multilineSeparator);

    VFile* getFile();
    bool isStoring();
    bool isLoading();
    void close();

    inline VArchive& operator<< (const int& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->write((char*)&v,sizeof(v));
#else
        (*_theArchive) << v;
#endif
        return(*this);
    }

    inline VArchive& operator<< (const unsigned int& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->write((char*)&v,sizeof(v));
#else
        (*_theArchive) << v;
#endif
        return(*this);
    }

    inline VArchive& operator<< (const float& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->write((char*)&v,sizeof(v));
#else
        (*_theArchive) << v;
#endif
        return(*this);
    }

    VArchive& operator<< (const double& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->write((char*)&v,sizeof(v));
#else
        // Very special here (because of the strange thing with _theArchive->setFloatingPointPrecision(QDataStream::SinglePrecision) )
        (*_theArchive) << ((unsigned char*)&v)[0];
        (*_theArchive) << ((unsigned char*)&v)[1];
        (*_theArchive) << ((unsigned char*)&v)[2];
        (*_theArchive) << ((unsigned char*)&v)[3];
        (*_theArchive) << ((unsigned char*)&v)[4];
        (*_theArchive) << ((unsigned char*)&v)[5];
        (*_theArchive) << ((unsigned char*)&v)[6];
        (*_theArchive) << ((unsigned char*)&v)[7];
#endif
        return(*this);
    }

    inline VArchive& operator<< (const unsigned short& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->write((char*)&v,sizeof(v));
#else
        (*_theArchive) << v;
#endif
        return(*this);
    }

    inline VArchive& operator<< (const unsigned char& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->write((char*)&v,sizeof(v));
#else
        (*_theArchive) << quint8(v);
#endif
        return(*this);
    }

    inline VArchive& operator<< (const char& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->write(&v,sizeof(v));
#else
        (*_theArchive) << qint8(v);
#endif
        return(*this);
    }

    inline VArchive& operator<< (const signed char& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->write((char*)&v,sizeof(v));
#else
        (*_theArchive) << qint8(v);
#endif
        return(*this);
    }

    inline VArchive& operator>> (int& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->read((char*)&v,sizeof(v));
#else
        (*_theArchive) >> v;
#endif
        return(*this);
    }

    inline VArchive& operator>> (unsigned int& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->read((char*)&v,sizeof(v));
#else
        (*_theArchive) >> v;
#endif
        return(*this);
    }

    inline VArchive& operator>> (float& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->read((char*)&v,sizeof(v));
#else
        (*_theArchive) >> v;
#endif
        return(*this);
    }

    VArchive& operator>> (double& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->read((char*)&v,sizeof(v));
#else
        // Very special here (because of the strange thing with _theArchive->setFloatingPointPrecision(QDataStream::SinglePrecision) )
        (*_theArchive) >> ((unsigned char*)&v)[0];
        (*_theArchive) >> ((unsigned char*)&v)[1];
        (*_theArchive) >> ((unsigned char*)&v)[2];
        (*_theArchive) >> ((unsigned char*)&v)[3];
        (*_theArchive) >> ((unsigned char*)&v)[4];
        (*_theArchive) >> ((unsigned char*)&v)[5];
        (*_theArchive) >> ((unsigned char*)&v)[6];
        (*_theArchive) >> ((unsigned char*)&v)[7];
#endif
        return(*this);
    }

    inline VArchive& operator>> (unsigned short& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->read((char*)&v,sizeof(v));
#else
        (*_theArchive) >> v;
#endif
        return(*this);
    }

    inline VArchive& operator>> (unsigned char& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->read((char*)&v,sizeof(v));
#else
        (*_theArchive) >> v;
#endif
        return(*this);
    }

    inline VArchive& operator>> (signed char& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->read((char*)&v,sizeof(v));
#else
        (*_theArchive) >> v;
#endif
        return(*this);
    }

    inline VArchive& operator>> (char& v)
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        _theFile->getFile()->read(&v,sizeof(v));
#else
        (*_theArchive) >> ((qint8*)(&v))[0];
#endif
        return(*this);
    }

private:
#ifndef SIM_WITHOUT_QT_AT_ALL
    QDataStream* _theArchive;
#endif
    VFile* _theFile;
    bool _loading;

public:
    static unsigned short LOAD;
    static unsigned short STORE;
};
