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
#include "funcDebug.h"
#include "app.h"
#include "vThread.h"
#include "ttUtil.h"
#include "pluginContainer.h"
#include "v_repStrings.h"
#include "vDateTime.h"
#include "pathPlanningInterface.h"
#include "vVarious.h"
#include "tt.h"
#include "persistentDataContainer.h"
#include "apiErrors.h"
#include "luaWrapper.h"
#include "geometric.h"
#include "rendering.h"
#ifdef WIN_VREP
    #include "auxLib.h"
#endif
#ifdef SIM_WITH_GUI
    #include "auxLibVideo.h"
    #include "vMessageBox.h"
    #include <QSplashScreen>
    #include <QBitmap>
#ifdef WIN_VREP
    #include <QStyleFactory>
#endif
#endif

void (*_workThreadLoopCallback)();

//VTHREAD_ID_TYPE App::_guiThread=0;
CUiThread* App::uiThread=NULL;
CSimThread* App::simThread=NULL;
CUserSettings* App::userSettings=NULL;
CDirectoryPaths* App::directories=NULL;
int App::operationalUIParts=0; // sim_gui_menubar,sim_gui_popupmenus,sim_gui_toolbar1,sim_gui_toolbar2, etc.
std::string App::_applicationName="V-REP (Customized)";
CMainContainer* App::ct=NULL;
bool App::_exitRequest=false;
bool App::_browserEnabled=true;

bool App::_simulatorIsRunning=false;
std::vector<std::string> App::_applicationArguments;
volatile int App::_quitLevel=0;

int App::sc=1;
#ifndef SIM_WITHOUT_QT_AT_ALL
CVrepQApp* App::qtApp=NULL;
int App::_qApp_argc=1;
char App::_qApp_arg0[]={"V-REP"};
char* App::_qApp_argv[1]={_qApp_arg0};
#endif
#ifdef SIM_WITH_GUI
    CMainWindow* App::mainWindow=NULL;
#endif


#ifdef SIM_WITHOUT_QT_AT_ALL
VTHREAD_RETURN_TYPE _workThread(VTHREAD_ARGUMENT_TYPE lpData)
#else
SIMPLE_VTHREAD_RETURN_TYPE _workThread(SIMPLE_VTHREAD_ARGUMENT_TYPE lpData)
#endif
{
    FUNCTION_DEBUG;
    VThread::setSimulationMainThreadId();
    CApiErrors::addNewThreadForErrorReporting(1);
    srand(VDateTime::getTimeInMs());    // Important so that the computer ID has some "true" random component!
                                        // Remember that each thread starts with a same seed!!!
    App::simThread=new CSimThread();
#ifndef SIM_WITHOUT_QT_AT_ALL
    CSimAndUiThreadSync::simThread_forbidUiThreadToWrite(true); // lock initially...
#endif
    while (!App::getExitRequest())
    {
        // Send the "instancePass" message to all plugins:
        int auxData[4]={App::ct->getModificationFlags(true),0,0,0};
        void* replyBuffer=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_instancepass,auxData,NULL,NULL);
        if (replyBuffer!=NULL)
            simReleaseBuffer_internal((simChar*)replyBuffer); // this should not happen! (or the replying module is not properly doing its job!)

        // Handle customization script execution:
        if ( App::ct->simulation->isSimulationStopped()&&(App::getEditModeType()==NO_EDIT_MODE) )
        {
            App::ct->luaScriptContainer->handleCustomizationScriptExecution(sim_customizationscriptcall_nonsimulation);
            App::ct->luaScriptContainer->removeDestroyedScripts(sim_scripttype_customizationscript);
        }
        if (App::ct->simulation->isSimulationPaused())
        {
            App::ct->luaScriptContainer->handleCustomizationScriptExecution(sim_customizationscriptcall_simulationpause);
            App::ct->luaScriptContainer->removeDestroyedScripts(sim_scripttype_customizationscript);
        }

        // Handle the main loop (one pass):
        if (_workThreadLoopCallback!=NULL)
            _workThreadLoopCallback();

        App::ct->luaScriptContainer->removeDestroyedScripts(sim_scripttype_childscript);
        App::ct->luaScriptContainer->removeDestroyedScripts(sim_scripttype_jointctrlcallback);

        // Handle add-on execution:
        if (!App::ct->simulation->isSimulationRunning()) // when simulation is running, we handle the add-on scripts after the main script was called
            App::ct->luaScriptContainer->handleAddOnScriptExecution();

#ifdef SIM_WITH_GUI
        App::ct->simulation->showAndHandleEmergencyStopButton(false,""); // 10/10/2015
#endif

        App::simThread->executeMessages(); // rendering, queued command execution, etc.
    }

    App::ct->luaScriptContainer->killAddOnScriptStates();
    App::setQuitLevel(1);

#ifdef SIM_WITHOUT_QT_AT_ALL
    SUIThreadCommand cmdIn;
    SUIThreadCommand cmdOut;
    cmdIn.cmdId=NO_SIGNAL_SLOT_EXIT_UITHREADCMD;
    App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
#else
    App::qtApp->quit();
#endif
    while (App::getQuitLevel()==1)
        VThread::sleep(1);

    // Ok, the UI thread has left its exec and is waiting for us
    delete App::simThread;

    CApiErrors::removeThreadFromErrorReporting();
    App::ct->copyBuffer->clearBuffer(); // important, some objects in the buffer might still call the mesh plugin or similar

#ifndef SIM_WITHOUT_QT_AT_ALL
    CSimAndUiThreadSync::simThread_allowUiThreadToWrite(); // ...finally unlock
#endif

    App::setQuitLevel(3); // tell the UI thread that we are done here

    return(0);
}

bool App::executeUiThreadCommand(SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut)
{
    return(uiThread->executeCommandViaUiThread(cmdIn,cmdOut));
}

void App::appendSimulationThreadCommand(int cmdId,int intP1,int intP2,float floatP1,float floatP2,const char* stringP1,const char* stringP2,int executionDelay)
{ // convenience function. All args have default values except for the first
    SSimulationThreadCommand cmd;
    cmd.cmdId=cmdId;
    cmd.intParams.push_back(intP1);
    cmd.intParams.push_back(intP2);
    cmd.floatParams.push_back(floatP1);
    cmd.floatParams.push_back(floatP2);
    if (stringP1==NULL)
        cmd.stringParams.push_back("");
    else
        cmd.stringParams.push_back(stringP1);
    if (stringP2==NULL)
        cmd.stringParams.push_back("");
    else
        cmd.stringParams.push_back(stringP2);
    appendSimulationThreadCommand(cmd,executionDelay);
}

void App::appendSimulationThreadCommand(SSimulationThreadCommand cmd,int executionDelay/*=0*/)
{
    static std::vector<SSimulationThreadCommand> delayed_cmd;
    static std::vector<int> delayed_delay;
    if (simThread!=NULL)
    {
        if (delayed_cmd.size()!=0)
        {
            for (unsigned int i=0;i<delayed_cmd.size();i++)
                simThread->appendSimulationThreadCommand(delayed_cmd[i],delayed_delay[i]);
            delayed_cmd.clear();
            delayed_delay.clear();
        }
        simThread->appendSimulationThreadCommand(cmd,executionDelay);
    }
    else
    { // can happen during the initialization phase, when the client loads a scene for instance
        delayed_cmd.push_back(cmd);
        delayed_delay.push_back(executionDelay);
    }
}

void App::setBrowserEnabled(bool e)
{
    _browserEnabled=e;
    setToolbarRefreshFlag();
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        mainWindow->setBrowserVisible(_browserEnabled);
#endif
}

bool App::getBrowserEnabled()
{
    return(_browserEnabled);
}

App::App(bool headless)
{
    FUNCTION_DEBUG;

    uiThread=NULL;
    _initSuccessful=false;
    _browserEnabled=true;

    userSettings=new CUserSettings();
    directories=new CDirectoryPaths();

    for (int i=0;i<9;i++)
        _applicationArguments.push_back("");

#ifdef SIM_WITH_OPENGL
    // Following strange construction is to have a work-around for a bug
    // on Qt5.5 (at least on Windows) where the application would only
    // show a black color for the openGl content when started from
    // QtCreator, or do very slow rendering. When starting from Qt Creator,
    // add following command-line:
    // vrep.exe -gCALLED_FROM_QTCREATOR
    bool fromQtCreator=false;
    for (int i=0;i<9;i++)
    {
        std::string s(App::getApplicationArgument(i));
        if (s.compare("CALLED_FROM_QTCREATOR")==0)
        {
            fromQtCreator=true;
            break;
        }
    }
    if (!fromQtCreator)
        QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL,true);
#endif

#ifdef SIM_WITH_GUI
    if (userSettings->highResDisplay!=-1)
    {
        char hr[2]="1";
        if (userSettings->highResDisplay!=0)
            hr[0]='2';
        qputenv("QT_DEVICE_PIXEL_RATIO",QByteArray(hr));
    }
#endif

#ifndef SIM_WITHOUT_QT_AT_ALL
    qtApp=new CVrepQApp(_qApp_argc,_qApp_argv);
#endif

#ifdef USING_QOPENGLWIDGET
    // Following mandatory on some platforms (e.g. OSX), call just after a QApplication was constructed:
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    QSurfaceFormat::setDefaultFormat(format);
#endif

#ifndef SIM_WITHOUT_QT_AT_ALL
    qRegisterMetaType<std::string>("std::string");
#endif

#ifdef SIM_WITH_GUI
    Q_INIT_RESOURCE(targaFiles);
    Q_INIT_RESOURCE(toolbarFiles);
    Q_INIT_RESOURCE(imageFiles_vrep);
    Q_INIT_RESOURCE(variousImageFiles);
#endif

#ifdef WIN_VREP
#ifdef SIM_WITH_GUI
    CVrepQApp::setStyle(QStyleFactory::create("Fusion")); // Probably most compatible. Other platforms: best in native (other styles have problems)!

    /*
        QPalette pal;
        pal.setColor(QPalette::Window,QColor(50,50,50));
        pal.setColor(QPalette::WindowText,Qt::white);
        pal.setColor(QPalette::Button,QColor(50,50,50));
        pal.setColor(QPalette::ButtonText,Qt::white);
        pal.setColor(QPalette::Text,Qt::white);
        pal.setColor(QPalette::BrightText,Qt::red);
        pal.setColor(QPalette::Highlight,QColor(40, 140, 220));
        pal.setColor(QPalette::HighlightedText,Qt::black);
        pal.setColor(QPalette::Link,QColor(40, 140, 220));
        pal.setColor(QPalette::Base,QColor(30,30,30));
        pal.setColor(QPalette::AlternateBase,QColor(50,50,50));
        pal.setColor(QPalette::ToolTipText,Qt::white);
        pal.setColor(QPalette::ToolTipBase,Qt::white);
        qtApp->setPalette(pal);
        qtApp->setStyleSheet("QToolTip{color:white;background-color:#3080E0;border:1px solid white;}");
        qtApp->setStyleSheet("QToolButton:checked{background-color:#606060;border:1px solid #808080;}");
    //*/
#endif
#endif

#ifdef WIN_VREP
    if (!CAuxLib::loadLibrary())
        return;
#endif

    loadExtLuaLibrary(userSettings->useExternalLuaLibrary,headless);

#ifdef SIM_WITH_GUI
    CAuxLibVideo::loadLibrary(headless);

    QFont f=QApplication::font();
#ifdef WIN_VREP
    f.setPixelSize(userSettings->guiFontSize_Win);
#endif
#ifdef MAC_VREP
    f.setPixelSize(userSettings->guiFontSize_Mac);
#endif
#ifdef LIN_VREP
    f.setPixelSize(userSettings->guiFontSize_Linux);
#endif
    QApplication::setFont(f);
#ifdef LIN_VREP // make the groupbox frame visible on Linux
    qtApp->setStyleSheet("QGroupBox {  border: 1px solid lightgray;} QGroupBox::title {  background-color: transparent; subcontrol-position: top left; padding:2 13px;}");
#endif
#endif

    uiThread=new CUiThread();
//  _guiThread=VThread::getCurrentThreadId();
    VThread::setUiThreadId();
    srand(VDateTime::getTimeInMs());    // Important so that the computer ID has some "true" random component!
                                        // Remember that each thread starts with a same seed!!!
    CMotionPlanningTask::randomSeed=VDateTime::getTimeInMs();
    _initSuccessful=true;
}

App::~App()
{
    FUNCTION_DEBUG;
    delete uiThread;

    // Clear the TAG that V-REP crashed! (because if we arrived here, we didn't crash!)
    CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
    cont.writeData("SIMSETTINGS_VREP_CRASHED","No",!App::userSettings->doNotWritePersistentData);

    // Remove any remaining auto-saved file:
    for (int i=1;i<30;i++)
    {
        std::string testScene=App::directories->executableDirectory+VREP_SLASH;
        testScene.append("AUTO_SAVED_INSTANCE_");
        testScene+=tt::FNb(i);
        testScene+=".";
        testScene+=VREP_SCENE_EXTENSION;
        if (VFile::doesFileExist(testScene))
            VFile::eraseFile(testScene);
    }

    delete directories;
    directories=NULL;
    delete userSettings;
    userSettings=NULL;
    unloadExtLuaLibrary();

#ifdef WIN_VREP
    CAuxLib::unloadLibrary();
#endif

#ifdef SIM_WITH_GUI
    CAuxLibVideo::unloadLibrary();
#endif

#ifndef SIM_WITHOUT_QT_AT_ALL
    if (qtApp!=NULL)
    {
#ifdef SIM_WITH_GUI
        Q_CLEANUP_RESOURCE(variousImageFiles);
        Q_CLEANUP_RESOURCE(imageFiles_vrep);
        Q_CLEANUP_RESOURCE(toolbarFiles);
        Q_CLEANUP_RESOURCE(targaFiles);
#endif // SIM_WITH_GUI
        delete qtApp;
        qtApp=NULL;
    }
#endif // SIM_WITHOUT_QT_AT_ALL
    _applicationArguments.clear();
}

bool App::wasInitSuccessful()
{
    return(_initSuccessful);
}

void App::postExitRequest()
{ // call only from sim thread!
    // Important to remove all objects before we destroy the main window,
    // since some of them might be linked it:

    uiThread->showOrHideEmergencyStop(false,"");
    uiThread->showOrHideProgressBar(true,-1,"Leaving...");
    while (ct->getInstanceCount()>1)
        ct->destroyCurrentInstance();
    ct->emptyScene(true);
    uiThread->showOrHideProgressBar(false);
    _exitRequest=true;
}

bool App::getExitRequest()
{
    return(_exitRequest);
}

bool App::isSimulatorRunning()
{
    return(_simulatorIsRunning);
}

void App::beep(int frequ,int duration)
{
#ifdef SIM_WITH_GUI
    for (int i=0;i<3;i++)
    {
#ifdef WIN_VREP
        Beep(frequ,duration);
#else
        if (qtApp!=NULL)
            qtApp->beep();
#endif
        VThread::sleep(500);
    }
#endif
}

void App::setApplicationName(const char* name)
{
    _applicationName=name;
}

std::string App::getApplicationName()
{
    return(_applicationName);
}

void App::createMainContainer()
{
    FUNCTION_DEBUG;
    ct=new CMainContainer();
    ct->initialize();
}

void App::deleteMainContainer()
{
    FUNCTION_DEBUG;
    ct->deinitialize();
    delete ct;
    ct=NULL;
}

void App::_runInitializationCallback(void(*initCallBack)())
{
    FUNCTION_DEBUG;
    if (initCallBack!=NULL)
        initCallBack(); // this should load all plugins

    App::ct->luaCustomFuncAndVarContainer->outputWarningWithFunctionNamesWithoutPlugin(true);

    if (CPluginContainer::isMeshPluginAvailable())
        printf("Using the 'MeshCalc' plugin.\n");
    else
        printf("The 'MeshCalc' plugin could not be initialized.\n");

    if (CPathPlanningInterface::initializeFunctionsIfNeeded())
        printf("Using the 'PathPlanning' plugin.\n");

    CSimpleFilter::readAllExternalFilters();

    // Following is so that we can display the correct splash screen at start-up (at that time we haven't yet loaded the plugins!):
    CPersistentDataContainer a(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
    if (CPluginContainer::isMeshPluginAvailable())
        a.writeData("usesMajorPlugin__","yes",!App::userSettings->doNotWritePersistentData);
    else
        a.writeData("usesMajorPlugin__","",!App::userSettings->doNotWritePersistentData);
}

void App::_runDeinitializationCallback(void(*deinitCallBack)())
{
    FUNCTION_DEBUG;
    if (deinitCallBack!=NULL)
        deinitCallBack(); // this will unload all plugins!!
}

void App::run(void(*initCallBack)(),void(*loopCallBack)(),void(*deinitCallBack)())
{ // We arrive here with a single thread: the UI thread!
    FUNCTION_DEBUG;
    _exitRequest=false;
    CApiErrors::addNewThreadForErrorReporting(0);
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        mainWindow->setFocus(Qt::MouseFocusReason); // needed because at first Qt behaves strangely (really??)
    uiThread->setFileDialogsNative(userSettings->fileDialogs);
#endif

    addStatusbarMessage(IDSNS_SIMULATOR_LAUNCHED);
    _simulatorIsRunning=true;

    // Load the plugins via callback mechanism
    // Plugins are loaded by the UI thread! (still the only thread)
    _runInitializationCallback(initCallBack);

    // Now start the main simulation thread (i.e. the "SIM thread", the one that handles a simulation):
    _workThreadLoopCallback=loopCallBack;

#ifdef SIM_WITHOUT_QT_AT_ALL
    VThread::launchThread(_workThread,VThread::VTHREAD_NORMAL_PRIORITY,false);
#else
    VThread::launchSimpleThread(_workThread,VThread::VTHREAD_NORMAL_PRIORITY);
#endif

    while (simThread==NULL)
        VThread::sleep(1);

#ifdef SIM_WITH_GUI
    // Prepare a few initial triggers:
    SSimulationThreadCommand cmd;
    cmd.cmdId=AUTO_SAVE_SCENE_CMD;
    cmd.intParams.push_back(0); // load autosaved scenes, if crashed
    App::appendSimulationThreadCommand(cmd,2000); // was 1000

    cmd.cmdId=MEMORIZE_UNDO_STATE_IF_NEEDED_CMD;
    cmd.intParams.clear();
    App::appendSimulationThreadCommand(cmd,2200); // was 200


    cmd.cmdId=REFRESH_DIALOGS_CMD;
    appendSimulationThreadCommand(cmd,1000);
    cmd.cmdId=DISPLAY_WARNING_IF_DEBUGGING_CMD;
    appendSimulationThreadCommand(cmd,3000);
#endif

    // The UI thread sits here during the whole application:
    _processGuiEventsUntilQuit();


    // App::qtApp->quit() was called from the SIM thread.
    _quitLevel=2; // indicate to the SIM thread that the UI thread has left its exec

    // Wait until the SIM thread ended:
    while (_quitLevel==2)
        VThread::sleep(1);

    // Ok, we unload the plugins. This happens with the UI thread.
    _runDeinitializationCallback(deinitCallBack);

    CApiErrors::removeThreadFromErrorReporting();

    deinitGl_ifNeeded();

    _simulatorIsRunning=false;
}

void App::_processGuiEventsUntilQuit()
{
#ifdef SIM_WITHOUT_QT_AT_ALL
    uiThread->processGuiEventsUntilQuit_noSignalSlots();
#else
    // 1. Best if possible to use:
    qtApp->exec();
    // 2. Second best if possible to use:
    // QEventLoop eventLoop;
    // eventLoop->exec();
#endif
}

void App::setQuitLevel(int l)
{
    _quitLevel=l;
}

int App::getQuitLevel()
{
    return(_quitLevel);
}

std::string App::getApplicationArgument(int index)
{
    if (_applicationArguments.size()==0)
    {
        for (int i=0;i<9;i++)
            _applicationArguments.push_back("");
    }
    if (index<9)
        return(_applicationArguments[index]);
    return("");
}

void App::setApplicationArgument(int index,std::string arg)
{
    if (_applicationArguments.size()==0)
    {
        for (int i=0;i<9;i++)
            _applicationArguments.push_back("");
    }
    if (index<9)
        _applicationArguments[index]=arg;
}

int App::getEditModeType()
{ // helper
    int retVal=NO_EDIT_MODE;
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        retVal=mainWindow->editModeContainer->getEditModeType();
#endif
    return(retVal);
}

void App::setRebuildHierarchyFlag()
{ // helper
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        mainWindow->oglSurface->hierarchy->setRebuildHierarchyFlag();
#endif
}

void App::setResetHierarchyViewFlag()
{ // helper
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        mainWindow->oglSurface->hierarchy->setResetViewFlag();
#endif
}

void App::setRefreshHierarchyViewFlag()
{ // helper
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        mainWindow->oglSurface->hierarchy->setRefreshViewFlag();
#endif
}

void App::setLightDialogRefreshFlag()
{ // helper
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        mainWindow->setLightDialogRefreshFlag();
#endif
}

void App::setFullDialogRefreshFlag()
{ // helper
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        mainWindow->setFullDialogRefreshFlag();
#endif
}

void App::setDialogRefreshDontPublishFlag()
{ // helper
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        mainWindow->setDialogRefreshDontPublishFlag();
#endif
}

void App::setToolbarRefreshFlag()
{ // helper
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        mainWindow->setToolbarRefreshFlag();
#endif
}

int App::getMouseMode()
{ // helper
    int retVal=0;
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        retVal=mainWindow->getMouseMode();
#endif
    return(retVal);
}

void App::setMouseMode(int mm)
{ // helper
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        mainWindow->setMouseMode(mm);
#endif
}

void App::setDefaultMouseMode()
{ // helper
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        mainWindow->setDefaultMouseMode();
#endif
}

bool App::isFullScreen()
{ // helper
    bool retVal=false;
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        retVal=mainWindow->isFullScreen();
#endif
    return(retVal);
}

void App::setFullScreen(bool f)
{ // helper
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
        mainWindow->setFullScreen(f);
#endif
}

void App::addStatusbarMessage(const std::string& txt)
{
#ifdef SIM_WITH_GUI
    if (mainWindow!=NULL)
    {
        if ((operationalUIParts&sim_gui_statusbar)&&(mainWindow->statusBar!=NULL) )
        {
            if (!VThread::isCurrentThreadTheUiThread())
            { // we are NOT in the UI thread. We execute the command in a delayed manner:
                SUIThreadCommand cmdIn;
                SUIThreadCommand cmdOut;
                cmdIn.cmdId=ADD_STATUSBAR_MESSAGE_UITHREADCMD;
                cmdIn.stringParams.push_back(txt);
                uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
            }
            else
            {
                mainWindow->statusBar->appendPlainText(txt.c_str());
                mainWindow->statusBar->moveCursor(QTextCursor::End);
                mainWindow->statusBar->moveCursor(QTextCursor::PreviousBlock);
                mainWindow->statusBar->ensureCursorVisible();
            }
        }
    }
    else
#endif
    {
        if (userSettings->redirectStatusbarMsgToConsoleInHeadlessMode)
            printf("%s\n",txt.c_str());
    }
}

float* App::getRGBPointerFromItem(int objType,int objID1,int objID2,int colComponent,std::string* auxDlgTitle)
{ // auxDlgTitle can be NULL
    std::string __auxDlgTitle;
    std::string* _auxDlgTitle=&__auxDlgTitle;
    if (auxDlgTitle!=NULL)
        _auxDlgTitle=auxDlgTitle;

    if (objType==COLOR_ID_AMBIENT_LIGHT)
    {
        _auxDlgTitle->assign("Ambient light");
        return(ct->environment->ambientLightColor);
    }
    if (objType==COLOR_ID_BACKGROUND_UP)
    {
        _auxDlgTitle->assign("Background (up)");
        return(ct->environment->backGroundColor);
    }
    if (objType==COLOR_ID_BACKGROUND_DOWN)
    {
        _auxDlgTitle->assign("Background (down)");
        return(ct->environment->backGroundColorDown);
    }
    if (objType==COLOR_ID_FOG)
    {
        _auxDlgTitle->assign("Fog");
        return(ct->environment->fogBackgroundColor);
    }
    if (objType==COLOR_ID_MIRROR)
    {
        _auxDlgTitle->assign("Mirror");
        CMirror* it=App::ct->objCont->getMirror(objID1);
        if ((it!=NULL)&&it->getIsMirror())
            return(it->mirrorColor);
    }
    if (objType==COLOR_ID_OCTREE)
    {
        _auxDlgTitle->assign("Octree");
        COctree* it=App::ct->objCont->getOctree(objID1);
        if (it!=NULL)
            return(it->getColor()->colors);
    }
    if (objType==COLOR_ID_POINTCLOUD)
    {
        _auxDlgTitle->assign("Point cloud");
        CPointCloud* it=App::ct->objCont->getPointCloud(objID1);
        if (it!=NULL)
            return(it->getColor()->colors);
    }
    if (objType==COLOR_ID_GRAPH_2DCURVE)
    {
        _auxDlgTitle->assign("Graph - 2D curve");
        CGraph* it=ct->objCont->getGraph(objID1);
        if (it!=NULL)
        {
            CGraphDataComb* grDataComb=it->getGraphData2D(objID2);
            if (grDataComb!=NULL)
                return(grDataComb->curveColor.colors);
        }
    }
    if (objType==COLOR_ID_GRAPH_BACKGROUND)
    {
        _auxDlgTitle->assign("Graph - background");
        CGraph* it=ct->objCont->getGraph(objID1);
        if (it!=NULL)
            return(it->backgroundColor);
    }
    if (objType==COLOR_ID_GRAPH_GRID)
    {
        _auxDlgTitle->assign("Graph - grid");
        CGraph* it=ct->objCont->getGraph(objID1);
        if (it!=NULL)
            return(it->textColor);
    }
    if (objType==COLOR_ID_GRAPH_TIMECURVE)
    {
        _auxDlgTitle->assign("Graph - data stream");
        CGraph* it=ct->objCont->getGraph(objID1);
        if (it!=NULL)
        {
            CGraphData* grData=it->getGraphData(objID2);
            if (grData!=NULL)
                return(grData->ambientColor);
        }
    }
    if ((objType==COLOR_ID_OPENGLBUTTON_UP)||(objType==COLOR_ID_OPENGLBUTTON_DOWN)||(objType==COLOR_ID_OPENGLBUTTON_TEXT))
    {
        if (objType==COLOR_ID_OPENGLBUTTON_UP)
            _auxDlgTitle->assign("Button - up");
        if (objType==COLOR_ID_OPENGLBUTTON_DOWN)
            _auxDlgTitle->assign("Button - down");
        if (objType==COLOR_ID_OPENGLBUTTON_TEXT)
            _auxDlgTitle->assign("Button - text");
        CButtonBlock* block=App::ct->buttonBlockContainer->getBlockWithID(objID1);
        if (block!=NULL)
        {
            CSoftButton* itButton=block->getButtonWithID(objID2);
            if (itButton!=NULL)
            {
                if (objType==COLOR_ID_OPENGLBUTTON_UP)
                    return(itButton->backgroundColor);
                if (objType==COLOR_ID_OPENGLBUTTON_DOWN)
                    return(itButton->downBackgroundColor);
                if (objType==COLOR_ID_OPENGLBUTTON_TEXT)
                    return(itButton->textColor);
            }
        }
    }


    int allowedParts=0;
    CVisualParam* vp=getVisualParamPointerFromItem(objType,objID1,objID2,_auxDlgTitle,&allowedParts);
    if (vp!=NULL)
    {
        if ((colComponent==sim_colorcomponent_ambient_diffuse)&&(allowedParts&1))
            return((vp->colors+0));
        if ((colComponent==sim_colorcomponent_diffuse)&&(allowedParts&2))
            return((vp->colors+3));
        if ((colComponent==sim_colorcomponent_specular)&&(allowedParts&4))
            return((vp->colors+6));
        if ((colComponent==sim_colorcomponent_emission)&&(allowedParts&8))
            return((vp->colors+9));
        if ((colComponent==sim_colorcomponent_auxiliary)&&(allowedParts&16))
            return((vp->colors+12));
    }

    return(NULL);
}

CVisualParam* App::getVisualParamPointerFromItem(int objType,int objID1,int objID2,std::string* auxDlgTitle,int* allowedParts)
{ // auxDlgTitle and allowedParts can be NULL. Bit-coded: 1=ambient/diffuse, 2=diffuse(light only), 4=spec, 8=emiss., 16=aux channels, 32=pulsation, 64=shininess, 128=opacity, 256=colorName, 512=ext. string
    std::string __auxDlgTitle;
    int __allowedParts;
    std::string* _auxDlgTitle=&__auxDlgTitle;
    int* _allowedParts=&__allowedParts;
    if (auxDlgTitle!=NULL)
        _auxDlgTitle=auxDlgTitle;
    if (allowedParts!=NULL)
        _allowedParts=allowedParts;

    if (objType==COLOR_ID_CAMERA_A)
    {
        _auxDlgTitle->assign("Camera - part A");
        _allowedParts[0]=1+4+8+16+32+64;
        CCamera* it=ct->objCont->getCamera(objID1);
        if (it!=NULL)
            return(it->getColor(false));
    }
    if (objType==COLOR_ID_CAMERA_B)
    {
        _auxDlgTitle->assign("Camera - part B");
        _allowedParts[0]=1+4+8+16+32+64;
        CCamera* it=ct->objCont->getCamera(objID1);
        if (it!=NULL)
            return(it->getColor(true));
    }
    if (objType==COLOR_ID_FORCESENSOR_A)
    {
        _auxDlgTitle->assign("Force sensor - part A");
        _allowedParts[0]=1+4+8+16+32+64;
        CForceSensor* it=ct->objCont->getForceSensor(objID1);
        if (it!=NULL)
            return(it->getColor(false));
    }
    if (objType==COLOR_ID_FORCESENSOR_B)
    {
        _auxDlgTitle->assign("Force sensor - part B");
        _allowedParts[0]=1+4+8+16+32+64;
        CForceSensor* it=ct->objCont->getForceSensor(objID1);
        if (it!=NULL)
            return(it->getColor(true));
    }
    if (objType==COLOR_ID_JOINT_A)
    {
        _auxDlgTitle->assign("Joint - part A");
        _allowedParts[0]=1+4+8+16+32+64;
        CJoint* it=ct->objCont->getJoint(objID1);
        if (it!=NULL)
            return(it->getColor(false));
    }
    if (objType==COLOR_ID_JOINT_B)
    {
        _auxDlgTitle->assign("Joint - part B");
        _allowedParts[0]=1+4+8+16+32+64;
        CJoint* it=ct->objCont->getJoint(objID1);
        if (it!=NULL)
            return(it->getColor(true));
    }
    if (objType==COLOR_ID_PATH)
    {
        _auxDlgTitle->assign("Path");
        _allowedParts[0]=1+4+8+16+32+64;
        CPath* it=ct->objCont->getPath(objID1);
        if ( (it!=NULL)&&(it->pathContainer!=NULL) )
            return(&it->pathContainer->_lineColor);
    }
    if (objType==COLOR_ID_PATH_SHAPING)
    {
        _auxDlgTitle->assign("Path shaping");
        _allowedParts[0]=1+4+8+16+32+64;
        CPath* it=ct->objCont->getPath(objID1);
        if (it!=NULL)
            return(it->getShapingColor());
    }
    if (objType==COLOR_ID_GRAPH_3DCURVE)
    {
        _auxDlgTitle->assign("Graph - 3D curve");
        _allowedParts[0]=1+8;
        CGraph* it=ct->objCont->getGraph(objID1);
        if (it!=NULL)
        {
            CGraphDataComb* grDataComb=it->getGraphData3D(objID2);
            if (grDataComb!=NULL)
                return(&grDataComb->curveColor);
        }
    }
    if (objType==COLOR_ID_WIRELESS_EMISSIONS)
    {
        _auxDlgTitle->assign("Wireless emissions");
        _allowedParts[0]=1+4+8+16+32+64+128;
        return(&App::ct->environment->wirelessEmissionVolumeColor);
    }
    if (objType==COLOR_ID_WIRELESS_RECEPTIONS)
    {
        _auxDlgTitle->assign("Wireless receptions");
        _allowedParts[0]=1+4+8+16+32+64+128;
        return(&App::ct->environment->wirelessReceptionVolumeColor);
    }
    if (objType==COLOR_ID_COLLISION)
    {
        _auxDlgTitle->assign("Collision");
        _allowedParts[0]=1+4+8+16+32+64;
        return(&App::ct->mainSettings->collisionColor);
    }
    if (objType==COLOR_ID_COLLISIONCONTOUR)
    {
        _auxDlgTitle->assign("Collision contour");
        _allowedParts[0]=1+4+8+16+32+64;
        CRegCollision* it=App::ct->collisions->getObject(objID1);
        if (it!=NULL)
            return(&it->contourColor);
    }
    if (objType==COLOR_ID_DISTANCESEGMENT)
    {
        _auxDlgTitle->assign("Distance segment");
        _allowedParts[0]=1+4+8+16+32+64;
        CRegDist* it=App::ct->distances->getObject(objID1);
        if (it!=NULL)
            return(&it->segmentColor);
    }
    if (objType==COLOR_ID_CLIPPINGPLANE)
    {
        _auxDlgTitle->assign("Clipping plane");
        _allowedParts[0]=1+4+8+16+32+64+128;
        CMirror* it=App::ct->objCont->getMirror(objID1);
        if ((it!=NULL)&&(!it->getIsMirror()))
            return(it->getClipPlaneColor());
    }
    if (objType==COLOR_ID_LIGHT_CASING)
    {
        _auxDlgTitle->assign("Light - casing");
        _allowedParts[0]=1+4+8+16+64;
        CLight* it=ct->objCont->getLight(objID1);
        if (it!=NULL)
            return(it->getColor(false));
    }
    if (objType==COLOR_ID_LIGHT_LIGHT)
    {
        _auxDlgTitle->assign("Light");
        _allowedParts[0]=2+4;
        CLight* it=ct->objCont->getLight(objID1);
        if (it!=NULL)
            return(it->getColor(true));
    }
    if (objType==COLOR_ID_DUMMY)
    {
        _auxDlgTitle->assign("Dummy");
        _allowedParts[0]=1+4+8+16+32+64;
        CDummy* it=ct->objCont->getDummy(objID1);
        if (it!=NULL)
            return(it->getColor());
    }
    if (objType==COLOR_ID_VISIONSENSOR_PASSIVE)
    {
        _auxDlgTitle->assign("Vision sensor - passive");
        _allowedParts[0]=1+4+8+16+32;
        CVisionSensor* it=ct->objCont->getVisionSensor(objID1);
        if (it!=NULL)
            return(it->getColor(false));
    }
    if (objType==COLOR_ID_VISIONSENSOR_ACTIVE)
    {
        _auxDlgTitle->assign("Vision sensor - active");
        _allowedParts[0]=1+4+8+16+32;
        CVisionSensor* it=ct->objCont->getVisionSensor(objID1);
        if (it!=NULL)
            return(it->getColor(true));
    }
    if (objType==COLOR_ID_PROXSENSOR_PASSIVE)
    {
        _auxDlgTitle->assign("Proximity sensor - passive");
        _allowedParts[0]=1+4+8+16+32;
        CProxSensor* it=ct->objCont->getProximitySensor(objID1);
        if (it!=NULL)
            return(it->getColor(0));
    }
    if (objType==COLOR_ID_PROXSENSOR_ACTIVE)
    {
        _auxDlgTitle->assign("Proximity sensor - active");
        _allowedParts[0]=1+4+8+16+32;
        CProxSensor* it=ct->objCont->getProximitySensor(objID1);
        if (it!=NULL)
            return(it->getColor(1));
    }
    if (objType==COLOR_ID_PROXSENSOR_RAY)
    {
        _auxDlgTitle->assign("Proximity sensor - ray");
        _allowedParts[0]=1+4+8+16+32;
        CProxSensor* it=ct->objCont->getProximitySensor(objID1);
        if (it!=NULL)
            return(it->getColor(2));
    }
    if (objType==COLOR_ID_PROXSENSOR_MINDIST)
    {
        _auxDlgTitle->assign("Proximity sensor - min. dist.");
        _allowedParts[0]=1+4+8+16+32;
        CProxSensor* it=ct->objCont->getProximitySensor(objID1);
        if (it!=NULL)
            return(it->getColor(3));
    }
    if (objType==COLOR_ID_MILL_PASSIVE)
    {
        _auxDlgTitle->assign("Mill - passive");
        _allowedParts[0]=1+4+8+16+32;
        CMill* it=ct->objCont->getMill(objID1);
        if (it!=NULL)
            return(it->getColor(false));
    }
    if (objType==COLOR_ID_MILL_ACTIVE)
    {
        _auxDlgTitle->assign("Mill - active");
        _allowedParts[0]=1+4+8+16+32;
        CMill* it=ct->objCont->getMill(objID1);
        if (it!=NULL)
            return(it->getColor(true));
    }
    if (objType==COLOR_ID_SHAPE)
    {
        _auxDlgTitle->assign("Shape");
        _allowedParts[0]=1+4+8+16+32+64+128+256+512;
        CShape* it=ct->objCont->getShape(objID1);
        if ((it!=NULL)&&(!it->isCompound()))
            return(&((CGeometric*)it->geomData->geomInfo)->color);
    }
    if (objType==COLOR_ID_SHAPE_GEOMETRY)
    {
#ifdef SIM_WITH_GUI
        if ((App::mainWindow->editModeContainer->getEditModeObjectID()==objID1)&&(App::mainWindow->editModeContainer->getMultishapeEditMode()->getMultishapeGeometricComponentIndex()==objID2))
        {
            _auxDlgTitle->assign("Shape component");
            _allowedParts[0]=1+4+8+16+32+64+128+256+512;
            CShape* it=ct->objCont->getShape(objID1);
            if ((it!=NULL)&&it->isCompound())
            {
                std::vector<CGeometric*> allGeometrics;
                it->geomData->geomInfo->getAllShapeComponentsCumulative(allGeometrics);
                if ((objID2>=0)&&(objID2<int(allGeometrics.size())))
                    return(&allGeometrics[objID2]->color);
            }
        }
#endif
    }


    _allowedParts[0]=0;
    return(NULL);
}

CTextureProperty* App::getTexturePropertyPointerFromItem(int objType,int objID1,int objID2,std::string* auxDlgTitle,bool* is3D,bool* valid,CGeometric** geom)
{ // auxDlgTitle, is3D, isValid and geom can be NULL.
    std::string __auxDlgTitle;
    bool __is3D=false;
    bool __isValid=false;
    CGeometric* __geom=NULL;
    std::string* _auxDlgTitle=&__auxDlgTitle;
    bool* _is3D=&__is3D;
    bool* _isValid=&__isValid;
    CGeometric** _geom=&__geom;
    if (auxDlgTitle!=NULL)
        _auxDlgTitle=auxDlgTitle;
    if (is3D!=NULL)
        _is3D=is3D;
    if (valid!=NULL)
        _isValid=valid;
    if (geom!=NULL)
        _geom=geom;
    _isValid[0]=false;
    _geom[0]=NULL;
    if (objType==TEXTURE_ID_SIMPLE_SHAPE)
    {
        _auxDlgTitle->assign("Shape");
        _is3D[0]=true;
        CShape* it=ct->objCont->getShape(objID1);
        if ( (it!=NULL)&&(!it->isCompound()) )
        {
            _isValid[0]=true;
            _geom[0]=((CGeometric*)it->geomData->geomInfo);
            return(_geom[0]->getTextureProperty());
        }
    }
    if (objType==TEXTURE_ID_COMPOUND_SHAPE)
    {
        _auxDlgTitle->assign("Shape component");
        _is3D[0]=true;
        CShape* it=ct->objCont->getShape(objID1);
        if (it!=NULL)
        {
            std::vector<CGeometric*> allGeometrics;
            it->geomData->geomInfo->getAllShapeComponentsCumulative(allGeometrics);
            if ((objID2>=0)&&(objID2<int(allGeometrics.size())))
            {
                _isValid[0]=true;
                _geom[0]=allGeometrics[objID2];
                return(_geom[0]->getTextureProperty());
            }
        }
    }
    if (objType==TEXTURE_ID_OPENGL_GUI_BACKGROUND)
    {
        _auxDlgTitle->assign("OpenGl custom UI background");
        _is3D[0]=false;
        CButtonBlock* it=ct->buttonBlockContainer->getBlockWithID(objID1);
        if (it!=NULL)
        {
            _isValid[0]=true;
            return(it->getTextureProperty());
        }
    }
    if (objType==TEXTURE_ID_OPENGL_GUI_BUTTON)
    {
        _auxDlgTitle->assign("OpenGl custom UI button");
        _is3D[0]=false;
        CButtonBlock* it=ct->buttonBlockContainer->getBlockWithID(objID1);
        if (it!=NULL)
        {
            CSoftButton* butt=it->getButtonWithID(objID2);
            if (butt!=NULL)
            {
                _isValid[0]=true;
                return(butt->getTextureProperty());
            }
        }
    }

    return(NULL);
}


#ifdef SIM_WITH_GUI
void App::showSplashScreen()
{
    App::setShowConsole(false);
    QPixmap pixmap;
    // At this stage the plugins are not yet loaded. So we verify if previous run used one of the major plugins that have a different license:
    CPersistentDataContainer temp(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE); // very unelegant, change this later!
    std::string dummyVal;
    if (temp.readData("usesMajorPlugin__",dummyVal))
        pixmap.load(":/imageFiles_vrep/splashEdu.png"); // you may change this splash screen only if you use the commercial version of the plugins!
    else
        pixmap.load(":/imageFiles_vrep/splashBasic.png");

    QSplashScreen splash(pixmap,Qt::WindowStaysOnTopHint);
    splash.setMask(pixmap.mask());
    QString txt("Version ");
    txt+=VREP_PROGRAM_VERSION;
    txt+=" ";
    txt+=VREP_PROGRAM_REVISION;
    txt+=", Built ";
    txt+=__DATE__;
    splash.showMessage(txt,Qt::AlignLeft|Qt::AlignBottom);
    splash.show();
    int ct=VDateTime::getTimeInMs();
    while (VDateTime::getTimeDiffInMs(ct)<2000)
    {
        splash.raise();
        App::qtApp->processEvents();
        VThread::sleep(1);
    }
    splash.hide();
    App::setShowConsole(true);
}

void App::setIcon()
{
#ifdef WIN_VREP
        App::qtApp->setWindowIcon(QIcon(":/variousImageFiles/v_repIcon16.png"));
#else




    App::qtApp->setWindowIcon(QIcon(":/imageFiles_vrep/v_repIcon128.png"));
#endif
}

void App::createMainWindow()
{
    FUNCTION_DEBUG;
    mainWindow=new CMainWindow();
    mainWindow->initializeWindow();
    setShowConsole(userSettings->alwaysShowConsole);
}

void App::deleteMainWindow()
{
    FUNCTION_DEBUG;
    delete mainWindow;
    mainWindow=NULL;
}

void App::setShowConsole(bool s)
{
#ifdef WIN_VREP
    CAuxLib::vrepWin_showConsole(char(s));
#endif
}
#endif
