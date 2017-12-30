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
#include "modelFolderWidget.h"
#include "app.h"
#include "vFileFinder.h"
#include "v_repStrings.h"
#include "tt.h"

CModelFolderWidget::CModelFolderWidget(CModelListWidget* modelListWidget,const char* folderName,const char* folderPath,const char* nameOfSelectedFolder) : QTreeWidget()
{
    _hasError=false;
    _modelListWidget=modelListWidget;
    setColumnCount(1);
    setAnimated(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    std::string st="QTreeView::branch:has-siblings:!adjoins-item{border-image: url(:/variousImageFiles/iline.png) 0;}";
    st+="QTreeView::branch:has-siblings:adjoins-item{border-image: url(:/variousImageFiles/tline.png) 0;}";
    st+="QTreeView::branch:!has-children:!has-siblings:adjoins-item{border-image: url(:/variousImageFiles/lline.png) 0;}";
    st+="QTreeView::branch:has-children:!has-siblings:closed,QTreeView::branch:closed:has-children:has-siblings{border-image: none;image: url(:/variousImageFiles/plus.png);}";
    st+="QTreeView::branch:open:has-children:!has-siblings,QTreeView::branch:open:has-children:has-siblings{border-image: none;image: url(:/variousImageFiles/minus.png);}";
    st+="QTreeView::branch:selected{background: palette(base);}";
    setStyleSheet(st.c_str());
//    setHeaderHidden(true);

    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(onItemSelectionChanged()));
    if (VFile::doesFolderExist(folderPath))
    {
        setHeaderLabel(folderName);
        VFileFinder finder;
        finder.searchFolders(folderPath);
        std::vector<std::string> subFolders;
        std::vector<std::string> subNames;
        std::vector<int> indd;
        int index=0;
        SFileOrFolder* foundFolder=finder.getFoundItem(index++);
        while (foundFolder!=NULL)
        {
            if ( (foundFolder->name!=".")&&(foundFolder->name!="..") )
            {
                indd.push_back(int(subNames.size()));
                subNames.push_back(foundFolder->name);
                subFolders.push_back(foundFolder->path);
            }
            foundFolder=finder.getFoundItem(index++);
        }
        tt::orderStrings(subNames,indd);
        std::vector<std::string> _subFolders;
        for (size_t i=0;i<indd.size();i++)
            _subFolders.push_back(subFolders[indd[i]]);

/*
        QTreeWidgetItem* topLevelItem=new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("Model folder")));
        topLevelItem->setIcon(0,*(new QIcon(":/variousImageFiles/folder.png")));
        topLevelItem->setData(0,Qt::UserRole,QVariant(folderPath));
        insertTopLevelItem(0,topLevelItem);
        topLevelItem->setExpanded(true);
        */
        for (size_t i=0;i<_subFolders.size();i++)
        {
            QTreeWidgetItem* itemToSelect=NULL;
            QTreeWidgetItem* itm=_buildChild(&itemToSelect,subNames[i].c_str(),_subFolders[i].c_str(),nameOfSelectedFolder);
//            topLevelItem->addChild(itm);
            insertTopLevelItem((int)i,itm);
            if (itemToSelect!=NULL)
            {
                itm->setExpanded(true);
                itemToSelect->setSelected(true);
            }
        }
    }
    else
    {
        /*
        QTreeWidgetItem* topLevelItem=new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(IDSOGL_MODEL_FOLDER_NOT_FOUND)));
        topLevelItem->setData(0,Qt::UserRole,QVariant(folderPath));
        insertTopLevelItem(0,topLevelItem);
        */
        setHeaderLabel(IDSOGL_MODEL_FOLDER_NOT_FOUND);
        _hasError=true;
    }
}

CModelFolderWidget::~CModelFolderWidget()
{
}

QTreeWidgetItem* CModelFolderWidget::_buildChild(QTreeWidgetItem** itemToSelect,const char* folderName,const char* folderPath,const char* nameOfSelectedFolder) const
{
    QTreeWidgetItem* item=NULL;
    if (VFile::doesFolderExist(folderPath))
    {
        item=new QTreeWidgetItem((QTreeWidget*)0,QStringList(QString(folderName)));
        if (std::string(folderName).compare(nameOfSelectedFolder)==0)
            itemToSelect[0]=item;
        VFileFinder finder;
        finder.searchFolders(folderPath);
        std::vector<std::string> subFolders;
        std::vector<std::string> subNames;
        std::vector<int> indd;
        int index=0;
        SFileOrFolder* foundFolder=finder.getFoundItem(index++);
        while (foundFolder!=NULL)
        {
            if ( (foundFolder->name!=".")&&(foundFolder->name!="..") )
            {
                indd.push_back(int(subNames.size()));
                subNames.push_back(foundFolder->name);
                subFolders.push_back(foundFolder->path);
            }
            foundFolder=finder.getFoundItem(index++);
        }
        tt::orderStrings(subNames,indd);
        std::vector<std::string> _subFolders;
        for (size_t i=0;i<indd.size();i++)
            _subFolders.push_back(subFolders[indd[i]]);
        for (size_t i=0;i<_subFolders.size();i++)
        {
            QTreeWidgetItem* itemToSelect2=NULL;
            QTreeWidgetItem* itm=_buildChild(&itemToSelect2,subNames[i].c_str(),_subFolders[i].c_str(),nameOfSelectedFolder);
            item->insertChild((int)i,itm);
            if (itemToSelect2!=NULL)
            {
                itemToSelect[0]=itemToSelect2;
                itm->setExpanded(true);
            }
        }
    }
    else
        item=new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("ERROR")));
    item->setIcon(0,*(new QIcon(":/variousImageFiles/folder.png")));
    item->setData(0,Qt::UserRole,QVariant(folderPath));
    return(item);
}

void CModelFolderWidget::onItemSelectionChanged()
{
    const QList<QTreeWidgetItem*> sel=selectedItems();
    if (sel.size()!=0)
    {
        QTreeWidgetItem* item=sel[0];
        QVariant path(item->data(0,Qt::UserRole));
        _modelListWidget->setFolder(path.toString().toStdString().c_str());
    }
}

bool CModelFolderWidget::hasError() const
{
    return(_hasError);
}

void CModelFolderWidget::selectFolder(const char* folderPath)
{
    clearSelection();
    const QList<QTreeWidgetItem*> items=findItems("*",Qt::MatchRecursive|Qt::MatchWildcard);
    for (int i=0;i<items.size();i++)
    {
        QTreeWidgetItem* item=items[i];
        QVariant path(item->data(0,Qt::UserRole));
        if (path.toString().compare(folderPath)==0)
        {
            bool first=true;
            while (true)
            {
                item->setSelected(first);
                item->setExpanded(true);
                item=item->parent();
                first=false;
                if (item==NULL)
                    break;
            }
        }
    }
}
