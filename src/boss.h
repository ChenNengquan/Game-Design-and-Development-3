#ifndef BOSS_H
#define BOSS_H

#include <glad/glad.h>
#include "texture.h"
#include "shader.h"
#include "model.h"
#include "camera.h"
#include "player.h"  // �����Ѿ������� Player ��
#include "texture.h"
class Boss {

private:
    Model* bossModel;       // ����ģ��
    Shader* bossShader;     // ������ɫ��
    Texture* bossTexture;   // ��������
    Camera* camera;         // ����ͷ
    Player* player;         // ��Ҷ���
    mat4 model;             // ����ģ�ͱ任����
    mat4 view;              //�۲����
    vec3 bossPos;           //����λ��
    float rotationAngle;    //����ն��ת�Ƕ�
    GLint HP;               //boss����ֵ
public:
   
    Boss(Camera* camera, Player* player) {
        this->camera = camera;
        this->player = player;
        LoadModel();
        LoadShader();
        LoadTexture();
        init();//����(����)��ʼ��...
    }
    void Update(vec3 pos, vec3 dir, bool isShoot) {
        if (isShoot) {
            vec3 des = (pos.z - bossPos.z) / (-dir.z) * dir + pos;
            bossPos.y = 10;//λ�ý��������������λ��
            float distance = pow(bossPos.x - des.x, 2) + pow(bossPos.y - des.y, 2);
            if (distance < 20)
                deductHp();//�ӵ�����boss���۳�����ֵ

            std::cout << "�뼯���о��룺" << distance << endl;
        }

        // ��ȡ�������ҵ�λ��
        vec3 playerPos = player->GetPosition();  // ���λ��

        // ������ﵽ��ҵķ�������
        vec3 direction = normalize(playerPos - bossPos);

        // ���ù�����ƶ��ٶ�
        float movespeed = 0.1f;
        // ������������ͷ֮��ľ���
        float distance = length(playerPos - bossPos);
        model = mat4(1.0);  // ��ʼ��ģ�;���Ϊ��λ����

        // ���ݷ����������ٶȸ��¹���λ��
        if (distance > 25 ) {
            //����Bossλ��
            bossPos += direction * movespeed;
            bossPos.y = 0.0f; // ����boss�ڵ���
            playerPos.y = 0.0f;
            //ֹͣ��ת������ն��
            rotationAngle = glm::radians(0.0f);
            // ������ת���󣨹��ﳯ�������,��ʼ����X�����򣬾��忴ģ����ƣ�
            float angle = glm::acos(direction.x);
            if (direction.z > 0) 
                angle = -angle; // ˳ʱ����ת
            //������ת����
            mat4 rotationMatrix = glm::rotate(mat4(1.0f), angle, vec3(0.0f, 1.0f, 0.0f));

            // ����ƽ�ƾ��󲢽���Ӧ�õ�ģ�;�����
            mat4 translationMatrix = glm::translate(mat4(1.0f), bossPos);
            model = translationMatrix * rotationMatrix;
            //model = translationMatrix;

        }
        else {
            // ������ת
           
            std::cout << "�ܵ�����ն�������۳�1��" << std::endl;
            player->deductHp();//�۳���������ֵ
            rotationAngle += glm::radians(20.0f); // ת��Ϊ����
            mat4 translationMatrix = glm::translate(mat4(1.0f), bossPos);//ƽ�ƾ���
            mat4 rotationMatrix = glm::rotate(mat4(1.0f), rotationAngle, vec3(0.0f, 1.0f, 0.0f));//��ת����
            // ���������ת�����ƽ�ƾ���Ӧ�õ�ģ�;�����
            model = translationMatrix * rotationMatrix;
 
        }
        view = camera->GetViewMatrix();//��ͼ����
    }

    void Render() {
        Shader* shader = bossShader;
        shader->Bind();
        // ����ͶӰ����Ϊ�����͸��ͶӰ����
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
        // ���ع���ģ��
        bossModel = new Model("res/model/boss.obj",0.1f);
        //bossModel = new Model("res/model/PAN.FBX");

    }

    void LoadShader() {
        // ���ع�����ɫ��
        bossShader = new Shader("res/shader/boss.vert", "res/shader/boss.frag");

    }
    void init(){
        //������飩��ʼ��...
        this->bossPos = vec3(-10.0f, 0.0f, 0.0f); // ���ù���ĳ�ʼλ��Ϊԭ��
        this->rotationAngle = glm::radians(0.0f);
        this->model = mat4(1.0f);
        this->HP = 10;
    }
    void LoadTexture() {
        //���ع�������
        bossTexture = new Texture("res/texture/boss.jpg");
    }

};

#endif // BOSS_H
