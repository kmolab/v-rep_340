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
#include "oGL.h"
#include "glShader.h"

CGlShader::CGlShader()
{
    _shaderProgram.addShaderFromSourceFile(QGLShader::Vertex,"vertexShaderTest.txt");
    _shaderProgram.addShaderFromSourceFile(QGLShader::Fragment,"fragmentShaderTest.txt");
    /*
    _shaderProgram.addShaderFromSourceCode(QGLShader::Vertex,
                                           "varying vec4 diffuse,ambient;\n"
                                           "varying vec3 normal,halfVector;\n"
                                           " \n"
                                           "void main()\n"
                                           "{\n"
                                           "    \n"
                                           "    normal = normalize(gl_NormalMatrix * gl_Normal);\n"
                                           " \n"
                                           "    \n"
                                           "    halfVector = gl_LightSource[0].halfVector.xyz;\n"
                                           " \n"
                                           "    \n"
                                           "    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;\n"
                                           "    ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;\n"
                                           "    ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;\n"
                                           "    gl_Position = ftransform();\n"
                                           " \n"
                                           "}");
    _shaderProgram.addShaderFromSourceCode(QGLShader::Fragment,
                                           "varying vec4 diffuse,ambient;\n"
                                           "varying vec3 normal,halfVector;\n"
                                           " \n"
                                           "void main()\n"
                                           "{\n"
                                           "    vec3 n,halfV,lightDir;\n"
                                           "    float NdotL,NdotHV;\n"
                                           " \n"
                                           "    lightDir = vec3(gl_LightSource[0].position);\n"
                                           " \n"
                                           "    \n"
                                           "    vec4 color = ambient;\n"
                                           "    \n"
                                           "    n = normalize(normal);\n"
                                           "    \n"
                                           " \n"
                                           "    NdotL = max(dot(n,lightDir),0.0);\n"
                                            "      if (NdotL > 0.0) {\n"
                                            "       color += diffuse * NdotL;\n"
                                            "       halfV = normalize(halfVector);\n"
                                            "       NdotHV = max(dot(n,halfV),0.0);\n"
                                            "       color += gl_FrontMaterial.specular *\n"
                                            "               gl_LightSource[0].specular *\n"
                                            "               pow(NdotHV, gl_FrontMaterial.shininess);\n"
                                            "   }\n"
                                            "\n"
                                            "   gl_FragColor = color;\n"
                                               "}");
*/

                                               /*
    _shaderProgram.addShaderFromSourceCode(QGLShader::Vertex,
        "attribute vec4 in_vertex;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "uniform mat4 model;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * in_vertex;\n"
        "}");
    _shaderProgram.addShaderFromSourceCode(QGLShader::Fragment,
        "void main()\n"
        "{\n"
        "   gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}");
    */
    if (_shaderProgram.link())
    {
        printf("Link successful.\n");
    }
    else
    {
        printf("Link failed: %s\n",_shaderProgram.log().toStdString().c_str());
    }
}

CGlShader::~CGlShader()
{
    _shaderProgram.release();
}

bool CGlShader::bind()
{
    bool result=_shaderProgram.bind();
    if (result)
        printf("Bind successful.\n");
    else
        printf("Bind failed.\n");
    return(result);
}
