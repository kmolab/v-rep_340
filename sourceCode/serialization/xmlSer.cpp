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

#include "vrepMainHeader.h"
#include "xmlSer.h"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>


CXmlSer::CXmlSer(const char* filename)
{
    _filename=filename;
}

CXmlSer::~CXmlSer()
{
}

xmlNode* CXmlSer::writeOpen()
{
    xmlNode* mainNode=_xmlDocument.NewElement("V-REP");
    _xmlDocument.InsertFirstChild(mainNode);
    return(mainNode);
}

bool CXmlSer::writeClose()
{
    return(_xmlDocument.SaveFile(_filename.c_str(),false)==tinyxml2::XML_NO_ERROR);
}

xmlNode* CXmlSer::readOpen()
{
    if (_xmlDocument.LoadFile(_filename.c_str())==tinyxml2::XML_NO_ERROR)
        return(_xmlDocument.FirstChildElement("V-REP"));
    return(NULL);
}

xmlNode* CXmlSer::createNode(const char* name)
{
    return(_xmlDocument.NewElement(name));
}

void CXmlSer::insertFirstChildNode(xmlNode* parentNode,xmlNode* nodeToInsert)
{
    parentNode->InsertFirstChild(nodeToInsert);
}

void CXmlSer::insertNodeAfterSiblingNode(xmlNode* parentNode,xmlNode* siblingNode,xmlNode* nodeToInsert)
{
    parentNode->InsertAfterChild(siblingNode,nodeToInsert);
}

xmlNode* CXmlSer::getFirstChildNode(xmlNode* parentNode,const char* name)
{
    return(parentNode->FirstChildElement(name));
}

xmlNode* CXmlSer::getNodeAfterSiblingNode(xmlNode* siblingNode,const char* name)
{
    return(siblingNode->NextSiblingElement(name));
}

void CXmlSer::addWarningMessage(const char* msg)
{
    std::string m("Warning: ");
    m+=msg;
    addMessage(m.c_str());
}

void CXmlSer::addErrorMessage(const char* msg)
{
    std::string m("Error: ");
    m+=msg;
    addMessage(m.c_str());
}

void CXmlSer::addMessage(const char* msg,unsigned int indent/*=0*/)
{
    if (_messages.size()!=0)
        _messages+="\n";
    for (unsigned int i=0;i<indent;i++)
        _messages+="    ";
    _messages+=msg;
}

std::string CXmlSer::getMessages()
{
    return(_messages);
}





void CXmlSer::addBoolAttribute(xmlNode* node,const char* attribName,bool attrib)
{
    node->SetAttribute(attribName,attrib);
}

void CXmlSer::addIntAttribute(xmlNode* node,const char* attribName,int attrib)
{
    node->SetAttribute(attribName,attrib);
}

void CXmlSer::addInt2Attribute(xmlNode* node,const char* attribName,const int attrib[2])
{
    addStringAttribute(node,attribName,boost::str(boost::format("%i %i") % attrib[0] % attrib[1]));
}

void CXmlSer::addInt2Attribute(xmlNode* node,const char* attribName,int attrib1,int attrib2)
{
    addStringAttribute(node,attribName,boost::str(boost::format("%i %i") % attrib1 % attrib2));
}

void CXmlSer::addInt2Attribute(xmlNode* node,const char* attribName,const std::vector<int>& attrib)
{
    addStringAttribute(node,attribName,boost::str(boost::format("%i %i") % attrib[0] % attrib[1]));
}

void CXmlSer::addInt3Attribute(xmlNode* node,const char* attribName,const int attrib[3])
{
    addStringAttribute(node,attribName,boost::str(boost::format("%i %i %i") % attrib[0] % attrib[1] % attrib[2]));
}

void CXmlSer::addInt3Attribute(xmlNode* node,const char* attribName,int attrib1,int attrib2,int attrib3)
{
    addStringAttribute(node,attribName,boost::str(boost::format("%i %i %i") % attrib1 % attrib2 % attrib3));
}

void CXmlSer::addInt3Attribute(xmlNode* node,const char* attribName,const std::vector<int>& attrib)
{
    addStringAttribute(node,attribName,boost::str(boost::format("%i %i %i") % attrib[0] % attrib[1] % attrib[2]));
}

void CXmlSer::addIntArrayAttribute(xmlNode* node,const char* attribName,const int* attrib,unsigned int cnt)
{
    std::string str;
    for (unsigned int i=0;i<cnt;i++)
    {
        if (i!=0)
            str+=' ';
        str+=boost::str(boost::format("%i") % attrib[i]);
    }
    addStringAttribute(node,attribName,str);
}

void CXmlSer::addIntVectorAttribute(xmlNode* node,const char* attribName,const std::vector<int>& attrib)
{
    std::string str;
    for (size_t i=0;i<attrib.size();i++)
    {
        if (i!=0)
            str+=' ';
        str+=boost::str(boost::format("%i") % attrib[i]);
    }
    addStringAttribute(node,attribName,str);
}

void CXmlSer::addFloatAttribute(xmlNode* node,const char* attribName,double attrib)
{
    node->SetAttribute(attribName,attrib);
}

void CXmlSer::addFloat2Attribute(xmlNode* node,const char* attribName,const float attrib[2])
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f") % attrib[0] % attrib[1]));
}

void CXmlSer::addFloat2Attribute(xmlNode* node,const char* attribName,const double attrib[2])
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f") % attrib[0] % attrib[1]));
}

void CXmlSer::addFloat2Attribute(xmlNode* node,const char* attribName,double attrib1,double attrib2)
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f") % attrib1 % attrib2));
}

void CXmlSer::addFloat2Attribute(xmlNode* node,const char* attribName,const std::vector<float>& attrib)
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f") % attrib[0] % attrib[1]));
}

void CXmlSer::addFloat2Attribute(xmlNode* node,const char* attribName,const std::vector<double>& attrib)
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f") % attrib[0] % attrib[1]));
}

void CXmlSer::addFloat3Attribute(xmlNode* node,const char* attribName,const float attrib[3])
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f %f") % attrib[0] % attrib[1] % attrib[2]));
}

void CXmlSer::addFloat3Attribute(xmlNode* node,const char* attribName,const double attrib[3])
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f %f") % attrib[0] % attrib[1] % attrib[2]));
}

void CXmlSer::addFloat3Attribute(xmlNode* node,const char* attribName,double attrib1,double attrib2,double attrib3)
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f %f") % attrib1 % attrib2 % attrib3));
}

void CXmlSer::addFloat3Attribute(xmlNode* node,const char* attribName,const std::vector<float>& attrib)
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f %f") % attrib[0] % attrib[1] % attrib[2]));
}

void CXmlSer::addFloat3Attribute(xmlNode* node,const char* attribName,const std::vector<double>& attrib)
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f %f") % attrib[0] % attrib[1] % attrib[2]));
}

void CXmlSer::addFloat4Attribute(xmlNode* node,const char* attribName,const float attrib[4])
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f %f %f") % attrib[0] % attrib[1] % attrib[2] % attrib[3]));
}

void CXmlSer::addFloat4Attribute(xmlNode* node,const char* attribName,const double attrib[4])
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f %f %f") % attrib[0] % attrib[1] % attrib[2] % attrib[3]));
}

void CXmlSer::addFloat4Attribute(xmlNode* node,const char* attribName,double attrib1,double attrib2,double attrib3,double attrib4)
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f %f %f") % attrib1 % attrib2 % attrib3 % attrib4));
}

void CXmlSer::addFloat4Attribute(xmlNode* node,const char* attribName,const std::vector<float>& attrib)
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f %f %f") % attrib[0] % attrib[1] % attrib[2] % attrib[3]));
}

void CXmlSer::addFloat4Attribute(xmlNode* node,const char* attribName,const std::vector<double>& attrib)
{
    addStringAttribute(node,attribName,boost::str(boost::format("%f %f %f %f") % attrib[0] % attrib[1] % attrib[2] % attrib[3]));
}

void CXmlSer::addFloatVectorAttribute(xmlNode* node,const char* attribName,const std::vector<float>& attrib)
{
    std::string str;
    for (size_t i=0;i<attrib.size();i++)
    {
        if (i!=0)
            str+=' ';
        str+=boost::str(boost::format("%f") % attrib[i]);
    }
    addStringAttribute(node,attribName,str);
}

void CXmlSer::addFloatVectorAttribute(xmlNode* node,const char* attribName,const std::vector<double>& attrib)
{
    std::string str;
    for (size_t i=0;i<attrib.size();i++)
    {
        if (i!=0)
            str+=' ';
        str+=boost::str(boost::format("%f") % attrib[i]);
    }
    addStringAttribute(node,attribName,str);
}

void CXmlSer::addFloatArrayAttribute(xmlNode* node,const char* attribName,const float* attrib,unsigned int cnt)
{
    std::string str;
    for (unsigned int i=0;i<cnt;i++)
    {
        if (i!=0)
            str+=' ';
        str+=boost::str(boost::format("%f") % attrib[i]);
    }
    addStringAttribute(node,attribName,str);
}

void CXmlSer::addFloatArrayAttribute(xmlNode* node,const char* attribName,const double* attrib,unsigned int cnt)
{
    std::string str;
    for (unsigned int i=0;i<cnt;i++)
    {
        if (i!=0)
            str+=' ';
        str+=boost::str(boost::format("%f") % attrib[i]);
    }
    addStringAttribute(node,attribName,str);
}

void CXmlSer::addStringAttribute(xmlNode* node,const char* attribName,const char* attrib)
{
    node->SetAttribute(attribName,attrib);
}

void CXmlSer::addStringAttribute(xmlNode* node,const char* attribName,const std::string& attrib)
{
    node->SetAttribute(attribName,attrib.c_str());
}


bool CXmlSer::getBoolAttribute(const xmlNode* node,const char* attribName,bool& attrib)
{
    const char* str=node->Attribute(attribName);
    attrib=false;
    if (str!=NULL)
    {
        try
        {
            attrib=boost::lexical_cast<bool>(str);
            return(true);
        }
        catch (boost::bad_lexical_cast &)
        {
            return(false);
        }
    }
    return(false);
}

bool CXmlSer::getBoolAttribute(const xmlNode* node,const char* attribName)
{
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        try
        {
            return(boost::lexical_cast<bool>(str));
        }
        catch (boost::bad_lexical_cast &)
        {
            return(false);
        }
    }
    return(false);
}

bool CXmlSer::getIntAttribute(const xmlNode* node,const char* attribName,int& attrib)
{
    const char* str=node->Attribute(attribName);
    attrib=0;
    if (str!=NULL)
    {
        try
        {
            attrib=boost::lexical_cast<int>(str);
            return(true);
        }
        catch (boost::bad_lexical_cast &)
        {
            attrib=0;
            return(false);
        }
    }
    return(false);
}

int CXmlSer::getIntAttribute(const xmlNode* node,const char* attribName)
{
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        try
        {
            return(boost::lexical_cast<int>(str));
        }
        catch (boost::bad_lexical_cast &)
        {
            return(0);
        }
    }
    return(0);
}

bool CXmlSer::getInt2Attribute(const xmlNode* node,const char* attribName,int attrib[2])
{
    for (int i=0;i<2;i++)
        attrib[i]=0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (int i=0;i<2;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<int>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

bool CXmlSer::getInt2Attribute(const xmlNode* node,const char* attribName,int& attrib1,int& attrib2)
{
    attrib1=0;
    attrib2=0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        if (ss >> buff)
        {
            try
            {
                attrib1=boost::lexical_cast<int>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib2=boost::lexical_cast<int>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        return(true);
    }
    return(false);
}

bool CXmlSer::getInt2Attribute(const xmlNode* node,const char* attribName,std::vector<int>& attrib)
{
    attrib.resize(2);
    return(getInt2Attribute(node,attribName,&attrib[0]));
}

bool CXmlSer::getInt3Attribute(const xmlNode* node,const char* attribName,int attrib[3])
{
    for (int i=0;i<3;i++)
        attrib[i]=0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (int i=0;i<3;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<int>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

bool CXmlSer::getInt3Attribute(const xmlNode* node,const char* attribName,int& attrib1,int& attrib2,int& attrib3)
{
    attrib1=0;
    attrib2=0;
    attrib3=0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        if (ss >> buff)
        {
            try
            {
                attrib1=boost::lexical_cast<int>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib2=boost::lexical_cast<int>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib3=boost::lexical_cast<int>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        return(true);
    }
    return(false);
}

bool CXmlSer::getInt3Attribute(const xmlNode* node,const char* attribName,std::vector<int>& attrib)
{
    attrib.resize(3);
    return(getInt3Attribute(node,attribName,&attrib[0]));
}

bool CXmlSer::getIntArrayAttribute(const xmlNode* node,const char* attribName,int* attrib,unsigned int cnt)
{
    for (unsigned int i=0;i<cnt;i++)
        attrib[i]=0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (unsigned int i=0;i<cnt;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<int>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

int CXmlSer::getIntVectorAttribute(const xmlNode* node,const char* attribName,std::vector<int>& attrib,unsigned int cnt)
{
    attrib.resize(cnt);
    for (unsigned int i=0;i<cnt;i++)
        attrib[i]=0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (unsigned int i=0;i<cnt;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<int>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloatAttribute(const xmlNode* node,const char* attribName,float& attrib)
{
    attrib=0.0f;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        try
        {
            attrib=boost::lexical_cast<float>(str);
            return(true);
        }
        catch (boost::bad_lexical_cast &)
        {
            return(false);
        }
    }
    return(false);
}

bool CXmlSer::getFloatAttribute(const xmlNode* node,const char* attribName,double& attrib)
{
    attrib=0.0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        try
        {
            attrib=boost::lexical_cast<double>(str);
            return(true);
        }
        catch (boost::bad_lexical_cast &)
        {
            return(false);
        }
    }
    return(false);
}


double CXmlSer::getFloatAttribute(const xmlNode* node,const char* attribName)
{
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        try
        {
            return(boost::lexical_cast<double>(str));
        }
        catch (boost::bad_lexical_cast &)
        {
            return(0.0);
        }
    }
    return(0.0);
}

bool CXmlSer::getFloat2Attribute(const xmlNode* node,const char* attribName,float attrib[2])
{
    for (int i=0;i<2;i++)
        attrib[i]=0.0f;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (int i=0;i<2;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<float>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloat2Attribute(const xmlNode* node,const char* attribName,double attrib[2])
{
    for (int i=0;i<2;i++)
        attrib[i]=0.0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (int i=0;i<2;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<double>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloat2Attribute(const xmlNode* node,const char* attribName,float& attrib1,float& attrib2)
{
    attrib1=0.0f;
    attrib2=0.0f;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        if (ss >> buff)
        {
            try
            {
                attrib1=boost::lexical_cast<float>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib2=boost::lexical_cast<float>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloat2Attribute(const xmlNode* node,const char* attribName,double& attrib1,double& attrib2)
{
    attrib1=0.0;
    attrib2=0.0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        if (ss >> buff)
        {
            try
            {
                attrib1=boost::lexical_cast<double>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib2=boost::lexical_cast<double>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloat2Attribute(const xmlNode* node,const char* attribName,std::vector<float>& attrib)
{
    attrib.resize(2);
    return(getFloat2Attribute(node,attribName,&attrib[0]));
}

bool CXmlSer::getFloat2Attribute(const xmlNode* node,const char* attribName,std::vector<double>& attrib)
{
    attrib.resize(2);
    return(getFloat2Attribute(node,attribName,&attrib[0]));
}

bool CXmlSer::getFloat3Attribute(const xmlNode* node,const char* attribName,float attrib[3])
{
    for (int i=0;i<3;i++)
        attrib[i]=0.0f;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (int i=0;i<3;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<float>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloat3Attribute(const xmlNode* node,const char* attribName,double attrib[3])
{
    for (int i=0;i<3;i++)
        attrib[i]=0.0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (int i=0;i<3;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<double>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloat3Attribute(const xmlNode* node,const char* attribName,float& attrib1,float& attrib2,float& attrib3)
{
    attrib1=0.0f;
    attrib2=0.0f;
    attrib3=0.0f;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        if (ss >> buff)
        {
            try
            {
                attrib1=boost::lexical_cast<float>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib2=boost::lexical_cast<float>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib3=boost::lexical_cast<float>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloat3Attribute(const xmlNode* node,const char* attribName,double& attrib1,double& attrib2,double& attrib3)
{
    attrib1=0.0;
    attrib2=0.0;
    attrib3=0.0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        if (ss >> buff)
        {
            try
            {
                attrib1=boost::lexical_cast<double>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib2=boost::lexical_cast<double>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib3=boost::lexical_cast<double>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloat3Attribute(const xmlNode* node,const char* attribName,std::vector<float>& attrib)
{
    attrib.resize(3);
    return(getFloat3Attribute(node,attribName,&attrib[0]));
}

bool CXmlSer::getFloat3Attribute(const xmlNode* node,const char* attribName,std::vector<double>& attrib)
{
    attrib.resize(3);
    return(getFloat3Attribute(node,attribName,&attrib[0]));
}

bool CXmlSer::getFloat4Attribute(const xmlNode* node,const char* attribName,float attrib[4])
{
    for (int i=0;i<4;i++)
        attrib[i]=0.0f;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (int i=0;i<4;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<float>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloat4Attribute(const xmlNode* node,const char* attribName,double attrib[4])
{
    for (int i=0;i<4;i++)
        attrib[i]=0.0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (int i=0;i<4;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<double>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloat4Attribute(const xmlNode* node,const char* attribName,float& attrib1,float& attrib2,float& attrib3,float& attrib4)
{
    attrib1=0.0f;
    attrib2=0.0f;
    attrib3=0.0f;
    attrib4=0.0f;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        if (ss >> buff)
        {
            try
            {
                attrib1=boost::lexical_cast<float>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib2=boost::lexical_cast<float>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib3=boost::lexical_cast<float>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib4=boost::lexical_cast<float>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloat4Attribute(const xmlNode* node,const char* attribName,double& attrib1,double& attrib2,double& attrib3,double& attrib4)
{
    attrib1=0.0;
    attrib2=0.0;
    attrib3=0.0;
    attrib4=0.0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        if (ss >> buff)
        {
            try
            {
                attrib1=boost::lexical_cast<double>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib2=boost::lexical_cast<double>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib3=boost::lexical_cast<double>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        if (ss >> buff)
        {
            try
            {
                attrib4=boost::lexical_cast<double>(buff);
            }
            catch (boost::bad_lexical_cast &)
            {
                return(false);
            }
        }
        else
            return(false);
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloat4Attribute(const xmlNode* node,const char* attribName,std::vector<float>& attrib)
{
    attrib.resize(4);
    return(getFloat4Attribute(node,attribName,&attrib[0]));
}

bool CXmlSer::getFloat4Attribute(const xmlNode* node,const char* attribName,std::vector<double>& attrib)
{
    attrib.resize(4);
    return(getFloat4Attribute(node,attribName,&attrib[0]));
}

int CXmlSer::getFloatVectorAttribute(const xmlNode* node,const char* attribName,std::vector<float>& attrib,unsigned int cnt)
{
    attrib.resize(cnt);
    for (unsigned int i=0;i<cnt;i++)
        attrib[i]=0.0f;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (unsigned int i=0;i<cnt;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<float>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

int CXmlSer::getFloatVectorAttribute(const xmlNode* node,const char* attribName,std::vector<double>& attrib,unsigned int cnt)
{
    attrib.resize(cnt);
    for (unsigned int i=0;i<cnt;i++)
        attrib[i]=0.0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (unsigned int i=0;i<cnt;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<double>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloatArrayAttribute(const xmlNode* node,const char* attribName,float* attrib,unsigned int cnt)
{
    for (unsigned int i=0;i<cnt;i++)
        attrib[i]=0.0f;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (unsigned int i=0;i<cnt;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<float>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

bool CXmlSer::getFloatArrayAttribute(const xmlNode* node,const char* attribName,double* attrib,unsigned int cnt)
{
    for (unsigned int i=0;i<cnt;i++)
        attrib[i]=0.0;
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        std::string buff;
        std::stringstream ss(str);
        for (unsigned int i=0;i<cnt;i++)
        {
            if (ss >> buff)
            {
                try
                {
                    attrib[i]=boost::lexical_cast<double>(buff);
                }
                catch (boost::bad_lexical_cast &)
                {
                    return(false);
                }
            }
            else
                return(false);
        }
        return(true);
    }
    return(false);
}

bool CXmlSer::getStringAttribute(const xmlNode* node,const char* attribName,std::string& attrib)
{
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
    {
        attrib=str;
        return(true);
    }
    attrib="";
    return(false);
}

std::string CXmlSer::getStringAttribute(const xmlNode* node,const char* attribName)
{
    const char* str=node->Attribute(attribName);
    if (str!=NULL)
        return(std::string(str));
    return(std::string(""));
}

