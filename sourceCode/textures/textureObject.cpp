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
#include "v_rep_internal.h"
#include "textureObject.h"
#include "app.h"
#include <boost/format.hpp>

unsigned int CTextureObject::_textureContentUniqueId=0;

CTextureObject::CTextureObject()
{ // for serialization
    _objectID=SIM_IDSTART_TEXTURE;
    _objectName="Texture";
    _textureSize[0]=16;
    _textureSize[1]=16;
    _oglTextureName=(unsigned int)-1;
    _textureBuffer.resize(4*_textureSize[0]*_textureSize[1],0);
    _providedImageWasRGBA=false;
    _changedFlag=true;
    _currentTextureContentUniqueId=_textureContentUniqueId++;
}

CTextureObject::CTextureObject(int sizeX,int sizeY)
{
    _objectID=SIM_IDSTART_TEXTURE;
    _objectName="Texture";
    _textureSize[0]=sizeX;
    _textureSize[1]=sizeY;
    _oglTextureName=(unsigned int)-1;
    _textureBuffer.resize(4*_textureSize[0]*_textureSize[1],0);
    _providedImageWasRGBA=false;
    _changedFlag=true;
    _currentTextureContentUniqueId=_textureContentUniqueId++;
}

CTextureObject::~CTextureObject()
{
    if (_oglTextureName!=(unsigned int)-1)
    { // destroy the texture in the UI thread
        SUIThreadCommand cmdIn;
        SUIThreadCommand cmdOut;
        cmdIn.cmdId=DESTROY_GL_TEXTURE_UITHREADCMD;
        cmdIn.uintParams.push_back(_oglTextureName);
        App::uiThread->executeCommandViaUiThread(&cmdIn,&cmdOut);
    }
}

void CTextureObject::setObjectID(int newID)
{
    _objectID=newID;
}

int CTextureObject::getObjectID()
{
    return(_objectID);
}

void CTextureObject::setObjectName(const char* newName)
{
    _objectName=newName;
}

std::string CTextureObject::getObjectName()
{
    return(_objectName);
}

void CTextureObject::getTextureSize(int& sizeX,int& sizeY)
{
    sizeX=_textureSize[0];
    sizeY=_textureSize[1];
}

void CTextureObject::setImage(bool rgba,bool horizFlip,bool vertFlip,unsigned char* data)
{
    int dirX=1;
    int dirY=1;
    int stX=0;
    int stY=0;
    if (horizFlip)
    {
        dirX=-1;
        stX=_textureSize[0]-1;
    }
    if (!vertFlip)
    {
        dirY=-1;
        stY=_textureSize[1]-1;
    }
    if (rgba)
    {
        for (int i=0;i<_textureSize[1];i++)
        {
            int p=i*_textureSize[0];
            int w=(stY+dirY*i)*_textureSize[0];
            for (int j=0;j<_textureSize[0];j++)
            {
                int q=4*(p+j);
                int v=4*(w+stX+dirX*j);
                _textureBuffer[q+0]=data[v+0];
                _textureBuffer[q+1]=data[v+1];
                _textureBuffer[q+2]=data[v+2];
                _textureBuffer[q+3]=data[v+3];
            }
        }
    }
    else
    {
        for (int i=0;i<_textureSize[1];i++)
        {
            int p=i*_textureSize[0];
            int w=(stY+dirY*i)*_textureSize[0];
            for (int j=0;j<_textureSize[0];j++)
            {
                int q=4*(p+j);
                int v=3*(w+stX+dirX*j);
                _textureBuffer[q+0]=data[v+0];
                _textureBuffer[q+1]=data[v+1];
                _textureBuffer[q+2]=data[v+2];
                _textureBuffer[q+3]=255;
            }
        }
    }
    _providedImageWasRGBA=rgba;
    _changedFlag=true;
    _currentTextureContentUniqueId=_textureContentUniqueId++;
}

bool CTextureObject::announceGeneralObjectWillBeErased(int objectID,int subObjectID)
{ // return value true means this object needs destruction!
    for (int i=0;i<int(_dependentObjects.size());i++)
    {
        if (_dependentObjects[i]==objectID)
        {
            if (subObjectID==-1)
            {
                _dependentObjects.erase(_dependentObjects.begin()+i);
                _dependentSubObjects.erase(_dependentSubObjects.begin()+i);
                i--; // we have to reprocess this position!
            }
            else
            {
                if (subObjectID==_dependentSubObjects[i])
                {
                    _dependentObjects.erase(_dependentObjects.begin()+i);
                    _dependentSubObjects.erase(_dependentSubObjects.begin()+i);
                    i--; // we have to reprocess this position!
                }
            }
        }
    }
    return(_dependentObjects.size()==0);
}

void CTextureObject::transferDependenciesToThere(CTextureObject* receivingObject)
{
    for (int i=0;i<int(_dependentObjects.size());i++)
        receivingObject->_dependentObjects.push_back(_dependentObjects[i]);
    for (int i=0;i<int(_dependentSubObjects.size());i++)
        receivingObject->_dependentSubObjects.push_back(_dependentSubObjects[i]);
    clearAllDependencies();
}


void CTextureObject::addDependentObject(int objectID,int subObjectID)
{
    _dependentObjects.push_back(objectID);
    _dependentSubObjects.push_back(subObjectID);
}

void CTextureObject::clearAllDependencies()
{
    _dependentObjects.clear();
    _dependentSubObjects.clear();
}

bool CTextureObject::isSame(CTextureObject* obj)
{
    if ( (obj->_textureSize[0]==_textureSize[0])&&(obj->_textureSize[1]==_textureSize[1]) )
    {
        if (obj->_providedImageWasRGBA!=_providedImageWasRGBA)
            return(false);
        for (int i=0;i<4*_textureSize[0]*_textureSize[1];i++)
        {
            if (obj->_textureBuffer[i]!=_textureBuffer[i])
                return(false);
        }
        return(true);
    }
    return(false);
}

void CTextureObject::setTextureBuffer(const std::vector<unsigned char>& tb)
{
    _textureBuffer.assign(tb.begin(),tb.end());
    _changedFlag=true;
    _currentTextureContentUniqueId=_textureContentUniqueId++;
}

void CTextureObject::getTextureBuffer(std::vector<unsigned char>& tb)
{
    tb.assign(_textureBuffer.begin(),_textureBuffer.end());
}

unsigned char* CTextureObject::getTextureBufferPointer()
{
    return(&_textureBuffer[0]);
}

void CTextureObject::lightenUp()
{
    for (int i=0;i<int(_textureBuffer.size())/4;i++)
    {
        int avg=_textureBuffer[4*i+0];
        avg+=_textureBuffer[4*i+1];
        avg+=_textureBuffer[4*i+2];
        avg/=3;
        avg=128+avg/2;
        _textureBuffer[4*i+0]=avg;
        _textureBuffer[4*i+1]=avg;
        _textureBuffer[4*i+2]=avg;
    }
    /*
    // do some sort of contrast filter and shift all values upwards:
    std::vector<unsigned char> tmp(int(_textureBuffer.size())/4);
    for (int i=0;i<int(_textureBuffer.size())/4;i++)
    {
        int avg=_textureBuffer[4*i+0];
        avg+=_textureBuffer[4*i+1];
        avg+=_textureBuffer[4*i+2];
        avg/=3;
        tmp[i]=avg;
        _textureBuffer[4*i+0]=avg;
    }
    std::sort(tmp.begin(),tmp.end());
    unsigned char l0=tmp[tmp.size()/4];
    unsigned char l2=tmp[tmp.size()/2];
    unsigned char l3=tmp[3*tmp.size()/4];
    for (int i=0;i<int(_textureBuffer.size())/4;i++)
    {
        int v=_textureBuffer[4*i+0];
        if (v<=l0)
            v=160;
        else
        {
            if (v<=l2)
                v=192;
            else
            {
                if (v<=l3)
                    v=224;
                else
                    v=255;
            }
        }
        _textureBuffer[4*i+0]=v;
        _textureBuffer[4*i+1]=v;
        _textureBuffer[4*i+2]=v;
        _textureBuffer[4*i+3]=255;
    }
*/
    _changedFlag=true;
    _currentTextureContentUniqueId=_textureContentUniqueId++;
}

void CTextureObject::setRandomContent()
{
    for (int i=0;i<int(_textureBuffer.size())/4;i++)
    {
        _textureBuffer[4*i+0]=(unsigned char)(SIM_RAND_FLOAT*255.0f);
        _textureBuffer[4*i+1]=(unsigned char)(SIM_RAND_FLOAT*255.0f);
        _textureBuffer[4*i+2]=(unsigned char)(SIM_RAND_FLOAT*255.0f);
    }
    _changedFlag=true;
    _currentTextureContentUniqueId=_textureContentUniqueId++;
}

unsigned int CTextureObject::getCurrentTextureContentUniqueId()
{
    return(_currentTextureContentUniqueId);
}

void CTextureObject::setOglTextureName(unsigned int n)
{
    _oglTextureName=n;
}

unsigned int CTextureObject::getOglTextureName()
{
    return(_oglTextureName);
}

bool CTextureObject::getChangedFlag()
{
    return(_changedFlag);
}

void CTextureObject::setChangedFlag(bool c)
{
    _changedFlag=c;
}

CTextureObject* CTextureObject::copyYourself()
{
    CTextureObject* newObj=new CTextureObject();
    newObj->_objectID=_objectID;
    newObj->_objectName=_objectName;

    newObj->_textureSize[0]=_textureSize[0];
    newObj->_textureSize[1]=_textureSize[1];

    newObj->_textureBuffer.assign(_textureBuffer.begin(),_textureBuffer.end());
    newObj->_providedImageWasRGBA=_providedImageWasRGBA;
    newObj->_changedFlag=true;
    newObj->_currentTextureContentUniqueId=_textureContentUniqueId++;

    newObj->_dependentObjects.assign(_dependentObjects.begin(),_dependentObjects.end());
    newObj->_dependentSubObjects.assign(_dependentSubObjects.begin(),_dependentSubObjects.end());

    return(newObj);
}

char* CTextureObject::readPortionOfTexture(int posX,int posY,int sizeX,int sizeY)
{
    if ( (posX<0)||(posY<0)||(sizeX<1)||(sizeY<1)||(posX+sizeX>_textureSize[0])||(posY+sizeY>_textureSize[1]) )
        return(NULL);
    char* buff;
    buff=new char[sizeX*sizeY*3];
    int p=0;
    int resX=_textureSize[0];
    for (int j=posY;j<posY+sizeY;j++)
    {
        for (int i=posX;i<posX+sizeX;i++)
        {
            buff[3*p+0]=_textureBuffer[4*(j*resX+i)+0];
            buff[3*p+1]=_textureBuffer[4*(j*resX+i)+1];
            buff[3*p+2]=_textureBuffer[4*(j*resX+i)+2];
            p++;
        }
    }
    return(buff);
}

bool CTextureObject::writePortionOfTexture(const unsigned char* rgbData,int posX,int posY,int sizeX,int sizeY,bool circular,float interpol)
{
    if ( (posX<0)||(posY<0)||(sizeX<1)||(sizeY<1)||(posX+sizeX>_textureSize[0])||(posY+sizeY>_textureSize[1]) )
        return(false);
    int p=0;
    int resX=_textureSize[0];
    if (interpol==0.0f)
    {
        if (circular)
        { // circular
            int hx=posX+sizeX/2;
            int hy=posY+sizeY/2;
            for (int j=posY;j<posY+sizeY;j++)
            {
                float dy=float(hy-j)/float(sizeY/2);
                float dy2=dy*dy;
                for (int i=posX;i<posX+sizeX;i++)
                {
                    float dx=float(hx-i)/float(sizeX/2);
                    float dx2=dx*dx;
                    if (dx2+dy2<=1.0f)
                    {
                        _textureBuffer[4*(j*resX+i)+0]=rgbData[3*p+0];
                        _textureBuffer[4*(j*resX+i)+1]=rgbData[3*p+1];
                        _textureBuffer[4*(j*resX+i)+2]=rgbData[3*p+2];
                    }
                    p++;
                }
            }
        }
        else
        {
            for (int j=posY;j<posY+sizeY;j++)
            {
                for (int i=posX;i<posX+sizeX;i++)
                {
                    _textureBuffer[4*(j*resX+i)+0]=rgbData[3*p+0];
                    _textureBuffer[4*(j*resX+i)+1]=rgbData[3*p+1];
                    _textureBuffer[4*(j*resX+i)+2]=rgbData[3*p+2];
                    p++;
                }
            }
        }
    }
    else
    {
        float interpolI=1.0f-interpol;
        if (circular)
        { // circular
            int hx=posX+sizeX/2;
            int hy=posY+sizeY/2;
            for (int j=posY;j<posY+sizeY;j++)
            {
                float dy=float(hy-j)/float(sizeY/2);
                float dy2=dy*dy;
                for (int i=posX;i<posX+sizeX;i++)
                {
                    float dx=float(hx-i)/float(sizeX/2);
                    float dx2=dx*dx;
                    if (dx2+dy2<=1.0f)
                    {
                        _textureBuffer[4*(j*resX+i)+0]=rgbData[3*p+0]*interpolI+_textureBuffer[4*(j*resX+i)+0]*interpol;
                        _textureBuffer[4*(j*resX+i)+1]=rgbData[3*p+1]*interpolI+_textureBuffer[4*(j*resX+i)+1]*interpol;
                        _textureBuffer[4*(j*resX+i)+2]=rgbData[3*p+2]*interpolI+_textureBuffer[4*(j*resX+i)+2]*interpol;
                    }
                    p++;
                }
            }
        }
        else
        {
            for (int j=posY;j<posY+sizeY;j++)
            {
                for (int i=posX;i<posX+sizeX;i++)
                {
                    _textureBuffer[4*(j*resX+i)+0]=rgbData[3*p+0]*interpolI+_textureBuffer[4*(j*resX+i)+0]*interpol;
                    _textureBuffer[4*(j*resX+i)+1]=rgbData[3*p+1]*interpolI+_textureBuffer[4*(j*resX+i)+1]*interpol;
                    _textureBuffer[4*(j*resX+i)+2]=rgbData[3*p+2]*interpolI+_textureBuffer[4*(j*resX+i)+2]*interpol;
                    p++;
                }
            }
        }
    }
    _changedFlag=true;
    _currentTextureContentUniqueId=_textureContentUniqueId++;
    return(true);
}

void CTextureObject::serialize(CSer& ar)
{
    if (ar.isStoring())
    {       // Storing
        ar.storeDataName("Ipa");
        ar << _objectID << _textureSize[0] << _textureSize[1];
        ar.flush();

        ar.storeDataName("Gon");
        ar << _objectName;
        ar.flush();

        ar.storeDataName("Bst");
        unsigned char nothing=0;
        SIM_SET_CLEAR_BIT(nothing,0,_providedImageWasRGBA);
        ar << nothing;
        ar.flush();

        if (App::ct->undoBufferContainer->isUndoSavingOrRestoringUnderWay())
        { // undo/redo serialization:
            ar.storeDataName("Img");
            ar << App::ct->undoBufferContainer->undoBufferArrays.addTextureBuffer(_textureBuffer,App::ct->undoBufferContainer->getNextBufferId());
            ar.flush();
        }
        else
        { // normal serialization
            ar.storeDataName("Img");
            for (int i=0;i<_textureSize[0]*_textureSize[1];i++)
            {
                ar << _textureBuffer[4*i+0];
                ar << _textureBuffer[4*i+1];
                ar << _textureBuffer[4*i+2];
                if (_providedImageWasRGBA)
                    ar << _textureBuffer[4*i+3];
            }
            ar.flush();
        }

        ar.storeDataName(SER_END_OF_OBJECT);
    }
    else
    {       // Loading
        int byteQuantity;
        std::string theName="";
        while (theName.compare(SER_END_OF_OBJECT)!=0)
        {
            theName=ar.readDataName();
            if (theName.compare(SER_END_OF_OBJECT)!=0)
            {
                bool noHit=true;
                if (theName.compare("Ipa")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> _objectID >> _textureSize[0] >> _textureSize[1];
                }
                if (theName.compare("Gon")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> _objectName;
                }
                if (theName=="Bst")
                {
                    noHit=false;
                    ar >> byteQuantity;
                    unsigned char nothing;
                    ar >> nothing;
                    _providedImageWasRGBA=SIM_IS_BIT_SET(nothing,0);
                }
                if (App::ct->undoBufferContainer->isUndoSavingOrRestoringUnderWay())
                { // undo/redo serialization
                    if (theName.compare("Img")==0)
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        int id;
                        ar >> id;
                        App::ct->undoBufferContainer->undoBufferArrays.getTextureBuffer(id,_textureBuffer);
                        _changedFlag=true;
                        _currentTextureContentUniqueId=_textureContentUniqueId++;
                    }
                }
                else
                { // normal serialization
                    if (theName=="Img")
                    {
                        noHit=false;
                        ar >> byteQuantity;
                        _textureBuffer.resize(4*_textureSize[0]*_textureSize[1],0);
                        for (int i=0;i<_textureSize[0]*_textureSize[1];i++)
                        {
                            ar >> _textureBuffer[4*i+0];
                            ar >> _textureBuffer[4*i+1];
                            ar >> _textureBuffer[4*i+2];
                            if (_providedImageWasRGBA)
                                ar >> _textureBuffer[4*i+3];
                            else
                                _textureBuffer[4*i+3]=255;
                        }
                        _changedFlag=true;
                        _currentTextureContentUniqueId=_textureContentUniqueId++;
                    }
                }
                if (noHit)
                    ar.loadUnknownData();
            }
        }
    }
}
