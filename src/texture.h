#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <iostream>
using namespace std;

class Texture {
private:
    GLuint id;

public:
    Texture(const char* path) {
        glGenTextures(1, &id); // ������������ID

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0); // ʹ��stb_image�����ͼ������
        if (data) {
            int format;
            if (nrComponents == 1 || nrComponents == 2)
                format = GL_RED; // ��ͨ����˫ͨ��ͼ��ʹ��GL_RED��ʽ
            else if (nrComponents == 3)
                format = GL_RGB; // ��ͨ��ͼ��ʹ��GL_RGB��ʽ
            else if (nrComponents == 4)
                format = GL_RGBA; // ��ͨ��ͼ��ʹ��GL_RGBA��ʽ

            glBindTexture(GL_TEXTURE_2D, id); // ���������
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // ��ͼ�������ϴ����������
            glGenerateMipmap(GL_TEXTURE_2D); // ���ɶ༶��Զ����
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // �������ش洢�������

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // ���������Ʒ�ʽ
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // ��������Ŵ���˷�ʽ
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // ����������С���˷�ʽ

            stbi_image_free(data); // �ͷ�ͼ�������ڴ�
        }
        else {
            cout << "Texture failed to load at path: " << path << endl;
            stbi_image_free(data); // �ͷ�ͼ�������ڴ�
        }
    }

    GLuint GetId() {
        return id; // ������������ID
    }
};

#endif
