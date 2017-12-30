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

#include "vrepMainHeader.h"
#include "7Vector.h"
#include "ser.h"

class CGraphData  
{
public:
    CGraphData();
    CGraphData(int theDataType,int theDataObjectID,int theDataObjectAuxID);
    virtual ~CGraphData();

    // Various
    int getDataType() const;
    int getDataObjectID() const;
    int getDataObjectAuxID() const;
    void setDataObjectID(int newID);
    void setDataObjectAuxID(int newID);

    void setZoomFactor(float newZoomFactor);
    float getZoomFactor() const;
    void setAddCoeff(float newCoeff);
    float getAddCoeff() const;
    void setVisible(bool v);
    bool getVisible() const;
    void setIdentifier(int newIdentifier);
    int getIdentifier() const;
    void setLinkPoints(bool l);
    bool getLinkPoints() const;
    std::string getName() const;
    void setName(std::string theName);
    void setLabel(bool l);
    bool getLabel() const;
    void resetData(int bufferSize);
    void setDerivativeIntegralAndCumulative(int val);
    int getDerivativeIntegralAndCumulative() const;

    void setValue(const C7Vector* graphCTM,int absIndex,bool firstValue,bool cyclic,float range,const std::vector<float>& times);
    void setValueDirect(int absIndex,float theValue,bool firstValue,bool cyclic,float range,const std::vector<float>& times);
    bool getValue(int absIndex,float& v) const;
    bool getValueRaw(int absIndex,float& v) const;

    int getDataLength();
    void setUserData(float data);
    void clearUserData();

    void setMovingAverageCount(int c);
    int getMovingAverageCount() const;

    void serialize(CSer& ar,void* it);
    CGraphData* copyYourself();
    void performObjectLoadingMapping(std::vector<int>* map);
    void performCollisionLoadingMapping(std::vector<int>* map);
    void performDistanceLoadingMapping(std::vector<int>* map);
    void performIkLoadingMapping(std::vector<int>* map);
    void performGcsLoadingMapping(std::vector<int>* map);
    bool announceObjectWillBeErased(int objID,bool copyBuffer);
    bool announceCollisionWillBeErased(int collisionID,bool copyBuffer);
    bool announceDistanceWillBeErased(int distanceID,bool copyBuffer);
    bool announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer);
    bool announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer);
    
    // Variables which need to be serialized & copied 
    float ambientColor[3];

protected:
    // Variables which need to be serialized & copied 
    std::vector <float> _floatData;
    std::vector <float> _transformedFloatData;
    std::vector <unsigned char> _floatDataValidFlags;
    std::vector <unsigned char> _transformedFloatDataValidFlags;
    int dataType;
    int dataObjectID;
    int dataObjectAuxID;
    float zoomFactor;
    float addCoeff;
    int identifier;
    bool visible;
    bool linkPoints;
    bool label;
    int _derivativeIntegralAndCumulative;
    std::string name;


private:
    int _lifeID;
    float _userData;
    bool _userDataValid;
    int _movingAverageCount;
};
