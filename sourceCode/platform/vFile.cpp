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

#include "vFile.h"
#include "app.h"
#include "v_repStrings.h"
#include "vVarious.h"
#ifdef SIM_WITHOUT_QT_AT_ALL
#include <sys/stat.h>
#include <sys/types.h>
#endif

unsigned short VFile::CREATE_WRITE      =1;
unsigned short VFile::SHARE_EXCLUSIVE   =2;
unsigned short VFile::READ              =4;
unsigned short VFile::SHARE_DENY_NONE   =8;

VFile::VFile(const std::string& filename,unsigned short flags,bool dontThrow)
{
    _pathAndFilename=filename;
#ifdef SIM_WITHOUT_QT_AT_ALL
    if (flags&CREATE_WRITE)
    { // Create the path directories if needed
        std::string f(VVarious::splitPath_path(filename));
        if (!doesFolderExist(f))
            createFolder(f);
    }

    std::ios_base::openmode openFlags=std::ios_base::binary;
    if (flags&CREATE_WRITE)
        openFlags|=std::ios_base::out|std::ios_base::trunc;
    if (flags&READ)
        openFlags|=std::ios_base::in;
    _theFile=new WFile(filename.c_str(),openFlags);
    std::exception dummyException;
    _fileLength=0;
    if (!_theFile->is_open())
    {
        if (!dontThrow)
            throw dummyException;
    }
    else
    {
        if (flags&READ)
        {
            _theFile->seekg(0,std::ios::beg);
            std::streampos b=_theFile->tellg();
            _theFile->seekg(0,std::ios::end);
            std::streampos e=_theFile->tellg();
            _theFile->seekg(0,std::ios::beg);
            _fileLength=(e-b);
        }
    }
#else
    _theFile=new QFile(QString::fromLocal8Bit(filename.c_str()));
    QFlags<QIODevice::OpenModeFlag> openFlags=0;
    if (flags&CREATE_WRITE)
        openFlags|=QIODevice::Truncate|QIODevice::WriteOnly;
    if (flags&READ)
        openFlags|=QIODevice::ReadOnly;

    // Create the path directories if needed (added on 13/6/2012 because of a bug report of Matthias F�ller):
    if (flags&CREATE_WRITE)
    {
        QFileInfo pathInfo(QString::fromLocal8Bit(filename.c_str()));
        QDir dir("");
        dir.mkpath(pathInfo.path());
    }

    std::exception dummyException;
    if (!_theFile->open(openFlags))
    {
        delete _theFile;
        _theFile=NULL;
        if (!dontThrow)
            throw dummyException;
    }
    else
    {
        if (flags&CREATE_WRITE)
            _theFile->resize(0);
    }
#endif
}

VFile::VFile(const std::string& filename)
{ // opens a Qt resource file
#ifndef SIM_WITHOUT_QT_AT_ALL
    _theFile=new QFile(QString::fromLocal8Bit(filename.c_str()));
    std::exception dummyException;
    if (!_theFile->exists())
    {
       throw dummyException;
    }
#endif
}

VFile::~VFile()
{
#ifdef SIM_WITHOUT_QT_AT_ALL
    if (_theFile!=NULL)
    {
        _theFile->close();
        delete _theFile;
    }
#else
    delete _theFile;
#endif
}

void VFile::reportAndHandleFileExceptionError(VFILE_EXCEPTION_TYPE e)
{
#ifdef SIM_WITH_GUI
    // stl file exceptions:
    App::uiThread->messageBox_critical(NULL,strTranslate(IDSN_FILE_EXCEPTION_ERROR),e.what(),VMESSAGEBOX_OKELI);
    // And #include <exception>
#endif
    printf("File exception error: %s\n",e.what());
}

void VFile::eraseFile(const std::string& filenameAndPath)
{
    try
    {
#ifdef SIM_WITHOUT_QT_AT_ALL
        std::remove(filenameAndPath.c_str());
#else
        QFile::remove(filenameAndPath.c_str());
#endif
    }
    catch(VFILE_EXCEPTION_TYPE e)
    {
        VFile::reportAndHandleFileExceptionError(e);
    }
}


bool VFile::doesFileExist(const std::string& filenameAndPath)
{
    return(_doesFileOrFolderExist(filenameAndPath,false));
}

bool VFile::doesFolderExist(const std::string& foldernameAndPath)
{
    return(_doesFileOrFolderExist(foldernameAndPath,true));
}

bool VFile::_doesFileOrFolderExist(const std::string& filenameOrFoldernameAndPath,bool checkForFolder)
{
#ifdef SIM_WITHOUT_QT_AT_ALL
    struct stat info;
    if (stat(filenameOrFoldernameAndPath.c_str(),&info)!=0)
        return(false); // actually means this cannot be accessed
    else
    {
        if (info.st_mode&S_IFDIR)
            return(checkForFolder);
        else
            return(!checkForFolder);
    }
    return(false);
#else
    QString dat(QString::fromLocal8Bit(filenameOrFoldernameAndPath.c_str()));
    if (checkForFolder)
    {
        QDir dir(dat);
        return(dir.exists());       
    }
    else
    {
        QFile file(dat);
        return(file.exists());      
    }
#endif
}

bool VFile::createFolder(const std::string& pathAndName)
{
#ifdef SIM_WITHOUT_QT_AT_ALL
#ifdef WIN_VREP
    return(CreateDirectoryA(pathAndName.c_str(),NULL)!=0);
#else // WIN_VREP
    std::string tmp("mkdir -p ");
    tmp+=pathAndName;
    system(tmp.c_str());
    return(true);
#endif // WIN_VREP
#else // SIM_WITHOUT_QT_AT_ALL
    QDir qdir("");
    return(qdir.mkdir(QString::fromLocal8Bit(pathAndName.c_str())));
#endif // SIM_WITHOUT_QT_AT_ALL
}

quint64 VFile::getLength()
{
#ifdef SIM_WITHOUT_QT_AT_ALL
    return(_fileLength);
#else
    return(_theFile->size());
#endif
}

void VFile::close()
{
    _theFile->close();
}

WFile* VFile::getFile()
{
    return(_theFile);
}

std::string VFile::getPathAndFilename()
{
    return(_pathAndFilename);
}


bool VFile::flush()
{
#ifdef SIM_WITHOUT_QT_AT_ALL
    _theFile->flush();
    return(true);
#else
    return(_theFile->flush());
#endif
}

