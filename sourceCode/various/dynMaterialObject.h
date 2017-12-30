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
#include "ser.h"

enum { /* Bullet body float params */
    simi_bullet_body_restitution=0,
    simi_bullet_body_oldfriction,
    simi_bullet_body_friction,
    simi_bullet_body_lineardamping,
    simi_bullet_body_angulardamping,
    simi_bullet_body_nondefaultcollisionmargingfactor,
    simi_bullet_body_nondefaultcollisionmargingfactorconvex
};
enum { /* Bullet body int params */
    simi_bullet_body_bitcoded=0
};
enum { /* Bullet body bit params */
    simi_bullet_body_sticky=1,
    simi_bullet_body_usenondefaultcollisionmargin=2,
    simi_bullet_body_usenondefaultcollisionmarginconvex=4,
    simi_bullet_body_autoshrinkconvex=8
};

enum { /* Ode body float params */
    simi_ode_body_friction=0,
    simi_ode_body_softerp,
    simi_ode_body_softcfm,
    simi_ode_body_lineardamping,
    simi_ode_body_angulardamping
};
enum { /* Ode body int params */
    simi_ode_body_maxcontacts=0
};
enum { /* Ode body bit params */
    /* if you add something here, search for obb11032016 */
};

enum { /* Vortex body float params */
    simi_vortex_body_primlinearaxisfriction=0,
    simi_vortex_body_seclinearaxisfriction,
    simi_vortex_body_primangularaxisfriction,
    simi_vortex_body_secangularaxisfriction,
    simi_vortex_body_normalangularaxisfriction,
    simi_vortex_body_primlinearaxisstaticfrictionscale,
    simi_vortex_body_seclinearaxisstaticfrictionscale,
    simi_vortex_body_primangularaxisstaticfrictionscale,
    simi_vortex_body_secangularaxisstaticfrictionscale,
    simi_vortex_body_normalangularaxisstaticfrictionscale,
    simi_vortex_body_compliance,
    simi_vortex_body_damping,
    simi_vortex_body_restitution,
    simi_vortex_body_restitutionthreshold,
    simi_vortex_body_adhesiveforce,
    simi_vortex_body_linearvelocitydamping,
    simi_vortex_body_angularvelocitydamping,
    simi_vortex_body_primlinearaxisslide,
    simi_vortex_body_seclinearaxisslide,
    simi_vortex_body_primangularaxisslide,
    simi_vortex_body_secangularaxisslide,
    simi_vortex_body_normalangularaxisslide,
    simi_vortex_body_primlinearaxisslip,
    simi_vortex_body_seclinearaxisslip,
    simi_vortex_body_primangularaxisslip,
    simi_vortex_body_secangularaxisslip,
    simi_vortex_body_normalangularaxisslip,
    simi_vortex_body_autosleeplinearspeedthreshold,
    simi_vortex_body_autosleeplinearaccelthreshold,
    simi_vortex_body_autosleepangularspeedthreshold,
    simi_vortex_body_autosleepangularaccelthreshold,
    simi_vortex_body_skinthickness,
    simi_vortex_body_autoangulardampingtensionratio,
    simi_vortex_body_primaxisvectorx,
    simi_vortex_body_primaxisvectory,
    simi_vortex_body_primaxisvectorz
};
enum { /* Vortex body int params */
    simi_vortex_body_primlinearaxisfrictionmodel=0,
    simi_vortex_body_seclinearaxisfrictionmodel,
    simi_vortex_body_primangulararaxisfrictionmodel,
    simi_vortex_body_secmangulararaxisfrictionmodel,
    simi_vortex_body_normalmangulararaxisfrictionmodel,
    simi_vortex_body_bitcoded,
    simi_vortex_body_autosleepsteplivethreshold,
    simi_vortex_body_materialuniqueid
};
enum { /* Vortex body bit params */
    simi_vortex_body_pureshapesasconvex=1,
    simi_vortex_body_convexshapesasrandom=2,
    simi_vortex_body_randomshapesasterrain=4,
    simi_vortex_body_fastmoving=8,
    simi_vortex_body_autoslip=16,
    simi_vortex_body_seclinaxissameasprimlinaxis=32,
    simi_vortex_body_secangaxissameasprimangaxis=64,
    simi_vortex_body_normangaxissameasprimangaxis=128,
    simi_vortex_body_autoangulardamping=256
};

enum { /* Newton body float params */
    simi_newton_body_staticfriction=0,
    simi_newton_body_kineticfriction,
    simi_newton_body_restitution,
    simi_newton_body_lineardrag,
    simi_newton_body_angulardrag
};
enum { /* Newton body int params */
    simi_newton_body_bitcoded=0
};
enum { /* Newton body bit params */
    simi_newton_body_fastmoving=1
};

class CDynMaterialObject
{
public:
    CDynMaterialObject();
    virtual ~CDynMaterialObject();

    void setObjectID(int newID);
    int getObjectID();
    void setObjectName(const char* newName);
    void generateDefaultMaterial(int defMatId);
    static std::string getDefaultMaterialName(int defMatId);

    std::string getObjectName();
    CDynMaterialObject* copyYourself();
    void serialize(CSer& ar);

    static std::string getIndividualName();


    // Engine properties
    // ---------------------
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

    void getVortexFloatParams(std::vector<float>& p);
    void setVortexFloatParams(const std::vector<float>& p);
    void getVortexIntParams(std::vector<int>& p);
    void setVortexIntParams(const std::vector<int>& p);

    void getNewtonFloatParams(std::vector<float>& p);
    void setNewtonFloatParams(const std::vector<float>& p);
    void getNewtonIntParams(std::vector<int>& p);
    void setNewtonIntParams(const std::vector<int>& p);
    // ---------------------

    
protected:
    void _setDefaultParameters();
    static int _nextUniqueID;

    int _objectID;
    std::string _objectName;
    int _uniqueID;

    // Engine properties
    // ---------------------
    std::vector<float> _bulletFloatParams;
    std::vector<int> _bulletIntParams;

    std::vector<float> _odeFloatParams;
    std::vector<int> _odeIntParams;

    std::vector<float> _vortexFloatParams;
    std::vector<int> _vortexIntParams;

    std::vector<float> _newtonFloatParams;
    std::vector<int> _newtonIntParams;
    // ---------------------

};
