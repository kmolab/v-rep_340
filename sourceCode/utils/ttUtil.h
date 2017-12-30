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

class CTTUtil  
{ // FULLY STATIC!!
public:
    static void lightBinaryEncode(char* data,int length);
    static void lightBinaryDecode(char* data,int length);
    static unsigned short getCRC(char* data,int length);
    static unsigned short getCRC(const std::string& data);
    static bool extractCommaSeparatedWord(std::string& line,std::string& word);
    static bool extractSpaceSeparatedWord(std::string& line,std::string& word);
    static bool extractLine(std::string& multiline,std::string& line);
    static std::string intToString(int intVal);
    static std::string dwordToString(unsigned int dwordVal);
    static std::string dataToString(char* data,int startPos,int length);
    static void stringToData(const std::string& str,char* data,int startPos);
    static std::string getAdjustedString(const std::string& str,int length);
    static std::string getLightEncodedString(const std::string& ss);
    static std::string getLightDecodedString(const std::string& ss);
    static bool removeSpacesAtBeginningAndEnd(std::string& line);
    static std::string getFormattedString(const char* a=NULL,const char* b=NULL,const char* c=NULL,const char* d=NULL,const char* e=NULL,const char* f=NULL,const char* g=NULL,const char* h=NULL);
    static std::string formatString(const std::string fmt, ...);
    static std::string getLowerCaseString(const char* str);
    static void scaleLightDown_(float& r,float& g,float& b);
    static void scaleColorUp_(float& r,float& g,float& b);
    static void scaleLightDown_(float* rgb);
    static void scaleColorUp_(float* rgb);

    static int replaceWordInLine(std::string& line,const std::string& oldText,const std::string& newText);
};
