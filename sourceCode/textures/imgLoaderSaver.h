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

// FULLY STATIC CLASS
class CImageLoaderSaver
{
public:
    static unsigned char* load(const char* filename,int* resX,int* resY,int* colorComponents,int desiredColorComponents,int scaleTo=0);
    static unsigned char* loadQTgaImageData(const char* fileAndPath,int& resX,int& resY,bool& rgba,unsigned char invisibleColor[3]=NULL,int bitsPerPixel[1]=NULL);
    static unsigned char* getScaledImage(const unsigned char* originalImg,int colorComponents,int originalX,int originalY,int newX,int newY);
    static unsigned char* getScaledImage(const unsigned char* originalImg,const int resolIn[2],int resolOut[2],int options);
    static bool transformImage(unsigned char* img,const int resol[2],int options);
    static bool save(const unsigned char* data,const int resolution[2],int options,const char* filename,int quality);
    static unsigned char* load(int resolution[2],int options,const char* filename);
};
