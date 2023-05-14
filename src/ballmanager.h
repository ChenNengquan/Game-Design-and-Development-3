#ifndef BALLMANAGER_H
#define BALLMANAGER_H

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
using namespace glm;
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;
#include "model.h"
#include "shader.h"
#include "camera.h"

class BallManager {
private:
	vec2 windowSize;

	Model* ball;
	Shader* ballShader;
	GLuint number;						// 当前小球数目
	GLuint maxNumber;					// 小球最大数目
	vec3 basicPos;						// 小球基础坐标
	vector<vec3> position;				// 场上存在的小球坐标
	float moveSpeed;					// 小球移动速度
	GLuint score;						// 得分
	GLuint gameModel;					// 游戏模式
	vec3 lightPos;						// 光源位置
	mat4 lightSpaceMatrix;				// 将顶点世界坐标转换为以光源为中心的坐标

	Camera* camera;
	// 模型变换矩阵
	mat4 model;
	mat4 projection;
	mat4 view;
public:
	BallManager(vec2 windowSize, Camera* camera) {
		this->windowSize = windowSize;  // 设置窗口大小
		this->camera = camera;  // 设置相机对象

		basicPos = vec3(0.0, 5.0, -30.0);  // 设置小球的基础位置
		number = 0;  // 当前小球数目
		maxNumber = 3;  // 小球的最大数目
		moveSpeed = 0.1f;  // 小球的移动速度
		score = 0;  // 得分
		this->lightPos = vec3(0.0, 400.0, 150.0);  // 设置光源的位置
		mat4 lightProjection = ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 500.0f);  // 创建光源的正交投影矩阵
		mat4 lightView = lookAt(lightPos, vec3(0.0f), vec3(0.0, 1.0, 0.0));  // 创建从光源位置观察的观察矩阵
		this->lightSpaceMatrix = lightProjection * lightView;  // 将投影矩阵和观察矩阵相乘，得到光空间矩阵

		AddBall();  // 添加小球
		LoadModel();  // 加载模型和着色器
	}

	// 设置游戏模式
	void SetGameModel(GLuint num) {
		gameModel = num;
	}
	// 更新变换矩阵，判断射击是否击中小球
	void Update(vec3 pos, vec3 dir, bool isShoot) {
		this->view = camera->GetViewMatrix();// 获取相机的视图矩阵，用于设置渲染的视角。
		//根据相机的缩放参数、窗口大小等信息生成透视投影矩阵，用于设置渲染的投影效果。
		this->projection = perspective(radians(camera->GetZoom()), windowSize.x / windowSize.y, 0.1f, 500.0f);
		// 判断是否发生射击动作。如果是，则进行击中判定。
		if (isShoot) {
			vector<vec3> temp;
			for (GLuint i = 0; i < position.size(); i++) {
				/*
				* (pos.z - position[i].z)：首先计算pos.z减去position[i].z的差值，得到一个表示Z轴方向距离的值。
				*(-dir.z) * dir：接下来计算-dir.z乘以dir，得到一个与射击方向相反的单位向量。
				*(pos.z - position[i].z) / (-dir.z) * dir：将上述两个结果相除，并乘以射击方向的单位向量。这个计算的结果是一个从射击位置出发，沿着射击方向的单位向量。
				*+ pos：最后，将上述计算得到的单位向量与射击位置pos相加，得到射击击中点的位置，赋值给vec3 des。
				*简而言之，这行代码的目的是计算出射击击中点的位置，并将其赋值给vec3 des变量。它利用了射击位置、射击方向和小球位置之间的关系来计算击中点的位置。
				*/
				//计算击中点的位置，通过射击方向、射击位置和小球位置进行计算。
				vec3 des = (pos.z - position[i].z) / (-dir.z) * dir + pos;

				if (pow(position[i].x - des.x, 2) + pow(position[i].y - des.y, 2) > 5) {
					temp.push_back(position[i]);
				}
				else {
					number--;
					score++;
				}
			}
			position = temp;
		}
		if (gameModel == 1) {//如果游戏模式为1，则添加新的小球并返回。
			AddBall();
			return;
		}
		for (GLuint i = 0; i < position.size(); i++)
			position[i].z += moveSpeed;//更新小球的位置，使其沿着Z轴正向移动。

		if (number == 0) {
			//如果小球数量为0，则增加最大小球数量，并根据条件增加移动速度。然后添加新的小球。
			maxNumber++;
			if (maxNumber == 10) {
				moveSpeed += 0.1f;
				maxNumber = 3;
			}
			AddBall();
		}
	}
	// 判断游戏是否结束
	bool IsOver() {
		if (position.size() > 0)
			if (position[0].z >= 70)
				return true;
		return false;
	}

	GLuint GetScore() {
		return score;
	}
	// 渲染小球
	void Render(Shader* shader, GLuint depthMap = -1) {
		for (GLuint i = 0; i < position.size(); i++) {
			model = mat4(1.0);//创建一个单位矩阵model，用于存储小球的模型变换矩阵
			model[3] = vec4(position[i], 1.0);//将小球的位置作为平移变换的一部分，将其赋值给model矩阵的第四行。
			model = scale(model, vec3(5));//将model矩阵进行缩放变换，使小球变为合适的大小。
			/*
			* 判断传入的着色器shader是否为空。如果为空，
			*表示未指定特定的着色器，将使用默认的ballShader。
			*在这种情况下，将绑定ballShader，并将投影矩阵和视图矩阵传递给着色器。
			*/
			if (shader == NULL) {
				shader = ballShader;
				shader->Bind();
				shader->SetMat4("projection", projection);
				shader->SetMat4("view", view);
			}
			else {
				shader->Bind();
			}
			// 设置着色器的模型矩阵
			shader->SetMat4("model", model);

			// 激活纹理单元0，并绑定深度贴图
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthMap);

			// 绑定小球模型的顶点数组对象
			glBindVertexArray(ball->GetVAO());

			// 绘制小球模型
			glDrawElements(GL_TRIANGLES, static_cast<GLuint>(ball->GetIndices().size()), GL_UNSIGNED_INT, 0);

			// 解绑着色器和顶点数组对象
			shader->Unbind();
			glBindVertexArray(0);

			// 重置模型矩阵
			model = mat4(1.0);
		}
	}
private:
	void LoadModel() {
		// 加载小球模型
		ball = new Model("res/model/dot.obj");

		// 加载小球的着色器
		ballShader = new Shader("res/shader/ball.vert", "res/shader/ball.frag");

		// 绑定小球的着色器
		ballShader->Bind();

		// 设置小球着色器的uniform变量

		// 设置小球的颜色
		ballShader->SetVec3("color", vec3(0.2, 0.5, 0.5f));

		// 设置阴影贴图的纹理单元
		ballShader->SetInt("shadowMap", 0);

		// 设置光源位置
		ballShader->SetVec3("lightPos", lightPos);

		// 设置相机位置
		ballShader->SetVec3("viewPos", camera->GetPosition());

		// 设置光源的投影视图矩阵
		ballShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

		// 解绑小球的着色器
		ballShader->Unbind();
	}

	// 添加小球
	/*	judgeX：随机生成的0或1，用于判断小球的水平方向。
		x：根据judgeX的值随机生成小球的水平位置，范围为[-30, 30]。
		y：随机生成小球的垂直位置，范围为[0, 30]。
		pos：根据生成的水平和垂直位置计算得到的小球位置。
		如果位置合法（即与已存在的小球位置不重叠），则将该位置添加到position向量中，并增加小球数目number。
		如果位置不合法，则重新生成位置，继续尝试添加小球。
	*/
	// 添加小球
	void AddBall() {
		for (GLuint i = number; i < maxNumber; i++) {
			// 随机生成0或1，用于确定小球的水平方向
			float judgeX = rand() % 2;
			// 根据judgeX的值随机生成小球的水平位置，范围为[-30, 30]
			float x = (judgeX >= 0.5) ? rand() % 30 : -(rand() % 30);
			// 随机生成小球的垂直位置，范围为[0, 30]
			float y = rand() % 30;
			// 计算小球的最终位置
			vec3 pos = vec3(basicPos.x + x, basicPos.y + y, basicPos.z);

			// 检查位置是否与已存在的小球位置重叠
			if (CheckPosition(pos)) {
				// 如果位置合法，则将该位置添加到position向量中
				position.push_back(pos);
				// 增加小球数目
				number++;
			}
			else {
				// 如果位置不合法，则重新生成位置，继续尝试添加小球
				i--;
			}
		}
	}

	// 检查已存在小球的位置，避免添加的小球出现重叠
	/*
	*	pos：待检查的小球位置。
		away：计算已存在小球位置与待检查位置之间的距离。
		如果距离小于100，则表示位置重叠，函数返回false。
		如果距离大于等于100，表示位置合法，函数返回true。
	*/
	bool CheckPosition(vec3 pos) {
		for (GLuint i = 0; i < position.size(); i++) {
			float away = pow(position[i].x - pos.x, 2) + pow(position[i].y - pos.y, 2);
			if (away < 100)
				return false;
		}
		return true;
	}
};

#endif // !BALLMANAGER_H
