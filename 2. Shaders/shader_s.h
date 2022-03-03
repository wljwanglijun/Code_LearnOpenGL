//
// Created by Lijun Wang on 2022/2/28.
//

#ifndef OPENGL_CLION_SHADER_S_H
#define OPENGL_CLION_SHADER_S_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

class Shader
{
public:
    //shader program id
    unsigned int ID;

    //shader constructor: read and build the shader
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        //从文件中读取shader
        string vertexCode;
        string fragmentCode;
        ifstream vShaderFile;
        ifstream fShaderFile;

        vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        try
        {
            //打开着色器文件
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);

            //将着色器文件写入stringStream
            stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            //关闭文件
            vShaderFile.close();
            fShaderFile.close();

            //将stringStream转化为string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (ifstream::failure& e) {
            cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << endl;
        }
        //将string转化为字符串数组
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        /*编译shader*/
        //创建并编译顶点着色器
        unsigned int vertex;
        unsigned int fragment;
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        //创建并编译片段着色器
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        //shader program
        ID = glCreateProgram(); //创建shader program object
        glAttachShader(ID, vertex);//将顶点着色器附加到shader program
        glAttachShader(ID, fragment);//将片段着色器附加到shader program
        glLinkProgram(ID);//链接shader program object
        checkCompileErrors(ID, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);//shader program链接成功后就可以删除顶点着色器和片段着色器
    }

    void use()
    {
        glUseProgram(ID);
    }

    void setBool(const string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setInt(const string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat(const string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

private:
    void checkCompileErrors(unsigned int shader, string type)
    {
        int success;
        char infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                cout<<"ERROR::SHADER_COMPILATION_ERROR of type" << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                cout<<"ERROR::PROGRAM_LINK_ERROR of type" << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
            }
        }
    }
};



#endif //OPENGL_CLION_SHADER_S_H
