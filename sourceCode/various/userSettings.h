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

#include "mainCont.h"
#include "visualParam.h"

class CUserSettings
{
public:
    CUserSettings();
    virtual ~CUserSettings();

    void setUndoRedoEnabled(bool isEnabled);
    bool getUndoRedoEnabled();
    bool getUndoRedoOnlyPartialWithCameras();

    void saveUserSettings();
    void loadUserSettings();

    void setTranslationStepSize(float s);
    float getTranslationStepSize();
    void setRotationStepSize(float s);
    float getRotationStepSize();
    bool useBuiltInScriptEditor();
    bool getUseBuiltInScriptEditorToSave();
    void setUseBuiltInScriptEditorToSave(bool u);
    int getIdleFps();
    void setIdleFps(int fps);

    int getNextFreeServerPortToUse();
    void setNextFreeServerPortToUse(int p);

    CVisualParam groupSelectionColor;
    bool displayWorldReference;
    bool useGlFinish;
    bool useGlFinish_visionSensors;
    bool oglCompatibilityTweak1;
    float stereoDist;
    int vsync;
    bool debugOpenGl;
    bool identicalVerticesCheck;
    float identicalVerticesTolerance;
    bool identicalTrianglesCheck;
    bool identicalTrianglesWindingCheck;
    bool compressFiles;
    int triCountInOBB;
    bool saveApproxNormals;
    bool packIndices;
    bool runCustomizationScripts;
    bool test1;
    bool orderHierarchyAlphabetically;
    int macChildDialogType;
    bool useExternalLuaLibrary;
    bool raiseErrorWithApiScriptFunctions;

    int undoRedoLevelCount;
    int undoRedoMaxBufferSize;
    bool alwaysShowConsole;
    bool forceBugFix_rel30002;
    bool allowTransparentDialogs;
    bool statusbarInitiallyVisible;
    bool modelBrowserInitiallyVisible;
    bool sceneHierarchyInitiallyVisible;
    float dialogTransparencyFactor;
    int autoSaveDelay;
    int bulletSerializationBuffer;
    bool askToIncludeScriptFiles;
    int abortScriptExecutionButton;
    int timeInMsForUndoRedoTooLongWarning;

    bool middleMouseButtonSwitchesModes;
    bool navigationBackwardCompatibility;
    float colorAdjust_backCompatibility;
    bool specificGpuTweak;
    bool enableOldPathPlanningGui;
    bool enableOldMotionPlanningGui;
    bool useAlternateSerialPortRoutines;
    bool enableOpenGlBasedCustomUiEditor;


    bool displayBoundingBoxeWhenObjectSelected;
    bool antiAliasing;
    float dynamicActivityRange;
    int freeServerPortStart;
    int freeServerPortRange;
    int renderingSurfaceVShift;
    int renderingSurfaceVResize;
    int threadedRenderingDuringSimulation;
    int offscreenContextType;
    int fboType;
    bool forceFboViaExt;
    int vboOperation;
    int vboPersistenceInMs;
    int desiredOpenGlMajor;
    int desiredOpenGlMinor;
    int visionSensorsUseGuiThread_windowed;
    int visionSensorsUseGuiThread_headless;
    int fileDialogs;
    float mouseWheelZoomFactor;

    bool doNotWritePersistentData;
    bool doNotShowCrashRecoveryMessage;
    bool doNotShowUpdateCheckMessage;
    bool doNotShowSceneSelectionThumbnails;
    bool doNotShowProgressBars;
    bool doNotShowAcknowledgmentMessages;
    bool redirectStatusbarMsgToConsoleInHeadlessMode;

    std::string scriptEditorFont;
    int scriptEditorFontSize;
    std::string auxConsoleFont;
    int auxConsoleFontSize;

    std::string externalScriptEditor;
    bool _useExternalScriptEditor;
    std::string additionalLuaPath;

    std::string externalScriptEditorToSave;
    bool _useExternalScriptEditorToSave;

    int guiFontSize_Win;
    int guiFontSize_Mac;
    int guiFontSize_Linux;

    std::string defaultDirectoryForScenes;
    std::string defaultDirectoryForModels;
    std::string defaultDirectoryForCadFiles;
    std::string defaultDirectoryForMiscFiles;
    std::string defaultDirectoryForExternalScriptEditor;
    std::string defaultDirectoryForRemoteApiFiles;

    int threadedChildScriptColor_background[3];
    int threadedChildScriptColor_selection[3];
    int threadedChildScriptColor_comment[3];
    int threadedChildScriptColor_number[3];
    int threadedChildScriptColor_string[3];
    int threadedChildScriptColor_character[3];
    int threadedChildScriptColor_operator[3];
    int threadedChildScriptColor_preprocessor[3];
    int threadedChildScriptColor_identifier[3];
    int threadedChildScriptColor_word[3];
    int threadedChildScriptColor_word2[3];
    int threadedChildScriptColor_word3[3];
    int threadedChildScriptColor_word4[3];

    int nonThreadedChildScriptColor_background[3];
    int nonThreadedChildScriptColor_selection[3];
    int nonThreadedChildScriptColor_comment[3];
    int nonThreadedChildScriptColor_number[3];
    int nonThreadedChildScriptColor_string[3];
    int nonThreadedChildScriptColor_character[3];
    int nonThreadedChildScriptColor_operator[3];
    int nonThreadedChildScriptColor_preprocessor[3];
    int nonThreadedChildScriptColor_identifier[3];
    int nonThreadedChildScriptColor_word[3];
    int nonThreadedChildScriptColor_word2[3];
    int nonThreadedChildScriptColor_word3[3];
    int nonThreadedChildScriptColor_word4[3];

    int mainScriptColor_background[3];
    int mainScriptColor_selection[3];
    int mainScriptColor_comment[3];
    int mainScriptColor_number[3];
    int mainScriptColor_string[3];
    int mainScriptColor_character[3];
    int mainScriptColor_operator[3];
    int mainScriptColor_preprocessor[3];
    int mainScriptColor_identifier[3];
    int mainScriptColor_word[3];
    int mainScriptColor_word2[3];
    int mainScriptColor_word3[3];
    int mainScriptColor_word4[3];

    int jointCallbackScriptColor_background[3];
    int jointCallbackScriptColor_selection[3];
    int jointCallbackScriptColor_comment[3];
    int jointCallbackScriptColor_number[3];
    int jointCallbackScriptColor_string[3];
    int jointCallbackScriptColor_character[3];
    int jointCallbackScriptColor_operator[3];
    int jointCallbackScriptColor_preprocessor[3];
    int jointCallbackScriptColor_identifier[3];
    int jointCallbackScriptColor_word[3];
    int jointCallbackScriptColor_word2[3];
    int jointCallbackScriptColor_word3[3];
    int jointCallbackScriptColor_word4[3];

    int contactCallbackScriptColor_background[3];
    int contactCallbackScriptColor_selection[3];
    int contactCallbackScriptColor_comment[3];
    int contactCallbackScriptColor_number[3];
    int contactCallbackScriptColor_string[3];
    int contactCallbackScriptColor_character[3];
    int contactCallbackScriptColor_operator[3];
    int contactCallbackScriptColor_preprocessor[3];
    int contactCallbackScriptColor_identifier[3];
    int contactCallbackScriptColor_word[3];
    int contactCallbackScriptColor_word2[3];
    int contactCallbackScriptColor_word3[3];
    int contactCallbackScriptColor_word4[3];

    int generalCallbackScriptColor_background[3];
    int generalCallbackScriptColor_selection[3];
    int generalCallbackScriptColor_comment[3];
    int generalCallbackScriptColor_number[3];
    int generalCallbackScriptColor_string[3];
    int generalCallbackScriptColor_character[3];
    int generalCallbackScriptColor_operator[3];
    int generalCallbackScriptColor_preprocessor[3];
    int generalCallbackScriptColor_identifier[3];
    int generalCallbackScriptColor_word[3];
    int generalCallbackScriptColor_word2[3];
    int generalCallbackScriptColor_word3[3];
    int generalCallbackScriptColor_word4[3];

    int customizationScriptColor_background[3];
    int customizationScriptColor_selection[3];
    int customizationScriptColor_comment[3];
    int customizationScriptColor_number[3];
    int customizationScriptColor_string[3];
    int customizationScriptColor_character[3];
    int customizationScriptColor_operator[3];
    int customizationScriptColor_preprocessor[3];
    int customizationScriptColor_identifier[3];
    int customizationScriptColor_word[3];
    int customizationScriptColor_word2[3];
    int customizationScriptColor_word3[3];
    int customizationScriptColor_word4[3];

    int addOnScriptColor_background[3];
    int addOnScriptColor_selection[3];
    int addOnScriptColor_comment[3];
    int addOnScriptColor_number[3];
    int addOnScriptColor_string[3];
    int addOnScriptColor_character[3];
    int addOnScriptColor_operator[3];
    int addOnScriptColor_preprocessor[3];
    int addOnScriptColor_identifier[3];
    int addOnScriptColor_word[3];
    int addOnScriptColor_word2[3];
    int addOnScriptColor_word3[3];
    int addOnScriptColor_word4[3];

    int highResDisplay;
    bool noEdgesWhenMouseDownInCameraView;
    bool noTexturesWhenMouseDownInCameraView;
    bool noCustomUisWhenMouseDownInCameraView;
    int hierarchyRefreshCnt;
    int desktopRecordingIndex;
    int desktopRecordingWidth;

private:
    void _setIntVector3(int v[3],int a,int b,int c);
    bool _undoRedoEnabled;
    bool _undoRedoOnlyPartialWithCameras;
    float _translationStepSize;
    float _rotationStepSize;
    int _idleFps;
    int _nextfreeServerPortToUse;
};
