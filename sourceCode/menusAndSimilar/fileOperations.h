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
#ifdef SIM_WITH_GUI
#include "vMenubar.h"
#endif

struct SSimulationThreadCommand;

//FULLY STATIC CLASS
class CFileOperations  
{
public:
    static bool processCommand(int commandID);
    static bool processCommand(const SSimulationThreadCommand& cmd);
    static bool loadScene(const char* pathAndFilename,bool displayMessages,bool displayDialogs,bool setCurrentDir);
    static bool loadModel(const char* pathAndFilename,bool displayMessages,bool displayDialogs,bool setCurrentDir,bool noScreenRefresh,std::string* acknowledgmentPointerInReturn,bool doUndoThingInHere);
    static bool saveScene(const char* pathAndFilename,bool displayMessages,bool displayDialogs,bool setCurrentDir);
    static bool saveModel(int modelBaseDummyID,const char* pathAndFilename,bool displayMessages,bool displayDialogs,bool setCurrentDir);

    static bool saveUserInterfaces(const char* pathAndFilename,bool displayMessages,bool displayDialogs,bool setCurrentDir,std::vector<int>* uiHandlesOrNullForAll);
    static bool loadUserInterfaces(const char* pathAndFilename,bool displayMessages,bool displayDialogs,bool setCurrentDir,std::vector<int>* uiHandles,bool doUndoThingInHere);


    static bool commonImportRoutine(int importType,const std::string& pathName,bool showDlg,bool showProgress,float& sf,int options,bool useOrientSizeFromPreviousImport);
    static bool apiImportRoutine(int importType,const std::string& pathName,int options,float identicalVerticesTolerance,float sizeFactor,std::vector<std::vector<float>*>& listOfVertices,std::vector<std::vector<int>*>& listOfIndices,std::vector<std::string>& groupNames);
    static int apiAddHeightfieldToScene(int xSize,float pointSpacing,const std::vector<std::vector<float>*>& readData,float shadingAngle,int options);
    static void createNewScene(bool displayMessages,bool forceForNewInstance);
    static void closeScene(bool displayMessages,bool displayDialogs);
    static void addToRecentlyOpenedScenes(std::string filenameAndPath);
    static bool apiExportIkContent(const char* pathAndName,bool displayDialogs);

private:
    static void _removeFromRecentlyOpenedScenes(std::string filenameAndPath);
    static bool _pathImportRoutine(const std::string& pathName,bool displayDialogs);
    static bool _pathExportPoints(const std::string& pathName,int pathID,bool bezierPoints,bool displayDialogs);
    static bool heightfieldImportRoutine(const std::string& pathName);
    static std::string _getStringOfVersionAndLicenseThatTheFileWasWrittenWith(unsigned short vrepVer,int licenseType,char revision);

#ifdef SIM_WITH_GUI
public:
    static void keyPress(int key);
    static void addMenu(VMenu* menu);

private:
    static bool _saveSceneWithDialogAndEverything();
    static bool _saveSceneAsWithDialogAndEverything(bool vrepFormat);
#endif
};
