#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "setmaze.h"
using namespace glm;

// �����Ĭ�ϲ���
const float YAW = -90.0f;			// ��ʼƫ����
const float PITCH = 0.0f;			// ��ʼ������
const float SPEED = 30.0f;			// ����ͷ�ƶ��ٶ�
const float HEIGHT = 10.0f;			// ����ͷ�߶�
//const float HEIGHT = 50.0f;			// ����ͷ�߶�
const float SENSITIVITY = 0.05f;     // �ӽ��ƶ��ٶ�
const float ZOOM = 45.0f;			// �ӽǷ�Χ
const float JUMPTIME = 0.1f;		// ��Ծ����ʱ��
//const float GRAVITY = 9.8f;			// �������ٶ�
const float GRAVITY = 0.8f;			// �������ٶ�
const float JUMPSTRENGTH = 60.0f;	// ��Ծ���ٶ�



class Camera {
private:
	GLFWwindow* window;

	int screenWidth;          // ��Ļ���
	int screenHeight;         // ��Ļ�߶�
	mat4 projectionMatrix;	  // ͶӰ����

	vec3 position;				// ����ͷλ��
	vec3 front;					// ָ������ͷǰ���ĵ�λ����
	vec3 right;					// ָ������ͷ�ҷ��ĵ�λ����
	vec3 up;					// ָ������ͷ�Ϸ��ĵ�λ����
	vec3 worldUp;				// ָ��������Ϸ��ĵ�λ����������ı�

	// ��Ծ
	float jumpTimer;			// ��Ծ����ʱ��
	float gravity;				// �������ٶ�
	bool isJump;				// �����жϵ�ǰ�ǲ��Ǵ�����Ծ״̬

	// ŷ����
	float yaw;					// ƫ����
	float pitch;				// ������

	// ����ͷѡ��
	float movementSpeed;		// ����ͷ�ƶ��ٶ�
	float mouseSensitivity;		// �ӽ��ƶ��ٶ�
	float zoom;					// �ӽǵĴ�С��һ��45.0f�Ƚ���ʵ

	// ���λ��
	double mouseX;
	double mouseY;
	bool firstMouse;			// ����Ƿ��һ�ν��봰��
public:
	Camera(GLFWwindow* window) {
		this->window = window;
		// ��ȡ���ڵĿ�Ⱥ͸߶�
		glfwGetWindowSize(window, &this->screenWidth, &this->screenHeight);

		movementSpeed = SPEED;                   // ����ͷ�ƶ��ٶ�
		mouseSensitivity = SENSITIVITY;           // ���������
		zoom = ZOOM;                             // �ӽǷ�Χ
		firstMouse = true;                       // �Ƿ��ǵ�һ���ƶ����

		jumpTimer = 0;                           // ��Ծ����ʱ��
		isJump = false;                          // �Ƿ�������Ծ
		gravity = -GRAVITY;                      // �������ٶ�

		position = vec3(0.0f, HEIGHT, 500.0f);		// ����ͷ�ĳ�ʼλ��
		worldUp = vec3(0.0f, 1.0f, 0.0f);         // ������Ϸ�����
		yaw = YAW;                               // ��ʼƫ����
		pitch = PITCH;                           // ��ʼ������

		// ���������ͶӰ����
		projectionMatrix = perspective(radians(zoom), static_cast<float>(this->screenWidth) / this->screenHeight, 0.1f, 1000.0f);
		UpdateCamera();                          // ��������ͷ����
	}
	// ��������ͷ�����ݺͿ��Ƽ��̡��������
	void Update(float deltaTime) {
		//����ƶ���Ӧ
		MouseMovement();
		//�����û���Ӧ
		KeyboardInput(deltaTime);
		// �ڻ���ѭ����ĩβ���û���HUD�ĺ���
		//DrawHUD();
		// ��ʾ��ǰλ�õ������
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
	// �������
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

		// ��֤��������-90���90��֮��
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		// ����Front��Right��Up
		UpdateCamera();
	}
	// ��������
	void KeyboardInput(float deltaTime) {
		// �����ƶ��ٶ�
		float velocity = movementSpeed * deltaTime;
		// ��������ͷǰ������
		vec3 forward = normalize(cross(worldUp, right));
		//�ƶ���λ��
		vec3 pos=position;
		//����----speed*2
		//�����߼��Ӽ�
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

		//�����ǰ����λ��
		GLint Index_col = static_cast<GLint>(pos.x / 20);
		GLint Index_row = static_cast<GLint>(pos.z / 20);
		//��ʾ����ͷ���λ�ã����ڵ���
		//std::cout << "Current Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
		//std::cout << "Current Position:<<(" << position.x << ", " << position.y << ", " << position.z << ")"<< "Index:(" << Index_row << ", " << Index_col << ")" << std::endl;

	}
	// ��ײ���
	void CheckCollision(vec3 pos, float velocity, vec3 forward) {
		// �����Թ��Ķ�ά������ǰ���Ƿ�����ϰ���
		Maze maze;
		std::vector<std::vector<int>> mazeArray = maze.GetMazeArray();
		int width = mazeArray[0].size();
		int height = mazeArray.size();

		GLint Index_col = static_cast<GLint>(pos.x / 20);
		GLint Index_row = static_cast<GLint>(pos.z / 20);

		// �����һ��λ���Ƿ񳬳��Թ���Χ
		if (Index_row >= 0 && Index_row < height && Index_col >= 0 && Index_col < width) {
			// �����һ��λ���Ƿ����ϰ��ֵΪ9��
			if (mazeArray[Index_row][Index_col] == 9) {
				// ���ϰ���..ִ����ײ�����߼�...
				return;
			}
		}

		// ��������ͷλ��
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
	// ��������ͷ���������
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
