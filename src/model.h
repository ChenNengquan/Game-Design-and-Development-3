#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <vector>
using namespace std;
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h > 
#include <assimp/anim.h > 
#include<map>
using namespace Assimp;

class Model {
private:
    vector<GLfloat> vertices;           // 顶点坐标
    vector<GLuint> indices;             // 坐标索引

    GLuint VAO;                         // 模型的缓冲数据

public:
    //普通构造方法
    Model(const string& path) {
        LoadModel(path);
        SetVAO();
    }
    //增加缩放功能的模型构造方法
    Model(const string& path, float scale ) {
        LoadModel(path);
        Scale(scale);
        SetVAO();
    }

    GLuint GetVAO() {
        return VAO;
    }

    vector<GLuint> GetIndices() {
        return indices;
    }

 

private:
    // 从文件中载入模型，使用assimp库操作
    void LoadModel(const string& path) {
        Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals| aiProcess_LimitBoneWeights
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
            return;
        }


        ProcessNode(scene->mRootNode, scene);
    }
    // 对所有节点进行操作
    void ProcessNode(aiNode* node, const aiScene* scene) {

        // 对当前节点的所有网格进行操作
        for (GLuint i = 0; i < node->mNumMeshes; i++) {
            // node中只包含了指向scene中数据的索引
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(mesh, scene);
        }
        // 处理子节点
        for (GLuint i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene);
        }
    }


    // 对节点中的所有网格进行操作
    void ProcessMesh(aiMesh* mesh, const aiScene* scene) {
        // 遍历所有网格的顶点
        for (GLuint i = 0; i < mesh->mNumVertices; i++) {
            // 位置坐标
            vertices.push_back(mesh->mVertices[i].x);
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);

            // 法线坐标
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);

            // 纹理坐标
            if (mesh->mTextureCoords[0]) {
                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            }
            else {
                vertices.push_back(0);
                vertices.push_back(0);
            }
        }

        // 遍历网格的面，获取索引
        for (GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);

        }

    }
    // 将读取的模型数据载入缓冲区，便于后续使用
    void SetVAO() {
        // 生成顶点数组对象、顶点缓冲对象和索引缓冲对象
        GLuint VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        // 绑定顶点数组对象
        glBindVertexArray(VAO);

        // 绑定顶点缓冲对象，并将顶点数据复制到缓冲区中
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        // 绑定索引缓冲对象，并将索引数据复制到缓冲区中
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
            // 启用顶点属性，并指定顶点属性指针
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(3 * sizeof(GLfloat)));

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(6 * sizeof(GLfloat)));

            // 解绑顶点数组对象
            glBindVertexArray(0);

    }



    // 缩放模型函数
    void Scale(float scale) {
        for (unsigned int i = 0; i < vertices.size(); i += 8) {
            vertices[i] *= scale;        // 缩放 x 坐标
            vertices[i + 1] *= scale;    // 缩放 y 坐标
            vertices[i + 2] *= scale;    // 缩放 z 坐标
        }
    }

};


#endif // !MODEL_H