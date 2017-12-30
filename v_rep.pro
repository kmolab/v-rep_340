# This file is part of V-REP, the Virtual Robot Experimentation Platform.
# 
# Copyright 2006-2017 Coppelia Robotics GmbH. All rights reserved. 
# marc@coppeliarobotics.com
# www.coppeliarobotics.com
# 
# V-REP is dual-licensed, under the terms of EITHER (at your option):
#   1. V-REP commercial license (contact us for details)
#   2. GNU GPL (see below)
# 
# GNU GPL license:
# -------------------------------------------------------------------
# V-REP is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# V-REP IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
# AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
# DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
# MISUSING THIS SOFTWARE.
# 
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with V-REP.  If not, see <http://www.gnu.org/licenses/>.
# -------------------------------------------------------------------
#
# This file was automatically created for V-REP release V3.4.0 rev. 1 on April 5th 2017

TARGET = v_rep
TEMPLATE = lib
CONFIG += shared

CONFIG += WITH_GUI
CONFIG += WITH_OPENGL # comment only if above line is commented
CONFIG += WITH_QT # comment only if above 2 lines are commented. Without Qt uses some sub-optimal routines for now, check TODO_SIM_WITHOUT_QT_AT_ALL
CONFIG += WITH_SERIAL
QT += printsupport #printsupport required from MacOS, otherwise crashes strangely ('This application failed to start because it could not find or load the Qt platform plugin "cocoa"')

WITH_GUI {
    QT      += widgets
    DEFINES += SIM_WITH_GUI
    greaterThan(QT_MAJOR_VERSION,4): DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
}

WITH_OPENGL {
    QT      += opengl
    DEFINES += SIM_WITH_OPENGL
} else {
    QT -= gui
}

WITH_SERIAL {
    QT      += serialport
    DEFINES += SIM_WITH_SERIAL
}

!WITH_QT {
    DEFINES += SIM_WITHOUT_QT_AT_ALL
    QT -= core
}



# Compiler switches.
# Switch a few warnings off (V-REP is still going through heavy refactoring)
*-msvc* {
    QMAKE_CXXFLAGS += -O2
    QMAKE_CXXFLAGS += -fp:precise
    QMAKE_CXXFLAGS += -W3 # warning level
    QMAKE_CXXFLAGS_WARN_ON -= -w34100
    QMAKE_CXXFLAGS_WARN_ON -= -w34189
    QMAKE_CXXFLAGS_WARN_ON -= -w44996

    QMAKE_CXXFLAGS += -wd4100 # "unreferenced formal parameter"
    QMAKE_CXXFLAGS += -wd4189 # "local variable is initialized but not referenced"
    QMAKE_CXXFLAGS += -wd4996 # "function or variable may be unsafe..."
    QMAKE_CXXFLAGS += -wd4101 # "unreferenced local variable"
}

*-g++* { #includes MinGW

    CONFIG(debug,debug|release) {
        QMAKE_CXXFLAGS += -g -ggdb
        message( debug )
    } else {
        QMAKE_CFLAGS_RELEASE += -O3
        QMAKE_CXXFLAGS += -O3
        message( release )
    }

    QMAKE_CFLAGS_RELEASE += -Wall
    QMAKE_CXXFLAGS += -Wall

    QMAKE_CFLAGS_RELEASE += -Wno-strict-aliasing
    QMAKE_CFLAGS_RELEASE += -Wno-unused-parameter
    QMAKE_CFLAGS_RELEASE += -Wno-unused-but-set-variable
    QMAKE_CFLAGS_RELEASE += -Wno-unused-local-typedefs

    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-strict-aliasing
    QMAKE_CXXFLAGS += -Wno-empty-body
    QMAKE_CXXFLAGS += -Wno-write-strings
    QMAKE_CXXFLAGS += -Wno-unused-but-set-variable
    QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
    QMAKE_CXXFLAGS += -Wno-narrowing
    #QMAKE_CXXFLAGS += -ggdb
}

clang* {

    CONFIG(debug,debug|release) {
        QMAKE_CXXFLAGS += -g -ggdb
        message( debug )
    } else {
        QMAKE_CFLAGS_RELEASE += -O3
        QMAKE_CXXFLAGS += -O3
        message( release )
    }

    QMAKE_CFLAGS_RELEASE += -Wall
    QMAKE_CXXFLAGS += -Wall

    QMAKE_CFLAGS_RELEASE += -Wno-strict-aliasing
    QMAKE_CFLAGS_RELEASE += -Wno-unused-parameter
    QMAKE_CFLAGS_RELEASE += -Wno-unused-but-set-variable
    QMAKE_CFLAGS_RELEASE += -Wno-unused-local-typedefs

    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-strict-aliasing
    QMAKE_CXXFLAGS += -Wno-empty-body
    QMAKE_CXXFLAGS += -Wno-write-strings
    QMAKE_CXXFLAGS += -Wno-unused-but-set-variable
    QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
    QMAKE_CXXFLAGS += -Wno-narrowing
}

WITH_GUI {
RESOURCES += targaFiles.qrc
RESOURCES += toolbarFiles.qrc
RESOURCES += variousImageFiles.qrc
}

WITH_GUI {
RESOURCES += imageFiles_vrep.qrc
}

win32 {
    INCLUDEPATH += # your boost folder, e.g. "c:/boost_1_52_0" #
    WITH_GUI {
        INCLUDEPATH += # your QScintilla "Qt4" folder, e.g. "d:/QScintilla-gpl-2.7.2/Qt4Qt5" #
        INCLUDEPATH += # your QScintilla "include" folder, e.g. "d:/QScintilla-gpl-2.7.2/include" #
        LIBS += # your "libqscintilla2.a" location, e.g. "d:/QScintilla-gpl-2.7.2/release/release/libqscintilla2.a" #
    }
    INCLUDEPATH += # your lua5.1 "include" folder, e.g. "d:/Lua/5.1/include" #
    LIBS += #  your "lua5.1.lib" location, e.g. "d:/Lua/5.1/lib/lua5.1.lib" #
    LIBS += -lwinmm
    LIBS += -lShell32
    greaterThan(QT_MAJOR_VERSION,4) {
        greaterThan(QT_MINOR_VERSION,4) {
            DEFINES += QT55
            LIBS += -lopengl32
            # For now, we don't use QOpenGLWidget on Windows,
            # since there are still problems depending on the
            # machine
            #DEFINES += USING_QOPENGLWIDGET # otherwise QGLWidget
        }
    }
    DEFINES += WIN_VREP
    DEFINES += QT_VREP_64BIT
}

macx {
    INCLUDEPATH += # your boost folder if needed (but probably already in your paths) #
    WITH_GUI {
        INCLUDEPATH += # your QScintilla "Qt4" folder, e.g. "../../QScintilla-gpl-2.7.2/Qt4Qt5" #
        INCLUDEPATH += # your QScintilla "include" folder, e.g. "../../QScintilla-gpl-2.7.2/include" #
        LIBS += # your "libqscintilla2.dylib" location, e.g. "/usr/lib/libqscintilla2.dylib" #
    }
    INCLUDEPATH += # your lua5.1 "include" folder, e.g. "../../Lua/5.1/include" #
    LIBS += # your "liblua5.1.dylib" location, e.g. , e.g. "/usr/lib/liblua5.1.dylib" #
    greaterThan(QT_MAJOR_VERSION,4) {
        greaterThan(QT_MINOR_VERSION,4) {
            DEFINES += QT55
            # For now, do not use QOpenGLWidget on Mac:
            # When using QOpenGLWidget on Mac, there is a lot of flickering
            # and buffer swaps while doing the rendering as it seems.
            # The problem can be resolved by using a double buffer explicitely
            # (normally not needed with QOpenGLWidget) and using glFinish.
            # Then rendering looks normal, but the framerate drops by about 30%
            # DEFINES += USING_QOPENGLWIDGET # otherwise QGLWidget
        }
    }
    DEFINES += MAC_VREP
    DEFINES += QT_VREP_64BIT
}

unix:!macx {
    INCLUDEPATH += # your boost folder if needed (but probably already in your paths) #
    WITH_GUI {
        INCLUDEPATH += # your QScintilla "Qt4" folder, e.g. "../../QScintilla-gpl-2.7.2/Qt4Qt5" #
        INCLUDEPATH += # your QScintilla "include" folder, e.g. "../../QScintilla-gpl-2.7.2/include" #
        LIBS += # your "libqscintilla2.so" location, e.g. "../../QScintilla-commercial-2.7.2/release/libqscintilla2.so" #
    }
    contains(QMAKE_HOST.arch, x86_64):{ # 64 Bit
        INCLUDEPATH += # your lua5.1 64 "include" folder, e.g. "../../lua5_1_4_Linux26g4_64_lib/include" #
        LIBS += # your Lua lib location, e.g. -L"../../lua5_1_4_Linux26g4_64_lib/" -llua5.1 #
        DEFINES += QT_VREP_64BIT
    }
    !contains(QMAKE_HOST.arch, x86_64):{ # 32 Bit
        INCLUDEPATH += # your lua5.1 32 "include" folder, e.g. "../../lua5_1_4_Linux26g4_lib/include" #
        LIBS += # your Lua lib location, e.g. -L"../../lua5_1_4_Linux26g4_lib/" -llua5.1 #
    }
    greaterThan(QT_MAJOR_VERSION,4) {
        greaterThan(QT_MINOR_VERSION,4) {
            DEFINES += QT55
            # For now, we don't use QOpenGLWidget on Linux,
            # since there are still problems depending on the
            # machine
            #DEFINES += USING_QOPENGLWIDGET # otherwise QGLWidget
        }
    }
    DEFINES += LIN_VREP
}

INCLUDEPATH += "sourceCode"
INCLUDEPATH += "sourceCode/inverseKinematics/geomConstraintSolver"
INCLUDEPATH += "sourceCode/inverseKinematics/ik"
INCLUDEPATH += "sourceCode/shared/sharedBufferFunctions"
INCLUDEPATH += "sourceCode/communication"
INCLUDEPATH += "sourceCode/communication/tubes"
INCLUDEPATH += "sourceCode/communication/wireless"
INCLUDEPATH += "sourceCode/drawingObjects"
INCLUDEPATH += "sourceCode/platform"
INCLUDEPATH += "sourceCode/collections"
INCLUDEPATH += "sourceCode/collisions"
INCLUDEPATH += "sourceCode/distances"
INCLUDEPATH += "sourceCode/pathPlanning_old"
INCLUDEPATH += "sourceCode/motionPlanning_old"
INCLUDEPATH += "sourceCode/3dObjects"
INCLUDEPATH += "sourceCode/3dObjects/related"
INCLUDEPATH += "sourceCode/3dObjects/graphObjectRelated"
INCLUDEPATH += "sourceCode/3dObjects/millObjectRelated"
INCLUDEPATH += "sourceCode/3dObjects/pathObjectRelated"
INCLUDEPATH += "sourceCode/3dObjects/proximitySensorObjectRelated"
INCLUDEPATH += "sourceCode/3dObjects/shapeObjectRelated"
INCLUDEPATH += "sourceCode/3dObjects/visionSensorObjectRelated"
INCLUDEPATH += "sourceCode/mainContainers"
INCLUDEPATH += "sourceCode/mainContainers/sceneContainers"
INCLUDEPATH += "sourceCode/mainContainers/applicationContainers"
INCLUDEPATH += "sourceCode/luaScripting"
INCLUDEPATH += "sourceCode/luaScripting/customLuaFuncAndVar"
INCLUDEPATH += "sourceCode/pagesAndViews"
INCLUDEPATH += "sourceCode/importExport"
INCLUDEPATH += "sourceCode/textures"
INCLUDEPATH += "sourceCode/serialization"
INCLUDEPATH += "sourceCode/strings"
INCLUDEPATH += "sourceCode/interfaces"
INCLUDEPATH += "sourceCode/interfaces/interfaceStack"
INCLUDEPATH += "sourceCode/menusAndSimilar"
INCLUDEPATH += "sourceCode/variousFunctions"
INCLUDEPATH += "sourceCode/geometricAlgorithms"
INCLUDEPATH += "sourceCode/various"
INCLUDEPATH += "sourceCode/libsAndPlugins"
INCLUDEPATH += "sourceCode/visual"
INCLUDEPATH += "sourceCode/utils"
INCLUDEPATH += "sourceCode/sharedBufferFunctions"
INCLUDEPATH += "sourceCode/backwardCompatibility"
INCLUDEPATH += "sourceCode/backwardCompatibility/pathPlanning"
INCLUDEPATH += "sourceCode/backwardCompatibility/motionPlanning"
INCLUDEPATH += "sourceCode/customUserInterfaces"
INCLUDEPATH += "sourceCode/undoRedo"
INCLUDEPATH += "sourceCode/rendering"

WITH_SERIAL {
    INCLUDEPATH += "sourceCode/communication/serialPort"
}

WITH_GUI {
    INCLUDEPATH += "sourceCode/gui/qtPropertyBrowserRelated"
    INCLUDEPATH += "sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles"
    INCLUDEPATH += "sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserRelatedFiles"
    INCLUDEPATH += "sourceCode/gui/dialogs"
    INCLUDEPATH += "sourceCode/gui/dialogs/other"
    INCLUDEPATH += "sourceCode/gui/platform"
    INCLUDEPATH += "sourceCode/gui/mainContainers"
    INCLUDEPATH += "sourceCode/gui/mainContainers/sceneContainers"
    INCLUDEPATH += "sourceCode/gui/mainContainers/applicationContainers"
    INCLUDEPATH += "sourceCode/gui/editModes"
    INCLUDEPATH += "sourceCode/gui/modelBrowser"
    INCLUDEPATH += "sourceCode/gui/sceneHierarchy"
    INCLUDEPATH += "sourceCode/gui/selectors"
    INCLUDEPATH += "sourceCode/gui/scintillaRelated"
    INCLUDEPATH += "sourceCode/gui/menusAndSimilar"
    INCLUDEPATH += "sourceCode/gui/various"
    INCLUDEPATH += "sourceCode/gui/libs"
}

INCLUDEPATH += "../programming/include"
INCLUDEPATH += "../programming/v_repMath"

WITH_GUI {
    FORMS += ui/qdlgsettings.ui \
    ui/qdlglayers.ui \
    ui/qdlgavirecorder.ui \
    ui/qdlgselection.ui \
    ui/qdlgabout.ui \
    ui/qdlgsimulation.ui \
    ui/qdlginteractiveik.ui \
    ui/qdlgcommonproperties.ui \
    ui/qdlgdetectableproperties.ui \
    ui/qdlgmodelproperties.ui \
    ui/qdlgassembly.ui \
    ui/qdlgslider.ui \
    ui/qdlgslider2.ui \
    ui/qdlginertiatensor.ui \
    ui/qdlgmodelthumbnailvisu.ui \
    ui/qdlgmodelthumbnail.ui \
    ui/qdlgenvironment.ui \
    ui/qdlgcolor.ui \
    ui/qdlgmaterial.ui \
    ui/qdlglightmaterial.ui \
    ui/qdlgcolorpulsation.ui \
    ui/qdlgfog.ui \
    ui/qdlgcollections.ui \
    ui/qdlgcollisions.ui \
    ui/qdlgdistances.ui \
    ui/qdlgentityvsentityselection.ui \
    ui/qdlgscripts.ui \
    ui/qdlginsertscript.ui \
    ui/qdlgdynamics.ui \
    ui/qdlgscaling.ui \
    ui/qdlgcameras.ui \
    ui/qdlglights.ui \
    ui/qdlgmirrors.ui \
    ui/qdlgdummies.ui \
    ui/qdlgoctrees.ui \
    ui/qdlgpointclouds.ui \
    ui/qdlgscriptparameters.ui \
    ui/qdlgforcesensors.ui \
    ui/qdlgprimitives.ui \
    ui/qdlgconvexdecomposition.ui \
    ui/qdlgconstraintsolver.ui \
    ui/qdlgvisionsensors.ui \
    ui/qdlgimagecolor.ui \
    ui/qdlgsimpleSearch.ui \
    ui/qdlgfilters.ui \
    ui/qdlgsimplefilter_rotate.ui \
    ui/qdlgsimplefilter_uniformImage.ui \
    ui/qdlgsimplefilter_shift.ui \
    ui/qdlgsimplefilter_scaleCols.ui \
    ui/qdlgsimplefilter_resize.ui \
    ui/qdlgsimplefilter_coordExtraction.ui \
    ui/qdlgsimplefilter_velodyne.ui \
    ui/qdlgsimplefilter_pixelChange.ui \
    ui/qdlgsimplefilter_rectangularCut.ui \
    ui/qdlgsimplefilter_keepRemoveCols.ui \
    ui/qdlgsimplefilter_intensityScale.ui \
    ui/qdlgsimplefilter_edge.ui \
    ui/qdlgsimplefilter_correlation.ui \
    ui/qdlgsimplefilter_colorSegmentation.ui \
    ui/qdlgsimplefilter_circularCut.ui \
    ui/qdlgsimplefilter_blob.ui \
    ui/qdlgsimplefilter_binary.ui \
    ui/qdlgsimplefilter_5x5.ui \
    ui/qdlgsimplefilter_3x3.ui \
    ui/qdlgshapes.ui \
    ui/qdlgmultishapeedition.ui \
    ui/qdlgtextures.ui \
    ui/qdlgtextureselection.ui \
    ui/qdlggeometry.ui \
    ui/qdlgshapedyn.ui \
    ui/qdlgdetectionvolume.ui \
    ui/qdlgproximitysensors.ui \
    ui/qdlgproxsensdetectionparam.ui \
    ui/qdlgmills.ui \
    ui/qdlgobjectdialogcontainer.ui \
    ui/qdlgshapeeditioncontainer.ui \
    ui/qdlgcalcdialogcontainer.ui \
    ui/qdlgheightfielddimension.ui \
    ui/qdlgstopscripts.ui \
    ui/qdlgimportoptions.ui \
    ui/qdlgtextureloadoptions.ui \
    ui/qdlgprogress.ui \
    ui/qdlgjoints.ui \
    ui/qdlgdependencyequation.ui \
    ui/qdlgjointdyn.ui \
    ui/qdlggraphs.ui \
    ui/qdlgdatastreamselection.ui \
    ui/qdlg2d3dgraphproperties.ui \
    ui/qdlgaddgraphcurve.ui \
    ui/qdlgik.ui \
    ui/qdlgikelements.ui \
    ui/qdlgikconditional.ui \
    ui/qdlgikavoidance.ui \
    ui/qdlgui.ui \
    ui/qdlguidialogcontainer.ui \
    ui/qdlguibuttons.ui \
    ui/qdlgnewui.ui \
    ui/qdlguirolledup.ui \
    ui/qdlgpathplanning.ui \
    ui/qdlgmotionplanning.ui \
    ui/qdlgpathplanningparams.ui \
    ui/qdlgmotionplanningjoints.ui \
    ui/qdlgworkspacemetric.ui \
    ui/qdlgpathplanningaddnew.ui \
    ui/qdlgpaths.ui \
    ui/qdlgpathshaping.ui \
    ui/qdlgmessageandcheckbox.ui \
    ui/qdlgopenglsettings.ui \
    ui/qdlgtriangleedit.ui \
    ui/qdlgvertexedit.ui \
    ui/qdlgedgeedit.ui \
    ui/qdlgpathedit.ui \
    ui/qdlgtranslation.ui \
    ui/qdlgrotation.ui \
    ui/qdlgtranslationrotationcont.ui \
    ui/qdlgobjectmanipsettings.ui \
}

HEADERS += ../programming/v_repMath/mathDefines.h \
    ../programming/v_repMath/Vector.h \
    ../programming/v_repMath/MyMath.h \
    ../programming/v_repMath/MMatrix.h \
    ../programming/v_repMath/7Vector.h \
    ../programming/v_repMath/6X6Matrix.h \
    ../programming/v_repMath/6Vector.h \
    ../programming/v_repMath/4X4Matrix.h \
    ../programming/v_repMath/4X4FullMatrix.h \
    ../programming/v_repMath/4Vector.h \
    ../programming/v_repMath/3X3Matrix.h \
    ../programming/v_repMath/3Vector.h \
    sourceCode/various/vrepMainHeader.h

HEADERS += ../programming/include/v_repConst.h \
    ../programming/include/v_repTypes.h \

HEADERS += sourceCode/inverseKinematics/geomConstraintSolver/iKObject.h \
    sourceCode/inverseKinematics/geomConstraintSolver/iKObjCont.h \
    sourceCode/inverseKinematics/geomConstraintSolver/iKMesh.h \
    sourceCode/inverseKinematics/geomConstraintSolver/iKJoint.h \
    sourceCode/inverseKinematics/geomConstraintSolver/iKGraphObject.h \
    sourceCode/inverseKinematics/geomConstraintSolver/iKGraphObjCont.h \
    sourceCode/inverseKinematics/geomConstraintSolver/iKGraphNode.h \
    sourceCode/inverseKinematics/geomConstraintSolver/iKGraphJoint.h \
    sourceCode/inverseKinematics/geomConstraintSolver/iKDummy.h \
    sourceCode/inverseKinematics/geomConstraintSolver/iKChainCont.h \
    sourceCode/inverseKinematics/geomConstraintSolver/iKChain.h \
    sourceCode/inverseKinematics/geomConstraintSolver/geometricConstraintSolverInt.h \
    sourceCode/inverseKinematics/geomConstraintSolver/geometricConstraintSolver.h \
    sourceCode/inverseKinematics/geomConstraintSolver/gCSDefs.h \

HEADERS += sourceCode/inverseKinematics/ik/ikRoutine.h \
    sourceCode/inverseKinematics/ik/ikGroup.h \
    sourceCode/inverseKinematics/ik/ikEl.h \

HEADERS += sourceCode/shared/sharedBufferFunctions/sharedFloatVector.h \
    sourceCode/shared/sharedBufferFunctions/sharedIntVector.h \
    sourceCode/shared/sharedBufferFunctions/sharedUCharVector.h \

HEADERS += sourceCode/drawingObjects/bannerObject.h \
    sourceCode/drawingObjects/drawingObject2D.h \
    sourceCode/drawingObjects/drawingObject.h \
    sourceCode/drawingObjects/drawingContainer2D.h \
    sourceCode/drawingObjects/ptCloud_old.h \

HEADERS += sourceCode/platform/vVarious.h \
    sourceCode/platform/vThread.h \
    sourceCode/platform/vMutex.h \
    sourceCode/platform/vFileFinder.h \
    sourceCode/platform/vFile.h \
    sourceCode/platform/vDateTime.h \
    sourceCode/platform/vArchive.h

WITH_QT {
    HEADERS += sourceCode/platform/vSimUiMutex.h \
    sourceCode/platform/wThread.h
}

HEADERS += sourceCode/collections/regCollectionEl.h \
    sourceCode/collections/regCollection.h \

HEADERS += sourceCode/collisions/regCollision.h \
    sourceCode/collisions/collisionRoutine.h \

HEADERS += sourceCode/distances/regDist.h \
    sourceCode/distances/distanceRoutine.h \
    sourceCode/distances/statDistObj.h \

HEADERS += sourceCode/3dObjects/related/3DObject.h \
    sourceCode/3dObjects/related/convexVolume.h \
    sourceCode/3dObjects/related/viewableBase.h \

HEADERS += sourceCode/3dObjects/jointObject.h \
    sourceCode/3dObjects/camera.h \
    sourceCode/3dObjects/dummy.h \
    sourceCode/3dObjects/octree.h \
    sourceCode/3dObjects/pointCloud.h \
    sourceCode/3dObjects/forceSensor.h \
    sourceCode/3dObjects/graph.h \
    sourceCode/3dObjects/light.h \
    sourceCode/3dObjects/mill.h \
    sourceCode/3dObjects/mirror.h \
    sourceCode/3dObjects/shape.h \
    sourceCode/3dObjects/path.h \
    sourceCode/3dObjects/proximitySensor.h \
    sourceCode/3dObjects/visionSensor.h \

HEADERS += sourceCode/3dObjects/graphObjectRelated/graphingRoutines.h \
    sourceCode/3dObjects/graphObjectRelated/graphDataComb.h \
    sourceCode/3dObjects/graphObjectRelated/graphData.h \
    sourceCode/3dObjects/graphObjectRelated/staticGraphCurve.h \

HEADERS += sourceCode/3dObjects/millObjectRelated/cuttingRoutine.h \

HEADERS += sourceCode/3dObjects/pathObjectRelated/bezierPathPoint.h \
    sourceCode/3dObjects/pathObjectRelated/simplePathPoint.h \
    sourceCode/3dObjects/pathObjectRelated/pathPoint.h \
    sourceCode/3dObjects/pathObjectRelated/pathCont.h \

HEADERS += sourceCode/3dObjects/shapeObjectRelated/geometric.h \
    sourceCode/3dObjects/shapeObjectRelated/geomWrap.h \
    sourceCode/3dObjects/shapeObjectRelated/geomProxy.h \
    sourceCode/3dObjects/shapeObjectRelated/volInt.h \

HEADERS += sourceCode/backwardCompatibility/geomObject.h \
    sourceCode/backwardCompatibility/shapeComponent.h \
    sourceCode/backwardCompatibility/pathPlanning/pathPlanning.h \
    sourceCode/backwardCompatibility/pathPlanning/holonomicPathPlanning.h \
    sourceCode/backwardCompatibility/pathPlanning/holonomicPathNode.h \
    sourceCode/backwardCompatibility/pathPlanning/nonHolonomicPathPlanning.h \
    sourceCode/backwardCompatibility/pathPlanning/nonHolonomicPathNode.h \
    sourceCode/backwardCompatibility/motionPlanning/mpPhase1Node.h \
    sourceCode/backwardCompatibility/motionPlanning/mpPhase2Node.h \
    sourceCode/backwardCompatibility/motionPlanning/mpObject.h \

HEADERS += sourceCode/communication/tubes/commTube.h \

HEADERS += sourceCode/communication/wireless/broadcastDataContainer.h \
    sourceCode/communication/wireless/broadcastData.h \
    sourceCode/communication/wireless/broadcastDataVisual.h \

HEADERS += sourceCode/mainContainers/mainContainer.h \

HEADERS += sourceCode/mainContainers/sceneContainers/drawingContainer.h \
    sourceCode/mainContainers/sceneContainers/bannerContainer.h \
    sourceCode/mainContainers/sceneContainers/textureContainer.h \
    sourceCode/mainContainers/sceneContainers/signalContainer.h \
    sourceCode/mainContainers/sceneContainers/simulation.h \
    sourceCode/mainContainers/sceneContainers/registeredPathPlanningTasks.h \
    sourceCode/mainContainers/sceneContainers/registeredMotionPlanningTasks.h \
    sourceCode/mainContainers/sceneContainers/registerediks.h \
    sourceCode/mainContainers/sceneContainers/registeredCollections.h \
    sourceCode/mainContainers/sceneContainers/registeredDistances.h \
    sourceCode/mainContainers/sceneContainers/registeredCollisions.h \
    sourceCode/mainContainers/sceneContainers/objCont.h \
    sourceCode/mainContainers/sceneContainers/memorizedConfContainer.h \
    sourceCode/mainContainers/sceneContainers/mainSettings.h \
    sourceCode/mainContainers/sceneContainers/mainCont.h \
    sourceCode/mainContainers/sceneContainers/luaScriptContainer.h \
    sourceCode/mainContainers/sceneContainers/environment.h \
    sourceCode/mainContainers/sceneContainers/dynamicsContainer.h \
    sourceCode/mainContainers/sceneContainers/constraintSolverContainer.h \
    sourceCode/mainContainers/sceneContainers/commTubeContainer.h \
    sourceCode/mainContainers/sceneContainers/apiErrors.h \
    sourceCode/mainContainers/sceneContainers/outsideCommandQueue.h \
    sourceCode/mainContainers/sceneContainers/cacheCont.h \
    sourceCode/mainContainers/sceneContainers/customData.h \
    sourceCode/mainContainers/sceneContainers/ghostObjectContainer.h \
    sourceCode/mainContainers/sceneContainers/pointCloudContainer_old.h \
    sourceCode/mainContainers/sceneContainers/buttonBlockContainer.h \
    sourceCode/mainContainers/sceneContainers/undoBufferCont.h \

HEADERS += sourceCode/mainContainers/applicationContainers/copyBuffer.h \
    sourceCode/mainContainers/applicationContainers/persistentDataContainer.h \
    sourceCode/mainContainers/applicationContainers/simulatorMessageQueue.h \
    sourceCode/mainContainers/applicationContainers/calculationInfo.h \
    sourceCode/mainContainers/applicationContainers/interfaceStackContainer.h \

WITH_SERIAL {
    HEADERS += sourceCode/mainContainers/applicationContainers/serialPortContainer.h
}

HEADERS += sourceCode/3dObjects/visionSensorObjectRelated/imageProcess.h \
    sourceCode/3dObjects/visionSensorObjectRelated/simpleFilter.h \
    sourceCode/3dObjects/visionSensorObjectRelated/composedFilter.h \

WITH_OPENGL {
    HEADERS += sourceCode/3dObjects/visionSensorObjectRelated/offscreenGlContext.h \
    sourceCode/3dObjects/visionSensorObjectRelated/frameBufferObject.h \
    sourceCode/3dObjects/visionSensorObjectRelated/visionSensorGlStuff.h
}


HEADERS += sourceCode/pathPlanning_old/pathPlanningTask_old.h \

HEADERS += sourceCode/motionPlanning_old/motionPlanningTask_old.h \

HEADERS += sourceCode/luaScripting/luaScriptParameters.h \
    sourceCode/luaScripting/luaScriptObject.h \
    sourceCode/luaScripting/outsideCommandQueueForScript.h \
    sourceCode/luaScripting/luaWrapper.h \

HEADERS += sourceCode/luaScripting/customLuaFuncAndVar/luaCustomFuncAndVarContainer.h \
    sourceCode/luaScripting/customLuaFuncAndVar/luaCustomFunction.h \
    sourceCode/luaScripting/customLuaFuncAndVar/luaCustomVariable.h \

HEADERS += sourceCode/pagesAndViews/pageContainer.h \
    sourceCode/pagesAndViews/sPage.h \
    sourceCode/pagesAndViews/sView.h \

HEADERS += sourceCode/importExport/importExport.h \
    sourceCode/importExport/stlFile.h \
    sourceCode/importExport/objFile.h \
    sourceCode/importExport/dxfFile.h \

HEADERS += sourceCode/textures/textureObject.h \
    sourceCode/textures/imgLoaderSaver.h \
    sourceCode/textures/tGAFormat.h \
    sourceCode/textures/stb_image.h \
    sourceCode/textures/textureProperty.h \

HEADERS += sourceCode/serialization/ser.h \
    sourceCode/serialization/xmlSer.h \
    sourceCode/serialization/extIkSer.h \
    sourceCode/serialization/huffman.h \
    sourceCode/serialization/tinyxml2.cpp \

HEADERS += sourceCode/strings/v_repStringTable.h \
    sourceCode/strings/v_repStringTable_openGl.h \
    sourceCode/strings/v_repStringTable_noTranslation.h \

HEADERS += sourceCode/interfaces/v_rep.h \
    sourceCode/interfaces/v_rep_internal.h \
    sourceCode/interfaces/luaScriptFunctions.h \
    sourceCode/interfaces/pathPlanningInterface.h \
    sourceCode/interfaces/dummyClasses.h \

HEADERS += sourceCode/interfaces/interfaceStack/interfaceStack.h \
    sourceCode/interfaces/interfaceStack/interfaceStackObject.h \
    sourceCode/interfaces/interfaceStack/interfaceStackNull.h \
    sourceCode/interfaces/interfaceStack/interfaceStackBool.h \
    sourceCode/interfaces/interfaceStack/interfaceStackNumber.h \
    sourceCode/interfaces/interfaceStack/interfaceStackString.h \
    sourceCode/interfaces/interfaceStack/interfaceStackTable.h \

HEADERS += sourceCode/menusAndSimilar/fileOperations.h \
    sourceCode/menusAndSimilar/addOperations.h \

HEADERS += sourceCode/variousFunctions/sceneObjectOperations.h \

HEADERS += sourceCode/geometricAlgorithms/linMotionRoutines.h \
    sourceCode/geometricAlgorithms/meshRoutines.h \
    sourceCode/geometricAlgorithms/meshManip.h \
    sourceCode/geometricAlgorithms/edgeElement.h \
    sourceCode/geometricAlgorithms/algos.h \

HEADERS += sourceCode/various/vrepConfig.h \
    sourceCode/various/gV.h \
    sourceCode/various/global.h \
    sourceCode/various/embeddedFonts.h \
    sourceCode/various/userSettings.h \
    sourceCode/various/memorizedConf.h \
    sourceCode/various/uiThread.h \
    sourceCode/various/simThread.h \
    sourceCode/various/app.h \
    sourceCode/various/directoryPaths.h \
    sourceCode/various/constraintSolverObject.h \
    sourceCode/various/dynMaterialObject.h \
    sourceCode/various/easyLock.h \
    sourceCode/various/funcDebug.h \
    sourceCode/various/ghostObject.h \
    sourceCode/various/debugLogFile.h \
    sourceCode/various/vrepConfig.h

HEADERS += sourceCode/undoRedo/undoBufferArrays.h \
    sourceCode/undoRedo/undoBuffer.h \
    sourceCode/undoRedo/undoBufferCameras.h \

HEADERS += sourceCode/rendering/rendering.h \
    sourceCode/rendering/cameraRendering.h \
    sourceCode/rendering/visionSensorRendering.h \
    sourceCode/rendering/forceSensorRendering.h \
    sourceCode/rendering/proximitySensorRendering.h \
    sourceCode/rendering/dummyRendering.h \
    sourceCode/rendering/lightRendering.h \
    sourceCode/rendering/pointCloudRendering.h \
    sourceCode/rendering/octreeRendering.h \
    sourceCode/rendering/jointRendering.h \
    sourceCode/rendering/graphRendering.h \
    sourceCode/rendering/millRendering.h \
    sourceCode/rendering/mirrorRendering.h \
    sourceCode/rendering/pathRendering.h \
    sourceCode/rendering/shapeRendering.h \
    sourceCode/rendering/ghostRendering.h \
    sourceCode/rendering/drawingObjectRendering.h \
    sourceCode/rendering/drawingObject2DRendering.h \
    sourceCode/rendering/ptCloudRendering_old.h \
    sourceCode/rendering/collisionContourRendering.h \
    sourceCode/rendering/distanceRendering.h \
    sourceCode/rendering/bannerRendering.h \
    sourceCode/rendering/thumbnailRendering.h \
    sourceCode/rendering/pathPlanningTaskRendering_old.h \
    sourceCode/rendering/motionPlanningTaskRendering_old.h \
    sourceCode/rendering/broadcastDataVisualRendering.h \
    sourceCode/rendering/dynamicsRendering.h \
    sourceCode/rendering/environmentRendering.h \
    sourceCode/rendering/pageRendering.h \
    sourceCode/rendering/viewRendering.h \


WITH_QT {
    HEADERS += sourceCode/various/simAndUiThreadSync.h \
    sourceCode/various/vrepQApp.h
}

HEADERS += sourceCode/libsAndPlugins/pluginContainer.h \
    sourceCode/libsAndPlugins/auxLib.h \

HEADERS += sourceCode/visual/visualParam.h \
    sourceCode/visual/thumbnail.h \

WITH_OPENGL {
    HEADERS += sourceCode/visual/oGL.h \
    sourceCode/visual/oglExt.h \
    sourceCode/visual/glShader.h \
    sourceCode/visual/glBufferObjects.h \
}

HEADERS += sourceCode/utils/threadPool.h \
    sourceCode/utils/tt.h \
    sourceCode/utils/ttUtil.h \
    sourceCode/utils/confReaderAndWriter.h \

HEADERS += sourceCode/customUserInterfaces/buttonBlock.h \
    sourceCode/customUserInterfaces/softButton.h \

WITH_SERIAL {
    HEADERS += sourceCode/communication/serialPort/serialPort.h \
    sourceCode/communication/serialPort/serial_mac_linux.h
}

WITH_GUI {
    HEADERS += sourceCode/gui/dialogs/qdlglayers.h \
    sourceCode/gui/dialogs/qdlgavirecorder.h \
    sourceCode/gui/dialogs/qdlgselection.h \
    sourceCode/gui/dialogs/qdlgabout.h \
    sourceCode/gui/dialogs/qdlgsimulation.h \
    sourceCode/gui/dialogs/qdlginteractiveik.h \
    sourceCode/gui/dialogs/qdlgcommonproperties.h \
    sourceCode/gui/dialogs/qdlgdetectableproperties.h \
    sourceCode/gui/dialogs/qdlgmodelproperties.h \
    sourceCode/gui/dialogs/qdlgassembly.h \
    sourceCode/gui/dialogs/qdlgslider.h \
    sourceCode/gui/dialogs/qdlgslider2.h \
    sourceCode/gui/dialogs/qdlginertiatensor.h \
    sourceCode/gui/dialogs/qdlgmodelthumbnailvisu.h \
    sourceCode/gui/dialogs/qdlgmodelthumbnail.h \
    sourceCode/gui/dialogs/qdlgenvironment.h \
    sourceCode/gui/dialogs/qdlgcolor.h \
    sourceCode/gui/dialogs/qdlgmaterial.h \
    sourceCode/gui/dialogs/qdlglightmaterial.h \
    sourceCode/gui/dialogs/qdlgcolorpulsation.h \
    sourceCode/gui/dialogs/qdlgfog.h \
    sourceCode/gui/dialogs/qdlgcollections.h \
    sourceCode/gui/dialogs/qdlgcollisions.h \
    sourceCode/gui/dialogs/qdlgdistances.h \
    sourceCode/gui/dialogs/qdlgentityvsentityselection.h \
    sourceCode/gui/dialogs/qdlgscripts.h \
    sourceCode/gui/dialogs/qdlginsertscript.h \
    sourceCode/gui/dialogs/qdlgdynamics.h \
    sourceCode/gui/dialogs/qdlgscaling.h \
    sourceCode/gui/dialogs/qdlgcameras.h \
    sourceCode/gui/dialogs/qdlglights.h \
    sourceCode/gui/dialogs/qdlgmirrors.h \
    sourceCode/gui/dialogs/qdlgdummies.h \
    sourceCode/gui/dialogs/qdlgoctrees.h \
    sourceCode/gui/dialogs/qdlgpointclouds.h \
    sourceCode/gui/dialogs/qdlgscriptparameters.h \
    sourceCode/gui/dialogs/qdlgforcesensors.h \
    sourceCode/gui/dialogs/qdlgprimitives.h \
    sourceCode/gui/dialogs/qdlgconvexdecomposition.h \
    sourceCode/gui/dialogs/qdlgconstraintsolver.h \
    sourceCode/gui/dialogs/qdlgvisionsensors.h \
    sourceCode/gui/dialogs/qdlgimagecolor.h \
    sourceCode/gui/dialogs/qdlgsimpleSearch.h \
    sourceCode/gui/dialogs/qdlgfilters.h \
    sourceCode/gui/dialogs/qdlgsimplefilter.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_rotate.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_uniformImage.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_shift.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_coordExtraction.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_velodyne.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_pixelChange.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_scaleCols.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_resize.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_rectangularCut.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_keepRemoveCols.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_intensityScale.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_edge.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_correlation.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_colorSegmentation.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_circularCut.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_blob.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_binary.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_5x5.h \
    sourceCode/gui/dialogs/qdlgsimplefilter_3x3.h \
    sourceCode/gui/dialogs/qdlgshapes.h \
    sourceCode/gui/dialogs/qdlgmultishapeedition.h \
    sourceCode/gui/dialogs/qdlgtextures.h \
    sourceCode/gui/dialogs/qdlgtextureselection.h \
    sourceCode/gui/dialogs/qdlggeometry.h \
    sourceCode/gui/dialogs/qdlgshapedyn.h \
    sourceCode/gui/dialogs/qdlgdetectionvolume.h \
    sourceCode/gui/dialogs/qdlgproximitysensors.h \
    sourceCode/gui/dialogs/qdlgproxsensdetectionparam.h \
    sourceCode/gui/dialogs/qdlgmills.h \
    sourceCode/gui/dialogs/qdlgobjectdialogcontainer.h \
    sourceCode/gui/dialogs/qdlgshapeeditioncontainer.h \
    sourceCode/gui/dialogs/qdlgcalcdialogcontainer.h \
    sourceCode/gui/dialogs/qdlgheightfielddimension.h \
    sourceCode/gui/dialogs/qdlgstopscripts.h \
    sourceCode/gui/dialogs/qdlgimportoptions.h \
    sourceCode/gui/dialogs/qdlgtextureloadoptions.h \
    sourceCode/gui/dialogs/qdlgprogress.h \
    sourceCode/gui/dialogs/qdlgjoints.h \
    sourceCode/gui/dialogs/qdlgdependencyequation.h \
    sourceCode/gui/dialogs/qdlgjointdyn.h \
    sourceCode/gui/dialogs/qdlggraphs.h \
    sourceCode/gui/dialogs/qdlgdatastreamselection.h \
    sourceCode/gui/dialogs/qdlg2d3dgraphproperties.h \
    sourceCode/gui/dialogs/qdlgaddgraphcurve.h \
    sourceCode/gui/dialogs/qdlgik.h \
    sourceCode/gui/dialogs/qdlgikelements.h \
    sourceCode/gui/dialogs/qdlgikconditional.h \
    sourceCode/gui/dialogs/qdlgikavoidance.h \
    sourceCode/gui/dialogs/qdlgui.h \
    sourceCode/gui/dialogs/qdlguidialogcontainer.h \
    sourceCode/gui/dialogs/qdlguibuttons.h \
    sourceCode/gui/dialogs/qdlgnewui.h \
    sourceCode/gui/dialogs/qdlguirolledup.h \
    sourceCode/gui/dialogs/qdlgpathplanning.h \
    sourceCode/gui/dialogs/qdlgmotionplanning.h \
    sourceCode/gui/dialogs/qdlgpathplanningparams.h \
    sourceCode/gui/dialogs/qdlgmotionplanningjoints.h \
    sourceCode/gui/dialogs/qdlgworkspacemetric.h \
    sourceCode/gui/dialogs/qdlgpathplanningaddnew.h \
    sourceCode/gui/dialogs/qdlgpaths.h \
    sourceCode/gui/dialogs/qdlgpathshaping.h \
    sourceCode/gui/dialogs/qdlgmessageandcheckbox.h \
    sourceCode/gui/dialogs/qdlgopenglsettings.h \
    sourceCode/gui/dialogs/qdlgtriangleedit.h \
    sourceCode/gui/dialogs/qdlgvertexedit.h \
    sourceCode/gui/dialogs/qdlgedgeedit.h \
    sourceCode/gui/dialogs/qdlgpathedit.h \
    sourceCode/gui/dialogs/qdlgtranslation.h \
    sourceCode/gui/dialogs/qdlgrotation.h \
    sourceCode/gui/dialogs/qdlgtranslationrotationcont.h \
    sourceCode/gui/dialogs/qdlgsettings.h \
    sourceCode/gui/dialogs/qdlgobjectmanipsettings.h \

    HEADERS += sourceCode/gui/dialogs/other/editboxdelegate.h \
    sourceCode/gui/dialogs/other/toolDlgWrapper.h \
    sourceCode/gui/dialogs/other/dlgEx.h \
    sourceCode/gui/dialogs/other/dlgCont.h \
    sourceCode/gui/dialogs/other/qvrep_openglwidget.h \

    HEADERS += sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qtbuttonpropertybrowser.h \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qteditorfactory.h \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qtgroupboxpropertybrowser.h \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qtpropertybrowser.h \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qtpropertybrowserutils_p.h \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qtpropertymanager.h \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qttreepropertybrowser.h \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qtvariantproperty.h \

    HEADERS += sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserRelatedFiles/buttoneditfactory.h \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserRelatedFiles/buttoneditmanager.h \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserRelatedFiles/pushbuttonedit.h \

    HEADERS += sourceCode/gui/qtPropertyBrowserRelated/propBrowser_engineProp_material.h \
    sourceCode/gui/qtPropertyBrowserRelated/propBrowser_engineProp_general.h \
    sourceCode/gui/qtPropertyBrowserRelated/propBrowser_engineProp_joint.h \

    HEADERS += sourceCode/gui/platform/vMessageBox.h \
    sourceCode/gui/platform/vMenubar.h \
    sourceCode/gui/platform/vFileDialog.h \
    sourceCode/gui/platform/vDialog.h \

    HEADERS += sourceCode/gui/mainContainers/sceneContainers/scintillaEditorContainer.h \
    sourceCode/gui/mainContainers/sceneContainers/genericDialogContainer.h \
    sourceCode/gui/mainContainers/sceneContainers/oglSurface.h \

    HEADERS += sourceCode/gui/mainContainers/applicationContainers/scintillaConsoleContainer.h \
    sourceCode/gui/mainContainers/applicationContainers/globalGuiTextureContainer.h \

    HEADERS += sourceCode/gui/editModes/editModeContainer.h \
    sourceCode/gui/editModes/shapeEditMode.h \
    sourceCode/gui/editModes/multishapeEditMode.h \
    sourceCode/gui/editModes/pathEditMode.h \
    sourceCode/gui/editModes/uiEditMode.h \
    sourceCode/gui/editModes/edgeCont.h \
    sourceCode/gui/editModes/pathPointManipulation.h \

    HEADERS += sourceCode/gui/sceneHierarchy/hierarchy.h \
    sourceCode/gui/sceneHierarchy/hierarchyElement.h \
    sourceCode/gui/sceneHierarchy/sceneHierarchyWidget.h \

    HEADERS += sourceCode/gui/modelBrowser/modelFolderWidget.h \
    sourceCode/gui/modelBrowser/modelListWidget.h \
    sourceCode/gui/modelBrowser/modelListWidgetBase.h \

    HEADERS += sourceCode/gui/selectors/viewSelector.h \
    sourceCode/gui/selectors/pageSelector.h \
    sourceCode/gui/selectors/sceneSelector.h \

    HEADERS += sourceCode/gui/scintillaRelated/scintillaEditor.h \
    sourceCode/gui/scintillaRelated/scintillaDlg.h \
    sourceCode/gui/scintillaRelated/scintillaModalDlg.h \
    sourceCode/gui/scintillaRelated/scintillaUserModalDlg.h \
    sourceCode/gui/scintillaRelated/scintillaConsoleDlg.h \

    HEADERS += sourceCode/gui/menusAndSimilar/toolBarCommand.h \
    sourceCode/gui/menusAndSimilar/customMenuBarItemContainer.h \
    sourceCode/gui/menusAndSimilar/customMenuBarItem.h \
    sourceCode/gui/menusAndSimilar/mainWindow.h \
    sourceCode/gui/menusAndSimilar/helpMenu.h \

    HEADERS += sourceCode/gui/various/simRecorder.h \
    sourceCode/gui/various/genericDialog.h \

    HEADERS += sourceCode/gui/libs/auxLibVideo.h \
}

#**********************************************************************
#**********************************************************************

SOURCES += ../programming/v_repMath/Vector.cpp \
    ../programming/v_repMath/MyMath.cpp \
    ../programming/v_repMath/MMatrix.cpp \
    ../programming/v_repMath/7Vector.cpp \
    ../programming/v_repMath/6X6Matrix.cpp \
    ../programming/v_repMath/6Vector.cpp \
    ../programming/v_repMath/4X4Matrix.cpp \
    ../programming/v_repMath/4X4FullMatrix.cpp \
    ../programming/v_repMath/4Vector.cpp \
    ../programming/v_repMath/3X3Matrix.cpp \
    ../programming/v_repMath/3Vector.cpp \

SOURCES += sourceCode/inverseKinematics/geomConstraintSolver/iKObject.cpp \
    sourceCode/inverseKinematics/geomConstraintSolver/iKObjCont.cpp \
    sourceCode/inverseKinematics/geomConstraintSolver/iKMesh.cpp \
    sourceCode/inverseKinematics/geomConstraintSolver/iKJoint.cpp \
    sourceCode/inverseKinematics/geomConstraintSolver/iKGraphObject.cpp \
    sourceCode/inverseKinematics/geomConstraintSolver/iKGraphObjCont.cpp \
    sourceCode/inverseKinematics/geomConstraintSolver/iKGraphNode.cpp \
    sourceCode/inverseKinematics/geomConstraintSolver/iKGraphJoint.cpp \
    sourceCode/inverseKinematics/geomConstraintSolver/iKDummy.cpp \
    sourceCode/inverseKinematics/geomConstraintSolver/iKChainCont.cpp \
    sourceCode/inverseKinematics/geomConstraintSolver/iKChain.cpp \
    sourceCode/inverseKinematics/geomConstraintSolver/geometricConstraintSolverInt.cpp \
    sourceCode/inverseKinematics/geomConstraintSolver/geometricConstraintSolver.cpp \

SOURCES += sourceCode/inverseKinematics/ik/ikRoutine.cpp \
    sourceCode/inverseKinematics/ik/ikGroup.cpp \
    sourceCode/inverseKinematics/ik/ikEl.cpp \

SOURCES += sourceCode/shared/sharedBufferFunctions/sharedFloatVector.cpp \
    sourceCode/shared/sharedBufferFunctions/sharedIntVector.cpp \
    sourceCode/shared/sharedBufferFunctions/sharedUCharVector.cpp \

SOURCES += sourceCode/drawingObjects/bannerObject.cpp \
    sourceCode/drawingObjects/drawingObject2D.cpp \
    sourceCode/drawingObjects/drawingObject.cpp \
    sourceCode/drawingObjects/drawingContainer2D.cpp \
    sourceCode/drawingObjects/ptCloud_old.cpp \

SOURCES += sourceCode/platform/vVarious.cpp \
    sourceCode/platform/vThread.cpp \
    sourceCode/platform/vMutex.cpp \
    sourceCode/platform/vFileFinder.cpp \
    sourceCode/platform/vFile.cpp \
    sourceCode/platform/vDateTime.cpp \
    sourceCode/platform/vArchive.cpp

WITH_QT {
    SOURCES += sourceCode/platform/vSimUiMutex.cpp \
    sourceCode/platform/wThread.cpp
}


SOURCES += sourceCode/collections/regCollectionEl.cpp \
    sourceCode/collections/regCollection.cpp \

SOURCES += sourceCode/collisions/regCollision.cpp \
    sourceCode/collisions/collisionRoutine.cpp \

SOURCES += sourceCode/distances/regDist.cpp \
    sourceCode/distances/distanceRoutine.cpp \
    sourceCode/distances/statDistObj.cpp \

SOURCES += sourceCode/3dObjects/related/3DObject.cpp \
    sourceCode/3dObjects/related/convexVolume.cpp \
    sourceCode/3dObjects/related/viewableBase.cpp \

SOURCES += sourceCode/3dObjects/jointObject.cpp \
    sourceCode/3dObjects/camera.cpp \
    sourceCode/3dObjects/dummy.cpp \
    sourceCode/3dObjects/octree.cpp \
    sourceCode/3dObjects/pointCloud.cpp \
    sourceCode/3dObjects/forceSensor.cpp \
    sourceCode/3dObjects/graph.cpp \
    sourceCode/3dObjects/light.cpp \
    sourceCode/3dObjects/mirror.cpp \
    sourceCode/3dObjects/mill.cpp \
    sourceCode/3dObjects/path.cpp \
    sourceCode/3dObjects/proximitySensor.cpp \
    sourceCode/3dObjects/shape.cpp \
    sourceCode/3dObjects/visionSensor.cpp \

SOURCES += sourceCode/3dObjects/graphObjectRelated/graphingRoutines.cpp \
    sourceCode/3dObjects/graphObjectRelated/graphDataComb.cpp \
    sourceCode/3dObjects/graphObjectRelated/graphData.cpp \
    sourceCode/3dObjects/graphObjectRelated/staticGraphCurve.cpp \

SOURCES += sourceCode/3dObjects/millObjectRelated/cuttingRoutine.cpp \

SOURCES += sourceCode/3dObjects/pathObjectRelated/bezierPathPoint.cpp \
    sourceCode/3dObjects/pathObjectRelated/simplePathPoint.cpp \
    sourceCode/3dObjects/pathObjectRelated/pathPoint.cpp \
    sourceCode/3dObjects/pathObjectRelated/pathCont.cpp \

SOURCES += sourceCode/3dObjects/proximitySensorObjectRelated/proxSensorRoutine.cpp \

SOURCES += sourceCode/3dObjects/shapeObjectRelated/geometric.cpp \
    sourceCode/3dObjects/shapeObjectRelated/geomWrap.cpp \
    sourceCode/3dObjects/shapeObjectRelated/geomProxy.cpp \
    sourceCode/3dObjects/shapeObjectRelated/volInt.cpp \


SOURCES += sourceCode/backwardCompatibility/geomObject.cpp \
    sourceCode/backwardCompatibility/shapeComponent.cpp \
    sourceCode/backwardCompatibility/pathPlanning/pathPlanning.cpp \
    sourceCode/backwardCompatibility/pathPlanning/holonomicPathPlanning.cpp \
    sourceCode/backwardCompatibility/pathPlanning/holonomicPathNode.cpp \
    sourceCode/backwardCompatibility/pathPlanning/nonHolonomicPathPlanning.cpp \
    sourceCode/backwardCompatibility/pathPlanning/nonHolonomicPathNode.cpp \
    sourceCode/backwardCompatibility/motionPlanning/mpPhase1Node.cpp \
    sourceCode/backwardCompatibility/motionPlanning/mpPhase2Node.cpp \
    sourceCode/backwardCompatibility/motionPlanning/mpObject.cpp \

SOURCES += sourceCode/communication/tubes/commTube.cpp \

SOURCES += sourceCode/communication/wireless/broadcastDataContainer.cpp \
    sourceCode/communication/wireless/broadcastData.cpp \
    sourceCode/communication/wireless/broadcastDataVisual.cpp \

SOURCES += sourceCode/mainContainers/mainContainer.cpp \

SOURCES += sourceCode/mainContainers/sceneContainers/bannerContainer.cpp \
    sourceCode/mainContainers/sceneContainers/drawingContainer.cpp \
    sourceCode/mainContainers/sceneContainers/textureContainer.cpp \
    sourceCode/mainContainers/sceneContainers/simulation.cpp \
    sourceCode/mainContainers/sceneContainers/signalContainer.cpp \
    sourceCode/mainContainers/sceneContainers/registeredPathPlanningTasks.cpp \
    sourceCode/mainContainers/sceneContainers/registeredMotionPlanningTasks.cpp \
    sourceCode/mainContainers/sceneContainers/registerediks.cpp \
    sourceCode/mainContainers/sceneContainers/registeredCollections.cpp \
    sourceCode/mainContainers/sceneContainers/registeredDistances.cpp \
    sourceCode/mainContainers/sceneContainers/registeredCollisions.cpp \
    sourceCode/mainContainers/sceneContainers/outsideCommandQueue.cpp \
    sourceCode/mainContainers/sceneContainers/objCont.cpp \
    sourceCode/mainContainers/sceneContainers/memorizedConfContainer.cpp \
    sourceCode/mainContainers/sceneContainers/mainSettings.cpp \
    sourceCode/mainContainers/sceneContainers/mainCont.cpp \
    sourceCode/mainContainers/sceneContainers/luaScriptContainer.cpp \
    sourceCode/mainContainers/sceneContainers/environment.cpp \
    sourceCode/mainContainers/sceneContainers/dynamicsContainer.cpp \
    sourceCode/mainContainers/sceneContainers/customData.cpp \
    sourceCode/mainContainers/sceneContainers/constraintSolverContainer.cpp \
    sourceCode/mainContainers/sceneContainers/commTubeContainer.cpp \
    sourceCode/mainContainers/sceneContainers/cacheCont.cpp \
    sourceCode/mainContainers/sceneContainers/apiErrors.cpp \
    sourceCode/mainContainers/sceneContainers/ghostObjectContainer.cpp \
    sourceCode/mainContainers/sceneContainers/pointCloudContainer_old.cpp \
    sourceCode/mainContainers/sceneContainers/buttonBlockContainer.cpp \
    sourceCode/mainContainers/sceneContainers/undoBufferCont.cpp \

SOURCES += sourceCode/mainContainers/applicationContainers/copyBuffer.cpp \
    sourceCode/mainContainers/applicationContainers/persistentDataContainer.cpp \
    sourceCode/mainContainers/applicationContainers/simulatorMessageQueue.cpp \
    sourceCode/mainContainers/applicationContainers/calculationInfo.cpp \
    sourceCode/mainContainers/applicationContainers/interfaceStackContainer.cpp \

WITH_SERIAL {
    SOURCES += sourceCode/mainContainers/applicationContainers/serialPortContainer.cpp
}

SOURCES += sourceCode/3dObjects/visionSensorObjectRelated/imageProcess.cpp \
    sourceCode/3dObjects/visionSensorObjectRelated/simpleFilter.cpp \
    sourceCode/3dObjects/visionSensorObjectRelated/composedFilter.cpp \

WITH_OPENGL {
    SOURCES += sourceCode/3dObjects/visionSensorObjectRelated/offscreenGlContext.cpp \
    sourceCode/3dObjects/visionSensorObjectRelated/frameBufferObject.cpp \
    sourceCode/3dObjects/visionSensorObjectRelated/visionSensorGlStuff.cpp \
}


SOURCES += sourceCode/pathPlanning_old/pathPlanningTask_old.cpp \

SOURCES += sourceCode/motionPlanning_old/motionPlanningTask_old.cpp \

SOURCES += sourceCode/luaScripting/luaScriptParameters.cpp \
    sourceCode/luaScripting/luaScriptObject.cpp \
    sourceCode/luaScripting/outsideCommandQueueForScript.cpp \
    sourceCode/luaScripting/luaWrapper.cpp \

SOURCES += sourceCode/luaScripting/customLuaFuncAndVar/luaCustomFuncAndVarContainer.cpp \
    sourceCode/luaScripting/customLuaFuncAndVar/luaCustomFunction.cpp \
    sourceCode/luaScripting/customLuaFuncAndVar/luaCustomVariable.cpp \

SOURCES += sourceCode/pagesAndViews/pageContainer.cpp \
    sourceCode/pagesAndViews/sPage.cpp \
    sourceCode/pagesAndViews/sView.cpp \

SOURCES += sourceCode/importExport/importExport.cpp \
    sourceCode/importExport/objFile.cpp \
    sourceCode/importExport/dxfFile.cpp \
    sourceCode/importExport/stlFile.cpp \

SOURCES += sourceCode/textures/textureObject.cpp \
    sourceCode/textures/imgLoaderSaver.cpp \
    sourceCode/textures/tGAFormat.cpp \
    sourceCode/textures/stb_image.c \
    sourceCode/textures/textureProperty.cpp \

SOURCES += sourceCode/serialization/ser.cpp \
    sourceCode/serialization/xmlSer.cpp \
    sourceCode/serialization/extIkSer.cpp \
    sourceCode/serialization/huffman.c \
    sourceCode/serialization/tinyxml2.cpp \

SOURCES += sourceCode/interfaces/v_rep.cpp \
    sourceCode/interfaces/v_rep_internal.cpp \
    sourceCode/interfaces/luaScriptFunctions.cpp \
    sourceCode/interfaces/pathPlanningInterface.cpp \

SOURCES += sourceCode/interfaces/interfaceStack/interfaceStack.cpp \
    sourceCode/interfaces/interfaceStack/interfaceStackObject.cpp \
    sourceCode/interfaces/interfaceStack/interfaceStackNull.cpp \
    sourceCode/interfaces/interfaceStack/interfaceStackBool.cpp \
    sourceCode/interfaces/interfaceStack/interfaceStackNumber.cpp \
    sourceCode/interfaces/interfaceStack/interfaceStackString.cpp \
    sourceCode/interfaces/interfaceStack/interfaceStackTable.cpp \

SOURCES += sourceCode/menusAndSimilar/addOperations.cpp \
    sourceCode/menusAndSimilar/fileOperations.cpp \

SOURCES += sourceCode/variousFunctions/sceneObjectOperations.cpp \

SOURCES += sourceCode/geometricAlgorithms/linMotionRoutines.cpp \
    sourceCode/geometricAlgorithms/meshRoutines.cpp \
    sourceCode/geometricAlgorithms/meshManip.cpp \
    sourceCode/geometricAlgorithms/edgeElement.cpp \
    sourceCode/geometricAlgorithms/algos.cpp \

SOURCES += sourceCode/various/gV.cpp \
    sourceCode/various/memorizedConf.cpp \
    sourceCode/various/userSettings.cpp \
    sourceCode/various/directoryPaths.cpp \
    sourceCode/various/constraintSolverObject.cpp \
    sourceCode/various/uiThread.cpp \
    sourceCode/various/simThread.cpp \
    sourceCode/various/app.cpp \
    sourceCode/various/dynMaterialObject.cpp \
    sourceCode/various/easyLock.cpp \
    sourceCode/various/funcDebug.cpp \
    sourceCode/various/ghostObject.cpp \
    sourceCode/various/debugLogFile.cpp

SOURCES += sourceCode/undoRedo/undoBufferArrays.cpp \
    sourceCode/undoRedo/undoBuffer.cpp \
    sourceCode/undoRedo/undoBufferCameras.cpp \

SOURCES += sourceCode/rendering/rendering.cpp \
    sourceCode/rendering/cameraRendering.cpp \
    sourceCode/rendering/visionSensorRendering.cpp \
    sourceCode/rendering/forceSensorRendering.cpp \
    sourceCode/rendering/proximitySensorRendering.cpp \
    sourceCode/rendering/dummyRendering.cpp \
    sourceCode/rendering/lightRendering.cpp \
    sourceCode/rendering/pointCloudRendering.cpp \
    sourceCode/rendering/octreeRendering.cpp \
    sourceCode/rendering/jointRendering.cpp \
    sourceCode/rendering/graphRendering.cpp \
    sourceCode/rendering/millRendering.cpp \
    sourceCode/rendering/mirrorRendering.cpp \
    sourceCode/rendering/pathRendering.cpp \
    sourceCode/rendering/shapeRendering.cpp \
    sourceCode/rendering/ghostRendering.cpp \
    sourceCode/rendering/drawingObjectRendering.cpp \
    sourceCode/rendering/drawingObject2DRendering.cpp \
    sourceCode/rendering/ptCloudRendering_old.cpp \
    sourceCode/rendering/collisionContourRendering.cpp \
    sourceCode/rendering/distanceRendering.cpp \
    sourceCode/rendering/bannerRendering.cpp \
    sourceCode/rendering/thumbnailRendering.cpp \
    sourceCode/rendering/pathPlanningTaskRendering_old.cpp \
    sourceCode/rendering/motionPlanningTaskRendering_old.cpp \
    sourceCode/rendering/broadcastDataVisualRendering.cpp \
    sourceCode/rendering/dynamicsRendering.cpp \
    sourceCode/rendering/environmentRendering.cpp \
    sourceCode/rendering/pageRendering.cpp \
    sourceCode/rendering/viewRendering.cpp \

WITH_QT {
    SOURCES += sourceCode/various/simAndUiThreadSync.cpp \
    sourceCode/various/vrepQApp.cpp
}

SOURCES += sourceCode/libsAndPlugins/pluginContainer.cpp \
    sourceCode/libsAndPlugins/auxLib.cpp \

SOURCES += sourceCode/visual/visualParam.cpp \
    sourceCode/visual/thumbnail.cpp \

WITH_OPENGL {
    SOURCES += sourceCode/visual/oGL.cpp \
    sourceCode/visual/oglExt.cpp \
    sourceCode/visual/glShader.cpp \
    sourceCode/visual/glBufferObjects.cpp \
}

SOURCES += sourceCode/utils/threadPool.cpp \
    sourceCode/utils/ttUtil.cpp \
    sourceCode/utils/tt.cpp \
    sourceCode/utils/confReaderAndWriter.cpp \

SOURCES += sourceCode/customUserInterfaces/buttonBlock.cpp \
    sourceCode/customUserInterfaces/softButton.cpp \

WITH_SERIAL {
    SOURCES += sourceCode/communication/serialPort/serialPort.cpp \
    sourceCode/communication/serialPort/serial_mac_linux.cpp
}

WITH_GUI {
    SOURCES += sourceCode/gui/dialogs/qdlgsettings.cpp \
    sourceCode/gui/dialogs/qdlglayers.cpp \
    sourceCode/gui/dialogs/qdlgavirecorder.cpp \
    sourceCode/gui/dialogs/qdlgselection.cpp \
    sourceCode/gui/dialogs/qdlgabout.cpp \
    sourceCode/gui/dialogs/qdlgsimulation.cpp \
    sourceCode/gui/dialogs/qdlginteractiveik.cpp \
    sourceCode/gui/dialogs/qdlgcommonproperties.cpp \
    sourceCode/gui/dialogs/qdlgdetectableproperties.cpp \
    sourceCode/gui/dialogs/qdlgmodelproperties.cpp \
    sourceCode/gui/dialogs/qdlgassembly.cpp \
    sourceCode/gui/dialogs/qdlgslider.cpp \
    sourceCode/gui/dialogs/qdlgslider2.cpp \
    sourceCode/gui/dialogs/qdlginertiatensor.cpp \
    sourceCode/gui/dialogs/qdlgmodelthumbnailvisu.cpp \
    sourceCode/gui/dialogs/qdlgmodelthumbnail.cpp \
    sourceCode/gui/dialogs/qdlgenvironment.cpp \
    sourceCode/gui/dialogs/qdlgcolor.cpp \
    sourceCode/gui/dialogs/qdlgmaterial.cpp \
    sourceCode/gui/dialogs/qdlglightmaterial.cpp \
    sourceCode/gui/dialogs/qdlgcolorpulsation.cpp \
    sourceCode/gui/dialogs/qdlgfog.cpp \
    sourceCode/gui/dialogs/qdlgcollections.cpp \
    sourceCode/gui/dialogs/qdlgcollisions.cpp \
    sourceCode/gui/dialogs/qdlgdistances.cpp \
    sourceCode/gui/dialogs/qdlgentityvsentityselection.cpp \
    sourceCode/gui/dialogs/qdlgscripts.cpp \
    sourceCode/gui/dialogs/qdlginsertscript.cpp \
    sourceCode/gui/dialogs/qdlgdynamics.cpp \
    sourceCode/gui/dialogs/qdlgscaling.cpp \
    sourceCode/gui/dialogs/qdlgcameras.cpp \
    sourceCode/gui/dialogs/qdlglights.cpp \
    sourceCode/gui/dialogs/qdlgmirrors.cpp \
    sourceCode/gui/dialogs/qdlgdummies.cpp \
    sourceCode/gui/dialogs/qdlgoctrees.cpp \
    sourceCode/gui/dialogs/qdlgpointclouds.cpp \
    sourceCode/gui/dialogs/qdlgscriptparameters.cpp \
    sourceCode/gui/dialogs/qdlgforcesensors.cpp \
    sourceCode/gui/dialogs/qdlgprimitives.cpp \
    sourceCode/gui/dialogs/qdlgconvexdecomposition.cpp \
    sourceCode/gui/dialogs/qdlgconstraintsolver.cpp \
    sourceCode/gui/dialogs/qdlgvisionsensors.cpp \
    sourceCode/gui/dialogs/qdlgimagecolor.cpp \
    sourceCode/gui/dialogs/qdlgsimpleSearch.cpp \
    sourceCode/gui/dialogs/qdlgfilters.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_rotate.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_uniformImage.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_shift.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_coordExtraction.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_velodyne.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_pixelChange.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_scaleCols.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_resize.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_rectangularCut.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_keepRemoveCols.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_intensityScale.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_edge.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_correlation.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_colorSegmentation.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_circularCut.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_blob.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_binary.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_5x5.cpp \
    sourceCode/gui/dialogs/qdlgsimplefilter_3x3.cpp \
    sourceCode/gui/dialogs/qdlgshapes.cpp \
    sourceCode/gui/dialogs/qdlgmultishapeedition.cpp \
    sourceCode/gui/dialogs/qdlgtextures.cpp \
    sourceCode/gui/dialogs/qdlgtextureselection.cpp \
    sourceCode/gui/dialogs/qdlggeometry.cpp \
    sourceCode/gui/dialogs/qdlgshapedyn.cpp \
    sourceCode/gui/dialogs/qdlgdetectionvolume.cpp \
    sourceCode/gui/dialogs/qdlgproximitysensors.cpp \
    sourceCode/gui/dialogs/qdlgproxsensdetectionparam.cpp \
    sourceCode/gui/dialogs/qdlgmills.cpp \
    sourceCode/gui/dialogs/qdlgobjectdialogcontainer.cpp \
    sourceCode/gui/dialogs/qdlgshapeeditioncontainer.cpp \
    sourceCode/gui/dialogs/qdlgcalcdialogcontainer.cpp \
    sourceCode/gui/dialogs/qdlgheightfielddimension.cpp \
    sourceCode/gui/dialogs/qdlgstopscripts.cpp \
    sourceCode/gui/dialogs/qdlgimportoptions.cpp \
    sourceCode/gui/dialogs/qdlgtextureloadoptions.cpp \
    sourceCode/gui/dialogs/qdlgprogress.cpp \
    sourceCode/gui/dialogs/qdlgjoints.cpp \
    sourceCode/gui/dialogs/qdlgdependencyequation.cpp \
    sourceCode/gui/dialogs/qdlgjointdyn.cpp \
    sourceCode/gui/dialogs/qdlggraphs.cpp \
    sourceCode/gui/dialogs/qdlgdatastreamselection.cpp \
    sourceCode/gui/dialogs/qdlg2d3dgraphproperties.cpp \
    sourceCode/gui/dialogs/qdlgaddgraphcurve.cpp \
    sourceCode/gui/dialogs/qdlgik.cpp \
    sourceCode/gui/dialogs/qdlgikelements.cpp \
    sourceCode/gui/dialogs/qdlgikconditional.cpp \
    sourceCode/gui/dialogs/qdlgikavoidance.cpp \
    sourceCode/gui/dialogs/qdlgui.cpp \
    sourceCode/gui/dialogs/qdlguidialogcontainer.cpp \
    sourceCode/gui/dialogs/qdlguibuttons.cpp \
    sourceCode/gui/dialogs/qdlgnewui.cpp \
    sourceCode/gui/dialogs/qdlguirolledup.cpp \
    sourceCode/gui/dialogs/qdlgpathplanning.cpp \
    sourceCode/gui/dialogs/qdlgmotionplanning.cpp \
    sourceCode/gui/dialogs/qdlgpathplanningparams.cpp \
    sourceCode/gui/dialogs/qdlgmotionplanningjoints.cpp \
    sourceCode/gui/dialogs/qdlgworkspacemetric.cpp \
    sourceCode/gui/dialogs/qdlgpathplanningaddnew.cpp \
    sourceCode/gui/dialogs/qdlgpaths.cpp \
    sourceCode/gui/dialogs/qdlgpathshaping.cpp \
    sourceCode/gui/dialogs/qdlgmessageandcheckbox.cpp \
    sourceCode/gui/dialogs/qdlgopenglsettings.cpp \
    sourceCode/gui/dialogs/qdlgtriangleedit.cpp \
    sourceCode/gui/dialogs/qdlgvertexedit.cpp \
    sourceCode/gui/dialogs/qdlgedgeedit.cpp \
    sourceCode/gui/dialogs/qdlgpathedit.cpp \
    sourceCode/gui/dialogs/qdlgtranslation.cpp \
    sourceCode/gui/dialogs/qdlgrotation.cpp \
    sourceCode/gui/dialogs/qdlgtranslationrotationcont.cpp \
    sourceCode/gui/dialogs/qdlgobjectmanipsettings.cpp \

    SOURCES += sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qtbuttonpropertybrowser.cpp \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qteditorfactory.cpp \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qtgroupboxpropertybrowser.cpp \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qtpropertybrowser.cpp \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qtpropertybrowserutils.cpp \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qtpropertymanager.cpp \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qttreepropertybrowser.cpp \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserFiles/qtvariantproperty.cpp \

    SOURCES += sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserRelatedFiles/buttoneditfactory.cpp \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserRelatedFiles/buttoneditmanager.cpp \
    sourceCode/gui/qtPropertyBrowserRelated/qtPropertyBrowserRelatedFiles/pushbuttonedit.cpp \

    SOURCES += sourceCode/gui/qtPropertyBrowserRelated/propBrowser_engineProp_material.cpp \
    sourceCode/gui/qtPropertyBrowserRelated/propBrowser_engineProp_general.cpp \
    sourceCode/gui/qtPropertyBrowserRelated/propBrowser_engineProp_joint.cpp \

    SOURCES += sourceCode/gui/dialogs/other/editboxdelegate.cpp \
    sourceCode/gui/dialogs/other/toolDlgWrapper.cpp \
    sourceCode/gui/dialogs/other/dlgEx.cpp \
    sourceCode/gui/dialogs/other/dlgCont.cpp \
    sourceCode/gui/dialogs/other/qvrep_openglwidget.cpp \

    SOURCES += sourceCode/gui/platform/vMessageBox.cpp \
    sourceCode/gui/platform/vMenubar.cpp \
    sourceCode/gui/platform/vFileDialog.cpp \
    sourceCode/gui/platform/vDialog.cpp \

    SOURCES += sourceCode/gui/mainContainers/sceneContainers/scintillaEditorContainer.cpp \
    sourceCode/gui/mainContainers/sceneContainers/genericDialogContainer.cpp \
    sourceCode/gui/mainContainers/sceneContainers/oglSurface.cpp \

    SOURCES += sourceCode/gui/mainContainers/applicationContainers/scintillaConsoleContainer.cpp \
    sourceCode/gui/mainContainers/applicationContainers/globalGuiTextureContainer.cpp \

    SOURCES += sourceCode/gui/editModes/editModeContainer.cpp \
    sourceCode/gui/editModes/shapeEditMode.cpp \
    sourceCode/gui/editModes/multishapeEditMode.cpp \
    sourceCode/gui/editModes/pathEditMode.cpp \
    sourceCode/gui/editModes/uiEditMode.cpp \
    sourceCode/gui/editModes/edgeCont.cpp \
    sourceCode/gui/editModes/pathPointManipulation.cpp \

    SOURCES += sourceCode/gui/sceneHierarchy/hierarchy.cpp \
    sourceCode/gui/sceneHierarchy/hierarchyElement.cpp \
    sourceCode/gui/sceneHierarchy/sceneHierarchyWidget.cpp \

    SOURCES += sourceCode/gui/modelBrowser/modelFolderWidget.cpp \
    sourceCode/gui/modelBrowser/modelListWidget.cpp \
    sourceCode/gui/modelBrowser/modelListWidgetBase.cpp \

    SOURCES += sourceCode/gui/selectors/pageSelector.cpp \
    sourceCode/gui/selectors/viewSelector.cpp \
    sourceCode/gui/selectors/sceneSelector.cpp \

    SOURCES += sourceCode/gui/scintillaRelated/scintillaEditor.cpp \
     sourceCode/gui/scintillaRelated/scintillaDlg.cpp \
     sourceCode/gui/scintillaRelated/scintillaModalDlg.cpp \
     sourceCode/gui/scintillaRelated/scintillaUserModalDlg.cpp \
     sourceCode/gui/scintillaRelated/scintillaConsoleDlg.cpp \

    SOURCES += sourceCode/gui/menusAndSimilar/toolBarCommand.cpp \
    sourceCode/gui/menusAndSimilar/customMenuBarItemContainer.cpp \
    sourceCode/gui/menusAndSimilar/customMenuBarItem.cpp \
    sourceCode/gui/menusAndSimilar/mainWindow.cpp \
    sourceCode/gui/menusAndSimilar/helpMenu.cpp \

    SOURCES += sourceCode/gui/various/simRecorder.cpp \
    sourceCode/gui/various/genericDialog.cpp \

    SOURCES += sourceCode/gui/libs/auxLibVideo.cpp \
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}


