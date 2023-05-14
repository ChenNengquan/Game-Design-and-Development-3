#ifndef PLAYER_H
#define PLAYER_H

#include <glad/glad.h>
#include "texture.h"
#include "shader.h"
#include "model.h"
#include "camera.h"

class Player {
private:
	vec2 windowSize;					// 窗口尺寸
	// 枪的相关数据
	Model* gun;
	vec3 gunPos;						// 枪的位置坐标
	Shader* gunShader;
	mat4 gunModel;						// 枪模型位置变换矩阵
	Texture* diffuseMap;				// 漫反射贴图
	Texture* specularMap;				// 镜面反射贴图
	float gunRecoil;					// 后座力
	// 准星
	Model* dot;							
	Shader* dotShader;
	mat4 dotModel;						// 准星模型位置变换矩阵
	// 摄像头
	Camera* camera;
	// 变换矩阵
	mat4 projection;
	mat4 view;
	GLint HP;               //人物生命值
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
	// 更新变换矩阵、摄像头位置等数据
	void Update(float deltaTime, bool isShoot) {
		if (isShoot) 
			gunRecoil = 10.0f;  // 如果正在射击，则设置枪的后坐力为 10.0f
		else 
			gunRecoil = 0.0f;   // 如果没有射击，则设置枪的后坐力为 0.0f

		view = camera->GetViewMatrix();  // 更新视图矩阵为相机的视图矩阵
		// 更新投影矩阵为相机的透视投影矩阵
		projection = perspective(radians(camera->GetZoom()), windowSize.x / windowSize.y, 0.1f, 500.0f);

		dotModel = mat4(1.0);  // 初始化点模型的变换矩阵为单位矩阵
		dotModel[3] = vec4(camera->GetPosition(), 1.0);  // 将点模型的位置设置为相机的位置
		dotModel = translate(dotModel, camera->GetFront());  // 根据相机的前方向进行平移变换
		if (isShoot) {
			// 限制准星模型的最大大小
			if (dotModel[0][0] >= 2.0f && dotModel[1][1] >= 2.0f && dotModel[2][2] >= 2.0f) {
				dotModel = scale(dotModel, vec3(1.0f / 2.0f));  // 将准星模型缩放为原来的一半大小
			}
			else {
				dotModel = scale(dotModel, vec3(2.0f));  // 将准星模型缩放为原来的两倍大小

			}
		}


		//dotModel = scale(dotModel, vec3(0.005));  // 进行缩放变换，将点模型缩小为原来的 0.005 倍

		this->gunPos = (camera->GetFront() * 0.25f) + (camera->GetRight() * 0.2f) + (camera->GetUp() * -0.125f) + camera->GetPosition();  // 计算枪的位置
		gunModel = mat4(1.0);  // 初始化枪模型的变换矩阵为单位矩阵
		gunModel[0] = vec4(camera->GetRight(), 0.0);  // 将枪模型的 x 轴设置为相机的右方向
		gunModel[1] = vec4(camera->GetUp(), 0.0);  // 将枪模型的 y 轴设置为相机的上方向
		gunModel[2] = vec4(-camera->GetFront(), 0.0);  // 将枪模型的 z 轴设置为相机的反方向
		gunModel[3] = vec4(this->gunPos, 1.0);  // 将枪模型的位置设置为计算得到的枪的位置
		gunModel = rotate(gunModel, radians(gunRecoil), vec3(1.0, 0.0, 0.0));  // 根据后坐力进行绕 x 轴旋转
		gunModel = scale(gunModel, vec3(0.225));  // 进行缩放变换，将枪模型缩小为原来的 0.225 倍
		gunModel = translate(gunModel, vec3(-0.225, 0.0, -0.225));  // 进行平移变换
		gunModel = rotate(gunModel, radians(-170.0f), vec3(0.0, 1.0, 0.0));  //
	}

	// 渲染场景
	void Render() {
		dotShader->Bind();  // 使用准星着色器
		dotShader->SetMat4("projection", projection);  // 设置准星着色器中的投影矩阵
		dotShader->SetMat4("view", view);  // 设置准星着色器中的视图矩阵
		dotShader->SetMat4("model", dotModel);  // 设置准星着色器中的模型矩阵

		glBindVertexArray(dot->GetVAO());  // 绑定准星的顶点数组对象
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(dot->GetIndices().size()), GL_UNSIGNED_INT, 0);  // 绘制准星的三角形面片


		gunShader->Bind();  // 使用枪的着色器
		gunShader->SetMat4("projection", projection);  // 设置枪着色器中的投影矩阵
		gunShader->SetMat4("view", view);  // 设置枪着色器中的视图矩阵
		gunShader->SetMat4("model", gunModel);  // 设置枪着色器中的模型矩阵

		glActiveTexture(GL_TEXTURE0);  // 激活纹理单元0
		glBindTexture(GL_TEXTURE_2D, diffuseMap->GetId());  // 绑定漫反射贴图到纹理单元0
		glActiveTexture(GL_TEXTURE1);  // 激活纹理单元1
		glBindTexture(GL_TEXTURE_2D, specularMap->GetId());  // 绑定镜面反射贴图到纹理单元1

		glBindVertexArray(gun->GetVAO());  // 绑定枪的顶点数组对象
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(gun->GetIndices().size()), GL_UNSIGNED_INT, 0);  // 绘制枪的三角形面片

		glBindVertexArray(0);  // 解绑顶点数组对象
		gunShader->Unbind();  // 解绑枪的着色器

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
	// 加载枪模型
	void LoadGun() {
		gun = new Model("res/model/gun.obj");
		dot = new Model("res/model/dot.obj",0.01);
	}
	// 加载纹理
	void LoadTexture() {
		diffuseMap = new Texture("res/texture/gun-diffuse-map.jpg");
		specularMap = new Texture("res/texture/gun-specular-map.jpg");
	}
	// 加载着色器
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
