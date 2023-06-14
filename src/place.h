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
	Model* sky;                        // 天空模型
	Texture* skyTexture;               // 天空纹理
	Shader* skyShader;                 // 天空着色器

	Model* initial;							//开始、胜利、失败界面模型
	Texture* initialTexture;               // 初始界面纹理
	Texture* DefeatTexture;					//胜利界面纹理
	Texture* VictoryTexture;					//失败界面纹理
	Shader* initialShader;                 // 界面着色器


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
		mat4 projection0 = perspective(radians(camera->GetZoom()), windowSize.x / windowSize.y, 0.1f, 500.0f);
		shader->Bind();
		shader->SetMat4("projection", projection0);
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

	//渲染天空
	void skyRender() {
		Shader* shader = skyShader;
		shader->Bind();
		// 更新投影矩阵为相机的透视投影矩阵
		shader->SetMat4("projection", camera->GetProjectionMatrix());
		shader->SetMat4("view", view);
		mat4 model = mat4(1.0);
		shader->SetMat4("model", model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, skyTexture->GetId());

		glBindVertexArray(sky->GetVAO());
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(sky->GetIndices().size()), GL_UNSIGNED_INT, 0);
		skyShader->Unbind();
		glBindVertexArray(0);
	}
	
	//渲染开始/失败/胜利界面，根据value的不同值
	void initialRender(int value) {
		Shader* shader = initialShader;
		vec3 Pos = (camera->GetFront() * 0.25f) + (camera->GetRight() * 0.2f) + (camera->GetUp() * -0.125f) + camera->GetPosition();  // 计算枪的位置
		//vec3 Pos = (camera->GetFront() * 0.25f) + (camera->GetRight() * 0.0f) + (camera->GetUp() * 0.025f) + camera->GetPosition();
		mat4 model = mat4(1.0);  // 初始化点模型的变换矩阵为单位矩阵
		model[0] = vec4(camera->GetRight(), 0.0);  // 将血条模型的 x 轴设置为相机的右方向
		model[1] = vec4(camera->GetUp(), 0.0);  // 将血条模型的 y 轴设置为相机的上方向
		model[2] = vec4(-camera->GetFront(), 0.0);  // 将血条模型的 z 轴设置为相机的反方向
		model[3] = vec4(Pos, 1.0);  // 将血条模型的位置设置为计算得到的模型的位置
		//model[3] = vec4(camera->GetPosition(), 1.0);  // 将点模型的位置设置为相机的位置
		//model = translate(model, camera->GetFront());  // 根据相机的前方向进行平移变换
		model = translate(model, vec3(-0.2, 0.15, -1.0));  // 进行平移变换
		shader->Bind();
		// 更新投影矩阵为相机的透视投影矩阵
		shader->SetMat4("projection", camera->GetProjectionMatrix());
		shader->SetMat4("view", view);
		shader->SetMat4("model", model);

		glActiveTexture(GL_TEXTURE0);

		if(value==1)
			glBindTexture(GL_TEXTURE_2D, initialTexture->GetId());
		else if(value==2)
			glBindTexture(GL_TEXTURE_2D, VictoryTexture->GetId());
		else if(value==3)
			glBindTexture(GL_TEXTURE_2D, DefeatTexture->GetId());

		glBindVertexArray(initial->GetVAO());
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(initial->GetIndices().size()), GL_UNSIGNED_INT, 0);
		initialShader->Unbind();
		glBindVertexArray(0);
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
		//加载天空盒模型
		sky= new Model("res/model/skybox.obj",1000);

		initial = new Model("res/model/initial.obj",0.001);


	}
	// 加载纹理
	void LoadTexture() {
		//加载房间纹理
		roomTexture = new Texture("res/texture/wall.jpg");
		//地板纹理
		floorTexture = new Texture("res/texture/grass.jpg");

		//天空纹理
		skyTexture=new Texture("res/texture/sky.jpg");

		//初始界面纹理
		initialTexture=new Texture("res/texture/initial.jpg");
		//...
		VictoryTexture=new Texture("res/texture/VictoryTexture.jpg");
		//...
		DefeatTexture= new Texture("res/texture/DefeatTexture.jpg");
		
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

		floorShader = new Shader("res/shader/floor.vert", "res/shader/floor.frag");

		//天空着色器
		skyShader= new Shader("res/shader/room.vert", "res/shader/room.frag");

		initialShader=new Shader("res/shader/room.vert", "res/shader/room.frag");


	}

};

#endif // !PLACE_H
