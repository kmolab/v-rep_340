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
#include "3Vector.h"
#include "4Vector.h"
#include "7Vector.h"
#include "3DObject.h"
#ifndef SIM_WITHOUT_QT_AT_ALL
#include <QObject>
#endif

struct SUIThreadCommand
{
    int cmdId;
    std::vector<bool> boolParams;
    std::vector<int> intParams;
    std::vector<unsigned int> uintParams;
    std::vector<float> floatParams;
    std::vector<void*> objectParams;
    std::vector<std::string> stringParams;
    std::vector<C3Vector> posParams;
    std::vector<C4Vector> quatParams;
    std::vector<C7Vector> transfParams;
};

enum {
        VMESSAGEBOX_OKELI           =0,
        VMESSAGEBOX_YES_NO          =1,
        VMESSAGEBOX_YES_NO_CANCEL   =2,
        VMESSAGEBOX_OK_CANCEL       =3,
};

enum{
        VMESSAGEBOX_REPLY_CANCEL    =0,
        VMESSAGEBOX_REPLY_NO        =1,
        VMESSAGEBOX_REPLY_YES       =2,
        VMESSAGEBOX_REPLY_OK        =3,
        VMESSAGEBOX_REPLY_ERROR     =4,
};

enum {  ADD_PRIMITIVE_WITH_DLG_UITHREADCMD=0,
        OPEN_OR_CLOSE_UITHREADCMD,

        CREATE_DEFAULT_MENU_BAR_UITHREADCMD,
        KEEP_THUMBNAIL_QUESTION_DLG_UITHREADCMD,
        SELECT_THUMBNAIL_DLG_UITHREADCMD,
        TEXTURE_SCALING_OPTION_DLG_UITHREADCMD,
        IMPORT_OPTION_DLG_UITHREADCMD,
        HEIGHTFIELD_DIMENSION_DLG_UITHREADCMD,
        OPEN_MODAL_USER_EDITOR_UITHREADCMD,
        OPEN_MODAL_CUSTOMIZATION_SCRIPT_EDITOR_UITHREADCMD,
        OPEN_MODAL_SCRIPT_SIMULATION_PARAMETERS_UITHREADCMD,
        OPEN_MODAL_MODEL_PROPERTIES_UITHREADCMD,
        REFRESH_DIALOGS_UITHREADCMD,
        CALL_DIALOG_FUNCTION_UITHREADCMD,
        CHECK_VERSION_UPDATE_UITHREADCMD,
        HANDLE_VERSION_UPDATE_DISPLAY_UITHREADCMD,
        TOGGLE_VISUALIZATION_UITHREADCMD,
        VISUALIZATION_OFF_UITHREADCMD,
        VISUALIZATION_ON_UITHREADCMD,
        FLASH_STATUSBAR_UITHREADCMD,
        OPEN_HIERARCHY_UITHREADCMD,
        CLOSE_HIERARCHY_UITHREADCMD,

        SCINTILLA_CONSOLES_START_SCUITHREADCMD,
        SCINTILLA_CONSOLES_SIMULATION_ENDED_SCUITHREADCMD,
        SCINTILLA_CONSOLES_HANDLE_CONSOLE_SCUITHREADCMD,
        SCINTILLA_CONSOLES_END_SCUITHREADCMD,

        SCINTILLA_EDITOR_START_SEUITHREADCMD,
        SCINTILLA_EDITOR_OPEN_FOR_SCRIPT_SEUITHREADCMD,
        SCINTILLA_EDITOR_CLOSE_ALL_SEUITHREADCMD,
        SCINTILLA_EDITOR_CLOSE_OR_SHOW_ALL_SEUITHREADCMD,
        SCINTILLA_EDITOR_CLOSE_SEUITHREADCMD,
        SCINTILLA_EDITOR_UPDATE_SEUITHREADCMD,
        SCINTILLA_EDITOR_APPLY_CHANGES_SEUITHREADCMD,
        SCINTILLA_EDITOR_END_SEUITHREADCMD,

        MAIN_WINDOW_START_MWUITHREADCMD,
        MAIN_WINDOW_NON_EDIT_MODE_DLG_CLOSE_MWUITHREADCMD,
        MAIN_WINDOW_NON_EDIT_MODE_DLG_REOPEN_MWUITHREADCMD,
        MAIN_WINDOW_VIEW_SELECTOR_DLG_CLOSE_MWUITHREADCMD,
        MAIN_WINDOW_VIEW_SELECTOR_DLG_REOPEN_MWUITHREADCMD,
        MAIN_WINDOW_PAGE_SELECTOR_DLG_CLOSE_MWUITHREADCMD,
        MAIN_WINDOW_PAGE_SELECTOR_DLG_REOPEN_MWUITHREADCMD,
        MAIN_WINDOW_SCENE_SELECTOR_DLG_CLOSE_MWUITHREADCMD,
        MAIN_WINDOW_SCENE_SELECTOR_DLG_REOPEN_MWUITHREADCMD,
        MAIN_WINDOW_SET_FULLSCREEN_MWTHREADCMD,
        MAIN_WINDOW_ACTIVATE_MWUITHREADCMD,
        MAIN_WINDOW_CLOSE_DLG_MWUITHREADCMD,
        MAIN_WINDOW_OPEN_DLG_OR_BRING_TO_FRONT_MWUITHREADCMD,
        MAIN_WINDOW_END_MWUITHREADCMD,

        SERIAL_PORT_START_SPUITHREADCMD,
        SERIAL_PORT_ALT_OPEN_SPUITHREADCMD,
        SERIAL_PORT_ALT_CLOSE_SPUITHREADCMD,
        SERIAL_PORT_ALT_CHECK_SPUITHREADCMD,
        SERIAL_PORT_ALT_SEND_SPUITHREADCMD,
        SERIAL_PORT_ALT_RECEIVE_SPUITHREADCMD,
        SERIAL_PORT_END_SPUITHREADCMD,

        DISPLAY_CONVEX_DECOMPOSITION_DIALOG_UITHREADCMD,
        SHOW_HIDE_EMERGENCY_STOP_BUTTON_UITHREADCMD,
        DISPLAY_MESH_DECIMATION_DIALOG_UITHREADCMD,
        DISPLAY_MESH_INSIDE_EXTRACTION_ITERATIONS_DIALOG_UITHREADCMD,

        DISPLAY_OR_HIDE_PROGRESS_DIALOG_UITHREADCMD,


        PLUGIN_START_PLUGUITHREADCMD,
        PLUGIN_LOAD_AND_START_PLUGUITHREADCMD,
        PLUGIN_STOP_AND_UNLOAD_PLUGUITHREADCMD,
        PLUGIN_END_PLUGUITHREADCMD,

        DESTROY_GL_TEXTURE_UITHREADCMD,
        DISPLAY_MSGBOX_API_UITHREADCMD,
        DISPLAY_FILE_DLG_UITHREADCMD,
        DISPLAY_MSG_WITH_CHECKBOX_UITHREADCMD,
        DISPLAY_MSGBOX_UITHREADCMD,
        DETECT_VISION_SENSOR_ENTITY_UITHREADCMD,
        DISPLAY_SAVE_DLG_UITHREADCMD,
        DISPLAY_OPEN_DLG_UITHREADCMD,
        DISPLAY_OPEN_DLG_MULTIFILE_UITHREADCMD,
        DLG_INPUT_GET_FLOAT_UITHREADCMD,
        CREATE_GL_CONTEXT_FBO_TEXTURE_IF_NEEDED_UITHREADCMD,
        SET_FILEDIALOG_NATIVE_UITHREADCMD,
        SHOW_PRIMITIVE_SHAPE_DLG_UITHREADCMD,

        DESTROY_UIOBJECT_UITHREADCMD,

        ADD_STATUSBAR_MESSAGE_UITHREADCMD,
        NO_SIGNAL_SLOT_EXIT_UITHREADCMD,

        INSTANCE_ABOUT_TO_BE_CREATED_UITHREADCMD,
        INSTANCE_WAS_JUST_CREATED_UITHREADCMD,
        INSTANCE_ABOUT_TO_CHANGE_UITHREADCMD,
        INSTANCE_ABOUT_TO_BE_DESTROYED_UITHREADCMD,
        NEW_SCENE_NAME_UITHREADCMD,
        SIMULATION_ABOUT_TO_START_UITHREADCMD,
        SIMULATION_JUST_ENDED_UITHREADCMD,
        EDIT_MODE_ABOUT_TO_START_UITHREADCMD,
        EDIT_MODE_JUST_ENDED_UITHREADCMD,
        COPY_TEXT_TO_CLIPBOARD_UITHREADCMD,
     };

#ifdef SIM_WITHOUT_QT_AT_ALL
class CUiThread
{
#else
class CUiThread : public QObject
{
    Q_OBJECT
#endif
public:
    CUiThread();
    virtual ~CUiThread();

    bool executeCommandViaUiThread(SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut);
    void showOrHideProgressBar(bool show,float pos=999.0f,const char* txt=NULL);
    bool showOrHideEmergencyStop(bool show,const char* txt);

#ifdef SIM_WITHOUT_QT_AT_ALL
    void processGuiEventsUntilQuit_noSignalSlots();
#endif

private:
    int _frameId;
    bool _frame_bufferMainDisplayStateVariables;
    int _lastFrameId;

#ifndef SIM_WITHOUT_QT_AT_ALL
signals:
    void _executeCommandViaUiThread(SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut);

private slots:
    void __executeCommandViaUiThread(SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut);
#else
    void __executeCommandViaUiThread(SUIThreadCommand* cmdIn,SUIThreadCommand* cmdOut);
    VMutex _noSigSlotMutex;
    int _noSigSlot_cnter;
    SUIThreadCommand* _noSigSlot_cmdIn;
    SUIThreadCommand* _noSigSlot_cmdOut;
#endif

#ifdef SIM_WITH_GUI
public:
    int getLastFrameId();
    void setLastFrameId(int fid);
    void requestSceneRender(bool bufferMainDisplayStateVariables); // not waiting
    void requestSceneRender_wait();
    int messageBox_api(int boxType,int buttons,const char* title,const char* message);
    bool messageBox_checkbox(void* parentWidget,const std::string& title,const std::string& message,const std::string& checkboxMessage);
    void setFileDialogsNative(int n);
    std::string getOpenFileName(void* parentWidget,unsigned short option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2="",const std::string& extension3="",const std::string& extension4="",const std::string& extension5="",const std::string& extension6="",const std::string& extension7="",const std::string& extension8="",const std::string& extension9="",const std::string& extension10="");
    void getOpenFileNames(std::vector<std::string>& files,void* parentWidget,unsigned short option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2="",const std::string& extension3="",const std::string& extension4="",const std::string& extension5="",const std::string& extension6="",const std::string& extension7="",const std::string& extension8="",const std::string& extension9="",const std::string& extension10="");
    std::string getSaveFileName(void* parentWidget,unsigned short option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2="",const std::string& extension3="",const std::string& extension4="",const std::string& extension5="",const std::string& extension6="",const std::string& extension7="",const std::string& extension8="",const std::string& extension9="",const std::string& extension10="");
    std::string getOpenOrSaveFileName_api(int mode,const char* title,const char* startPath,const char* initName,const char* extName,const char* ext);
    bool showPrimitiveShapeDialog(int type,const C3Vector* optionalSizesIn,C3Vector& sizes,int subdiv[3],int& faces,int& sides,int& discSubdiv,bool& smooth,int& openEnds,bool& dynamic,bool& pure,bool& cone,float& density,bool& negVolume,float& negVolumeScaling);
    unsigned short messageBox_informationSystemModal(void* parentWidget,const std::string& title,const std::string& message,unsigned short flags);
    unsigned short messageBox_information(void* parentWidget,const std::string& title,const std::string& message,unsigned short flags);
    unsigned short messageBox_question(void* parentWidget,const std::string& title,const std::string& message,unsigned short flags);
    unsigned short messageBox_warning(void* parentWidget,const std::string& title,const std::string& message,unsigned short flags);
    unsigned short messageBox_critical(void* parentWidget,const std::string& title,const std::string& message,unsigned short flags);
    bool dialogInputGetFloat(void* parentWidget,const char* title,const char* msg,float def,float minV,float maxV,int decimals,float* outFloat);

private:
    unsigned short _messageBox(int type,void* parentWidget,const std::string& title,const std::string& message,unsigned short flags);

signals:
    void _requestSceneRender_wait(bool bufferMainDisplayStateVariables);
    void _requestSceneRender(bool bufferMainDisplayStateVariables);

private slots:
    void __requestSceneRender(bool bufferMainDisplayStateVariables);
    void __requestSceneRender_wait(bool bufferMainDisplayStateVariables);
#endif
};
