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
#include "qdlgentityvsentityselection.h"
#include "ui_qdlgentityvsentityselection.h"
#include "tt.h"
#include "v_repStrings.h"
#include "app.h"
#include "vMessageBox.h"

CQDlgEntityVsEntitySelection::CQDlgEntityVsEntitySelection(QWidget *parent) :
    VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgEntityVsEntitySelection)
{
    ui->setupUi(this);
}

CQDlgEntityVsEntitySelection::~CQDlgEntityVsEntitySelection()
{
    delete ui;
}

void CQDlgEntityVsEntitySelection::cancelEvent()
{
    defaultModalDialogEndRoutine(false);
}

void CQDlgEntityVsEntitySelection::okEvent()
{
    if (checkSelectionValidity())
        defaultModalDialogEndRoutine(true);
}

void CQDlgEntityVsEntitySelection::initialize(int theMode)
{
    mode=theMode;
    if (theMode==0)
    { // collisions
        setWindowTitle(strTranslate(IDSN_COLLISION_OBJECT_SELECTION_OF_ENTITIES));
        fillComboWithCollisionEntities(ui->qqEntity1,false);
        fillComboWithCollisionEntities(ui->qqEntity2,true);
    }
    if ((theMode==1)||(theMode==2))
    { // distances
        if (theMode==1)
            setWindowTitle(strTranslate(IDSN_DISTANCE_OBJECT_SELECTION_OF_ENTITIES));
        if (theMode==2)
            setWindowTitle(strTranslate(IDSN_OBSTACLE_AVOIDANCE_SELECTION_OF_ENTITIES));
        fillComboWithDistanceEntities(ui->qqEntity1,false);
        fillComboWithDistanceEntities(ui->qqEntity2,true);
    }
}

void CQDlgEntityVsEntitySelection::on_qqOkCancel_accepted()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        if (checkSelectionValidity())
            defaultModalDialogEndRoutine(true);
    }
}

void CQDlgEntityVsEntitySelection::on_qqOkCancel_rejected()
{
    IF_UI_EVENT_CAN_READ_DATA
    {
        defaultModalDialogEndRoutine(false);
    }
}

void CQDlgEntityVsEntitySelection::fillComboWithCollisionEntities(QComboBox* combo,bool insertAllAtBeginning)
{
    combo->clear();
    if (insertAllAtBeginning)
        combo->addItem(strTranslate(IDS_ALL_OTHER_COLLIDABLE_OBJECTS_IN_SCENE),QVariant(-1));

    std::vector<std::string> names;
    std::vector<int> ids;

    // Now collections:
    for (int i=0;i<int(App::ct->collections->allCollections.size());i++)
    {
        CRegCollection* it=App::ct->collections->allCollections[i];
        std::string name(tt::decorateString("[",strTranslate(IDSN_COLLECTION),"] "));
        name+=it->getCollectionName();
        names.push_back(name);
        ids.push_back(it->getCollectionID());
    }
    tt::orderStrings(names,ids);
    for (int i=0;i<int(names.size());i++)
        combo->addItem(names[i].c_str(),QVariant(ids[i]));

    names.clear();
    ids.clear();

    // Now shapes:
    for (size_t i=0;i<App::ct->objCont->shapeList.size();i++)
    {
        CShape* it=App::ct->objCont->getShape(App::ct->objCont->shapeList[i]);
        std::string name(tt::decorateString("[",strTranslate(IDSN_SHAPE),"] "));
        name+=it->getName();
        names.push_back(name);
        ids.push_back(it->getID());
    }
    tt::orderStrings(names,ids);
    for (size_t i=0;i<names.size();i++)
        combo->addItem(names[i].c_str(),QVariant(ids[i]));

    names.clear();
    ids.clear();

    // Now Octrees:
    for (size_t i=0;i<App::ct->objCont->octreeList.size();i++)
    {
        COctree* it=App::ct->objCont->getOctree(App::ct->objCont->octreeList[i]);
        std::string name(tt::decorateString("[",strTranslate(IDSN_OCTREE),"] "));
        name+=it->getName();
        names.push_back(name);
        ids.push_back(it->getID());
    }
    tt::orderStrings(names,ids);
    for (size_t i=0;i<names.size();i++)
        combo->addItem(names[i].c_str(),QVariant(ids[i]));

    names.clear();
    ids.clear();

    // Now Point clouds:
    for (size_t i=0;i<App::ct->objCont->pointCloudList.size();i++)
    {
        CPointCloud* it=App::ct->objCont->getPointCloud(App::ct->objCont->pointCloudList[i]);
        std::string name(tt::decorateString("[",strTranslate(IDSN_POINTCLOUD),"] "));
        name+=it->getName();
        names.push_back(name);
        ids.push_back(it->getID());
    }
    tt::orderStrings(names,ids);
    for (size_t i=0;i<names.size();i++)
        combo->addItem(names[i].c_str(),QVariant(ids[i]));

    names.clear();
    ids.clear();

    // Now dummies:
    for (size_t i=0;i<App::ct->objCont->dummyList.size();i++)
    {
        CDummy* it=App::ct->objCont->getDummy(App::ct->objCont->dummyList[i]);
        std::string name(tt::decorateString("[",strTranslate(IDSN_DUMMY),"] "));
        name+=it->getName();
        names.push_back(name);
        ids.push_back(it->getID());
    }
    tt::orderStrings(names,ids);
    for (size_t i=0;i<names.size();i++)
        combo->addItem(names[i].c_str(),QVariant(ids[i]));

    names.clear();
    ids.clear();
}

void CQDlgEntityVsEntitySelection::fillComboWithDistanceEntities(QComboBox* combo,bool insertAllAtBeginning)
{
    combo->clear();
    if (insertAllAtBeginning)
        combo->addItem(strTranslate(IDS_ALL_OTHER_MEASURABLE_OBJECTS_IN_SCENE),QVariant(-1));

    std::vector<std::string> names;
    std::vector<int> ids;

    // Now collections:
    for (int i=0;i<int(App::ct->collections->allCollections.size());i++)
    {
        CRegCollection* it=App::ct->collections->allCollections[i];
        std::string name(tt::decorateString("[",strTranslate(IDSN_COLLECTION),"] "));
        name+=it->getCollectionName();
        names.push_back(name);
        ids.push_back(it->getCollectionID());
    }
    tt::orderStrings(names,ids);
    for (int i=0;i<int(names.size());i++)
        combo->addItem(names[i].c_str(),QVariant(ids[i]));

    names.clear();
    ids.clear();

    // Now shapes:
    for (int i=0;i<int(App::ct->objCont->shapeList.size());i++)
    {
        CShape* it=App::ct->objCont->getShape(App::ct->objCont->shapeList[i]);
        std::string name(tt::decorateString("[",strTranslate(IDSN_SHAPE),"] "));
        name+=it->getName();
        names.push_back(name);
        ids.push_back(it->getID());
    }
    tt::orderStrings(names,ids);
    for (int i=0;i<int(names.size());i++)
        combo->addItem(names[i].c_str(),QVariant(ids[i]));

    names.clear();
    ids.clear();

    // Now Octrees:
    for (size_t i=0;i<App::ct->objCont->octreeList.size();i++)
    {
        COctree* it=App::ct->objCont->getOctree(App::ct->objCont->octreeList[i]);
        std::string name(tt::decorateString("[",strTranslate(IDSN_OCTREE),"] "));
        name+=it->getName();
        names.push_back(name);
        ids.push_back(it->getID());
    }
    tt::orderStrings(names,ids);
    for (size_t i=0;i<names.size();i++)
        combo->addItem(names[i].c_str(),QVariant(ids[i]));

    names.clear();
    ids.clear();

    // Now Point clouds:
    for (size_t i=0;i<App::ct->objCont->pointCloudList.size();i++)
    {
        CPointCloud* it=App::ct->objCont->getPointCloud(App::ct->objCont->pointCloudList[i]);
        std::string name(tt::decorateString("[",strTranslate(IDSN_POINTCLOUD),"] "));
        name+=it->getName();
        names.push_back(name);
        ids.push_back(it->getID());
    }
    tt::orderStrings(names,ids);
    for (size_t i=0;i<names.size();i++)
        combo->addItem(names[i].c_str(),QVariant(ids[i]));

    names.clear();
    ids.clear();

    // Now dummies:
    for (size_t i=0;i<App::ct->objCont->dummyList.size();i++)
    {
        CDummy* it=App::ct->objCont->getDummy(App::ct->objCont->dummyList[i]);
        std::string name(tt::decorateString("[",strTranslate(IDSN_DUMMY),"] "));
        name+=it->getName();
        names.push_back(name);
        ids.push_back(it->getID());
    }
    tt::orderStrings(names,ids);
    for (size_t i=0;i<names.size();i++)
        combo->addItem(names[i].c_str(),QVariant(ids[i]));

    names.clear();
    ids.clear();
}

bool CQDlgEntityVsEntitySelection::checkSelectionValidity()
{
    entity1=ui->qqEntity1->itemData(ui->qqEntity1->currentIndex()).toInt();
    entity2=ui->qqEntity2->itemData(ui->qqEntity2->currentIndex()).toInt();
    if ((entity1==entity2)&&(entity1<=SIM_IDEND_3DOBJECT))
    { // error, we cannot check against itself if object
        App::uiThread->messageBox_warning(this,IDSN_COLLISION_DISTANCE_OBJECT,IDS_CANNOT_CHECK_OBJECT_AGAINST_ITSELF,VMESSAGEBOX_OKELI);
        return(false);
    }
    if (mode==0)
    {
        // We check if such an object already exists:
        for (size_t i=0;i<App::ct->collisions->collisionObjects.size();i++)
        {
            if (App::ct->collisions->collisionObjects[i]->isSame(entity1,entity2))
            {
                App::uiThread->messageBox_warning(this,IDSN_COLLISION_DISTANCE_OBJECT,IDS_SIMILAR_OBJECT_ALREADY_EXISTS,VMESSAGEBOX_OKELI);
                return(false);
            }
        }

        // Now check if the combination is valid:
        bool invalidCombination=false;
        if ( (entity1<SIM_IDSTART_COLLECTION)&&(entity2<SIM_IDSTART_COLLECTION) )
        {
            int t1=App::ct->objCont->getObject(entity1)->getObjectType();
            int t2=sim_object_octree_type;
            if (entity2!=-1)
                t2=App::ct->objCont->getObject(entity2)->getObjectType();
            if (t1==sim_object_shape_type)
            {
                if ( (t2!=sim_object_shape_type)&&(t2!=sim_object_octree_type) )
                    invalidCombination=true;
            }
            if (t1==sim_object_octree_type)
            {
                if ( (t2!=sim_object_shape_type)&&(t2!=sim_object_octree_type)&&(t2!=sim_object_pointcloud_type)&&(t2!=sim_object_dummy_type) )
                    invalidCombination=true;
            }
            if ( (t1==sim_object_pointcloud_type)||(t1==sim_object_dummy_type) )
            {
                if (t2!=sim_object_octree_type)
                    invalidCombination=true;
            }
        }
        if (invalidCombination)
        {
            App::uiThread->messageBox_warning(this,IDSN_COLLISION_DISTANCE_OBJECT,IDS_INVALID_COLLISION_ITEM_COMBINATION,VMESSAGEBOX_OKELI);
            return(false);
        }

        bool displayWarning=false;
        if ((entity1<=SIM_IDEND_3DOBJECT)&&(entity1>=SIM_IDSTART_3DOBJECT))
        {
            C3DObject* it=App::ct->objCont->getObject(entity1);
            if (it!=NULL)
                displayWarning|=((it->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_collidable)==0);
        }
        if ((entity2<=SIM_IDEND_3DOBJECT)&&(entity2>=SIM_IDSTART_3DOBJECT))
        {
            C3DObject* it=App::ct->objCont->getObject(entity2);
            if (it!=NULL)
                displayWarning|=((it->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_collidable)==0);
        }
        if (displayWarning)
            App::uiThread->messageBox_warning(this,IDSN_COLLISION_OBJECT,IDS_OBJECTS_NOT_COLLIDABLE_WARNING,VMESSAGEBOX_OKELI);
    }
    if ((mode==1)||(mode==2))
    {
        if (mode==1)
        {
            // We check if such an object already exists:
            for (int i=0;i<int(App::ct->distances->distanceObjects.size());i++)
            {
                if (App::ct->distances->distanceObjects[i]->isSame(entity1,entity2))
                {
                    App::uiThread->messageBox_warning(this,strTranslate(IDSN_COLLISION_DISTANCE_OBJECT),strTranslate(IDS_SIMILAR_OBJECT_ALREADY_EXISTS),VMESSAGEBOX_OKELI);
                    return(false);
                }
            }
        }


        // Now check if the combination is valid:
        bool invalidCombination=false;
        if ( (entity1<SIM_IDSTART_COLLECTION)&&(entity2<SIM_IDSTART_COLLECTION) )
        {
            int t1=App::ct->objCont->getObject(entity1)->getObjectType();
            int t2=sim_object_octree_type;
            if (entity2!=-1)
                t2=App::ct->objCont->getObject(entity2)->getObjectType();
            if ( (t1==sim_object_shape_type)||(t1==sim_object_octree_type)||(t1==sim_object_pointcloud_type)||(t1==sim_object_dummy_type) )
            {
                if ( (t2!=sim_object_shape_type)&&(t2!=sim_object_octree_type)&&(t2!=sim_object_pointcloud_type)&&(t2!=sim_object_dummy_type) )
                    invalidCombination=true;
            }
            else
                invalidCombination=true;
        }
        if (invalidCombination)
        {
            App::uiThread->messageBox_warning(this,IDSN_COLLISION_DISTANCE_OBJECT,IDS_INVALID_DISTANCE_ITEM_COMBINATION,VMESSAGEBOX_OKELI);
            return(false);
        }

        bool displayWarning=false;
        if ((entity1<=SIM_IDEND_3DOBJECT)&&(entity1>=SIM_IDSTART_3DOBJECT))
        {
            C3DObject* it=App::ct->objCont->getObject(entity1);
            if (it!=NULL)
                displayWarning|=((it->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_measurable)==0);
        }
        if ((entity2<=SIM_IDEND_3DOBJECT)&&(entity2>=SIM_IDSTART_3DOBJECT))
        {
            C3DObject* it=App::ct->objCont->getObject(entity2);
            if (it!=NULL)
                displayWarning|=((it->getCumulativeObjectSpecialProperty()&sim_objectspecialproperty_measurable)==0);
        }
        if (displayWarning)
        {
            if (mode==1)
                App::uiThread->messageBox_warning(this,strTranslate(IDSN_DISTANCE_OBJECT),strTranslate(IDS_OBJECTS_NOT_MEASURABLE_WARNING),VMESSAGEBOX_OKELI);
            if (mode==2)
                App::uiThread->messageBox_warning(this,strTranslate(IDSN_AVOIDANCE_OBJECT),strTranslate(IDS_OBJECTS_NOT_MEASURABLE_WARNING),VMESSAGEBOX_OKELI);
        }
    }
    return(true);
}
