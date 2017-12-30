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

#include "dummyRendering.h"

#ifdef SIM_WITH_OPENGL

void displayDummy(CDummy* dummy,CViewableBase* renderingObject,int displayAttrib)
{
    // At the beginning of every 3DObject display routine:
    _commonStart(dummy,renderingObject,displayAttrib);
    // Bounding box display:
    if (displayAttrib&sim_displayattribute_renderpass)
        _displayBoundingBox(dummy,displayAttrib,true,dummy->getSize()*2.0f);

    C3Vector normalVectorForLinesAndPoints(dummy->getCumulativeTransformation().Q.getInverse()*C3Vector::unitZVector);

    // Object display:
    if (dummy->getShouldObjectBeDisplayed(renderingObject->getID(),displayAttrib))
    {
        if ((App::getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
        {
            if (dummy->getLocalObjectProperty()&sim_objectproperty_selectmodelbaseinstead)
                glLoadName(dummy->getModelSelectionID());
            else
                glLoadName(dummy->getID());
        }
        else
            glLoadName(-1);

        if ( (displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass) )
            glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);
        if ( (displayAttrib&sim_displayattribute_forcewireframe)==0)
            glEnable(GL_CULL_FACE);

        _enableAuxClippingPlanes(dummy->getID());
        if ((displayAttrib&sim_displayattribute_selected)==0)
            ogl::drawReference(dummy->getSize()*2.0f,true,true,false,normalVectorForLinesAndPoints.data);
        if (displayAttrib&sim_displayattribute_dynamiccontentonly)
            ogl::setMaterialColor(0.0f,0.6f,0.6f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f);
        else
        {
            bool setOtherColor=(App::ct->collisions->getCollisionColor(dummy->getID())!=0);
            for (size_t i=0;i<App::ct->collections->allCollections.size();i++)
            {
                if (App::ct->collections->allCollections[i]->isObjectInCollection(dummy->getID()))
                    setOtherColor|=(App::ct->collisions->getCollisionColor(App::ct->collections->allCollections[i]->getCollectionID())!=0);
            }
            if (!setOtherColor)
                dummy->getColor()->makeCurrentColor((displayAttrib&sim_displayattribute_useauxcomponent)!=0);
            else
                App::ct->mainSettings->collisionColor.makeCurrentColor(false);
        }
        CDummy* linkedDummy=NULL;
        if (dummy->getLinkedDummyID()!=-1)
            linkedDummy=App::ct->objCont->getDummy(dummy->getLinkedDummyID());
        ogl::drawSphere(dummy->getSize()/2.0f,12,6,false);
        glDisable(GL_CULL_FACE);
        if (linkedDummy!=NULL)
        {
            C7Vector cumulBase(dummy->getCumulativeTransformation());
            C7Vector cumulMobile(linkedDummy->getCumulativeTransformation());
            cumulMobile=cumulBase.getInverse()*cumulMobile;
            ogl::setMaterialColor(ogl::colorBlack,ogl::colorBlack,ogl::colorBlack);
            int _linkType=dummy->getLinkType();
            if ((_linkType==sim_dummy_linktype_gcs_loop_closure)||(_linkType==sim_dummy_linktype_gcs_tip)||(_linkType==sim_dummy_linktype_gcs_target))
                ogl::setMaterialColor(sim_colorcomponent_ambient_diffuse,ogl::colorGreen);
            if (_linkType==sim_dummy_linktype_ik_tip_target)
                ogl::setMaterialColor(sim_colorcomponent_ambient_diffuse,ogl::colorRed);
            if ((_linkType==sim_dummy_linktype_dynamics_loop_closure)||(_linkType==sim_dummy_linktype_dynamics_force_constraint))
                ogl::setMaterialColor(sim_colorcomponent_ambient_diffuse,ogl::colorBlue);
            ogl::buffer.clear();
            ogl::addBuffer3DPoints(0.0f,0.0f,0.0f);
            ogl::addBuffer3DPoints(cumulMobile.X.data);
            ogl::drawRandom3dLines(&ogl::buffer[0],2,false,normalVectorForLinesAndPoints.data);
            ogl::buffer.clear();
        }
        _disableAuxClippingPlanes();
    }

    // At the end of every 3DObject display routine:
    _commonFinish(dummy,renderingObject);
}

#else

void displayDummy(CDummy* dummy,CViewableBase* renderingObject,int displayAttrib)
{

}

#endif



