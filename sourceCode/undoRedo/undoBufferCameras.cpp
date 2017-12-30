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
#include "undoBufferCameras.h"
#include "app.h"


CUndoBufferCameras::CUndoBufferCameras()
{
}

CUndoBufferCameras::~CUndoBufferCameras()
{
}

void CUndoBufferCameras::preRestoreCameras()
{
    if (App::userSettings->getUndoRedoOnlyPartialWithCameras())
    {
        std::vector<C3DObject*> cameraProxies;
        for (int cnt=0;cnt<int(App::ct->objCont->cameraList.size());cnt++)
        {
            CCamera* cam=App::ct->objCont->getCamera(App::ct->objCont->cameraList[cnt]);
            SCamBuff buff;
            buff.localTr=cam->getLocalTransformation();
            buff.orthoViewSize=cam->getOrthoViewSize();
            _preRestoreCameraBuffers[cam->getName()]=buff;
            if ((cam->getUseParentObjectAsManipulationProxy())&&(cam->getParent()!=NULL))
            {
                bool present=false;
                for (int i=0;i<int(cameraProxies.size());i++)
                {
                    if (cameraProxies[i]==cam->getParent())
                    {
                        present=true;
                        break;
                    }
                }
                if (!present)
                    cameraProxies.push_back(cam->getParent());
            }
        }

        // now camera proxies:
        for (int cnt=0;cnt<int(cameraProxies.size());cnt++)
        {
            SCamBuff buff;
            buff.localTr=cameraProxies[cnt]->getLocalTransformation();
            _preRestoreCameraProxyBuffers[cameraProxies[cnt]->getName()]=buff;
        }
    }
}

void CUndoBufferCameras::restoreCameras()
{
    if (App::userSettings->getUndoRedoOnlyPartialWithCameras())
    {
        // this is the saved cameras, might be a while back:
        std::vector<C3DObject*> cameraProxies;
        for (int cnt=0;cnt<int(App::ct->objCont->cameraList.size());cnt++)
        {
            CCamera* cam=App::ct->objCont->getCamera(App::ct->objCont->cameraList[cnt]);
            std::map<std::string,SCamBuff>::iterator it=_cameraBuffers.find(cam->getName());
            if (it!=_cameraBuffers.end())
            {
                cam->setLocalTransformation(it->second.localTr);
                cam->setOrthoViewSize(it->second.orthoViewSize);
                cam->bufferMainDisplayStateVariables();
                cam->bufferedMainDisplayStateVariablesToDisplay();
            }
            if ((cam->getUseParentObjectAsManipulationProxy())&&(cam->getParent()!=NULL))
            {
                bool present=false;
                for (int i=0;i<int(cameraProxies.size());i++)
                {
                    if (cameraProxies[i]==cam->getParent())
                    {
                        present=true;
                        break;
                    }
                }
                if (!present)
                    cameraProxies.push_back(cam->getParent());
            }
        }

        for (int cnt=0;cnt<int(App::ct->objCont->objectList.size());cnt++)
        {
            C3DObject* obj=App::ct->objCont->getObject(App::ct->objCont->objectList[cnt]);
            std::map<std::string,SCamBuff>::iterator it=_cameraProxyBuffers.find(obj->getName());
            if (it!=_cameraProxyBuffers.end())
            {
                obj->setLocalTransformation(it->second.localTr);
                obj->bufferMainDisplayStateVariables();
                obj->bufferedMainDisplayStateVariablesToDisplay();
            }
        }


        // this is the saved cameras, just a moment ago:
        for (int cnt=0;cnt<int(App::ct->objCont->cameraList.size());cnt++)
        {
            CCamera* cam=App::ct->objCont->getCamera(App::ct->objCont->cameraList[cnt]);
            std::map<std::string,SCamBuff>::iterator it=_preRestoreCameraBuffers.find(cam->getName());
            if (it!=_preRestoreCameraBuffers.end())
            {
                cam->setLocalTransformation(it->second.localTr);
                cam->setOrthoViewSize(it->second.orthoViewSize);
                cam->bufferMainDisplayStateVariables();
                cam->bufferedMainDisplayStateVariablesToDisplay();
            }
        }
        _preRestoreCameraBuffers.clear();

        for (int cnt=0;cnt<int(App::ct->objCont->objectList.size());cnt++)
        {
            C3DObject* obj=App::ct->objCont->getObject(App::ct->objCont->objectList[cnt]);
            std::map<std::string,SCamBuff>::iterator it=_preRestoreCameraProxyBuffers.find(obj->getName());
            if (it!=_preRestoreCameraProxyBuffers.end())
            {
                obj->setLocalTransformation(it->second.localTr);
                obj->bufferMainDisplayStateVariables();
                obj->bufferedMainDisplayStateVariablesToDisplay();
            }
        }
        _preRestoreCameraProxyBuffers.clear();

    }
}

void CUndoBufferCameras::storeCameras()
{
    if (App::userSettings->getUndoRedoOnlyPartialWithCameras())
    {
        std::vector<C3DObject*> cameraProxies;
        for (int cnt=0;cnt<int(App::ct->objCont->cameraList.size());cnt++)
        {
            CCamera* cam=App::ct->objCont->getCamera(App::ct->objCont->cameraList[cnt]);
            SCamBuff buff;
            buff.localTr=cam->getLocalTransformation();
            buff.orthoViewSize=cam->getOrthoViewSize();
            _cameraBuffers[cam->getName()]=buff;
            C7Vector tr;
            tr.setIdentity();
            cam->setLocalTransformation(tr);
            cam->setOrthoViewSize(1.0f);
            if ((cam->getUseParentObjectAsManipulationProxy())&&(cam->getParent()!=NULL))
            {
                bool present=false;
                for (int i=0;i<int(cameraProxies.size());i++)
                {
                    if (cameraProxies[i]==cam->getParent())
                    {
                        present=true;
                        break;
                    }
                }
                if (!present)
                    cameraProxies.push_back(cam->getParent());
            }
        }

        // now camera proxies:
        for (int cnt=0;cnt<int(cameraProxies.size());cnt++)
        {
            SCamBuff buff;
            buff.localTr=cameraProxies[cnt]->getLocalTransformation();
            _cameraProxyBuffers[cameraProxies[cnt]->getName()]=buff;
            C7Vector tr;
            tr.setIdentity();
            cameraProxies[cnt]->setLocalTransformation(tr);
        }
    }
}

