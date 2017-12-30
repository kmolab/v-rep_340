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

#include "vrepMainHeader.h"
#include "scintillaUserModalDlg.h"
#include "v_rep_internal.h"
#include "luaScriptFunctions.h"
#include "vMessageBox.h"
#include <algorithm>
#include "app.h"
#include "v_repStringTable.h"
#include <SciLexer.h>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QShortcut>
#include "qdlgsimpleSearch.h"
#include "tinyxml2.h"
#include "tt.h"
#include "ttUtil.h"

CScintillaUserModalDlg::CScintillaUserModalDlg(const std::string& xmlInfo,QWidget* pParent, Qt::WindowFlags f) : QDialog(pParent,QT_MODAL_SCINTILLA_DLG_STYLE)
{
    _editable=true;
    _searchable=true;
    _isLua=false;
    _useVrepKeywords=false;
    _tabWidth=4;
    _size[0]=800;
    _size[1]=600;
    _position[0]=100;
    _position[1]=100;
    _title="Editor";

    _textColor=RGB(50,50,50);
    _backgroundColor=RGB(210,210,210);
    _selectionColor=RGB(60,60,210);
    _keywords1Color=RGB(152,0,0); //RGB(100,100,210);
    _keywords2Color=RGB(220,80,20); //RGB(210,100,100);

    _commentColor=RGB(0,140,0);
    _numberColor=RGB(220,0,220);
    _stringColor=RGB(255,255,0);
    _characterColor=RGB(255,255,0);
    _operatorColor=RGB(0,0,0);
    _preprocessorColor=RGB(0,128,128);
    _identifierColor=RGB(64,64,64);
    _wordColor=RGB(0,0,255);
    _word4Color=RGB(152,64,0);




    int tabWidth=4;
    std::string lexerStr("lua");


    tinyxml2::XMLDocument xmldoc;
    tinyxml2::XMLError error=xmldoc.Parse(xmlInfo.c_str(),xmlInfo.size());
    if(error==tinyxml2::XML_NO_ERROR)
    {
        tinyxml2::XMLElement* rootElement=xmldoc.FirstChildElement();
        const char* str=rootElement->Attribute("title");
        if (str!=NULL)
            setWindowTitle(str);
        str=rootElement->Attribute("textColor");
        _getColorFromString(str,_textColor);
        str=rootElement->Attribute("backgroundColor");
        _getColorFromString(str,_backgroundColor);
        str=rootElement->Attribute("selectionColor");
        _getColorFromString(str,_selectionColor);
        str=rootElement->Attribute("commentColor");
        _getColorFromString(str,_commentColor);
        str=rootElement->Attribute("numberColor");
        _getColorFromString(str,_numberColor);
        str=rootElement->Attribute("stringColor");
        _getColorFromString(str,_stringColor);
        str=rootElement->Attribute("characterColor");
        _getColorFromString(str,_characterColor);
        str=rootElement->Attribute("operatorColor");
        _getColorFromString(str,_operatorColor);
        str=rootElement->Attribute("preprocessorColor");
        _getColorFromString(str,_preprocessorColor);
        str=rootElement->Attribute("identifierColor");
        _getColorFromString(str,_identifierColor);
        str=rootElement->Attribute("wordColor");
        _getColorFromString(str,_wordColor);
        str=rootElement->Attribute("word4Color");
        _getColorFromString(str,_word4Color);

        str=rootElement->Attribute("tabWidth");
        if (str!=NULL)
            tt::stringToInt(str,tabWidth);
        str=rootElement->Attribute("size");
        if (str!=NULL)
        {
            std::string line(str);
            std::string w;
            if (tt::extractSpaceSeparatedWord(line,w))
            {
                tt::stringToInt(w.c_str(),_size[0]);
                if (tt::extractSpaceSeparatedWord(line,w))
                    tt::stringToInt(w.c_str(),_size[1]);
            }
        }
        str=rootElement->Attribute("position");
        if (str!=NULL)
        {
            std::string line(str);
            std::string w;
            if (tt::extractSpaceSeparatedWord(line,w))
            {
                tt::stringToInt(w.c_str(),_position[0]);
                if (tt::extractSpaceSeparatedWord(line,w))
                    tt::stringToInt(w.c_str(),_position[1]);
            }
        }
        rootElement->QueryBoolAttribute("searchable",&_searchable);
        rootElement->QueryBoolAttribute("editable",&_editable);
        rootElement->QueryBoolAttribute("isLua",&_isLua);
        rootElement->QueryBoolAttribute("useVrepKeywords",&_useVrepKeywords);
//      str=rootElement->Attribute("lexer");
//      if (str!=NULL)
//          lexerStr=str;

        tinyxml2::XMLElement* keyw1Element=rootElement->FirstChildElement("keywords1");
        if (keyw1Element!=NULL)
        {
            str=keyw1Element->Attribute("color");
            _getColorFromString(str,_keywords1Color);
            tinyxml2::XMLElement* w=keyw1Element->FirstChildElement("item");
            while (w!=NULL)
            {
                str=w->Attribute("word");
                if (str!=NULL)
                {
                    if (_allKeywords1.size()>0)
                        _allKeywords1+=" ";
                    _allKeywords1+=str;
                    SScintillaUserKeyword b;
                    b.keyword=str;
                    b.autocomplete=true;
                    w->QueryBoolAttribute("autocomplete",&b.autocomplete);
                    str=w->Attribute("calltip");
                    if (str!=NULL)
                        b.callTip=str;
                    _keywords1.push_back(b);
                }
                w=w->NextSiblingElement("item");
            }
        }
        if (_useVrepKeywords)
        {
            for (size_t i=0;simLuaCommands[i].name!="";i++)
            {
                if (_allKeywords1.size()>0)
                    _allKeywords1+=" ";
                _allKeywords1+=simLuaCommands[i].name;
                SScintillaUserKeyword b;
                b.keyword=simLuaCommands[i].name;
                b.autocomplete=true;
                b.callTip=simLuaCommands[i].callTip;
                _keywords1.push_back(b);
            }
        }

        tinyxml2::XMLElement* keyw2Element=rootElement->FirstChildElement("keywords2");
        if (keyw2Element!=NULL)
        {
            str=keyw2Element->Attribute("color");
            _getColorFromString(str,_keywords2Color);
            tinyxml2::XMLElement* w=keyw2Element->FirstChildElement("item");
            while (w!=NULL)
            {
                str=w->Attribute("word");
                if (str!=NULL)
                {
                    if (_allKeywords2.size()>0)
                        _allKeywords2+=" ";
                    _allKeywords2+=str;
                    SScintillaUserKeyword b;
                    b.keyword=str;
                    b.autocomplete=true;
                    w->QueryBoolAttribute("autocomplete",&b.autocomplete);
                    str=w->Attribute("calltip");
                    if (str!=NULL)
                        b.callTip=str;
                    _keywords2.push_back(b);

                }
                w=w->NextSiblingElement("item");
            }
        }
        if (_useVrepKeywords)
        {
            for (size_t i=0;simLuaVariables[i].name!="";i++)
            {
                if (_allKeywords2.size()>0)
                    _allKeywords2+=" ";
                _allKeywords2+=simLuaVariables[i].name;
                SScintillaUserKeyword b;
                b.keyword=simLuaVariables[i].name;
                b.autocomplete=true;
                b.callTip="";
                _keywords2.push_back(b);
            }
        }
    }

    _scintillaObject=new QsciScintilla;

    // Use following if using a QDialog!!
    QVBoxLayout *bl=new QVBoxLayout(this);
    bl->setContentsMargins(0,0,0,0);
    setLayout(bl);
    bl->addWidget(_scintillaObject);

// use following if using a QMainWindow!!!   setCentralWidget(_scintillaObject);

    QsciLexer* lexer=NULL;
    if (lexerStr.compare("lua")==0)
        lexer=new QsciLexerLua;
    /*
    if (lexerStr.compare("java")==0)
        lexer=new QsciLexerJava;
    if (lexerStr.compare("python")==0)
        lexer=new QsciLexerPython;
    if (lexerStr.compare("cpp")==0)
        lexer=new QsciLexerCPP;
    if (lexerStr.compare("html")==0)
        lexer=new QsciLexerHTML;
    if (lexerStr.compare("octave")==0)
        lexer=new QsciLexerOctave;
    */
    if (lexer!=NULL)
        _scintillaObject->setLexer(lexer);

    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETSTYLEBITS,(int)5);
    _scintillaObject->setTabWidth(tabWidth);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETUSETABS,(int)0);

    if (_allKeywords1.size()>0)
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETKEYWORDS,(unsigned long)1,_allKeywords1.c_str());
    if (_allKeywords2.size()>0)
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETKEYWORDS,(unsigned long)2,_allKeywords2.c_str());

    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETMARGINWIDTHN,(unsigned long)0,(long)48); // Line numbers
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETMARGINWIDTHN,(unsigned long)1,(long)0); // Symbols
    connect(_scintillaObject,SIGNAL(SCN_CHARADDED(int)),this,SLOT(_charAdded(int)));
    connect(_scintillaObject,SIGNAL(SCN_UPDATEUI(int)),this,SLOT(_updateUi(int)));

    if (_searchable)
    {
        QShortcut* shortcut = new QShortcut(QKeySequence(tr("Ctrl+f", "Find")), this);
        connect(shortcut,SIGNAL(activated()), this, SLOT(_onFind()));
    }

    _setColorsAndMainStyles();
}

CScintillaUserModalDlg::~CScintillaUserModalDlg()
{
    delete _scintillaObject;
}

bool CScintillaUserModalDlg::initialize(const char* text)
{
    move(_position[0],_position[1]);
    resize(_size[0],_size[1]);

    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETTEXT,(unsigned long)0,text);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_EMPTYUNDOBUFFER); // Make sure the undo buffer is empty (otherwise the first undo will remove the whole script --> a bit confusing)
    if (!_editable)
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETREADONLY,(int)1);
    return(true);
}

void CScintillaUserModalDlg::_setColorsAndMainStyles()
{ // backgroundStyle=0 Main script, 1=non-threaded, 2=threaded
    struct SScintillaColors
    {
        int iItem;
        unsigned int rgb;
    };
    const unsigned int black=RGB(0,0,0);
    const unsigned int darkGrey=RGB(70,70,70);
    const unsigned int white=RGB(255,255,255);

    int fontSize=12;
    std::string theFont("Courier"); // since Scintilla 2.7.2 and Qt5.1.1, "Courier New" gives strange artifacts (with Mac and Linux)!
#ifdef MAC_VREP
    fontSize=16; // bigger fonts here
#endif
    if (App::userSettings->scriptEditorFontSize!=-1)
        fontSize=App::userSettings->scriptEditorFontSize;
    if (App::userSettings->scriptEditorFont.length()!=0)
        theFont=App::userSettings->scriptEditorFont;
#ifndef MAC_VREP
    if (App::sc>1)
        fontSize*=2;
#endif

    _setAStyle(QsciScintillaBase::STYLE_DEFAULT,_textColor,_backgroundColor,fontSize,theFont.c_str()); // set global default style

    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETCARETFORE,(unsigned long)black);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLECLEARALL); // set all styles
    _setAStyle(QsciScintillaBase::STYLE_LINENUMBER,white,darkGrey);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETSELBACK,(unsigned long)1,(long)_selectionColor); // selection color

    _setAStyle(SCE_LUA_WORD2,_keywords1Color,_backgroundColor);
    _setAStyle(SCE_LUA_WORD3,_keywords2Color,_backgroundColor);

    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_INDICSETSTYLE,(unsigned long)20,(long)QsciScintillaBase::INDIC_STRAIGHTBOX);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_INDICSETALPHA,(unsigned long)20,(long)160);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_INDICSETFORE,(unsigned long)20,(long)_selectionColor);


    if (_isLua)
    {
        static SScintillaColors syntaxColors[]=
        {
            {SCE_LUA_COMMENT,_commentColor},
            {SCE_LUA_COMMENTLINE,_commentColor},
            {SCE_LUA_COMMENTDOC,_commentColor},
            {SCE_LUA_NUMBER,_numberColor},
            {SCE_LUA_STRING,_stringColor},
            {SCE_LUA_LITERALSTRING,_stringColor},
            {SCE_LUA_CHARACTER,_characterColor},
            {SCE_LUA_OPERATOR,_operatorColor},
            {SCE_LUA_PREPROCESSOR,_preprocessorColor},
            {SCE_LUA_WORD,_wordColor},
            // {SCE_LUA_WORD2,colWord2},
            // {SCE_LUA_WORD3,colWord3},
            {SCE_LUA_WORD4,_word4Color},
            {SCE_LUA_IDENTIFIER,_identifierColor},
            {-1,0}
        };

        // Set syntax colors
        for (int i=0;syntaxColors[i].iItem!=-1;i++)
            _setAStyle(syntaxColors[i].iItem,syntaxColors[i].rgb,_backgroundColor);
    }
}

std::string CScintillaUserModalDlg::makeDialogModal()
{
    setModal(true);
    exec();
    int l=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETLENGTH);
    std::string retVal;
    retVal.resize(l+1);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETTEXT,(unsigned long)l+1,&retVal[0]);
    return(retVal);
}

void CScintillaUserModalDlg::getSizeAndPosition(int s[2],int p[2])
{
    QRect geom(geometry());
    s[0]=geom.width();
    s[1]=geom.height();
    p[0]=geom.x();
    p[1]=geom.y();
}

void CScintillaUserModalDlg::_setAStyle(int style,unsigned int fore,unsigned int back,int size,const char *face)
{
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETFORE,(unsigned long)style,(long)fore);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETBACK,(unsigned long)style,(long)back);
    if (size>=1)
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETSIZE,(unsigned long)style,(long)size);
    if (face)
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETFONT,(unsigned long)style,face);
}

void CScintillaUserModalDlg::_prepAutoCompletionList(const std::string& txt)
{
    _autoCompletionList.clear();
    std::vector<std::string> t;

    for (size_t i=0;i<_keywords1.size();i++)
    {
        if ((_keywords1[i].autocomplete)&&(_keywords1[i].keyword.size()>=txt.size())&&(_keywords1[i].keyword.compare(0,txt.size(),txt)==0))
            t.push_back(_keywords1[i].keyword);
    }

    for (size_t i=0;i<_keywords2.size();i++)
    {
        if ((_keywords2[i].autocomplete)&&(_keywords2[i].keyword.size()>=txt.size())&&(_keywords2[i].keyword.compare(0,txt.size(),txt)==0))
            t.push_back(_keywords2[i].keyword);
    }

    std::sort(t.begin(),t.end());

    for (size_t i=0;i<t.size();i++)
    {
        _autoCompletionList+=t[i];
        if (i!=t.size()-1)
            _autoCompletionList+=' ';
    }
}

std::string CScintillaUserModalDlg::_getCallTip(const char* txt)
{ 
    size_t l=strlen(txt);

    for (size_t i=0;i<_keywords1.size();i++)
    {
        if ( (_keywords1[i].keyword.size()==l)&&(_keywords1[i].keyword.compare(txt)==0) )
            return(_keywords1[i].callTip);
    }

    for (size_t i=0;i<_keywords2.size();i++)
    {
        if ( (_keywords2[i].keyword.size()==l)&&(_keywords2[i].keyword.compare(txt)==0) )
            return(_keywords2[i].callTip);
    }

    return("");
}

void CScintillaUserModalDlg::_updateUi(int updated)
{
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETINDICATORCURRENT,(int)20);

    int totTextLength=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETLENGTH);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_INDICATORCLEARRANGE,(unsigned long)0,(long)totTextLength);

    int txtL=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETSELTEXT,(unsigned long)0,(long)0)-1;
    if (txtL>=1)
    {
        int selStart=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETSELECTIONSTART);

        char* txt=new char[txtL+1];
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETSELTEXT,(unsigned long)0,txt);

        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETSEARCHFLAGS,(int)QsciScintillaBase::SCFIND_MATCHCASE|QsciScintillaBase::SCFIND_WHOLEWORD);
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETTARGETSTART,(int)0);
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETTARGETEND,(int)totTextLength);

        int p=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_SEARCHINTARGET,(unsigned long)txtL,txt);
        while (p!=-1)
        {
            if (p!=selStart)
                _scintillaObject->SendScintilla(QsciScintillaBase::SCI_INDICATORFILLRANGE,(unsigned long)p,(long)strlen(txt));
            _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETTARGETSTART,(int)p+1);
            _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETTARGETEND,(int)totTextLength);
            p=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_SEARCHINTARGET,(unsigned long)txtL,txt);
        }
        delete[] txt;
    }
}

void CScintillaUserModalDlg::_onFind()
{
    int txtL=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETSELTEXT,(unsigned long)0,(long)0)-1;
    if (txtL>=1)
    {
        char* txt=new char[txtL+1];
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETSELTEXT,(unsigned long)0,txt);
        _findText(txt,CQDlgSimpleSearch::matchCase);
        delete[] txt;
    }
    else
    {
        CQDlgSimpleSearch simpleSearch(this);
        simpleSearch.init();
        simpleSearch.makeDialogModal();
        if (CQDlgSimpleSearch::textToSearch.length()!=0)
            _findText(CQDlgSimpleSearch::textToSearch.c_str(),CQDlgSimpleSearch::matchCase);
    }
}

void CScintillaUserModalDlg::_getColorFromString(const char* txt,unsigned int& col)
{
    if (txt!=NULL)
    {
        std::string str(txt);
        int r,g,b;
        std::string line(txt);
        std::string w;
        if (tt::extractSpaceSeparatedWord(line,w))
        {
            tt::stringToInt(w.c_str(),r);
            if (tt::extractSpaceSeparatedWord(line,w))
            {
                tt::stringToInt(w.c_str(),g);
                if (tt::extractSpaceSeparatedWord(line,w))
                {
                    tt::stringToInt(w.c_str(),b);
                    col=RGB(r,g,b);
                }
            }
        }
    }
}

void CScintillaUserModalDlg::_findText(const char* txt,bool caseSensitive)
{
    int totTextLength=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETLENGTH);
    int caseInfo=0;
    if (caseSensitive)
        caseInfo=QsciScintillaBase::SCFIND_MATCHCASE;
    int txtL=strlen(txt);
    int pos=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETSELECTIONSTART);

    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETSEARCHFLAGS,(int)caseInfo);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETTARGETSTART,(int)pos+1);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETTARGETEND,(int)totTextLength);

    int p=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_SEARCHINTARGET,(unsigned long)txtL,txt);
    if (p==-1)
    {
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETSEARCHFLAGS,(int)caseInfo);
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETTARGETSTART,(int)0);
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETTARGETEND,(int)totTextLength);

        p=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_SEARCHINTARGET,(unsigned long)txtL,txt);
    }
    if (p!=-1)
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETSEL,(unsigned long)p,(long)p+txtL);
}

void CScintillaUserModalDlg::_charAdded(int charAdded)
{
    if (_scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCACTIVE)!=0)
    { // Autocomplete is active
        if (charAdded=='(')
            _scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCCANCEL);
    }
    if (_scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCACTIVE)==0)
    { // Autocomplete is not active
        if (_scintillaObject->SendScintilla(QsciScintillaBase::SCI_CALLTIPACTIVE)!=0)
        { // CallTip is active
        }
        else
        { // Calltip is not active
            if ( (charAdded=='(')||(charAdded==',') )
            { // Do we need to activate a calltip?

                char linebuf[1000];
                int current=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETCURLINE,(unsigned long)999,linebuf);
                int pos=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETCURRENTPOS);
                linebuf[current]='\0';
                std::string line(linebuf);
                // 1. Find '('. Not perfect, will also detect e.g. "(" or similar
                int cnt=0;
                int pahr=-1;
                for (pahr=current-1;pahr>0;pahr--)
                {
                    if (line[pahr]==')')
                        cnt--;
                    if (line[pahr]=='(')
                    {
                        cnt++;
                        if (cnt==1)
                            break;
                    }
                }
                if ( (cnt==1)&&(pahr>0) )
                { // 2. Get word
                    int spaceCnt=0;
                    int startword=pahr-1;
                    int endword=startword;
                    while ((startword>=0)&&(isalpha(line[startword])||isdigit(line[startword])||(line[startword]=='_')||((line[startword]==' ')&&(spaceCnt>=0)) ))
                    {
                        if (line[startword]==' ')
                        {
                            if ( (spaceCnt==0)&&(endword!=startword) )
                                break;
                            spaceCnt++;
                            endword--;
                        }
                        else
                        {
                            if (spaceCnt>0)
                                spaceCnt=-spaceCnt;
                        }
                        startword--;
                    }
                    std::string s;
                    if (startword!=endword)
                    {
                        s.assign(line.begin()+startword+1,line.begin()+endword+1);
                        s=_getCallTip(s.c_str());
                    }
                    if (s!="")
                    {

                        int fontSize=12-4;
                        std::string theFont("Courier"); // since Scintilla 2.7.2 and Qt5.1.1, "Courier New" gives strange artifacts (with Mac and Linux)!
#ifdef MAC_VREP
                        fontSize=16-4; // bigger fonts here
#endif
                        if (App::userSettings->scriptEditorFontSize!=-1)
                            fontSize=App::userSettings->scriptEditorFontSize-4;
                        if (App::userSettings->scriptEditorFont.length()!=0)
                            theFont=App::userSettings->scriptEditorFont;
#ifndef MAC_VREP
//  if (App::sc>1)
//      fontSize*=2;
#endif
                        _setAStyle(QsciScintillaBase::STYLE_CALLTIP,RGB(0,0,0),RGB(255,255,255),fontSize,theFont.c_str());
                        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_CALLTIPUSESTYLE,(int)0);
                        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_CALLTIPSHOW,(unsigned long)pos,s.c_str());
                    }
                }
            }
            else
            { // Do we need to activate autocomplete?
                int p=-1+_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETCURRENTPOS);
                if (p>=2)
                {
                    char linebuf[1000];
                    int current=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETCURLINE,(unsigned long)999,linebuf);
                    linebuf[current]='\0';
                    std::string line(linebuf);
                    int ind=line.size()-1;
                    int cnt=0;
                    std::string theWord;
                    while ((ind>=0)&&(isalpha(line[ind])||isdigit(line[ind])||(line[ind]=='_') ))
                    {
                        theWord=line[ind]+theWord;
                        ind--;
                        cnt++;
                    }
                    if (theWord.size()>=3)
                    {
                        _prepAutoCompletionList(theWord);
                        if (_autoCompletionList.size()!=0)
                        { // We need to activate autocomplete!
                            _scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCSETAUTOHIDE,(int)0);
                            _scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCSTOPS,(unsigned long)0," ()[]{}:;~`',.=*-+/?!@#$%^&|\\<>\"");
                            _scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCSETMAXHEIGHT,(int)10); // it seems that SCI_AUTOCSETMAXHEIGHT and SCI_AUTOCSETMAXWIDTH are not implemented yet!
                            _scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCSHOW,(unsigned long)cnt,&(_autoCompletionList[0]));
                        }
                    }
                }
            }
        }
    }
}
