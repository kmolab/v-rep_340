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
#include "modelListWidget.h"
#include "vFileFinder.h"
#include "imgLoaderSaver.h"
#include "app.h"
#include <QMimeData>
#include <QScrollBar>

CModelListWidget::CModelListWidget() : CModelListWidgetBase()
{
    setViewMode(QListView::IconMode);
    setGridSize(QSize(160,160));
    setIconSize(QSize(128,128));

    setWrapping(true);
    setFlow(QListView::LeftToRight);
    setMovement(QListView::Static);
    setResizeMode(QListView::Adjust);

    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    viewport()->setAcceptDrops(false);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragOnly);
    setWordWrap(true);
    setTextElideMode(Qt::ElideNone);

//    setStyleSheet(QString("QListWidget:item:selected:active { background: transparent;} "));

    connect(this,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(onItemClicked(QListWidgetItem*)));
}

CModelListWidget::~CModelListWidget()
{
    clearAll();
}

void CModelListWidget::clearAll()
{
    for (size_t i=0;i<_allThumbnailsInfo.size();i++)
        delete _allThumbnailsInfo[i].thumbnail;
    _allThumbnailsInfo.clear();
    _folderPath="";
    clear();
}

void CModelListWidget::addThumbnail(CThumbnail* thumbN,const char* nameWithExtension,unsigned int creationTime,unsigned char modelOrFolder,bool validFileformat,C7Vector* optionalModelTr,C3Vector* optionalModelBoundingBoxSize,float* optionalModelNonDefaultTranslationStepSize)
{
    SModelThumbnailInfo info;
    info.thumbnail=thumbN;
    info.nameWithExtension=nameWithExtension;
    info.creationTime=creationTime;
    info.modelOrFolder=modelOrFolder;
    info.validFileFormat=validFileformat;
    if (optionalModelTr)
        info.modelTr=optionalModelTr[0];
    else
        info.modelTr.setIdentity();
    if (optionalModelBoundingBoxSize)
        info.modelBoundingBoxSize=optionalModelBoundingBoxSize[0];
    else
        info.modelBoundingBoxSize.clear();
    if (optionalModelNonDefaultTranslationStepSize)
        info.modelNonDefaultTranslationStepSize=optionalModelNonDefaultTranslationStepSize[0];
    else
        info.modelNonDefaultTranslationStepSize=0.0;
    _allThumbnailsInfo.push_back(info);

    _addThumbnailItemToList(_allThumbnailsInfo.size()-1);
}

void CModelListWidget::_addThumbnailItemToList(int index)
{
    QListWidgetItem* item=new QListWidgetItem(_allThumbnailsInfo[index].nameWithExtension.c_str());
    if (_allThumbnailsInfo[index].modelOrFolder==1)
    {
        const char* rgba=_allThumbnailsInfo[index].thumbnail->getPointerToUncompressedImage();
        QImage img(128,128,QImage::Format_ARGB32);
        QRgb value;
        for (int i=0;i<128;i++)
        {
            for (int j=0;j<128;j++)
            {
                char r=rgba[(i*128+j)*4+0];
                char g=rgba[(i*128+j)*4+1];
                char b=rgba[(i*128+j)*4+2];
                char a=rgba[(i*128+j)*4+3];
                value=qRgba(r,g,b,a);
                img.setPixel(j,127-i,value);
            }
        }
        QPixmap pimg;
        pimg.convertFromImage(img);
        item->setIcon(*(new QIcon(pimg)));
    }
    else
        item->setIcon(*(new QIcon(":/variousImageFiles/128x128folder.png")));
    item->setData(Qt::UserRole,index);
    insertItem(index,item);
}

CThumbnail* CModelListWidget::loadModelThumbnail(const char* pathAndFilename,int& result,C7Vector& modelTr,C3Vector& modelBoundingBoxSize,float& modelNonDefaultTranslationStepSize)
{ // result: -1=model not recognized, 0=model has no thumbnail, 1=model has thumbnail
    result=-1;
    CThumbnail* retThumbnail=NULL;
    if (VFile::doesFileExist(pathAndFilename))
    {
        VFile file(pathAndFilename,VFile::READ|VFile::SHARE_DENY_NONE);
        VArchive archive(&file,VArchive::LOAD);
        CSer serObj(archive);

        int serializationVersion;
        unsigned short vrepVersionThatWroteThis;
        int licenseTypeThatWroteThis;
        char revisionNumber;
        result=serObj.readOpen(serializationVersion,vrepVersionThatWroteThis,licenseTypeThatWroteThis,revisionNumber);
        if (result==1)
        {
            result=0;

            CThumbnail* thumbO=App::ct->environment->modelThumbnail_notSerializedHere.copyYourself();
            modelTr.setIdentity();
            modelBoundingBoxSize.clear();
            modelNonDefaultTranslationStepSize=0.0;
            App::ct->objCont->loadModel(serObj,true,&modelTr,&modelBoundingBoxSize,&modelNonDefaultTranslationStepSize);
            retThumbnail=App::ct->environment->modelThumbnail_notSerializedHere.copyYourself();
            App::ct->environment->modelThumbnail_notSerializedHere.copyFrom(thumbO);
            delete thumbO;
            if (retThumbnail->getPointerToUncompressedImage()!=NULL)
                result=1;
            serObj.readClose();
        }
        archive.close();
        file.close();
    }
    return(retThumbnail);
}

void CModelListWidget::setFolder(const char* folderPath)
{
    clearAll();
    std::vector<int> initialSelection;
    App::ct->objCont->getSelectedObjects(initialSelection);
    if (folderPath!=NULL)
    {
        _folderPath=folderPath;
        // 1. Get all files and their info from the folder:
        std::vector<std::string> allModelNames;
        std::vector<unsigned int> allModelCreationTimes;
        std::vector<unsigned char> allModelOrFolder; // 1=model, 0=folder

        VFileFinder finder;
        finder.searchFilesOrFolders(folderPath);
        int index=0;
        SFileOrFolder* foundItem=finder.getFoundItem(index++);
        while (foundItem!=NULL)
        {
            if (foundItem->isFile)
            { // Files
                std::string filename(foundItem->name);
                int l=int(filename.length());
                char extt[4]=VREP_MODEL_EXTENSION;
                bool cond=false;
                if ( cond || ((l>=5)&&(filename[l-4]=='.')&&(filename[l-3]==extt[0])&&(filename[l-2]==extt[1])&&(filename[l-1]==extt[2])) )
                { // above condition is somehow needed, otherwise file "*.ttmt" or similar are also retrieved!
                    allModelNames.push_back(filename);
                    allModelCreationTimes.push_back((unsigned int)foundItem->lastWriteTime);
                    allModelOrFolder.push_back(1); // this is a model
                }
            }
            else
            { // Added on 28/05/2011 to accomodate for folder thumbnail display
                std::string filename(foundItem->name);
                if ( (filename!=".")&&(filename!="..") )
                { // We don't wanna the . and .. folders
                    allModelNames.push_back(filename);
                    allModelCreationTimes.push_back((unsigned int)foundItem->lastWriteTime);
                    allModelOrFolder.push_back(0); // this is a folder
                }
            }
            foundItem=finder.getFoundItem(index++);
        }
        // 2. Check if a thumbnail file exists:
        clearAll();
        std::string thmbFile(folderPath);
        thmbFile+=VREP_SLASH;
        thmbFile+=VREP_MODEL_THUMBNAILFILE_NAME;
        bool thumbnailFileExistsAndWasLoaded=false;
        if (VFile::doesFileExist(thmbFile))
        {
            VFile file(thmbFile.c_str(),VFile::READ|VFile::SHARE_DENY_NONE);
            VArchive archive(&file,VArchive::LOAD);
            CSer serObj(archive);
            int serializationVersion;
            unsigned short vrepVersionThatWroteThis;
            int licenseTypeThatWroteThis;
            char revisionNumber;
            int result=serObj.readOpen(serializationVersion,vrepVersionThatWroteThis,licenseTypeThatWroteThis,revisionNumber);
            if (result==1)
            {
                thumbnailFileExistsAndWasLoaded=true;
                serializePart1(serObj);
                // a. do we have the same directory as written in the file?
                if (_folderPath.compare(folderPath)!=0)
                    thumbnailFileExistsAndWasLoaded=false;
                // b. do we have the same number of files?
                if (_allThumbnailsInfo.size()!=allModelNames.size())
                    thumbnailFileExistsAndWasLoaded=false;
                else
                { // we have the same number of files. Check if the names and last write times are same:
                    bool same=true;
                    for (int i=0;i<int(allModelNames.size());i++)
                    {
                        if (allModelNames[i].compare(_allThumbnailsInfo[i].nameWithExtension)!=0)
                        {
                            same=false;
                            break;
                        }
                        if (allModelCreationTimes[i]!=_allThumbnailsInfo[i].creationTime)
                        {
                            same=false;
                            break;
                        }
                        if (allModelOrFolder[i]!=_allThumbnailsInfo[i].modelOrFolder)
                        { // Check also if both are models or folders!
                            same=false;
                            break;
                        }
                    }
                    if (same)
                        serializePart2(serObj);
                    else
                        thumbnailFileExistsAndWasLoaded=false;
                }
                serObj.readClose();
                archive.close();
                file.close();
            }
        }
        // 3. Now load all thumbnails freshly (if needed):
        if ( (!thumbnailFileExistsAndWasLoaded)&&(allModelNames.size()!=0) )
        {
            clearAll();
            _folderPath=folderPath;
            for (int i=0;i<int(allModelNames.size());i++)
            {
                if (allModelOrFolder[i]==1)
                { // we have a model here
                    std::string nameAndPath(_folderPath);
                    nameAndPath+=VREP_SLASH;
                    nameAndPath+=allModelNames[i];
                    int result;
                    C7Vector modelTr;
                    C3Vector modelBBs;
                    float ndss;
                    CThumbnail* thumbnail=loadModelThumbnail(nameAndPath.c_str(),result,modelTr,modelBBs,ndss);
                    addThumbnail(thumbnail,allModelNames[i].c_str(),allModelCreationTimes[i],1,result>=0,&modelTr,&modelBBs,&ndss);
                }
                else
                { // we have a folder here!
                    int xres,yres;
                    bool rgba;
                    unsigned char* thumbnail=CImageLoaderSaver::loadQTgaImageData(":/targaFiles/128x128folder.tga",xres,yres,rgba,NULL);
                    CThumbnail* foldThumb=new CThumbnail();
                    foldThumb->setUncompressedThumbnailImage((char*)thumbnail,true,false);
                    delete[] thumbnail;
                    addThumbnail(foldThumb,allModelNames[i].c_str(),allModelCreationTimes[i],0,true,NULL,NULL,NULL);
                }
            }
            // 4. Serialize the thumbnail file for fast access in future:
            std::string thmbFile(_folderPath);
            thmbFile+=VREP_SLASH;
            thmbFile+=VREP_MODEL_THUMBNAILFILE_NAME;
            VFile myFile(thmbFile.c_str(),VFile::CREATE_WRITE|VFile::SHARE_EXCLUSIVE);
            VArchive archive(&myFile,VArchive::STORE);
            CSer serObj(archive);
            serObj.writeOpen();
            serializePart1(serObj);
            serializePart2(serObj);
            serObj.writeClose(App::userSettings->compressFiles);
            archive.close();
            myFile.close();
        }
    }
    // Now restore previous object selection state:
    App::ct->objCont->deselectObjects();
    for (size_t i=0;i<initialSelection.size();i++)
        App::ct->objCont->addObjectToSelection(initialSelection[i]);
}

void CModelListWidget::serializePart1(CSer& ar)
{
    if (ar.isStoring())
    { // Storing
        ar.storeDataName("Tfp");
        ar << _folderPath;
        ar.flush();

        ar.storeDataName("Tn5");
        ar << int(_allThumbnailsInfo.size());
        for (size_t i=0;i<_allThumbnailsInfo.size();i++)
        {
            ar << _allThumbnailsInfo[i].nameWithExtension;
            ar << _allThumbnailsInfo[i].creationTime;
            ar << _allThumbnailsInfo[i].modelOrFolder;
            ar << _allThumbnailsInfo[i].validFileFormat;
            for (size_t j=0;j<7;j++)
                ar << _allThumbnailsInfo[i].modelTr(j);
            for (size_t j=0;j<3;j++)
                ar << _allThumbnailsInfo[i].modelBoundingBoxSize(j);
            ar << _allThumbnailsInfo[i].modelNonDefaultTranslationStepSize;
        }
        ar.flush();

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
                if (theName.compare("Tfp")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    ar >> _folderPath;
                }
                if (theName.compare("Tnd")==0)
                { // For backward compatibility (28/05/2011)
                    noHit=false;
                    ar >> byteQuantity;
                    int thmbCnt;
                    ar >> thmbCnt;
                    _allThumbnailsInfo.resize(thmbCnt);
                    for (int i=0;i<thmbCnt;i++)
                    {
                        std::string dum;
                        ar >> dum;
                        _allThumbnailsInfo[i].nameWithExtension=dum;
                        unsigned int dum2;
                        ar >> dum2;
                        _allThumbnailsInfo[i].creationTime=dum2;
                        _allThumbnailsInfo[i].modelOrFolder=1; // 1=model
                        unsigned char dum3;
                        ar >> dum3;
                        _allThumbnailsInfo[i].validFileFormat=dum3;
                        _allThumbnailsInfo[i].modelTr.setIdentity();
                        _allThumbnailsInfo[i].modelBoundingBoxSize.clear();
                        _allThumbnailsInfo[i].modelNonDefaultTranslationStepSize=0.0;
                    }
                }
                if (theName.compare("Tn2")==0)
                { // For backward compatibility (29/12/2016)
                    noHit=false;
                    ar >> byteQuantity;
                    int thmbCnt;
                    ar >> thmbCnt;
                    _allThumbnailsInfo.resize(thmbCnt);
                    for (int i=0;i<thmbCnt;i++)
                    {
                        std::string dum;
                        ar >> dum;
                        _allThumbnailsInfo[i].nameWithExtension=dum;
                        unsigned int dum2;
                        ar >> dum2;
                        _allThumbnailsInfo[i].creationTime=dum2;
                        unsigned char dum3;
                        ar >> dum3;
                        _allThumbnailsInfo[i].modelOrFolder=dum3;
                        ar >> dum3;
                        _allThumbnailsInfo[i].validFileFormat=dum3;
                        _allThumbnailsInfo[i].modelTr.setIdentity();
                        _allThumbnailsInfo[i].modelBoundingBoxSize.clear();
                        _allThumbnailsInfo[i].modelNonDefaultTranslationStepSize=0.0;
                    }
                }
                if (theName.compare("Tn5")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    int thmbCnt;
                    ar >> thmbCnt;
                    _allThumbnailsInfo.resize(thmbCnt);
                    for (int i=0;i<thmbCnt;i++)
                    {
                        std::string dum;
                        ar >> dum;
                        _allThumbnailsInfo[i].nameWithExtension=dum;
                        unsigned int dum2;
                        ar >> dum2;
                        _allThumbnailsInfo[i].creationTime=dum2;
                        unsigned char dum3;
                        ar >> dum3;
                        _allThumbnailsInfo[i].modelOrFolder=dum3;
                        ar >> dum3;
                        _allThumbnailsInfo[i].validFileFormat=dum3;
                        for (size_t j=0;j<7;j++)
                            ar >> _allThumbnailsInfo[i].modelTr(j);
                        for (size_t j=0;j<3;j++)
                            ar >> _allThumbnailsInfo[i].modelBoundingBoxSize(j);
                        ar >> _allThumbnailsInfo[i].modelNonDefaultTranslationStepSize;
                    }
                }
                if (noHit)
                    ar.loadUnknownData();
            }
        }
    }
}

void CModelListWidget::serializePart2(CSer& ar)
{
    if (ar.isStoring())
    { // Storing
        for (size_t i=0;i<_allThumbnailsInfo.size();i++)
        {
            ar.storeDataName("Tc2");
            ar.setCountingMode();
            _allThumbnailsInfo[i].thumbnail->serialize(ar);
            if (ar.setWritingMode())
                _allThumbnailsInfo[i].thumbnail->serialize(ar);
        }

        ar.storeDataName(SER_END_OF_OBJECT);
    }
    else
    {       // Loading
        int byteQuantity;
        std::string theName="";
        char* compressedImage=new char[128*64*3];
        int thumbIndex=0;
        while (theName.compare(SER_END_OF_OBJECT)!=0)
        {
            theName=ar.readDataName();
            if (theName.compare(SER_END_OF_OBJECT)!=0)
            {
                bool noHit=true;
                if (theName.compare("Tci")==0)
                { // for backward compatibility! (serialization version 15 sometime!)
                    noHit=false;
                    ar >> byteQuantity;
                    unsigned char imagePresent;
                    ar >> imagePresent;
                    char dum;
                    CThumbnail* it=new CThumbnail();
                    if (imagePresent!=0)
                    {
                        for (int j=0;j<128*64*3;j++)
                        {
                            ar >> dum;
                            compressedImage[j]=dum;
                        }
                        it->setCompressedThumbnailImage(compressedImage);
                    }
                    _allThumbnailsInfo[thumbIndex++].thumbnail=it;
                    _addThumbnailItemToList(thumbIndex-1);
                }
                if (theName.compare("Tc2")==0)
                {
                    noHit=false;
                    ar >> byteQuantity;
                    CThumbnail* it=new CThumbnail();
                    it->serialize(ar);
                    _allThumbnailsInfo[thumbIndex++].thumbnail=it;
                    _addThumbnailItemToList(thumbIndex-1);
                }
                if (noHit)
                    ar.loadUnknownData();
            }
        }
        delete[] compressedImage;
    }
}

QMimeData* CModelListWidget::mimeData(const QList<QListWidgetItem *> items) const
{
    if (items.size()!=1)
        return(NULL);
    QListWidgetItem* item=items[0];
    int index=item->data(Qt::UserRole).toInt();
    QMimeData* data=new QMimeData();
    SModelThumbnailInfo* info=(SModelThumbnailInfo*)&_allThumbnailsInfo[index];
    data->setText(info->nameWithExtension.c_str());
    info->modelPathAndNameWithExtension=_folderPath;
    info->modelPathAndNameWithExtension+="/";
    info->modelPathAndNameWithExtension+=info->nameWithExtension;
    return(data);
}

SModelThumbnailInfo* CModelListWidget::getThumbnailInfoFromModelName(const char* nameWithExtension,int* index)
{
    for (size_t i=0;i<_allThumbnailsInfo.size();i++)
    {
        if (_allThumbnailsInfo[i].nameWithExtension.compare(nameWithExtension)==0)
        {
            if (index!=NULL)
                index[0]=i;
            return(&_allThumbnailsInfo[i]);
        }
    }
    if (index!=NULL)
        index[0]=-1;
    return(NULL);
}

void CModelListWidget::onItemClicked(QListWidgetItem* item)
{
    int index=item->data(Qt::UserRole).toInt();
    if (_allThumbnailsInfo[index].modelOrFolder==0)
    {
        _folderPath+="/";
        _folderPath+=_allThumbnailsInfo[index].nameWithExtension;
        App::mainWindow->modelFolderWidget->selectFolder(_folderPath.c_str());
    }
}
