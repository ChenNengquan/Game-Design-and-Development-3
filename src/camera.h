#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "setmaze.h"
using namespace glm;

// 摄像机默认参数
const float YAW = -90.0f;			// 初始偏航角
const float PITCH = 0.0f;			// 初始俯仰角
const float SPEED = 30.0f;			// 摄像头移动速度
const float HEIGHT = 10.0f;			// 摄像头高度
//const float HEIGHT = 50.0f;			// 摄像头高度
const float SENSITIVITY = 0.05f;     // 视角移动速度
const float ZOOM = 45.0f;			// 视角范围
const float JUMPTIME = 0.1f;		// 跳跃持续时间
//const float GRAVITY = 9.8f;			// 重力加速度
const float GRAVITY = 0.8f;			// 重力加速度
const float JUMPSTRENGTH = 60.0f;	// 跳跃加速度



class Camera {
private:
	GLFWwindow* window;

	int screenWidth;          // 屏幕宽度
	int screenHeight;         // 屏幕高度
	mat4 projectionMatrix;	  // 投影矩阵

	vec3 position;				// 摄像头位置
	vec3 front;					// 指向摄像头前方的单位向量
	vec3 right;					// 指向摄像头右方的单位向量
	vec3 up;					// 指向摄像头上方的单位向量
	vec3 worldUp;				// 指向世界的上方的单位向量，不会改变

	// 跳跃
	float jumpTimer;			// 跳跃持续时间
	float gravity;				// 重力加速度
	bool isJump;				// 用于判断当前是不是处于跳跃状态

	// 欧拉角
	float yaw;					// 偏航角
	float pitch;				// 俯仰角

	// 摄像头选项
	float movementSpeed;		// 摄像头移动速度
	float mouseSensitivity;		// 视角移动速度
	float zoom;					// 视角的大小，一般45.0f比较真实

	// 鼠标位置
	double mouseX;
	double mouseY;
	bool firstMouse;			// 鼠标是否第一次进入窗口
public:
	Camera(GLFWwindow* window) {
		this->window = window;
		// 获取窗口的宽度和高度
		glfwGetWindowSize(window, &this->screenWidth, &this->screenHeight);

		movementSpeed = SPEED;                   // 摄像头移动速度
		mouseSensitivity = SENSITIVITY;           // 鼠标灵敏度
		zoom = ZOOM;                             // 视角范围
		firstMouse = true;                       // 是否是第一次移动鼠标

		jumpTimer = 0;                           // 跳跃持续时间
		isJump = false;                          // 是否正在跳跃
		gravity = -GRAVITY;                      // 重力加速度

		position = vec3(0.0f, HEIGHT, 500.0f);		// 摄像头的初始位置
		worldUp = vec3(0.0f, 1.0f, 0.0f);         // 世界的上方向量
		yaw = YAW;                               // 初始偏航角
		pitch = PITCH;                           // 初始俯仰角

		// 计算和设置投影矩阵
		projectionMatrix = perspective(radians(zoom), static_cast<float>(this->screenWidth) / this->screenHeight, 0.1f, 1000.0f);
		UpdateCamera();                          // 更新摄像头参数
	}
	// 更新摄像头的数据和控制键盘、鼠标输入
	void Update(float deltaTime) {
		//鼠标移动响应
		MouseMovement();
		//键盘敲击响应
		KeyboardInput(deltaTime);
		// 在绘制循环的末尾调用绘制HUD的函数
		//DrawHUD();
		// 显示当前位置的坐标点
		//std::cout << "Current Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;

	}

	mat4 GetViewMatrix() {
		return lookAt(position, position + front, up);
	}

	vec3 GetPosition() {
		return position;
	}

	vec3 GetFront() {
		return front;
	}

	vec3 GetRight() {
		return right;
	}

	vec3 GetUp() {
		return up;
	}

	float GetZoom() {
		return zoom;
	}
	void DrawHUD() {
		
	}

	mat4 GetProjectionMatrix() {
		return projectionMatrix;
	}

private:
	// 鼠标输入
	void MouseMovement() {
		double newMouseX, newMouseY;

		glfwGetCursorPos(window, &newMouseX, &newMouseY);

		if (firstMouse) {
			mouseX = newMouseX;
			mouseY = newMouseY;
			firstMouse = false;
		}

		yaw += ((newMouseX - mouseX) * mouseSensitivity);
		pitch += ((mouseY - newMouseY) * mouseSensitivity);

		mouseX = newMouseX;
		mouseY = newMouseY;

		// 保证俯仰角在-90°和90°之间
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		// 更新Front，Right，Up
		UpdateCamera();
	}
	// 键盘输入
	void KeyboardInput(float deltaTime) {
		// 计算移动速度
		float velocity = movementSpeed * deltaTime;
		// 计算摄像头前方向量
		vec3 forward = normalize(cross(worldUp, right));
		//移动后位置
		vec3 pos=position;
		//加速----speed*2
		//坐标逻辑加减
		bool isadd = false;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			velocity *= 2;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			pos += forward * velocity;
			isadd = true;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			pos -= forward * velocity;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			pos -= right * velocity;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
			pos += right * velocity;
			isadd = true;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isJump) {
			jumpTimer = JUMPTIME;
			//jumpTimer = 0.2f;
			isJump = true;
		}
		if (jumpTimer > 0) {
			gravity += (JUMPSTRENGTH * (jumpTimer / JUMPTIME)) * deltaTime;
			jumpTimer -= deltaTime;
		}
		gravity -= GRAVITY * deltaTime;
		pos.y += gravity * deltaTime * 10;

		if (pos.y < HEIGHT) {
			pos.y = HEIGHT;
			gravity = 0;
			isJump = false;
		}

		CheckCollision(pos, velocity, forward);

		//输出当前索引位置
		GLint Index_col = static_cast<GLint>(pos.x / 20);
		GLint Index_row = static_cast<GLint>(pos.z / 20);
		//显示摄像头相关位置，用于调试
		//std::cout << "Current Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
		//std::cout << "Current Position:<<(" << position.x << ", " << position.y << ", " << position.z << ")"<< "Index:(" << Index_row << ", " << Index_col << ")" << std::endl;

	}
	// 碰撞检查
	void CheckCollision(vec3 pos, float velocity, vec3 forward) {
		// 根据迷宫的二维数组检查前方是否存在障碍物
		Maze maze;
		std::vector<std::vector<int>> mazeArray = maze.GetMazeArray();
		int width = mazeArray[0].size();
		int height = mazeArray.size();

		GLint Index_col = static_cast<GLint>(pos.x / 20);
		GLint Index_row = static_cast<GLint>(pos.z / 20);

		// 检查下一个位置是否超出迷宫范围
		if (Index_row >= 0 && Index_row < height && Index_col >= 0 && Index_col < width) {
			// 检查下一个位置是否是障碍物（值为9）
			if (mazeArray[Index_row][Index_col] == 9) {
				// 有障碍物..执行碰撞处理逻辑...
				return;
			}
		}

		// 更新摄像头位置
		position = pos;


		/*if (position.x > 90.0f)
			position.x = 90.0f;
		if (position.x < -90.0f)
			position.x = -90.0f;
		if (position.z > 75.0f)
			position.z = 75.0f;
		if (position.z < -35.0f)
			position.z = -35.0f;*/
	}
	// 更新摄像头的相关向量
	void UpdateCamera() {
		vec3 front;
		front.x = cos(radians(yaw)) * cos(radians(pitch));
		front.y = sin(radians(pitch));
		front.z = sin(radians(yaw)) * cos(radians(pitch));
		this->front = normalize(front);

		this->right = normalize(cross(this->front, this->worldUp));
		this->up = normalize(cross(this->right, this->front));
	}


};

#endif // !CAMERA_H
