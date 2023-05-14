#ifndef BOSS_H
#define BOSS_H

#include <glad/glad.h>
#include "texture.h"
#include "shader.h"
#include "model.h"
#include "camera.h"
#include "player.h"  // 假设已经定义了 Player 类
#include "texture.h"
class Boss {

private:
    Model* bossModel;       // 怪物模型
    Shader* bossShader;     // 怪物着色器
    Texture* bossTexture;   // 怪物纹理
    Camera* camera;         // 摄像头
    Player* player;         // 玩家对象
    mat4 model;             // 怪物模型变换矩阵
    mat4 view;              //观察矩阵
    vec3 bossPos;           //怪物位置
    float rotationAngle;    //旋风斩旋转角度
    GLint HP;               //boss生命值
public:
   
    Boss(Camera* camera, Player* player) {
        this->camera = camera;
        this->player = player;
        LoadModel();
        LoadShader();
        LoadTexture();
        init();//怪物(数组)初始化...
    }
    void Update(vec3 pos, vec3 dir, bool isShoot) {
        if (isShoot) {
            vec3 des = (pos.z - bossPos.z) / (-dir.z) * dir + pos;
            bossPos.y = 10;//位置矫正，修正被射击位置
            float distance = pow(bossPos.x - des.x, 2) + pow(bossPos.y - des.y, 2);
            if (distance < 20)
                deductHp();//子弹射中boss，扣除生命值

            std::cout << "离集击中距离：" << distance << endl;
        }

        // 获取怪物和玩家的位置
        vec3 playerPos = player->GetPosition();  // 玩家位置

        // 计算怪物到玩家的方向向量
        vec3 direction = normalize(playerPos - bossPos);

        // 设置怪物的移动速度
        float movespeed = 0.1f;
        // 计算怪物和摄像头之间的距离
        float distance = length(playerPos - bossPos);
        model = mat4(1.0);  // 初始化模型矩阵为单位矩阵

        // 根据方向向量和速度更新怪物位置
        if (distance > 25 ) {
            //更新Boss位置
            bossPos += direction * movespeed;
            bossPos.y = 0.0f; // 控制boss在地面
            playerPos.y = 0.0f;
            //停止旋转（旋风斩）
            rotationAngle = glm::radians(0.0f);
            // 计算旋转矩阵（怪物朝向摄像机,初始朝向X轴正向，具体看模型设计）
            float angle = glm::acos(direction.x);
            if (direction.z > 0) 
                angle = -angle; // 顺时针旋转
            //创建旋转矩阵
            mat4 rotationMatrix = glm::rotate(mat4(1.0f), angle, vec3(0.0f, 1.0f, 0.0f));

            // 创建平移矩阵并将其应用到模型矩阵上
            mat4 translationMatrix = glm::translate(mat4(1.0f), bossPos);
            model = translationMatrix * rotationMatrix;
            //model = translationMatrix;

        }
        else {
            // 怪物旋转
           
            std::cout << "受到旋风斩，生命扣除1点" << std::endl;
            player->deductHp();//扣除人物生命值
            rotationAngle += glm::radians(20.0f); // 转换为弧度
            mat4 translationMatrix = glm::translate(mat4(1.0f), bossPos);//平移矩阵
            mat4 rotationMatrix = glm::rotate(mat4(1.0f), rotationAngle, vec3(0.0f, 1.0f, 0.0f));//旋转矩阵
            // 将怪物的旋转矩阵和平移矩阵应用到模型矩阵上
            model = translationMatrix * rotationMatrix;
 
        }
        view = camera->GetViewMatrix();//视图矩阵
    }

    void Render() {
        Shader* shader = bossShader;
        shader->Bind();
        // 更新投影矩阵为相机的透视投影矩阵
        shader->SetMat4("projection", camera->GetProjectionMatrix());
        shader->SetMat4("view", view);
        shader->SetMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,bossTexture->GetId());

        glBindVertexArray(bossModel->GetVAO());
        glDrawElements(GL_TRIANGLES, static_cast<GLuint>(bossModel->GetIndices().size()), GL_UNSIGNED_INT, 0);
        bossShader->Unbind();
        glBindVertexArray(0);

    }
    GLint GetHp() {
        return HP;
    }
    void deductHp() {
        HP--;
    }

private:
    void LoadModel() {
        // 加载怪物模型
        bossModel = new Model("res/model/boss.obj",0.1f);
        //bossModel = new Model("res/model/PAN.FBX");

    }

    void LoadShader() {
        // 加载怪物着色器
        bossShader = new Shader("res/shader/boss.vert", "res/shader/boss.frag");

    }
    void init(){
        //怪物（数组）初始化...
        this->bossPos = vec3(-10.0f, 0.0f, 0.0f); // 设置怪物的初始位置为原点
        this->rotationAngle = glm::radians(0.0f);
        this->model = mat4(1.0f);
        this->HP = 10;
    }
    void LoadTexture() {
        //加载怪物纹理
        bossTexture = new Texture("res/texture/boss.jpg");
    }

};

#endif // BOSS_H
