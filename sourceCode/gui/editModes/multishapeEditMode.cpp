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

#include "multishapeEditMode.h"
#include "geometric.h"

CMultishapeEditMode::CMultishapeEditMode(CShape* shape)
{
    _shape=shape;
    _shape->geomData->geomInfo->getAllShapeComponentsCumulative(_multishapeGeometricComponents);
    _multishapeGeometricComponentIndex=-1;
}

CMultishapeEditMode::~CMultishapeEditMode()
{
}

bool CMultishapeEditMode::processCommand(int commandID)
{ // Return value is true means success
    return(false);
}

CShape* CMultishapeEditMode::getEditModeMultishape()
{
    return(_shape);
}

int CMultishapeEditMode::getMultishapeGeometricComponentsSize()
{
    return(int(_multishapeGeometricComponents.size()));
}

int CMultishapeEditMode::getMultishapeGeometricComponentIndex()
{
    return(_multishapeGeometricComponentIndex);
}

void CMultishapeEditMode::setMultishapeGeometricComponentIndex(int index)
{
    _multishapeGeometricComponentIndex=index;
}

bool CMultishapeEditMode::isCurrentMultishapeGeometricComponentValid()
{
    return((_multishapeGeometricComponentIndex>=0)&&(_multishapeGeometricComponentIndex<int(_multishapeGeometricComponents.size())));
}

CGeometric* CMultishapeEditMode::getCurrentMultishapeGeometricComponent()
{
    return(_multishapeGeometricComponents[_multishapeGeometricComponentIndex]);
}

CGeometric* CMultishapeEditMode::getMultishapeGeometricComponentAtIndex(int index)
{
    return(_multishapeGeometricComponents[index]);
}

void CMultishapeEditMode::displayAllGeometricComponents(CGeomProxy* geomData,int displayAttrib,CVisualParam* collisionColor,int dynObjFlag_forVisualization,int transparencyHandling)
{
    for (int i=0;i<int(_multishapeGeometricComponents.size());i++)
        _multishapeGeometricComponents[i]->display(geomData,displayAttrib,collisionColor,dynObjFlag_forVisualization,transparencyHandling,i==_multishapeGeometricComponentIndex);
}
