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

#include "uiThread.h"
#include "simThread.h"
#include "directoryPaths.h"
#include "userSettings.h"
#include "vMutex.h"
#include "mainContainer.h"
#ifndef SIM_WITHOUT_QT_AT_ALL
#include "vrepQApp.h"
#include "simAndUiThreadSync.h"
#endif
#ifdef SIM_WITH_GUI
#include "mainWindow.h"
#endif

class App
{
public:
    App(bool headless);
    virtual ~App();

    bool wasInitSuccessful();
    static void setBrowserEnabled(bool e);
    static bool getBrowserEnabled();

    static void beep(int frequ=5000,int duration=1000);
    static void setApplicationName(const char* name);
    static std::string getApplicationName();
    static void createMainContainer();
    static void deleteMainContainer();

    static void run(void(*initCallBack)(),void(*loopCallBack)(),void(*deinitCallBack)());
    static void postExitRequest();
    static bool getExitRequest();
    static bool isSimulatorRunning();

    static void setQuitLevel(int l);
    static int getQuitLevel();

    static std::string getApplicationArgument(int index);
    static void setApplicationArgument(int index,std::string arg);

    static bool executeUiThreadCommand(SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut);
    static void appendSimulationThreadCommand(int cmdId,int intP1=-1,int intP2=-1,float floatP1=0.0,float floatP2=0.0,const char* stringP1=NULL,const char* stringP2=NULL,int executionDelay=0);
    static void appendSimulationThreadCommand(SSimulationThreadCommand cmd,int executionDelay=0);

    static int getEditModeType(); // helper
    static void setRebuildHierarchyFlag(); // helper
    static void setResetHierarchyViewFlag(); // helper
    static void setRefreshHierarchyViewFlag(); // helper
    static void setLightDialogRefreshFlag(); // helper
    static void setFullDialogRefreshFlag(); // helper
    static void setDialogRefreshDontPublishFlag(); // helper
    static void setToolbarRefreshFlag(); // helper
    static int getMouseMode(); // helper
    static void setMouseMode(int mm); // helper
    static void setDefaultMouseMode(); // helper
    static bool isFullScreen(); // helper
    static void setFullScreen(bool f); // helper

    static void addStatusbarMessage(const std::string& txt);

    static float* getRGBPointerFromItem(int objType,int objID1,int objID2,int colComponent,std::string* auxDlgTitle);
    static CVisualParam* getVisualParamPointerFromItem(int objType,int objID1,int objID2,std::string* auxDlgTitle,int* allowedParts);
    static CTextureProperty* getTexturePropertyPointerFromItem(int objType,int objID1,int objID2,std::string* auxDlgTitle,bool* is3D,bool* valid,CGeometric** geom);

    static CDirectoryPaths* directories;
    static CUserSettings* userSettings;
    static CMainContainer* ct;
    static CUiThread* uiThread;
    static CSimThread* simThread;

    static int operationalUIParts;
    static int sc;

private:
    bool _initSuccessful;
    static bool _browserEnabled;

    static void _runInitializationCallback(void(*initCallBack)());
    static void _runDeinitializationCallback(void(*deinitCallBack)());
    static void _processGuiEventsUntilQuit();

//  static VTHREAD_ID_TYPE _guiThread;
    static bool _exitRequest;
    static bool _simulatorIsRunning;
    static std::string _applicationName;
    static std::vector<std::string> _applicationArguments;
    static volatile int _quitLevel;

#ifndef SIM_WITHOUT_QT_AT_ALL
public:
    static CVrepQApp* qtApp;

private:
    static int _qApp_argc;
    static char _qApp_arg0[];
    static char* _qApp_argv[1];
#endif

#ifdef SIM_WITH_GUI
public:
    static void showSplashScreen();
    static void setIcon();
    static CMainWindow* mainWindow;
    static void createMainWindow();
    static void deleteMainWindow();
    static void setShowConsole(bool s);
#endif
};
