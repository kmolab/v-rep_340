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

// Some parts of this code were inspired from Robert Umbehant's personal c compiler
// (http://www.codeproject.com/KB/cpp/Personal_C___Compiler.aspx)

#pragma once

#include "vrepMainHeader.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscistyle.h>
#include <QDialog>

class CScintillaDlg : public QDialog
{
    Q_OBJECT

public:
    CScintillaDlg(QWidget* pParent = NULL, Qt::WindowFlags f= 0);
    virtual ~CScintillaDlg();

    void closeEvent(QCloseEvent *event);

    void setOperational();
    void setShowWindow(bool show);
    void initPhase1(int posAndSize[4]);
    void initPhase2(const char* scriptText,const std::vector<int>* foldingInfo);
    void setWindowTitleText(const std::string& title);

    void setColorsAndMainStyles(int scriptType);
    void setScriptID(int id);
    int _scriptID;
    int _lastSetStyle;

    void _setAStyle(int style,unsigned int fore,unsigned int back=RGB(0,0,0),int size=-1,const char *face=0);
    void _setKeywords();
    void _prepAutoCompletionList(const std::string& txt);
    std::string _getCallTip(const char* txt);


    bool _operational;
    void InitialiseEditor();
    void _findText(const char* txt,bool caseSensitive);
    std::vector<int> _unfold();
    void _fold(const std::vector<int>& foldingSt);
    int _fold_getHighestIndex(std::vector<int>& foldingState);
    void _fold_overwriteFromIndex(std::vector<int>& foldingState,int index,int newValue);
    std::vector<int> getFoldingInfo();


    void getWindowPosAndSize(int posAndSize[4]);

    std::string _autoCompletionList;
    static std::string vrepKeywords1;
    static std::string vrepKeywords2;

    std::vector<int> _tempFoldingThing;

    QsciScintilla* _scintillaObject;

    static Qt::WindowFlags dialogStyle;

public slots:
    void _onFind();
    void _charAdded(int charAdded);
    void _modified(int, int, const char *, int, int, int, int, int, int, int);
    void _updateUi(int updated);

};

