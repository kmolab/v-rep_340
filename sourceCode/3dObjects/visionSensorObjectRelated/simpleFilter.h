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
#include "drawingContainer2D.h"
#include "ser.h"

struct SExternalFilterInfo
{
    std::string name;
    int header;
    int id;
};

class CVisionSensor; // forward declaration

class CSimpleFilter
{
public:

    CSimpleFilter();
    virtual ~CSimpleFilter();

    CSimpleFilter* copyYourself();
    void serialize(CSer& ar);

    void setFilterType(int t);
    int getFilterType();
    void setCustomFilterInfo(int header,int id);
    void getCustomFilterInfo(int& header,int& id);
    bool processAndTrigger(CVisionSensor* sensor,int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* outputDepthBuffer,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    static int getAvailableFilter(int filterTypeIndex);
    static std::string getSpecificFilterString(int filterType);
    std::string getFilterString();
    static bool canFilterBeEdited(int filterType);
    bool canFilterBeEdited();
    void getParameters(std::vector<unsigned char>& byteParams,std::vector<int>& intParams,std::vector<float>& floatParams,int& filterVersion);
    void setParameters(const std::vector<unsigned char>& byteParams,const std::vector<int>& intParams,const std::vector<float>& floatParams,int filterVersion);
    void setEnabled(bool e);
    bool getEnabled();
    int getUniqueID();
    void getCustomFilterParameters(std::vector<unsigned char>& params);
    void setCustomFilterParameters(const std::vector<unsigned char>& params);

    static void readAllExternalFilters();

protected:
    static int _getFilterTypeAndNameFromHeaderAndID(int header,int id,std::string& name);

    bool processAndTrigger_originalImage(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_originalDepth(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_uniformImage(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_toOutput(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_toDepthOutput(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* outputDepthBuffer,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_toBuffer1(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_toBuffer2(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_fromBuffer1(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_fromBuffer2(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);

    bool processAndTrigger_swapWithBuffer1(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_addToBuffer1(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_subtractFromBuffer1(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);

    bool processAndTrigger_swapBuffers(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_addBuffer1(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_subtractBuffer1(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_multiplyWithBuffer1(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_horizontalFlip(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_verticalFlip(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_3x3filter(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_5x5filter(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_normalize(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_colorSegmentation(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_intensityScale(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_keepOrRemoveColors(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_scaleAndOffsetColors(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_correlationWithBuffer1(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_binary(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_sharpen(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_edge(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_shift(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_rectangularCut(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_circularCut(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_resize(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_rotate(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_customized(int objectHandle,int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);

    bool processAndTrigger_blobExtraction(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_imageToCoord(CVisionSensor* sensor,int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_pixelChange(int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);
    bool processAndTrigger_velodyne(CVisionSensor* sensor,int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,float* workImage,std::vector<float>& returnData,float* buffer1,float* buffer2,CDrawingContainer2D& drawingContainer);

    // Variables which need to be serialized & copied 
    bool _enabled;
    int _filterType;
    int _customFilterHeader;
    int _customFilterID;
    std::string _customFilterName;
    int _filterVersion;
    std::vector<int> _intParameters;
    std::vector<float> _floatParameters;
    std::vector<unsigned char> _byteParameters;
    std::vector<unsigned char> _customFilterParameters;

    // Other variables:
    int _uniqueID;
    static std::vector<SExternalFilterInfo> _externalFilters;
};
