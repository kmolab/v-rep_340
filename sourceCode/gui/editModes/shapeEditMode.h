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
#include "shape.h"
#include "objCont.h"
#include "textureContainer.h"
#include "uiThread.h"
#include "textureProperty.h"
#include "edgeCont.h"
#include "vMenubar.h"

class CShapeEditMode
{
public:
    CShapeEditMode(CShape* shape,int editModeType,CObjCont* objCont,CTextureContainer* textureCont,CUiThread* uiThread,bool identicalVerticesCheck,bool identicalTrianglesCheck,float identicalVerticesTolerance);
    virtual ~CShapeEditMode();

    bool endEditMode(bool cancelChanges);
    int getEditModeType();
    CShape* getEditModeShape();
    void swapShapeEditModeType(int theType);

    void selectionFromTriangleToVertexEditMode(std::vector<int>* newVertexSel=NULL);
    void selectionFromVertexToTriangleEditMode(std::vector<int>* newTriangleSel=NULL);
    void selectionFromEdgeToTriangleEditMode(std::vector<int>* newTriangleSel=NULL);
    void selectionFromEdgeToVertexEditMode(std::vector<int>* newTriangleSel=NULL);
    void selectionFromTriangleToEdgeEditMode(std::vector<int>* newEdgeSel=NULL);
    void selectionFromVertexToEdgeEditMode(std::vector<int>* newEdgeSel=NULL);

    // Edit mode selection routines:
    int getEditModeBufferSize();
    int getLastEditModeBufferValue();
    int getEditModeBufferValue(int index);
    std::vector<int>* getEditModeBuffer();
    C3Vector getEditionVertex(int index);
    void setEditionVertex(int index,const C3Vector& v);
    void getEditionTriangle(int index,int tri[3]);
    void getEditionEdge(int index,int edge[2]);
    void deselectEditModeBuffer();
    void removeItemFromEditModeBuffer(int item);
    bool isEditModeItemAValidItem(int item);
    void xorAddItemToEditModeBuffer(int item,bool disableEdgeFollowing);
    void addItemToEditModeBuffer(int item,bool disableEdgeFollowing);
    bool alreadyInEditModeBuffer(int item);

    // Other edit mode routines:
    void actualizeEditModeEditionEdges();
    float getEdgeAngle(int edgeID);
    void getNextEdges(int rearVertex,int frontVertex,std::vector<int>& edgeList);

    void displayVertices(int displayAttrib);
    void displayFaceOrientation(int displayAttrib);
    void displayEdgeEditMode(int displayAttrib);
    void flipTriangle(int index);
    void addTriangle(int ind1,int ind2,int ind3);

    void copySelection(std::vector<int>* selection);
    void paste();
    void cutSelection(std::vector<int>* selection);
    void deleteSelection(std::vector<int>* selection);
    void addMenu(VMenu* menu);
    bool processCommand(int commandID);
    void copySelectedFaces(std::vector<int>* sel,std::vector<float>* vert,std::vector<int>* ind,std::vector<float>* norm,std::vector<float>* tex);

    bool getShowHiddenVerticeAndEdges();
    void setShowHiddenVerticeAndEdges(bool show);
    bool getAutomaticallyFollowEdges();
    void setAutomaticallyFollowEdges(bool follow);
    float getEdgeMaxAngle();
    void setEdgeMaxAngle(float a);
    float getEdgeDirectionChangeMaxAngle();
    void setEdgeDirectionChangeMaxAngle(float a);

    int getEditionVerticesSize();
    int getEditionIndicesSize();
    int getEditionEdgesSize();

    void makeShape();
    void makePrimitive(int what);
    void flipTriangles();
    void subdivideTriangles();

    void insertTriangles();
    void insertTriangleFan();
    void makeDummies();

    void makePath();

private:
    CShape* _shape;
    int _editModeType;
    CObjCont* _objCont;
    CTextureContainer* _textureCont;
    CUiThread* _uiThread;

    std::vector<int> editModeBuffer;
    std::vector<float> editBufferVerticesCopy;
    std::vector<int> editBufferIndicesCopy;
    std::vector<float> editBufferNormalsCopy;
    std::vector<float> editBufferTextureCoordsCopy;

    std::vector<float> _editionVertices;
    std::vector<int> _editionIndices;
    std::vector<float> _editionNormals;
    std::vector<float> _editionTextureCoords;
    CTextureProperty* _editionTextureProperty;
    std::vector<unsigned char> _editionTexture;
    CEdgeCont _edgeCont;
    std::vector<int> _editionIndicesToEdgesIndex;
    bool showHiddenVerticeAndEdges;
    bool automaticallyFollowEdges;
    float edgeMaxAngle;
    float edgeDirectionChangeMaxAngle;
    bool _identicalVerticesCheck;
    bool _identicalTrianglesCheck;
    float _identicalVerticesTolerance;
};
