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
	GLuint number;						// ��ǰС����Ŀ
	GLuint maxNumber;					// С�������Ŀ
	vec3 basicPos;						// С���������
	vector<vec3> position;				// ���ϴ��ڵ�С������
	float moveSpeed;					// С���ƶ��ٶ�
	GLuint score;						// �÷�
	GLuint gameModel;					// ��Ϸģʽ
	vec3 lightPos;						// ��Դλ��
	mat4 lightSpaceMatrix;				// ��������������ת��Ϊ�Թ�ԴΪ���ĵ�����

	Camera* camera;
	// ģ�ͱ任����
	mat4 model;
	mat4 projection;
	mat4 view;
public:
	BallManager(vec2 windowSize, Camera* camera) {
		this->windowSize = windowSize;  // ���ô��ڴ�С
		this->camera = camera;  // �����������

		basicPos = vec3(0.0, 5.0, -30.0);  // ����С��Ļ���λ��
		number = 0;  // ��ǰС����Ŀ
		maxNumber = 3;  // С��������Ŀ
		moveSpeed = 0.1f;  // С����ƶ��ٶ�
		score = 0;  // �÷�
		this->lightPos = vec3(0.0, 400.0, 150.0);  // ���ù�Դ��λ��
		mat4 lightProjection = ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 500.0f);  // ������Դ������ͶӰ����
		mat4 lightView = lookAt(lightPos, vec3(0.0f), vec3(0.0, 1.0, 0.0));  // �����ӹ�Դλ�ù۲�Ĺ۲����
		this->lightSpaceMatrix = lightProjection * lightView;  // ��ͶӰ����͹۲������ˣ��õ���ռ����

		AddBall();  // ���С��
		LoadModel();  // ����ģ�ͺ���ɫ��
	}

	// ������Ϸģʽ
	void SetGameModel(GLuint num) {
		gameModel = num;
	}
	// ���±任�����ж�����Ƿ����С��
	void Update(vec3 pos, vec3 dir, bool isShoot) {
		this->view = camera->GetViewMatrix();// ��ȡ�������ͼ��������������Ⱦ���ӽǡ�
		//������������Ų��������ڴ�С����Ϣ����͸��ͶӰ��������������Ⱦ��ͶӰЧ����
		this->projection = perspective(radians(camera->GetZoom()), windowSize.x / windowSize.y, 0.1f, 500.0f);
		// �ж��Ƿ����������������ǣ�����л����ж���
		if (isShoot) {
			vector<vec3> temp;
			for (GLuint i = 0; i < position.size(); i++) {
				/*
				* (pos.z - position[i].z)�����ȼ���pos.z��ȥposition[i].z�Ĳ�ֵ���õ�һ����ʾZ�᷽������ֵ��
				*(-dir.z) * dir������������-dir.z����dir���õ�һ������������෴�ĵ�λ������
				*(pos.z - position[i].z) / (-dir.z) * dir���������������������������������ĵ�λ�������������Ľ����һ�������λ�ó����������������ĵ�λ������
				*+ pos����󣬽���������õ��ĵ�λ���������λ��pos��ӣ��õ�������е��λ�ã���ֵ��vec3 des��
				*�����֮�����д����Ŀ���Ǽ����������е��λ�ã������丳ֵ��vec3 des�����������������λ�á���������С��λ��֮��Ĺ�ϵ��������е��λ�á�
				*/
				//������е��λ�ã�ͨ������������λ�ú�С��λ�ý��м��㡣
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
		if (gameModel == 1) {//�����ϷģʽΪ1��������µ�С�򲢷��ء�
			AddBall();
			return;
		}
		for (GLuint i = 0; i < position.size(); i++)
			position[i].z += moveSpeed;//����С���λ�ã�ʹ������Z�������ƶ���

		if (number == 0) {
			//���С������Ϊ0�����������С�����������������������ƶ��ٶȡ�Ȼ������µ�С��
			maxNumber++;
			if (maxNumber == 10) {
				moveSpeed += 0.1f;
				maxNumber = 3;
			}
			AddBall();
		}
	}
	// �ж���Ϸ�Ƿ����
	bool IsOver() {
		if (position.size() > 0)
			if (position[0].z >= 70)
				return true;
		return false;
	}

	GLuint GetScore() {
		return score;
	}
	// ��ȾС��
	void Render(Shader* shader, GLuint depthMap = -1) {
		for (GLuint i = 0; i < position.size(); i++) {
			model = mat4(1.0);//����һ����λ����model�����ڴ洢С���ģ�ͱ任����
			model[3] = vec4(position[i], 1.0);//��С���λ����Ϊƽ�Ʊ任��һ���֣����丳ֵ��model����ĵ����С�
			model = scale(model, vec3(5));//��model����������ű任��ʹС���Ϊ���ʵĴ�С��
			/*
			* �жϴ������ɫ��shader�Ƿ�Ϊ�ա����Ϊ�գ�
			*��ʾδָ���ض�����ɫ������ʹ��Ĭ�ϵ�ballShader��
			*����������£�����ballShader������ͶӰ�������ͼ���󴫵ݸ���ɫ����
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
			// ������ɫ����ģ�;���
			shader->SetMat4("model", model);

			// ��������Ԫ0�����������ͼ
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthMap);

			// ��С��ģ�͵Ķ����������
			glBindVertexArray(ball->GetVAO());

			// ����С��ģ��
			glDrawElements(GL_TRIANGLES, static_cast<GLuint>(ball->GetIndices().size()), GL_UNSIGNED_INT, 0);

			// �����ɫ���Ͷ����������
			shader->Unbind();
			glBindVertexArray(0);

			// ����ģ�;���
			model = mat4(1.0);
		}
	}
private:
	void LoadModel() {
		// ����С��ģ��
		ball = new Model("res/model/dot.obj");

		// ����С�����ɫ��
		ballShader = new Shader("res/shader/ball.vert", "res/shader/ball.frag");

		// ��С�����ɫ��
		ballShader->Bind();

		// ����С����ɫ����uniform����

		// ����С�����ɫ
		ballShader->SetVec3("color", vec3(0.2, 0.5, 0.5f));

		// ������Ӱ��ͼ������Ԫ
		ballShader->SetInt("shadowMap", 0);

		// ���ù�Դλ��
		ballShader->SetVec3("lightPos", lightPos);

		// �������λ��
		ballShader->SetVec3("viewPos", camera->GetPosition());

		// ���ù�Դ��ͶӰ��ͼ����
		ballShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

		// ���С�����ɫ��
		ballShader->Unbind();
	}

	// ���С��
	/*	judgeX��������ɵ�0��1�������ж�С���ˮƽ����
		x������judgeX��ֵ�������С���ˮƽλ�ã���ΧΪ[-30, 30]��
		y���������С��Ĵ�ֱλ�ã���ΧΪ[0, 30]��
		pos���������ɵ�ˮƽ�ʹ�ֱλ�ü���õ���С��λ�á�
		���λ�úϷ��������Ѵ��ڵ�С��λ�ò��ص������򽫸�λ����ӵ�position�����У�������С����Ŀnumber��
		���λ�ò��Ϸ�������������λ�ã������������С��
	*/
	// ���С��
	void AddBall() {
		for (GLuint i = number; i < maxNumber; i++) {
			// �������0��1������ȷ��С���ˮƽ����
			float judgeX = rand() % 2;
			// ����judgeX��ֵ�������С���ˮƽλ�ã���ΧΪ[-30, 30]
			float x = (judgeX >= 0.5) ? rand() % 30 : -(rand() % 30);
			// �������С��Ĵ�ֱλ�ã���ΧΪ[0, 30]
			float y = rand() % 30;
			// ����С�������λ��
			vec3 pos = vec3(basicPos.x + x, basicPos.y + y, basicPos.z);

			// ���λ���Ƿ����Ѵ��ڵ�С��λ���ص�
			if (CheckPosition(pos)) {
				// ���λ�úϷ����򽫸�λ����ӵ�position������
				position.push_back(pos);
				// ����С����Ŀ
				number++;
			}
			else {
				// ���λ�ò��Ϸ�������������λ�ã������������С��
				i--;
			}
		}
	}

	// ����Ѵ���С���λ�ã�������ӵ�С������ص�
	/*
	*	pos��������С��λ�á�
		away�������Ѵ���С��λ��������λ��֮��ľ��롣
		�������С��100�����ʾλ���ص�����������false��
		���������ڵ���100����ʾλ�úϷ�����������true��
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
