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

    vector<aiMatrix4x4> boneTransforms;   // �����任����
    map<string, GLuint> boneMapping;      // �������ƺ�������ӳ���ϵ
    GLuint numBones;                      // ��������
    vector<GLfloat> weights;              // ����Ȩ��

    GLuint VAO;                         // ģ�͵Ļ�������
    
public:
    //��ͨ���췽��
    Model(const string& path) {
        std::string extension = GetFileExtension(path);
        LoadModel(path);

        if (extension == "obj") {
           SetOBJVAO();
        }
        else if (extension == "FBX") {
           SetFBXVAO();
        }
        else {
            std::cout << "Unsupported file type: " << path << std::endl;
            return;
        }
    }
    //�������Ź��ܵ�ģ�͹��췽��
    Model(const string& path, float scale ) {
        std::string extension = GetFileExtension(path);
        LoadModel(path);
        Scale(scale);

        if (extension == "obj") {
            SetOBJVAO();
        }
        else if (extension == "fbx") {
            SetFBXVAO();
        }
        else {
            std::cout << "Unsupported file type: " << path << std::endl;
            return;
        }
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

        // ���ع�����Ϣ
        /*if (scene->HasAnimations()) {
            aiAnimation* animation = scene->mAnimations[0];
            numBones = animation->mNumChannels;

            for (GLuint i = 0; i < numBones; i++) {
                aiNodeAnim* boneNodeAnim = animation->mChannels[i];
                string boneName = boneNodeAnim->mNodeName.data;

                if (boneMapping.find(boneName) == boneMapping.end()) {
                    GLuint boneIndex = numBones;
                    boneMapping[boneName] = boneIndex;
                    numBones++;
                }
            }
        }*/

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

            // ����Ȩ��
            for (GLuint j = 0; j < mesh->mNumBones; j++) {
                aiBone* bone = mesh->mBones[j];
                for (GLuint k = 0; k < bone->mNumWeights; k++) {
                    aiVertexWeight vertexWeight = bone->mWeights[k];
                    if (vertexWeight.mVertexId == i) {
                        weights.push_back(vertexWeight.mWeight);
                    }
                }
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
    void SetOBJVAO() {
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
    void SetFBXVAO() {
        // ���ɶ���������󡢶��㻺�����������������
        GLuint VBO, EBO, bonesVBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &bonesVBO);
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
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 11, (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 11, (void*)(3 * sizeof(GLfloat)));

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 11, (void*)(6 * sizeof(GLfloat)));

            // ���ù����������ԣ���ָ��������������ָ��
            glEnableVertexAttribArray(3);
            glVertexAttribIPointer(3, 4, GL_INT, sizeof(GLfloat) * 11, (void*)(8 * sizeof(GLfloat)));

            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 11, (void*)(8 * sizeof(GLfloat) + sizeof(int) * 4));

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

    std::string GetFileExtension(const std::string& filePath) {
        size_t dotPos = filePath.find_last_of('.');
        if (dotPos != std::string::npos) {
            return filePath.substr(dotPos + 1);
        }
        return "";
    }

};


#endif // !MODEL_H