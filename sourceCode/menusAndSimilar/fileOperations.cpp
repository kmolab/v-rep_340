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
#include "v_rep_internal.h"
#include "fileOperations.h"
#include "simulation.h"
#include "tt.h"
#include "dxfFile.h"
#include "objFile.h"
#include "stlFile.h"
#include "xmlSer.h"
#include "persistentDataContainer.h"
#include "sceneObjectOperations.h"
#include "algos.h"
#include "app.h"
#include "pluginContainer.h"
#include "meshManip.h"
#include "geometric.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>
#include "imgLoaderSaver.h"
#include "vVarious.h"
#include "vDateTime.h"
#include "ttUtil.h"
#ifdef SIM_WITH_GUI
    #include "vFileDialog.h"
    #include "vMessageBox.h"
#endif

bool CFileOperations::processCommand(int commandID)
{ // Return value is true if the command belonged to file menu and was executed
    if ( (commandID>FILE_OPERATION_START_FOCMD)&&(commandID<FILE_OPERATION_END_FOCMD) )
    {
        SSimulationThreadCommand cmd;
        cmd.cmdId=commandID;
        App::appendSimulationThreadCommand(cmd);
        return(true);
    }
    return(false);
}

bool CFileOperations::processCommand(const SSimulationThreadCommand& cmd)
{
    //-----------
    if (cmd.cmdId==FILE_OPERATION_NEW_SCENE_FOCMD)
    {
        SSimulationThreadCommand cmd2;
        cmd2.cmdId=FILE_OPERATION_NEW_SCENE_PHASE2_FOCMD;
#ifdef SIM_WITH_GUI
        if ( (App::mainWindow!=NULL)&&(!App::userSettings->doNotShowSceneSelectionThumbnails) )
            App::mainWindow->prepareSceneThumbnail(cmd2);
        else
#endif
            App::appendSimulationThreadCommand(cmd2);
        return(true);
    }
    if (cmd.cmdId==FILE_OPERATION_NEW_SCENE_PHASE2_FOCMD)
    { // Cannot undo this command
        if ( App::ct->simulation->isSimulationStopped()&&(App::getEditModeType()==NO_EDIT_MODE) )
        { // execute the command only when simulation is not running and not in an edit mode
            if (!VThread::isCurrentThreadTheUiThread())
            { // we are NOT in the UI thread. We execute the command now:
                createNewScene(true,false);
            }
            else
                App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        }
        return(true);
    }
    //-----------

    if (cmd.cmdId==FILE_OPERATION_CLOSE_SCENE_FOCMD)
    { // Cannot undo this command
        if (App::ct->simulation->isSimulationStopped()&&(App::getEditModeType()==NO_EDIT_MODE) )
        { // execute the command only when simulation is not running and not in an edit mode
            if (!VThread::isCurrentThreadTheUiThread())
            { // we are NOT in the UI thread. We execute the command now:
                closeScene(true,true);
            }
            else
                App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        }
        return(true);
    }

    //-----------
#ifdef SIM_WITH_GUI
    if (cmd.cmdId==FILE_OPERATION_OPEN_SCENE_FOCMD)
    {
        SSimulationThreadCommand cmd2;
        cmd2.cmdId=FILE_OPERATION_OPEN_SCENE_PHASE2_FOCMD;
        if ( (App::mainWindow!=NULL)&&(!App::userSettings->doNotShowSceneSelectionThumbnails) )
            App::mainWindow->prepareSceneThumbnail(cmd2);
        else
            App::appendSimulationThreadCommand(cmd2);
        return(true);
    }
    if (cmd.cmdId==FILE_OPERATION_OPEN_SCENE_PHASE2_FOCMD)
    {
        if ( App::ct->simulation->isSimulationStopped()&&(App::getEditModeType()==NO_EDIT_MODE) )
        { // execute the command only when simulation is not running and not in an edit mode
            if (!VThread::isCurrentThreadTheUiThread())
            { // we are NOT in the UI thread. We execute the command now:
                App::addStatusbarMessage(tt::decorateString("",IDSNS_LOADING_SCENE,"...").c_str());

                std::string tst(App::directories->sceneDirectory);
                std::string filenameAndPath=App::uiThread->getOpenFileName(App::mainWindow,0,strTranslate(IDSN_LOADING_SCENE),tst,"",false,"V-REP Scenes",VREP_SCENE_EXTENSION);

                if (filenameAndPath.length()!=0)
                {
                    bool useNewInstance=(App::ct->undoBufferContainer->isSceneSaveMaybeNeededFlagSet()||(App::ct->mainSettings->getScenePathAndName()!=""))&&(!App::ct->environment->getSceneCanBeDiscardedWhenNewSceneOpened());
                    App::setRebuildHierarchyFlag();
                    App::setDefaultMouseMode();
                    if (useNewInstance)
                    {
                        App::ct->createNewInstance();
                        createNewScene(true,false);
                    }
                    else
                    {
                        if (App::mainWindow!=NULL)
                            App::mainWindow->scintillaEditorContainer->closeAllEditors();
                        App::ct->simulation->stopSimulation();
                        App::ct->emptyScene(true);
                    }
                    if (loadScene(filenameAndPath.c_str(),true,true,true))
                        addToRecentlyOpenedScenes(filenameAndPath);
                    else
                        _removeFromRecentlyOpenedScenes(filenameAndPath);
                }
                else
                    App::addStatusbarMessage(IDSNS_ABORTED);
                App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
            }
            else
                App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        }
        return(true);
    }
    //-----------

    //-----------
    if ((cmd.cmdId>=FILE_OPERATION_OPEN_RECENT_SCENE0_FOCMD)&&(cmd.cmdId<=FILE_OPERATION_OPEN_RECENT_SCENE9_FOCMD))
    {
        SSimulationThreadCommand cmd2;
        cmd2.cmdId=cmd.cmdId-FILE_OPERATION_OPEN_RECENT_SCENE0_FOCMD+FILE_OPERATION_OPEN_RECENT_SCENE0_PHASE2_FOCMD;
        if ( (App::mainWindow!=NULL)&&(!App::userSettings->doNotShowSceneSelectionThumbnails) )
            App::mainWindow->prepareSceneThumbnail(cmd2);
        else
            App::appendSimulationThreadCommand(cmd2);
        return(true);
    }
    if ((cmd.cmdId>=FILE_OPERATION_OPEN_RECENT_SCENE0_PHASE2_FOCMD)&&(cmd.cmdId<=FILE_OPERATION_OPEN_RECENT_SCENE9_PHASE2_FOCMD))
    {
        if (App::ct->simulation->isSimulationStopped()&&(App::getEditModeType()==NO_EDIT_MODE) )
        { // execute the command only when simulation is not running and not in an edit mode
            if (!VThread::isCurrentThreadTheUiThread())
            { // we are NOT in the UI thread. We execute the command now:
                CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
                std::string filenameAndPath;
                int recentScenesCnt=0;
                for (int i=0;i<10;i++)
                {
                    std::string tmp("SIMSETTINGS_RECENTSCENE0");
                    tmp[23]=48+i;
                    cont.readData(tmp.c_str(),filenameAndPath);
                    if (filenameAndPath.length()>3)
                        recentScenesCnt++;
                    if (recentScenesCnt==cmd.cmdId-FILE_OPERATION_OPEN_RECENT_SCENE0_PHASE2_FOCMD+1)
                        break;
                }

                if (VFile::doesFileExist(filenameAndPath))
                {
                    bool useNewInstance=(App::ct->undoBufferContainer->isSceneSaveMaybeNeededFlagSet()||(App::ct->mainSettings->getScenePathAndName()!=""))&&(!App::ct->environment->getSceneCanBeDiscardedWhenNewSceneOpened());
                    App::setDefaultMouseMode();
                    if (useNewInstance)
                    {
                        App::ct->createNewInstance();
                        CFileOperations::createNewScene(true,false);
                    }
                    else
                    {
                        if (App::mainWindow!=NULL)
                            App::mainWindow->scintillaEditorContainer->closeAllEditors();
                        App::ct->simulation->stopSimulation();
                        App::ct->emptyScene(true);
                    }

                    if (loadScene(filenameAndPath.c_str(),true,true,true))
                        addToRecentlyOpenedScenes(filenameAndPath);
                    else
                        _removeFromRecentlyOpenedScenes(filenameAndPath);
                    App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
                }
                else
                { // file does not exist anymore
                    App::uiThread->messageBox_information(App::mainWindow,"Open Recent Scene","File does not exist anymore.",VMESSAGEBOX_OKELI);
                    _removeFromRecentlyOpenedScenes(filenameAndPath);
                }
            }
            else
                App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        }
        return(true);
    }
    //-----------

    if (cmd.cmdId==FILE_OPERATION_LOAD_MODEL_FOCMD)
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            std::string tst(App::directories->modelDirectory);
            std::string filenameAndPath=App::uiThread->getOpenFileName(App::mainWindow,0,strTranslate(IDSN_LOADING_MODEL),tst,"",false,"V-REP Models",VREP_MODEL_EXTENSION);
            if (filenameAndPath.length()!=0)
                loadModel(filenameAndPath.c_str(),true,true,true,false,NULL,true); // Undo things is in here.
            else
                App::addStatusbarMessage(IDSNS_ABORTED);
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        return(true);
    }
    if (cmd.cmdId==FILE_OPERATION_LOAD_UI_FOCMD)
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            std::string tst(App::directories->uiDirectory);
            std::string filenameAndPath=App::uiThread->getOpenFileName(App::mainWindow,0,strTranslate(IDSN_LOADING_UI),tst,"",false,"V-REP Custom UI Files","ttb");
            if (filenameAndPath.length()!=0)
                loadUserInterfaces(filenameAndPath.c_str(),true,true,true,NULL,true); // Undo thing done here too
            else
                App::addStatusbarMessage(IDSNS_ABORTED);
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        return(true);
    }

    if (cmd.cmdId==FILE_OPERATION_SAVE_SCENE_FOCMD)
    {
        if (App::ct->simulation->isSimulationStopped()&&(App::getEditModeType()==NO_EDIT_MODE) )
        { // execute the command only when simulation is not running and not in an edit mode
            if (!VThread::isCurrentThreadTheUiThread())
            { // we are NOT in the UI thread. We execute the command now:
                _saveSceneWithDialogAndEverything(); // will call save as if needed!
            }
            else
                App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        }
        return(true);
    }
    if (cmd.cmdId==FILE_OPERATION_EXPORT_IK_CONTENT_FOCMD)
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            if (!App::ct->environment->getSceneLocked())
            {
                App::addStatusbarMessage(IDSNS_EXPORTING_IK_CONTENT);
                std::string tst(App::directories->otherFilesDirectory);
                std::string filenameAndPath=App::uiThread->getSaveFileName(App::mainWindow,0,strTranslate(IDS_EXPORTING_IK_CONTENT___),tst,"",false,"V-REP IK Content Files","ik");
                if (filenameAndPath.length()!=0)
                {
                    App::directories->otherFilesDirectory=App::directories->getPathFromFull(filenameAndPath);
                    if (apiExportIkContent(filenameAndPath.c_str(),true))
                        App::addStatusbarMessage(IDSNS_DONE);
                    else
                        App::addStatusbarMessage(IDSNS_OPERATION_FAILED);
                }
                else
                    App::addStatusbarMessage(IDSNS_ABORTED);
            }
            else
                App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXPORT),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMESSAGEBOX_OKELI);
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        return(true);
    }
    if ( (cmd.cmdId==FILE_OPERATION_SAVE_SCENE_AS_FOCMD)||(cmd.cmdId==FILE_OPERATION_SAVE_SCENE_AS_BLUEREALITY_FOCMD) )
    {
        if (App::ct->simulation->isSimulationStopped()&&(App::getEditModeType()==NO_EDIT_MODE) )
        { // execute the command only when simulation is not running and not in an edit mode
            if (!VThread::isCurrentThreadTheUiThread())
            { // we are NOT in the UI thread. We execute the command now:
                _saveSceneAsWithDialogAndEverything(cmd.cmdId==FILE_OPERATION_SAVE_SCENE_AS_FOCMD);
            }
            else
                App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        }
        return(true);
    }
    if ( (cmd.cmdId==FILE_OPERATION_SAVE_MODEL_FOCMD)||(cmd.cmdId==FILE_OPERATION_SAVE_MODEL_BLUEREALITY_FOCMD) )
    {
        if (App::ct->simulation->isSimulationStopped()&&(App::getEditModeType()==NO_EDIT_MODE) )
        { // execute the command only when simulation is not running and not in an edit mode
            if (!VThread::isCurrentThreadTheUiThread())
            { // we are NOT in the UI thread. We execute the command now:
                std::vector<int> sel;
                for (int i=0;i<App::ct->objCont->getSelSize();i++)
                    sel.push_back(App::ct->objCont->getSelID(i));
                if (!App::ct->environment->getSceneLocked())
                {
                    std::string infoM(IDSNS_SAVING_MODEL);
                    infoM+="...";
                    App::addStatusbarMessage(infoM.c_str());
                    if (sel.size()!=0)
                    {
                        int modelBase=App::ct->objCont->getLastSelectionID();
                        if (App::userSettings->askToIncludeScriptFiles&&App::ct->luaScriptContainer->hasModelIncludeScripts(modelBase))
                        {
                            if (VMESSAGEBOX_REPLY_YES==App::uiThread->messageBox_question(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_INCLUDE_SCRIPT_FILES_QUESTION),VMESSAGEBOX_YES_NO))
                                App::ct->luaScriptContainer->setSaveIncludeScriptFiles(true);
                        }

                        // Display a warning if needed
                        CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
                        std::string val;
                        cont.readData("SIMSETTINGS_MODEL_SAVE_OFFSET_WARNING",val);
                        int intVal=0;
                        tt::getValidInt(val,intVal);
                        if (intVal<1)
                        {
                            if (App::uiThread->messageBox_checkbox(App::mainWindow,IDSN_MODEL,IDSN_MODEL_SAVE_POSITION_OFFSET_INFO,IDSN_DO_NOT_SHOW_THIS_MESSAGE_AGAIN))
                            {
                                intVal++;
                                val=tt::FNb(intVal);
                                cont.writeData("SIMSETTINGS_MODEL_SAVE_OFFSET_WARNING",val,!App::userSettings->doNotWritePersistentData);
                            }
                        }

                        bool keepCurrentThumbnail=false;
                        bool operationCancelled=false;
                        while (true)
                        {
                            if (App::ct->environment->modelThumbnail_notSerializedHere.hasImage())
                            { // we already have a thumbnail!
                                SUIThreadCommand cmdIn;
                                SUIThreadCommand cmdOut;
                                cmdIn.cmdId=KEEP_THUMBNAIL_QUESTION_DLG_UITHREADCMD;
                                App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
                                if (cmdOut.boolParams.size()>0)
                                    keepCurrentThumbnail=cmdOut.boolParams[0];
                            }
                            if (!keepCurrentThumbnail)
                            {
                                SUIThreadCommand cmdIn;
                                SUIThreadCommand cmdOut;
                                cmdIn.intParams.push_back(modelBase);
                                cmdIn.cmdId=SELECT_THUMBNAIL_DLG_UITHREADCMD;
                                App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
                                if (cmdOut.boolParams.size()>0)
                                {
                                    if (!cmdOut.boolParams[0])
                                    {
                                        if (cmdOut.boolParams[1])
                                            break;
                                    }
                                    else
                                    {
                                        operationCancelled=true;
                                        break;
                                    }
                                }
                                else
                                {
                                    operationCancelled=true;
                                    break;
                                }
                            }
                            else
                                break;
                        }
                        if (!operationCancelled)
                        {
                            std::string tst(App::directories->modelDirectory);
                            std::string filenameAndPath=App::uiThread->getSaveFileName(App::mainWindow,0,strTranslate(IDS_SAVING_MODEL___),tst,"",false,"V-REP Model",VREP_MODEL_EXTENSION);
                            if (filenameAndPath.length()!=0)
                                saveModel(modelBase,filenameAndPath.c_str(),true,true,true);
                            else
                                App::addStatusbarMessage(IDSNS_ABORTED);
                        }
                        else
                            App::addStatusbarMessage(IDSNS_ABORTED);
                        App::ct->luaScriptContainer->setSaveIncludeScriptFiles(false);

                    }
                    else
                        App::addStatusbarMessage(IDSNS_CANNOT_PROCEED_SELECTION_IS_EMPTY);
                }
                else
                    App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_MODEL),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMESSAGEBOX_OKELI);
            }
            else
                App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        }
        return(true);
    }
    if (cmd.cmdId==FILE_OPERATION_SAVE_UI_FOCMD)
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            if (!App::ct->environment->getSceneLocked())
            {
                App::addStatusbarMessage(IDSNS_SAVING_CUSTOM_USER_INTERFACES);
                std::string filenameAndPath=App::uiThread->getSaveFileName(App::mainWindow,0,strTranslate(IDSNS_SAVING_CUSTOM_USER_INTERFACES),"","",false,"V-REP OpenGl-based Custom UI Files","ttb");
                if (filenameAndPath.length()!=0)
                    saveUserInterfaces(filenameAndPath.c_str(),true,true,true,NULL);
                else
                    App::addStatusbarMessage(IDSNS_ABORTED);
            }
            else
                App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMESSAGEBOX_OKELI);
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        return(true);
    }
    if (cmd.cmdId==FILE_OPERATION_IMPORT_MESH_FOCMD)
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            App::addStatusbarMessage(IDS_IMPORTING_MESH___);
            App::ct->objCont->deselectObjects();
            std::string tst(App::directories->cadFormatDirectory);

            float scalingFactor=1.0f;
            bool showDlg=true;

            std::vector<std::string> filenamesAndPaths;

            App::uiThread->getOpenFileNames(filenamesAndPaths,App::mainWindow,0,IDS_IMPORTING_MESH___,tst,"",false,"Mesh files","obj","dxf","stl");
            bool prevImp=false;
            for (int i=0;i<int(filenamesAndPaths.size());i++)
            {
                std::string filenameAndPath=filenamesAndPaths[i];
                App::addStatusbarMessage((std::string("--> ")+filenameAndPath).c_str());
                if (VFile::doesFileExist(filenameAndPath))
                {
                    std::string ext(CTTUtil::getLowerCaseString(VVarious::splitPath_fileExtension(filenameAndPath).c_str()));
                    int fileFormat=FILE_FORMAT_OBJ;
                    if (ext.compare("dxf")==0)
                        fileFormat=FILE_FORMAT_DXF;
                    if (ext.compare("3ds")==0)
                        fileFormat=FILE_FORMAT_3DS;
                    if (ext.compare("stl")==0)
                        fileFormat=FILE_FORMAT_ANY_STL;
                    if (commonImportRoutine(fileFormat,filenameAndPath,showDlg,true,scalingFactor,-1,prevImp))
                        App::addStatusbarMessage(IDSNS_DONE);
                    else
                        App::addStatusbarMessage(IDSNS_AN_ERROR_OCCURRED_DURING_THE_IMPORT_OPERATION);
                    showDlg=false;
                    prevImp=true;
                }
                else
                    App::addStatusbarMessage(IDSNS_ABORTED_FILE_DOES_NOT_EXIST);
            }
            POST_SCENE_CHANGED_ANNOUNCEMENT(""); // ************************** UNDO thingy **************************
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        return(true);
    }
    if (cmd.cmdId==FILE_OPERATION_IMPORT_PATH_FOCMD)
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            App::addStatusbarMessage(IDSNS_IMPORTING_PATH_FROM_CSV_FILE);
            App::ct->objCont->deselectObjects();
            std::string tst(App::directories->cadFormatDirectory);
            std::string filenameAndPath=App::uiThread->getOpenFileName(App::mainWindow,0,strTranslate(IDS_IMPORTING_PATH_FROM_CSV_FILE),tst,"",false,"CSV Files","csv");
            if (filenameAndPath.length()!=0)
            {
                if (VFile::doesFileExist(filenameAndPath))
                {
                    if (_pathImportRoutine(filenameAndPath,true))
                        App::addStatusbarMessage(IDSNS_DONE);
                    else
                        App::addStatusbarMessage(IDSNS_AN_ERROR_OCCURRED_DURING_THE_IMPORT_OPERATION);
                    POST_SCENE_CHANGED_ANNOUNCEMENT(""); // ************************** UNDO thingy **************************
                }
                else
                    App::addStatusbarMessage(IDSNS_ABORTED_FILE_DOES_NOT_EXIST);
            }
            else
                App::addStatusbarMessage(IDSNS_ABORTED);
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        return(true);
    }

    if (cmd.cmdId==FILE_OPERATION_IMPORT_HEIGHTFIELD_FOCMD)
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            App::addStatusbarMessage(IDSNS_IMPORTING_HEIGHTFIELD_SHAPE);
            App::ct->objCont->deselectObjects();
            std::string tst(App::directories->cadFormatDirectory);
            std::string filenameAndPath=App::uiThread->getOpenFileName(App::mainWindow,0,strTranslate(IDS_IMPORTING_HEIGHTFIELD___),tst,"",true,"Image, CSV and TXT files","tga","jpg","jpeg","png","gif","bmp","tiff","csv","txt");

            if (filenameAndPath.length()!=0)
            {
                if (VFile::doesFileExist(filenameAndPath))
                {
                    if (heightfieldImportRoutine(filenameAndPath))
                        App::addStatusbarMessage(IDSNS_DONE);
                    else
                        App::addStatusbarMessage(IDSNS_AN_ERROR_OCCURRED_DURING_THE_IMPORT_OPERATION);
                    POST_SCENE_CHANGED_ANNOUNCEMENT(""); // ************************** UNDO thingy **************************
                }
                else
                    App::addStatusbarMessage(IDSNS_ABORTED_FILE_DOES_NOT_EXIST);
            }
            else
                App::addStatusbarMessage(IDSNS_ABORTED);
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        return(true);
    }

    if (cmd.cmdId==FILE_OPERATION_EXPORT_SHAPE_FOCMD)
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            std::vector<int> sel;
            for (int i=0;i<App::ct->objCont->getSelSize();i++)
                sel.push_back(App::ct->objCont->getSelID(i));
            if (!App::ct->environment->getSceneLocked())
            {
                App::addStatusbarMessage(IDSNS_EXPORTING_SHAPES);
                CSceneObjectOperations::addRootObjectChildrenToSelection(sel);
                if (0==App::ct->objCont->getShapeNumberInSelection(&sel))
                    return(true); // Selection contains nothing that can be exported!
                std::string tst(App::directories->cadFormatDirectory);
                std::string filenameAndPath=App::uiThread->getSaveFileName(App::mainWindow,0,strTranslate(IDSNS_EXPORTING_SHAPES),tst,"",false,"Mesh files","obj","dxf","stl");
                if (filenameAndPath.length()!=0)
                {
                    App::directories->cadFormatDirectory=App::directories->getPathFromFull(filenameAndPath);

                    std::string ext(CTTUtil::getLowerCaseString(VVarious::splitPath_fileExtension(filenameAndPath).c_str()));
                    bool error=true;
                    if (ext.compare("obj")==0)
                    {
                        CObjFile exporter;
                        error=!exporter.exportFunc(filenameAndPath,false,&sel);
                    }
                    if (ext.compare("dxf")==0)
                    {
                        CDxfFile dxf;
                        error=!dxf.exportFunc(filenameAndPath,true,&sel);
                    }
                    if (ext.compare("stl")==0)
                    {
                        CStlFile exporter(true);
                        error=!exporter.exportFunc(filenameAndPath,false,&sel);
                    }
                    App::ct->objCont->deselectObjects();
                    if (error)
                        App::addStatusbarMessage(IDSNS_AN_ERROR_OCCURRED_DURING_THE_EXPORT_OPERATION);
                    else
                        App::addStatusbarMessage(IDSNS_DONE);
                }
                else
                    App::addStatusbarMessage(IDSNS_ABORTED);
            }
            else
                App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXPORT),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMESSAGEBOX_OKELI);
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        return(true);
    }
    if (cmd.cmdId==FILE_OPERATION_EXPORT_GRAPHS_FOCMD)
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            std::vector<int> sel;
            for (int i=0;i<App::ct->objCont->getSelSize();i++)
                sel.push_back(App::ct->objCont->getSelID(i));
            App::addStatusbarMessage(IDSNS_EXPORTING_GRAPH_DATA);
            App::ct->simulation->stopSimulation();
            if (App::ct->objCont->getGraphNumberInSelection(&sel)!=0)
            {
                std::string tst(App::directories->otherFilesDirectory);
                std::string filenameAndPath=App::uiThread->getSaveFileName(App::mainWindow,0,strTranslate(IDS_SAVING_GRAPHS___),tst,"",false,"CSV Files","csv");
                if (filenameAndPath.length()!=0)
                {
                    VFile myFile(filenameAndPath.c_str(),VFile::CREATE_WRITE|VFile::SHARE_EXCLUSIVE);
                    VArchive ar(&myFile,VArchive::STORE);
                    App::directories->otherFilesDirectory=App::directories->getPathFromFull(filenameAndPath);
                    for (int i=0;i<int(sel.size());i++)
                    {
                        CGraph* it=App::ct->objCont->getGraph(sel[i]);
                        if (it!=NULL)
                            it->exportGraphData(ar);
                    }
                    ar.close();
                    myFile.close();
                    App::addStatusbarMessage(IDSNS_DONE);
                }
                else
                    App::addStatusbarMessage(IDSNS_ABORTED);
            }
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        return(true);
    }

    if ( (cmd.cmdId==FILE_OPERATION_EXPORT_PATH_SIMPLE_POINTS_FOCMD)||(cmd.cmdId==FILE_OPERATION_EXPORT_PATH_BEZIER_POINTS_FOCMD) )
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            std::vector<int> sel;
            for (int i=0;i<App::ct->objCont->getSelSize();i++)
                sel.push_back(App::ct->objCont->getSelID(i));
            if (App::ct->objCont->isLastSelectionAPath(&sel))
            {
                CPath* it=(CPath*)App::ct->objCont->getLastSelection(&sel);
                if (it->pathContainer->getSimplePathPointCount()!=0)
                {
                    if (cmd.cmdId==FILE_OPERATION_EXPORT_PATH_SIMPLE_POINTS_FOCMD)
                        App::addStatusbarMessage(IDSNS_EXPORTING_PATH);
                    else
                        App::addStatusbarMessage(IDSNS_EXPORTING_PATHS_BEZIER_CURVE);
                    App::ct->simulation->stopSimulation();
                    std::string titleString;
                    if (cmd.cmdId==FILE_OPERATION_EXPORT_PATH_SIMPLE_POINTS_FOCMD)
                        titleString=strTranslate(IDS_EXPORTING_PATH___);
                    else
                        titleString=strTranslate(IDS_EXPORTING_PATH_BEZIER_CURVE___);
                    std::string filenameAndPath=App::uiThread->getSaveFileName(App::mainWindow,0,titleString,App::directories->executableDirectory,"",false,"CSV Files","csv");
                    if (filenameAndPath.length()!=0)
                    {
                        _pathExportPoints(filenameAndPath,it->getID(),cmd.cmdId==FILE_OPERATION_EXPORT_PATH_BEZIER_POINTS_FOCMD,true);
                        App::addStatusbarMessage(IDSNS_DONE);
                    }
                    else
                        App::addStatusbarMessage(IDSNS_ABORTED);
                }
                else
                    App::addStatusbarMessage(IDSNS_CANNOT_EXPORT_AN_EMPTY_PATH);
            }
            else
                App::addStatusbarMessage(IDSNS_LAST_SELECTION_IS_NOT_A_PATH);
            App::ct->objCont->deselectObjects();
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        return(true);
    }
    if (cmd.cmdId==FILE_OPERATION_EXPORT_DYNAMIC_CONTENT_FOCMD)
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            if (!App::ct->environment->getSceneLocked())
            {
                App::addStatusbarMessage(IDSNS_EXPORTING_DYNAMIC_CONTENT);
                if (CPluginContainer::dyn_isDynamicContentAvailable()!=0)
                {
                    int eng=App::ct->dynamicsContainer->getDynamicEngineType(NULL);
                    if (eng==sim_physics_ode)
                    {
                        std::string tst(App::directories->otherFilesDirectory);
                        std::string filenameAndPath=App::uiThread->getSaveFileName(App::mainWindow,0,strTranslate(IDS_EXPORTING_DYNAMIC_CONTENT___),tst,"",false,"ODE Dynamics World Files","ode");
                        if (filenameAndPath.length()!=0)
                        {
                            App::directories->otherFilesDirectory=App::directories->getPathFromFull(filenameAndPath);
                            CPluginContainer::dyn_serializeDynamicContent(filenameAndPath.c_str(),0);
                            App::addStatusbarMessage(IDSNS_DONE);
                        }
                        else
                            App::addStatusbarMessage(IDSNS_ABORTED);
                    }
                    if (eng==sim_physics_bullet)
                    {
                        std::string tst(App::directories->otherFilesDirectory);
                        std::string filenameAndPath=App::uiThread->getSaveFileName(App::mainWindow,0,strTranslate(IDS_EXPORTING_DYNAMIC_CONTENT___),tst,"",false,"Bullet Dynamics World Files","bullet");
                        if (filenameAndPath.length()!=0)
                        {
                            App::directories->otherFilesDirectory=App::directories->getPathFromFull(filenameAndPath);
                            CPluginContainer::dyn_serializeDynamicContent(filenameAndPath.c_str(),App::userSettings->bulletSerializationBuffer);
                            App::addStatusbarMessage(IDSNS_DONE);
                        }
                        else
                            App::addStatusbarMessage(IDSNS_ABORTED);
                    }
                    if (eng==sim_physics_vortex)
                    {
                        std::string tst(App::directories->otherFilesDirectory);
                        std::string filenameAndPath=App::uiThread->getSaveFileName(App::mainWindow,0,strTranslate(IDS_EXPORTING_DYNAMIC_CONTENT___),tst,"",false,"Vortex Dynamics World Files","vortex");
                        if (filenameAndPath.length()!=0)
                        {
                            App::directories->otherFilesDirectory=App::directories->getPathFromFull(filenameAndPath);
                            CPluginContainer::dyn_serializeDynamicContent(filenameAndPath.c_str(),App::userSettings->bulletSerializationBuffer);
                            App::addStatusbarMessage(IDSNS_DONE);
                        }
                        else
                            App::addStatusbarMessage(IDSNS_ABORTED);
                    }
                    if (eng==sim_physics_newton)
                    {
                        std::string tst(App::directories->otherFilesDirectory);
                        std::string filenameAndPath=App::uiThread->getSaveFileName(App::mainWindow,0,strTranslate(IDS_EXPORTING_DYNAMIC_CONTENT___),tst,"",false,"Newton Dynamics World Files","newton");
                        if (filenameAndPath.length()!=0)
                        {
                            App::directories->otherFilesDirectory=App::directories->getPathFromFull(filenameAndPath);
                            CPluginContainer::dyn_serializeDynamicContent(filenameAndPath.c_str(),App::userSettings->bulletSerializationBuffer);
                            App::addStatusbarMessage(IDSNS_DONE);
                        }
                        else
                            App::addStatusbarMessage(IDSNS_ABORTED);
                    }
                }
                else
                    App::addStatusbarMessage(IDSNS_CANNOT_PROCEED_NO_DYNAMIC_CONTENT_AVAILABLE);
            }
            else
                App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXPORT),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMESSAGEBOX_OKELI);
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread
        return(true);
    }
#endif

    if (cmd.cmdId==FILE_OPERATION_EXIT_SIMULATOR_FOCMD)
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            bool displayed=false;
#ifdef SIM_WITH_GUI
            int ci=-1;
            int si=-1;
            int ei=-1;
            if (App::mainWindow!=NULL)
                App::mainWindow->simulationRecorder->stopRecording(false);

            ci=App::ct->getInstanceIndexOfASceneNotYetSaved(App::ct->environment->getSceneLocked());
            if (!App::ct->simulation->isSimulationStopped())
                si=App::ct->getCurrentInstanceIndex();
            if (App::getEditModeType()!=NO_EDIT_MODE)
                ei=App::ct->getCurrentInstanceIndex();

            if (!App::ct->environment->getSceneLocked())
            {
                if (App::ct->undoBufferContainer->isSceneSaveMaybeNeededFlagSet())
                    ci=App::ct->getCurrentInstanceIndex();
            }
            if ((ei==App::ct->getCurrentInstanceIndex())&&(!displayed))
            {
                if (VMESSAGEBOX_REPLY_OK==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXIT),strTranslate(IDS_INSTANCE_STILL_IN_EDIT_MODE_MESSAGE),VMESSAGEBOX_OK_CANCEL))
                {
                    if (App::mainWindow!=NULL)
                        App::mainWindow->editModeContainer->processCommand(ANY_EDIT_MODE_FINISH_AND_CANCEL_CHANGES_EMCMD,NULL);
                    ei=-1;
                }
                else
                    displayed=true;
            }
            if ((si==App::ct->getCurrentInstanceIndex())&&(!displayed))
            {
                if (VMESSAGEBOX_REPLY_OK==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXIT),strTranslate(IDS_SIMULATION_STILL_RUNNING_MESSAGE),VMESSAGEBOX_OK_CANCEL))
                    App::ct->simulatorMessageQueue->addCommand(sim_message_simulation_stop_request,0,0,0,0,NULL,0);
                displayed=true;
            }
            if ((ci==App::ct->getCurrentInstanceIndex())&&(!displayed))
            {
                unsigned short action=VMESSAGEBOX_REPLY_NO;
                action=App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_WANNA_SAVE_THE_SCENE_WARNING),VMESSAGEBOX_YES_NO_CANCEL);
                if (action==VMESSAGEBOX_REPLY_YES)
                {
                    if (_saveSceneWithDialogAndEverything()) // will call save as if needed!
                        action=VMESSAGEBOX_REPLY_NO;
                }
                if (action==VMESSAGEBOX_REPLY_NO)
                {
                    App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
                    ci=App::ct->getInstanceIndexOfASceneNotYetSaved(false);
                }
                else
                    displayed=true;
            }
            if ((ei!=-1)&&(!displayed))
            {
                App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXIT),strTranslate(IDS_ANOTHER_INSTANCE_STILL_IN_EDIT_MODE_MESSAGE),VMESSAGEBOX_OKELI);
                App::ct->setInstanceIndex(ei);
                displayed=true;
            }
            if ((si!=-1)&&(!displayed))
            {
                App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXIT),strTranslate(IDS_ANOTHER_SIMULATION_STILL_RUNNING_MESSAGE),VMESSAGEBOX_OKELI);
                App::ct->setInstanceIndex(si);
                displayed=true;
            }
            if ((ci!=-1)&&(!displayed))
            {
                if (VMESSAGEBOX_REPLY_CANCEL==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_ANOTHER_INSTANCE_STILL_NOT_SAVED_WANNA_LEAVE_ANYWAY_MESSAGE),VMESSAGEBOX_OK_CANCEL))
                {
                    App::ct->setInstanceIndex(ci);
                    displayed=true;
                }
            }
#endif
            if (!displayed)
            {
                App::addStatusbarMessage(IDSNS_LEAVING);
                SSimulationThreadCommand cmd;
                cmd.cmdId=FINAL_EXIT_REQUEST_CMD;
                App::appendSimulationThreadCommand(cmd);
            }
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread

        return(true);
    }

    if (cmd.cmdId==FILE_OPERATION_OPEN_DRAG_AND_DROP_MODEL_FOCMD)
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            CFileOperations::loadModel(cmd.stringParams[0].c_str(),true,cmd.boolParams[0],false,true,NULL,cmd.boolParams[1]);
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread:
        return(true);
    }
    if (cmd.cmdId==FILE_OPERATION_OPEN_DRAG_AND_DROP_UI_FOCMD)
    {
        if (!VThread::isCurrentThreadTheUiThread())
        { // we are NOT in the UI thread. We execute the command now:
            CFileOperations::loadUserInterfaces(cmd.stringParams[0].c_str(),true,cmd.boolParams[0],false,NULL,cmd.boolParams[1]);
        }
        else
            App::appendSimulationThreadCommand(cmd); // We are in the UI thread. Execute the command via the main thread:
        return(true);
    }
    return(false);
}


void CFileOperations::createNewScene(bool displayMessages,bool forceForNewInstance)
{
    FUNCTION_DEBUG;
    bool useNewInstance=false;
    useNewInstance=(App::ct->undoBufferContainer->isSceneSaveMaybeNeededFlagSet()||(App::ct->mainSettings->getScenePathAndName()!=""))&&(!App::ct->environment->getSceneCanBeDiscardedWhenNewSceneOpened());
    if (forceForNewInstance)
        useNewInstance=true;
    App::setDefaultMouseMode();
    if (useNewInstance)
        App::ct->createNewInstance();
    else
    {
#ifdef SIM_WITH_GUI
        if (App::mainWindow!=NULL)
            App::mainWindow->scintillaEditorContainer->closeAllEditors();
#endif
        App::ct->simulation->stopSimulation();
    }
    App::ct->emptyScene(true);
    std::string fullPathAndFilename=App::directories->systemDirectory+VREP_SLASH;
    fullPathAndFilename+="dfltscn.";
    fullPathAndFilename+=VREP_SCENE_EXTENSION;
    loadScene(fullPathAndFilename.c_str(),false,false,false);
    App::ct->mainSettings->setScenePathAndName("");//savedLoc;
    if (displayMessages)
        App::addStatusbarMessage(IDSNS_DEFAULT_SCENE_WAS_SET_UP);
    App::ct->undoBufferContainer->memorizeState(); // so that we can come back to the initial state!
    App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
    App::ct->objCont->setDefaultSceneID(App::ct->objCont->computeSceneID());
}

void CFileOperations::closeScene(bool displayMessages,bool displayDialogs)
{
    FUNCTION_DEBUG;
    unsigned short action=VMESSAGEBOX_REPLY_NO;
#ifdef SIM_WITH_GUI
    if (displayMessages&&(!App::ct->environment->getSceneCanBeDiscardedWhenNewSceneOpened()))
    {
        if (!App::ct->environment->getSceneLocked())
        {
            if (displayDialogs&&App::ct->undoBufferContainer->isSceneSaveMaybeNeededFlagSet())
            {
                action=App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_WANNA_SAVE_THE_SCENE_WARNING),VMESSAGEBOX_YES_NO_CANCEL);
                if (action==VMESSAGEBOX_REPLY_YES)
                {
                    if (_saveSceneWithDialogAndEverything()) // will call save as if needed!
                        action=VMESSAGEBOX_REPLY_NO;
                }
            }
        }
    }
#endif
    if (action==VMESSAGEBOX_REPLY_NO)
    {
#ifdef SIM_WITH_GUI
        App::setDefaultMouseMode();
        if (App::mainWindow!=NULL)
            App::mainWindow->scintillaEditorContainer->closeAllEditors();
#endif
        App::ct->simulation->stopSimulation();
#ifdef SIM_WITH_GUI
        if (App::mainWindow!=NULL)
            App::mainWindow->editModeContainer->processCommand(ANY_EDIT_MODE_FINISH_AND_CANCEL_CHANGES_EMCMD,NULL);
        if (displayDialogs)
            App::uiThread->showOrHideProgressBar(true,-1,"Closing scene...");
#endif
        App::ct->emptyScene(true);
        if (displayDialogs)
            App::uiThread->showOrHideProgressBar(false);
        if (App::ct->getInstanceCount()>1)
        { // remove this instance:
            App::ct->destroyCurrentInstance();
        }
        else
        { // simply set-up an empty (default) scene
            std::string savedLoc=App::ct->mainSettings->getScenePathAndName();
            std::string fullPathAndFilename=App::directories->systemDirectory+VREP_SLASH;
            fullPathAndFilename+="dfltscn.";
            fullPathAndFilename+=VREP_SCENE_EXTENSION;
            loadScene(fullPathAndFilename.c_str(),false,false,false);
            App::ct->mainSettings->setScenePathAndName(savedLoc.c_str());
            App::addStatusbarMessage(IDSNS_DEFAULT_SCENE_WAS_SET_UP);
            App::ct->undoBufferContainer->memorizeState(); // so that we can come back to the initial state!
            App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
            App::ct->objCont->setDefaultSceneID(App::ct->objCont->computeSceneID());
        }
    }
    App::setRebuildHierarchyFlag();
}

bool CFileOperations::commonImportRoutine(int importType,const std::string& pathName,bool showDlg,bool showProgress,float& sf,int options,bool useOrientSizeFromPreviousImport)
{ // Should only be called by the main simulation thread!
    // options==-1 --> default
    // bit0 set --> keep identical vertices
    // bit1 set --> keep identival triangles
    // bit2 set --> not used
    // bit3 set --> not used
    // bit4 set --> not used
    // bit5 set --> not used
    FUNCTION_DEBUG;

    bool keepIdenticalVertices=0;
    bool keepIdenticalTriangles=0;
    if (options==-1)
    {
        keepIdenticalVertices=App::userSettings->identicalVerticesCheck;
        keepIdenticalTriangles=App::userSettings->identicalTrianglesCheck;
    }
    else
    {
        keepIdenticalVertices=((options&1)!=0);
        keepIdenticalTriangles=((options&2)!=0);
    }

    CImportExport* importer=NULL;
    App::directories->cadFormatDirectory=App::directories->getPathFromFull(pathName);
    if (importType==FILE_FORMAT_DXF)
        importer=new CDxfFile();
    if (importType==FILE_FORMAT_OBJ)
        importer=new CObjFile();
    if ((importType==FILE_FORMAT_ASCII_STL)||(importType==FILE_FORMAT_BINARY_STL)||(importType==FILE_FORMAT_ANY_STL))
        importer=new CStlFile(importType);

    if (showProgress)
        App::uiThread->showOrHideProgressBar(true,0.05f,"Reading file...");

    std::vector<std::vector<float>*> aVertices;
    std::vector<std::vector<int>*> aIndices;
    std::vector<std::vector<float>*> aTextureCoordinates;
    std::vector<std::vector<float>*> aNormals;
    std::vector<SObjMaterial*> aMaterials;
    std::vector<std::string> groupNames;
    bool error=!importer->importFunc(pathName);

    if (showProgress)
        App::uiThread->showOrHideProgressBar(true,0.2f,"Verifying meshes...");

    for (int i=0;i<importer->getGroupCount();i++)
    {
        if ((importer->getVertexCount(i)>=3)&&(importer->getTriangleCount(i)>=1))
        { 
            std::vector<float>* vert=new std::vector<float>;
            std::vector<int>* ind=new std::vector<int>;
            std::vector<float>* tex=NULL;
            std::vector<float>* norm=NULL;
            SObjMaterial* mat=NULL;
            for (int j=0;j<importer->getVertexCount(i);j++)
            {
                float vertex[3];
                importer->getVertex(i,j,vertex);
                vert->push_back(vertex[0]);
                vert->push_back(vertex[1]);
                vert->push_back(vertex[2]);
            }
            for (int j=0;j<importer->getTriangleCount(i);j++)
            {
                int triangle[3];
                importer->getTriangle(i,j,triangle);
                ind->push_back(triangle[0]);
                ind->push_back(triangle[1]);
                ind->push_back(triangle[2]);
            }
            if (importer->getTextureCoordinateCount(i)==importer->getTriangleCount(i)*3)
            {
                tex=new std::vector<float>;
                for (int j=0;j<importer->getTextureCoordinateCount(i);j++)
                {
                    float coord[2];
                    importer->getTextureCoordinate(i,j,coord);
                    tex->push_back(coord[0]);
                    tex->push_back(coord[1]);
                }
            }
            if (importer->getNormalsCount(i)==importer->getTriangleCount(i)*3)
            {
                norm=new std::vector<float>;
                for (int j=0;j<importer->getNormalsCount(i);j++)
                {
                    float n[3];
                    importer->getNormal(i,j,n);
                    norm->push_back(n[0]);
                    norm->push_back(n[1]);
                    norm->push_back(n[2]);
                }
            }
            mat=importer->getAndClearMaterial(i);

            CMeshManip::checkVerticesIndicesNormalsTexCoords(*vert,*ind,norm,tex,keepIdenticalVertices,App::userSettings->identicalVerticesTolerance/sf,keepIdenticalTriangles);
            if ((norm!=NULL)&&(norm->size()==0))
            {
                delete norm;
                norm=NULL;
            }
            if ((tex!=NULL)&&(tex->size()==0))
            {
                delete tex;
                tex=NULL;
            }

            if ((vert->size()>=9)&&(ind->size()>=3))
            {
                aVertices.push_back(vert);
                aIndices.push_back(ind);
                aNormals.push_back(norm);
                aTextureCoordinates.push_back(tex);
                std::string tmpStr(importer->getName(i));
                tt::removeIllegalCharacters(tmpStr,false);
                groupNames.push_back(tmpStr);
                aMaterials.push_back(mat);
            }
            else
            {
                delete vert;
                delete ind;
                delete norm;
                delete tex;
                delete norm;
                delete mat;
            }
        }
    }
    delete importer;

    if (showProgress)
        App::uiThread->showOrHideProgressBar(true,0.4f,"Building shape(s)...");

    static float prevTotalScale=1.0f;
    static bool prevZIsUp=true;
    static int prevScaleTextureTo=-1;
    float performedScaling=1.0f;
    bool zIsUp=true;
    int invScaling=1;

    if (!useOrientSizeFromPreviousImport)
    {
        C3Vector minV,maxV;
        C3Vector minVRot,maxVRot;
        float underZCounter=0;
        float underZCounter2=0;
        C4X4Matrix tr;
        tr.setIdentity();
        tr.M.buildXRotation(piValD2);
        for (int i=0;i<int(aVertices.size());i++)
        {
            for (int j=0;j<int(aVertices[i]->size())/3;j++)
            {
                C3Vector v(&aVertices[i]->at(3*j));
                if ((i==0)&&(j==0))
                {
                    minV=v;
                    maxV=v;
                    v*=tr;
                    minVRot=v;
                    maxVRot=v;
                }
                else
                {
                    minV.keepMin(v);
                    maxV.keepMax(v);
                    float vz=v(2);
                    v*=tr;
                    minVRot.keepMin(v);
                    maxVRot.keepMax(v);
                    if (v(2)<vz)
                        underZCounter2++;
                    else
                        underZCounter++;
                }
            }
        }

        if (underZCounter>underZCounter2)
        { // the imported scene was probably using a coord. system with Y up!
            minV=minVRot;
            maxV=maxVRot;
            zIsUp=false;
        }
        else
            tr.setIdentity();

        C3Vector dim(maxV-minV);
        float maxDim=SIM_MAX(SIM_MAX(dim(0),dim(1)),dim(2));
        for (int i=0;i<3;i++)
        {
            if (maxDim>2.0f)
            {
                maxDim*=0.1f;
                performedScaling/=10.0f;
                invScaling*=10;
            }
        }
        if (invScaling==10)
        { // no one uses a unit of 10cm
            invScaling=1;
            performedScaling=1.0f;
        }

        if (!showDlg)
        {
            performedScaling=sf;
            tr.setIdentity();
        }

        for (int i=0;i<int(aVertices.size());i++)
        {
            for (int j=0;j<int(aVertices[i]->size())/3;j++)
            {
                C3Vector v(&aVertices[i]->at(3*j));
                v*=performedScaling;
                v*=tr;
                aVertices[i]->at(3*j+0)=v(0);
                aVertices[i]->at(3*j+1)=v(1);
                aVertices[i]->at(3*j+2)=v(2);
            }
        }
    }


    std::vector<int> newSelection;
    App::ct->objCont->enableObjectActualization(false);
    bool groupImport=false;
    bool groupedOrMergedImport=false;
    if (aVertices.size()>2000)
    { // We have too many lose elements... we merge them
        groupedOrMergedImport=true;
        printf("Import operation with more than 2000 separate items (%i).\n",int(aVertices.size()));

#ifdef SIM_WITH_GUI
        if (showDlg)
        {
            if (showProgress)
                App::uiThread->showOrHideProgressBar(false);
            App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_IMPORT),strTranslate(IDS_TOO_MANY_MESH_ELEMENTS_DURING_IMPORT),VMESSAGEBOX_OKELI);
            if (showProgress)
                App::uiThread->showOrHideProgressBar(true);
        }
#endif

        for (int i=0;i<int(aVertices.size());i++)
        {
            if (aMaterials[i]!=NULL)
                groupImport=true;
        }
        if (groupImport)
            printf("Shapes will be grouped.\n");
        else
        {
            printf("Shapes will be merged.\n");
            for (int i=1;i<int(aVertices.size());i++)
            {
                int indOffset=aVertices[0]->size()/3;
                aVertices[0]->insert(aVertices[0]->end(),aVertices[i]->begin(),aVertices[i]->end());
                for (int j=0;j<int(aIndices[i]->size());j++)
                    aIndices[0]->push_back(aIndices[i]->at(j)+indOffset);
                delete aVertices[i];
                delete aIndices[i];
                delete aNormals[i];
                delete aTextureCoordinates[i];
                delete aMaterials[i];
            }
            aVertices.resize(1);
            aIndices.resize(1);
            aNormals.resize(1);
            groupNames.resize(1);
            // we have to kill the texures:
            aTextureCoordinates.resize(1);
            delete aTextureCoordinates[0];
            aTextureCoordinates[0]=NULL;
            aMaterials.resize(1);
            delete aMaterials[0];
            aMaterials[0]=NULL;

            // Important to check the meshe here again (we might have merged individual triangles of a complexe shape here)
            CMeshManip::checkVerticesIndicesNormalsTexCoords(aVertices[0][0],aIndices[0][0],aNormals[0],aTextureCoordinates[0],keepIdenticalVertices,App::userSettings->identicalVerticesTolerance,keepIdenticalTriangles);
            if ((aNormals[0]!=NULL)&&(aNormals[0]->size()==0))
            {
                delete aNormals[0];
                aNormals[0]=NULL;
            }
            if ((aTextureCoordinates[0]!=NULL)&&(aTextureCoordinates[0]->size()==0))
            {
                delete aTextureCoordinates[0];
                aTextureCoordinates[0]=NULL;
            }
        }

        if (showProgress)
            App::uiThread->showOrHideProgressBar(true,0.8f);
    }

    int scaleTexturesTo=-1;

    std::vector<std::string> _allTextureNames;
    std::vector<unsigned char*> _allTextureData;
    std::vector<int> _allTextureProp;

    float bprog=0.4f;
    float rprog=0.6f;
    if (groupedOrMergedImport)
    {
        bprog=0.8f;
        rprog=0.2f;
    }

    for (int i=0;i<int(aVertices.size());i++)
    {
        if (showProgress)
            App::uiThread->showOrHideProgressBar(true,bprog+rprog*(float(i)/float(aVertices.size())));

        printf("Generating a shape (%i)... ",i);
        CGeomProxy* geom;
        geom=new CGeomProxy(NULL,*aVertices[i],*aIndices[i],aNormals[i],aTextureCoordinates[i]);
        CShape* shape=new CShape();
        shape->setLocalTransformation(geom->getCreationTransformation());
        geom->setCreationTransformation(C7Vector::identityTransformation);
        shape->geomData=geom;
        App::ct->objCont->addObjectToScene(shape,false);
        // Handle colors and textures:
        if (aMaterials[i]!=NULL)
        {
            // Color:
            /*
            if ((aMaterials[i]->textureFile.length()!=0)&&(aTextureCoordinates[i]!=NULL))
            { // because some formats have zero here, and then the texture seems not to be present!
                for (int j=0;j<3;j++)
                {
                    ((CGeometric*)geom->geomInfo)->color.colors[j]=0.5f;
                    ((CGeometric*)geom->geomInfo)->color.colors[3+j]=0.25f;
                }
            }
            else
            {
                for (int j=0;j<3;j++)
                {
                    ((CGeometric*)geom->geomInfo)->color.colors[j]=aMaterials[i]->ambient[j];
                    ((CGeometric*)geom->geomInfo)->color.colors[3+j]=aMaterials[i]->diffuse[j];
                }
            }
            */
            for (int j=0;j<3;j++)
            { // in V-REP, ambient and diffuse use the same color and are the same!
                ((CGeometric*)geom->geomInfo)->color.colors[j]=SIM_MAX(aMaterials[i]->ambient[j],aMaterials[i]->diffuse[j]);
                ((CGeometric*)geom->geomInfo)->color.colors[3+j]=SIM_MAX(aMaterials[i]->ambient[j],aMaterials[i]->diffuse[j]);
                ((CGeometric*)geom->geomInfo)->color.colors[6+j]=aMaterials[i]->specular[j];
                ((CGeometric*)geom->geomInfo)->color.colors[9+j]=aMaterials[i]->emission[j];
            }
            /*
            ((CGeometric*)geom->geomInfo)->color.colors[6]=aMaterials[i]->specular[0];
            ((CGeometric*)geom->geomInfo)->color.colors[7]=aMaterials[i]->specular[1];
            ((CGeometric*)geom->geomInfo)->color.colors[8]=aMaterials[i]->specular[2];
            ((CGeometric*)geom->geomInfo)->color.colors[9]=aMaterials[i]->emission[0];
            ((CGeometric*)geom->geomInfo)->color.colors[10]=aMaterials[i]->emission[1];
            ((CGeometric*)geom->geomInfo)->color.colors[11]=aMaterials[i]->emission[2];
            */
            if (aMaterials[i]->transparency>0.01f)
            {
                ((CGeometric*)geom->geomInfo)->color.translucid=true;
                shape->actualizeContainsTransparentComponent();
                ((CGeometric*)geom->geomInfo)->color.transparencyFactor=1.0f-aMaterials[i]->transparency;
            }
            // ((CGeometric*)geom->geomInfo)->color.shininess=int(aMaterials[i]->shininess*128.1f);

            if ((aMaterials[i]->textureFile.length()!=0)&&(aTextureCoordinates[i]!=NULL))
            { // Texture:
                bool textureLoadFail=true;

                if (VVarious::isAbsolutePath(aMaterials[i]->textureFile))
                    aMaterials[i]->textureFile=VVarious::splitPath_fileBaseAndExtension(aMaterials[i]->textureFile);
                std::string tmp=VVarious::splitPath_path(pathName)+VREP_SLASH+aMaterials[i]->textureFile;
                if (VFile::doesFileExist(tmp))
                {
                    if (scaleTexturesTo==-1)
                    {
                        if (!useOrientSizeFromPreviousImport)
                        {
#ifdef SIM_WITH_GUI
                            if (showDlg)
                            {
                                int pageIndex=App::ct->pageContainer->getActivePageIndex();
                                CSPage* page=App::ct->pageContainer->getPage(pageIndex);
                                if (page!=NULL)
                                {
                                    int ind=page->getLastMouseDownViewIndex();
                                    if (ind==-1)
                                        ind=0;
                                    CSView* view=page->getView(ind);
                                    if (view!=NULL)
                                    {
                                        CCamera* cam=App::ct->objCont->getCamera(view->getLinkedObjectID());
                                        if ( (cam!=NULL) )
                                        {
                                            int viewSize[2];
                                            view->getViewSize(viewSize);

                                            cam->frameSceneOrSelectedObjects(float(viewSize[0])/float(viewSize[1]),view->getPerspectiveDisplay(),NULL,true,true,1.0f,NULL);
                                            cam->setFogTimer(4.0f);
                                        }
                                    }
                                }

                                // Display the texture scaling dialog:
                                if (showProgress)
                                    App::uiThread->showOrHideProgressBar(false);
                                SUIThreadCommand cmdIn2;
                                SUIThreadCommand cmdOut2;
                                cmdIn2.cmdId=TEXTURE_SCALING_OPTION_DLG_UITHREADCMD;
                                App::uiThread->executeCommandViaUiThread(&cmdIn2,&cmdOut2);
                                scaleTexturesTo=0;
                                if ( (cmdOut2.boolParams.size()>0)&&(cmdOut2.boolParams[0]) )
                                    scaleTexturesTo=cmdOut2.intParams[0];
                                if (showProgress)
                                    App::uiThread->showOrHideProgressBar(true);
                            }
                            else
#endif
                                scaleTexturesTo=0;//256;
                        }
                        else
                            scaleTexturesTo=prevScaleTextureTo;
                    }
                    prevScaleTextureTo=scaleTexturesTo;

                    int resX,resY,n;

                    int theIndex=-1;
                    for (int j=0;j<int(_allTextureNames.size());j++)
                    {
                        if (_allTextureNames[j].compare(tmp)==0)
                        {
                            theIndex=j;
                            break;
                        }
                    }
                    unsigned char* data=NULL;
                    if (theIndex!=-1)
                    { // that texture was already loaded
                        data=_allTextureData[theIndex];
                        resX=_allTextureProp[3*theIndex+0];
                        resY=_allTextureProp[3*theIndex+1];
                        n=_allTextureProp[3*theIndex+2];
                    }
                    else
                    { // that texture was not yet loaded
                        data=CImageLoaderSaver::load(tmp.c_str(),&resX,&resY,&n,0,scaleTexturesTo);
                        if (n<3)
                        {
                            delete[] data;
                            data=NULL;
                        }
                        if (data!=NULL)
                        {
                            _allTextureNames.push_back(tmp);
                            _allTextureData.push_back(data);
                            _allTextureProp.push_back(resX);
                            _allTextureProp.push_back(resY);
                            _allTextureProp.push_back(n);
                        }
                    }
                    bool rgba=(n==4);
                    if (data!=NULL)
                    {
                        textureLoadFail=false;
                        CTextureObject* textureObj=new CTextureObject(resX,resY);
                        textureObj->setImage(rgba,false,false,data); // keep false,false
                        textureObj->setObjectName(App::directories->getNameFromFull(aMaterials[i]->textureFile).c_str());
                        // data is destroyed later (when _allTextureProp is destroyed)!
                        textureObj->addDependentObject(shape->getID(),((CGeometric*)shape->geomData->geomInfo)->getUniqueID());
                        int textureID=App::ct->textureCont->addObject(textureObj,false); // might erase the textureObj and return a similar object already present!!
                        CTextureProperty* tp=new CTextureProperty(textureID);
                        ((CGeometric*)shape->geomData->geomInfo)->setTextureProperty(tp);

                        std::vector<float> wvert;
                        std::vector<int> wind;
                        ((CGeometric*)shape->geomData->geomInfo)->getCumulativeMeshes(wvert,&wind,NULL);
                        if (((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized.size()/2==wind.size())
                        { // we have texture coordinate data attached to the shape's geometry (was added during shape import)
                            tp->setFixedCoordinates(&((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized);
                            ((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized.clear();
                        }
                    }
                }
                if (textureLoadFail)
                {
                    std::string txtTmp(IDS_ERROR);
                    txtTmp+=": ";
                    txtTmp+=IDS_A_TEXTURE_FILE_COULD_NOT_BE_LOADED;
                    txtTmp+=": ";
                    txtTmp+=tmp;
                    App::addStatusbarMessage(txtTmp.c_str());
                }
            }
        }


        shape->setVisibleEdges(aTextureCoordinates[i]==NULL); // Visible edges only if no texture coords (otherwise it looks like a bug sometimes)
        if (aNormals[i]==NULL)
            ((CGeometric*)shape->geomData->geomInfo)->setGouraudShadingAngle(30.0f*degToRad); 
        ((CGeometric*)shape->geomData->geomInfo)->setEdgeThresholdAngle(30.0f*degToRad); 
        shape->geomData->geomInfo->setLocalInertiaFrame(C7Vector::identityTransformation); // 11/2/2013
        newSelection.push_back(shape->getID());

        std::string tempName("imported_part_0");
        if (groupNames[i]!="")
            tempName=groupNames[i];

        std::string tempName2=App::ct->objCont->getSimilarNameWithHighestSuffix(tempName,false);
        if (tempName2.length()!=0)
            tempName=tempName2;

        while (App::ct->objCont->getObject(tempName)!=NULL)
            tempName=tt::generateNewName_noDash(tempName);
        shape->setName(tempName);

        delete aVertices[i];
        delete aIndices[i];
        delete aNormals[i];
        delete aTextureCoordinates[i];
        delete aMaterials[i];
        printf("done.\n");
    }

    for (int i=0;i<int(_allTextureData.size());i++)
        delete[] _allTextureData[i];

    if (showProgress)
        App::uiThread->showOrHideProgressBar(false);

    App::ct->objCont->enableObjectActualization(true);
    App::ct->objCont->actualizeObjectInformation();
    groupNames.clear();
    aVertices.clear();
    aIndices.clear();
    aNormals.clear();
    aTextureCoordinates.clear();

    std::vector<int> newObjects;
    if (groupImport)
        newObjects.push_back(CSceneObjectOperations::groupSelection(&newSelection,false));
    else
        newObjects.assign(newSelection.begin(),newSelection.end());

    for (int i=0;i<int(newObjects.size());i++)
        App::ct->objCont->addObjectToSelection(newObjects[i]);


    if (!useOrientSizeFromPreviousImport)
    {
        if (showDlg)
        { // display the scaling/roation dialog:
            SUIThreadCommand cmdIn;
            SUIThreadCommand cmdOut;
            cmdIn.cmdId=IMPORT_OPTION_DLG_UITHREADCMD;
            cmdIn.intParams.push_back(0); // sizeIndex
            if (invScaling==100)
                cmdIn.intParams[0]=4;
            if (invScaling==1000)
                cmdIn.intParams[0]=5;
            cmdIn.boolParams.push_back(zIsUp);
            App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
            if (cmdOut.boolParams.size()>0)
            {
                performedScaling*=cmdOut.floatParams[0];
                zIsUp=cmdOut.boolParams[0];
            }
        }
        prevTotalScale=performedScaling;
        prevZIsUp=zIsUp;
    }
    else
    {
        simScaleObjects_internal(&newObjects[0],int(newObjects.size()),prevTotalScale,true);
        std::vector<int> sel;
        App::ct->objCont->getSelectedObjects(sel);
        C4X4Matrix _r;
        _r.setIdentity();
        _r.M.buildXRotation(piValD2);
        C7Vector rot(_r);
        for (int i=0;i<int(newObjects.size());i++)
        {
            C3DObject* it=App::ct->objCont->getObject(newObjects[i]);
            it->setSizeFactor(1.0f);
            if (!prevZIsUp)
            {
                C7Vector tr(it->getLocalTransformation());
                it->setLocalTransformation(rot*tr);
            }
        }
    }

    return(!error);
}

bool CFileOperations::apiImportRoutine(int importType,const std::string& pathName,int options,float identicalVerticesTolerance,float sizeFactor,std::vector<std::vector<float>*>& listOfVertices,std::vector<std::vector<int>*>& listOfIndices,std::vector<std::string>& groupNames)
{ // options: bit 0: keep identical vertices, bit 1: keep identical triangles, bit 2: no winding check
    CImportExport* importer=NULL;
    if (importType==FILE_FORMAT_DXF)
        importer=new CDxfFile();
    if (importType==FILE_FORMAT_OBJ)
        importer=new CObjFile();
    if ((importType==FILE_FORMAT_ASCII_STL)||(importType==FILE_FORMAT_BINARY_STL)||(importType==FILE_FORMAT_ANY_STL))
        importer=new CStlFile(importType);
    importer->importFunc(pathName);

    for (int i=0;i<importer->getGroupCount();i++)
    {
        if ((importer->getVertexCount(i)>=3)&&(importer->getTriangleCount(i)>=1))
        { 
            std::vector<float>* vert=new std::vector<float>;
            std::vector<int>* ind=new std::vector<int>;
            for (int j=0;j<importer->getVertexCount(i);j++)
            {
                float vertex[3];
                importer->getVertex(i,j,vertex);
                vert->push_back(vertex[0]);
                vert->push_back(vertex[1]);
                vert->push_back(vertex[2]);
            }
            for (int j=0;j<importer->getTriangleCount(i);j++)
            {
                int triangle[3];
                importer->getTriangle(i,j,triangle);
                ind->push_back(triangle[0]);
                ind->push_back(triangle[1]);
                ind->push_back(triangle[2]);
            }
            CMeshManip::checkVerticesIndicesNormalsTexCoords(*vert,*ind,NULL,NULL,(options&1)==0,identicalVerticesTolerance,(options&2)==0);
            if ((vert->size()>=9)&&(ind->size()>=3))
            {
                listOfVertices.push_back(vert);
                listOfIndices.push_back(ind);
                // Here we don't need to remove illegal chars! (API importer)
                groupNames.push_back(importer->getName(i));
            }
            else
            {
                delete vert;
                delete ind;
            }
        }
    }
    delete importer;


    for (int i=0;i<int(listOfVertices.size());i++)
        CMeshManip::centerAndScale(listOfVertices[i],0.0f,0.0f,0.0f,sizeFactor);

    return(listOfVertices.size()!=0);
}



bool CFileOperations::_pathImportRoutine(const std::string& pathName,bool displayDialogs)
{ // Should only be called by the NON-UI thread
    bool retVal=false;
    if (VFile::doesFileExist(pathName))
    {
        if (displayDialogs)
            App::uiThread->showOrHideProgressBar(true,-1,"Importing path...");
        try
        {
            VFile file(pathName.c_str(),VFile::READ|VFile::SHARE_DENY_NONE);
            VArchive archive(&file,VArchive::LOAD);
            unsigned int currentPos=0;
            std::string line;
            std::vector<float> readData;
            bool automaticOrientation=true;
            const int ds=16;
            // We read:
            // (x,y,z), (a,b,g), velocityRatio, bezierPtCount,beforeRatio,afterRatio,virtualDistance,auxFlags,auxChannel1,auxChannel2,auxChannel3,auxChannel4
            while (archive.readSingleLine(currentPos,line,false))
            {
                float data[ds];
                int cnt=0;
                std::string word;
                float val;
                while (tt::extractCommaSeparatedWord(line,word))
                {
                    if (tt::getValidFloat(word,val))
                        data[cnt]=val;
                    else
                        break;
                    cnt++;
                    if (cnt>=ds)
                        break;
                }
                if (cnt>=3)
                { // We have a point!
                    int dataOff=readData.size();
                    readData.push_back(0.0f); // x
                    readData.push_back(0.0f); // y
                    readData.push_back(0.0f); // z
                    readData.push_back(0.0f); // Euler 0
                    readData.push_back(0.0f); // Euler 1
                    readData.push_back(0.0f); // Euler 2
                    readData.push_back(1.0f); // Velocity ratio
                    readData.push_back(1.0f); // Bezier count
                    readData.push_back(0.5f); // Bezier 0
                    readData.push_back(0.5f); // Bezier 1
                    readData.push_back(0.0f); // virtual distance
                    readData.push_back(0.0f); // aux flags
                    readData.push_back(0.0f); // aux channel1
                    readData.push_back(0.0f); // aux channel2
                    readData.push_back(0.0f); // aux channel3
                    readData.push_back(0.0f); // aux channel4

                    readData[dataOff+0]=data[0]; // x
                    readData[dataOff+1]=data[1]; // y
                    readData[dataOff+2]=data[2]; // z
                    if (cnt>=6)
                    {
                        automaticOrientation=false;
                        readData[dataOff+3]=data[3]*degToRad; // Euler 0
                        readData[dataOff+4]=data[4]*degToRad; // Euler 1
                        readData[dataOff+5]=data[5]*degToRad; // Euler 2
                    }
                    if (cnt>=7)
                        readData[dataOff+6]=data[6]; // velocity ratio
                    if (cnt>=8)
                        readData[dataOff+7]=data[7]; // Bezier count
                    if (cnt>=10)
                    {
                        readData[dataOff+8]=data[8]; // Bezier 0
                        readData[dataOff+9]=data[9]; // Bezier 1
                    }
                    if (cnt>=11)
                        readData[dataOff+10]=data[10]; // Virtual distance
                    if (cnt>=12)
                        readData[dataOff+11]=data[11]; // aux flags
                    if (cnt>=13)
                        readData[dataOff+12]=data[12]; // aux channel1
                    if (cnt>=14)
                        readData[dataOff+13]=data[13]; // aux channel2
                    if (cnt>=15)
                        readData[dataOff+14]=data[14]; // aux channel3
                    if (cnt>=16)
                        readData[dataOff+15]=data[15]; // aux channel4
                }
            }
            if (readData.size()!=0)
            {
                CPath* newObject=new CPath();
                newObject->pathContainer->enableActualization(false);
                int attr=newObject->pathContainer->getAttributes()|sim_pathproperty_automatic_orientation;
                if (!automaticOrientation)
                    attr-=sim_pathproperty_automatic_orientation;
                newObject->pathContainer->setAttributes(attr);
                for (int i=0;i<int(readData.size())/ds;i++)
                {
                    CSimplePathPoint* it=new CSimplePathPoint();
                    C7Vector tr;
                    tr.X(0)=readData[ds*i+0];
                    tr.X(1)=readData[ds*i+1];
                    tr.X(2)=readData[ds*i+2];
                    tr.Q.setEulerAngles(readData[ds*i+3],readData[ds*i+4],readData[ds*i+5]);
                    it->setTransformation(tr,newObject->pathContainer->getAttributes());
                    it->setMaxRelAbsVelocity(readData[ds*i+6]);
                    it->setBezierPointCount(int(readData[ds*i+7]+0.5f));
                    it->setBezierFactors(readData[ds*i+8],readData[ds*i+9]);
                    it->setOnSpotDistance(readData[ds*i+10]);
                    it->setAuxFlags(int(readData[ds*i+11]+0.5f));
                    it->setAuxChannels(&readData[ds*i+12]);
                    newObject->pathContainer->addSimplePathPoint(it);
                }
                newObject->pathContainer->enableActualization(true);
                newObject->pathContainer->actualizePath();
                newObject->setName(IDSOGL_IMPORTEDPATH);
                App::ct->objCont->addObjectToScene(newObject,false);
                App::ct->objCont->selectObject(newObject->getID());
            }
            archive.close();
            file.close();
            retVal=readData.size()!=0;
        }
        catch(VFILE_EXCEPTION_TYPE e)
        {
            VFile::reportAndHandleFileExceptionError(e);
            retVal=false;
        }
        if (displayDialogs)
            App::uiThread->showOrHideProgressBar(false);
    }
    return(retVal);
}

bool CFileOperations::_pathExportPoints(const std::string& pathName,int pathID,bool bezierPoints,bool displayDialogs)
{
    CPath* pathObject=App::ct->objCont->getPath(pathID);
    if (pathObject==NULL)
        return(false);
    bool retVal=false;
    if (displayDialogs)
        App::uiThread->showOrHideProgressBar(true,-1,"Exporting path points...");
    try
    {
        VFile myFile(pathName.c_str(),VFile::CREATE_WRITE|VFile::SHARE_EXCLUSIVE);
        VArchive ar(&myFile,VArchive::STORE);

        CPathCont* it=pathObject->pathContainer;
        C7Vector pathTr(pathObject->getCumulativeTransformation());
        if (bezierPoints)
        {
            for (int i=0;i<it->getBezierPathPointCount();i++)
            {
                CBezierPathPoint* bp=it->getBezierPathPoint(i);
                C7Vector tr(pathTr*bp->getTransformation());
                C3Vector euler(tr.Q.getEulerAngles());
                std::string line(tt::FNb(tr.X(0))+',');
                line+=tt::FNb(tr.X(1))+',';
                line+=tt::FNb(tr.X(2))+',';
                line+=tt::FNb(euler(0)*radToDeg)+',';
                line+=tt::FNb(euler(1)*radToDeg)+',';
                line+=tt::FNb(euler(2)*radToDeg)+',';
                line+=tt::FNb(bp->getMaxRelAbsVelocity())+',';
                line+=tt::FNb(bp->getOnSpotDistance())+',';
                line+=tt::FNb(bp->getAuxFlags())+',';
                float auxChannels[4];
                bp->getAuxChannels(auxChannels);
                line+=tt::FNb(auxChannels[0])+',';
                line+=tt::FNb(auxChannels[1])+',';
                line+=tt::FNb(auxChannels[2])+',';
                line+=tt::FNb(auxChannels[3]);
                ar.writeLine(line);
            }
        }
        else
        {
            for (int i=0;i<it->getSimplePathPointCount();i++)
            {
                CSimplePathPoint* bp=it->getSimplePathPoint(i);
                C7Vector tr(pathTr*bp->getTransformation());
                C3Vector euler(tr.Q.getEulerAngles());
                float f0,f1;
                bp->getBezierFactors(f0,f1);
                std::string line(tt::FNb(tr.X(0))+',');
                line+=tt::FNb(tr.X(1))+',';
                line+=tt::FNb(tr.X(2))+',';
                line+=tt::FNb(euler(0)*radToDeg)+',';
                line+=tt::FNb(euler(1)*radToDeg)+',';
                line+=tt::FNb(euler(2)*radToDeg)+',';
                line+=tt::FNb(bp->getMaxRelAbsVelocity())+',';
                line+=tt::FNb(bp->getBezierPointCount())+',';
                line+=tt::FNb(f0)+',';
                line+=tt::FNb(f1)+',';
                line+=tt::FNb(bp->getOnSpotDistance())+',';
                line+=tt::FNb(bp->getAuxFlags())+',';
                float auxChannels[4];
                bp->getAuxChannels(auxChannels);
                line+=tt::FNb(auxChannels[0])+',';
                line+=tt::FNb(auxChannels[1])+',';
                line+=tt::FNb(auxChannels[2])+',';
                line+=tt::FNb(auxChannels[3]);
                ar.writeLine(line);
            }
        }
        ar.close();
        myFile.close();
        retVal=true;
    }
    catch(VFILE_EXCEPTION_TYPE e)
    {
        VFile::reportAndHandleFileExceptionError(e);
        retVal=false;
    }
    if (displayDialogs)
        App::uiThread->showOrHideProgressBar(false);
    return(retVal);
}

bool CFileOperations::loadScene(const char* pathAndFilename,bool displayMessages,bool displayDialogs,bool setCurrentDir)
{
    FUNCTION_DEBUG;
    if (App::isFullScreen()||App::userSettings->doNotShowAcknowledgmentMessages)
        displayDialogs=false;

    if (strlen(pathAndFilename)==0)
    {
        createNewScene(displayMessages,true);
        return(true);
    }

    App::setDefaultMouseMode();

    int result=-3;
#ifdef SIM_WITH_GUI
    if (App::mainWindow!=NULL)
        App::mainWindow->scintillaEditorContainer->closeAllEditors();
#endif
    App::ct->objCont->deselectObjects();
    App::ct->simulation->stopSimulation(); // should be anyway stopped!
    if (VFile::doesFileExist(pathAndFilename))
    {
        App::ct->emptyScene(true);

        App::ct->mainSettings->setScenePathAndName(pathAndFilename);
        if (setCurrentDir)
            App::directories->sceneDirectory=App::ct->mainSettings->getScenePath();


        if (displayDialogs)
            App::uiThread->showOrHideProgressBar(true,-1,"Opening scene...");

        VFile file(pathAndFilename,VFile::READ|VFile::SHARE_DENY_NONE);
        VArchive archive(&file,VArchive::LOAD);
        CSer serObj(archive);

        int serializationVersion;
        unsigned short vrepVersionThatWroteThis;
        int licenseTypeThatWroteThis;
        char revisionNumber;
        result=serObj.readOpen(serializationVersion,vrepVersionThatWroteThis,licenseTypeThatWroteThis,revisionNumber);
        std::string infoPrintOut(tt::decorateString("",IDSNS_LOADING_SCENE," ("));
        infoPrintOut+=std::string(pathAndFilename)+"). ";
#ifdef SIM_WITH_GUI
        if ((result==-3)&&(App::mainWindow!=NULL))
        {
            if (displayMessages)
                App::addStatusbarMessage(IDS_NOT_VALID_V_REP_FILE);
            if (displayDialogs)
            {
                App::uiThread->showOrHideProgressBar(false);
                App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_NOT_VALID_V_REP_FILE),VMESSAGEBOX_OKELI);
                App::uiThread->showOrHideProgressBar(true);
            }
        }
        if ((result!=-3)&&displayMessages&&(App::mainWindow!=NULL))
        {
            infoPrintOut+=" ";
            infoPrintOut+=IDSNS_SERIALIZATION_VERSION_IS;
            infoPrintOut+=" ";
            infoPrintOut+=boost::lexical_cast<std::string>(serializationVersion)+".";
            App::addStatusbarMessage(infoPrintOut.c_str());
            infoPrintOut=_getStringOfVersionAndLicenseThatTheFileWasWrittenWith(vrepVersionThatWroteThis,licenseTypeThatWroteThis,revisionNumber);
            if (infoPrintOut!="")
                App::addStatusbarMessage(infoPrintOut.c_str());
        }
        if ((result==-2)&&(App::mainWindow!=NULL))
        {
            if (displayMessages)
                App::addStatusbarMessage(IDS_SERIALIZATION_VERSION_NOT_SUPPORTED_ANYMORE);
            if (displayDialogs)
            {
                App::uiThread->showOrHideProgressBar(false);
                App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_SERIALIZATION_VERSION_NOT_SUPPORTED_ANYMORE),VMESSAGEBOX_OKELI);
                App::uiThread->showOrHideProgressBar(true);
            }
        }
        if ((result==-1)&&(App::mainWindow!=NULL))
        {
            if (displayMessages)
                App::addStatusbarMessage(IDS_SERIALIZATION_VERSION_TOO_RECENT);
            if (displayDialogs)
            {
                App::uiThread->showOrHideProgressBar(false);
                App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_SERIALIZATION_VERSION_TOO_RECENT),VMESSAGEBOX_OKELI);
                App::uiThread->showOrHideProgressBar(true);
            }
        }
        if ((result==0)&&(App::mainWindow!=NULL))
        {
            if (displayMessages)
                App::addStatusbarMessage(IDS_COMPRESSION_SCHEME_NOT_SUPPORTED);
            if (displayDialogs)
            {
                App::uiThread->showOrHideProgressBar(false);
                App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_COMPRESSION_SCHEME_NOT_SUPPORTED),VMESSAGEBOX_OKELI);
                App::uiThread->showOrHideProgressBar(true);
            }
        }
#endif
        if (result==1)
        {
            App::ct->objCont->loadScene(serObj,false);
            serObj.readClose();
#ifdef SIM_WITH_GUI
            if (App::mainWindow!=NULL)
                App::mainWindow->refreshDimensions(); // this is important so that the new pages and views are set to the correct dimensions
            if (displayMessages)
                App::addStatusbarMessage(IDSNS_SCENE_OPENED);
            if ((vrepVersionThatWroteThis>VREP_PROGRAM_VERSION_NB)&&displayDialogs&&(App::mainWindow!=NULL))
                App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SCENE),strTranslate(IDS_SAVED_WITH_MORE_RECENT_VERSION_WARNING),VMESSAGEBOX_OKELI);
            std::string acknowledgement(App::ct->environment->getAcknowledgement());
            std::string tmp(acknowledgement);
            tt::removeSpacesAtBeginningAndEnd(tmp);
            if (displayDialogs&&(App::mainWindow!=NULL))
            {
                if (tmp.length()!=0)
                {
                    SSimulationThreadCommand cmd;
                    cmd.cmdId=POSTPONE_PROCESSING_THIS_LOOP_CMD;
                    cmd.intParams.push_back(3);
                    App::appendSimulationThreadCommand(cmd);
                    cmd.cmdId=DISPLAY_ACKNOWLEDGMENT_MESSAGE_CMD;
                    cmd.stringParams.push_back(IDS_SCENE_CONTENT_ACKNOWLEDGMENTS);
                    cmd.stringParams.push_back(acknowledgement);
                    App::appendSimulationThreadCommand(cmd);
                }
            }
#endif
        }
        else
        {
            if (displayMessages)
                App::addStatusbarMessage(IDSNS_SCENE_COULD_NOT_BE_OPENED);
        }
        archive.close();
        file.close();

        if (displayDialogs)
            App::uiThread->showOrHideProgressBar(false);
        App::ct->undoBufferContainer->memorizeState(); // so that we can come back to the initial state!
    }
    else
    {
        if (displayMessages)
            App::addStatusbarMessage(IDSNS_ABORTED_FILE_DOES_NOT_EXIST);
    }
    App::setRebuildHierarchyFlag();
    return(result==1);
}

bool CFileOperations::loadModel(const char* pathAndFilename,bool displayMessages,bool displayDialogs,bool setCurrentDir,bool noScreenRefresh,std::string* acknowledgmentPointerInReturn,bool doUndoThingInHere)
{ // if acknowledgment is NULL, then acknowledgments are directly displayed here!
    FUNCTION_DEBUG;
    if (App::isFullScreen()||App::userSettings->doNotShowAcknowledgmentMessages)
        displayDialogs=false;
    int result=-3;
    if (VFile::doesFileExist(pathAndFilename))
    {
        std::string theAcknowledgement;
        App::ct->objCont->deselectObjects();

        if (setCurrentDir)
            App::directories->modelDirectory=App::directories->getPathFromFull(pathAndFilename);

        if (displayDialogs)
            App::uiThread->showOrHideProgressBar(true,-1,"Loading model...");

        VFile file(pathAndFilename,VFile::READ|VFile::SHARE_DENY_NONE);
        VArchive archive(&file,VArchive::LOAD);
        CSer serObj(archive);

        int serializationVersion;
        unsigned short vrepVersionThatWroteThis;
        int licenseTypeThatWroteThis;
        char revisionNumber;
        result=serObj.readOpen(serializationVersion,vrepVersionThatWroteThis,licenseTypeThatWroteThis,revisionNumber);
        std::string infoPrintOut(tt::decorateString("",IDSNS_LOADING_MODEL," ("));
        infoPrintOut+=std::string(pathAndFilename)+"). ";
#ifdef SIM_WITH_GUI
        if ((result==-3)&&(App::mainWindow!=NULL))
        {
            if (displayMessages)
                App::addStatusbarMessage(IDS_NOT_VALID_V_REP_FILE);
            if (displayDialogs)
            {
                App::uiThread->showOrHideProgressBar(false);
                App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_NOT_VALID_V_REP_FILE),VMESSAGEBOX_OKELI);
                App::uiThread->showOrHideProgressBar(true);
            }
        }
        if (((result!=-3)&&displayMessages)&&(App::mainWindow!=NULL))
        {
            infoPrintOut+=" ";
            infoPrintOut+=IDSNS_SERIALIZATION_VERSION_IS;
            infoPrintOut+=" ";
            infoPrintOut+=boost::lexical_cast<std::string>(serializationVersion)+".";
            App::addStatusbarMessage(infoPrintOut.c_str());
            infoPrintOut=_getStringOfVersionAndLicenseThatTheFileWasWrittenWith(vrepVersionThatWroteThis,licenseTypeThatWroteThis,revisionNumber);
            if (infoPrintOut!="")
                App::addStatusbarMessage(infoPrintOut.c_str());
        }
        if ((result==-2)&&(App::mainWindow!=NULL))
        {
            if (displayMessages)
                App::addStatusbarMessage(IDS_SERIALIZATION_VERSION_NOT_SUPPORTED_ANYMORE);
            if (displayDialogs)
            {
                App::uiThread->showOrHideProgressBar(false);
                App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_SERIALIZATION_VERSION_NOT_SUPPORTED_ANYMORE),VMESSAGEBOX_OKELI);
                App::uiThread->showOrHideProgressBar(true);
            }
        }
        if ((result==-1)&&(App::mainWindow!=NULL))
        {
            if (displayMessages)
                App::addStatusbarMessage(IDS_SERIALIZATION_VERSION_TOO_RECENT);
            if (displayDialogs)
            {
                App::uiThread->showOrHideProgressBar(false);
                App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_SERIALIZATION_VERSION_TOO_RECENT),VMESSAGEBOX_OKELI);
                App::uiThread->showOrHideProgressBar(true);
            }
        }
        if ((result==0)&&(App::mainWindow!=NULL))
        {
            if (displayMessages)
                App::addStatusbarMessage(IDS_COMPRESSION_SCHEME_NOT_SUPPORTED);
            if (displayDialogs)
            {
                App::uiThread->showOrHideProgressBar(false);
                App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_COMPRESSION_SCHEME_NOT_SUPPORTED),VMESSAGEBOX_OKELI);
                App::uiThread->showOrHideProgressBar(true);
            }
        }
#endif
        bool removeLoadedModel=false;
        if (result==1)
        {
            App::ct->objCont->loadModel(serObj,false,NULL,NULL,NULL);
            serObj.readClose();
            if (displayMessages)
                App::addStatusbarMessage(IDSNS_MODEL_LOADED);
#ifdef SIM_WITH_GUI
            if ((vrepVersionThatWroteThis>VREP_PROGRAM_VERSION_NB)&&displayDialogs&&(App::mainWindow!=NULL))
            {
                App::uiThread->showOrHideProgressBar(false);
                App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_MODEL),strTranslate(IDS_MODEL_SAVED_WITH_MORE_RECENT_VERSION_WARNING),VMESSAGEBOX_OKELI);
                App::uiThread->showOrHideProgressBar(true);
            }
#endif
            if (!removeLoadedModel)
            {
                std::string acknowledgement;
                std::string tmp;
                // now we search for the model base that contains the acknowledgment:
                std::vector<C3DObject*> loadedObjects;
                App::ct->objCont->getSelectedObjects(loadedObjects);
                for (int obba=0;obba<int(loadedObjects.size());obba++)
                {
                    if (loadedObjects[obba]->getParent()==NULL)
                    {
                        acknowledgement=loadedObjects[obba]->getModelAcknowledgement();
                        tmp=acknowledgement;
                        tt::removeSpacesAtBeginningAndEnd(tmp);
                        break;
                    }
                }

                if (tmp.length()!=0)
                {
                    if (acknowledgmentPointerInReturn==NULL)
                    {

                        if (displayMessages)
                            theAcknowledgement=acknowledgement;
                    }
                    else
                        acknowledgmentPointerInReturn[0]=acknowledgement;
                }
            }
        }
        else
        {
            if (displayMessages)
                App::addStatusbarMessage(IDSNS_MODEL_COULD_NOT_BE_LOADED);
        }
        archive.close();
        file.close();
        if (removeLoadedModel)
        {
            std::vector<C3DObject*> toRemove;
            App::ct->objCont->getSelectedObjects(toRemove);
            App::ct->objCont->deselectObjects();
            for (int i=0;i<int(toRemove.size());i++)
                App::ct->objCont->eraseObject(toRemove[i]);
        }
        if (displayDialogs)
            App::uiThread->showOrHideProgressBar(false);

        App::ct->objCont->removeFromSelectionAllExceptModelBase(false);
        App::setRebuildHierarchyFlag();
        if (doUndoThingInHere)
        {
            POST_SCENE_CHANGED_ANNOUNCEMENT(""); // ************************** UNDO thingy **************************
        }
#ifdef SIM_WITH_GUI
        if ((theAcknowledgement.length()!=0)&&displayDialogs&&(App::mainWindow!=NULL))
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=POSTPONE_PROCESSING_THIS_LOOP_CMD;
            cmd.intParams.push_back(3);
            App::appendSimulationThreadCommand(cmd);
            cmd.cmdId=DISPLAY_ACKNOWLEDGMENT_MESSAGE_CMD;
            cmd.stringParams.push_back(IDS_MODEL_CONTENT_ACKNOWLEDGMENTS);
            cmd.stringParams.push_back(theAcknowledgement);
            App::appendSimulationThreadCommand(cmd);
        }
#endif
    }
    else
    {
        if (displayMessages)
            App::addStatusbarMessage(IDSNS_ABORTED_FILE_DOES_NOT_EXIST);
    }
    return(result==1);
}
bool CFileOperations::saveUserInterfaces(const char* pathAndFilename,bool displayMessages,bool displayDialogs,bool setCurrentDir,std::vector<int>* uiHandlesOrNullForAll)
{
    if (App::isFullScreen())
        displayDialogs=false;
    bool retVal=false; // means error
    if ((App::ct->buttonBlockContainer!=NULL)&&(App::ct->buttonBlockContainer->getUnassociatedNonSystemBlockCount()!=0))
    { // ok, we have something to save!
        if (displayDialogs)
            App::uiThread->showOrHideProgressBar(true,-1,"Saving custom UI(s)...");

        VFile myFile(pathAndFilename,VFile::CREATE_WRITE|VFile::SHARE_EXCLUSIVE);
        VArchive archive(&myFile,VArchive::STORE);
        CSer ar(archive);

        std::string infoPrintOut(IDSNS_SAVING_CUSTOM_USER_INTERFACES);
        infoPrintOut+=" (";
        infoPrintOut+=std::string(pathAndFilename)+"). ";
        infoPrintOut+=" ";
        infoPrintOut+=IDSNS_SERIALIZATION_VERSION_IS;
        infoPrintOut+=" ";
        infoPrintOut+=boost::lexical_cast<std::string>(CSer::SER_SERIALIZATION_VERSION)+".";
        if (displayMessages)
            App::addStatusbarMessage(infoPrintOut.c_str());
        
        //**********************************
        ar.writeOpen();
        std::vector<CTextureProperty*> allTextureProperties;
        bool somethingWasWritten=false;
        for (int i=0;i<int(App::ct->buttonBlockContainer->allBlocks.size());i++)
        {
            if ((App::ct->buttonBlockContainer->allBlocks[i]->getAttributes()&sim_ui_property_systemblock)==0)
            {
                bool takeIt=false;
                if (uiHandlesOrNullForAll==NULL)
                    takeIt=true;
                if (!takeIt)
                {
                    for (int j=0;j<int(uiHandlesOrNullForAll->size());j++)
                    {
                        if (uiHandlesOrNullForAll->at(j)==App::ct->buttonBlockContainer->allBlocks[i]->getBlockID())
                        {
                            takeIt=true;
                            break;
                        }
                    }
                }
                if (takeIt)
                {
                    somethingWasWritten=true;
                    ar.storeDataName(SER_BUTTON_BLOCK);
                    ar.setCountingMode();
                    App::ct->buttonBlockContainer->allBlocks[i]->serialize(ar);
                    if (ar.setWritingMode())
                        App::ct->buttonBlockContainer->allBlocks[i]->serialize(ar);
                    App::ct->buttonBlockContainer->allBlocks[i]->getAllAttachedTextureProperties(allTextureProperties);
                }
            }
        }
        if (somethingWasWritten)
        {
            // Now make sure that every texture object appears no more than once and is not linked to any vision sensor, then serialize the related texture object:
            std::vector<CTextureProperty*> tpc(allTextureProperties);
            allTextureProperties.clear();
            for (int i=0;i<int(tpc.size());i++)
            {
                bool present=false;
                for (int j=0;j<int(allTextureProperties.size());j++)
                {
                    if (allTextureProperties[j]->getTextureObjectID()==tpc[i]->getTextureObjectID())
                    {
                        present=true;
                        break;
                    }
                }
                if (!present)
                {
                    allTextureProperties.push_back(tpc[i]);
                    int objid=tpc[i]->getTextureObjectID();
                    CTextureObject* to=App::ct->textureCont->getObject(objid);
                    if (to!=NULL)
                    {
                        ar.storeDataName(SER_TEXTURE);
                        ar.setCountingMode();
                        to->serialize(ar);
                        if (ar.setWritingMode())
                            to->serialize(ar);
                    }

                }
            }
            ar.storeDataName(SER_END_OF_FILE);
            ar.writeClose(App::userSettings->compressFiles);
            //**********************************
            retVal=true;

            if (displayMessages)
                App::addStatusbarMessage(IDSNS_DONE);
        }
        archive.close();
        myFile.close();
        if (displayDialogs)
            App::uiThread->showOrHideProgressBar(false);
    }
    return(retVal);
}

bool CFileOperations::loadUserInterfaces(const char* pathAndFilename,bool displayMessages,bool displayDialogs,bool setCurrentDir,std::vector<int>* uiHandles,bool doUndoThingInHere)
{
    bool retVal=false;
    if (uiHandles!=NULL)
        uiHandles->clear();
    if (VFile::doesFileExist(pathAndFilename))
    {
        if (displayDialogs)
            App::uiThread->showOrHideProgressBar(true,-1,"Loading custom UI(s)...");
        App::ct->objCont->deselectObjects();
        if (setCurrentDir)
            App::directories->uiDirectory=App::directories->getPathFromFull(pathAndFilename);
        VFile file(pathAndFilename,VFile::READ|VFile::SHARE_DENY_NONE);
        VArchive archive(&file,VArchive::LOAD);
        CSer ar(archive);
        int serializationVersion;
        unsigned short vrepVersionThatWroteThis;
        int licenseTypeThatWroteThis;
        char revisionNumber;
        int result=ar.readOpen(serializationVersion,vrepVersionThatWroteThis,licenseTypeThatWroteThis,revisionNumber);

        std::string infoPrintOut(tt::decorateString("",IDSNS_LOADING_UI," ("));
        infoPrintOut+=std::string(pathAndFilename)+"). ";

#ifdef SIM_WITH_GUI
        if ((result==-3)&&(App::mainWindow!=NULL))
        {
            if (displayMessages)
                App::addStatusbarMessage(IDS_NOT_VALID_V_REP_FILE);
            if (displayDialogs)
            {
                App::uiThread->showOrHideProgressBar(false);
                App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_NOT_VALID_V_REP_FILE),VMESSAGEBOX_OKELI);
                App::uiThread->showOrHideProgressBar(true);
            }
        }
        if ((result!=-3)&&displayMessages&&(App::mainWindow!=NULL))
        {
            infoPrintOut+=" ";
            infoPrintOut+=IDSNS_SERIALIZATION_VERSION_IS;
            infoPrintOut+=" ";
            infoPrintOut+=boost::lexical_cast<std::string>(serializationVersion)+".";
            App::addStatusbarMessage(infoPrintOut.c_str());
            infoPrintOut=_getStringOfVersionAndLicenseThatTheFileWasWrittenWith(vrepVersionThatWroteThis,licenseTypeThatWroteThis,revisionNumber);
            if (infoPrintOut!="")
                App::addStatusbarMessage(infoPrintOut.c_str());
        }
        if ((result==-2)&&(App::mainWindow!=NULL))
        {
            if (displayMessages)
                App::addStatusbarMessage(IDS_SERIALIZATION_VERSION_NOT_SUPPORTED_ANYMORE);
            if (displayDialogs)
            {
                App::uiThread->showOrHideProgressBar(false);
                App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_SERIALIZATION_VERSION_NOT_SUPPORTED_ANYMORE),VMESSAGEBOX_OKELI);
                App::uiThread->showOrHideProgressBar(true);
            }
        }
        if ((result==-1)&&(App::mainWindow!=NULL))
        {
            if (displayMessages)
                App::addStatusbarMessage(IDS_SERIALIZATION_VERSION_TOO_RECENT);
            if (displayDialogs)
            {
                App::uiThread->showOrHideProgressBar(false);
                App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_SERIALIZATION_VERSION_TOO_RECENT),VMESSAGEBOX_OKELI);
                App::uiThread->showOrHideProgressBar(true);
            }
        }
        if ((result==0)&&(App::mainWindow!=NULL))
        {
            if (displayMessages)
                App::addStatusbarMessage(IDS_COMPRESSION_SCHEME_NOT_SUPPORTED);
            if (displayDialogs)
            {
                App::uiThread->showOrHideProgressBar(false);
                App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_COMPRESSION_SCHEME_NOT_SUPPORTED),VMESSAGEBOX_OKELI);
                App::uiThread->showOrHideProgressBar(true);
            }
        }
#endif
        if (result==1)
        {
            std::vector<CButtonBlock*> loadedButtonBlockList;
            std::vector<CTextureObject*> loadedTextureList;
            int byteQuantity;
            std::string theName="";
            while (theName.compare(SER_END_OF_FILE)!=0)
            {
                theName=ar.readDataName();
                if (theName.compare(SER_END_OF_FILE)!=0)
                {
                    bool noHit=true;
                    if (theName.compare(SER_END_OF_OBJECT)==0) // those 2 lines can probably be removed..
                        noHit=false;
                    if (theName.compare(SER_BUTTON_BLOCK)==0)
                    {
                        ar >> byteQuantity;
                        CButtonBlock* it=new CButtonBlock(1,1,10,10,0);
                        it->serialize(ar);
                        loadedButtonBlockList.push_back(it);
                        noHit=false;
                    }
                    if (theName.compare(SER_TEXTURE)==0)
                    {
                        ar >> byteQuantity;
                        CTextureObject* it=new CTextureObject();
                        it->serialize(ar);
                        loadedTextureList.push_back(it);
                        noHit=false;
                    }
                    if (noHit)
                        ar.loadUnknownData();
                }
            }
            bool addUis=true;

#ifdef SIM_WITH_GUI
            if ((App::mainWindow!=NULL))
            {
                if (displayMessages)
                    App::addStatusbarMessage(IDSNS_UI_LOADED);
                if ((vrepVersionThatWroteThis>VREP_PROGRAM_VERSION_NB)&&displayDialogs)
                {
                    App::uiThread->showOrHideProgressBar(false);
                    App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_CUSTOM_USER_INTERFACES),strTranslate(IDS_SAVED_WITH_MORE_RECENT_VERSION_WARNING),VMESSAGEBOX_OKELI);
                    App::uiThread->showOrHideProgressBar(true);
                }
            }
#endif
            if (addUis)
            {
                // We add the 2Delements to the container and do the mapping:
                std::vector<int> buttonBlockMapping;
                std::vector<int> textureObjectMapping;
                for (int i=0;i<int(loadedButtonBlockList.size());i++)
                {
                    buttonBlockMapping.push_back(loadedButtonBlockList[i]->getBlockID()); // Old ID
                    App::ct->buttonBlockContainer->insertBlock(loadedButtonBlockList[i],false);
                    buttonBlockMapping.push_back(loadedButtonBlockList[i]->getBlockID()); // New ID
                    if (uiHandles!=NULL)
                        uiHandles->push_back(loadedButtonBlockList[i]->getBlockID());
                }
                App::ct->objCont->prepareFastLoadingMapping(buttonBlockMapping);

                for (int i=0;i<int(loadedTextureList.size());i++)
                {
                    textureObjectMapping.push_back(loadedTextureList[i]->getObjectID()); // Old ID
                    int nID=App::ct->textureCont->addObject(loadedTextureList[i],false);
                    textureObjectMapping.push_back(nID); // New ID
                }
                App::ct->objCont->prepareFastLoadingMapping(textureObjectMapping);

                for (int i=0;i<int(loadedButtonBlockList.size());i++)
                {
                    loadedButtonBlockList[i]->removeAllVisionSensorTextures();
                    loadedButtonBlockList[i]->removeAllObjectAttachements();
                    loadedButtonBlockList[i]->performTextureObjectLoadingMapping(&textureObjectMapping);
                }

                // We set ALL texture object dependencies (not just for loaded objects):
                App::ct->textureCont->clearAllDependencies();
                App::ct->buttonBlockContainer->setTextureDependencies();
                for (int i=0;i<int(App::ct->objCont->shapeList.size());i++)
                {
                    CShape* it=App::ct->objCont->getShape(App::ct->objCont->shapeList[i]);
                    it->geomData->setTextureDependencies(it->getID());
                }

                retVal=true;
            }
            else
            {
                for (int i=0;i<int(loadedButtonBlockList.size());i++)
                    delete loadedButtonBlockList[i];
                for (int i=0;i<int(loadedTextureList.size());i++)
                    delete loadedTextureList[i];
                retVal=false;
            }
            ar.readClose();
            App::uiThread->showOrHideProgressBar(false);
        }
        else
        {
            if (displayMessages)
                App::addStatusbarMessage(IDSNS_UI_COULD_NOT_BE_LOADED);
        }
        archive.close();
        file.close();

        // We need to make sure texture dependencies are ok:
        App::ct->textureCont->clearAllDependencies();
        for (int j=0;j<int(App::ct->objCont->shapeList.size());j++)
            App::ct->objCont->getShape(App::ct->objCont->shapeList[j])->geomData->setTextureDependencies(App::ct->objCont->shapeList[j]);
        App::ct->buttonBlockContainer->setTextureDependencies();

        if (doUndoThingInHere)
        {
            POST_SCENE_CHANGED_ANNOUNCEMENT(""); // ************************** UNDO thingy **************************
        }
    }
    else
    {
        if (displayMessages)
            App::addStatusbarMessage(IDSNS_ABORTED_FILE_DOES_NOT_EXIST);
    }
    return(retVal);
}

bool CFileOperations::saveScene(const char* pathAndFilename,bool displayMessages,bool displayDialogs,bool setCurrentDir)
{ // There is a similar routine in CUndoBuffer!!
    if (App::isFullScreen())
        displayDialogs=false;

#ifdef SIM_WITH_GUI
    if (App::mainWindow!=NULL)
        App::mainWindow->scintillaEditorContainer->applyChanges(true);
#endif

    void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_scenesave,NULL,NULL,NULL);
    delete[] (char*)returnVal;

    VFile myFile(pathAndFilename,VFile::CREATE_WRITE|VFile::SHARE_EXCLUSIVE,true);
#ifdef SIM_WITH_GUI
    if ((myFile.getFile()==NULL)&&(App::mainWindow!=NULL)&&displayDialogs)
    { // to avoid an error when saving a file that was opened while still attached to an email for instance
        App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_FILE_ACCESS),strTranslate(IDSN_ACCESS_TO_FILE_WAS_DENIED),VMESSAGEBOX_OKELI);
        return(false);
    }
#else
    return(false);
#endif
#ifdef SIM_WITH_GUI
    if (displayDialogs)
        App::uiThread->showOrHideProgressBar(true,-1,"Saving scene...");
#endif
    App::ct->mainSettings->setScenePathAndName(pathAndFilename);

    App::ct->luaScriptContainer->sceneOrModelAboutToBeSaved(-1);

    if (setCurrentDir)
        App::directories->sceneDirectory=App::ct->mainSettings->getScenePath();
    VArchive archive(&myFile,VArchive::STORE);
    CSer serObj(archive);

    std::string infoPrintOut(IDSN_SAVING_SCENE);
    infoPrintOut+=" (";
    infoPrintOut+=std::string(pathAndFilename)+"). ";
    infoPrintOut+=IDSNS_SERIALIZATION_VERSION_IS;
    infoPrintOut+=" ";
    infoPrintOut+=boost::lexical_cast<std::string>(CSer::SER_SERIALIZATION_VERSION)+".";
    if (displayMessages)
        App::addStatusbarMessage(infoPrintOut.c_str());

    serObj.writeOpen();
    App::ct->objCont->saveScene(serObj);
    serObj.writeClose(App::userSettings->compressFiles);

    if (displayMessages)
        App::addStatusbarMessage(IDSNS_SCENE_WAS_SAVED);

    archive.close();
    myFile.close();
    if (displayDialogs)
        App::uiThread->showOrHideProgressBar(false);
    App::setRebuildHierarchyFlag(); // we might have saved under a different name, we need to reflect it

    return(true);
}

bool CFileOperations::saveModel(int modelBaseDummyID,const char* pathAndFilename,bool displayMessages,bool displayDialogs,bool setCurrentDir)
{
    if (App::isFullScreen())
        displayDialogs=false;
    bool retVal=false;
    std::vector<int> sel;
    sel.push_back(modelBaseDummyID);

    C3DObject* modelBaseObject=App::ct->objCont->getObject(modelBaseDummyID);
    C3Vector minV,maxV;
    bool b=true;
    C7Vector modelTr(modelBaseObject->getCumulativeTransformationPart1());
    C3Vector modelBBSize;
    float modelNonDefaultTranslationStepSize=modelBaseObject->getNonDefaultTranslationStepSize();

    if (modelBaseObject->getGlobalMarkingBoundingBox(modelTr.getInverse(),minV,maxV,b,true,false))
    {
        modelBBSize=maxV-minV;
        modelTr.X+=modelTr.Q*((minV+maxV)*0.5);
    }
    else
        modelBBSize=C3Vector::zeroVector;

#ifdef SIM_WITH_GUI
    if (App::mainWindow!=NULL)
        App::mainWindow->scintillaEditorContainer->applyChanges(true);
#endif

    if (sel.size()>0)
    {
        // Following is very important for root objects!
        CSceneObjectOperations::addRootObjectChildrenToSelection(sel);
        void* plugRetVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_modelsave,NULL,NULL,NULL);
        delete[] (char*)plugRetVal;

        if (setCurrentDir)
            App::directories->modelDirectory=App::directories->getPathFromFull(pathAndFilename);
        std::string infoPrintOut(IDSNS_SAVING_MODEL);
        infoPrintOut+=" (";
        infoPrintOut+=std::string(pathAndFilename)+"). ";

        VFile myFile(pathAndFilename,VFile::CREATE_WRITE|VFile::SHARE_EXCLUSIVE);
        VArchive archive(&myFile,VArchive::STORE);
        CSer serObj(archive);

        infoPrintOut+=IDSNS_SERIALIZATION_VERSION_IS;
        infoPrintOut+=" ";
        infoPrintOut+=boost::lexical_cast<std::string>(CSer::SER_SERIALIZATION_VERSION)+".";

        serObj.writeOpen();
        App::ct->copyBuffer->serializeCurrentSelection(serObj,&sel,modelTr,modelBBSize,modelNonDefaultTranslationStepSize);
        serObj.writeClose(App::userSettings->compressFiles);

        archive.close();
        myFile.close();
        retVal=true;

        if (displayMessages)
            App::addStatusbarMessage(infoPrintOut.c_str());
        if (displayMessages)
            App::addStatusbarMessage(IDSNS_MODEL_WAS_SAVED);
    }
    return(retVal);
}

std::string CFileOperations::_getStringOfVersionAndLicenseThatTheFileWasWrittenWith(unsigned short vrepVer,int licenseType,char revision)
{
    if (vrepVer==0)
        return("");
    std::string retStr;
    retStr=tt::decorateString("",IDSNS_FILE_WAS_PREVIOUSLY_WRITTEN_WITH_VREP_VERSION," ");
    int v=vrepVer;
    retStr+=char('0')+(unsigned char)(v/10000);
    retStr+='.';
    v=v-(v/10000)*10000;
    retStr+=char('0')+(unsigned char)(v/1000);
    v=v-(v/1000)*1000;
    retStr+=char('0')+(unsigned char)(v/100);
    v=v-(v/100)*100;
    retStr+='.';
    retStr+=char('0')+(unsigned char)(v/10);
    v=v-(v/10)*10;
    retStr+=char('0')+(unsigned char)v;

    retStr+=" (rev ";
    retStr+=tt::FNb(0,(int)revision);
    retStr+=')';

    if (licenseType!=-1)
    {
        licenseType=(licenseType|0x00040000)-0x00040000;
        if (licenseType==VREP_LICENSE_VREP_DEMO)
            retStr+=" (V-REP EVAL/STUDENT license)";
        if (licenseType==VREP_LICENSE_VREP_PRO_EDU)
            retStr+=" (V-REP PRO EDU license)";
        if ((licenseType==VREP_LICENSE_VREP_PRO)||(licenseType==VREP_LICENSE_VREP_PRO_P))
            retStr+=" (V-REP PRO license)";
        if (licenseType==VREP_LICENSE_VREP_SUBLICENSE)
            retStr+=" (custom V-REP license)";
        if (licenseType==VREP_LICENSE_VREP_PLAYER)
            retStr+=" (V-REP PLAYER license)";
        if (licenseType==VREP_LICENSE_VREP_NAKED)
            retStr+=" (V-REP license, custom compilation)";
        if (licenseType==VREP_LICENSE_BLUE_REALITY)
            retStr+=" (BlueReality license)";
    }
    return(retStr);
}

bool CFileOperations::heightfieldImportRoutine(const std::string& pathName)
{
    if (VFile::doesFileExist(pathName))
    {
        try
        {
            std::vector<std::vector<float>*> readData;
            // We read each line at a time, which gives rows:
            int minRow=-1;

            std::string ext(CTTUtil::getLowerCaseString(VVarious::splitPath_fileExtension(pathName).c_str()));
            if ((ext.compare("csv")!=0)&&(ext.compare("txt")!=0))
            { // from image file
                int resX,resY,n;
                unsigned char* data=CImageLoaderSaver::load(pathName.c_str(),&resX,&resY,&n,3);
                if (data!=NULL)
                {
                    if ( (resX>1)&&(resY>1) )
                    {
                        int bytesPerPixel=0;
                        if (n==3)
                            bytesPerPixel=3;
                        if (n==4)
                            bytesPerPixel=4;
                        if (bytesPerPixel!=0)
                        {
                            for (int i=0;i<resY;i++)
                            {
                                std::vector<float>* lineVect=new std::vector<float>;
                                for (int j=0;j<resX;j++)
                                    lineVect->push_back(float(data[bytesPerPixel*(i*resX+j)+0]+data[bytesPerPixel*(i*resX+j)+1]+data[bytesPerPixel*(i*resX+j)+2])/768.0f);
                                readData.push_back(lineVect);
                            }
                            minRow=resX;
                        }
                    }
                    delete[] data;
                }
#ifdef SIM_WITH_GUI
                else
                    App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_IMPORT),strTranslate(IDS_TEXTURE_FILE_COULD_NOT_BE_LOADED),VMESSAGEBOX_OKELI);
#endif
            }
            else
            { // from csv or txt file:
                VFile file(pathName.c_str(),VFile::READ|VFile::SHARE_DENY_NONE);
                VArchive archive(&file,VArchive::LOAD);
                unsigned int currentPos=0;
                std::string line;
                while (archive.readSingleLine(currentPos,line,false))
                {
                    std::string word;
                    std::vector<float>* lineVect=new std::vector<float>;
                    while (tt::extractCommaSeparatedWord(line,word))
                    {
                        tt::removeSpacesAtBeginningAndEnd(word);
                        float val;
                        if (tt::getValidFloat(word,val))
                            lineVect->push_back(val);
                        else
                            break;
                    }
                    if (lineVect->size()!=0)
                    {
                        if ( (minRow==-1)||(int(lineVect->size())<minRow) )
                            minRow=int(lineVect->size());
                        readData.push_back(lineVect);
                    }
                    else
                    {
                        delete lineVect;
                        break;
                    }
                }
                archive.close();
                file.close();
            }
            if ( (readData.size()>1)&&(minRow>1) )
            {
                int xSize=minRow;
                int ySize=int(readData.size());

                // Display the heightfield scaling dialog:
                SUIThreadCommand cmdIn;
                SUIThreadCommand cmdOut;
                cmdIn.cmdId=HEIGHTFIELD_DIMENSION_DLG_UITHREADCMD;
                cmdIn.floatParams.push_back(10.0f);
                cmdIn.floatParams.push_back(float(ySize-1)/float(xSize-1));
                App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
                if (cmdOut.floatParams.size()==0)
                {
                    cmdOut.floatParams.push_back(1.0f);
                    cmdOut.floatParams.push_back(1.0f);
                }
                float pointSpacing=cmdOut.floatParams[0]/float(xSize-1);
                for (int i=0;i<int(readData.size());i++)
                {
                    for (int j=0;j<int(readData[i]->size());j++)
                    {
                        readData[i]->at(j)*=cmdOut.floatParams[1];
                    }
                }
                int shapeHandle=apiAddHeightfieldToScene(xSize,pointSpacing,readData,0.0f,2);
                App::ct->objCont->deselectObjects();
                App::ct->objCont->addObjectToSelection(shapeHandle);

            }
            for (int i=0;i<int(readData.size());i++)
                delete readData[i];
            return(readData.size()!=0);
        }
        catch(VFILE_EXCEPTION_TYPE e)
        {
            VFile::reportAndHandleFileExceptionError(e);
            return(false);
        }
    }
    return(false);
}

int CFileOperations::apiAddHeightfieldToScene(int xSize,float pointSpacing,const std::vector<std::vector<float>*>& readData,float shadingAngle,int options)
{ // options bits:
    // 0 set --> backfaces are culled
    // 1 set --> edges are visible
    // 2 set --> a normal shape is created instead
    // 4 set --> non respondable
    int ySize=int(readData.size());
    std::vector<float> allHeights;
    float maxHeight=-99999999.0f;
    float minHeight=+99999999.0f;
    for (int i=ySize-1;i>=0;i--)
    {
        for (int j=0;j<xSize;j++)
        {
            allHeights.push_back(readData[i]->at(j));
            if (readData[i]->at(j)>maxHeight)
                maxHeight=readData[i]->at(j);
            if (readData[i]->at(j)<minHeight)
                minHeight=readData[i]->at(j);
        }
    }
    CGeomProxy* geom=new CGeomProxy(allHeights,xSize,ySize,pointSpacing,maxHeight-minHeight);
    CShape* shape=new CShape();
    shape->setLocalTransformation(geom->getCreationTransformation());
    geom->setCreationTransformation(C7Vector::identityTransformation);
    shape->geomData=geom;

    if (options&4)
        ((CGeometric*)geom->geomInfo)->setPurePrimitiveType(sim_pure_primitive_none,1.0f,1.0f,1.0f);

    App::ct->objCont->addObjectToScene(shape,false);
    shape->setCulling((options&1)!=0);
    shape->setVisibleEdges((options&2)!=0);
    ((CGeometric*)shape->geomData->geomInfo)->setGouraudShadingAngle(shadingAngle);
    ((CGeometric*)shape->geomData->geomInfo)->setEdgeThresholdAngle(shadingAngle);
    ((CGeometric*)shape->geomData->geomInfo)->color.colors[0]=0.68f;
    ((CGeometric*)shape->geomData->geomInfo)->color.colors[1]=0.56f;
    ((CGeometric*)shape->geomData->geomInfo)->color.colors[2]=0.36f;
    ((CGeometric*)shape->geomData->geomInfo)->color.colors[6]=0.25f;
    ((CGeometric*)shape->geomData->geomInfo)->color.colors[7]=0.25f;
    ((CGeometric*)shape->geomData->geomInfo)->color.colors[8]=0.25f;
    std::string tempName(IDSOGL_HEIGHTFIELD);
    while (App::ct->objCont->getObject(tempName)!=NULL)
        tempName=tt::generateNewName_noDash(tempName);
    shape->setName(tempName);
    shape->alignBoundingBoxWithWorld();

    int propToRemove=sim_objectspecialproperty_collidable|sim_objectspecialproperty_measurable;
    shape->setLocalObjectSpecialProperty((shape->getLocalObjectSpecialProperty()|propToRemove)-propToRemove);
    shape->setRespondable((options&8)==0);
    shape->setShapeIsDynamicallyStatic(true);

    return(shape->getID());
}

void CFileOperations::addToRecentlyOpenedScenes(std::string filenameAndPath)
{
    CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
    std::string recentScenes[10];
    int sameIndex=-1;
    for (int i=0;i<10;i++)
    {
        std::string tmp("SIMSETTINGS_RECENTSCENE0");
        tmp[23]=48+i;
        cont.readData(tmp.c_str(),recentScenes[i]);
        if (recentScenes[i].compare(filenameAndPath)==0)
            sameIndex=i;
    }
    if (sameIndex==-1)
    {
        for (int i=8;i>=0;i--)
            recentScenes[i+1]=recentScenes[i];
    }
    else
    {
        for (int i=sameIndex;i>0;i--)
            recentScenes[i]=recentScenes[i-1];
    }
    recentScenes[0]=filenameAndPath;
    int cnt=0;
    for (int i=0;i<10;i++)
    {
        if (recentScenes[i].length()>3)
        {
            std::string tmp("SIMSETTINGS_RECENTSCENE0");
            tmp[23]=48+cnt;
            cont.writeData(tmp.c_str(),recentScenes[i],!App::userSettings->doNotWritePersistentData);
            cnt++;
        }
    }
}

void CFileOperations::_removeFromRecentlyOpenedScenes(std::string filenameAndPath)
{
    CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
    std::string recentScenes[10];
    int sameIndex=-1;
    for (int i=0;i<10;i++)
    {
        std::string tmp("SIMSETTINGS_RECENTSCENE0");
        tmp[23]=48+i;
        cont.readData(tmp.c_str(),recentScenes[i]);
        if (recentScenes[i].compare(filenameAndPath)==0)
            sameIndex=i;
    }
    if (sameIndex!=-1)
    {
        for (int i=sameIndex;i<9;i++)
            recentScenes[i]=recentScenes[i+1];
        recentScenes[9]="";
        int cnt=0;
        for (int i=0;i<10;i++)
        {
            if (recentScenes[i].length()>3)
            {
                std::string tmp("SIMSETTINGS_RECENTSCENE0");
                tmp[23]=48+cnt;
                cont.writeData(tmp.c_str(),recentScenes[i],!App::userSettings->doNotWritePersistentData);
                cnt++;
            }
        }
        for (int i=cnt;i<10;i++)
        {
            std::string tmp("SIMSETTINGS_RECENTSCENE0");
            tmp[23]=48+i;
            cont.writeData(tmp.c_str(),"",!App::userSettings->doNotWritePersistentData);
        }
    }
}

bool CFileOperations::apiExportIkContent(const char* pathAndName,bool displayDialogs)
{ // Call only from SIM thread
    CExtIkSer ar;
    App::ct->objCont->exportIkContent(ar);
    bool retVal=true;
    if (displayDialogs)
        App::uiThread->showOrHideProgressBar(true,-1,"Exporting IK content...");
    try
    {
        VFile myFile(pathAndName,VFile::CREATE_WRITE|VFile::SHARE_EXCLUSIVE);
        VArchive arch(&myFile,VArchive::STORE);

        int dataLength;
        unsigned char* data=ar.getBuffer(dataLength);
        for (int i=0;i<dataLength;i++)
            arch << data[i];
        arch.close();
        myFile.close();
    }
    catch(VFILE_EXCEPTION_TYPE e)
    {
        retVal=false;
    }
    if (displayDialogs)
        App::uiThread->showOrHideProgressBar(false);
    return(retVal);
}

#ifdef SIM_WITH_GUI
void CFileOperations::keyPress(int key)
{
    if (key==CTRL_S_KEY)
        processCommand(FILE_OPERATION_SAVE_SCENE_FOCMD);
    if (key==CTRL_O_KEY)
        processCommand(FILE_OPERATION_OPEN_SCENE_FOCMD);
    if (key==CTRL_W_KEY)
        processCommand(FILE_OPERATION_CLOSE_SCENE_FOCMD);
    if (key==CTRL_Q_KEY)
        processCommand(FILE_OPERATION_EXIT_SIMULATOR_FOCMD);
    if (key==CTRL_N_KEY)
        processCommand(FILE_OPERATION_NEW_SCENE_FOCMD);
}

void CFileOperations::addMenu(VMenu* menu)
{
    bool fileOpOk=(App::ct->simulation->isSimulationStopped())&&(App::getEditModeType()==NO_EDIT_MODE);
    bool simStoppedOrPausedNoEditMode=App::ct->simulation->isSimulationStopped()||App::ct->simulation->isSimulationPaused();
    bool fileOpOkAlsoDuringSimulation=(App::getEditModeType()==NO_EDIT_MODE);
    int selItems=App::ct->objCont->getSelSize();
    bool justModelSelected=false;
    if (selItems==1)
    {
        C3DObject* obj=App::ct->objCont->getObject(App::ct->objCont->getSelID(0));
        justModelSelected=(obj!=NULL)&&(obj->getModelBase());
    }
    std::vector<int> sel;
    sel.reserve(App::ct->objCont->getSelSize());
    for (int i=0;i<App::ct->objCont->getSelSize();i++)
        sel.push_back(App::ct->objCont->getSelID(i));
    CSceneObjectOperations::addRootObjectChildrenToSelection(sel);
    int shapeNumber=App::ct->objCont->getShapeNumberInSelection(&sel);
    int pathNumber=App::ct->objCont->getPathNumberInSelection(&sel);
    int graphNumber=App::ct->objCont->getGraphNumberInSelection(&sel);

    bool uiSaveOk=false;
    for (int i=0;i<int(App::ct->buttonBlockContainer->allBlocks.size());i++)
    {
        if ((App::ct->buttonBlockContainer->allBlocks[i]->getAttributes()&sim_ui_property_systemblock)==0)
        {
            uiSaveOk=true;
            break;
        }
    }

    menu->appendMenuItem(fileOpOk,false,FILE_OPERATION_NEW_SCENE_FOCMD,IDS_NEW_SCENE_MENU_ITEM);
    menu->appendMenuItem(fileOpOk,false,FILE_OPERATION_OPEN_SCENE_FOCMD,IDS_OPEN_SCENE___MENU_ITEM);

    // recent scene files:
    CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
    std::string recentScenes[10];
    int recentScenesCnt=0;
    for (int i=0;i<10;i++)
    {
        std::string tmp("SIMSETTINGS_RECENTSCENE0");
        tmp[23]=48+i;
        cont.readData(tmp.c_str(),recentScenes[i]);
        if (recentScenes[i].length()>3)
            recentScenesCnt++;
    }
    VMenu* recentSceneMenu=new VMenu();
    for (int i=0;i<10;i++)
    {
        if (recentScenes[i].length()>3)
            recentSceneMenu->appendMenuItem(fileOpOk,false,FILE_OPERATION_OPEN_RECENT_SCENE0_FOCMD+i,VVarious::splitPath_fileBaseAndExtension(recentScenes[i]).c_str());
    }
    menu->appendMenuAndDetach(recentSceneMenu,(recentScenesCnt>0)&&fileOpOk,IDS_OPEN_RECENT_SCENE_MENU_ITEM);

    menu->appendMenuItem(fileOpOkAlsoDuringSimulation,false,FILE_OPERATION_LOAD_MODEL_FOCMD,IDS_LOAD_MODEL___MENU_ITEM);
//    menu->appendMenuItem(fileOpOk,false,FILE_OPERATION_LOAD_UI_FOCMD,IDS_LOAD_UI___MENU_ITEM);
    menu->appendMenuSeparator();

    menu->appendMenuItem(fileOpOk,false,FILE_OPERATION_CLOSE_SCENE_FOCMD,IDS_CLOSE_SCENE_MENU_ITEM);
    menu->appendMenuSeparator();

    menu->appendMenuItem(fileOpOk,false,FILE_OPERATION_SAVE_SCENE_FOCMD,IDS_SAVE_SCENE_MENU_ITEM);
    menu->appendMenuItem(fileOpOk,false,FILE_OPERATION_SAVE_SCENE_AS_FOCMD,IDS_SAVE_SCENE_AS___MENU_ITEM);
    menu->appendMenuItem(fileOpOk&&justModelSelected,false,FILE_OPERATION_SAVE_MODEL_FOCMD,IDS_SAVE_MODEL_AS___MENU_ITEM);
//    menu->appendMenuItem(fileOpOk&&uiSaveOk,false,FILE_OPERATION_SAVE_UI_FOCMD,IDS_SAVE_UI_AS___MENU_ITEM);
    menu->appendMenuSeparator();


    VMenu* impMenu=new VMenu();
    impMenu->appendMenuItem(fileOpOk,false,FILE_OPERATION_IMPORT_MESH_FOCMD,IDS_IMPORT_MESH___MENU_ITEM);
    impMenu->appendMenuItem(fileOpOk,false,FILE_OPERATION_IMPORT_PATH_FOCMD,IDS_IMPORT_PATH___MENU_ITEM);
    impMenu->appendMenuItem(fileOpOk,false,FILE_OPERATION_IMPORT_HEIGHTFIELD_FOCMD,std::string(IDSN_IMPORT_HEIGHTFIELD)+"...");
    menu->appendMenuAndDetach(impMenu,true,IDSN_IMPORT_MENU_ITEM);

    VMenu* expMenu=new VMenu();
    expMenu->appendMenuItem(simStoppedOrPausedNoEditMode&&(shapeNumber>0),false,FILE_OPERATION_EXPORT_SHAPE_FOCMD,IDS_EXPORT_SHAPE_MENU_ITEM);
    expMenu->appendMenuItem(fileOpOk&&(graphNumber!=0),false,FILE_OPERATION_EXPORT_GRAPHS_FOCMD,IDS_EXPORT_SELECTED_GRAPHS_MENU_ITEM);
    expMenu->appendMenuItem(fileOpOk&&(pathNumber==1)&&(selItems==1),false,FILE_OPERATION_EXPORT_PATH_SIMPLE_POINTS_FOCMD,IDS_EXPORT_SELECTED_PATH_MENU_ITEM);
    expMenu->appendMenuItem(fileOpOk&&(pathNumber==1)&&(selItems==1),false,FILE_OPERATION_EXPORT_PATH_BEZIER_POINTS_FOCMD,IDS_EXPORT_SELECTED_PATH_BEZIER_CURVE_MENU_ITEM);
    expMenu->appendMenuItem(fileOpOk,false,FILE_OPERATION_EXPORT_IK_CONTENT_FOCMD,IDS_EXPORT_IK_CONTENT_MENU_ITEM);
    bool canExportDynamicContent=CPluginContainer::dyn_isDynamicContentAvailable()!=0;
    expMenu->appendMenuItem(canExportDynamicContent,false,FILE_OPERATION_EXPORT_DYNAMIC_CONTENT_FOCMD,IDSN_EXPORT_DYNAMIC_CONTENT);
    menu->appendMenuAndDetach(expMenu,true,IDSN_EXPORT_MENU_ITEM);
    menu->appendMenuSeparator();



    menu->appendMenuSeparator();
    menu->appendMenuItem(true,false,FILE_OPERATION_EXIT_SIMULATOR_FOCMD,IDS_EXIT_MENU_ITEM);
}

bool CFileOperations::_saveSceneWithDialogAndEverything()
{ // SHOULD ONLY BE CALLED BY THE MAIN SIMULATION THREAD!
    bool retVal=false;
    if (!App::ct->environment->getSceneLocked())
    {
        if (App::ct->mainSettings->getScenePathAndName()=="")
            retVal=_saveSceneAsWithDialogAndEverything(true);
        else
        {
            if ( (!App::ct->environment->getRequestFinalSave())||(VMESSAGEBOX_REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_FINAL_SCENE_SAVE_WARNING),VMESSAGEBOX_YES_NO)) )
            {
                if (App::ct->environment->getRequestFinalSave())
                    App::ct->environment->setSceneLocked();
                std::string infoPrintOut(IDSN_SAVING_SCENE);
                infoPrintOut+="...";
                App::addStatusbarMessage(infoPrintOut.c_str());
                saveScene(App::ct->mainSettings->getScenePathAndName().c_str(),true,true,true);
                addToRecentlyOpenedScenes(App::ct->mainSettings->getScenePathAndName());
                App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
            }
            retVal=true;
        }
    }
    else
        App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SCENE),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMESSAGEBOX_OKELI);
    return(retVal);
}

bool CFileOperations::_saveSceneAsWithDialogAndEverything(bool vrepFormat)
{
    bool retVal=false;
    if (!App::ct->environment->getSceneLocked())
    {
        if ( (!App::ct->environment->getRequestFinalSave())||(VMESSAGEBOX_REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_FINAL_SCENE_SAVE_WARNING),VMESSAGEBOX_YES_NO)) )
        {
            if (App::ct->environment->getRequestFinalSave())
                App::ct->environment->setSceneLocked();

            if (App::userSettings->askToIncludeScriptFiles&&App::ct->luaScriptContainer->hasSceneIncludeScripts())
            {
                if (VMESSAGEBOX_REPLY_YES==App::uiThread->messageBox_question(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_INCLUDE_SCRIPT_FILES_QUESTION),VMESSAGEBOX_YES_NO))
                    App::ct->luaScriptContainer->setSaveIncludeScriptFiles(true);
            }


            std::string infoPrintOut(IDSN_SAVING_SCENE);
            infoPrintOut+="...";
            App::addStatusbarMessage(infoPrintOut.c_str());
            std::string initPath;
            if (App::ct->mainSettings->getScenePathAndName().compare("")==0)
                initPath=App::directories->sceneDirectory;
            else
                initPath=App::ct->mainSettings->getScenePath();
            std::string filenameAndPath=App::uiThread->getSaveFileName(App::mainWindow,0,tt::decorateString("",strTranslate(IDSN_SAVING_SCENE),"..."),initPath,App::ct->mainSettings->getScenePathAndName(),false,"V-REP Scene",VREP_SCENE_EXTENSION);
            if (filenameAndPath.length()!=0)
            {
                if (saveScene(filenameAndPath.c_str(),true,true,true))
                {
                    addToRecentlyOpenedScenes(App::ct->mainSettings->getScenePathAndName());
                    App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
                    retVal=true;
                }
            }
            else
                App::addStatusbarMessage(IDSNS_ABORTED);
        }
    }
    else
        App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SCENE),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMESSAGEBOX_OKELI);
    App::ct->luaScriptContainer->setSaveIncludeScriptFiles(false);
    return(retVal);
}
#endif
