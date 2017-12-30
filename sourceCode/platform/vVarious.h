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
#ifndef SIM_WITHOUT_QT_AT_ALL
#include <QLibrary>
typedef QLibrary* WLibrary;
typedef void* WLibraryFunc;
#else
#ifdef WIN_VREP
#include <Windows.h>
typedef HINSTANCE WLibrary;
typedef FARPROC WLibraryFunc;
#else
typedef void* WLibrary;
typedef void* WLibraryFunc;
#endif
#endif

#define VVARIOUS_SHOWNORMAL 0
#define VVARIOUS_SHOWMAXIMIZED 1
#define VVARIOUS_HIDE 2

// FULLY STATIC CLASS
class VVarious  
{
public:
    static bool executeExternalApplication(const std::string& file,const std::string& arguments,const std::string& switchToDirectory,int showFlag);

    static WLibrary openLibrary(const char* filename);
    static void closeLibrary(WLibrary lib);
    static WLibraryFunc resolveLibraryFuncName(WLibrary lib,const char* funcName);


    static std::string getModulePath();
    static void removePathFinalSlashOrBackslash(std::string& pathWithOrWithoutFinalThing);

    static std::string splitPath_path(const std::string& fullPathAndName);
    static std::string splitPath_fileBase(const std::string& fullPathAndName);
    static std::string splitPath_fileExtension(const std::string& fullPathAndName);
    static std::string splitPath_fileBaseAndExtension(const std::string& fullPathAndName);
    static bool isAbsolutePath(const std::string& pathAndOptionalFilename);
    static bool copyTextToClipboard(const std::string& text);

#ifdef SIM_WITH_GUI
public:
    static bool openUrl(const std::string& url);
    static bool openTextFile(const std::string& file);
#endif
};
