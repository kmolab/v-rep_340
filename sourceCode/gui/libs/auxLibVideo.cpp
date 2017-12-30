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

#include "auxLibVideo.h"
#include "app.h"

WLibrary CAuxLibVideo::_lib=NULL;

pVideo_recorderGetEncoderString CAuxLibVideo::video_recorderGetEncoderString=0;
pVideo_recorderInitialize CAuxLibVideo::video_recorderInitialize=0;
pVideo_recorderAddFrame CAuxLibVideo::video_recorderAddFrame=0;
pVideo_recorderEnd CAuxLibVideo::video_recorderEnd=0;

void CAuxLibVideo::loadLibrary(bool headless)
{
    if (!headless)
    {
        std::string vidLibPathAndName(VVarious::getModulePath());
        vidLibPathAndName+=VREP_SLASH;
#ifdef WIN_VREP
        vidLibPathAndName+="vvcl.dll";
#endif
#ifdef MAC_VREP
        vidLibPathAndName+="libvvcl.dylib";
#endif
#ifdef LIN_VREP
        vidLibPathAndName+="libvvcl.so";
#endif
        if (_loadLibrary(vidLibPathAndName.c_str()))
        {
            if (!_getAuxLibProcAddresses())
            { // Error
                unloadLibrary();
                SSimulationThreadCommand cmd;
                cmd.cmdId=DISPLAY_MESSAGE_CMD;
                cmd.intParams.push_back(sim_msgbox_type_info);
                cmd.stringParams.push_back("Video compression library");
                cmd.stringParams.push_back("Could not find all required functions in the video compression library.");
                App::appendSimulationThreadCommand(cmd,5000);
                printf("Could not find all required functions in the video compression library.\n");
            }
            else
                printf("Loaded the video compression library.\n");
        }
        else
        {
            SSimulationThreadCommand cmd;
            cmd.cmdId=DISPLAY_MESSAGE_CMD;
            cmd.intParams.push_back(sim_msgbox_type_info);
            cmd.stringParams.push_back("Video compression library");
            cmd.stringParams.push_back("Could not find or correctly load the video compression library.");
            App::appendSimulationThreadCommand(cmd,5000);
            printf("Could not find or correctly load the video compression library.\n");
        }
    }
    else
        _lib=NULL;
}

bool CAuxLibVideo::_loadLibrary(const char* pathAndFilename)
{
    _lib=VVarious::openLibrary(pathAndFilename);
    return(_lib!=NULL);
}

void CAuxLibVideo::unloadLibrary()
{
    if (_lib!=NULL)
        VVarious::closeLibrary(_lib);
    _lib=NULL;
}

WLibraryFunc CAuxLibVideo::_getProcAddress(const char* funcName)
{
    return(VVarious::resolveLibraryFuncName(_lib,funcName));
}

bool CAuxLibVideo::_getAuxLibProcAddresses()
{
    video_recorderGetEncoderString=(pVideo_recorderGetEncoderString)(_getProcAddress("getAvailableEncoderName"));
    video_recorderInitialize=(pVideo_recorderInitialize)(_getProcAddress("recorderInitialize"));
    video_recorderAddFrame=(pVideo_recorderAddFrame)(_getProcAddress("recorderAddFrame"));
    video_recorderEnd=(pVideo_recorderEnd)(_getProcAddress("recorderEnd"));

    if (video_recorderGetEncoderString==0) return false;
    if (video_recorderInitialize==0) return false;
    if (video_recorderAddFrame==0) return false;
    if (video_recorderEnd==0) return false;
    return true;
}
