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
//#include <Qsci/qscilexerjava.h>
//#include <Qsci/qscilexerpython.h>
//#include <Qsci/qscilexercpp.h>
//#include <Qsci/qscilexerhtml.h>
//#include <Qsci/qscilexeroctave.h>
#include <Qsci/qscistyle.h>
#include <QDialog>

struct SScintillaUserKeyword {
    std::string keyword;
    std::string callTip;
    bool autocomplete;
};


class CScintillaUserModalDlg : public QDialog
{
    Q_OBJECT

public:
    CScintillaUserModalDlg(const std::string& xmlInfo,QWidget* pParent = NULL, Qt::WindowFlags f= 0);
    virtual ~CScintillaUserModalDlg();

    bool initialize(const char* text);
    std::string makeDialogModal();

    void getSizeAndPosition(int s[2],int p[2]);

protected:
    void _getColorFromString(const char* txt,unsigned int& col);
    void _findText(const char* txt,bool caseSensitive);
    void _setColorsAndMainStyles();

    void _setAStyle(int style,unsigned int fore,unsigned int back=RGB(0,0,0),int size=-1,const char *face=0);
    void _prepAutoCompletionList(const std::string& txt);
    std::string _getCallTip(const char* txt);

    std::vector<SScintillaUserKeyword> _keywords1;
    std::vector<SScintillaUserKeyword> _keywords2;
    std::string _autoCompletionList;
    std::string _allKeywords1;
    std::string _allKeywords2;
    bool _editable;
    bool _searchable;
    int _tabWidth;
    int _size[2];
    int _position[2];
    std::string _title;
    bool _isLua;
    bool _useVrepKeywords;
    unsigned int _textColor;
    unsigned int _backgroundColor;
    unsigned int _selectionColor;
    unsigned int _keywords1Color;
    unsigned int _keywords2Color;

    unsigned int _commentColor;
    unsigned int _numberColor;
    unsigned int _stringColor;
    unsigned int _characterColor;
    unsigned int _operatorColor;
    unsigned int _preprocessorColor;
    unsigned int _identifierColor;
    unsigned int _wordColor;
//    unsigned int _word2Color;
//    unsigned int _word3Color;
    unsigned int _word4Color;

    QsciScintilla* _scintillaObject;

public slots:
    void _onFind();
    void _charAdded(int charAdded);
    void _updateUi(int updated);
};

