#ifndef PLACE_H
#define PLACE_H

#include <glad/glad.h>
#include "model.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"


class Place {
private:
	vec2 windowSize;
	// 房间
	Model* room;                        // 房间模型
	Texture* roomTexture;               // 房间纹理
	Shader* roomShader;                 // 房间着色器

	Model* floor;						//地板
	Texture* floorTexture;              // 地板纹理
	Shader* floorShader;					//地板着色器

	// 太阳
	Model* sun;                         // 太阳模型
	vec3 lightPos;                      // 光源位置
	mat4 lightSpaceMatrix;              // 光空间矩阵，用于将顶点世界坐标转换为以光源为中心的坐标
	Shader* sunShader;                  // 太阳着色器


	Shader* hudShader;					//界面着色器
	Texture* hudTexture;				//界面纹理

	Camera* camera;                     // 摄像机
	
	// 模型变换矩阵
	mat4 model;
	mat4 projection;
	mat4 view;

public:
	//构造函数声明，接受一个名为windowSize的vec2参数和一个名为camera的Camera对象指针参数
	Place(vec2 windowSize, Camera* camera) {
		this->windowSize = windowSize;
		this->camera = camera;
		// 用光源的位置(0.0, 400.0, 150.0)初始化lightPos成员变量。
		this->lightPos = vec3(0.0, 400.0, 150.0);
		//this->lightPos = vec3(-60.0, 400.0, -400.0);
		//创建一个用于光源的正交投影矩阵。该矩阵定义了光源的视觉空间。
		//mat4 lightProjection = ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 500.0f);
		mat4 lightProjection = ortho(-500.0f, 500.0f, -500.0f, 500.0f, 1.0f, 1000.0f);
		// 创建一个光源的视图矩阵。该矩阵将光源定位于场景中，并定义了光源的朝向。
		mat4 lightView = lookAt(lightPos, vec3(0.0f), vec3(0.0, 1.0, 0.0));
		this->lightSpaceMatrix = lightProjection * lightView;
		LoadModel();//加载房间和太阳的模型。
		LoadTexture();//加载房间的纹理
		LoadShader();//加载房间和太阳的着色器
	}
	// 更新变换矩阵
	void Update() {
		//将模型变换矩阵重置为单位矩阵，表示没有应用额外的模型变换。这将保持模型的原始状态。
		this->model = mat4(1.0);
		//获取相机的视图矩阵并将其赋值给类的view成员变量。相机的视图矩阵定义了观察者的位置和朝向，用于将场景从世界坐标系转换到相机坐标系。
		this->view = camera->GetViewMatrix();
		/*根据相机的参数计算透视投影矩阵并将其赋值给类的projection成员变量。
		*透视投影矩阵定义了场景的可视范围和透视效果，
		*用于将相机坐标系中的物体投影到屏幕坐标系。
		*/
		this->projection = perspective(radians(camera->GetZoom()), windowSize.x / windowSize.y, 0.1f, 500.0f);

	}
	// 渲染房间(迷宫)
	void RoomRender(Shader* shader, int depthMap = -1) {
		if (shader == NULL) {
			shader = roomShader;
			shader->Bind();
			shader->SetMat4("view", view);
			shader->SetMat4("projection", projection);
		}
		else {
			shader->Bind();
		}
		shader->SetMat4("model", model);
		glActiveTexture(GL_TEXTURE0);//启用纹理
		glBindTexture(GL_TEXTURE_2D, roomTexture->GetId());
		if (depthMap != -1) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depthMap);
		}
		glBindVertexArray(room->GetVAO());
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(room->GetIndices().size()), GL_UNSIGNED_INT, 0);
		shader->Unbind();
		glBindVertexArray(0);
	}
	// 渲染太阳
	void SunRender() {
		Shader* shader = sunShader;
		shader->Bind();
		shader->SetMat4("projection", projection);
		shader->SetMat4("model", model);
		shader->SetMat4("view", view);
		glBindVertexArray(sun->GetVAO());
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(sun->GetIndices().size()), GL_UNSIGNED_INT, 0);
		shader->Unbind();
		glBindVertexArray(0);
	}
	//渲染地板
	void FloorRender() {
		Shader* shader = floorShader;
		shader->Bind();
		shader->SetMat4("projection", projection);
		shader->SetMat4("model", model);
		shader->SetMat4("view", view);

		//启用纹理.....自行美化..(可自行在LoadTexture()加载其他纹理，然后启用..类似)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture->GetId());

		glBindVertexArray(floor->GetVAO());
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(floor->GetIndices().size()), GL_UNSIGNED_INT, 0);
		shader->Unbind();
		glBindVertexArray(0);
	}

	void DrawHUD(GLint PlyerHP, GLint remainingTime) {
		// 使用HUD着色器程序
		hudShader->Bind();
		// 设置uniform变量
		hudShader->SetFloat("health", PlyerHP);
		hudShader->SetFloat("time", remainingTime);
		// 绑定HUD纹理
		//glBindTexture(GL_TEXTURE_2D, hudTexture);

		// 设置顶点数据
		GLfloat vertices[] = {
			// 顶点坐标    纹理坐标
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 1.0f
		};

		// 创建和绑定顶点缓冲对象（VBO）
		GLuint VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// 设置顶点属性指针
		GLuint positionAttrib = glGetAttribLocation(hudShader->GetProgram(), "position");
		glEnableVertexAttribArray(positionAttrib);
		glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

		GLuint texCoordAttrib = glGetAttribLocation(hudShader->GetProgram(), "texCoord");
		glEnableVertexAttribArray(texCoordAttrib);
		glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

		// 绘制HUD
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		// 解绑和删除顶点缓冲对象（VBO）
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &VBO);

		// 解绑HUD纹理
		glBindTexture(GL_TEXTURE_2D, 0);

		// 禁用着色器
		hudShader->Unbind();
	}
	
	
private:
	// 加载模型
	void LoadModel() {
		//加载迷宫模型
		room = new Model("res/model/maze.obj");
		//加载太阳模型
		sun = new Model("res/model/sun.obj");
		//加载地板
		floor= new Model("res/model/floor.obj");
	}
	// 加载纹理
	void LoadTexture() {
		//加载房间纹理
		roomTexture = new Texture("res/texture/wall.jpg");
		floorTexture = new Texture("res/texture/grass.jpg");
		
	}
	// 加载着色器
	void LoadShader() {
		roomShader = new Shader("res/shader/room.vert", "res/shader/room.frag");
		roomShader->Bind();
		roomShader->SetInt("diffuse", 0);
		roomShader->SetInt("shadowMap", 1);
		roomShader->SetVec3("lightPos", lightPos);
		roomShader->SetVec3("viewPos", camera->GetPosition());
		roomShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		roomShader->Unbind();

		sunShader = new Shader("res/shader/sun.vert", "res/shader/sun.frag");
		sunShader->Bind();
		sunShader->Unbind();

		floorShader = new Shader("res/shader/floor.vert", "res/shader/floor.frag");

		/*hudShader = new Shader("res/shader/HUD.vert", "res/shader/HUD.frag");
		hudShader->Bind();
		hudShader->Unbind();*/

	}

};

#endif // !PLACE_H
