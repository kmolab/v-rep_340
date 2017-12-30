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
#include "userSettings.h"
#include "confReaderAndWriter.h"
#include "global.h"
#include "threadPool.h"
#include "debugLogFile.h"
#include "tt.h"
#include "easyLock.h"
#include "vVarious.h"
#include "app.h"
#ifdef SIM_WITH_GUI
    #include "scintillaDlg.h"
    #include "scintillaConsoleDlg.h"
    #include "vDialog.h"
#endif

#define USER_SETTINGS_FILENAME "usrset.txt"

#define _USR_ANTIALIASING "antialiasing"
#define _USR_DISPLAY_WORLD_REF "displayWorldRef"
#define _USR_USE_GLFINISH "useGlFinish"
#define _USR_USE_GLFINISH_VISION_SENSORS "useGlFinish_visionSensors"
#define _USR_OGL_COMPATIBILITY_TWEAK_1 "oglCompatibilityTweak1"

#define _USR_STEREO_DIST "stereoDist"
#define _USR_VSYNC "vsync"
#define _USR_DEBUG_OPENGL "debugOpenGl"
#define _USR_DISPLAY_BOUNDING_BOXES "displayBoundingBoxeWhenObjectSelected"
#define _USR_REMOVE_IDENTICAL_VERTICES "removeIdenticalVertices"
#define _USR_IDENTICAL_VERTICES_TOLERANCE "identicalVerticesTolerance"
#define _USR_REMOVE_IDENTICAL_TRIANGLES "removeIdenticalTriangles"
#define _USR_TRIANGLE_WINDING_CHECK "triangleWindingCheck"
#define _USR_PROCESSOR_CORE_AFFINITY "processorCoreAffinity"
#define _USR_DYNAMIC_ACTIVITY_RANGE "dynamicActivityRange"
#define _USR_FREE_SERVER_PORT_START "freeServerPortStart"
#define _USR_FREE_SERVER_PORT_RANGE "freeServerPortRange"
#define _USR_GUI_FONT_SIZE_WIN "guiFontSize_Win"
#define _USR_GUI_FONT_SIZE_MAC "guiFontSize_Mac"
#define _USR_GUI_FONT_SIZE_LINUX "guiFontSize_Linux"
#define _USR_TRANSLATION_STEP_SIZE "objectTranslationStepSize"
#define _USR_ROTATION_STEP_SIZE "objectRotationStepSize"
#define _USR_COMPRESS_FILES "compressFiles"
#define _USR_TRIANGLE_COUNT_IN_OBB "triCountInOBB"
#define _USR_APPROXIMATED_NORMALS "saveApproxNormals"
#define _USR_PACK_INDICES "packIndices"
#define _USR_UNDO_REDO_ENABLED "undoRedoEnabled"
#define _USR_UNDO_REDO_PARTIAL_WITH_CAMERAS "undoRedoOnlyPartialWithCameras"
#define _USR_UNDO_REDO_LEVEL_COUNT "undoRedoLevelCount"
#define _USR_RUN_CUSTOMIZATION_SCRIPTS "runCustomizationScripts"
#define _USR_TEST1 "test1"
#define _USR_ORDER_HIERARCHY_ALPHABETICALLY "orderHierarchyAlphabetically"
#define _USR_MAC_CHILD_DIALOG_TYPE "macChildDialogType"
#define _USR_USE_EXTERNAL_LUA_LIBRARY "useExternalLuaLibrary"
#define _USR_RAISE_ERROR_WITH_API_SCRIPT_FUNCTIONS "raiseErrorWithApiScriptFunctions"
#define _USR_DESKTOP_RECORDING_INDEX "desktopRecordingIndex"
#define _USR_DESKTOP_RECORDING_WIDTH "desktopRecordingWidth"


#define _USR_IDLE_FPS "idleFps"
#define _USR_UNDO_REDO_MAX_BUFFER_SIZE "undoRedoMaxBufferSize"
#define _USR_ALWAYS_SHOW_CONSOLE "alwaysShowConsole"
#define _USR_DEBUG_GUI_SIM_THREAD_SYNC_ACTIVITY "debugGuiThreadAndSimThreadSynchronizationActivity"
#define _USR_DEBUG_GUI_SIM_THREAD_SYNC_FAILS_ACTIVITY "debugGuiThreadAndSimThreadSynchronizationLockFailActivity"
#define _USR_DEBUG_INTERNAL_FUNCTION_ACCESS "debugInternalFunctionAccess"
#define _USR_DEBUG_C_API_ACCESS "debugCApiAccess"
#define _USR_DEBUG_LUA_API_ACCESS "debugLuaApiAccess"
#define _USR_DEBUG_THREAD_SWITCHES "debugThreadSwitches"
#define _USR_DEBUG_EASYLOCK_ACTIVITY "debugEasylockActivity"
#define _USR_DEBUG_TO_FILE "sendDebugInformationToFile"
#define _USR_FORCE_BUG_FIX_REL_30002 "forceBugFix_rel30002"
#define _USR_ALLOW_TRANSPARENT_DIALOGS  "allowTransparentDialogs"
#define _USR_DIALOG_TRANSPARENCY_FACTOR "dialogTransparencyFactor"
#define _USR_STATUSBAR_INITIALLY_VISIBLE "statusbarInitiallyVisible"
#define _USR_MODELBROWSER_INITIALLY_VISIBLE "modelBrowserInitiallyVisible"
#define _USR_SCENEHIERARCHY_INITIALLY_VISIBLE "sceneHierarchyInitiallyVisible"
#define _USR_AUTO_SAVE_DELAY "autoSaveDelay"
#define _USR_BULLET_SERIALIZATION_BUFFER "bulletSerializationBuffer"
#define _USR_ASK_TO_INCLUDE_SCRIPT_FILES "askToIncludeScriptFiles"
#define _USR_TIME_FOR_UNDO_REDO_TOO_LONG_WARNING "timeInMsForUndoRedoTooLongWarning"
#define _USR_MIDDLE_MOUSE_BUTTON_SWITCHES_MODES "middleMouseButtonSwitchesModes"
#define _USR_NAVIGATION_BACKWARD_COMPATIBILITY_MODE "navigationBackwardCompatibility"
#define _USR_COLOR_ADJUST_BACK_COMPATIBILITY "colorAdjust_backCompatibility"
#define _USR_SPECIFIC_GPU_TWEAK "specificGpuTweak"
#define _USR_ENABLE_OLD_PATH_PLANNING_GUI "enableOldPathPlanningGui"
#define _USR_ENABLE_OLD_MOTION_PLANNING_GUI "enableOldMotionPlanningGui"
#define _USR_USE_ALTERNATE_SERIAL_PORT_ROUTINES "useAlternateSerialPortRoutines"
#define _USR_ENABLE_OPENGL_BASED_CUSTOM_UI_EDITOR "enableOpenGlBasedCustomUiEditor"

#define _USR_ABORT_SCRIPT_EXECUTION_BUTTON "abortScriptExecutionButton"
#define _USR_RENDERING_SURFACE_VERTICAL_SHIFT "renderingSurfaceVShift"
#define _USR_RENDERING_SURFACE_VERTICAL_RESIZE "renderingSurfaceVResize"
#define _USR_USE_EXTERNAL_SCRIPT_EDITOR "useExternalScriptEditor"
#define _USR_EXTERNAL_SCRIPT_EDITOR "externalScriptEditor"
#define _USR_ADDITIONAL_LUA_PATH "additionalLuaPath"

#define _USR_DIRECTORY_FOR_SCENES "defaultDirectoryForScenes"
#define _USR_DIRECTORY_FOR_MODELS "defaultDirectoryForModels"
#define _USR_DIRECTORY_FOR_CAD "defaultDirectoryForCadFiles"
#define _USR_DIRECTORY_FOR_MISC "defaultDirectoryForMiscFiles"
#define _USR_DIRECTORY_FOR_SCRIPT_EDITOR "defaultDirectoryForExternalScriptEditor"
#define _USR_DIRECTORY_FOR_REMOTE_API "defaultDirectoryForRemoteApiFiles"


#define _USR_THREADED_RENDERING_DURING_SIMULATION "threadedRenderingDuringSimulation"
#define _USR_OFFSCREEN_CONTEXT_TYPE "offscreenContextType"
#define _USR_FBO_TYPE "fboType"
#define _USR_FORCE_FBO_VIA_EXT "forceFboViaExt"
#define _USR_VBO_OPERATION "vboOperation"
#define _USR_VBO_PERSISTENCE_IN_MS "vboPersistenceInMs"
#define _USR_DESIRED_OPENGL_MAJOR "desiredOpenGlMajor"
#define _USR_DESIRED_OPENGL_MINOR "desiredOpenGlMinor"
#define _USR_HIGH_RES_DISPLAY "highResDisplay"
#define _USR_NO_EDGES_WHEN_MOUSE_DOWN "noEdgesWhenMouseDownInCameraView"
#define _USR_NO_TEXTURES_WHEN_MOUSE_DOWN "noTexturesWhenMouseDownInCameraView"
#define _USR_NO_CUSTOM_UIS_WHEN_MOUSE_DOWN "noCustomUisWhenMouseDownInCameraView"
#define _USR_HIERARCHY_REFRESH_CNT "hierarchyRefreshCnt"

#define _USR_THREADED_CHILD_SCRIPT_COLOR_BACKGROUND "threadedChildScriptColor_background"
#define _USR_THREADED_CHILD_SCRIPT_COLOR_SELECTION "threadedChildScriptColor_selection"
#define _USR_THREADED_CHILD_SCRIPT_COLOR_COMMENT "threadedChildScriptColor_comment"
#define _USR_THREADED_CHILD_SCRIPT_COLOR_NUMBER "threadedChildScriptColor_number"
#define _USR_THREADED_CHILD_SCRIPT_COLOR_STRING "threadedChildScriptColor_string"
#define _USR_THREADED_CHILD_SCRIPT_COLOR_CHARACTER "threadedChildScriptColor_character"
#define _USR_THREADED_CHILD_SCRIPT_COLOR_OPERATOR "threadedChildScriptColor_operator"
#define _USR_THREADED_CHILD_SCRIPT_COLOR_PREPROCESSOR "threadedChildScriptColor_preprocessor"
#define _USR_THREADED_CHILD_SCRIPT_COLOR_IDENTIFIER "threadedChildScriptColor_identifier"
#define _USR_THREADED_CHILD_SCRIPT_COLOR_WORD "threadedChildScriptColor_word"
#define _USR_THREADED_CHILD_SCRIPT_COLOR_WORD2 "threadedChildScriptColor_word2"
#define _USR_THREADED_CHILD_SCRIPT_COLOR_WORD3 "threadedChildScriptColor_word3"
#define _USR_THREADED_CHILD_SCRIPT_COLOR_WORD4 "threadedChildScriptColor_word4"

#define _USR_NON_THREADED_CHILD_SCRIPT_COLOR_BACKGROUND "nonThreadedChildScriptColor_background"
#define _USR_NON_THREADED_CHILD_SCRIPT_COLOR_SELECTION "nonThreadedChildScriptColor_selection"
#define _USR_NON_THREADED_CHILD_SCRIPT_COLOR_COMMENT "nonThreadedChildScriptColor_comment"
#define _USR_NON_THREADED_CHILD_SCRIPT_COLOR_NUMBER "nonThreadedChildScriptColor_number"
#define _USR_NON_THREADED_CHILD_SCRIPT_COLOR_STRING "nonThreadedChildScriptColor_string"
#define _USR_NON_THREADED_CHILD_SCRIPT_COLOR_CHARACTER "nonThreadedChildScriptColor_character"
#define _USR_NON_THREADED_CHILD_SCRIPT_COLOR_OPERATOR "nonThreadedChildScriptColor_operator"
#define _USR_NON_THREADED_CHILD_SCRIPT_COLOR_PREPROCESSOR "nonThreadedChildScriptColor_preprocessor"
#define _USR_NON_THREADED_CHILD_SCRIPT_COLOR_IDENTIFIER "nonThreadedChildScriptColor_identifier"
#define _USR_NON_THREADED_CHILD_SCRIPT_COLOR_WORD "nonThreadedChildScriptColor_word"
#define _USR_NON_THREADED_CHILD_SCRIPT_COLOR_WORD2 "nonThreadedChildScriptColor_word2"
#define _USR_NON_THREADED_CHILD_SCRIPT_COLOR_WORD3 "nonThreadedChildScriptColor_word3"
#define _USR_NON_THREADED_CHILD_SCRIPT_COLOR_WORD4 "nonThreadedChildScriptColor_word4"

#define _USR_MAIN_SCRIPT_COLOR_BACKGROUND "mainScriptColor_background"
#define _USR_MAIN_SCRIPT_COLOR_SELECTION "mainScriptColor_selection"
#define _USR_MAIN_SCRIPT_COLOR_COMMENT "mainScriptColor_comment"
#define _USR_MAIN_SCRIPT_COLOR_NUMBER "mainScriptColor_number"
#define _USR_MAIN_SCRIPT_COLOR_STRING "mainScriptColor_string"
#define _USR_MAIN_SCRIPT_COLOR_CHARACTER "mainScriptColor_character"
#define _USR_MAIN_SCRIPT_COLOR_OPERATOR "mainScriptColor_operator"
#define _USR_MAIN_SCRIPT_COLOR_PREPROCESSOR "mainScriptColor_preprocessor"
#define _USR_MAIN_SCRIPT_COLOR_IDENTIFIER "mainScriptColor_identifier"
#define _USR_MAIN_SCRIPT_COLOR_WORD "mainScriptColor_word"
#define _USR_MAIN_SCRIPT_COLOR_WORD2 "mainScriptColor_word2"
#define _USR_MAIN_SCRIPT_COLOR_WORD3 "mainScriptColor_word3"
#define _USR_MAIN_SCRIPT_COLOR_WORD4 "mainScriptColor_word4"

#define _USR_JOINT_CALLBACK_SCRIPT_COLOR_BACKGROUND "jointCallbackScriptColor_background"
#define _USR_JOINT_CALLBACK_SCRIPT_COLOR_SELECTION "jointCallbackScriptColor_selection"
#define _USR_JOINT_CALLBACK_SCRIPT_COLOR_COMMENT "jointCallbackScriptColor_comment"
#define _USR_JOINT_CALLBACK_SCRIPT_COLOR_NUMBER "jointCallbackScriptColor_number"
#define _USR_JOINT_CALLBACK_SCRIPT_COLOR_STRING "jointCallbackScriptColor_string"
#define _USR_JOINT_CALLBACK_SCRIPT_COLOR_CHARACTER "jointCallbackScriptColor_character"
#define _USR_JOINT_CALLBACK_SCRIPT_COLOR_OPERATOR "jointCallbackScriptColor_operator"
#define _USR_JOINT_CALLBACK_SCRIPT_COLOR_PREPROCESSOR "jointCallbackScriptColor_preprocessor"
#define _USR_JOINT_CALLBACK_SCRIPT_COLOR_IDENTIFIER "jointCallbackScriptColor_identifier"
#define _USR_JOINT_CALLBACK_SCRIPT_COLOR_WORD "jointCallbackScriptColor_word"
#define _USR_JOINT_CALLBACK_SCRIPT_COLOR_WORD2 "jointCallbackScriptColor_word2"
#define _USR_JOINT_CALLBACK_SCRIPT_COLOR_WORD3 "jointCallbackScriptColor_word3"
#define _USR_JOINT_CALLBACK_SCRIPT_COLOR_WORD4 "jointCallbackScriptColor_word4"

#define _USR_CONTACT_CALLBACK_SCRIPT_COLOR_BACKGROUND "contactCallbackScriptColor_background"
#define _USR_CONTACT_CALLBACK_SCRIPT_COLOR_SELECTION "contactCallbackScriptColor_selection"
#define _USR_CONTACT_CALLBACK_SCRIPT_COLOR_COMMENT "contactCallbackScriptColor_comment"
#define _USR_CONTACT_CALLBACK_SCRIPT_COLOR_NUMBER "contactCallbackScriptColor_number"
#define _USR_CONTACT_CALLBACK_SCRIPT_COLOR_STRING "contactCallbackScriptColor_string"
#define _USR_CONTACT_CALLBACK_SCRIPT_COLOR_CHARACTER "contactCallbackScriptColor_character"
#define _USR_CONTACT_CALLBACK_SCRIPT_COLOR_OPERATOR "contactCallbackScriptColor_operator"
#define _USR_CONTACT_CALLBACK_SCRIPT_COLOR_PREPROCESSOR "contactCallbackScriptColor_preprocessor"
#define _USR_CONTACT_CALLBACK_SCRIPT_COLOR_IDENTIFIER "contactCallbackScriptColor_identifier"
#define _USR_CONTACT_CALLBACK_SCRIPT_COLOR_WORD "contactCallbackScriptColor_word"
#define _USR_CONTACT_CALLBACK_SCRIPT_COLOR_WORD2 "contactCallbackScriptColor_word2"
#define _USR_CONTACT_CALLBACK_SCRIPT_COLOR_WORD3 "contactCallbackScriptColor_word3"
#define _USR_CONTACT_CALLBACK_SCRIPT_COLOR_WORD4 "contactCallbackScriptColor_word4"

#define _USR_GENERAL_CALLBACK_SCRIPT_COLOR_BACKGROUND "generalCallbackScriptColor_background"
#define _USR_GENERAL_CALLBACK_SCRIPT_COLOR_SELECTION "generalCallbackScriptColor_selection"
#define _USR_GENERAL_CALLBACK_SCRIPT_COLOR_COMMENT "generalCallbackScriptColor_comment"
#define _USR_GENERAL_CALLBACK_SCRIPT_COLOR_NUMBER "generalCallbackScriptColor_number"
#define _USR_GENERAL_CALLBACK_SCRIPT_COLOR_STRING "generalCallbackScriptColor_string"
#define _USR_GENERAL_CALLBACK_SCRIPT_COLOR_CHARACTER "generalCallbackScriptColor_character"
#define _USR_GENERAL_CALLBACK_SCRIPT_COLOR_OPERATOR "generalCallbackScriptColor_operator"
#define _USR_GENERAL_CALLBACK_SCRIPT_COLOR_PREPROCESSOR "generalCallbackScriptColor_preprocessor"
#define _USR_GENERAL_CALLBACK_SCRIPT_COLOR_IDENTIFIER "generalCallbackScriptColor_identifier"
#define _USR_GENERAL_CALLBACK_SCRIPT_COLOR_WORD "generalCallbackScriptColor_word"
#define _USR_GENERAL_CALLBACK_SCRIPT_COLOR_WORD2 "generalCallbackScriptColor_word2"
#define _USR_GENERAL_CALLBACK_SCRIPT_COLOR_WORD3 "generalCallbackScriptColor_word3"
#define _USR_GENERAL_CALLBACK_SCRIPT_COLOR_WORD4 "generalCallbackScriptColor_word4"

#define _USR_CUSTOMIZATION_SCRIPT_COLOR_BACKGROUND "customizationScriptColor_background"
#define _USR_CUSTOMIZATION_SCRIPT_COLOR_SELECTION "customizationScriptColor_selection"
#define _USR_CUSTOMIZATION_SCRIPT_COLOR_COMMENT "customizationScriptColor_comment"
#define _USR_CUSTOMIZATION_SCRIPT_COLOR_NUMBER "customizationScriptColor_number"
#define _USR_CUSTOMIZATION_SCRIPT_COLOR_STRING "customizationScriptColor_string"
#define _USR_CUSTOMIZATION_SCRIPT_COLOR_CHARACTER "customizationScriptColor_character"
#define _USR_CUSTOMIZATION_SCRIPT_COLOR_OPERATOR "customizationScriptColor_operator"
#define _USR_CUSTOMIZATION_SCRIPT_COLOR_PREPROCESSOR "customizationScriptColor_preprocessor"
#define _USR_CUSTOMIZATION_SCRIPT_COLOR_IDENTIFIER "customizationScriptColor_identifier"
#define _USR_CUSTOMIZATION_SCRIPT_COLOR_WORD "customizationScriptColor_word"
#define _USR_CUSTOMIZATION_SCRIPT_COLOR_WORD2 "customizationScriptColor_word2"
#define _USR_CUSTOMIZATION_SCRIPT_COLOR_WORD3 "customizationScriptColor_word3"
#define _USR_CUSTOMIZATION_SCRIPT_COLOR_WORD4 "customizationScriptColor_word4"

#define _USR_ADD_ON_SCRIPT_COLOR_BACKGROUND "addOnScriptColor_background"
#define _USR_ADD_ON_SCRIPT_COLOR_SELECTION "addOnScriptColor_selection"
#define _USR_ADD_ON_SCRIPT_COLOR_COMMENT "addOnScriptColor_comment"
#define _USR_ADD_ON_SCRIPT_COLOR_NUMBER "addOnScriptColor_number"
#define _USR_ADD_ON_SCRIPT_COLOR_STRING "addOnScriptColor_string"
#define _USR_ADD_ON_SCRIPT_COLOR_CHARACTER "addOnScriptColor_character"
#define _USR_ADD_ON_SCRIPT_COLOR_OPERATOR "addOnScriptColor_operator"
#define _USR_ADD_ON_SCRIPT_COLOR_PREPROCESSOR "addOnScriptColor_preprocessor"
#define _USR_ADD_ON_SCRIPT_COLOR_IDENTIFIER "addOnScriptColor_identifier"
#define _USR_ADD_ON_SCRIPT_COLOR_WORD "addOnScriptColor_word"
#define _USR_ADD_ON_SCRIPT_COLOR_WORD2 "addOnScriptColor_word2"
#define _USR_ADD_ON_SCRIPT_COLOR_WORD3 "addOnScriptColor_word3"
#define _USR_ADD_ON_SCRIPT_COLOR_WORD4 "addOnScriptColor_word4"

#define _USR_VISION_SENSORS_USE_GUI_WINDOWED "visionSensorsUseGuiThread_windowed"
#define _USR_VISION_SENSORS_USE_GUI_HEADLESS "visionSensorsUseGuiThread_headless"
#define _USR_FILE_DIALOGS_NATIVE "fileDialogs"
#define _USR_MOUSE_WHEEL_ZOOM_FACTOR "mouseWheelZoomFactor"

#define _USR_DO_NOT_WRITE_PERSISTENT_DATA "doNotWritePersistentData"
#define _USR_DO_NOT_SHOW_CRASH_RECOVERY_MESSAGE "doNotShowCrashRecoveryMessage"
#define _USR_DO_NOT_SHOW_UPDATE_CHECK_MESSAGE "doNotShowUpdateCheckMessage"
#define _USR_DO_NOT_SHOW_SCENE_SELECTION_THUMBNAILS "doNotShowSceneSelectionThumbnails"
#define _USR_DO_NOT_SHOW_PROGRESS_BARS "doNotShowProgressBars"
#define _USR_DO_NOT_SHOW_ACKNOWLEDGMENT_MESSAGES "doNotShowAcknowledgmentMessages"
#define _USR_REDIRECT_STATUSBAR_MSG_TO_CONSOLE_IN_HEADLESS_MODE "redirectStatusbarMsgToConsoleInHeadlessMode"

#define _USR_SCRIPT_EDITOR_FONT "scriptEditorFont"
#define _USR_SCRIPT_EDITOR_FONT_SIZE "scriptEditorFontSize"
#define _USR_AUX_CONSOLE_FONT "auxConsoleFont"
#define _USR_AUX_CONSOLE_FONT_SIZE "auxConsoleFontSize"

#define _USR_CONNECTION_ADDRESS "conParam1"
#define _USR_CONNECTION_PORT "conParam2"


CUserSettings::CUserSettings()
{
    // Debugging section:
    // *****************************
    alwaysShowConsole=false;

    // Rendering section:
    // *****************************
    _idleFps=8;
    threadedRenderingDuringSimulation=0; // keep 0, will otherwise lead to very frequent crashes!!
    desiredOpenGlMajor=-1; // default
    desiredOpenGlMinor=-1; // default
    offscreenContextType=-1; // default type
    fboType=-1; // default type
    forceFboViaExt=false; // default
    vboOperation=-1; // default
    vboPersistenceInMs=5000; // default
    oglCompatibilityTweak1=false;
    visionSensorsUseGuiThread_windowed=-1; // default
    visionSensorsUseGuiThread_headless=-1; // default
    useGlFinish=false;
    useGlFinish_visionSensors=false;
    vsync=0;
    debugOpenGl=false;
    stereoDist=0.0f; // default, no stereo!


    // Visual section:
    // *****************************
    renderingSurfaceVShift=0;
    renderingSurfaceVResize=0;
    scriptEditorFont=""; // default
    scriptEditorFontSize=-1; // default
    auxConsoleFont=""; // default
    auxConsoleFontSize=-1; // default
    displayWorldReference=true;
    antiAliasing=false;
    displayBoundingBoxeWhenObjectSelected=true;
    guiFontSize_Win=11;
    guiFontSize_Mac=10;
    guiFontSize_Linux=11; // 10; changed on 19/8/2015
    allowTransparentDialogs=false;
    dialogTransparencyFactor=0.4f;
    statusbarInitiallyVisible=true;
    modelBrowserInitiallyVisible=true;
    sceneHierarchyInitiallyVisible=true;
    highResDisplay=-1;
    noEdgesWhenMouseDownInCameraView=false;
    noTexturesWhenMouseDownInCameraView=false;
    noCustomUisWhenMouseDownInCameraView=true;
    hierarchyRefreshCnt=3;

    _setIntVector3(threadedChildScriptColor_background,180,190,190);
    _setIntVector3(threadedChildScriptColor_selection,128,128,255);
    _setIntVector3(threadedChildScriptColor_comment,0,140,0);
    _setIntVector3(threadedChildScriptColor_number,220,0,220);
    _setIntVector3(threadedChildScriptColor_string,255,255,0);
    _setIntVector3(threadedChildScriptColor_character,255,255,0);
    _setIntVector3(threadedChildScriptColor_operator,0,0,0);
    _setIntVector3(threadedChildScriptColor_preprocessor,0,128,128);
    _setIntVector3(threadedChildScriptColor_identifier,64,64,64);
    _setIntVector3(threadedChildScriptColor_word,0,0,255);
    _setIntVector3(threadedChildScriptColor_word2,152,0,0);
    _setIntVector3(threadedChildScriptColor_word3,220,80,20);
    _setIntVector3(threadedChildScriptColor_word4,152,64,0);

    _setIntVector3(nonThreadedChildScriptColor_background,190,190,190);
    _setIntVector3(nonThreadedChildScriptColor_selection,128,128,255);
    _setIntVector3(nonThreadedChildScriptColor_comment,0,140,0);
    _setIntVector3(nonThreadedChildScriptColor_number,220,0,220);
    _setIntVector3(nonThreadedChildScriptColor_string,255,255,0);
    _setIntVector3(nonThreadedChildScriptColor_character,255,255,0);
    _setIntVector3(nonThreadedChildScriptColor_operator,0,0,0);
    _setIntVector3(nonThreadedChildScriptColor_preprocessor,0,128,128);
    _setIntVector3(nonThreadedChildScriptColor_identifier,64,64,64);
    _setIntVector3(nonThreadedChildScriptColor_word,0,0,255);
    _setIntVector3(nonThreadedChildScriptColor_word2,152,0,0);
    _setIntVector3(nonThreadedChildScriptColor_word3,220,80,20);
    _setIntVector3(nonThreadedChildScriptColor_word4,152,64,0);

    _setIntVector3(mainScriptColor_background,190,175,175);
    _setIntVector3(mainScriptColor_selection,128,128,255);
    _setIntVector3(mainScriptColor_comment,0,140,0);
    _setIntVector3(mainScriptColor_number,220,0,220);
    _setIntVector3(mainScriptColor_string,255,255,0);
    _setIntVector3(mainScriptColor_character,255,255,0);
    _setIntVector3(mainScriptColor_operator,0,0,0);
    _setIntVector3(mainScriptColor_preprocessor,0,128,128);
    _setIntVector3(mainScriptColor_identifier,64,64,64);
    _setIntVector3(mainScriptColor_word,0,0,255);
    _setIntVector3(mainScriptColor_word2,152,0,0);
    _setIntVector3(mainScriptColor_word3,220,80,20);
    _setIntVector3(mainScriptColor_word4,152,64,0);

    _setIntVector3(jointCallbackScriptColor_background,190,190,190);
    _setIntVector3(jointCallbackScriptColor_selection,128,128,255);
    _setIntVector3(jointCallbackScriptColor_comment,0,140,0);
    _setIntVector3(jointCallbackScriptColor_number,220,0,220);
    _setIntVector3(jointCallbackScriptColor_string,255,255,0);
    _setIntVector3(jointCallbackScriptColor_character,255,255,0);
    _setIntVector3(jointCallbackScriptColor_operator,0,0,0);
    _setIntVector3(jointCallbackScriptColor_preprocessor,0,128,128);
    _setIntVector3(jointCallbackScriptColor_identifier,64,64,64);
    _setIntVector3(jointCallbackScriptColor_word,0,0,255);
    _setIntVector3(jointCallbackScriptColor_word2,152,0,0);
    _setIntVector3(jointCallbackScriptColor_word3,220,80,20);
    _setIntVector3(jointCallbackScriptColor_word4,152,64,0);

    _setIntVector3(contactCallbackScriptColor_background,190,190,190);
    _setIntVector3(contactCallbackScriptColor_selection,128,128,255);
    _setIntVector3(contactCallbackScriptColor_comment,0,140,0);
    _setIntVector3(contactCallbackScriptColor_number,220,0,220);
    _setIntVector3(contactCallbackScriptColor_string,255,255,0);
    _setIntVector3(contactCallbackScriptColor_character,255,255,0);
    _setIntVector3(contactCallbackScriptColor_operator,0,0,0);
    _setIntVector3(contactCallbackScriptColor_preprocessor,0,128,128);
    _setIntVector3(contactCallbackScriptColor_identifier,64,64,64);
    _setIntVector3(contactCallbackScriptColor_word,0,0,255);
    _setIntVector3(contactCallbackScriptColor_word2,152,0,0);
    _setIntVector3(contactCallbackScriptColor_word3,220,80,20);
    _setIntVector3(contactCallbackScriptColor_word4,152,64,0);

    _setIntVector3(generalCallbackScriptColor_background,190,190,190);
    _setIntVector3(generalCallbackScriptColor_selection,128,128,255);
    _setIntVector3(generalCallbackScriptColor_comment,0,140,0);
    _setIntVector3(generalCallbackScriptColor_number,220,0,220);
    _setIntVector3(generalCallbackScriptColor_string,255,255,0);
    _setIntVector3(generalCallbackScriptColor_character,255,255,0);
    _setIntVector3(generalCallbackScriptColor_operator,0,0,0);
    _setIntVector3(generalCallbackScriptColor_preprocessor,0,128,128);
    _setIntVector3(generalCallbackScriptColor_identifier,64,64,64);
    _setIntVector3(generalCallbackScriptColor_word,0,0,255);
    _setIntVector3(generalCallbackScriptColor_word2,152,0,0);
    _setIntVector3(generalCallbackScriptColor_word3,220,80,20);
    _setIntVector3(generalCallbackScriptColor_word4,152,64,0);

    _setIntVector3(customizationScriptColor_background,150,150,150);
    _setIntVector3(customizationScriptColor_selection,128,128,255);
    _setIntVector3(customizationScriptColor_comment,0,120,0);
    _setIntVector3(customizationScriptColor_number,220,0,220);
    _setIntVector3(customizationScriptColor_string,205,205,0);
    _setIntVector3(customizationScriptColor_character,205,205,0);
    _setIntVector3(customizationScriptColor_operator,0,0,0);
    _setIntVector3(customizationScriptColor_preprocessor,0,128,128);
    _setIntVector3(customizationScriptColor_identifier,70,70,70);
    _setIntVector3(customizationScriptColor_word,0,0,255);
    _setIntVector3(customizationScriptColor_word2,152,0,0);
    _setIntVector3(customizationScriptColor_word3,220,80,20);
    _setIntVector3(customizationScriptColor_word4,152,64,0);

    _setIntVector3(addOnScriptColor_background,240,240,240);
    _setIntVector3(addOnScriptColor_selection,128,128,255);
    _setIntVector3(addOnScriptColor_comment,0,164,0);
    _setIntVector3(addOnScriptColor_number,220,0,220);
    _setIntVector3(addOnScriptColor_string,205,205,0);
    _setIntVector3(addOnScriptColor_character,205,205,0);
    _setIntVector3(addOnScriptColor_operator,0,0,0);
    _setIntVector3(addOnScriptColor_preprocessor,0,128,128);
    _setIntVector3(addOnScriptColor_identifier,100,100,100);
    _setIntVector3(addOnScriptColor_word,0,0,255);
    _setIntVector3(addOnScriptColor_word2,152,0,0);
    _setIntVector3(addOnScriptColor_word3,220,80,20);
    _setIntVector3(addOnScriptColor_word4,152,64,0);


    // Directories section:
    // *****************************
    defaultDirectoryForScenes="";
    defaultDirectoryForModels="";
    defaultDirectoryForCadFiles="";
    defaultDirectoryForMiscFiles="";
    defaultDirectoryForExternalScriptEditor="";
    defaultDirectoryForRemoteApiFiles="";


    // Serialization section:
    // *****************************
    autoSaveDelay=2; // 2 minutes for an auto-save by default. set to 0 for no auto-save
    doNotWritePersistentData=false;
    compressFiles=true;
    saveApproxNormals=true;
    packIndices=true;
    fileDialogs=-1; // default
    bulletSerializationBuffer=10000000;
    askToIncludeScriptFiles=false; // this one is however not put to / read from usrset.txt



    // Undo/redo section:
    // *****************************
    _undoRedoEnabled=true;
    _undoRedoOnlyPartialWithCameras=true;
    undoRedoLevelCount=16;
    undoRedoMaxBufferSize=80000000;
    timeInMsForUndoRedoTooLongWarning=200;


    // Messaging section:
    // *****************************
    doNotShowCrashRecoveryMessage=false;
    doNotShowUpdateCheckMessage=false;
    doNotShowSceneSelectionThumbnails=false;
    doNotShowProgressBars=false;
    doNotShowAcknowledgmentMessages=false;
    redirectStatusbarMsgToConsoleInHeadlessMode=false;


    // Compatibility section:
    // *****************************
    middleMouseButtonSwitchesModes=false; // set to false on 28/9/2014
    navigationBackwardCompatibility=false;
    colorAdjust_backCompatibility=1.0f; // default
    specificGpuTweak=false; // default
    enableOldPathPlanningGui=false; // default
    enableOldMotionPlanningGui=false; // default
    useAlternateSerialPortRoutines=false;
    enableOpenGlBasedCustomUiEditor=false;



    // Various section:
    // *****************************
    mouseWheelZoomFactor=1.0f; // default
    dynamicActivityRange=1000.0f;
    _translationStepSize=0.025f;
    _rotationStepSize=5.0f*degToRad;
    freeServerPortStart=20000;
    _nextfreeServerPortToUse=freeServerPortStart;
    freeServerPortRange=2000;
    _useExternalScriptEditor=false;
    externalScriptEditor="c:/Program Files (x86)/Notepad++/notepad++";
    _useExternalScriptEditorToSave=_useExternalScriptEditor;
    externalScriptEditorToSave=externalScriptEditor;
    abortScriptExecutionButton=2;
    triCountInOBB=8; // gave best results in 2009/07/21
    identicalVerticesCheck=true;
    identicalVerticesTolerance=0.0001f;
    identicalTrianglesCheck=true;
    identicalTrianglesWindingCheck=true;
    runCustomizationScripts=true;
    test1=false;
    orderHierarchyAlphabetically=false;
    macChildDialogType=-1; // default
    useExternalLuaLibrary=false; //when using the LUA JIT, we get crashes because of other Lua modules (e.g. LuaSocket). Probably those other modules too need to be recompiled
    raiseErrorWithApiScriptFunctions=true;
    additionalLuaPath="";
    desktopRecordingIndex=0;
    desktopRecordingWidth=-1;

    forceBugFix_rel30002=false;

    // Other, not serialized:
    groupSelectionColor.setDefaultValues();
    groupSelectionColor.setColor(0.75f,0.0f,0.4f,sim_colorcomponent_ambient_diffuse);
    groupSelectionColor.setColor(0.2f,0.0f,0.1f,sim_colorcomponent_emission);

    loadUserSettings();
}

CUserSettings::~CUserSettings()
{
}

bool CUserSettings::useBuiltInScriptEditor()
{
    return(!_useExternalScriptEditor);
}

void CUserSettings::setUseBuiltInScriptEditorToSave(bool u)
{
    _useExternalScriptEditorToSave=!u;
}

bool CUserSettings::getUseBuiltInScriptEditorToSave()
{
    return(!_useExternalScriptEditorToSave);
}

void CUserSettings::setTranslationStepSize(float s)
{
    float sc=1.0f;
    if ((s>=0.0075f)&&(s<0.075f))
        sc=10.0f;
    if (s>=0.075f)
        sc=100.0f;
    if (s<0.0015f*sc)
        s=0.001f*sc;
    else
    {
        if (s<0.00375f*sc)
        {
            if (sc<2.0f)
                s=0.002f*sc;
            else
                s=0.0025f*sc;
        }
        else
            s=0.005f*sc;
    }
    _translationStepSize=s;
}
float CUserSettings::getTranslationStepSize()
{
    return(_translationStepSize);
}

void CUserSettings::setRotationStepSize(float s)
{
    if (s<1.5f*degToRad)
        s=1.0f*degToRad;
    else
    {
        if (s<3.5f*degToRad)
            s=2.0f*degToRad;
        else
        {
            if (s<7.5f*degToRad)
                s=5.0f*degToRad;
            else
            {
                if (s<12.5f*degToRad)
                    s=10.0f*degToRad;
                else
                {
                    if (s<22.5f*degToRad)
                        s=15.0f*degToRad;
                    else
                    {
                        if (s<37.5f*degToRad)
                            s=30.0f*degToRad;
                        else
                            s=45.0f*degToRad;
                    }
                }
            }
        }
    }
    _rotationStepSize=s;
}

float CUserSettings::getRotationStepSize()
{
    return(_rotationStepSize);
}

void CUserSettings::setUndoRedoEnabled(bool isEnabled)
{
    _undoRedoEnabled=isEnabled;
    if (App::ct->undoBufferContainer!=NULL)
        App::ct->undoBufferContainer->emptySceneProcedure();
}

bool CUserSettings::getUndoRedoEnabled()
{
    return(_undoRedoEnabled);
}

bool CUserSettings::getUndoRedoOnlyPartialWithCameras()
{
    return(_undoRedoOnlyPartialWithCameras);
}

int CUserSettings::getIdleFps()
{
    return(_idleFps);
}

void CUserSettings::setIdleFps(int fps)
{
    _idleFps=tt::getLimitedInt(0,25,fps);
}

int CUserSettings::getNextFreeServerPortToUse()
{
    return(_nextfreeServerPortToUse);
}

void CUserSettings::setNextFreeServerPortToUse(int p)
{
    _nextfreeServerPortToUse=p;
}

void CUserSettings::_setIntVector3(int v[3],int a,int b,int c)
{
    v[0]=a;
    v[1]=b;
    v[2]=c;
}

void CUserSettings::saveUserSettings()
{
    CConfReaderAndWriter c;

    c.addRandomLine("// Debugging");
    c.addRandomLine("// =================================================");
    c.addBoolean(_USR_ALWAYS_SHOW_CONSOLE,alwaysShowConsole,"");
#ifdef SIM_WITHOUT_QT_AT_ALL
    c.addBoolean(_USR_DEBUG_GUI_SIM_THREAD_SYNC_ACTIVITY,false,"will also slow down V-REP");
    c.addBoolean(_USR_DEBUG_GUI_SIM_THREAD_SYNC_FAILS_ACTIVITY,false,"");
#else
    c.addBoolean(_USR_DEBUG_GUI_SIM_THREAD_SYNC_ACTIVITY,CSimAndUiThreadSync::getShowActivityInConsole(),"will also slow down V-REP");
    c.addBoolean(_USR_DEBUG_GUI_SIM_THREAD_SYNC_FAILS_ACTIVITY,CSimAndUiThreadSync::getShowLockFailsActivityInConsole(),"");
#endif
    c.addBoolean(_USR_DEBUG_INTERNAL_FUNCTION_ACCESS,(CFuncDebug::getDebugMask()&1)!=0,"will also heavily slow down V-REP");
    c.addBoolean(_USR_DEBUG_C_API_ACCESS,(CFuncDebug::getDebugMask()&2)!=0,"will also drastically slow down V-REP");
    c.addBoolean(_USR_DEBUG_LUA_API_ACCESS,(CFuncDebug::getDebugMask()&4)!=0,"will also slow down V-REP");
    c.addBoolean(_USR_DEBUG_THREAD_SWITCHES,CEasyLock::getShowActivity(),"will also slow down V-REP");
    c.addBoolean(_USR_DEBUG_EASYLOCK_ACTIVITY,CThreadPool::getShowThreadSwitches(),"will also slow down V-REP");
    c.addBoolean(_USR_DEBUG_TO_FILE,CDebugLogFile::getDebugToFile(),"if true, debug info is sent to debugLog.txt");
    c.addRandomLine("");
    c.addRandomLine("");


    c.addRandomLine("// Rendering");
    c.addRandomLine("// =================================================");
    c.addInteger(_USR_IDLE_FPS,_idleFps,"");
    c.addInteger(_USR_THREADED_RENDERING_DURING_SIMULATION,threadedRenderingDuringSimulation,"recommended to keep 0 (-1=disabled, 0=pre-enabled, 1=enabled).");
    c.addInteger(_USR_DESIRED_OPENGL_MAJOR,desiredOpenGlMajor,"recommended to keep -1.");
    c.addInteger(_USR_DESIRED_OPENGL_MINOR,desiredOpenGlMinor,"recommended to keep -1.");
    c.addInteger(_USR_OFFSCREEN_CONTEXT_TYPE,offscreenContextType,"recommended to keep -1 (-1=default, 0=Qt offscreen, 1=QGLWidget/QOpenGLWidget visible, 2=QGLWidget/QOpenGLWidget invisible).");
    c.addInteger(_USR_FBO_TYPE,fboType,"recommended to keep -1 (-1=default, 0=native, 1=QOpenGLFramebufferObject).");
    c.addBoolean(_USR_FORCE_FBO_VIA_EXT,forceFboViaExt,"recommended to keep false.");
    c.addInteger(_USR_VBO_OPERATION,vboOperation,"recommended to keep -1 (-1=default, 0=always off, 1=on when available).");
    c.addInteger(_USR_VBO_PERSISTENCE_IN_MS,vboPersistenceInMs,"recommended to keep 5000.");
    c.addBoolean(_USR_OGL_COMPATIBILITY_TWEAK_1,oglCompatibilityTweak1,"recommended to keep false since it causes small memory leaks.");
    c.addInteger(_USR_VISION_SENSORS_USE_GUI_WINDOWED,visionSensorsUseGuiThread_windowed,"recommended to keep -1 (-1=default, 0=GUI when not otherwise possible, 1=always GUI).");
    c.addInteger(_USR_VISION_SENSORS_USE_GUI_HEADLESS,visionSensorsUseGuiThread_headless,"recommended to keep -1 (-1=default, 0=GUI when not otherwise possible, 1=always GUI).");
    c.addBoolean(_USR_USE_GLFINISH,useGlFinish,"recommended to keep false. Graphic card dependent.");
    c.addBoolean(_USR_USE_GLFINISH_VISION_SENSORS,useGlFinish_visionSensors,"recommended to keep false. Graphic card dependent.");
    c.addInteger(_USR_VSYNC,vsync,"recommended to keep at 0. Graphic card dependent.");
    c.addBoolean(_USR_DEBUG_OPENGL,debugOpenGl,"");
    c.addFloat(_USR_STEREO_DIST,stereoDist,"0=no stereo, otherwise the intra occular distance (0.0635 for the human eyes).");
    c.addInteger(_USR_HIGH_RES_DISPLAY,highResDisplay,"-1=automatic, 0=disabled, 1=enabled.");
    c.addBoolean(_USR_NO_EDGES_WHEN_MOUSE_DOWN,noEdgesWhenMouseDownInCameraView,"if true, rendering is faster during mouse/view interaction");
    c.addBoolean(_USR_NO_TEXTURES_WHEN_MOUSE_DOWN,noTexturesWhenMouseDownInCameraView,"if true, rendering is faster during mouse/view interaction");
    c.addBoolean(_USR_NO_CUSTOM_UIS_WHEN_MOUSE_DOWN,noCustomUisWhenMouseDownInCameraView,"if true, rendering is faster during mouse/view interaction");
    c.addInteger(_USR_HIERARCHY_REFRESH_CNT,hierarchyRefreshCnt,"");


    c.addRandomLine("");
    c.addRandomLine("");


    c.addRandomLine("// Visual");
    c.addRandomLine("// =================================================");
    c.addInteger(_USR_RENDERING_SURFACE_VERTICAL_SHIFT,renderingSurfaceVShift,"");
    c.addInteger(_USR_RENDERING_SURFACE_VERTICAL_RESIZE,renderingSurfaceVResize,"");
    c.addBoolean(_USR_DISPLAY_WORLD_REF,displayWorldReference,"");
    c.addBoolean(_USR_ANTIALIASING,antiAliasing,"");
    c.addBoolean(_USR_DISPLAY_BOUNDING_BOXES,displayBoundingBoxeWhenObjectSelected,"");
    c.addInteger(_USR_GUI_FONT_SIZE_WIN,guiFontSize_Win,"");
    c.addInteger(_USR_GUI_FONT_SIZE_MAC,guiFontSize_Mac,"");
    c.addInteger(_USR_GUI_FONT_SIZE_LINUX,guiFontSize_Linux,"");
    c.addBoolean(_USR_ALLOW_TRANSPARENT_DIALOGS,allowTransparentDialogs,"");
    c.addFloat(_USR_DIALOG_TRANSPARENCY_FACTOR,dialogTransparencyFactor,"");
    c.addBoolean(_USR_STATUSBAR_INITIALLY_VISIBLE,statusbarInitiallyVisible,"");
    c.addBoolean(_USR_MODELBROWSER_INITIALLY_VISIBLE,modelBrowserInitiallyVisible,"");
    c.addBoolean(_USR_SCENEHIERARCHY_INITIALLY_VISIBLE,sceneHierarchyInitiallyVisible,"");

    c.addString(_USR_SCRIPT_EDITOR_FONT,scriptEditorFont,"empty=default.");
    c.addInteger(_USR_SCRIPT_EDITOR_FONT_SIZE,scriptEditorFontSize,"-1=default.");
    c.addString(_USR_AUX_CONSOLE_FONT,auxConsoleFont,"empty=default");
    c.addInteger(_USR_AUX_CONSOLE_FONT_SIZE,auxConsoleFontSize,"-1=default.");

    c.addIntVector3(_USR_MAIN_SCRIPT_COLOR_BACKGROUND,mainScriptColor_background,"");
    c.addIntVector3(_USR_MAIN_SCRIPT_COLOR_SELECTION,mainScriptColor_selection,"");
    c.addIntVector3(_USR_MAIN_SCRIPT_COLOR_COMMENT,mainScriptColor_comment,"");
    c.addIntVector3(_USR_MAIN_SCRIPT_COLOR_NUMBER,mainScriptColor_number,"");
    c.addIntVector3(_USR_MAIN_SCRIPT_COLOR_STRING,mainScriptColor_string,"");
    c.addIntVector3(_USR_MAIN_SCRIPT_COLOR_CHARACTER,mainScriptColor_character,"");
    c.addIntVector3(_USR_MAIN_SCRIPT_COLOR_OPERATOR,mainScriptColor_operator,"");
    c.addIntVector3(_USR_MAIN_SCRIPT_COLOR_PREPROCESSOR,mainScriptColor_preprocessor,"");
    c.addIntVector3(_USR_MAIN_SCRIPT_COLOR_IDENTIFIER,mainScriptColor_identifier,"");
    c.addIntVector3(_USR_MAIN_SCRIPT_COLOR_WORD,mainScriptColor_word,"");
    c.addIntVector3(_USR_MAIN_SCRIPT_COLOR_WORD2,mainScriptColor_word2,"");
    c.addIntVector3(_USR_MAIN_SCRIPT_COLOR_WORD3,mainScriptColor_word3,"");
    c.addIntVector3(_USR_MAIN_SCRIPT_COLOR_WORD4,mainScriptColor_word4,"");

    c.addIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_BACKGROUND,nonThreadedChildScriptColor_background,"");
    c.addIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_SELECTION,nonThreadedChildScriptColor_selection,"");
    c.addIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_COMMENT,nonThreadedChildScriptColor_comment,"");
    c.addIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_NUMBER,nonThreadedChildScriptColor_number,"");
    c.addIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_STRING,nonThreadedChildScriptColor_string,"");
    c.addIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_CHARACTER,nonThreadedChildScriptColor_character,"");
    c.addIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_OPERATOR,nonThreadedChildScriptColor_operator,"");
    c.addIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_PREPROCESSOR,nonThreadedChildScriptColor_preprocessor,"");
    c.addIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_IDENTIFIER,nonThreadedChildScriptColor_identifier,"");
    c.addIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_WORD,nonThreadedChildScriptColor_word,"");
    c.addIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_WORD2,nonThreadedChildScriptColor_word2,"");
    c.addIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_WORD3,nonThreadedChildScriptColor_word3,"");
    c.addIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_WORD4,nonThreadedChildScriptColor_word4,"");

    c.addIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_BACKGROUND,threadedChildScriptColor_background,"");
    c.addIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_SELECTION,threadedChildScriptColor_selection,"");
    c.addIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_COMMENT,threadedChildScriptColor_comment,"");
    c.addIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_NUMBER,threadedChildScriptColor_number,"");
    c.addIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_STRING,threadedChildScriptColor_string,"");
    c.addIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_CHARACTER,threadedChildScriptColor_character,"");
    c.addIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_OPERATOR,threadedChildScriptColor_operator,"");
    c.addIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_PREPROCESSOR,threadedChildScriptColor_preprocessor,"");
    c.addIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_IDENTIFIER,threadedChildScriptColor_identifier,"");
    c.addIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_WORD,threadedChildScriptColor_word,"");
    c.addIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_WORD2,threadedChildScriptColor_word2,"");
    c.addIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_WORD3,threadedChildScriptColor_word3,"");
    c.addIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_WORD4,threadedChildScriptColor_word4,"");

    c.addIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_BACKGROUND,jointCallbackScriptColor_background,"");
    c.addIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_SELECTION,jointCallbackScriptColor_selection,"");
    c.addIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_COMMENT,jointCallbackScriptColor_comment,"");
    c.addIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_NUMBER,jointCallbackScriptColor_number,"");
    c.addIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_STRING,jointCallbackScriptColor_string,"");
    c.addIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_CHARACTER,jointCallbackScriptColor_character,"");
    c.addIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_OPERATOR,jointCallbackScriptColor_operator,"");
    c.addIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_PREPROCESSOR,jointCallbackScriptColor_preprocessor,"");
    c.addIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_IDENTIFIER,jointCallbackScriptColor_identifier,"");
    c.addIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_WORD,jointCallbackScriptColor_word,"");
    c.addIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_WORD2,jointCallbackScriptColor_word2,"");
    c.addIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_WORD3,jointCallbackScriptColor_word3,"");
    c.addIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_WORD4,jointCallbackScriptColor_word4,"");

    c.addIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_BACKGROUND,contactCallbackScriptColor_background,"");
    c.addIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_SELECTION,contactCallbackScriptColor_selection,"");
    c.addIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_COMMENT,contactCallbackScriptColor_comment,"");
    c.addIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_NUMBER,contactCallbackScriptColor_number,"");
    c.addIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_STRING,contactCallbackScriptColor_string,"");
    c.addIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_CHARACTER,contactCallbackScriptColor_character,"");
    c.addIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_OPERATOR,contactCallbackScriptColor_operator,"");
    c.addIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_PREPROCESSOR,contactCallbackScriptColor_preprocessor,"");
    c.addIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_IDENTIFIER,contactCallbackScriptColor_identifier,"");
    c.addIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_WORD,contactCallbackScriptColor_word,"");
    c.addIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_WORD2,contactCallbackScriptColor_word2,"");
    c.addIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_WORD3,contactCallbackScriptColor_word3,"");
    c.addIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_WORD4,contactCallbackScriptColor_word4,"");

    c.addIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_BACKGROUND,generalCallbackScriptColor_background,"");
    c.addIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_SELECTION,generalCallbackScriptColor_selection,"");
    c.addIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_COMMENT,generalCallbackScriptColor_comment,"");
    c.addIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_NUMBER,generalCallbackScriptColor_number,"");
    c.addIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_STRING,generalCallbackScriptColor_string,"");
    c.addIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_CHARACTER,generalCallbackScriptColor_character,"");
    c.addIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_OPERATOR,generalCallbackScriptColor_operator,"");
    c.addIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_PREPROCESSOR,generalCallbackScriptColor_preprocessor,"");
    c.addIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_IDENTIFIER,generalCallbackScriptColor_identifier,"");
    c.addIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_WORD,generalCallbackScriptColor_word,"");
    c.addIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_WORD2,generalCallbackScriptColor_word2,"");
    c.addIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_WORD3,generalCallbackScriptColor_word3,"");
    c.addIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_WORD4,generalCallbackScriptColor_word4,"");

    c.addIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_BACKGROUND,customizationScriptColor_background,"");
    c.addIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_SELECTION,customizationScriptColor_selection,"");
    c.addIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_COMMENT,customizationScriptColor_comment,"");
    c.addIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_NUMBER,customizationScriptColor_number,"");
    c.addIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_STRING,customizationScriptColor_string,"");
    c.addIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_CHARACTER,customizationScriptColor_character,"");
    c.addIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_OPERATOR,customizationScriptColor_operator,"");
    c.addIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_PREPROCESSOR,customizationScriptColor_preprocessor,"");
    c.addIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_IDENTIFIER,customizationScriptColor_identifier,"");
    c.addIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_WORD,customizationScriptColor_word,"");
    c.addIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_WORD2,customizationScriptColor_word2,"");
    c.addIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_WORD3,customizationScriptColor_word3,"");
    c.addIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_WORD4,customizationScriptColor_word4,"");

    c.addIntVector3(_USR_ADD_ON_SCRIPT_COLOR_BACKGROUND,addOnScriptColor_background,"");
    c.addIntVector3(_USR_ADD_ON_SCRIPT_COLOR_SELECTION,addOnScriptColor_selection,"");
    c.addIntVector3(_USR_ADD_ON_SCRIPT_COLOR_COMMENT,addOnScriptColor_comment,"");
    c.addIntVector3(_USR_ADD_ON_SCRIPT_COLOR_NUMBER,addOnScriptColor_number,"");
    c.addIntVector3(_USR_ADD_ON_SCRIPT_COLOR_STRING,addOnScriptColor_string,"");
    c.addIntVector3(_USR_ADD_ON_SCRIPT_COLOR_CHARACTER,addOnScriptColor_character,"");
    c.addIntVector3(_USR_ADD_ON_SCRIPT_COLOR_OPERATOR,addOnScriptColor_operator,"");
    c.addIntVector3(_USR_ADD_ON_SCRIPT_COLOR_PREPROCESSOR,addOnScriptColor_preprocessor,"");
    c.addIntVector3(_USR_ADD_ON_SCRIPT_COLOR_IDENTIFIER,addOnScriptColor_identifier,"");
    c.addIntVector3(_USR_ADD_ON_SCRIPT_COLOR_WORD,addOnScriptColor_word,"");
    c.addIntVector3(_USR_ADD_ON_SCRIPT_COLOR_WORD2,addOnScriptColor_word2,"");
    c.addIntVector3(_USR_ADD_ON_SCRIPT_COLOR_WORD3,addOnScriptColor_word3,"");
    c.addIntVector3(_USR_ADD_ON_SCRIPT_COLOR_WORD4,addOnScriptColor_word4,"");

    c.addRandomLine("");
    c.addRandomLine("");


    c.addRandomLine("// Directories");
    c.addRandomLine("// =================================================");
    c.addString(_USR_DIRECTORY_FOR_SCENES,defaultDirectoryForScenes,"absolute path, e.g. d:/myScenes (or leave empty for default path)");
    c.addString(_USR_DIRECTORY_FOR_MODELS,defaultDirectoryForModels,"absolute path, e.g. d:/myModels (or leave empty for default path)");
    c.addString(_USR_DIRECTORY_FOR_CAD,defaultDirectoryForCadFiles,"absolute path, e.g. d:/myCadFiles (or leave empty for default path)");
    c.addString(_USR_DIRECTORY_FOR_MISC,defaultDirectoryForMiscFiles,"absolute path, e.g. d:/myMiscFiles (or leave empty for default path)");
    c.addString(_USR_DIRECTORY_FOR_SCRIPT_EDITOR,defaultDirectoryForExternalScriptEditor,"absolute path, e.g. d:/myScriptTempFiles (or leave empty for default path)");
    c.addString(_USR_DIRECTORY_FOR_REMOTE_API,defaultDirectoryForRemoteApiFiles,"absolute path, e.g. d:/myRemoteApiTransfers (or leave empty for default path)");


    c.addRandomLine("");
    c.addRandomLine("");


    c.addRandomLine("// Serialization");
    c.addRandomLine("// =================================================");
    c.addInteger(_USR_AUTO_SAVE_DELAY,autoSaveDelay,"in minutes. 0 to disable.");
    c.addBoolean(_USR_DO_NOT_WRITE_PERSISTENT_DATA,doNotWritePersistentData,"");
    c.addBoolean(_USR_COMPRESS_FILES,compressFiles,"");
    c.addBoolean(_USR_APPROXIMATED_NORMALS,saveApproxNormals,"");
    c.addBoolean(_USR_PACK_INDICES,packIndices,"");
    c.addInteger(_USR_FILE_DIALOGS_NATIVE,fileDialogs,"recommended to keep -1 (-1=default, 0=native dialogs, 1=Qt dialogs).");
    c.addInteger(_USR_BULLET_SERIALIZATION_BUFFER,bulletSerializationBuffer,"");
// this one is not put to / read from usrset.txt    c.addBoolean(_USR_ASK_TO_INCLUDE_SCRIPT_FILES,askToIncludeScriptFiles,"recommended to keep false.");


    c.addRandomLine("");
    c.addRandomLine("");


    c.addRandomLine("// Undo/Redo");
    c.addRandomLine("// =================================================");
    c.addBoolean(_USR_UNDO_REDO_ENABLED,_undoRedoEnabled,"");
    c.addBoolean(_USR_UNDO_REDO_PARTIAL_WITH_CAMERAS,_undoRedoOnlyPartialWithCameras,"");
    c.addInteger(_USR_UNDO_REDO_LEVEL_COUNT,undoRedoLevelCount,"");
    c.addInteger(_USR_UNDO_REDO_MAX_BUFFER_SIZE,undoRedoMaxBufferSize,"");
    c.addInteger(_USR_TIME_FOR_UNDO_REDO_TOO_LONG_WARNING,timeInMsForUndoRedoTooLongWarning,"");
    c.addRandomLine("");
    c.addRandomLine("");


    c.addRandomLine("// Messaging");
    c.addRandomLine("// =================================================");
    c.addBoolean(_USR_DO_NOT_SHOW_CRASH_RECOVERY_MESSAGE,doNotShowCrashRecoveryMessage,"");
    c.addBoolean(_USR_DO_NOT_SHOW_UPDATE_CHECK_MESSAGE,doNotShowUpdateCheckMessage,"");
    c.addBoolean(_USR_DO_NOT_SHOW_SCENE_SELECTION_THUMBNAILS,doNotShowSceneSelectionThumbnails,"");
    c.addBoolean(_USR_DO_NOT_SHOW_PROGRESS_BARS,doNotShowProgressBars,"");
    c.addBoolean(_USR_DO_NOT_SHOW_ACKNOWLEDGMENT_MESSAGES,doNotShowAcknowledgmentMessages,"");
    c.addBoolean(_USR_REDIRECT_STATUSBAR_MSG_TO_CONSOLE_IN_HEADLESS_MODE,redirectStatusbarMsgToConsoleInHeadlessMode,"");


    c.addRandomLine("");
    c.addRandomLine("");


    c.addRandomLine("// Compatibility");
    c.addRandomLine("// =================================================");
    c.addBoolean(_USR_MIDDLE_MOUSE_BUTTON_SWITCHES_MODES,middleMouseButtonSwitchesModes,"has only an effect if navigationBackwardCompatibility is true.");
    c.addBoolean(_USR_NAVIGATION_BACKWARD_COMPATIBILITY_MODE,navigationBackwardCompatibility,"recommended to keep false.");
    c.addFloat(_USR_COLOR_ADJUST_BACK_COMPATIBILITY,colorAdjust_backCompatibility,"recommended to keep 1.0");
    c.addBoolean(_USR_SPECIFIC_GPU_TWEAK,specificGpuTweak,"");
    c.addBoolean(_USR_ENABLE_OLD_PATH_PLANNING_GUI,enableOldPathPlanningGui,"");
    c.addBoolean(_USR_ENABLE_OLD_MOTION_PLANNING_GUI,enableOldMotionPlanningGui,"");
    c.addBoolean(_USR_USE_ALTERNATE_SERIAL_PORT_ROUTINES,useAlternateSerialPortRoutines,"");
    c.addBoolean(_USR_ENABLE_OPENGL_BASED_CUSTOM_UI_EDITOR,enableOpenGlBasedCustomUiEditor,"");


    c.addRandomLine("");
    c.addRandomLine("");




    c.addRandomLine("// Various");
    c.addRandomLine("// =================================================");
    c.addFloat(_USR_MOUSE_WHEEL_ZOOM_FACTOR,mouseWheelZoomFactor,"");
    c.addFloat(_USR_DYNAMIC_ACTIVITY_RANGE,dynamicActivityRange,"");
    c.addFloat(_USR_TRANSLATION_STEP_SIZE,_translationStepSize,"");
    c.addFloat(_USR_ROTATION_STEP_SIZE,_rotationStepSize*radToDeg,"");
    c.addInteger(_USR_PROCESSOR_CORE_AFFINITY,CThreadPool::getProcessorCoreAffinity(),"recommended to keep 0 (-1:os default, 0:all threads on same core, m: affinity mask (bit1=core1, bit2=core2, etc.))");
    c.addInteger(_USR_FREE_SERVER_PORT_START,freeServerPortStart,"");
    c.addInteger(_USR_FREE_SERVER_PORT_RANGE,freeServerPortRange,"");
    c.addBoolean(_USR_USE_EXTERNAL_SCRIPT_EDITOR,_useExternalScriptEditorToSave,"");
    c.addString(_USR_EXTERNAL_SCRIPT_EDITOR,externalScriptEditorToSave,"");
    c.addInteger(_USR_ABORT_SCRIPT_EXECUTION_BUTTON,abortScriptExecutionButton,"in seconds. Zero to disable.");
    c.addInteger(_USR_TRIANGLE_COUNT_IN_OBB,triCountInOBB,"");
    c.addBoolean(_USR_REMOVE_IDENTICAL_VERTICES,identicalVerticesCheck,"");
    c.addFloat(_USR_IDENTICAL_VERTICES_TOLERANCE,identicalVerticesTolerance,"");
    c.addBoolean(_USR_REMOVE_IDENTICAL_TRIANGLES,identicalTrianglesCheck,"");
    c.addBoolean(_USR_TRIANGLE_WINDING_CHECK,identicalTrianglesWindingCheck,"");
    c.addBoolean(_USR_RUN_CUSTOMIZATION_SCRIPTS,runCustomizationScripts,"");
    c.addBoolean(_USR_TEST1,test1,"recommended to keep false.");
    c.addBoolean(_USR_ORDER_HIERARCHY_ALPHABETICALLY,orderHierarchyAlphabetically,"");
    c.addInteger(_USR_MAC_CHILD_DIALOG_TYPE,macChildDialogType,"-1=default.");
    c.addBoolean(_USR_USE_EXTERNAL_LUA_LIBRARY,useExternalLuaLibrary,"if true, will call all Lua functions via the v_repLua library ('v_repLua.dll', 'libv_repLua.so' or 'libv_repLua.dylib')");
    c.addBoolean(_USR_RAISE_ERROR_WITH_API_SCRIPT_FUNCTIONS,raiseErrorWithApiScriptFunctions,"");
    c.addString(_USR_ADDITIONAL_LUA_PATH,additionalLuaPath,"e.g. d:/myLuaRoutines");
    c.addInteger(_USR_DESKTOP_RECORDING_INDEX,desktopRecordingIndex,"");
    c.addInteger(_USR_DESKTOP_RECORDING_WIDTH,desktopRecordingWidth,"-1=default.");


    std::string filenameAndPath(VVarious::getModulePath()+VREP_SLASH+V_REP_SYSTEM_DIRECTORY_NAME+VREP_SLASH+USER_SETTINGS_FILENAME);
    c.writeConfiguration(filenameAndPath.c_str());
}

void CUserSettings::loadUserSettings()
{
    CConfReaderAndWriter c;

    // The CDirectoryPaths object might not yet be set-up
    std::string filenameAndPath(VVarious::getModulePath()+VREP_SLASH+V_REP_SYSTEM_DIRECTORY_NAME+VREP_SLASH+USER_SETTINGS_FILENAME);
    // Following call might fail.
    if (!c.readConfiguration(filenameAndPath.c_str()))
        saveUserSettings();

    // Debugging section:
    // *****************************
    c.getBoolean(_USR_ALWAYS_SHOW_CONSOLE,alwaysShowConsole);
    bool dummyBool=false;
#ifndef SIM_WITHOUT_QT_AT_ALL
    if (c.getBoolean(_USR_DEBUG_GUI_SIM_THREAD_SYNC_ACTIVITY,dummyBool))
        CSimAndUiThreadSync::setShowActivityInConsole(dummyBool);
    if (c.getBoolean(_USR_DEBUG_GUI_SIM_THREAD_SYNC_FAILS_ACTIVITY,dummyBool))
        CSimAndUiThreadSync::setShowLockFailsActivityInConsole(dummyBool);
#endif
    int dummyInt=0;
    if (c.getBoolean(_USR_DEBUG_INTERNAL_FUNCTION_ACCESS,dummyBool))
    {
        if (dummyBool)
            dummyInt+=1;
    }
    if (c.getBoolean(_USR_DEBUG_C_API_ACCESS,dummyBool))
    {
        if (dummyBool)
            dummyInt+=2;
    }
    if (c.getBoolean(_USR_DEBUG_LUA_API_ACCESS,dummyBool))
    {
        if (dummyBool)
            dummyInt+=4;
    }
    CFuncDebug::setDebugMask(dummyInt);
    if (c.getBoolean(_USR_DEBUG_THREAD_SWITCHES,dummyBool))
        CThreadPool::setShowThreadSwitches(dummyBool);
    if (c.getBoolean(_USR_DEBUG_EASYLOCK_ACTIVITY,dummyBool))
        CEasyLock::setShowActivity(dummyBool);
    if (c.getBoolean(_USR_DEBUG_TO_FILE,dummyBool))
        CDebugLogFile::setDebugToFile(dummyBool);


    // Rendering section:
    // *****************************
    c.getInteger(_USR_IDLE_FPS,_idleFps);
    setIdleFps(_idleFps);
    c.getInteger(_USR_THREADED_RENDERING_DURING_SIMULATION,threadedRenderingDuringSimulation);
    c.getInteger(_USR_DESIRED_OPENGL_MAJOR,desiredOpenGlMajor);
    c.getInteger(_USR_DESIRED_OPENGL_MINOR,desiredOpenGlMinor);
    c.getInteger(_USR_OFFSCREEN_CONTEXT_TYPE,offscreenContextType);
    c.getInteger(_USR_FBO_TYPE,fboType);
    c.getBoolean(_USR_FORCE_FBO_VIA_EXT,forceFboViaExt);
    c.getInteger(_USR_VBO_OPERATION,vboOperation);
    c.getInteger(_USR_VBO_PERSISTENCE_IN_MS,vboPersistenceInMs);
    c.getBoolean(_USR_OGL_COMPATIBILITY_TWEAK_1,oglCompatibilityTweak1);
    c.getInteger(_USR_VISION_SENSORS_USE_GUI_WINDOWED,visionSensorsUseGuiThread_windowed);
    c.getInteger(_USR_VISION_SENSORS_USE_GUI_HEADLESS,visionSensorsUseGuiThread_headless);
    c.getBoolean(_USR_USE_GLFINISH,useGlFinish);
    c.getBoolean(_USR_USE_GLFINISH_VISION_SENSORS,useGlFinish_visionSensors);
    c.getInteger(_USR_VSYNC,vsync);
    c.getBoolean(_USR_DEBUG_OPENGL,debugOpenGl);
    c.getFloat(_USR_STEREO_DIST,stereoDist);
    c.getInteger(_USR_HIGH_RES_DISPLAY,highResDisplay);
    c.getBoolean(_USR_NO_EDGES_WHEN_MOUSE_DOWN,noEdgesWhenMouseDownInCameraView);
    c.getBoolean(_USR_NO_TEXTURES_WHEN_MOUSE_DOWN,noTexturesWhenMouseDownInCameraView);
    c.getBoolean(_USR_NO_CUSTOM_UIS_WHEN_MOUSE_DOWN,noCustomUisWhenMouseDownInCameraView);
    c.getInteger(_USR_HIERARCHY_REFRESH_CNT,hierarchyRefreshCnt);

    // Visual section:
    // *****************************
    c.getInteger(_USR_RENDERING_SURFACE_VERTICAL_SHIFT,renderingSurfaceVShift);
    c.getInteger(_USR_RENDERING_SURFACE_VERTICAL_RESIZE,renderingSurfaceVResize);
    c.getBoolean(_USR_DISPLAY_WORLD_REF,displayWorldReference);
    c.getBoolean(_USR_ANTIALIASING,antiAliasing);
    c.getBoolean(_USR_DISPLAY_BOUNDING_BOXES,displayBoundingBoxeWhenObjectSelected);
    c.getInteger(_USR_GUI_FONT_SIZE_WIN,guiFontSize_Win);
    c.getInteger(_USR_GUI_FONT_SIZE_MAC,guiFontSize_Mac);
    c.getInteger(_USR_GUI_FONT_SIZE_LINUX,guiFontSize_Linux);
    c.getBoolean(_USR_ALLOW_TRANSPARENT_DIALOGS,allowTransparentDialogs);
    c.getFloat(_USR_DIALOG_TRANSPARENCY_FACTOR,dialogTransparencyFactor);
    c.getBoolean(_USR_STATUSBAR_INITIALLY_VISIBLE,statusbarInitiallyVisible);
    c.getBoolean(_USR_MODELBROWSER_INITIALLY_VISIBLE,modelBrowserInitiallyVisible);
#ifdef SIM_WITH_GUI
    //OLDMODELBROWSER COglSurface::_browserEnabled=modelBrowserInitiallyVisible;
    App::setBrowserEnabled(modelBrowserInitiallyVisible);
#endif
    c.getBoolean(_USR_SCENEHIERARCHY_INITIALLY_VISIBLE,sceneHierarchyInitiallyVisible);
#ifdef SIM_WITH_GUI
    COglSurface::_hierarchyEnabled=sceneHierarchyInitiallyVisible;
#endif

    c.getString(_USR_SCRIPT_EDITOR_FONT,scriptEditorFont);
    c.getInteger(_USR_SCRIPT_EDITOR_FONT_SIZE,scriptEditorFontSize);
    c.getString(_USR_AUX_CONSOLE_FONT,auxConsoleFont);
    c.getInteger(_USR_AUX_CONSOLE_FONT_SIZE,auxConsoleFontSize);

    c.getIntVector3(_USR_MAIN_SCRIPT_COLOR_BACKGROUND,mainScriptColor_background);
    c.getIntVector3(_USR_MAIN_SCRIPT_COLOR_SELECTION,mainScriptColor_selection);
    c.getIntVector3(_USR_MAIN_SCRIPT_COLOR_COMMENT,mainScriptColor_comment);
    c.getIntVector3(_USR_MAIN_SCRIPT_COLOR_NUMBER,mainScriptColor_number);
    c.getIntVector3(_USR_MAIN_SCRIPT_COLOR_STRING,mainScriptColor_string);
    c.getIntVector3(_USR_MAIN_SCRIPT_COLOR_CHARACTER,mainScriptColor_character);
    c.getIntVector3(_USR_MAIN_SCRIPT_COLOR_OPERATOR,mainScriptColor_operator);
    c.getIntVector3(_USR_MAIN_SCRIPT_COLOR_PREPROCESSOR,mainScriptColor_preprocessor);
    c.getIntVector3(_USR_MAIN_SCRIPT_COLOR_IDENTIFIER,mainScriptColor_identifier);
    c.getIntVector3(_USR_MAIN_SCRIPT_COLOR_WORD,mainScriptColor_word);
    c.getIntVector3(_USR_MAIN_SCRIPT_COLOR_WORD2,mainScriptColor_word2);
    c.getIntVector3(_USR_MAIN_SCRIPT_COLOR_WORD3,mainScriptColor_word3);
    c.getIntVector3(_USR_MAIN_SCRIPT_COLOR_WORD4,mainScriptColor_word4);

    c.getIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_BACKGROUND,nonThreadedChildScriptColor_background);
    c.getIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_SELECTION,nonThreadedChildScriptColor_selection);
    c.getIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_COMMENT,nonThreadedChildScriptColor_comment);
    c.getIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_NUMBER,nonThreadedChildScriptColor_number);
    c.getIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_STRING,nonThreadedChildScriptColor_string);
    c.getIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_CHARACTER,nonThreadedChildScriptColor_character);
    c.getIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_OPERATOR,nonThreadedChildScriptColor_operator);
    c.getIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_PREPROCESSOR,nonThreadedChildScriptColor_preprocessor);
    c.getIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_IDENTIFIER,nonThreadedChildScriptColor_identifier);
    c.getIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_WORD,nonThreadedChildScriptColor_word);
    c.getIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_WORD2,nonThreadedChildScriptColor_word2);
    c.getIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_WORD3,nonThreadedChildScriptColor_word3);
    c.getIntVector3(_USR_NON_THREADED_CHILD_SCRIPT_COLOR_WORD4,nonThreadedChildScriptColor_word4);

    c.getIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_BACKGROUND,threadedChildScriptColor_background);
    c.getIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_SELECTION,threadedChildScriptColor_selection);
    c.getIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_COMMENT,threadedChildScriptColor_comment);
    c.getIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_NUMBER,threadedChildScriptColor_number);
    c.getIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_STRING,threadedChildScriptColor_string);
    c.getIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_CHARACTER,threadedChildScriptColor_character);
    c.getIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_OPERATOR,threadedChildScriptColor_operator);
    c.getIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_PREPROCESSOR,threadedChildScriptColor_preprocessor);
    c.getIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_IDENTIFIER,threadedChildScriptColor_identifier);
    c.getIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_WORD,threadedChildScriptColor_word);
    c.getIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_WORD2,threadedChildScriptColor_word2);
    c.getIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_WORD3,threadedChildScriptColor_word3);
    c.getIntVector3(_USR_THREADED_CHILD_SCRIPT_COLOR_WORD4,threadedChildScriptColor_word4);

    c.getIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_BACKGROUND,jointCallbackScriptColor_background);
    c.getIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_SELECTION,jointCallbackScriptColor_selection);
    c.getIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_COMMENT,jointCallbackScriptColor_comment);
    c.getIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_NUMBER,jointCallbackScriptColor_number);
    c.getIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_STRING,jointCallbackScriptColor_string);
    c.getIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_CHARACTER,jointCallbackScriptColor_character);
    c.getIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_OPERATOR,jointCallbackScriptColor_operator);
    c.getIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_PREPROCESSOR,jointCallbackScriptColor_preprocessor);
    c.getIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_IDENTIFIER,jointCallbackScriptColor_identifier);
    c.getIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_WORD,jointCallbackScriptColor_word);
    c.getIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_WORD2,jointCallbackScriptColor_word2);
    c.getIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_WORD3,jointCallbackScriptColor_word3);
    c.getIntVector3(_USR_JOINT_CALLBACK_SCRIPT_COLOR_WORD4,jointCallbackScriptColor_word4);

    c.getIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_BACKGROUND,contactCallbackScriptColor_background);
    c.getIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_SELECTION,contactCallbackScriptColor_selection);
    c.getIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_COMMENT,contactCallbackScriptColor_comment);
    c.getIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_NUMBER,contactCallbackScriptColor_number);
    c.getIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_STRING,contactCallbackScriptColor_string);
    c.getIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_CHARACTER,contactCallbackScriptColor_character);
    c.getIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_OPERATOR,contactCallbackScriptColor_operator);
    c.getIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_PREPROCESSOR,contactCallbackScriptColor_preprocessor);
    c.getIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_IDENTIFIER,contactCallbackScriptColor_identifier);
    c.getIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_WORD,contactCallbackScriptColor_word);
    c.getIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_WORD2,contactCallbackScriptColor_word2);
    c.getIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_WORD3,contactCallbackScriptColor_word3);
    c.getIntVector3(_USR_CONTACT_CALLBACK_SCRIPT_COLOR_WORD4,contactCallbackScriptColor_word4);

    c.getIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_BACKGROUND,generalCallbackScriptColor_background);
    c.getIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_SELECTION,generalCallbackScriptColor_selection);
    c.getIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_COMMENT,generalCallbackScriptColor_comment);
    c.getIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_NUMBER,generalCallbackScriptColor_number);
    c.getIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_STRING,generalCallbackScriptColor_string);
    c.getIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_CHARACTER,generalCallbackScriptColor_character);
    c.getIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_OPERATOR,generalCallbackScriptColor_operator);
    c.getIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_PREPROCESSOR,generalCallbackScriptColor_preprocessor);
    c.getIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_IDENTIFIER,generalCallbackScriptColor_identifier);
    c.getIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_WORD,generalCallbackScriptColor_word);
    c.getIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_WORD2,generalCallbackScriptColor_word2);
    c.getIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_WORD3,generalCallbackScriptColor_word3);
    c.getIntVector3(_USR_GENERAL_CALLBACK_SCRIPT_COLOR_WORD4,generalCallbackScriptColor_word4);

    c.getIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_BACKGROUND,customizationScriptColor_background);
    c.getIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_SELECTION,customizationScriptColor_selection);
    c.getIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_COMMENT,customizationScriptColor_comment);
    c.getIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_NUMBER,customizationScriptColor_number);
    c.getIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_STRING,customizationScriptColor_string);
    c.getIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_CHARACTER,customizationScriptColor_character);
    c.getIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_OPERATOR,customizationScriptColor_operator);
    c.getIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_PREPROCESSOR,customizationScriptColor_preprocessor);
    c.getIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_IDENTIFIER,customizationScriptColor_identifier);
    c.getIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_WORD,customizationScriptColor_word);
    c.getIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_WORD2,customizationScriptColor_word2);
    c.getIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_WORD3,customizationScriptColor_word3);
    c.getIntVector3(_USR_CUSTOMIZATION_SCRIPT_COLOR_WORD4,customizationScriptColor_word4);

    c.getIntVector3(_USR_ADD_ON_SCRIPT_COLOR_BACKGROUND,addOnScriptColor_background);
    c.getIntVector3(_USR_ADD_ON_SCRIPT_COLOR_SELECTION,addOnScriptColor_selection);
    c.getIntVector3(_USR_ADD_ON_SCRIPT_COLOR_COMMENT,addOnScriptColor_comment);
    c.getIntVector3(_USR_ADD_ON_SCRIPT_COLOR_NUMBER,addOnScriptColor_number);
    c.getIntVector3(_USR_ADD_ON_SCRIPT_COLOR_STRING,addOnScriptColor_string);
    c.getIntVector3(_USR_ADD_ON_SCRIPT_COLOR_CHARACTER,addOnScriptColor_character);
    c.getIntVector3(_USR_ADD_ON_SCRIPT_COLOR_OPERATOR,addOnScriptColor_operator);
    c.getIntVector3(_USR_ADD_ON_SCRIPT_COLOR_PREPROCESSOR,addOnScriptColor_preprocessor);
    c.getIntVector3(_USR_ADD_ON_SCRIPT_COLOR_IDENTIFIER,addOnScriptColor_identifier);
    c.getIntVector3(_USR_ADD_ON_SCRIPT_COLOR_WORD,addOnScriptColor_word);
    c.getIntVector3(_USR_ADD_ON_SCRIPT_COLOR_WORD2,addOnScriptColor_word2);
    c.getIntVector3(_USR_ADD_ON_SCRIPT_COLOR_WORD3,addOnScriptColor_word3);
    c.getIntVector3(_USR_ADD_ON_SCRIPT_COLOR_WORD4,addOnScriptColor_word4);

    // Directories section:
    // *****************************
    c.getString(_USR_DIRECTORY_FOR_SCENES,defaultDirectoryForScenes);
    c.getString(_USR_DIRECTORY_FOR_MODELS,defaultDirectoryForModels);
    c.getString(_USR_DIRECTORY_FOR_CAD,defaultDirectoryForCadFiles);
    c.getString(_USR_DIRECTORY_FOR_MISC,defaultDirectoryForMiscFiles);
    c.getString(_USR_DIRECTORY_FOR_SCRIPT_EDITOR,defaultDirectoryForExternalScriptEditor);
    c.getString(_USR_DIRECTORY_FOR_REMOTE_API,defaultDirectoryForRemoteApiFiles);

    // Serialization section:
    // *****************************
    c.getInteger(_USR_AUTO_SAVE_DELAY,autoSaveDelay);
    c.getBoolean(_USR_DO_NOT_WRITE_PERSISTENT_DATA,doNotWritePersistentData);
    c.getBoolean(_USR_COMPRESS_FILES,compressFiles);
    c.getBoolean(_USR_APPROXIMATED_NORMALS,saveApproxNormals);
    c.getBoolean(_USR_PACK_INDICES,packIndices);
    c.getInteger(_USR_FILE_DIALOGS_NATIVE,fileDialogs);
    c.getInteger(_USR_BULLET_SERIALIZATION_BUFFER,bulletSerializationBuffer);
// this one is not put to / read from usrset.txt    c.getBoolean(_USR_ASK_TO_INCLUDE_SCRIPT_FILES,askToIncludeScriptFiles);

    // Undo/redo section:
    // *****************************
    c.getBoolean(_USR_UNDO_REDO_ENABLED,_undoRedoEnabled);
    c.getBoolean(_USR_UNDO_REDO_PARTIAL_WITH_CAMERAS,_undoRedoOnlyPartialWithCameras);
    c.getInteger(_USR_UNDO_REDO_LEVEL_COUNT,undoRedoLevelCount);
    undoRedoLevelCount=tt::getLimitedInt(3,50,undoRedoLevelCount);
    c.getInteger(_USR_UNDO_REDO_MAX_BUFFER_SIZE,undoRedoMaxBufferSize);
    undoRedoMaxBufferSize=tt::getLimitedInt(2000000,800000000,undoRedoMaxBufferSize);
    c.getInteger(_USR_TIME_FOR_UNDO_REDO_TOO_LONG_WARNING,timeInMsForUndoRedoTooLongWarning);


    // Messaging section:
    // *****************************
    c.getBoolean(_USR_DO_NOT_SHOW_CRASH_RECOVERY_MESSAGE,doNotShowCrashRecoveryMessage);
    c.getBoolean(_USR_DO_NOT_SHOW_UPDATE_CHECK_MESSAGE,doNotShowUpdateCheckMessage);
    c.getBoolean(_USR_DO_NOT_SHOW_SCENE_SELECTION_THUMBNAILS,doNotShowSceneSelectionThumbnails);
    c.getBoolean(_USR_DO_NOT_SHOW_PROGRESS_BARS,doNotShowProgressBars);
    c.getBoolean(_USR_DO_NOT_SHOW_ACKNOWLEDGMENT_MESSAGES,doNotShowAcknowledgmentMessages);
    c.getBoolean(_USR_REDIRECT_STATUSBAR_MSG_TO_CONSOLE_IN_HEADLESS_MODE,redirectStatusbarMsgToConsoleInHeadlessMode);


    // Compatibility section:
    // *****************************
    c.getBoolean(_USR_MIDDLE_MOUSE_BUTTON_SWITCHES_MODES,middleMouseButtonSwitchesModes);
    c.getBoolean(_USR_NAVIGATION_BACKWARD_COMPATIBILITY_MODE,navigationBackwardCompatibility);
    c.getFloat(_USR_COLOR_ADJUST_BACK_COMPATIBILITY,colorAdjust_backCompatibility);
    c.getBoolean(_USR_SPECIFIC_GPU_TWEAK,specificGpuTweak);
    c.getBoolean(_USR_ENABLE_OLD_PATH_PLANNING_GUI,enableOldPathPlanningGui);
    c.getBoolean(_USR_ENABLE_OLD_MOTION_PLANNING_GUI,enableOldMotionPlanningGui);
    c.getBoolean(_USR_USE_ALTERNATE_SERIAL_PORT_ROUTINES,useAlternateSerialPortRoutines);
    c.getBoolean(_USR_ENABLE_OPENGL_BASED_CUSTOM_UI_EDITOR,enableOpenGlBasedCustomUiEditor);




    // Various section:
    // *****************************
    c.getFloat(_USR_MOUSE_WHEEL_ZOOM_FACTOR,mouseWheelZoomFactor);
    c.getFloat(_USR_DYNAMIC_ACTIVITY_RANGE,dynamicActivityRange);
    if (c.getFloat(_USR_TRANSLATION_STEP_SIZE,_translationStepSize))
        setTranslationStepSize(_translationStepSize);
    if (c.getFloat(_USR_ROTATION_STEP_SIZE,_rotationStepSize))
        setRotationStepSize(_rotationStepSize*degToRad);
    int processorCoreAffinity=0;
    if (c.getInteger(_USR_PROCESSOR_CORE_AFFINITY,processorCoreAffinity))
        CThreadPool::setProcessorCoreAffinity(processorCoreAffinity);
    c.getInteger(_USR_FREE_SERVER_PORT_START,freeServerPortStart);
    _nextfreeServerPortToUse=freeServerPortStart;
    c.getInteger(_USR_FREE_SERVER_PORT_RANGE,freeServerPortRange);
    c.getBoolean(_USR_USE_EXTERNAL_SCRIPT_EDITOR,_useExternalScriptEditor);
    _useExternalScriptEditorToSave=_useExternalScriptEditor;
    c.getString(_USR_EXTERNAL_SCRIPT_EDITOR,externalScriptEditor);
    externalScriptEditorToSave=externalScriptEditor;
    c.getInteger(_USR_ABORT_SCRIPT_EXECUTION_BUTTON,abortScriptExecutionButton);
    c.getInteger(_USR_TRIANGLE_COUNT_IN_OBB,triCountInOBB);
    c.getBoolean(_USR_REMOVE_IDENTICAL_VERTICES,identicalVerticesCheck);
    c.getFloat(_USR_IDENTICAL_VERTICES_TOLERANCE,identicalVerticesTolerance);
    c.getBoolean(_USR_REMOVE_IDENTICAL_TRIANGLES,identicalTrianglesCheck);
    c.getBoolean(_USR_TRIANGLE_WINDING_CHECK,identicalTrianglesWindingCheck);
    c.getBoolean(_USR_RUN_CUSTOMIZATION_SCRIPTS,runCustomizationScripts);
    c.getBoolean(_USR_TEST1,test1);
    c.getBoolean(_USR_ORDER_HIERARCHY_ALPHABETICALLY,orderHierarchyAlphabetically);
    c.getInteger(_USR_MAC_CHILD_DIALOG_TYPE,macChildDialogType);
#ifdef SIM_WITH_GUI
#ifdef MAC_VREP
    if (macChildDialogType<=0)
    { // Qt::Tool
        VDialog::dialogStyle=QT_MODELESS_DLG_STYLE;
        CScintillaDlg::dialogStyle=QT_MODELESS_SCINTILLA_DLG_STYLE;
        CScintillaConsoleDlg::dialogStyle=QT_MODELESS_SCINTILLA_DLG_STYLE;
    }
    else
    { // Qt::Dialog
        VDialog::dialogStyle=Qt::Dialog;
        CScintillaDlg::dialogStyle=Qt::Dialog;
        CScintillaConsoleDlg::dialogStyle=Qt::Dialog;
    }
#else
    VDialog::dialogStyle=QT_MODELESS_DLG_STYLE;
    CScintillaDlg::dialogStyle=QT_MODELESS_SCINTILLA_DLG_STYLE;
    CScintillaConsoleDlg::dialogStyle=QT_MODELESS_SCINTILLA_DLG_STYLE;
#endif
#endif
    c.getBoolean(_USR_USE_EXTERNAL_LUA_LIBRARY,useExternalLuaLibrary);
    c.getBoolean(_USR_RAISE_ERROR_WITH_API_SCRIPT_FUNCTIONS,raiseErrorWithApiScriptFunctions);
    c.getString(_USR_ADDITIONAL_LUA_PATH,additionalLuaPath);
    c.getInteger(_USR_DESKTOP_RECORDING_INDEX,desktopRecordingIndex);
    c.getInteger(_USR_DESKTOP_RECORDING_WIDTH,desktopRecordingWidth);
    c.getBoolean(_USR_FORCE_BUG_FIX_REL_30002,forceBugFix_rel30002);
}

