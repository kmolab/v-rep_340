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
#include "visualParam.h"
#include "ser.h"
#include "3Vector.h"
#include "7Vector.h"
#include "dynMaterialObject.h"

class CViewableBase;
class CShape;
class CGeomProxy;
class CGeometric;

class CGeomWrap
{
public:

    CGeomWrap();
    virtual ~CGeomWrap();

    virtual void prepareVerticesIndicesNormalsAndEdgesForSerialization();
    virtual void perform3DObjectLoadingMapping(std::vector<int>* map);
    virtual void performTextureObjectLoadingMapping(std::vector<int>* map);
    void performDynMaterialObjectLoadingMapping(std::vector<int>* map);
    virtual void announce3DObjectWillBeErased(int objectID);
    virtual void setTextureDependencies(int shapeID);
    virtual bool getContainsTransparentComponents();
    virtual void displayGhost(CGeomProxy* geomData,int displayAttrib,bool originalColors,bool backfaceCulling,float transparency,const float* newColors);
    virtual void display(CGeomProxy* geomData,int displayAttrib,CVisualParam* collisionColor,int dynObjFlag_forVisualization,int transparencyHandling,bool multishapeEditSelected);
    virtual void display_extRenderer(CGeomProxy* geomData,int displayAttrib,const C7Vector& tr,int shapeHandle,int& componentIndex);
    virtual void display_colorCoded(CGeomProxy* geomData,int objectId,int displayAttrib);
    virtual void displayForCutting(CGeomProxy* geomData,int displayAttrib,CVisualParam* collisionColor);
    virtual CGeomWrap* copyYourself();
    virtual void scale(float xVal,float yVal,float zVal);
    virtual void setPurePrimitiveType(int theType,float xOrDiameter,float y,float zOrHeight);
    virtual int getPurePrimitiveType();
    virtual bool isGeometric();
    virtual bool isPure();
    virtual bool isConvex();
    virtual bool checkIfConvex();
    virtual void setConvex(bool convex);
    virtual bool containsOnlyPureConvexShapes();
    virtual void getCumulativeMeshes(std::vector<float>& vertices,std::vector<int>* indices,std::vector<float>* normals);
    virtual void setColor(const char* colorName,int colorComponent,const float* rgbData);
    virtual bool getColor(const char* colorName,int colorComponent,float* rgbData);
    virtual void getAllShapeComponentsCumulative(std::vector<CGeometric*>& shapeComponentList); // needed by the dynamics routine
    virtual CGeometric* getShapeComponentAtIndex(int& index);
    virtual void serialize(CSer& ar);
    virtual void preMultiplyAllVerticeLocalFrames(const C7Vector& preTr);
    virtual void flipFaces();
    virtual float getGouraudShadingAngle();
    virtual void setGouraudShadingAngle(float angle);
    virtual float getEdgeThresholdAngle();
    virtual void setEdgeThresholdAngle(float angle);
    virtual void setHideEdgeBorders(bool v);
    virtual bool getHideEdgeBorders();
    virtual int getTextureCount();
    virtual bool hasTextureThatUsesFixedTextureCoordinates();
    virtual void removeAllTextures();
    virtual void getColorStrings(std::string& colorStrings);

    void serializeWrapperInfos(CSer& ar);
    void scaleWrapperInfos(float xVal,float yVal,float zVal);
    void scaleMassAndInertia(float xVal,float yVal,float zVal);
    void copyWrapperInfos(CGeomWrap* target);
    void setDefaultInertiaParams();
    void setMass(float m);
    float getMass();
    void setName(std::string newName);
    std::string getName();

    int getDynMaterialId_OLD();
    void setDynMaterialId_OLD(int id);
    CDynMaterialObject* createDynMaterialObject_OLD();
    // ---------------------


    C7Vector getLocalInertiaFrame();
    void setLocalInertiaFrame(const C7Vector& li);
    C3Vector getPrincipalMomentsOfInertia();
    void setPrincipalMomentsOfInertia(const C3Vector& inertia);

    C7Vector getTransformationsSinceGrouping();
    void setTransformationsSinceGrouping(const C7Vector& tr);

    static void findPrincipalMomentOfInertia(const C3X3Matrix& tensor,C4Vector& rotation,C3Vector& principalMoments);
    static C3X3Matrix getNewTensor(const C3Vector& principalMoments,const C7Vector& newFrame);

    std::vector<CGeomWrap*> childList;

    // For backward compatibility (12/9/2013):
    // ---------------------------------------
    bool __bulletStickyContact;
    float __bulletRestitution;
    float __bulletFriction;
    float __bulletLinearDamping;
    float __bulletAngularDamping;
    float __bulletNonDefaultCollisionMarginFactor;
    bool __bulletNonDefaultCollisionMargin;
    float __bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape;
    bool __bulletNonDefaultCollisionMargin_forConvexAndNonPureShape;
    bool __bulletAutoShrinkConvexMesh;

    int __odeMaxContacts;
    float __odeFriction;
    float __odeSoftERP;
    float __odeSoftCFM;
    float __odeLinearDamping;
    float __odeAngularDamping;
    // ---------------------------------------

protected:
    static float _getTensorNonDiagonalMeasure(const C3X3Matrix& tensor);

    std::string _name;

    float _mass;

    int _dynMaterialId_OLD;

    C7Vector _localInertiaFrame; // frame relative to the shape.
    C3Vector _principalMomentsOfInertia; // remember that we always work with a massless tensor. The tensor is multiplied with the mass in the dynamics module!

    C7Vector _transformationsSinceGrouping; // used to keep track of this geomWrap or geometric's configuration relative to the shape

    bool _convex;

};
