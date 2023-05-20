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
    vector<GLfloat> vertices;           // ��������
    vector<GLuint> indices;             // ��������

    GLuint VAO;                         // ģ�͵Ļ�������

public:
    //��ͨ���췽��
    Model(const string& path) {
        LoadModel(path);
        SetVAO();
    }
    //�������Ź��ܵ�ģ�͹��췽��
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
    // ���ļ�������ģ�ͣ�ʹ��assimp�����
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
    // �����нڵ���в���
    void ProcessNode(aiNode* node, const aiScene* scene) {

        // �Ե�ǰ�ڵ������������в���
        for (GLuint i = 0; i < node->mNumMeshes; i++) {
            // node��ֻ������ָ��scene�����ݵ�����
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(mesh, scene);
        }
        // �����ӽڵ�
        for (GLuint i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene);
        }
    }


    // �Խڵ��е�����������в���
    void ProcessMesh(aiMesh* mesh, const aiScene* scene) {
        // ������������Ķ���
        for (GLuint i = 0; i < mesh->mNumVertices; i++) {
            // λ������
            vertices.push_back(mesh->mVertices[i].x);
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);

            // ��������
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);

            // ��������
            if (mesh->mTextureCoords[0]) {
                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            }
            else {
                vertices.push_back(0);
                vertices.push_back(0);
            }
        }

        // ����������棬��ȡ����
        for (GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);

        }

    }
    // ����ȡ��ģ���������뻺���������ں���ʹ��
    void SetVAO() {
        // ���ɶ���������󡢶��㻺�����������������
        GLuint VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        // �󶨶����������
        glBindVertexArray(VAO);

        // �󶨶��㻺����󣬲����������ݸ��Ƶ���������
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        // ������������󣬲����������ݸ��Ƶ���������
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
            // ���ö������ԣ���ָ����������ָ��
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(3 * sizeof(GLfloat)));

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(6 * sizeof(GLfloat)));

            // ��󶥵��������
            glBindVertexArray(0);

    }



    // ����ģ�ͺ���
    void Scale(float scale) {
        for (unsigned int i = 0; i < vertices.size(); i += 8) {
            vertices[i] *= scale;        // ���� x ����
            vertices[i + 1] *= scale;    // ���� y ����
            vertices[i + 2] *= scale;    // ���� z ����
        }
    }

};


#endif // !MODEL_H