#ifndef PLAYER_H
#define PLAYER_H

#include <glad/glad.h>
#include "texture.h"
#include "shader.h"
#include "model.h"
#include "camera.h"

class Player {
private:
	vec2 windowSize;					// ���ڳߴ�
	// ǹ���������
	Model* gun;
	vec3 gunPos;						// ǹ��λ������
	Shader* gunShader;
	mat4 gunModel;						// ǹģ��λ�ñ任����
	Texture* diffuseMap;				// ��������ͼ
	Texture* specularMap;				// ���淴����ͼ
	float gunRecoil;					// ������
	// ׼��
	Model* dot;							
	Shader* dotShader;
	mat4 dotModel;						// ׼��ģ��λ�ñ任����
	// ����ͷ
	Camera* camera;
	// �任����
	mat4 projection;
	mat4 view;
	GLint HP;               //��������ֵ
public:
	Player(vec2 windowSize, Camera* camera) {
		this->windowSize = windowSize;
		this->camera = camera;
		this->gunRecoil = 10.0f;
		this->dotModel = mat4(1.0);
		this->gunModel = mat4(1.0);
		this->gunPos = vec3(0.f);
		this->HP = 100;
		LoadGun();
		LoadTexture();
		LoadShader();
	}
	// ���±任��������ͷλ�õ�����
	void Update(float deltaTime, bool isShoot) {
		if (isShoot) 
			gunRecoil = 10.0f;  // ������������������ǹ�ĺ�����Ϊ 10.0f
		else 
			gunRecoil = 0.0f;   // ���û�������������ǹ�ĺ�����Ϊ 0.0f

		view = camera->GetViewMatrix();  // ������ͼ����Ϊ�������ͼ����
		// ����ͶӰ����Ϊ�����͸��ͶӰ����
		projection = perspective(radians(camera->GetZoom()), windowSize.x / windowSize.y, 0.1f, 500.0f);

		dotModel = mat4(1.0);  // ��ʼ����ģ�͵ı任����Ϊ��λ����
		dotModel[3] = vec4(camera->GetPosition(), 1.0);  // ����ģ�͵�λ������Ϊ�����λ��
		dotModel = translate(dotModel, camera->GetFront());  // ���������ǰ�������ƽ�Ʊ任
		if (isShoot) {
			// ����׼��ģ�͵�����С
			if (dotModel[0][0] >= 2.0f && dotModel[1][1] >= 2.0f && dotModel[2][2] >= 2.0f) {
				dotModel = scale(dotModel, vec3(1.0f / 2.0f));  // ��׼��ģ������Ϊԭ����һ���С
			}
			else {
				dotModel = scale(dotModel, vec3(2.0f));  // ��׼��ģ������Ϊԭ����������С

			}
		}


		//dotModel = scale(dotModel, vec3(0.005));  // �������ű任������ģ����СΪԭ���� 0.005 ��

		this->gunPos = (camera->GetFront() * 0.25f) + (camera->GetRight() * 0.2f) + (camera->GetUp() * -0.125f) + camera->GetPosition();  // ����ǹ��λ��
		gunModel = mat4(1.0);  // ��ʼ��ǹģ�͵ı任����Ϊ��λ����
		gunModel[0] = vec4(camera->GetRight(), 0.0);  // ��ǹģ�͵� x ������Ϊ������ҷ���
		gunModel[1] = vec4(camera->GetUp(), 0.0);  // ��ǹģ�͵� y ������Ϊ������Ϸ���
		gunModel[2] = vec4(-camera->GetFront(), 0.0);  // ��ǹģ�͵� z ������Ϊ����ķ�����
		gunModel[3] = vec4(this->gunPos, 1.0);  // ��ǹģ�͵�λ������Ϊ����õ���ǹ��λ��
		gunModel = rotate(gunModel, radians(gunRecoil), vec3(1.0, 0.0, 0.0));  // ���ݺ����������� x ����ת
		gunModel = scale(gunModel, vec3(0.225));  // �������ű任����ǹģ����СΪԭ���� 0.225 ��
		gunModel = translate(gunModel, vec3(-0.225, 0.0, -0.225));  // ����ƽ�Ʊ任
		gunModel = rotate(gunModel, radians(-170.0f), vec3(0.0, 1.0, 0.0));  //
	}

	// ��Ⱦ����
	void Render() {
		dotShader->Bind();  // ʹ��׼����ɫ��
		dotShader->SetMat4("projection", projection);  // ����׼����ɫ���е�ͶӰ����
		dotShader->SetMat4("view", view);  // ����׼����ɫ���е���ͼ����
		dotShader->SetMat4("model", dotModel);  // ����׼����ɫ���е�ģ�;���

		glBindVertexArray(dot->GetVAO());  // ��׼�ǵĶ����������
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(dot->GetIndices().size()), GL_UNSIGNED_INT, 0);  // ����׼�ǵ���������Ƭ


		gunShader->Bind();  // ʹ��ǹ����ɫ��
		gunShader->SetMat4("projection", projection);  // ����ǹ��ɫ���е�ͶӰ����
		gunShader->SetMat4("view", view);  // ����ǹ��ɫ���е���ͼ����
		gunShader->SetMat4("model", gunModel);  // ����ǹ��ɫ���е�ģ�;���

		glActiveTexture(GL_TEXTURE0);  // ��������Ԫ0
		glBindTexture(GL_TEXTURE_2D, diffuseMap->GetId());  // ����������ͼ������Ԫ0
		glActiveTexture(GL_TEXTURE1);  // ��������Ԫ1
		glBindTexture(GL_TEXTURE_2D, specularMap->GetId());  // �󶨾��淴����ͼ������Ԫ1

		glBindVertexArray(gun->GetVAO());  // ��ǹ�Ķ����������
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(gun->GetIndices().size()), GL_UNSIGNED_INT, 0);  // ����ǹ����������Ƭ

		glBindVertexArray(0);  // ��󶥵��������
		gunShader->Unbind();  // ���ǹ����ɫ��

	}
	vec3 GetPosition() {
		return camera->GetPosition();
	}
	GLint GetHp() {
		return HP;
	}
	void deductHp() {
		HP--;
	}

private:
	// ����ǹģ��
	void LoadGun() {
		gun = new Model("res/model/gun.obj");
		dot = new Model("res/model/dot.obj",0.01);
	}
	// ��������
	void LoadTexture() {
		diffuseMap = new Texture("res/texture/gun-diffuse-map.jpg");
		specularMap = new Texture("res/texture/gun-specular-map.jpg");
	}
	// ������ɫ��
	void LoadShader() {
		gunShader = new Shader("res/shader/gun.vert", "res/shader/gun.frag");
		gunShader->Bind();
		gunShader->SetInt("material.diffuse", 0);
		gunShader->SetInt("material.specular", 1);
		gunShader->SetFloat("material.shininess", 64.0);
		gunShader->SetVec3("light.position", vec3(0.0, 400.0, -400.0));
		gunShader->SetVec3("light.ambient", vec3(0.2));
		gunShader->SetVec3("light.diffuse", vec3(0.65));
		gunShader->SetVec3("light.specular", vec3(1.0));
		gunShader->SetVec3("viewPos", camera->GetPosition());
		gunShader->Unbind();

		dotShader = new Shader("res/shader/ball.vert", "res/shader/ball.frag");
		dotShader->Bind();
		dotShader->SetVec3("color", vec3(1.0, 0.0, 0.0));
		dotShader->Unbind();
	}
};

#endif // !PLAYER_H
