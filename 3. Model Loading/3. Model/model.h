//
// Created by Lijun Wang on 2022/6/11.
//

#ifndef OPENGL_CLION_MODEL_H
#define OPENGL_CLION_MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image.h"

//Assimp
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

#include "shader_s.h"
#include "mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model{
public:
    vector<Texture> textures_loaded;
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    Model(string const &path, bool gamma = false):gammaCorrection(gamma)
    {
        loadModel(path);
    }

    void Draw(Shader &shader)
    {
        for(unsigned int i=0;i<meshes.size();++i)
        {
            meshes[i].Draw(shader);
        }
    }

private:

    void loadModel(string const &path)
    {
        //读入模型文件
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        //const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);


        //检查是否读取错误
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
            return;
        }

        //获取模型所在目录(从string第一位开始，到最后一次出现/之前的，截取为子string，赋值给directory)
        directory = path.substr(0, path.find_last_of("/"));

        //处理节点
        processNode(scene->mRootNode, scene);

  }

    //这是一个递归函数 直至没有子节点 递归结束
    void processNode(aiNode *node, const aiScene *scene)
    {
        //获取并处理每一个node下的mesh
        for(unsigned int i=0; i<node->mNumMeshes; ++i)
        {
            //node->mMeshes[i]是当前node下mesh的index，获取了index后在scene的mMeshes库中索引到对应的模型
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            //处理mesh，并将处理后的mesh存储至meshes vector中
            meshes.push_back(processMesh(mesh, scene));
        }
        for(unsigned int i=0; i<node->mNumChildren; ++i)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        //遍历所有mesh中的所有顶点 设置所有的vertices
        for(unsigned int i=0; i<mesh->mNumVertices; ++i)
        {
            Vertex vertex;
            glm::vec3 vector;

            //position assimp顶点位置数组名称是mVertices
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            //normal
            if(mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            //texture coordinate assimp每个模型可以拥有最多8组纹理坐标，此处我们提取第一组
            if(mesh->mTextureCoords[0])
            {
                glm::vec2 vec;

                //texture coordinate
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoord = vec;

                //tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;

                //bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
            {
                vertex.TexCoord = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        //获取顶点索引 设置所有的indices
        for(unsigned int i=0; i<mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j=0; j<face.mNumIndices; ++j)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        //设置所有的textures 对应mesh的材质
        if(mesh->mMaterialIndex >=0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];//获取当前mesh对应的material

            //diffuseMap
            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            //specularMap
            vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            //normalMap
            vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            //HeightMap
            vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        //基于模型文件数据，创建mesh对象，与mesh类结合
        return Mesh(vertices, indices, textures);
    }

    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for(unsigned int i=0; i<mat->GetTextureCount(type); ++i)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false; //检查当前texture是否与之前读入的重复，如果重复，则跳过texture加载过程，避免重复
            for(unsigned int j=0; j<textures_loaded.size(); ++j)
            {
                //当前texture已重复出现过
                if(strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break; //当遇到 break 时，循环立即停止，程序跳转到循环后面的语句 if(!skip) 如果是continue 不执行之后的语句，直接进入下一次循环
                }
            }
            if(!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }

        }

        return textures;
    }
};

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    if(data)
    {
        GLenum format;
        if(nrComponents == 1)
        {
            format = GL_RED;
        }
        if(nrComponents == 3)
        {
            format = GL_RGB;
        }
        if(nrComponents == 4)
        {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        cout << "Texture failed to load at path: " << path << endl;
        stbi_image_free(data);
    }

    return textureID;

}

#endif //OPENGL_CLION_MODEL_H
