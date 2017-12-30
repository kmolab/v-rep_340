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

#pragma once

#include "pathPoint.h"
#include "ser.h"

class CSimplePathPoint : public CPathPoint 
{
public:
    CSimplePathPoint();
    virtual ~CSimplePathPoint(); 

    void commonInit();
    void serialize(CSer& ar);
    CSimplePathPoint* copyYourself();
    void scaleYourself(float scalingFactor);
    void scaleYourselfNonIsometrically(float x,float y,float z);

    void setBezierFactors(float fBefore,float fAfter);
    void getBezierFactors(float& fBefore,float& fAfter);
    void setBezierPointCount(int c);
    int getBezierPointCount();

protected:

    float _bezierFactorBefore;
    float _bezierFactorAfter;
    int _bezierPointCount;
};
