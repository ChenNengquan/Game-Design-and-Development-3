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
	// ����
	Model* room;                        // ����ģ��
	Texture* roomTexture;               // ��������
	Shader* roomShader;                 // ������ɫ��

	Model* floor;						//�ذ�
	Texture* floorTexture;              // �ذ�����
	Shader* floorShader;					//�ذ���ɫ��

	// ̫��
	Model* sun;                         // ̫��ģ��
	vec3 lightPos;                      // ��Դλ��
	mat4 lightSpaceMatrix;              // ��ռ�������ڽ�������������ת��Ϊ�Թ�ԴΪ���ĵ�����
	Shader* sunShader;                  // ̫����ɫ��


	Shader* hudShader;					//������ɫ��
	Texture* hudTexture;				//��������

	Camera* camera;                     // �����
	
	// ģ�ͱ任����
	mat4 model;
	mat4 projection;
	mat4 view;

public:
	//���캯������������һ����ΪwindowSize��vec2������һ����Ϊcamera��Camera����ָ�����
	Place(vec2 windowSize, Camera* camera) {
		this->windowSize = windowSize;
		this->camera = camera;
		// �ù�Դ��λ��(0.0, 400.0, 150.0)��ʼ��lightPos��Ա������
		this->lightPos = vec3(0.0, 400.0, 150.0);
		//this->lightPos = vec3(-60.0, 400.0, -400.0);
		//����һ�����ڹ�Դ������ͶӰ���󡣸þ������˹�Դ���Ӿ��ռ䡣
		//mat4 lightProjection = ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 500.0f);
		mat4 lightProjection = ortho(-500.0f, 500.0f, -500.0f, 500.0f, 1.0f, 1000.0f);
		// ����һ����Դ����ͼ���󡣸þ��󽫹�Դ��λ�ڳ����У��������˹�Դ�ĳ���
		mat4 lightView = lookAt(lightPos, vec3(0.0f), vec3(0.0, 1.0, 0.0));
		this->lightSpaceMatrix = lightProjection * lightView;
		LoadModel();//���ط����̫����ģ�͡�
		LoadTexture();//���ط��������
		LoadShader();//���ط����̫������ɫ��
	}
	// ���±任����
	void Update() {
		//��ģ�ͱ任��������Ϊ��λ���󣬱�ʾû��Ӧ�ö����ģ�ͱ任���⽫����ģ�͵�ԭʼ״̬��
		this->model = mat4(1.0);
		//��ȡ�������ͼ���󲢽��丳ֵ�����view��Ա�������������ͼ�������˹۲��ߵ�λ�úͳ������ڽ���������������ϵת�����������ϵ��
		this->view = camera->GetViewMatrix();
		/*��������Ĳ�������͸��ͶӰ���󲢽��丳ֵ�����projection��Ա������
		*͸��ͶӰ�������˳����Ŀ��ӷ�Χ��͸��Ч����
		*���ڽ��������ϵ�е�����ͶӰ����Ļ����ϵ��
		*/
		this->projection = perspective(radians(camera->GetZoom()), windowSize.x / windowSize.y, 0.1f, 500.0f);

	}
	// ��Ⱦ����(�Թ�)
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
		glActiveTexture(GL_TEXTURE0);//��������
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
	// ��Ⱦ̫��
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
	//��Ⱦ�ذ�
	void FloorRender() {
		Shader* shader = floorShader;
		shader->Bind();
		shader->SetMat4("projection", projection);
		shader->SetMat4("model", model);
		shader->SetMat4("view", view);

		//��������.....��������..(��������LoadTexture()������������Ȼ������..����)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture->GetId());

		glBindVertexArray(floor->GetVAO());
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(floor->GetIndices().size()), GL_UNSIGNED_INT, 0);
		shader->Unbind();
		glBindVertexArray(0);
	}

	void DrawHUD(GLint PlyerHP, GLint remainingTime) {
		// ʹ��HUD��ɫ������
		hudShader->Bind();
		// ����uniform����
		hudShader->SetFloat("health", PlyerHP);
		hudShader->SetFloat("time", remainingTime);
		// ��HUD����
		//glBindTexture(GL_TEXTURE_2D, hudTexture);

		// ���ö�������
		GLfloat vertices[] = {
			// ��������    ��������
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 1.0f
		};

		// �����Ͱ󶨶��㻺�����VBO��
		GLuint VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// ���ö�������ָ��
		GLuint positionAttrib = glGetAttribLocation(hudShader->GetProgram(), "position");
		glEnableVertexAttribArray(positionAttrib);
		glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

		GLuint texCoordAttrib = glGetAttribLocation(hudShader->GetProgram(), "texCoord");
		glEnableVertexAttribArray(texCoordAttrib);
		glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

		// ����HUD
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		// ����ɾ�����㻺�����VBO��
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &VBO);

		// ���HUD����
		glBindTexture(GL_TEXTURE_2D, 0);

		// ������ɫ��
		hudShader->Unbind();
	}
	
	
private:
	// ����ģ��
	void LoadModel() {
		//�����Թ�ģ��
		room = new Model("res/model/maze.obj");
		//����̫��ģ��
		sun = new Model("res/model/sun.obj");
		//���صذ�
		floor= new Model("res/model/floor.obj");
	}
	// ��������
	void LoadTexture() {
		//���ط�������
		roomTexture = new Texture("res/texture/wall.jpg");
		floorTexture = new Texture("res/texture/grass.jpg");
		
	}
	// ������ɫ��
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
