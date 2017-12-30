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

#include "3DObject.h"

enum { /* Bullet joint float params */
    simi_bullet_joint_stoperp=0,
    simi_bullet_joint_stopcfm,
    simi_bullet_joint_normalcfm
};
enum { /* Bullet joint int params */
    /* if you add something here, search for bji11032016 */
};
enum { /* Bullet joint bit params */
    /* if you add something here, search for bjb11032016 */
};

enum { /* Ode joint float params */
    simi_ode_joint_stoperp=0,
    simi_ode_joint_stopcfm,
    simi_ode_joint_bounce,
    simi_ode_joint_fudgefactor,
    simi_ode_joint_normalcfm
};
enum { /* Ode joint int params */
    /* if you add something here, search for oji11032016 */
};
enum { /* Ode joint bit params */
    /* if you add something here, search for ojb11032016 */
};

enum { /* Vortex joint float params */
    simi_vortex_joint_lowerlimitdamping=0,
    simi_vortex_joint_upperlimitdamping,
    simi_vortex_joint_lowerlimitstiffness,
    simi_vortex_joint_upperlimitstiffness,
    simi_vortex_joint_lowerlimitrestitution,
    simi_vortex_joint_upperlimitrestitution,
    simi_vortex_joint_lowerlimitmaxforce,
    simi_vortex_joint_upperlimitmaxforce,
    simi_vortex_joint_motorconstraintfrictioncoeff,
    simi_vortex_joint_motorconstraintfrictionmaxforce,
    simi_vortex_joint_motorconstraintfrictionloss,
    simi_vortex_joint_p0loss,
    simi_vortex_joint_p0stiffness,
    simi_vortex_joint_p0damping,
    simi_vortex_joint_p0frictioncoeff,
    simi_vortex_joint_p0frictionmaxforce,
    simi_vortex_joint_p0frictionloss,
    simi_vortex_joint_p1loss,
    simi_vortex_joint_p1stiffness,
    simi_vortex_joint_p1damping,
    simi_vortex_joint_p1frictioncoeff,
    simi_vortex_joint_p1frictionmaxforce,
    simi_vortex_joint_p1frictionloss,
    simi_vortex_joint_p2loss,
    simi_vortex_joint_p2stiffness,
    simi_vortex_joint_p2damping,
    simi_vortex_joint_p2frictioncoeff,
    simi_vortex_joint_p2frictionmaxforce,
    simi_vortex_joint_p2frictionloss,
    simi_vortex_joint_a0loss,
    simi_vortex_joint_a0stiffness,
    simi_vortex_joint_a0damping,
    simi_vortex_joint_a0frictioncoeff,
    simi_vortex_joint_a0frictionmaxforce,
    simi_vortex_joint_a0frictionloss,
    simi_vortex_joint_a1loss,
    simi_vortex_joint_a1stiffness,
    simi_vortex_joint_a1damping,
    simi_vortex_joint_a1frictioncoeff,
    simi_vortex_joint_a1frictionmaxforce,
    simi_vortex_joint_a1frictionloss,
    simi_vortex_joint_a2loss,
    simi_vortex_joint_a2stiffness,
    simi_vortex_joint_a2damping,
    simi_vortex_joint_a2frictioncoeff,
    simi_vortex_joint_a2frictionmaxforce,
    simi_vortex_joint_a2frictionloss,
    simi_vortex_joint_dependencyfactor,
    simi_vortex_joint_dependencyoffset
};
enum { /* Vortex joint int params */
    simi_vortex_joint_bitcoded=0,
    simi_vortex_joint_relaxationenabledbc,
    simi_vortex_joint_frictionenabledbc,
    simi_vortex_joint_frictionproportionalbc,
    simi_vortex_joint_objectid,
    simi_vortex_joint_dependentobjectid
};
enum { /* Vortex joint bit params */
    simi_vortex_joint_motorfrictionenabled=1,
    simi_vortex_joint_proportionalmotorfriction=2
};

enum { /* Newton joint float params */
    simi_newton_joint_dependencyfactor=0,
    simi_newton_joint_dependencyoffset
};
enum { /* Newton joint int params */
    simi_newton_joint_objectid=0,
    simi_newton_joint_dependentobjectid
};
enum { /* Newton joint bit params */
    /* if you add something here, search for njb11032016 */
};

class CJoint : public C3DObject  
{
public:
    CJoint(); // default, use only during serialization!
    CJoint(int jointType);
    virtual ~CJoint();

    // Following functions are inherited from 3DObject
    void display(CViewableBase* renderingObject,int displayAttrib);
    C3DObject* copyYourself();
    void scaleObject(float scalingFactor);
    void scaleObjectNonIsometrically(float x,float y,float z);
    void serialize(CSer& ar);
    void serializeWExtIk(CExtIkSer& ar);
    void announceCollectionWillBeErased(int groupID,bool copyBuffer);
    void announceCollisionWillBeErased(int collisionID,bool copyBuffer);
    void announceDistanceWillBeErased(int distanceID,bool copyBuffer);
    void announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer);
    void performIkLoadingMapping(std::vector<int>* map);
    void performGroupLoadingMapping(std::vector<int>* map);
    void performCollisionLoadingMapping(std::vector<int>* map);
    void performDistanceLoadingMapping(std::vector<int>* map);
    void performGcsLoadingMapping(std::vector<int>* map);
    void performTextureObjectLoadingMapping(std::vector<int>* map);
    void performDynMaterialObjectLoadingMapping(std::vector<int>* map);
    void bufferMainDisplayStateVariables();
    void bufferedMainDisplayStateVariablesToDisplay();
    void simulationAboutToStart();
    void simulationEnded();
    void initializeInitialValues(bool simulationIsRunning);
    bool getFullBoundingBox(C3Vector& minV,C3Vector& maxV) const;
    bool getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV) const;
    bool getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices) const;

    std::string getObjectTypeInfo() const;
    std::string getObjectTypeInfoExtended() const;
    bool isPotentiallyCollidable() const;
    bool isPotentiallyMeasurable() const;
    bool isPotentiallyDetectable() const;
    bool isPotentiallyRenderable() const;
    bool isPotentiallyCuttable() const;

    bool announceObjectWillBeErased(int objID,bool copyBuffer);
    void announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer);
    void performObjectLoadingMapping(std::vector<int>* map);

    void setPositionIntervalMin(float min);
    void setPositionIntervalRange(float range);

    void setScrewPitch(float pitch);

    void setPositionIsCyclic(bool isCyclic);
    void measureJointVelocity(float dt); // should be called just after the main script was called!!!
    float getMeasuredJointVelocity();

    // Dynamic routines:
    void setEnableDynamicMotor(bool e);
    bool getEnableDynamicMotor();
    void setDynamicMotorTargetVelocity(float v);
    float getDynamicMotorTargetVelocity();
    void setDynamicMotorLockModeWhenInVelocityControl(bool e);
    bool getDynamicMotorLockModeWhenInVelocityControl();
    void setDynamicMotorUpperLimitVelocity(float v);
    float getDynamicMotorUpperLimitVelocity();
    void setDynamicMotorMaximumForce(float f);
    float getDynamicMotorMaximumForce();
    void setDynamicSecondPartIsValid(bool v);
    bool getDynamicSecondPartIsValid() const;
    void setDynamicSecondPartLocalTransform(const C7Vector& tr);
    C7Vector getDynamicSecondPartLocalTransform() const;
    void getDynamicJointErrors(float& linear,float& angular);
    void getDynamicJointErrorsFull(C3Vector& linear,C3Vector& angular);
    void setEnableDynamicMotorControlLoop(bool p);
    bool getEnableDynamicMotorControlLoop();

    void setEnableDynamicMotorCustomControl(bool c,const char* scriptContent,const std::vector<int>* foldingInfo);
    bool getEnableDynamicMotorCustomControl();

    void handleDynJointControl(bool init,int loopCnt,int totalLoops,float currentPos,float effort,float dynStepSize,float errorV,float& velocity,float& forceTorque);

    void setEnableTorqueModulation(bool p);
    bool getEnableTorqueModulation();

    bool getHybridFunctionality();

    void setDynamicMotorPositionControlParameters(float p_param,float i_param,float d_param);
    void getDynamicMotorPositionControlParameters(float& p_param,float& i_param,float& d_param);

    void setDynamicMotorSpringControlParameters(float k_param,float c_param);
    void getDynamicMotorSpringControlParameters(float& k_param,float& c_param);


    void setDynamicMotorReflectedPosition_useOnlyFromDynamicPart(float rfp);
    void setDynamicMotorPositionControlTargetPosition(float pos);
    float getDynamicMotorPositionControlTargetPosition();
    void setHybridFunctionality(bool h);

    void addCumulativeForceOrTorque(float forceOrTorque,int countForAverage);
    bool getDynamicForceOrTorque(float& forceOrTorque,bool dynamicStepValue);
    void setForceOrTorqueNotValid();

    void setLength(float l);
    float getLength();
    void setDiameter(float d);
    float getDiameter();

    float getPosition_forDisplay(bool guiIsRendering);
    C4Vector getSphericalTransformation_forDisplay(bool guiIsRendering) const;



    void setDependencyJointID(int depJointID);
    void setDependencyJointCoeff(float coeff);
    void setDependencyJointFact(float fact);

    float getEngineFloatParam(int what,bool* ok);
    int getEngineIntParam(int what,bool* ok);
    bool getEngineBoolParam(int what,bool* ok);
    bool setEngineFloatParam(int what,float v);
    bool setEngineIntParam(int what,int v);
    bool setEngineBoolParam(int what,bool v);

    void getBulletFloatParams(std::vector<float>& p);
    void setBulletFloatParams(const std::vector<float>& p);
    void getBulletIntParams(std::vector<int>& p);
    void setBulletIntParams(const std::vector<int>& p);

    void getOdeFloatParams(std::vector<float>& p);
    void setOdeFloatParams(const std::vector<float>& p);
    void getOdeIntParams(std::vector<int>& p);
    void setOdeIntParams(const std::vector<int>& p);

    int getVortexDependentJointId(); // helper
    void getVortexFloatParams(std::vector<float>& p);
    void setVortexFloatParams(const std::vector<float>& p);
    void getVortexIntParams(std::vector<int>& p);
    void setVortexIntParams(const std::vector<int>& p);

    int getNewtonDependentJointId(); // helper
    void getNewtonFloatParams(std::vector<float>& p);
    void setNewtonFloatParams(const std::vector<float>& p);
    void getNewtonIntParams(std::vector<int>& p);
    void setNewtonIntParams(const std::vector<int>& p);


    void copyEnginePropertiesTo(CJoint* target);


    // DEPRECATED:
    void setExplicitHandling_DEPRECATED(bool explicitHandl);
    bool getExplicitHandling_DEPRECATED();
    void resetJoint_DEPRECATED();
    void handleJoint_DEPRECATED(float deltaTime);
    void setUnlimitedAcceleration_DEPRECATED(bool unlimited);
    bool getUnlimitedAcceleration_DEPRECATED();
    void setInvertTargetVelocityAtLimits_DEPRECATED(bool invert);
    bool getInvertTargetVelocityAtLimits_DEPRECATED();
    void setMaxAcceleration_DEPRECATED(float maxAccel);
    float getMaxAcceleration_DEPRECATED();
    void setVelocity_DEPRECATED(float vel);
    float getVelocity_DEPRECATED();
    void setTargetVelocity_DEPRECATED(float vel);
    float getTargetVelocity_DEPRECATED();
    int getJointCallbackCallOrder_backwardCompatibility();

    void commonInit();
    float getPosition(bool useTempValues=false);
    void setPosition(float parameter,bool useTempValues=false);
    float getPosition_ratio();
    void setPosition_ratio(float posRatio);
    void initializeParametersForIK(float angularJointLimitationThreshold);
    int getDoFs();
    void getLocalTransformationExPart1(C7Vector& mTr,int index,bool useTempValues=false);
    float getTempParameterEx(int index);
    void setTempParameterEx(float parameter,int index);
    void applyTempParametersEx();
    int getTempSphericalJointLimitations();
    float getScrewPitch() const;
    void setSphericalTransformation(const C4Vector& tr);
    C4Vector getSphericalTransformation() const;
    int getJointType();
    float getPositionIntervalMin();
    float getPositionIntervalRange();
    bool getPositionIsCyclic();
    float getIKWeight();
    void setIKWeight(float newWeight);
    void setMaxStepSize(float stepS);
    float getMaxStepSize();

    void _rectifyDependentJoints(bool useTempValues);

    void setJointMode(int theMode,bool correctDynMotorTargetPositions=true);
    int getJointMode();
    int getDependencyJointID();
    float getDependencyJointCoeff();
    float getDependencyJointFact();
    CVisualParam* getColor(bool getPart2);

    std::vector<CJoint*> directDependentJoints;

protected:
    C4Vector _sphericalTransformation_buffered;
    float _jointPosition_buffered;
    C4Vector _sphericalTransformation_forDisplay;
    float _jointPosition_forDisplay;

    // Variables needed for the reset procedure of the joint. Do not serialize
    CVisualParam colorPart1;
    CVisualParam colorPart2;
    bool _initialValuesInitialized;
    float _initialPosition;
    C4Vector _initialSphericalJointTransformation;
    float _initialTargetPosition;
    bool _initialDynamicMotorEnabled;
    float _initialDynamicMotorTargetVelocity;
    bool _initialDynamicMotorLockModeWhenInVelocityControl;
    float _initialDynamicMotorUpperLimitVelocity;
    float _initialDynamicMotorMaximumForce;
    bool _dynamicSecondPartIsValid;
    C7Vector _dynamicSecondPartLocalTransform;
    bool _initialDynamicMotorControlLoopEnabled;
    float _initialDynamicMotorPositionControl_P;
    float _initialDynamicMotorPositionControl_I;
    float _initialDynamicMotorPositionControl_D;
    float _initialDynamicMotorSpringControl_K;
    float _initialDynamicMotorSpringControl_C;
    int _initialJointMode;
    bool _initialHybridOperation;
    int _initialJointCallbackCallOrder;

    float _measuredJointVelocity_velocityMeasurement;
    float _previousJointPosition_velocityMeasurement;
    bool _previousJointPositionIsValid;

    int _jointType;
    C4Vector _sphericalTransformation;
    bool _positionIsCyclic;
    float _screwPitch;
    float _jointMinPosition;
    float _jointPositionRange;
    float _jointPosition;
    float _maxStepSize;
    float _ikWeight;
    int _jointMode;
    int _dependencyJointID;
    float _dependencyJointCoeff;
    float _dependencyJointFact;

    // Joint visual attributes:
    float _length;
    float _diameter;

    // Dynamic values:
    bool _dynamicMotorEnabled;
    float _dynamicMotorTargetVelocity;
    bool _dynamicLockModeWhenInVelocityControl;
    float _dynamicMotorUpperLimitVelocity;
    float _dynamicMotorMaximumForce;
    bool _dynamicMotorControlLoopEnabled;
    bool _dynamicMotorPositionControl_torqueModulation;
    float _dynamicMotorPositionControl_targetPosition; 
    float _dynamicMotorPositionControl_P;
    float _dynamicMotorPositionControl_I;
    float _dynamicMotorPositionControl_D;
    float _dynamicMotorSpringControl_K;
    float _dynamicMotorSpringControl_C;
    bool _dynamicMotorCustomControl;
    bool _jointHasHybridFunctionality;

    float _dynamicMotorPIDCumulativeErrorForIntegralParameter;
    float _dynamicMotorPIDLastErrorForDerivativeParameter;

    std::vector<float> _bulletFloatParams;
    std::vector<int> _bulletIntParams;

    std::vector<float> _odeFloatParams;
    std::vector<int> _odeIntParams;

    std::vector<float> _vortexFloatParams;
    std::vector<int> _vortexIntParams;

    std::vector<float> _newtonFloatParams;
    std::vector<int> _newtonIntParams;
    // ------------------

    // Dynamic values, not serialized:
    float _cumulatedForceOrTorque;
    float _cumulativeForceOrTorqueTmp;
    bool _averageForceOrTorqueValid;

    // Following is the force/torque acquired during a single dyn. calculation step:
    float _lastForceOrTorque_dynStep;
    bool _lastForceOrTorqueValid_dynStep;

    // DEPRECATED:
    float _jointPositionForMotionHandling_DEPRECATED;
    float _velocity_DEPRECATED;
    float _targetVelocity_DEPRECATED;
    bool _explicitHandling_DEPRECATED;
    bool _unlimitedAcceleration_DEPRECATED;
    bool _invertTargetVelocityAtLimits_DEPRECATED;
    float _maxAcceleration_DEPRECATED;
    int _jointCallbackCallOrder_backwardCompatibility;
    float _initialVelocity_DEPRECATED;
    float _initialTargetVelocity_DEPRECATED;
    bool _initialExplicitHandling_DEPRECATED;

    // Temporary values used when doing IK:
    float _jointPosition_tempForIK;
    float _sphericalTransformation_euler1TempForIK;
    float _sphericalTransformation_euler2TempForIK;
    float _sphericalTransformation_euler3TempForIK;
    int _sphericalTransformation_eulerLockTempForIK; // bit-coded, bit0--> _sphericalTransformation_euler1TempForIK, bit1--> _sphericalTransformation_euler2TempForIK, etc.
};
