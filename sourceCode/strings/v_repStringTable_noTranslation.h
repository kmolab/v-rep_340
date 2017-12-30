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

#define IDSNOTR_APPLICATION_ERROR "Application error"

#define IDSNOTR_STRANGE_ERROR1 "Strange error! (in CGraphDlg::OnItemchangedList3d)"
#define IDSNOTR_STRANGE_ERROR2 "Strange error! (in CGraphDlg::OnItemchangedList2d)"
#define IDSNOTR_STRANGE_ERROR3 "Strange error! (in CGraphDlg::refreshTPart())"
#define IDSNOTR_STRANGE_ERROR4 "Error in CParticleDyn destruction!"
#define IDSNOTR_STRANGE_ERROR5 "Strange error! (in CObjCont::announceObjectWillBeErased)"
#define IDSNOTR_STRANGE_ERROR6 "Strange error in '_getBezierPointsForVirtualDistance'"
#define IDSNOTR_STRANGE_ERROR7 "Strange error! (in CCopyBuffer::_announceObjectWillBeErased)"
#define IDSNOTR_STRANGE_ERROR8 "Strange error in startTextureDisplay routine!"
#define IDSNOTR_STRANGE_ERROR9 "Strange error in startTextureDisplay routine!"
#define IDSNOTR_NO_NUMBER_ERROR1 "Value is not a number (in robustAsin)."
#define IDSNOTR_NO_NUMBER_ERROR2 "Value is not a number (in robustAcos)."
#define IDSNOTR_NO_NUMBER_ERROR3 "Value is not a number (in robustFmod)."
#define IDSNOTR_NO_NUMBER_ERROR4 "Value is not a number (in robustmod)."

#define IDSNOTR_FUNCTION_CALL_ERROR_C_API_GUI "C API call error (GUI thread): "
#define IDSNOTR_FUNCTION_CALL_ERROR_C_API_NONGUI "C API call error (main sim. thread): "
#define IDSNOTR_FUNCTION_CALL_ERROR_LUA_API "Lua API call error: "
#define IDSNOTR_FUNCTION_CALL_WARNING_LUA_API "Lua API call warning: "



#define IDSNOTR_FRAME_BUFFER_OBJECT "Frame buffer object"
#define IDS__NONE_              " - None -"
#define IDS_PAGE               "Page"
#define IDS_LEAVE_ELEMENT_EDIT_MODE "Leave OpenGl-based custom UI edit mode"
#define IDS_ENTER_ELEMENT_EDIT_MODE "Enter OpenGl-based custom UI edit mode"
#define IDS_OBJECTS_COMPOSING_SELECTED_COLLECTION "Elements composing selected collection"
#define IDS_COLLECTIONS         "Collections"
#define IDS_COLLISION_PARTNER   "Collision partner"
#define IDS_DISTANCE_PARTNER    "Distance partner"
#define IDS_SHAPE_PROPERTIES_MENU_ITEM "Shape Properties"
#define IDS_JOINT_PROPERTIES_MENU_ITEM "Joint Properties"
#define IDS_CAMERA_PROPERTIES_MENU_ITEM "Camera Properties"
#define IDS_DUMMY_PROPERTIES_MENU_ITEM "Dummy Properties"
#define IDS_GRAPH_PROPERTIES_MENU_ITEM "Graph Properties"
#define IDS_PROXSENSOR_PROPERTIES_MENU_ITEM "Proximity Sensor Properties"
#define IDS_LIGHT_PROPERTIES_MENU_ITEM "Light Properties"
#define IDS_MIRROR_PROPERTIES_MENU_ITEM "Mirror Properties"
#define IDS_PATH_PROPERTIES_MENU_ITEM "Path Properties"
#define IDS_COLLISIONS_MENU_ITEM "Collisions"
#define IDS_DISTANCES_MENU_ITEM "Distances"
#define IDS_INVERSE_KINEMATICS_MENU_ITEM "Inverse Kinematics"
#define IDS_GEOMETRIC_CONSTRAINT_SOLVER_MENU_ITEM "Geometric Constraint Solver"
#define IDS_PATH_PLANNING_MENU_ITEM "Path Planning"
#define IDS_DYNAMICS_MENU_ITEM  "Dynamics"
#define IDS_OBJECTS_ASS_WITH_SELECT_DATA_STREAM_TYPE "Object to associate with selected data stream type"
#define IDS_ERROR__             "Error!]"
#define IDS_DATA_STREAMS_TO_RECORD "Data streams to record"
#define IDS_XYZ_CURVES          "XYZ curves"
#define IDS_XY_CURVES           "XY curves"
#define IDS_SELECT_DATA_STREAM_TYPE_TO_RECORD "- Select data stream type to record -"
#define IDS_IK_GROUPS           "IK groups"
#define IDS___PERFORM_ALWAYS__  "- Perform always -"
#define IDS_IK_ELEMENT_TOOLTIPS "IK element tips"
#define IDS___SELECT_A_TOOLTIP__ "- Select a tip -"
#define IDS___WORLD__           "- World -"
#define IDS_SCRIPTS             "Scripts"
#define IDS_TOOLBAR_TOOLTIP_PAGE1 "Page 1"
#define IDS_TOOLBAR_TOOLTIP_PAGE2 "Page 2"
#define IDS_TOOLBAR_TOOLTIP_PAGE3 "Page 3"
#define IDS_TOOLBAR_TOOLTIP_PAGE4 "Page 4"
#define IDS_TOOLBAR_TOOLTIP_PAGE5 "Page 5"
#define IDS_TOOLBAR_TOOLTIP_PAGE6 "Page 6"
#define IDS_TOOLBAR_TOOLTIP_PAGE7 "Page 7"
#define IDS_TOOLBAR_TOOLTIP_PAGE8 "Page 8"
#define IDS_TOOLBAR_TOOLTIP_INSTANCE1 "Scene 1"
#define IDS_TOOLBAR_TOOLTIP_INSTANCE2 "Scene 2"
#define IDS_TOOLBAR_TOOLTIP_INSTANCE3 "Scene 3"
#define IDS_TOOLBAR_TOOLTIP_INSTANCE4 "Scene 4"
#define IDS_TOOLBAR_TOOLTIP_COMMON_PROPERTIES "Object common properties"
#define IDS_TOOLBAR_TOOLTIP_COORDINATES "Coordinates/transformations"
#define IDS_TOOLBAR_TOOLTIP_COLLISIONS "Collisions"
#define IDS_TOOLBAR_TOOLTIP_DISTANCES "Distances"
#define IDS_TOOLBAR_TOOLTIP_IK  "Inverse kinematics"
#define IDS_TOOLBAR_TOOLTIP_GEOMETRIC_CONSTRAINT_SOLVER "Geometric constraint solver"
#define IDS_TOOLBAR_TOOLTIP_PATH_PLANNING "Path planning"
#define IDS_TOOLBAR_TOOLTIP_ENVIRONMENT "Environment"
#define IDS_TOOLBAR_TOOLTIP_DYNAMICS "Dynamics"
#define IDS_TOOLBAR_TOOLTIP_OBJECT_BROWSER "Model browser"
#define IDS_TOOLBAR_TOOLTIP_SCRIPTS "Scripts"
#define IDS_TOOLBAR_TOOLTIP_2DELEMENTS "OpenGl-based custom UIs"
#define IDS_TOOLBAR_TOOLTIP_COLLECTIONS "Collections"
#define IDS_TOOLBAR_TOOLTIP_SIMULATION_SETTINGS "Simulation settings"
#define IDS_TOOLBAR_TOOLTIP_SCENE_HIERARCHY "Scene hierarchy"
#define IDS_TOOLBAR_TOOLTIP_AVI_RECORDER "Video recorder"
#define IDS_TOOLBAR_TOOLTIP_USER_SETTINGS "User settings"
#define IDS_TOOLBAR_TOOLTIP_LAYERS "Layers"
#define IDS_TOOLBAR_TOOLTIP_SELECTION "Selection"
#define IDS_TRANSFORMATIONS_MENU_ITEM "Coordinates/Transformations"
#define IDS_GEOMETRIC_CONSTRAINT_SOLVER_OBJECTS "Geometric constraint solver objects"
#define IDS_VISION_SENSOR_PROPERTIES_MENU_ITEM "Vision Sensor Properties"
#define IDS_FORCE_SENSOR_PROPERTIES_MENU_ITEM "Force Sensor Properties"
#define IDS_ASSOCIATED_DUMMY_OBJECT "Associated dummy object"
#define IDS_ASSOCIATED_IK_GROUP "Associated IK group"
#define IDS_TOOLBAR_TOOLTIP_UNDO "Undo"
#define IDS_TOOLBAR_TOOLTIP_REDO "Redo"
#define IDS_MILL_PROPERTIES_MENU_ITEM "Mill Properties"
#define IDS_FILTER_COMPOSITION  "Filter composition"
#define IDS___SAME_AS_BASE__    "- Same as base -"
#define IDS_SET_ASSOCIATION__LAST_SELECTION_ "Set association (last selection)"
#define IDS_REMOVE_ASSOCIATION  "Remove association"
#define IDS_TASKS               "Tasks"
#define IDS____CHAIN_           "] (chain)"
#define IDS_PASSIVE_MODE        "Passive mode"
#define IDS_IK_MODE             "IK mode"
#define IDS_DEPENDENT_IK_MODE   "Dependent IK mode"
#define IDS_DEPENDENT_MODE      "Dependent mode"
#define IDS_MOTION_MODE         "Motion mode"
#define IDS_TORQUE_FORCE_MODE   "Torque/force mode"
#define IDS_TEXTURES            "Textures"
#define IDS___COLLECTION_       " [Collection]"
#define IDS___SHAPE_            " [Shape]"
#define IDS___DUMMY_            " [Dummy]"
