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
#include "objFile.h"
#include "tt.h"
#include "app.h"
#include "vVarious.h"

CObjFile::CObjFile()
{
    fileFormat=FILE_FORMAT_OBJ;
}

CObjFile::~CObjFile()
{
}

bool CObjFile::_getCode(VArchive &archive)
{
    code="";
    value="";
    if (archive.readMultiLine(actualPosition,value,false," "))
    {
        tt::removeSpacesAtBeginningAndEnd(value);
        if (tt::extractSpaceSeparatedWord(value,code))
            tt::removeSpacesAtBeginningAndEnd(value);
        return(true);
    }
    return(false);
}

bool CObjFile::_getMaterialCode(VArchive& archive,unsigned int& archiveCurrentPos,std::string& matCode,std::string& matValue)
{
    matCode="";
    matValue="";
    if (archive.readMultiLine(archiveCurrentPos,matValue,false," "))
    {
        tt::removeSpacesAtBeginningAndEnd(matValue);
        if (tt::extractSpaceSeparatedWord(matValue,matCode))
            tt::removeSpacesAtBeginningAndEnd(matValue);
        return(true);
    }
    return(false);
}

bool CObjFile::_writeVertex(VArchive& archive,C3Vector& v)
{
    std::string text("v ");
    text.append(tt::FNb(v(0)));
    text.append(1,' ');
    text.append(tt::FNb(v(1)));
    text.append(1,' ');
    text.append(tt::FNb(v(2)));
    archive.writeLine(text);
    return(true);
}

bool CObjFile::_writeIndex(VArchive& archive,int a,int b,int c)
{
    std::string text("f ");
    text.append(tt::FNb(a));
    text.append(1,' ');
    text.append(tt::FNb(b));
    text.append(1,' ');
    text.append(tt::FNb(c));
    archive.writeLine(text);
    return(true);
}

bool CObjFile::importFunc(std::string fileName)
{
    if (VFile::doesFileExist(fileName))
    {
        try
        {
            VFile file(fileName.c_str(),VFile::READ|VFile::SHARE_DENY_NONE);
            VArchive archive(&file,VArchive::LOAD);
            archiveLength=(unsigned int)file.getLength();
            bool result=_readFile(archive,fileName.c_str());
            archive.close();
            file.close();
            return(result);
        }
        catch(VFILE_EXCEPTION_TYPE e)
        {
            VFile::reportAndHandleFileExceptionError(e);
            return(false);
        }
    }
    return(false);
}


bool CObjFile::_readFile(VArchive& archive,const char* fileName)
{   
    actualPosition=0;
    std::vector<float> normals_tri;
    std::vector<float> textureCoords_tri;
    std::vector<float> normals;
    std::vector<float> textureCoords;
    std::vector<float> vertices;

    std::vector<int> indexList;
    std::vector<float> textureCoordList;
    std::vector<float> normalsList;

    std::vector<SObjMaterial> materialList;


    std::vector<std::vector<int>*> tmpImportedIndices;
    std::vector<std::vector<float>*> tmpImportedTextureCoordinates;
    std::vector<std::vector<float>*> tmpImportedNormals;
    std::vector<SObjMaterial*> tmpImportedMaterials;
    std::vector<std::string> tmpImportedNames;


    bool error=false;
    bool nextShape=true;
    int lineCount=0;
    int mode=-1;
    std::string lastGroupName,lastObjectName;
    int lastMaterialIndex=-1;
    while (_getCode(archive))
    {
        lineCount++;
        if (code=="g")
        {
            nextShape=true;
            tt::removeSpacesAtBeginningAndEnd(value);
            lastGroupName=value;
        }
        else if (code=="o")
        {
            nextShape=true;
            tt::removeSpacesAtBeginningAndEnd(value);
            lastObjectName=value;
        }
        else if (code=="mtllib")
        {
            nextShape=true;
            tt::removeSpacesAtBeginningAndEnd(value);
            if (VVarious::isAbsolutePath(value))
                value=VVarious::splitPath_fileBaseAndExtension(value);
            std::string tmp=VVarious::splitPath_path(fileName)+VREP_SLASH+value;
            _appendMaterialFileToMaterialList(tmp.c_str(),materialList);
        }
        else if (code=="usemtl")
        {
            tt::removeSpacesAtBeginningAndEnd(value);
            int materialIndex=-1;
            for (int i=0;i<int(materialList.size());i++)
            {
                if (materialList[i].materialName.compare(value)==0)
                    materialIndex=i;
            }
            if (materialIndex!=lastMaterialIndex)
                nextShape=true;
            lastMaterialIndex=materialIndex;
            // nextShape=true; REMOVED ON 14/5/2014: produced strange results with some files. By removing that instruction however, some file data might be ignored!
        }
        else if (code=="v")
        {
            nextShape=true;
            float v=0.0f;
            for (int i=0;i<3;i++)
            {
                if (_getVertexOrTextureCoordOrNormalValue(value,v))
                    vertices.push_back(v);
                else
                {
                    vertices.push_back(0.0f);
                    error=true;
                }
            }
        }
        else if (code=="vt")
        {
            nextShape=true;
            float v=0.0f;
            for (int i=0;i<2;i++)
            {
                if (_getVertexOrTextureCoordOrNormalValue(value,v))
                {
// Following 4 removed on 14/5/2014:
//                  if (v>1.0001f) // some do not respect the 0-1 limit!
//                      v=fmod(v,1.0f);
//                  if (v<0.0f) // some do not respect the 0-1 limit!
//                      v=fmod(v,1.0f)+1.0f;
                    textureCoords.push_back(v);
                }
                else
                {
                    textureCoords.push_back(0.0f);
                    error=true;
                }
            }
        }
        else if (code=="vn")
        {
            nextShape=true;
            float v=0.0f;
            for (int i=0;i<3;i++)
            {
                if (_getVertexOrTextureCoordOrNormalValue(value,v))
                    normals.push_back(v);
                else
                {
                    normals.push_back(1.0f);
                    error=true;
                }
            }
        }
        else
        {
            if (code=="f")
            {
                if (nextShape)
                {
                    std::vector<int>* indices=new std::vector<int>;
                    tmpImportedIndices.push_back(indices);

                    std::vector<float>* aaa=new std::vector<float>;
                    tmpImportedTextureCoordinates.push_back(aaa);

                    aaa=new std::vector<float>;
                    tmpImportedNormals.push_back(aaa);

                    if (lastObjectName.length()==0)
                        tmpImportedNames.push_back(lastGroupName);
                    else
                        tmpImportedNames.push_back(lastObjectName);

                    if (lastMaterialIndex!=-1)
                    {
                        SObjMaterial* m=new SObjMaterial;
                        for (int i=0;i<3;i++)
                        {
                            m->ambient[i]=materialList[lastMaterialIndex].ambient[i];
                            m->diffuse[i]=materialList[lastMaterialIndex].diffuse[i];
                            m->specular[i]=materialList[lastMaterialIndex].specular[i];
                            m->emission[i]=materialList[lastMaterialIndex].emission[i];
                        }
                        m->transparency=materialList[lastMaterialIndex].transparency;
                        m->shininess=materialList[lastMaterialIndex].shininess;
                        m->textureFile=materialList[lastMaterialIndex].textureFile;
                        m->materialName=materialList[lastMaterialIndex].materialName;
                        tmpImportedMaterials.push_back(m);
                    }
                    else
                        tmpImportedMaterials.push_back(NULL);

                    nextShape=false;
                    mode=-1;
                }
                int index=0;
                int tindex=0;
                int nindex=0;
                indexList.clear();
                textureCoordList.clear();
                normalsList.clear();

                std::vector<int> ind;
                std::vector<int> tind;
                std::vector<int> nind;
                bool indexError=false;
                int cnt=0;
                while (true)
                {
                    std::string aword;
                    if (tt::extractSpaceSeparatedWord(value,aword))
                    {
                        cnt++;
                        tt::removeSpacesAtBeginningAndEnd(value);
                        int tmode=_getIndexValue(aword,index,tindex,nindex);
                        if (tmode==-1)
                            indexError=true;
                        else
                        {
                            if (mode==-1)
                                mode=tmode;

                            int theInd;
                            if (index>=0)
                                theInd=index-1;
                            else
                                theInd=(vertices.size()/3)+index;
                            if ((theInd<0)||(theInd>=int(vertices.size())/3))
                            {
                                indexError=true;
                                error=true;
                            }
                            else
                                ind.push_back(theInd);
                            if (mode&1)
                            {
                                if (tindex>=0)
                                    theInd=tindex-1;
                                else
                                    theInd=(textureCoords.size()/2)+tindex;
                                if ((theInd<0)||(theInd>=int(textureCoords.size())/2))
                                {
                                    indexError=true;
                                    error=true;
                                }
                                else
                                    tind.push_back(theInd);
                            }
                            if (mode&2)
                            {
                                if (nindex>=0)
                                    theInd=nindex-1;
                                else
                                    theInd=(normals.size()/3)+nindex;
                                if ((theInd<0)||(theInd>=int(normals.size())/3))
                                {
                                    indexError=true;
                                    error=true;
                                }
                                else
                                    nind.push_back(theInd);
                            }
                        }
                    }
                    else
                    {
                        if (cnt<3)
                        {
                            indexError=true;
                            error=true;
                        }
                        break;
                    }
                }
                bool textureCoordsAreOk=false;
                if (!indexError)
                {
                    indexList.push_back(ind[0]);
                    indexList.push_back(ind[1]);
                    indexList.push_back(ind[2]);
                    if (mode&1)
                    {
// Following condition commented out on 16/3/2014: it is common to have sometimes 3 times the same text. coord.!
//                      if ((tind[0]!=tind[1])||(tind[0]!=tind[2])||(tind[1]!=tind[2]))
//                      { // some save the same texture coordinates for each triangle --> we ignore those texture coordinates
                            textureCoordsAreOk=true;
                            textureCoordList.push_back(textureCoords[2*tind[0]+0]);
                            textureCoordList.push_back(textureCoords[2*tind[0]+1]);
                            textureCoordList.push_back(textureCoords[2*tind[1]+0]);
                            textureCoordList.push_back(textureCoords[2*tind[1]+1]);
                            textureCoordList.push_back(textureCoords[2*tind[2]+0]);
                            textureCoordList.push_back(textureCoords[2*tind[2]+1]);
//                      }
                    }
                    if (mode&2)
                    {
                        normalsList.push_back(normals[3*nind[0]+0]);
                        normalsList.push_back(normals[3*nind[0]+1]);
                        normalsList.push_back(normals[3*nind[0]+2]);
                        normalsList.push_back(normals[3*nind[1]+0]);
                        normalsList.push_back(normals[3*nind[1]+1]);
                        normalsList.push_back(normals[3*nind[1]+2]);
                        normalsList.push_back(normals[3*nind[2]+0]);
                        normalsList.push_back(normals[3*nind[2]+1]);
                        normalsList.push_back(normals[3*nind[2]+2]);
                    }
                    C3Vector v0(&vertices[3*ind[0]]);
                    C3Vector v1(&vertices[3*ind[1]]);
                    C3Vector v2(&vertices[3*ind[2]]);
                    ind.erase(ind.begin()+1);

                    if (mode&1)
                        tind.erase(tind.begin()+1);
                    if (mode&2)
                        nind.erase(nind.begin()+1);

                    C3Vector w0(v1-v0);
                    C3Vector w1(v2-v0);
                    C3Vector n((w0^w1).getNormalized());
                    int offr=0;
                    int failCount=0;
                    while (ind.size()>=3)
                    {
                        int of[3]={offr,offr+1,offr+2};
                        for (int i=0;i<3;i++)
                        {
                            if (of[i]>=int(ind.size()))
                                of[i]-=ind.size();
                        }
                        v0.set(&vertices[3*ind[of[0]]]);
                        v1.set(&vertices[3*ind[of[1]]]);
                        v2.set(&vertices[3*ind[of[2]]]);

                        w0=v1-v0;
                        w1=v2-v0;
                        C3Vector n2((w0^w1).getNormalized());
                        if (n2*n>=0.0f)
                        {
                            indexList.push_back(ind[of[0]]);
                            indexList.push_back(ind[of[1]]);
                            indexList.push_back(ind[of[2]]);
                            ind.erase(ind.begin()+of[1]);

                            if (mode&1)
                            {
// Following condition commented out on 16/3/2014: it is common to have sometimes 3 times the same text. coord.!
//                              if ((tind[0]!=tind[1])||(tind[0]!=tind[2])||(tind[1]!=tind[2]))
//                              { // some save the same texture coordinates for each triangle --> we ignore those texture coordinates
                                    textureCoordList.push_back(textureCoords[2*tind[of[0]]+0]);
                                    textureCoordList.push_back(textureCoords[2*tind[of[0]]+1]);
                                    textureCoordList.push_back(textureCoords[2*tind[of[1]]+0]);
                                    textureCoordList.push_back(textureCoords[2*tind[of[1]]+1]);
                                    textureCoordList.push_back(textureCoords[2*tind[of[2]]+0]);
                                    textureCoordList.push_back(textureCoords[2*tind[of[2]]+1]);
                                    tind.erase(tind.begin()+of[1]);
//                              }
                            }
                            if (mode&2)
                            {
                                normalsList.push_back(normals[3*nind[of[0]]+0]);
                                normalsList.push_back(normals[3*nind[of[0]]+1]);
                                normalsList.push_back(normals[3*nind[of[0]]+2]);
                                normalsList.push_back(normals[3*nind[of[1]]+0]);
                                normalsList.push_back(normals[3*nind[of[1]]+1]);
                                normalsList.push_back(normals[3*nind[of[1]]+2]);
                                normalsList.push_back(normals[3*nind[of[2]]+0]);
                                normalsList.push_back(normals[3*nind[of[2]]+1]);
                                normalsList.push_back(normals[3*nind[of[2]]+2]);
                                nind.erase(nind.begin()+of[1]);
                            }
                            failCount=0;
                        }
                        offr++;
                        if (offr>=int(ind.size()))
                        {
                            offr=0;
                            failCount++;
                        }
                        if (failCount>4)
                            n=n*-1.0f;
                    }
                }
                if (!indexError) 
                {
                    for (int i=0;i<int(indexList.size())/3;i++)
                    {
                        tmpImportedIndices[tmpImportedIndices.size()-1]->push_back(indexList[3*i+0]);
                        tmpImportedIndices[tmpImportedIndices.size()-1]->push_back(indexList[3*i+1]);
                        tmpImportedIndices[tmpImportedIndices.size()-1]->push_back(indexList[3*i+2]);
                        numberOfTrianglesRead++;
                    }
                    if ((mode&1)&&textureCoordsAreOk)
                        tmpImportedTextureCoordinates[tmpImportedTextureCoordinates.size()-1]->insert(tmpImportedTextureCoordinates[tmpImportedTextureCoordinates.size()-1]->end(),textureCoordList.begin(),textureCoordList.end());
                    if (mode&2)
                        tmpImportedNormals[tmpImportedNormals.size()-1]->insert(tmpImportedNormals[tmpImportedNormals.size()-1]->end(),normalsList.begin(),normalsList.end());
                }
            }
        }
    }

    for (int i=0;i<int(tmpImportedIndices.size());i++)
    {
        SObjMaterial* mat=tmpImportedMaterials[i];
        std::string nm=tmpImportedNames[i];
        // Check if there is already a similar name/material. If yes, merge them:
        int sameIndex=-1;
        for (int j=0;j<int(_importedIndices.size());j++)
        {
            SObjMaterial* mat2=_importedMaterials[j];
            std::string nm2=_importedNames[j];
            if (nm.compare(nm2)==0)
            {
                if (mat==mat2)
                    sameIndex=j;
                else
                {
                    if ((mat!=NULL)&&(mat2!=NULL))
                    {
                        if (mat->materialName.compare(mat2->materialName)==0)
                            sameIndex=j;
                    }
                }
                if (sameIndex!=-1)
                    break;
            }
        }
        if (sameIndex!=-1)
        {
            _importedIndices[sameIndex]->insert(_importedIndices[sameIndex]->begin(),tmpImportedIndices[i]->begin(),tmpImportedIndices[i]->end());
            _importedTextureCoordinates[sameIndex]->insert(_importedTextureCoordinates[sameIndex]->begin(),tmpImportedTextureCoordinates[i]->begin(),tmpImportedTextureCoordinates[i]->end());
            _importedNormals[sameIndex]->insert(_importedNormals[sameIndex]->begin(),tmpImportedNormals[i]->begin(),tmpImportedNormals[i]->end());
            delete tmpImportedIndices[i];
            delete tmpImportedTextureCoordinates[i];
            delete tmpImportedNormals[i];
            delete tmpImportedMaterials[i];
        }
        else
        {
            _importedIndices.push_back(tmpImportedIndices[i]);
            _importedTextureCoordinates.push_back(tmpImportedTextureCoordinates[i]);
            _importedNormals.push_back(tmpImportedNormals[i]);
            _importedMaterials.push_back(tmpImportedMaterials[i]);
            _importedNames.push_back(tmpImportedNames[i]);
        }
    }

    std::vector<int> vertIndex;
    for (int i=0;i<int(_importedIndices.size());i++)
    {
        vertIndex.clear();
        for (int j=0;j<int(vertices.size())/3;j++)
            vertIndex.push_back(-1);
        std::vector<float>* theV=new std::vector<float>;
        _importedVertices.push_back(theV);
        for (int j=0;j<int(_importedIndices[i]->size());j++)
        {
            if (vertIndex[_importedIndices[i]->at(j)]==-1)
            {
                vertIndex[_importedIndices[i]->at(j)]=theV->size()/3;
                theV->push_back(vertices[3*_importedIndices[i]->at(j)+0]);
                theV->push_back(vertices[3*_importedIndices[i]->at(j)+1]);
                theV->push_back(vertices[3*_importedIndices[i]->at(j)+2]);
            }
            _importedIndices[i]->at(j)=vertIndex[_importedIndices[i]->at(j)];
        }
    }

    return(!error);
}

bool CObjFile::_getVertexOrTextureCoordOrNormalValue(std::string& text,float& v)
{
    tt::removeSpacesAtBeginningAndEnd(text);
    std::string front="";
    if (tt::extractSpaceSeparatedWord(text,front))
    {
        tt::removeSpacesAtBeginningAndEnd(text);
        if (tt::getValidFloat(front,v))
            return(true);
    }
    return(false);
}

int CObjFile::_getIndexValue(std::string& text,int& v,int& vt,int& vn)
{ // bit0 --> texture index present, bit1 --> normal index present. -1 --> error
    size_t p=text.find("/");
    if (p==std::string::npos)
    { // just a triangle index:
        if (tt::getValidInt(text,v))
            return(0);
        return(-1);
    }
    std::string subt(text.begin(),text.begin()+p);
    text.erase(text.begin(),text.begin()+p+1);
    if (!tt::getValidInt(subt,v))
        return(-1);
    if (text.length()==0)
        return(-1);
    if (text[0]=='/')
    { // a triangle index, and a normal index:
        text.erase(text.begin());
        if (!tt::getValidInt(text,vn))
            return(-1);
        return(2);
    }
    p=text.find("/");
    if (p==std::string::npos)
    { // just a triangle index and a texture coord index:
        if (tt::getValidInt(text,vt))
            return(1);
        return(-1);
    }
    // a triangle index, a texture coord index and a normal index:
    subt.assign(text.begin(),text.begin()+p);
    text.erase(text.begin(),text.begin()+p+1);
    if (!tt::getValidInt(subt,vt))
        return(-1);
    if (!tt::getValidInt(text,vn))
        return(-1);
    return(3);
}

bool CObjFile::exportFunc(std::string fileName,bool poly,const std::vector<std::vector<float>*>& vertices,const std::vector<std::vector<int>*>& indices,const std::vector<std::string>& names)
{   
    VFile myFile(fileName.c_str(),VFile::CREATE_WRITE|VFile::SHARE_EXCLUSIVE);
    VArchive archive(&myFile,VArchive::STORE);
    int initSize=0;
    int verticesCount=0;
    for (int i=0;i<int(vertices.size());i++)
    {
        C3Vector v;
        for (int j=0;j<int(vertices[i]->size())/3;j++)
        {
            v(0)=(*vertices[i])[3*j+0];
            v(1)=(*vertices[i])[3*j+1];
            v(2)=(*vertices[i])[3*j+2];
            _writeVertex(archive,v);
            verticesCount++;
        }
        std::string theName("g ");
        theName.append(names[i]);
        archive.writeLine(theName);
        for (int j=0;j<int(indices[i]->size())/3;j++)
        {
            _writeIndex(archive,(*indices[i])[3*j+0]+1+initSize,
                (*indices[i])[3*j+1]+1+initSize,
                (*indices[i])[3*j+2]+1+initSize);
        }
        initSize=initSize+vertices[i]->size()/3;
    }
    archive.close();
    myFile.close();
    return(true);
}

bool CObjFile::exportFunc(std::string fileName,bool poly,std::vector<int>* sel)
{   
    VFile myFile(fileName.c_str(),VFile::CREATE_WRITE|VFile::SHARE_EXCLUSIVE);
    VArchive archive(&myFile,VArchive::STORE);
    std::vector<int> objects(*sel);
    // Check number of vertices for the progress dialog
    int totalVertices=0;
    int verticesCount=0;
    for (int i=0;i<int(objects.size());i++)
    {
        CShape* it=App::ct->objCont->getShape(objects[i]);
        if (it!=NULL)
        {
            std::vector<float> wvert;
            it->geomData->geomInfo->getCumulativeMeshes(wvert,NULL,NULL);
            totalVertices=totalVertices+wvert.size()/3;
        }
    }
    int initSize=0;
    for (int i=0;i<int(objects.size());i++)
    {
        C3DObject* it=App::ct->objCont->getObject(objects[i]);
        if (it->getObjectType()==sim_object_shape_type)
        {
            CShape* sh=(CShape*)it;
            C7Vector m(sh->getCumulativeTransformationPart1());
            C3Vector v;
            std::vector<float> wvert;
            std::vector<int> wind;
            sh->geomData->geomInfo->getCumulativeMeshes(wvert,&wind,NULL);
            for (int j=0;j<int(wvert.size())/3;j++)
            {
                v(0)=wvert[3*j+0];
                v(1)=wvert[3*j+1];
                v(2)=wvert[3*j+2];
                v=m*v;
                _writeVertex(archive,v);
                verticesCount++;
            }
            std::string theName("g ");
            theName.append(it->getName());
            archive.writeLine(theName);
            for (int j=0;j<int(wind.size())/3;j++)
            {
                _writeIndex(archive,wind[3*j+0]+1+initSize,
                    wind[3*j+1]+1+initSize,
                    wind[3*j+2]+1+initSize);
            }
            initSize=initSize+wvert.size()/3;
        }
    }
    archive.close();
    myFile.close();
    return(true);
}

void CObjFile::_appendMaterialFileToMaterialList(const char* matFile,std::vector<SObjMaterial>& materialList)
{
    if (VFile::doesFileExist(matFile))
    {
        try
        {
            VFile file(matFile,VFile::READ|VFile::SHARE_DENY_NONE);
            VArchive archive(&file,VArchive::LOAD);
            unsigned int currentPos=0;

            std::string matCode,matValue;
            SObjMaterial* currentMat=NULL;
            while (_getMaterialCode(archive,currentPos,matCode,matValue))
            {
                tt::removeSpacesAtBeginningAndEnd(matValue);
                if (matCode=="newmtl")
                {
                    SObjMaterial mat;
                    mat.materialName=matValue;
                    for (int i=0;i<3;i++)
                    {
                        mat.ambient[i]=0.25f;
                        mat.diffuse[i]=0.25f;
                        mat.specular[i]=0.25f;
                        mat.emission[i]=0.0f;
                    }
                    mat.transparency=0.0f;
                    mat.shininess=0.37f;
                    mat.textureFile="";
                    materialList.push_back(mat);
                    currentMat=&materialList[materialList.size()-1];
                }
                else if (((matCode=="Ka")||(matCode=="Kd")||(matCode=="Ks")||(matCode=="Ke"))&&(currentMat!=NULL))
                {
                    for (int i=0;i<3;i++)
                    {
                        std::string aWord;
                        if (tt::extractSpaceSeparatedWord(matValue,aWord))
                        {
                            tt::removeSpacesAtBeginningAndEnd(matValue);
                            float fv;
                            if (tt::getValidFloat(aWord,fv))
                            {
                                float* comp=NULL;
                                if (matCode=="Ka")
                                    comp=currentMat->ambient;
                                if (matCode=="Kd")
                                    comp=currentMat->diffuse;
                                if (matCode=="Ks")
                                    comp=currentMat->specular;
                                if (matCode=="Ke")
                                    comp=currentMat->emission;
                                if (comp!=NULL)
                                {
                                    comp[i]=fv;
                                    for (int j=i+1;j<3;j++)
                                        comp[j]=fv; // sometimes, only the first value is specified. In that case, this means: the rest of the values are the same!
                                }
                            }
                        }
                        else
                            break;
                    }
                }
                else if ((matCode=="map_Ka")&&(currentMat!=NULL))
                    currentMat->textureFile=matValue;
                else if ((matCode=="map_Kd")&&(currentMat!=NULL))
                    currentMat->textureFile=matValue;
                else if ((matCode=="Tr")&&(currentMat!=NULL))
                    tt::getValidFloat(matValue,currentMat->transparency);
                else if ((matCode=="Ns")&&(currentMat!=NULL))
                {
                    if (tt::getValidFloat(matValue,currentMat->shininess))
                    {
                        currentMat->shininess/=200.0f; // some say it varies betweem 0-100, others say between 0-200, yet some others say it varies between 0-1000!
                        tt::limitValue(0.0f,1.0f,currentMat->shininess);
                    }
                }
            }

            archive.close();
            file.close();
        }
        catch(VFILE_EXCEPTION_TYPE e)
        {
            VFile::reportAndHandleFileExceptionError(e);
        }
    }
}
