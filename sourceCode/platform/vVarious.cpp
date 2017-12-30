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
#include "vVarious.h"
#include "auxLib.h"
#include "tt.h"
#include "vFile.h"
#include "vArchive.h"
#include "ttUtil.h"
#include "vDateTime.h"
#ifdef WIN_VREP
#include <Windows.h>
#include <Shellapi.h>
#endif
#ifdef SIM_WITH_GUI
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#endif
#ifndef SIM_WITHOUT_QT_AT_ALL
#include <QUrl>
#include <QProcess>
#else // SIM_WITHOUT_QT_AT_ALL
#ifndef WIN_VREP
#include <dlfcn.h>
#include <stdlib.h>
#endif
#endif // SIM_WITHOUT_QT_AT_ALL


#ifdef LIN_VREP
    #include <unistd.h>
    #include <stdio.h>
    #include <sys/ioctl.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <net/if.h>
#endif

bool VVarious::executeExternalApplication(const std::string& file,const std::string& arguments,const std::string& switchToDirectory,int showFlag)
{
#ifdef WIN_VREP
    int sh=SW_SHOWDEFAULT;
    if (showFlag==VVARIOUS_SHOWMAXIMIZED)
        sh=SW_SHOWMAXIMIZED;
    if (showFlag==VVARIOUS_HIDE)
        sh=SW_HIDE;
    if (switchToDirectory.length()!=0)
        SetCurrentDirectoryA(switchToDirectory.c_str()); // needed because otherwise the shellExecute command might switch directories!
    return (int(ShellExecuteA(NULL,"open",file.c_str(),arguments.c_str(),NULL,sh))>32);
#else // WIN_VREP
    // Check here: http://stackoverflow.com/questions/859517/osx-equivalent-of-shellexecute
#ifdef SIM_WITHOUT_QT_AT_ALL
    std::string cmd(file);
    if (file.find('/')==std::string::npos)
    {
        cmd=switchToDirectory+'/';
        cmd+=file;
    }
    pid_t pid;
    std::vector<char*> argsp;
    std::vector<std::string> args;
    for (int i=0;i<10;i++)
        argsp.push_back(NULL);
    std::string w;
    std::string argu(arguments);
    while (tt::extractSpaceSeparatedWord2(argu,w,true,true,false,false,false))
    {
        args.push_back(w);
        argsp[args.size()-1]=(char*)args[args.size()-1].c_str();
    }
    pid=fork();
    if (pid==0)
    {
        execl(cmd.c_str(),cmd.c_str(),argsp[0],argsp[1],argsp[2],argsp[3],argsp[4],argsp[5],argsp[6],argsp[7],argsp[8],argsp[9],(char*)0);
        exit(0);
    }
    return(true);
#else
    QString cmd;
    if (file.find('/')==std::string::npos)
        cmd="./";
    cmd+=QString::fromLocal8Bit(file.c_str());
    QStringList strList;
    std::string args(arguments);
    std::string word;
    while (tt::extractSpaceSeparatedWord(args,word))
        strList << QString(word.c_str());
    return(QProcess::startDetached(cmd,strList,QString::fromLocal8Bit(switchToDirectory.c_str()),NULL));
#endif // SIM_WITHOUT_QT_AT_ALL
#endif // WIN_VREP
}


std::string VVarious::getModulePath()
{
    static std::string retVal;
    if (retVal.size()==0)
    {
        char curDirAndFile[2048];
#ifdef _WIN32
        GetModuleFileNameA(NULL,curDirAndFile,2000);
        int i=0;
        while (true)
        {
            if (curDirAndFile[i]==0)
                break;
            if (curDirAndFile[i]=='\\')
                curDirAndFile[i]='/';
            i++;
        }
        retVal=splitPath_path(curDirAndFile);
#else
        getcwd(curDirAndFile, 2000);
        retVal=curDirAndFile;
#endif
    }
    return(retVal);
}

void VVarious::removePathFinalSlashOrBackslash(std::string& pathWithOrWithoutFinalThing)
{
    int l=pathWithOrWithoutFinalThing.length();
    if (l>0)
    {
        if ((pathWithOrWithoutFinalThing[l-1]=='/')||(pathWithOrWithoutFinalThing[l-1]=='\\'))
        { // this might be a / or \\ part of a double char. Check for that:
            bool removeIt=true;
            if (l>1)
            {
                if ((unsigned char)(pathWithOrWithoutFinalThing[l-2])>0x7F)
                    removeIt=false;
            }

            if (removeIt)
                pathWithOrWithoutFinalThing.erase(pathWithOrWithoutFinalThing.end()-1);
        }
    }
}

std::string VVarious::splitPath_path(const std::string& fullPathAndName)
{   // returns the absolute path, without a final / or backslash
    std::string retVal;
#ifdef SIM_WITHOUT_QT_AT_ALL
    // TODO_SIM_WITHOUT_QT_AT_ALL
    // This routine should be rewritten with OS-specific mechanisms
    retVal=fullPathAndName;
    while ( (retVal.size()>0)&&(retVal[retVal.size()-1]!='/')&&(retVal[retVal.size()-1]!='\\') )
        retVal.erase(retVal.end()-1);
    if (retVal.size()>0)
        retVal.erase(retVal.end()-1);
#else
    QFileInfo pathInfo(QString::fromLocal8Bit(fullPathAndName.c_str()));
    retVal=pathInfo.path().toLocal8Bit().data();
    int l=retVal.length();
    if (l!=0)
    {
        if ((retVal[l-1]=='/')||(retVal[l-1]=='\\'))
        { // this might be a / or \\ part of a double char. Check for that:
            bool removeIt=true;
            if (l>1)
            {
                if ((unsigned char)(retVal[l-2])>0x7F)
                    removeIt=false;
            }

            if (removeIt)
                retVal.erase(retVal.end()-1);
        }
    }
#endif
    return(retVal);
}

std::string VVarious::splitPath_fileBaseAndExtension(const std::string& fullPathAndName)
{ // returns the filename including extension
    std::string retVal;
#ifdef SIM_WITHOUT_QT_AT_ALL
    // TODO_SIM_WITHOUT_QT_AT_ALL
    // This routine should be rewritten with OS-specific mechanisms
    std::string tmp(fullPathAndName);
    while ( (tmp.size()>0)&&(tmp[tmp.size()-1]!='/')&&(tmp[tmp.size()-1]!='\\')&&(tmp[tmp.size()-1]!=':') )
    {
        retVal=tmp[tmp.size()-1]+retVal;
        tmp.erase(tmp.end()-1);
    }
#else
    QFileInfo pathInfo(QString::fromLocal8Bit(fullPathAndName.c_str()));
    retVal=pathInfo.fileName().toLocal8Bit().data();
#endif
    return(retVal);
}


std::string VVarious::splitPath_fileBase(const std::string& fullPathAndName)
{   // returns the base of a filename, without path or extension
    std::string retVal;
#ifdef SIM_WITHOUT_QT_AT_ALL
    // TODO_SIM_WITHOUT_QT_AT_ALL
    // This routine should be rewritten with OS-specific mechanisms
    retVal=splitPath_fileBaseAndExtension(fullPathAndName);
    std::string tmp(retVal);
    while (tmp.size()>0)
    {
        if (tmp[tmp.size()-1]=='.')
        {
            tmp.erase(tmp.end()-1);
            retVal=tmp;
            break;
        }
        tmp.erase(tmp.end()-1);
    }
#else
    QFileInfo pathInfo(QString::fromLocal8Bit(fullPathAndName.c_str()));
    retVal=pathInfo.completeBaseName().toLocal8Bit().data();
#endif
    return(retVal);
}

std::string VVarious::splitPath_fileExtension(const std::string& fullPathAndName)
{   // returns the filename extension (without '.')
    std::string retVal;
#ifdef SIM_WITHOUT_QT_AT_ALL
    // TODO_SIM_WITHOUT_QT_AT_ALL
    // This routine should be rewritten with OS-specific mechanisms
    std::string tmp(fullPathAndName);
    bool hadDot=false;
    while ( (tmp.size()>0)&&(tmp[tmp.size()-1]!='/')&&(tmp[tmp.size()-1]!='\\')&&(tmp[tmp.size()-1]!=':') )
    {
        if (tmp[tmp.size()-1]=='.')
        {
            hadDot=true;
            break;
        }
        retVal=tmp[tmp.size()-1]+retVal;
        tmp.erase(tmp.end()-1);
    }
    if (!hadDot)
        retVal.clear();
#else
    QFileInfo pathInfo(QString::fromLocal8Bit(fullPathAndName.c_str()));
    retVal=pathInfo.suffix().toLocal8Bit().data();
#endif
    return(retVal);
}

bool VVarious::isAbsolutePath(const std::string& pathAndOptionalFilename)
{
#ifdef SIM_WITHOUT_QT_AT_ALL
    // TODO_SIM_WITHOUT_QT_AT_ALL
    // This routine should be rewritten with OS-specific mechanisms
#ifdef WIN_VREP
    if (pathAndOptionalFilename.size()==0)
        return(false);
    if (pathAndOptionalFilename[0]=='\\')
        return(true);
    if (pathAndOptionalFilename.size()<2)
        return(false);
    if (pathAndOptionalFilename[1]==':')
        return(true);
    return(false);
#else // WIN_VREP
    if (pathAndOptionalFilename.size()==0)
        return(false);
    return(pathAndOptionalFilename[0]=='/');
#endif // WIN_VREP
#else // SIM_WITHOUT_QT_AT_ALL
    QFileInfo pathInfo(QString::fromLocal8Bit(pathAndOptionalFilename.c_str()));
    return(pathInfo.isAbsolute());
#endif // SIM_WITHOUT_QT_AT_ALL
}


WLibrary VVarious::openLibrary(const char* filename)
{ // here we have the extension in the filename (.dll, .so or .dylib)
#ifdef SIM_WITHOUT_QT_AT_ALL
#ifdef WIN_VREP
    return LoadLibraryA(filename);
#else
    return dlopen(filename,RTLD_LAZY);
#endif
#else // SIM_WITHOUT_QT_AT_ALL
    WLibrary lib=new QLibrary(filename);
    if (!lib->load())
    {
        delete lib;
        lib=NULL;
    }
    return(lib);
#endif // SIM_WITHOUT_QT_AT_ALL
}

void VVarious::closeLibrary(WLibrary lib)
{
#ifdef SIM_WITHOUT_QT_AT_ALL
#ifdef WIN_VREP
    if (lib!=0)
        FreeLibrary(lib);
#else
        dlclose(lib);
#endif
#else // SIM_WITHOUT_QT_AT_ALL
    if (lib!=NULL)
    {
        lib->unload();
        delete lib;
    }
#endif // SIM_WITHOUT_QT_AT_ALL
}

WLibraryFunc VVarious::resolveLibraryFuncName(WLibrary lib,const char* funcName)
{
#ifdef SIM_WITHOUT_QT_AT_ALL
#ifdef WIN_VREP
    if (lib!=NULL)
        return GetProcAddress(lib,funcName);
#else
    if (lib!=NULL)
        return dlsym(lib,funcName);
#endif
#else // SIM_WITHOUT_QT_AT_ALL
    if (lib!=NULL)
        return((void*)lib->resolve(funcName));
#endif // SIM_WITHOUT_QT_AT_ALL
    return(NULL);
}

bool VVarious::copyTextToClipboard(const std::string& text)
{
#ifdef SIM_WITH_GUI
    QClipboard *clipboard=QApplication::clipboard();
    clipboard->setText(text.c_str());
#endif
    return(true);
}

#ifdef SIM_WITH_GUI
bool VVarious::openUrl(const std::string& url)
{
    std::string f(url);
    f="file:///"+f;
    return(QDesktopServices::openUrl(QUrl(QString::fromLocal8Bit(f.c_str()),QUrl::TolerantMode)));
}

bool VVarious::openTextFile(const std::string& file)
{
    std::string f(file);
    f="file:///"+f;
    return(QDesktopServices::openUrl(QUrl(QString::fromLocal8Bit(f.c_str()),QUrl::TolerantMode)));
}
#endif
