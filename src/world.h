#ifndef WORLD_H
#define WORLD_H

#include <GLFW/glfw3.h>
#include "place.h"
#include "player.h"
#include "boss.h"
#include "camera.h"
#include "ballmanager.h"
#include"setmaze.h"
#include <irrKlang/irrKlang.h>			//irrKlang：一个跨平台的音频库

const int bossnum = 50;  //怪物数量
int nowbossnum = 1;
irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
class World {
private:
	GLFWwindow* window;
	vec2 windowSize;

	Place* place;				// 场景
	Player* player;				// 玩家
	Boss* boss[bossnum];				//怪物数组
	Camera* camera;				// 摄像机
	BallManager* ball;			// 小球

	// 阴影
	GLuint depthMap;			// 深度贴图
	GLuint depthMapFBO;			// 深度贴图帧缓冲对象
	Shader* simpleDepthShader;	// 简单深度着色器
	mat4 lightSpaceMatrix;		// 光源空间矩阵

	GLint totalTime;		//游戏总时间
	GLint remainingTime;	//剩余时间
	GLint times;			//用于判断deltaTime是否累计达1s

	bool isStart;			//游戏开始标志...当人物位于迷宫入口时为true；
	bool isWin;				//游戏通过标志
public:
	World(GLFWwindow* window, vec2 windowSize) {
		this->window = window;
		this->windowSize = windowSize;

		simpleDepthShader = new Shader("res/shader/shadow.vert", "res/shader/shadow.frag"); // 创建简单深度着色器

		vec3 lightPos(0.0, 400.0, 150.0);
		mat4 lightProjection = ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 500.0f); // 光源投影矩阵
		mat4 lightView = lookAt(lightPos, vec3(0.0f), vec3(0.0, 1.0, 0.0)); // 光源视图矩阵
		lightSpaceMatrix = lightProjection * lightView; // 光源空间矩阵

		camera = new Camera(window); // 创建摄像机对象
		place = new Place(windowSize, camera); // 创建场景对象
		player = new Player(windowSize, camera); // 创建玩家对象
		ball = new BallManager(windowSize, camera); // 创建小球管理器对象

		for (int i = 0; i < bossnum; i++)
			boss[i] = new Boss(camera, player);			//创建怪物对象

		this->totalTime = 360;						//初始化游戏时间
		this->remainingTime = totalTime;				//剩余游戏时间
		this->times = 0;
		this->isStart = false;

		glGenFramebuffers(1, &depthMapFBO); // 生成深度贴图帧缓冲对象
		glGenTextures(1, &depthMap); // 生成深度贴图
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // 设置深度贴图的纹理过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // 设置深度贴图的纹理环绕方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	// 更新数据
	void Update(float deltaTime) {

		

		camera->Update(deltaTime); // 更新摄像机
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			SoundEngine->play2D("res/sounds/shoot0.wav", GL_FALSE);	//调用irrKlang库的音频处理功能
			for (int i = 0; i < nowbossnum; i++) {
				if (boss[i]->GetHp() > 0)//若Boss生命值小于0,销毁对象(取消更新、渲染)
					if (isStart)
						boss[i]->Update(camera->GetPosition(), camera->GetFront(), true);   // 更新怪物的位置和状态（鼠标左键按下）
				player->Update(deltaTime, true);									// 更新人物的位置和状态
			}
		}
		else {
			for (int i = 0; i < nowbossnum; i++) {
				if (isStart)
					boss[i]->Update(camera->GetPosition(), camera->GetFront(), false); // 更新怪物的位置和状态（鼠标左键未按下）
				player->Update(deltaTime, false);
			}
		}
		//随机更新怪物数量
		if (isStart&& rand()%1000 == 0 && nowbossnum < bossnum) {
			nowbossnum++;
		}

		place->Update(); // 更新场景



		// 更新游戏剩余时间、
		static float elapsedTime = 0.0f;
		elapsedTime += deltaTime;
		if (elapsedTime >= 1.0f) {
			elapsedTime -= 1.0f;
			remainingTime--;
			std::cout << "Remaining Time: " << remainingTime <<"s" << std::endl;
		}

		//将显示坐标映射到迷宫坐标...检测人物是否位于入口或出口...
		Maze maze;//获取迷宫对象
		std::vector<std::vector<int>> mazeArray = maze.GetMazeArray();//获取迷宫二维数组
		int width = mazeArray[0].size();
		int height = mazeArray.size();
		vec3 pos = player->GetPosition();
		GLint Index_col = static_cast<GLint>(pos.x / 20);//列
		GLint Index_row = static_cast<GLint>(pos.z / 20);//行
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
	// 渲染模型
		void Render() {
		RenderDepth(); // 渲染深度贴图
		player->Render(); // 渲染玩家
		place->RoomRender(NULL, depthMap); // 渲染场景中的迷宫（不使用深度贴图）
		place->SunRender(); // 渲染场景中的太阳
		//ball->Render(NULL, depthMap); // 渲染小球（不使用深度贴图）
		place->FloorRender();		     //渲染地板
		for (int i = 0; i < nowbossnum; i++)
			if (boss[i]->GetHp() > 0)//若Boss生命值小于0,销毁对象(取消更新、渲染)
				boss[i]->Render();					//渲染boss
		//place->DrawHUD(player->GetHp(), remainingTime);

	}

	GLuint GetScore() {
		return ball->GetScore();// 获取得分
	}
	// 判断游戏是否结束
	bool IsOver() {
		//return ball->IsOver();// 判断小球是否超出范围，游戏是否结束
		//游戏结束结束判断逻辑....
		// 
		//判断游戏时间
		if (remainingTime <= 0) {
			std::cout << "游戏时间到！！" << std::endl;
			return true;
		}

		//人物生命值判断
		GLint HP = player->GetHp();
		if(HP<=0) {
			std::cout << "你已死亡！！" << std::endl;
			return true;
		}

		if (isWin) {
			std::cout << "恭喜，您已通关！！" << std::endl;
			return true;
		}
			

		return false;

	}
	// 设置游戏模式
	void SetGameModel(GLuint num) {// 设置小球的游戏模式
		ball->SetGameModel(num);
	}




private:
	// 渲染深度图
	void RenderDepth() {
		// 绑定深度贴图帧缓冲对象
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		// 将深度贴图附加到帧缓冲的深度附件
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE); // 禁用颜色输出
		glReadBuffer(GL_NONE); // 禁用颜色读取

		simpleDepthShader->Bind(); // 使用简单深度着色器
		simpleDepthShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix); // 设置光源空间矩阵

		glViewport(0, 0, 1024, 1024); // 设置视口大小为深度贴图的大小
		glClear(GL_DEPTH_BUFFER_BIT); // 清除深度缓冲区
		place->RoomRender(simpleDepthShader); // 渲染场景中的房间（使用深度贴图）
		ball->Render(simpleDepthShader); // 渲染小球（使用深度贴图）
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // 解绑帧缓冲对象

		glViewport(0, 0, windowSize.x, windowSize.y); // 恢复视口大小为窗口大小
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色和深度缓冲区
	}

};

#endif
