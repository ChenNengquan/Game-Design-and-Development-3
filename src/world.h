#ifndef WORLD_H
#define WORLD_H

#include <GLFW/glfw3.h>
#include "place.h"
#include "player.h"
#include "boss.h"
#include "camera.h"
#include "ballmanager.h"
#include"setmaze.h"
#include <irrKlang/irrKlang.h>			//irrKlang��һ����ƽ̨����Ƶ��

const int bossnum = 50;  //��������
int nowbossnum = 1;
irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
class World {
private:
	GLFWwindow* window;
	vec2 windowSize;

	Place* place;				// ����
	Player* player;				// ���
	Boss* boss[bossnum];				//��������
	Camera* camera;				// �����
	BallManager* ball;			// С��

	// ��Ӱ
	GLuint depthMap;			// �����ͼ
	GLuint depthMapFBO;			// �����ͼ֡�������
	Shader* simpleDepthShader;	// �������ɫ��
	mat4 lightSpaceMatrix;		// ��Դ�ռ����

	GLint totalTime;		//��Ϸ��ʱ��
	GLint remainingTime;	//ʣ��ʱ��
	GLint times;			//�����ж�deltaTime�Ƿ��ۼƴ�1s

	bool isStart;			//��Ϸ��ʼ��־...������λ���Թ����ʱΪtrue��
	bool isWin;				//��Ϸͨ����־
public:
	World(GLFWwindow* window, vec2 windowSize) {
		this->window = window;
		this->windowSize = windowSize;

		simpleDepthShader = new Shader("res/shader/shadow.vert", "res/shader/shadow.frag"); // �����������ɫ��

		vec3 lightPos(0.0, 400.0, 150.0);
		mat4 lightProjection = ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 500.0f); // ��ԴͶӰ����
		mat4 lightView = lookAt(lightPos, vec3(0.0f), vec3(0.0, 1.0, 0.0)); // ��Դ��ͼ����
		lightSpaceMatrix = lightProjection * lightView; // ��Դ�ռ����

		camera = new Camera(window); // �������������
		place = new Place(windowSize, camera); // ������������
		player = new Player(windowSize, camera); // ������Ҷ���
		ball = new BallManager(windowSize, camera); // ����С�����������

		for (int i = 0; i < bossnum; i++)
			boss[i] = new Boss(camera, player);			//�����������

		this->totalTime = 360;						//��ʼ����Ϸʱ��
		this->remainingTime = totalTime;				//ʣ����Ϸʱ��
		this->times = 0;
		this->isStart = false;

		glGenFramebuffers(1, &depthMapFBO); // ���������ͼ֡�������
		glGenTextures(1, &depthMap); // ���������ͼ
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // ���������ͼ��������˷�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // ���������ͼ�������Ʒ�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	// ��������
	void Update(float deltaTime) {

		

		camera->Update(deltaTime); // ���������
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			SoundEngine->play2D("res/sounds/shoot0.wav", GL_FALSE);	//����irrKlang�����Ƶ������
			for (int i = 0; i < nowbossnum; i++) {
				if (boss[i]->GetHp() > 0)//��Boss����ֵС��0,���ٶ���(ȡ�����¡���Ⱦ)
					if (isStart)
						boss[i]->Update(camera->GetPosition(), camera->GetFront(), true);   // ���¹����λ�ú�״̬�����������£�
				player->Update(deltaTime, true);									// ���������λ�ú�״̬
			}
		}
		else {
			for (int i = 0; i < nowbossnum; i++) {
				if (isStart)
					boss[i]->Update(camera->GetPosition(), camera->GetFront(), false); // ���¹����λ�ú�״̬��������δ���£�
				player->Update(deltaTime, false);
			}
		}
		//������¹�������
		if (isStart&& rand()%1000 == 0 && nowbossnum < bossnum) {
			nowbossnum++;
		}

		place->Update(); // ���³���



		// ������Ϸʣ��ʱ�䡢
		static float elapsedTime = 0.0f;
		elapsedTime += deltaTime;
		if (elapsedTime >= 1.0f) {
			elapsedTime -= 1.0f;
			remainingTime--;
			std::cout << "Remaining Time: " << remainingTime <<"s" << std::endl;
		}

		//����ʾ����ӳ�䵽�Թ�����...��������Ƿ�λ����ڻ����...
		Maze maze;//��ȡ�Թ�����
		std::vector<std::vector<int>> mazeArray = maze.GetMazeArray();//��ȡ�Թ���ά����
		int width = mazeArray[0].size();
		int height = mazeArray.size();
		vec3 pos = player->GetPosition();
		GLint Index_col = static_cast<GLint>(pos.x / 20);//��
		GLint Index_row = static_cast<GLint>(pos.z / 20);//��
		if (Index_row >= 0 && Index_row < height && Index_col >= 0 && Index_col < width) {
			if (mazeArray[Index_row][Index_col] == 1) {
				isStart = true;
				std::cout << "Game Start" << std::endl;
			}
			if (mazeArray[Index_row][Index_col] == 2) {
				isWin = true;
				std::cout << "victory" << std::endl;
			}
			
		}

	}
	// ��Ⱦģ��
		void Render() {
		RenderDepth(); // ��Ⱦ�����ͼ
		player->Render(); // ��Ⱦ���
		place->RoomRender(NULL, depthMap); // ��Ⱦ�����е��Թ�����ʹ�������ͼ��
		place->SunRender(); // ��Ⱦ�����е�̫��
		//ball->Render(NULL, depthMap); // ��ȾС�򣨲�ʹ�������ͼ��
		place->FloorRender();		     //��Ⱦ�ذ�
		for (int i = 0; i < nowbossnum; i++)
			if (boss[i]->GetHp() > 0)//��Boss����ֵС��0,���ٶ���(ȡ�����¡���Ⱦ)
				boss[i]->Render();					//��Ⱦboss
		//place->DrawHUD(player->GetHp(), remainingTime);

	}

	GLuint GetScore() {
		return ball->GetScore();// ��ȡ�÷�
	}
	// �ж���Ϸ�Ƿ����
	bool IsOver() {
		//return ball->IsOver();// �ж�С���Ƿ񳬳���Χ����Ϸ�Ƿ����
		//��Ϸ���������ж��߼�....
		// 
		//�ж���Ϸʱ��
		if (remainingTime <= 0) {
			std::cout << "��Ϸʱ�䵽����" << std::endl;
			return true;
		}

		//��������ֵ�ж�
		GLint HP = player->GetHp();
		if(HP<=0) {
			std::cout << "������������" << std::endl;
			return true;
		}

		if (isWin) {
			std::cout << "��ϲ������ͨ�أ���" << std::endl;
			return true;
		}
			

		return false;

	}
	// ������Ϸģʽ
	void SetGameModel(GLuint num) {// ����С�����Ϸģʽ
		ball->SetGameModel(num);
	}




private:
	// ��Ⱦ���ͼ
	void RenderDepth() {
		// �������ͼ֡�������
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		// �������ͼ���ӵ�֡�������ȸ���
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE); // ������ɫ���
		glReadBuffer(GL_NONE); // ������ɫ��ȡ

		simpleDepthShader->Bind(); // ʹ�ü������ɫ��
		simpleDepthShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix); // ���ù�Դ�ռ����

		glViewport(0, 0, 1024, 1024); // �����ӿڴ�СΪ�����ͼ�Ĵ�С
		glClear(GL_DEPTH_BUFFER_BIT); // �����Ȼ�����
		place->RoomRender(simpleDepthShader); // ��Ⱦ�����еķ��䣨ʹ�������ͼ��
		ball->Render(simpleDepthShader); // ��ȾС��ʹ�������ͼ��
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // ���֡�������

		glViewport(0, 0, windowSize.x, windowSize.y); // �ָ��ӿڴ�СΪ���ڴ�С
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // �����ɫ����Ȼ�����
	}

};

#endif
