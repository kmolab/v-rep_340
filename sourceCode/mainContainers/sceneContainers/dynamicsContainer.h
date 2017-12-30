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
#include "3Vector.h"
#include "ser.h"
#include "visualParam.h"

enum { /* Bullet global float params */
    simi_bullet_global_stepsize=0,
    simi_bullet_global_internalscalingfactor,
    simi_bullet_global_collisionmarginfactor
};
enum { /* Bullet global int params */
    simi_bullet_global_constraintsolvingiterations=0,
    simi_bullet_global_bitcoded,
    simi_bullet_global_constraintsolvertype
};
enum { /* Bullet global bit params */
    simi_bullet_global_fullinternalscaling=1
};

enum { /* Ode global float params */
    simi_ode_global_stepsize=0,
    simi_ode_global_internalscalingfactor,
    simi_ode_global_cfm,
    simi_ode_global_erp
};
enum { /* Ode global int params */
    simi_ode_global_constraintsolvingiterations=0,
    simi_ode_global_bitcoded,
    simi_ode_global_randomseed
};
enum { /* Ode global bit params */
    simi_ode_global_fullinternalscaling=1,
    simi_ode_global_quickstep=2
};

enum { /* Vortex global float params */
    simi_vortex_global_stepsize=0,
    simi_vortex_global_internalscalingfactor,
    simi_vortex_global_contacttolerance,
    simi_vortex_global_constraintlinearcompliance,
    simi_vortex_global_constraintlineardamping,
    simi_vortex_global_constraintlinearkineticloss,
    simi_vortex_global_constraintangularcompliance,
    simi_vortex_global_constraintangulardamping,
    simi_vortex_global_constraintangularkineticloss
};
enum { /* Vortex global int params */
    simi_vortex_global_bitcoded=0
};
enum { /* Vortex global bit params */
    simi_vortex_global_autosleep=1,
    simi_vortex_global_multithreading=2
};

enum { /* Newton global float params */
    simi_newton_global_stepsize=0,
    simi_newton_global_contactmergetolerance
};
enum { /* Newton global int params */
    simi_newton_global_constraintsolvingiterations=0,
    simi_newton_global_bitcoded
};
enum { /* Newton global bit params */
    simi_newton_global_multithreading=1,
    simi_newton_global_exactsolver=2,
    simi_newton_global_highjointaccuracy=4,
};

class CDynamicsContainer : public CMainCont 
{
public:
    CDynamicsContainer();
    virtual ~CDynamicsContainer();
    void serialize(CSer& ar);
    void simulationAboutToStart();
    void simulationEnded();
    void renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib);
    void renderYour3DStuff_overlay(CViewableBase* renderingObject,int displayAttrib);

    void handleDynamics(float dt);
    bool getContactForce(int dynamicPass,int objectHandle,int index,int objectHandles[2],float contactInfo[6]);

    void reportDynamicWorldConfiguration();

    void addWorldIfNotThere();
    void removeWorld();
    bool isWorldThere();

    void markForWarningDisplay_pureSpheroidNotSupported();
    void markForWarningDisplay_pureConeNotSupported();
    void markForWarningDisplay_containsNonPureNonConvexShapes();
    void markForWarningDisplay_containsStaticShapesOnDynamicConstruction();
    void markForWarningDisplay_pureHollowShapeNotSupported();
    void markForWarningDisplay_physicsEngineNotSupported();
    void markForWarningDisplay_vortexPluginIsDemo();
    void markForWarningDisplay_newtonDynamicRandomMeshNotSupported();

    void displayWarningsIfNeeded();
    bool displayNonDefaultParameterWarningRequired();
    bool displayNonPureNonConvexShapeWarningRequired();
    bool displayStaticShapeOnDynamicConstructionWarningRequired();
    bool displayVortexPluginIsDemoRequired();

    void setDynamicEngineType(int t,int version);
    int getDynamicEngineType(int* version);
    bool setCurrentDynamicStepSize(float s); // will modify the current engine's step size if setting is custom
    float getCurrentDynamicStepSize();
    bool setCurrentIterationCount(int c); // will modify the current engine's it. count if setting is custom
    int getCurrentIterationCount();
    void setDynamicsEnabled(bool e);
    bool getDynamicsEnabled();
    void setGravity(const C3Vector& gr);
    C3Vector getGravity();

    void setUseDynamicDefaultCalculationParameters(int defType); // 0=default very precise, 1=default precise, 2=default fast, 3=default very fast, 4=custom
    int getUseDynamicDefaultCalculationParameters();

    float getPositionScalingFactorDyn();
    float getLinearVelocityScalingFactorDyn();
    float getMassScalingFactorDyn();
    float getMasslessInertiaScalingFactorDyn();
    float getForceScalingFactorDyn();
    float getTorqueScalingFactorDyn();
    float getGravityScalingFactorDyn();


    void setDisplayContactPoints(bool d);
    bool getDisplayContactPoints();

    void setTempDisabledWarnings(int mask);
    int getTempDisabledWarnings();

    bool getCurrentlyInDynamicsCalculations();

    float getEngineFloatParam(int what,bool* ok);
    int getEngineIntParam(int what,bool* ok);
    bool getEngineBoolParam(int what,bool* ok);
    bool setEngineFloatParam(int what,float v,bool setDirect);
    bool setEngineIntParam(int what,int v,bool setDirect);
    bool setEngineBoolParam(int what,bool v,bool setDirect);

    void getBulletFloatParams(std::vector<float>& p);
    void setBulletFloatParams(const std::vector<float>& p,bool setDirect);
    void getBulletIntParams(std::vector<int>& p);
    void setBulletIntParams(const std::vector<int>& p,bool setDirect);
    void getBulletDefaultFloatParams(std::vector<float>& p,int defType);
    void getBulletDefaultIntParams(std::vector<int>& p,int defType);

    void getOdeFloatParams(std::vector<float>& p);
    void setOdeFloatParams(const std::vector<float>& p,bool setDirect);
    void getOdeIntParams(std::vector<int>& p);
    void setOdeIntParams(const std::vector<int>& p,bool setDirect);
    void getOdeDefaultFloatParams(std::vector<float>& p,int defType);
    void getOdeDefaultIntParams(std::vector<int>& p,int defType);

    void getVortexFloatParams(std::vector<float>& p);
    void setVortexFloatParams(const std::vector<float>& p,bool setDirect);
    void getVortexIntParams(std::vector<int>& p);
    void setVortexIntParams(const std::vector<int>& p,bool setDirect);
    void getVortexDefaultFloatParams(std::vector<float>& p,int defType);
    void getVortexDefaultIntParams(std::vector<int>& p,int defType);

    void getNewtonFloatParams(std::vector<float>& p);
    void setNewtonFloatParams(const std::vector<float>& p,bool setDirect);
    void getNewtonIntParams(std::vector<int>& p);
    void setNewtonIntParams(const std::vector<int>& p,bool setDirect);
    void getNewtonDefaultFloatParams(std::vector<float>& p,int defType);
    void getNewtonDefaultIntParams(std::vector<int>& p,int defType);

    CVisualParam contactPointColor;

protected:
    void _resetWarningFlags();

    unsigned char _pureSpheroidNotSupportedMark;
    unsigned char _pureConeNotSupportedMark;
    unsigned char _pureHollowShapeNotSupportedMark;
    unsigned char _physicsEngineNotSupportedWarning;
    unsigned char _newtonDynamicRandomMeshNotSupportedMark;

    unsigned char _containsNonPureNonConvexShapes;
    unsigned char _containsStaticShapesOnDynamicConstruction;
    unsigned char _nonDefaultEngineSettingsWarning;
    unsigned char _vortexPluginIsDemoWarning;

    int _tempDisabledWarnings; // bits in the same order as above messages

    bool _currentlyInDynamicsCalculations;

    // To serialize:
    bool _dynamicsEnabled;
    int _dynamicEngineToUse;
    int _dynamicEngineVersionToUse;
    C3Vector _gravity;
    int _dynamicDefaultTypeCalculationParameters;
    bool _displayContactPoints;


    std::vector<float> _bulletFloatParams;
    std::vector<int> _bulletIntParams;

    std::vector<float> _odeFloatParams;
    std::vector<int> _odeIntParams;

    std::vector<float> _vortexFloatParams;
    std::vector<int> _vortexIntParams;

    std::vector<float> _newtonFloatParams;
    std::vector<int> _newtonIntParams;
};
