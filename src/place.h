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
	Model* sky;                        // ���ģ��
	Texture* skyTexture;               // �������
	Shader* skyShader;                 // �����ɫ��

	Model* initial;							//��ʼ��ʤ����ʧ�ܽ���ģ��
	Texture* initialTexture;               // ��ʼ��������
	Texture* DefeatTexture;					//ʤ����������
	Texture* VictoryTexture;					//ʧ�ܽ�������
	Shader* initialShader;                 // ������ɫ��


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

	//��Ⱦ���
	void skyRender() {
		Shader* shader = skyShader;
		shader->Bind();
		// ����ͶӰ����Ϊ�����͸��ͶӰ����
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
	
	//��Ⱦ��ʼ/ʧ��/ʤ�����棬����value�Ĳ�ֵͬ
	void initialRender(int value) {
		Shader* shader = initialShader;
		vec3 Pos = (camera->GetFront() * 0.25f) + (camera->GetRight() * 0.2f) + (camera->GetUp() * -0.125f) + camera->GetPosition();  // ����ǹ��λ��
		//vec3 Pos = (camera->GetFront() * 0.25f) + (camera->GetRight() * 0.0f) + (camera->GetUp() * 0.025f) + camera->GetPosition();
		mat4 model = mat4(1.0);  // ��ʼ����ģ�͵ı任����Ϊ��λ����
		model[0] = vec4(camera->GetRight(), 0.0);  // ��Ѫ��ģ�͵� x ������Ϊ������ҷ���
		model[1] = vec4(camera->GetUp(), 0.0);  // ��Ѫ��ģ�͵� y ������Ϊ������Ϸ���
		model[2] = vec4(-camera->GetFront(), 0.0);  // ��Ѫ��ģ�͵� z ������Ϊ����ķ�����
		model[3] = vec4(Pos, 1.0);  // ��Ѫ��ģ�͵�λ������Ϊ����õ���ģ�͵�λ��
		//model[3] = vec4(camera->GetPosition(), 1.0);  // ����ģ�͵�λ������Ϊ�����λ��
		//model = translate(model, camera->GetFront());  // ���������ǰ�������ƽ�Ʊ任
		model = translate(model, vec3(-0.2, 0.15, -1.0));  // ����ƽ�Ʊ任
		shader->Bind();
		// ����ͶӰ����Ϊ�����͸��ͶӰ����
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
	// ����ģ��
	void LoadModel() {
		//�����Թ�ģ��
		room = new Model("res/model/maze.obj");
		//����̫��ģ��
		sun = new Model("res/model/sun.obj");
		//���صذ�
		floor= new Model("res/model/floor.obj");
		//������պ�ģ��
		sky= new Model("res/model/skybox.obj",1000);

		initial = new Model("res/model/initial.obj",0.001);


	}
	// ��������
	void LoadTexture() {
		//���ط�������
		roomTexture = new Texture("res/texture/wall.jpg");
		//�ذ�����
		floorTexture = new Texture("res/texture/grass.jpg");

		//�������
		skyTexture=new Texture("res/texture/sky.jpg");

		//��ʼ��������
		initialTexture=new Texture("res/texture/initial.jpg");
		//...
		VictoryTexture=new Texture("res/texture/VictoryTexture.jpg");
		//...
		DefeatTexture= new Texture("res/texture/DefeatTexture.jpg");
		
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

		floorShader = new Shader("res/shader/floor.vert", "res/shader/floor.frag");

		//�����ɫ��
		skyShader= new Shader("res/shader/room.vert", "res/shader/room.frag");

		initialShader=new Shader("res/shader/room.vert", "res/shader/room.frag");


	}

};

#endif // !PLACE_H
