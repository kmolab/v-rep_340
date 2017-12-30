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

#include "pathPointManipulation.h"
#include "funcDebug.h"
#include "app.h"

CPathPointManipulation::CPathPointManipulation()
{
    FUNCTION_DEBUG;
    _uniqueSelectedPathID_nonEditMode=-1;
    _simulationStopped=true;
}

CPathPointManipulation::~CPathPointManipulation()
{
    FUNCTION_DEBUG;
    // Following should already be erased:
}

void CPathPointManipulation::keyPress(int key)
{

}

std::vector<int>* CPathPointManipulation::getPointerToSelectedPathPointIndices_nonEditMode()
{
    return(&_selectedPathPointIndices_nonEditMode);
}

void CPathPointManipulation::clearPathPointIndices_nonEditMode()
{
    FUNCTION_DEBUG;
    _selectedPathPointIndices_nonEditMode.clear();
}

bool CPathPointManipulation::isPathPointIndexSelected_nonEditMode(int index,bool removeIfYes)
{
    FUNCTION_DEBUG;
    for (int i=0;i<int(_selectedPathPointIndices_nonEditMode.size());i++)
    {
        if (index==_selectedPathPointIndices_nonEditMode[i])
        {
            if (!removeIfYes)
                return(true);
            _selectedPathPointIndices_nonEditMode.erase(_selectedPathPointIndices_nonEditMode.begin()+i);
            return(true);
        }
    }
    return(false);
}

void CPathPointManipulation::addPathPointToSelection_nonEditMode(int pathPointIdentifier)
{
    FUNCTION_DEBUG;
    if (pathPointIdentifier>=NON_OBJECT_PICKING_ID_PATH_PTS_START)
    {
        if (_uniqueSelectedPathID_nonEditMode!=-1)
        { // Ok, we have one path selected
            if (!isPathPointIndexSelected_nonEditMode(pathPointIdentifier-NON_OBJECT_PICKING_ID_PATH_PTS_START,false))
                _selectedPathPointIndices_nonEditMode.push_back(pathPointIdentifier-NON_OBJECT_PICKING_ID_PATH_PTS_START);
        }
    }
}

void CPathPointManipulation::xorAddPathPointToSelection_nonEditMode(int pathPointIdentifier)
{
    FUNCTION_DEBUG;
    if (pathPointIdentifier>=NON_OBJECT_PICKING_ID_PATH_PTS_START)
    {
        if (_uniqueSelectedPathID_nonEditMode!=-1)
        {
            if (!isPathPointIndexSelected_nonEditMode(pathPointIdentifier-NON_OBJECT_PICKING_ID_PATH_PTS_START,true))
                _selectedPathPointIndices_nonEditMode.push_back(pathPointIdentifier-NON_OBJECT_PICKING_ID_PATH_PTS_START);
        }
    }
}

void CPathPointManipulation::announceObjectSelectionChanged()
{
    FUNCTION_DEBUG;
    if ((App::ct==NULL)||(App::ct->objCont==NULL))
        return;
    if ( (App::ct->objCont->getSelSize()==1)&&_simulationStopped )
    {
        CPath* selPath=App::ct->objCont->getPath(App::ct->objCont->getLastSelectionID());
        if (selPath!=NULL)
        {
            if (selPath->getID()!=_uniqueSelectedPathID_nonEditMode)
            {
                _selectedPathPointIndices_nonEditMode.clear();
                _uniqueSelectedPathID_nonEditMode=selPath->getID();
            }
        }
        else
        {
            _selectedPathPointIndices_nonEditMode.clear();
            _uniqueSelectedPathID_nonEditMode=-1;
        }
    }
    else
    {
        _selectedPathPointIndices_nonEditMode.clear();
        _uniqueSelectedPathID_nonEditMode=-1;
    }
}

void CPathPointManipulation::announceSceneInstanceChanged()
{
    FUNCTION_DEBUG;
    _selectedPathPointIndices_nonEditMode.clear();
    _uniqueSelectedPathID_nonEditMode=-1;
}

void CPathPointManipulation::simulationAboutToStart()
{
    FUNCTION_DEBUG;
    _selectedPathPointIndices_nonEditMode.clear();
    _uniqueSelectedPathID_nonEditMode=-1;
    _simulationStopped=false;
}

void CPathPointManipulation::simulationEnded()
{
    FUNCTION_DEBUG;
    _simulationStopped=true;
}

int CPathPointManipulation::getUniqueSelectedPathId_nonEditMode()
{
    FUNCTION_DEBUG;
    return(_uniqueSelectedPathID_nonEditMode);
}

int CPathPointManipulation::getSelectedPathPointIndicesSize_nonEditMode()
{
    FUNCTION_DEBUG;
    return(int(_selectedPathPointIndices_nonEditMode.size()));
}

bool CPathPointManipulation::deleteSelectedPathPoints_nonEditMode()
{
    FUNCTION_DEBUG;
    SSimulationThreadCommand cmd;
    cmd.cmdId=DELETE_SELECTED_PATH_POINTS_NON_EDIT_FROMUI_TOSIM_CMD;
    cmd.intParams.push_back(_uniqueSelectedPathID_nonEditMode);
    cmd.intVectorParams.push_back(_selectedPathPointIndices_nonEditMode);
    App::simThread->appendSimulationThreadCommand(cmd);
    _selectedPathPointIndices_nonEditMode.clear();
    return(true);
}
