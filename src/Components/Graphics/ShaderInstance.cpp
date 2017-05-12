/*
* ShaderInstance
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ShaderInstance.h"
#include "ShaderCache.h"

namespace Dream
{

    ShaderInstance::ShaderInstance(AssetDefinition* definition,Transform3D* transform)
        : IAssetInstance(definition,transform)
    {
        mShaderProgram = 0;
    }

    ShaderInstance::~ShaderInstance()
    {

        if (DEBUG)
        {
            cout << "ShaderInstance: Destroying Object" << endl;
        }

        /*if (mShaderProgram > 0) {
      glDeleteShader(mShaderProgram);
    }
    */
    }

    GLuint ShaderInstance::getShaderProgram()
    {
        return mShaderProgram;
    }

    bool ShaderInstance::load(string projectPath)
    {
        mShaderProgram = ShaderCache::getShader(mDefinition->getUuid());
        if (mShaderProgram == 0)
        {
            string mVertexShaderSource;
            string mFragmentShaderSource;
            GLuint mVertexShader = 0;
            GLuint mFragmentShader = 0;
            // 1. Open Shader Files into Memory
            FileReader *vertexReader, *fragmentReader;
            string absVertexPath, absFragmentPath;
            absVertexPath   = projectPath+mDefinition->getAssetPath() + SHADER_VERTEX;
            absFragmentPath = projectPath+mDefinition->getAssetPath() + SHADER_FRAGMENT;
            vertexReader = new FileReader(absVertexPath);
            vertexReader->readIntoStringStream();
            mVertexShaderSource = vertexReader->getContentsAsString();
            delete vertexReader;
            fragmentReader = new FileReader(absFragmentPath);
            fragmentReader->readIntoStringStream();
            mFragmentShaderSource = fragmentReader->getContentsAsString();
            delete fragmentReader;
            if (DEBUG)
            {
                cout << "ShaderInstance: Loading Shader..." << endl
                     << "ShaderInstance: Vertex: " << absVertexPath     << endl
                     << endl << mVertexShaderSource    << endl
                     << "ShaderInstance: Fragment: " << absFragmentPath   << endl
                     << endl << mFragmentShaderSource  << endl;
            }
            // 2. Compile shaders
            GLint success;
            GLchar infoLog[512];
            // Vertex Shader
            mVertexShader = glCreateShader(GL_VERTEX_SHADER);
            const char *vSource = mVertexShaderSource.c_str();
            glShaderSource(mVertexShader, 1, &vSource, nullptr);
            glCompileShader(mVertexShader);
            // Print compile errors if any
            glGetShaderiv(mVertexShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(mVertexShader, 512, nullptr, infoLog);
                cerr << "ShaderInstance: SHADER:VERTEX:COMPILATION_FAILED\n" << infoLog << endl;
            }
            // Fragment Shader
            mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            const char *fSource = mFragmentShaderSource.c_str();
            glShaderSource(mFragmentShader, 1, &fSource, nullptr);
            glCompileShader(mFragmentShader);
            // Print compile errors if any
            glGetShaderiv(mFragmentShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(mFragmentShader, 512, nullptr, infoLog);
                cerr << "ShaderInstance: SHADER:FRAGMENT:COMPILATION_FAILED\n" << infoLog << endl;
            }
            // Shader Program
            mShaderProgram = glCreateProgram();
            glAttachShader(mShaderProgram, mVertexShader);
            glAttachShader(mShaderProgram, mFragmentShader);
            glLinkProgram(mShaderProgram);
            // Print linking errors if any
            glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(mShaderProgram, 512, nullptr, infoLog);
                cerr << "ShaderInstance: SHADER:PROGRAM:LINKING_FAILED\n" << infoLog << endl;
            }
            // Delete the shaders as they're linked into our program now and no longer necessery
            glDeleteShader(mVertexShader);
            glDeleteShader(mFragmentShader);
            ShaderCache::putShader(mDefinition->getUuid(),mShaderProgram);
        }
        mLoaded = (mShaderProgram != 0);
        return mLoaded;
    }

    void ShaderInstance::use()
    {
        glUseProgram(mShaderProgram);
        syncUniforms();
    }

    void ShaderInstance::loadExtraAttributes(nlohmann::json jsonData)
    {
        // pass
        return;
    }

    // API Setters =============================================================

    void ShaderInstance::syncUniforms()
    {
       syncUniform1f();
    }

    void ShaderInstance::setUniform1f(string location, GLfloat value)
    {
        map<string,GLfloat>::iterator it;
        for (it = mUniform1fMap.begin(); it != mUniform1fMap.end(); it++)
        {
           if (it->first == location)
           {
               it->second = value;
               return;
           }
        }
        mUniform1fMap.insert(pair<string,GLfloat>(location,value));
    }

    // GL Syncros ==============================================================

    // 1f
    void ShaderInstance::syncUniform1f()
    {
        GLint prog = getShaderProgram();
        map<string,GLfloat>::iterator it;

        for (it = mUniform1fMap.begin(); it != mUniform1fMap.end(); it++)
        {
            string name = it->first;
            GLfloat val = it->second;
            GLint location = glGetUniformLocation(prog,name.c_str());
            if (VERBOSE)
            {
                cout << "ShaderInstance: "
                     << getUuid()
                     << " Sync Uinform1f -> "
                     << " prog: " << prog
                     << " name: " << name
                     << " loc: " << location
                     << " val: " << val
                     << endl;
            }
            glUniform1f(location,val);
        }
    }

    void ShaderInstance::exposeLuaApi(lua_State* state)
    {
        luabind::module(state) [
                luabind::class_<ShaderInstance>("ShaderInstance")
                .def("getUuid", &ShaderInstance::getUuid     )

                .def("setUniform1f", &ShaderInstance::setUniform1f )
                ];
    }


} // End of Dream
