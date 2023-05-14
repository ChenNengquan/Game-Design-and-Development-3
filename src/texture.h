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
        glGenTextures(1, &id); // 生成纹理对象的ID

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0); // 使用stb_image库加载图像数据
        if (data) {
            int format;
            if (nrComponents == 1 || nrComponents == 2)
                format = GL_RED; // 单通道或双通道图像使用GL_RED格式
            else if (nrComponents == 3)
                format = GL_RGB; // 三通道图像使用GL_RGB格式
            else if (nrComponents == 4)
                format = GL_RGBA; // 四通道图像使用GL_RGBA格式

            glBindTexture(GL_TEXTURE_2D, id); // 绑定纹理对象
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // 将图像数据上传到纹理对象
            glGenerateMipmap(GL_TEXTURE_2D); // 生成多级渐远纹理
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 设置像素存储对齐参数

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // 设置纹理环绕方式
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 设置纹理放大过滤方式
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // 设置纹理缩小过滤方式

            stbi_image_free(data); // 释放图像数据内存
        }
        else {
            cout << "Texture failed to load at path: " << path << endl;
            stbi_image_free(data); // 释放图像数据内存
        }
    }

    GLuint GetId() {
        return id; // 返回纹理对象的ID
    }
};

#endif
