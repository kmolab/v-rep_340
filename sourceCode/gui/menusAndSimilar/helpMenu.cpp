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
#include "easyLock.h"
#include "v_rep_internal.h"
#include "helpMenu.h"
#include "oGL.h"
#include "global.h"
#include "threadPool.h"
#include "algos.h"
#include "tt.h"
#include "app.h"
#include "v_repStrings.h"
#include "ttUtil.h"
#include "vVarious.h"
#include "qdlgabout.h"
#include <QDesktopServices>
#include <QUrl>
#include "debugLogFile.h"
#include "vMessageBox.h"
#include "collisionRoutine.h"
#include "distanceRoutine.h"

CHelpMenu::CHelpMenu()
{

}

CHelpMenu::~CHelpMenu()
{

}

void CHelpMenu::addMenu(VMenu* menu)
{
    menu->appendMenuItem(true,false,HELP_TOPICS_CMD,IDS_HELP_TOPICS_MENU_ITEM);
    menu->appendMenuItem(true,false,ABOUT_CMD,std::string(IDS_ABOUT_V_REP)+"...");
    menu->appendMenuItem(true,false,CREDITS_CMD,std::string(IDSN_CREDITS)+"...");

    VMenu* debugMenu=new VMenu();
    debugMenu->appendMenuItem(true,CSimAndUiThreadSync::getShowActivityInConsole(),SHOW_GUI_SIM_THREAD_SYNC_ACTIVITY_DEBUG_CMD,IDSN_SHOW_GUI_SIM_THREAD_SYNC_ACTIVITY_DEBUG_MENU_ITEM,true);
    debugMenu->appendMenuItem(true,CSimAndUiThreadSync::getShowLockFailsActivityInConsole(),SHOW_GUI_SIM_THREAD_SYNC_FAILS_ACTIVITY_DEBUG_CMD,IDSN_SHOW_GUI_SIM_THREAD_SYNC_FAILS_ACTIVITY_DEBUG_MENU_ITEM,true);
    debugMenu->appendMenuItem(true,CFuncDebug::getDebugMask()&1,SHOW_INTERNAL_FUNCTION_ACCESS_DEBUG_CMD,IDSN_SHOW_INTERNAL_FUNCTION_ACCESS_DEBUG_MENU_ITEM,true);
    debugMenu->appendMenuItem(true,CFuncDebug::getDebugMask()&2,SHOW_C_API_ACCESS_DEBUG_CMD,IDSN_SHOW_C_API_ACCESS_DEBUG_MENU_ITEM,true);
    debugMenu->appendMenuItem(true,CFuncDebug::getDebugMask()&4,SHOW_LUA_API_ACCESS_DEBUG_CMD,IDSN_SHOW_LUA_API_ACCESS_DEBUG_MENU_ITEM,true);
    debugMenu->appendMenuItem(true,CEasyLock::getShowActivity(),SHOW_EASYLOCK_ACTIVITY_DEBUG_CMD,IDSN_SHOW_EASYLOCK_ACTIVITY_DEBUG_MENU_ITEM,true);
    debugMenu->appendMenuItem(true,CThreadPool::getShowThreadSwitches(),SHOW_THREAD_SWITCHES_DEBUG_CMD,IDSN_SHOW_THREAD_SWITCHES_DEBUG_MENU_ITEM,true);
    debugMenu->appendMenuItem(true,CDebugLogFile::getDebugToFile(),DEBUG_TO_FILE_DEBUG_CMD,IDSN_SEND_DEBUG_INFO_TO_FILE_MENU_ITEM,true);
    debugMenu->appendMenuSeparator();
    debugMenu->appendMenuItem(true,!CViewableBase::getFrustumCullingEnabled(),DISABLE_FRUSTUM_CULLING_DEBUG_CMD,IDSN_DISABLE_FRUSTUM_CULLING_DEBUG_MENU_ITEM,true);
    debugMenu->appendMenuItem(true,!CDistanceRoutine::getDistanceCachingEnabled(),DISABLE_DISTANCE_CACHING_DEBUG_CMD,IDSN_DISABLE_DISTANCE_CACHING_DEBUG_MENU_ITEM,true);
    debugMenu->appendMenuItem(true,CShape::getDebugObbStructures(),VISUALIZE_OBB_STRUCTURE_DEBUG_CMD,IDSN_VISUALIZE_OBB_STRUCTURE_DEBUG_MENU_ITEM,true);
    menu->appendMenuAndDetach(debugMenu,true,IDSN_DEBUG_MENU_ITEM);

}

bool CHelpMenu::processCommand(int commandID)
{ // Return value is true if the command belonged to help menu and was executed
    if (commandID==HELP_TOPICS_CMD)
    {
        if (VThread::isCurrentThreadTheUiThread())
        { // We are in the UI thread. Execute the command via the main thread:
#ifdef MAC_VREP
            std::string tmp(App::directories->executableDirectory+"/../../../"+"helpFiles"+VREP_SLASH+"index.html");
#else
            std::string tmp(App::directories->executableDirectory+VREP_SLASH+"helpFiles"+VREP_SLASH+"index.html");
#endif
            VVarious::openUrl(tmp);
        }
        return(true);
    }
    if (commandID==ABOUT_CMD)
    {
        if (VThread::isCurrentThreadTheUiThread())
        { // We are in the UI thread. Execute the command via the main thread:
            IF_UI_EVENT_CAN_WRITE_DATA
            {
                CQDlgAbout aboutBox(App::mainWindow);
                aboutBox.makeDialogModal();
            }
        }
        return(true);
    }
    if (commandID==CREDITS_CMD)
    {
        if (VThread::isCurrentThreadTheUiThread())
        { // We are in the UI thread. Execute the command via the main thread:
#ifdef MAC_VREP
            std::string tmp(App::directories->executableDirectory+"/../../../"+"credits.txt");
#else
            std::string tmp(App::directories->executableDirectory+VREP_SLASH+"credits.txt");
#endif
            if (VFile::doesFileExist(tmp))
            { // FILE is present!
                VVarious::openTextFile(tmp);
            }
            else
            { // file doesn't exist.
                App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_CREDITS),strTranslate(IDS_FILE_COULD_NOT_BE_FOUND_),VMESSAGEBOX_OKELI);
            }
        }
        return(true);
    }

    if (commandID==SHOW_GUI_SIM_THREAD_SYNC_ACTIVITY_DEBUG_CMD)
    {
        IF_UI_EVENT_CAN_READ_DATA_CMD("SHOW_GUI_SIM_THREAD_SYNC_ACTIVITY_DEBUG_CMD")
        {
            CSimAndUiThreadSync::setShowActivityInConsole(!CSimAndUiThreadSync::getShowActivityInConsole());
            App::userSettings->saveUserSettings();
        }
        return(true);
    }
    if (commandID==SHOW_GUI_SIM_THREAD_SYNC_FAILS_ACTIVITY_DEBUG_CMD)
    {
        IF_UI_EVENT_CAN_READ_DATA_CMD("SHOW_GUI_SIM_THREAD_SYNC_FAILS_ACTIVITY_DEBUG_CMD")
        {
            CSimAndUiThreadSync::setShowLockFailsActivityInConsole(!CSimAndUiThreadSync::getShowLockFailsActivityInConsole());
            App::userSettings->saveUserSettings();
        }
        return(true);
    }
    if (commandID==SHOW_INTERNAL_FUNCTION_ACCESS_DEBUG_CMD)
    {
        IF_UI_EVENT_CAN_READ_DATA_CMD("SHOW_INTERNAL_FUNCTION_ACCESS_DEBUG_CMD")
        {
            CFuncDebug::setDebugMask(CFuncDebug::getDebugMask()^1);
            App::userSettings->saveUserSettings();
        }
        return(true);
    }
    if (commandID==SHOW_C_API_ACCESS_DEBUG_CMD)
    {
        IF_UI_EVENT_CAN_READ_DATA_CMD("SHOW_C_API_ACCESS_DEBUG_CMD")
        {
            CFuncDebug::setDebugMask(CFuncDebug::getDebugMask()^2);
            App::userSettings->saveUserSettings();
        }
        return(true);
    }
    if (commandID==SHOW_LUA_API_ACCESS_DEBUG_CMD)
    {
        IF_UI_EVENT_CAN_READ_DATA_CMD("SHOW_LUA_API_ACCESS_DEBUG_CMD")
        {
            CFuncDebug::setDebugMask(CFuncDebug::getDebugMask()^4);
            App::userSettings->saveUserSettings();
        }
        return(true);
    }
    if (commandID==SHOW_EASYLOCK_ACTIVITY_DEBUG_CMD)
    {
        IF_UI_EVENT_CAN_READ_DATA_CMD("SHOW_EASYLOCK_ACTIVITY_DEBUG_CMD")
        {
            CEasyLock::setShowActivity(!CEasyLock::getShowActivity());
            App::userSettings->saveUserSettings();
        }
        return(true);
    }
    if (commandID==SHOW_THREAD_SWITCHES_DEBUG_CMD)
    {
        IF_UI_EVENT_CAN_READ_DATA_CMD("SHOW_THREAD_SWITCHES_DEBUG_CMD")
        {
            CThreadPool::setShowThreadSwitches(!CThreadPool::getShowThreadSwitches());
            App::userSettings->saveUserSettings();
        }
        return(true);
    }
    if (commandID==DEBUG_TO_FILE_DEBUG_CMD)
    {
        IF_UI_EVENT_CAN_READ_DATA_CMD("DEBUG_TO_FILE_DEBUG_CMD")
        {
            CDebugLogFile::setDebugToFile(!CDebugLogFile::getDebugToFile());
            App::userSettings->saveUserSettings();
        }
        return(true);
    }
    if (commandID==DISABLE_FRUSTUM_CULLING_DEBUG_CMD)
    {
        IF_UI_EVENT_CAN_READ_DATA_CMD("DISABLE_FRUSTUM_CULLING_DEBUG_CMD")
        {
            CViewableBase::setFrustumCullingEnabled(!CViewableBase::getFrustumCullingEnabled());
        }
        return(true);
    }
    if (commandID==DISABLE_DISTANCE_CACHING_DEBUG_CMD)
    {
        IF_UI_EVENT_CAN_READ_DATA_CMD("DISABLE_DISTANCE_CACHING_DEBUG_CMD")
        {
            CDistanceRoutine::setDistanceCachingEnabled(!CDistanceRoutine::getDistanceCachingEnabled());
        }
        return(true);
    }
    if (commandID==VISUALIZE_OBB_STRUCTURE_DEBUG_CMD)
    {
        IF_UI_EVENT_CAN_READ_DATA_CMD("VISUALIZE_OBB_STRUCTURE_DEBUG_CMD")
        {
            CShape::setDebugObbStructures(!CShape::getDebugObbStructures());
        }
        return(true);
    }

    return(false);
}
