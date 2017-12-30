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
#include "scintillaModalDlg.h"
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

std::string CScintillaModalDlg::vrepKeywords1;
std::string CScintillaModalDlg::vrepKeywords2;

CScintillaModalDlg::CScintillaModalDlg(int scriptType,QWidget* pParent, Qt::WindowFlags f) : QDialog(pParent,QT_MODAL_SCINTILLA_DLG_STYLE)
{
    _scriptID=-1;
    _scriptType=scriptType;
 //   setAttribute(Qt::WA_DeleteOnClose);
    _scintillaObject=new QsciScintilla;

    // Use following if using a QDialog!!
    QVBoxLayout *bl=new QVBoxLayout(this);
    bl->setContentsMargins(0,0,0,0);
    setLayout(bl);
    bl->addWidget(_scintillaObject);

// use following if using a QMainWindow!!!   setCentralWidget(_scintillaObject);

    QsciLexerLua* lexer=new QsciLexerLua;
    _scintillaObject->setLexer(lexer);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETSTYLEBITS,(int)5);
    _scintillaObject->setTabWidth(4);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETUSETABS,(int)0);

    _setKeywords();
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETMARGINWIDTHN,(unsigned long)0,(long)48); // Line numbers
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETMARGINWIDTHN,(unsigned long)1,(long)0); // Symbols
    connect(_scintillaObject,SIGNAL(SCN_CHARADDED(int)),this,SLOT(_charAdded(int)));
    connect(_scintillaObject,SIGNAL(SCN_UPDATEUI(int)),this,SLOT(_updateUi(int)));

    QShortcut* shortcut = new QShortcut(QKeySequence(tr("Ctrl+f", "Find")), this);
    connect(shortcut,SIGNAL(activated()), this, SLOT(_onFind()));
//  QShortcut* shortcut2 = new QShortcut(QKeySequence(tr("Ctrl+shift+f", "Find")), this);
//  connect(shortcut2,SIGNAL(activated()), this, SLOT(_onFind2()));

    _setColorsAndMainStyles();
}

CScintillaModalDlg::~CScintillaModalDlg() 
{
    delete _scintillaObject;
}

bool CScintillaModalDlg::initialize(int scriptId,const char* titleText,bool readOnly,bool ignoreSceneLock)
{
    if ((App::operationalUIParts&sim_gui_scripteditor)==0)
        return(false);

    // Make sure that scripts cannot be viewed/edited with locked scenes:
    if (App::ct->environment->getSceneLocked()&&(!ignoreSceneLock))
    {
        App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Script"),strTranslate(IDS_SCENE_IS_LOCKED_WARNING_NO_SCRIPT_VIEWING),VMESSAGEBOX_OKELI);
        return(false);
    }

    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptId);
    if (it==NULL)
        return(false);

    _scriptID=scriptId;

    int posAndSize[4];
    it->getPreviousEditionWindowPosAndSize(posAndSize);

    move(posAndSize[0],posAndSize[1]);
    resize(posAndSize[2],posAndSize[3]);

    setWindowTitle(titleText);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETTEXT,(unsigned long)0,it->getScriptText(&_tempFoldingThing));
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_EMPTYUNDOBUFFER); // Make sure the undo buffer is empty (otherwise the first undo will remove the whole script --> a bit confusing)
    if (readOnly)
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETREADONLY,(int)1);

    // When the window is opened, we first scroll to the end of the document, otherwise the folding doesn't always work somehow. Folding happens later, in _updateUi
    int totLines=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETLINECOUNT);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_GOTOLINE,(int)totLines);

    return(true);
}

void CScintillaModalDlg::_setColorsAndMainStyles()
{ // backgroundStyle=0 Main script, 1=non-threaded, 2=threaded
    struct SScintillaColors
    {
        int iItem;
        unsigned int rgb;
    };
    const unsigned int black=RGB(0,0,0);
    const unsigned int darkGrey=RGB(70,70,70);
    const unsigned int white=RGB(255,255,255);

    // Values initialized with colors for the customization script:
    unsigned int colBackground=RGBW(App::userSettings->customizationScriptColor_background);
    unsigned int colSelection=RGBW(App::userSettings->customizationScriptColor_selection);
    unsigned int colComment=RGBW(App::userSettings->customizationScriptColor_comment);
    unsigned int colNumber=RGBW(App::userSettings->customizationScriptColor_number);
    unsigned int colString=RGBW(App::userSettings->customizationScriptColor_string);
    unsigned int colCharacter=RGBW(App::userSettings->customizationScriptColor_character);
    unsigned int colOperator=RGBW(App::userSettings->customizationScriptColor_operator);
    unsigned int colPreprocessor=RGBW(App::userSettings->customizationScriptColor_preprocessor);
    unsigned int colIdentifier=RGBW(App::userSettings->customizationScriptColor_identifier);
    unsigned int colWord=RGBW(App::userSettings->customizationScriptColor_word);
    unsigned int colWord2=RGBW(App::userSettings->customizationScriptColor_word2);
    unsigned int colWord3=RGBW(App::userSettings->customizationScriptColor_word3);
    unsigned int colWord4=RGBW(App::userSettings->customizationScriptColor_word4);

    if (_scriptType==sim_scripttype_addonscript)
    {
        colBackground=RGBW(App::userSettings->addOnScriptColor_background);
        colSelection=RGBW(App::userSettings->addOnScriptColor_selection);
        colComment=RGBW(App::userSettings->addOnScriptColor_comment);
        colNumber=RGBW(App::userSettings->addOnScriptColor_number);
        colString=RGBW(App::userSettings->addOnScriptColor_string);
        colCharacter=RGBW(App::userSettings->addOnScriptColor_character);
        colOperator=RGBW(App::userSettings->addOnScriptColor_operator);
        colPreprocessor=RGBW(App::userSettings->addOnScriptColor_preprocessor);
        colIdentifier=RGBW(App::userSettings->addOnScriptColor_identifier);
        colWord=RGBW(App::userSettings->addOnScriptColor_word);
        colWord2=RGBW(App::userSettings->addOnScriptColor_word2);
        colWord3=RGBW(App::userSettings->addOnScriptColor_word3);
        colWord4=RGBW(App::userSettings->addOnScriptColor_word4);
    }

    static SScintillaColors syntaxColors[]= 
    {
        {SCE_LUA_COMMENT,colComment},
        {SCE_LUA_COMMENTLINE,colComment},
        {SCE_LUA_COMMENTDOC,colComment},
        {SCE_LUA_NUMBER,colNumber},
        {SCE_LUA_STRING,colString},
        {SCE_LUA_LITERALSTRING,colString},
        {SCE_LUA_CHARACTER,colCharacter},
        {SCE_LUA_OPERATOR,colOperator},
        {SCE_LUA_PREPROCESSOR,colPreprocessor},
        {SCE_LUA_WORD,colWord},
        {SCE_LUA_WORD2,colWord2},
        {SCE_LUA_WORD3,colWord3},
        {SCE_LUA_WORD4,colWord4},
        {SCE_LUA_IDENTIFIER,colIdentifier},
        {-1,0}
    };


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

    _setAStyle(QsciScintillaBase::STYLE_DEFAULT,black,colBackground,fontSize,theFont.c_str()); // set global default style

    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETCARETFORE,(unsigned long)black);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLECLEARALL); // set all styles
    _setAStyle(QsciScintillaBase::STYLE_LINENUMBER,white,darkGrey);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETSELBACK,(unsigned long)1,(long)colSelection); // selection color

    // Set syntax colors
    for (int i=0;syntaxColors[i].iItem!=-1;i++)
        _setAStyle(syntaxColors[i].iItem,syntaxColors[i].rgb,colBackground);

    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_INDICSETSTYLE,(unsigned long)20,(long)QsciScintillaBase::INDIC_STRAIGHTBOX);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_INDICSETALPHA,(unsigned long)20,(long)160);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_INDICSETFORE,(unsigned long)20,(long)colSelection);
    _scintillaObject->setFolding(QsciScintilla::BoxedTreeFoldStyle);
}

void CScintillaModalDlg::makeDialogModal()
{
    setModal(true);
    exec();
    CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(_scriptID);
    int l=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETLENGTH);
    char* buff=new char[l+1];
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETTEXT,(unsigned long)l+1,buff);

    std::vector<int> foldLev=_unfold();
    _fold(foldLev);

    it->setScriptText(buff,&foldLev);

    int posAndSize[4];
    QRect geom(geometry());
    posAndSize[0]=geom.x();
    posAndSize[1]=geom.y();
    posAndSize[2]=geom.width();
    posAndSize[3]=geom.height();
    it->setPreviousEditionWindowPosAndSize(posAndSize);

    delete[] buff;
}

void CScintillaModalDlg::_setAStyle(int style,unsigned int fore,unsigned int back,int size,const char *face)
{
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETFORE,(unsigned long)style,(long)fore);
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETBACK,(unsigned long)style,(long)back);
    if (size>=1)
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETSIZE,(unsigned long)style,(long)size);
    if (face)
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETFONT,(unsigned long)style,face);
}

void CScintillaModalDlg::_setKeywords()
{
    if (vrepKeywords1.size()==0)
    {
        for (size_t i=0;simLuaCommands[i].name!="";i++)
        {
            vrepKeywords1+=simLuaCommands[i].name;
            vrepKeywords1+=" ";
        }
        App::ct->luaCustomFuncAndVarContainer->appendAllFunctionNames_spaceSeparated(vrepKeywords1);
    }
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETKEYWORDS,(unsigned long)1,vrepKeywords1.c_str());
    if (vrepKeywords2.size()==0)
    {
        for (size_t i=0;simLuaVariables[i].name!="";i++)
        {
            vrepKeywords2+=simLuaVariables[i].name;
            vrepKeywords2+=" ";
        }
        App::ct->luaCustomFuncAndVarContainer->appendAllVariableNames_spaceSeparated(vrepKeywords2);
    }
    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETKEYWORDS,(unsigned long)2,vrepKeywords2.c_str());
}

void CScintillaModalDlg::_prepAutoCompletionList(const std::string& txt)
{
    _autoCompletionList.clear();
    std::vector<std::string> t;
    for (int i=0;simLuaCommands[i].name!="";i++)
    {
        if ((simLuaCommands[i].name.size()>=txt.size())&&(simLuaCommands[i].name.compare(0,txt.size(),txt)==0))
            t.push_back(simLuaCommands[i].name);
    }

    for (int i=0;simLuaVariables[i].name!="";i++)
    {
        if ((simLuaVariables[i].name.size()>=txt.size())&&(simLuaVariables[i].name.compare(0,txt.size(),txt)==0))
            t.push_back(simLuaVariables[i].name);
    }

    for (int i=0;i<int(App::ct->luaCustomFuncAndVarContainer->allCustomFunctions.size());i++)
    {
        std::string n=App::ct->luaCustomFuncAndVarContainer->allCustomFunctions[i]->getFunctionName();
        if ((n.size()>=txt.size())&&(n.compare(0,txt.size(),txt)==0))
            t.push_back(n);
    }

    App::ct->luaCustomFuncAndVarContainer->pushAllVariableNamesThatStartSame(txt,t);
    std::sort(t.begin(),t.end());

    for (int i=0;i<int(t.size());i++)
    {
        _autoCompletionList+=t[i];
        if (i!=int(t.size()-1))
            _autoCompletionList+=' ';
    }
}

std::string CScintillaModalDlg::_getCallTip(const char* txt)
{ 
    std::string retVal="";
    int l=strlen(txt);
    for (int i=0;simLuaCommands[i].name!="";i++)
    {
        if (int(simLuaCommands[i].name.size())==l)
        {
            bool same=true;
            for (int j=0;j<l;j++)
            {
                if (txt[j]!=simLuaCommands[i].name[j])
                {
                    same=false;
                    break;
                }
            }
            if (same)
                return(simLuaCommands[i].callTip);
        }
    }
    // Following is not elegant (almost same code as above)
    for (int i=0;i<int(App::ct->luaCustomFuncAndVarContainer->allCustomFunctions.size());i++)
    {
        std::string n=App::ct->luaCustomFuncAndVarContainer->allCustomFunctions[i]->getFunctionName();
        if (int(n.size())==l)
        {
            bool same=true;
            for (int j=0;j<l;j++)
            {
                if (txt[j]!=n[j])
                {
                    same=false;
                    break;
                }
            }
            if (same)
                return(App::ct->luaCustomFuncAndVarContainer->allCustomFunctions[i]->getCallTips());
        }
    }
    return(retVal);
}

void CScintillaModalDlg::_updateUi(int updated)
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

    if (_tempFoldingThing.size()!=0)
    { // When the window is opened, then we first scroll to the end of the document (in the init2 phase), otherwise the folding doesn't always work somehow
        std::vector<int> tmp(_tempFoldingThing);
        _tempFoldingThing.clear();
        _fold(tmp);
        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_GOTOLINE,(int)0); // scroll back to the beginning of the document
    }
}

void CScintillaModalDlg::_onFind()
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

void CScintillaModalDlg::_findText(const char* txt,bool caseSensitive)
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
    {
        // We might have to unfold the code:
        int theLine=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_LINEFROMPOSITION,(int)p);
        for (int i=0;i<64;i++)
        { // not really trusting the unfold... while might hang?!
            if (_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETLINEVISIBLE,(int)theLine)==0)
                _scintillaObject->foldLine(theLine);
            else
                break;
        }

        _scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETSEL,(unsigned long)p,(long)p+txtL);
    }
}


std::vector<int> CScintillaModalDlg::_unfold()
{
    int totLines=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETLINECOUNT);
    std::vector<int> retV(totLines,0);
    for (int i=0;i<totLines;i++)
    {
        if (_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETLINEVISIBLE,(int)i)==0)
        {
            std::vector<int> before(totLines,0);
            for (int j=i;j<totLines;j++)
                before[j]=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETLINEVISIBLE,(int)j);
            _scintillaObject->foldLine(i-1); // unfold this section (we actually unfold the previous line that is already visible)
            int lastChangedLine=i;
            for (int j=i;j<totLines;j++)
            {
                int isVisible=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETLINEVISIBLE,(int)j);
                if (isVisible!=before[j])
                    lastChangedLine=j;
            }

            for (int j=i;j<=lastChangedLine;j++)
                retV[j]++;
        }
    }
    return(retV);
}

void CScintillaModalDlg::_fold(const std::vector<int>& foldingSt)
{
    std::vector<int> foldingState(foldingSt);
    int totLines=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETLINECOUNT);
    if (totLines>int(foldingState.size()))
        return;
    int index=_fold_getHighestIndex(foldingState);
    while (index!=-1)
    {
        _scintillaObject->foldLine(index-1); // fold this section (we actually fold the previous line that will remain visible)
        _fold_overwriteFromIndex(foldingState,index,foldingState[index-1]);
        index=_fold_getHighestIndex(foldingState);
    }
}

int CScintillaModalDlg::_fold_getHighestIndex(std::vector<int>& foldingState)
{
    int highest=0;
    int index=-1;
    for (int i=0;i<int(foldingState.size());i++)
    {
        if (foldingState[i]>highest)
        {
            highest=foldingState[i];
            index=i;
        }
    }
    return(index);
}

void CScintillaModalDlg::_fold_overwriteFromIndex(std::vector<int>& foldingState,int index,int newValue)
{
    int v=foldingState[index];
    for (int i=index;i<int(foldingState.size());i++)
    {
        if (foldingState[i]==v)
            foldingState[i]=newValue;
        else
            break;
    }
}

void CScintillaModalDlg::_charAdded(int charAdded)
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
