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

#include "app.h"

#ifdef SIM_WITH_OPENGL
#include "oGL.h"

void _activateNonAmbientLights(int lightHandle,CViewableBase* viewable);
void _prepareAuxClippingPlanes();
void _enableAuxClippingPlanes(int objID);
void _prepareOrEnableAuxClippingPlanes(bool prepare,int objID);
void _disableAuxClippingPlanes();
void _drawReference(C3DObject* object,float refSize);
void _displayBoundingBox(C3DObject* object,int displayAttrib,bool displRef,float refSize);
void _selectLights(C3DObject* object,CViewableBase* viewable);
void _restoreDefaultLights(C3DObject* object,CViewableBase* viewable);
void _commonStart(C3DObject* object,CViewableBase* viewable,int displayAttrib);
void _commonFinish(C3DObject* object,CViewableBase* viewable);

void _drawTriangles(const float* vertices,int verticesCnt,const int* indices,int indicesCnt,const float* normals,const float* textureCoords,int* vertexBufferId,int* normalBufferId,int* texCoordBufferId);
bool _drawEdges(const float* vertices,int verticesCnt,const int* indices,int indicesCnt,const unsigned char* edges,int* edgeBufferId);
void _drawColorCodedTriangles(const float* vertices,int verticesCnt,const int* indices,int indicesCnt,const float* normals,int* vertexBufferId,int* normalBufferId);

bool _start3DTextureDisplay(CTextureProperty* tp);
void _end3DTextureDisplay(CTextureProperty* tp);
bool _start2DTextureDisplay(CTextureProperty* tp);
void _end2DTextureDisplay(CTextureProperty* tp);

void _startTextureDisplay(CTextureObject* to,bool interpolateColor,int applyMode,bool repeatX,bool repeatY);
void _endTextureDisplay();

#endif

void initializeRendering();
void deinitializeRendering();
void initGl_ifNeeded();
void deinitGl_ifNeeded();
void increaseVertexBufferRefCnt(int vertexBufferId);
void decreaseVertexBufferRefCnt(int vertexBufferId);
void increaseNormalBufferRefCnt(int normalBufferId);
void decreaseNormalBufferRefCnt(int normalBufferId);
void increaseEdgeBufferRefCnt(int edgeBufferId);
void decreaseEdgeBufferRefCnt(int edgeBufferId);
void decreaseTexCoordBufferRefCnt(int texCoordBufferId);
void destroyGlTexture(unsigned int texName);
void makeColorCurrent(const CVisualParam* visParam,bool forceNonTransparent,bool useAuxiliaryComponent);
