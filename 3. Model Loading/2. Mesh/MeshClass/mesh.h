//
// Created by Lijun Wang on 2022/6/7.
//

#ifndef OPENGL_CLION_MESH_H
#define OPENGL_CLION_MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader_s.h"

#include <string>
#include <vector>
using namespace std;

struct Vertex{
    //位置
    glm::vec3 Position;
    //法线
    glm::vec3 Normal;
    //纹理坐标
    glm::vec2 TexCoord;
};

struct Texture{
    //id
    unsigned int id;
    //类型：diffuse specular
    string type;
};

class Mesh{
public:
    //网格数据
    vector<Vertex> vertices; //顶点
    vector<unsigned int> indices; //索引
    vector<Texture> textures; //贴图

    //构造函数
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        //网格数据传入并赋值
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        //设置opengl绘制需要的数据 vao vbo ebo
        setUpMesh();
    }

    //绘制函数
    void Draw(Shader &shader)
    {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;

        for(unsigned int i=0; i < textures.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0+i);
            string number;
            string name = textures[i].type;
            if(name == "texture_diffuse")
            {
                number = std::to_string(diffuseNr++);//先使用diffuseNr转化为string，再将diffuseNr的值+1
            }
            else if(name == "texture_specular")
            {
                number = std::to_string(specularNr++);
            }

            shader.setInt(("material." + name+number).c_str(), i);//将texture sampler属性传输至着色器
            glBindTexture(GL_TEXTURE_2D, textures[i].id); //绑定的是texture 的ID
        }

        //首次绘制时绑定纹理
        glActiveTexture(GL_TEXTURE0);//保证初始状态正确

        //绘制
        glBindVertexArray(VAO);//绑定vao
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);//绘制
        glBindVertexArray(0);//解绑vao

    }

private:
    //显卡渲染存储数据
    unsigned int VAO, VBO, EBO;

    //模型数据设置
    void setUpMesh()
    {
        //VAO, VBO, EBO创建
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        //VAO, VBO绑定
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        //VBO数据传入
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        //EBO绑定及索引数据传入
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        //顶点属性数据指针设置 传入着色器
        //顶点位置属性
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        //顶点法线属性
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        //顶点纹理坐标属性
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));

        //VAO解绑
        glBindVertexArray(0);

    };


};








#endif //OPENGL_CLION_MESH_H
