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

#include "vFileDialog.h"
#include <QFileDialog>

int VFileDialog::_fileDialogNative=-1; // default

void VFileDialog::setFileDialogNative(int n)
{
    _fileDialogNative=n;
}

std::string VFileDialog::getOpenFileName(QWidget* parent,unsigned short option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2,const std::string& extension3,const std::string& extension4,const std::string& extension5,const std::string& extension6,const std::string& extension7,const std::string& extension8,const std::string& extension9,const std::string& extension10)
{
    return(_getOpenOrSaveFileName(true,parent,option,title,startPath,initFilename,allowAnyFile,extensionName,extension1,extension2,extension3,extension4,extension5,extension6,extension7,extension8,extension9,extension10));
}

std::string VFileDialog::getSaveFileName(QWidget* parent,unsigned short option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2,const std::string& extension3,const std::string& extension4,const std::string& extension5,const std::string& extension6,const std::string& extension7,const std::string& extension8,const std::string& extension9,const std::string& extension10)
{
    return(_getOpenOrSaveFileName(false,parent,option,title,startPath,initFilename,allowAnyFile,extensionName,extension1,extension2,extension3,extension4,extension5,extension6,extension7,extension8,extension9,extension10));
}

std::string VFileDialog::_getOpenOrSaveFileName(bool open,QWidget* parent,unsigned short option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2,const std::string& extension3,const std::string& extension4,const std::string& extension5,const std::string& extension6,const std::string& extension7,const std::string& extension8,const std::string& extension9,const std::string& extension10)
{
    QString retString;
    QString pathAndOptionalFilename(QString::fromLocal8Bit(startPath.c_str()));
    if (initFilename.length()!=0)
        pathAndOptionalFilename=QString::fromLocal8Bit(initFilename.c_str());
    std::string filter=extensionName;
    if (extensionName.find(";;")==std::string::npos)
    {
        filter=extensionName+" (*.";
        filter+=extension1;
        if (extension2.length()!=0)
            filter+=" *."+extension2;
        if (extension3.length()!=0)
            filter+=" *."+extension3;
        if (extension4.length()!=0)
            filter+=" *."+extension4;
        if (extension5.length()!=0)
            filter+=" *."+extension5;
        if (extension6.length()!=0)
            filter+=" *."+extension6;
        if (extension7.length()!=0)
            filter+=" *."+extension7;
        if (extension8.length()!=0)
            filter+=" *."+extension8;
        if (extension9.length()!=0)
            filter+=" *."+extension9;
        if (extension10.length()!=0)
            filter+=" *."+extension10;
        filter+=")";
        if (allowAnyFile)
            filter+=";;all (*.*)";
    }

    bool native=true;
#ifndef WIN_VREP // native dialogs have a bug on MacOS/Linux versions: the initial directory is not set. Because of that, we don't use native dialogs
    native=false;
#endif
    if (_fileDialogNative!=-1)
    {
        if (_fileDialogNative==0)
            native=true;
        if (_fileDialogNative==1)
            native=false;
    }

#ifndef WIN_VREP
    // On Linux (and sometimes on Mac too), we have several problems if the message queue is not fully processed,
    // but only partially processed (e.g. App::qtApp->processEvents(QEventLoop::ExcludeUserInputEvents|QEventLoop::ExcludeSocketNotifiers)).
    // A quick/dirty fix is tweak1 below. The full fix is to process all messages while
    // waiting for a lock, which is done now by default.
    QFileDialog dlg(parent,title.c_str(),pathAndOptionalFilename,filter.c_str());
    dlg.setNameFilter(filter.c_str());
    if (open)
    {
        dlg.setFileMode(QFileDialog::ExistingFile);
        dlg.setAcceptMode(QFileDialog::AcceptOpen);
    }
    else
    {
        dlg.setFileMode(QFileDialog::AnyFile);
        dlg.setAcceptMode(QFileDialog::AcceptSave);
    }
    if (!native)
        dlg.setOptions(QFileDialog::DontUseNativeDialog);

    if (dlg.exec())
    {
        QStringList qfiles=dlg.selectedFiles();
        if (qfiles.size()==1)
            retString=qfiles.at(0);
    }
#else
    if (!native)
    {
        if (open)
            retString=QFileDialog::getOpenFileName(parent,title.c_str(),pathAndOptionalFilename,filter.c_str(),0,QFileDialog::DontUseNativeDialog);
        else
            retString=QFileDialog::getSaveFileName(parent,title.c_str(),pathAndOptionalFilename,filter.c_str(),0,QFileDialog::DontUseNativeDialog);
    }
    else
    {
        if (open)
            retString=QFileDialog::getOpenFileName(parent,title.c_str(),pathAndOptionalFilename,filter.c_str());
        else
            retString=QFileDialog::getSaveFileName(parent,title.c_str(),pathAndOptionalFilename,filter.c_str());
    }
#endif

    // the extension is sometimes omited! Check it:
    std::string rs(retString.toLocal8Bit().data());
    if ((!open)&&(extension1!="*")&&(extension1!="")&&(extension2=="")&&(rs.size()>0))
    {
        bool addExtension=false;
        if (rs.size()>extension1.size()+1)
        {
            if (rs[rs.size()-1-extension1.size()]!='.')
                addExtension=true;
            else
            {
                for (unsigned int i=0;i<extension1.size();i++)
                {
                    if (rs[rs.size()-extension1.size()+i]!=extension1[i])
                        addExtension=true;
                }
            }
        }
        else
            addExtension=true;
        if (addExtension)
        {
            rs+='.';
            rs+=extension1;
        }
    }
    return(rs);
}


void VFileDialog::getOpenFileNames(std::vector<std::string>& files,QWidget* parent,unsigned short option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2,const std::string& extension3,const std::string& extension4,const std::string& extension5,const std::string& extension6,const std::string& extension7,const std::string& extension8,const std::string& extension9,const std::string& extension10)
{
    files.clear();
    QString pathAndOptionalFilename(QString::fromLocal8Bit(startPath.c_str()));
    if (initFilename.length()!=0)
        pathAndOptionalFilename=QString::fromLocal8Bit(initFilename.c_str());
    std::string filter=extensionName+" (*.";
    filter+=extension1;
    if (extension2.length()!=0)
        filter+=" *."+extension2;
    if (extension3.length()!=0)
        filter+=" *."+extension3;
    if (extension4.length()!=0)
        filter+=" *."+extension4;
    if (extension5.length()!=0)
        filter+=" *."+extension5;
    if (extension6.length()!=0)
        filter+=" *."+extension6;
    if (extension7.length()!=0)
        filter+=" *."+extension7;
    if (extension8.length()!=0)
        filter+=" *."+extension8;
    if (extension9.length()!=0)
        filter+=" *."+extension9;
    if (extension10.length()!=0)
        filter+=" *."+extension10;
    filter+=")";
    if (allowAnyFile)
        filter+=";;all (*.*)";

    bool native=true;
#ifndef WIN_VREP // native dialogs have a bug on MacOS/Linux versions: the initial directory is not set. Because of that, we don't use native dialogs
    native=false;
#endif
    if (_fileDialogNative!=-1)
    {
        if (_fileDialogNative==0)
            native=true;
        if (_fileDialogNative==1)
            native=false;
    }

    QStringList qfiles;

#ifndef WIN_VREP
    // On Linux (and sometimes on Mac too), the static functions sometimes just display a blank dialog. For that reason,
    // we use the non-static version on those platforms! The "show()/hide()" below might be important, otherwise we have the same
    // problem as with the static version of the functions!
    QFileDialog dlg(parent,title.c_str(),pathAndOptionalFilename,filter.c_str());
    dlg.setNameFilter(filter.c_str());
    dlg.setFileMode(QFileDialog::ExistingFiles);
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    if (!native)
        dlg.setOptions(QFileDialog::DontUseNativeDialog);
    if (dlg.exec())
        qfiles=dlg.selectedFiles();
#else
    if (!native)
        qfiles=QFileDialog::getOpenFileNames(parent,title.c_str(),pathAndOptionalFilename,filter.c_str(),0,QFileDialog::DontUseNativeDialog);
    else
        qfiles=QFileDialog::getOpenFileNames(parent,title.c_str(),pathAndOptionalFilename,filter.c_str());
#endif

    for (int i=0;i<int(qfiles.size());i++)
        files.push_back(qfiles.at(i).toLocal8Bit().data());
}

std::string VFileDialog::getExistingFolder(QWidget* parent,const std::string& title,const std::string& startPath)
{
    QString retString;
    QString path(QString::fromLocal8Bit(startPath.c_str()));

    bool native=true;
#ifndef WIN_VREP // native dialogs have a bug on MacOS/Linux versions: the initial directory is not set. Because of that, we don't use native dialogs
    native=false;
#endif
    if (_fileDialogNative!=-1)
    {
        if (_fileDialogNative==0)
            native=true;
        if (_fileDialogNative==1)
            native=false;
    }
    QFileDialog::Options options=QFileDialog::ShowDirsOnly;
    if (!native)
        options|=QFileDialog::DontUseNativeDialog;
    retString=QFileDialog::getExistingDirectory(parent,title.c_str(),path,options);
    return(retString.toLocal8Bit().data());
}

