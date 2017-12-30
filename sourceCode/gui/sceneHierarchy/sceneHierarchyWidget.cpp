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
#include "sceneHierarchyWidget.h"
#include "app.h"

CSceneHierarchyWidget::CSceneHierarchyWidget() : QTreeWidget()
{
    setColumnCount(1);
    setAnimated(true);
    setSelectionMode(QAbstractItemView::MultiSelection);
    std::string st="QTreeView::branch:has-siblings:!adjoins-item{border-image: url(:/variousImageFiles/iline.png) 0;}";
    st+="QTreeView::branch:has-siblings:adjoins-item{border-image: url(:/variousImageFiles/tline.png) 0;}";
    st+="QTreeView::branch:!has-children:!has-siblings:adjoins-item{border-image: url(:/variousImageFiles/lline.png) 0;}";
    st+="QTreeView::branch:has-children:!has-siblings:closed,QTreeView::branch:closed:has-children:has-siblings{border-image: none;image: url(:/variousImageFiles/plus.png);}";
    st+="QTreeView::branch:open:has-children:!has-siblings,QTreeView::branch:open:has-children:has-siblings{border-image: none;image: url(:/variousImageFiles/minus.png);}";
    st+="QTreeView::branch:selected{background: palette(base);}";
    setStyleSheet(st.c_str());
//    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(onItemSelectionChanged()));
    static int a=0;
    setHeaderHidden(true);
//    setHeaderLabel(QString("Scene hierarchy %1").arg(a++));
}

void CSceneHierarchyWidget::rebuild()
{
    /*
    removeAll();
    for (size_t i=0;i<App::ct->objCont->orphanList.size();i++)
    {
        C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->orphanList[i]);
        QTreeWidgetItem* itm=_buildObjectWithHierarchy(it);
        insertTopLevelItem((int)i,itm);
        // itemToSelect->setSelected(true);
    }
    for (std::map<int,QTreeWidgetItem*>::iterator it=_allTreeItems.begin();it!=_allTreeItems.end();it++)
    {
        C3DObject* obj=App::ct->objCont->getObject(it->first);
        it->second->setExpanded((obj->getLocalObjectProperty()&sim_objectproperty_collapsed)==0);
    }
    */
}

CSceneHierarchyWidget::~CSceneHierarchyWidget()
{
    removeAll();
}

void CSceneHierarchyWidget::removeAll()
{
    clear();
    _allTreeItems.clear();
}

QTreeWidgetItem* CSceneHierarchyWidget::_buildObjectWithHierarchy(C3DObject* it)
{
    QTreeWidgetItem* item=new QTreeWidgetItem((QTreeWidget*)0,QStringList(it->getName().c_str()));
    _allTreeItems[it->getID()]=item;

    for (size_t i=0;i<it->childList.size();i++)
    {
        QTreeWidgetItem* child=_buildObjectWithHierarchy(it->childList[i]);
        item->insertChild((int)i,child);
    }
    return(item);
}
