#include <glad/glad.h>                     // ����OpenGL��������
#include <GLFW/glfw3.h>                     // ����GLFW��ĺ�������
#include "world.h"                          // �����Զ����world.hͷ�ļ�
#include "setmaze.h"                        //����maze.obj
#include <irrKlang/irrKlang.h>			//irrKlang��һ����ƽ̨����Ƶ��

#pragma comment(lib, "library/lib/irrKlang.lib")	//irrKlang��һ����ƽ̨����Ƶ��

void OpenWindow();                          // ����OpenWindow����
void PrepareOpenGL();                       // ����PrepareOpenGL����

irrklang::ISoundEngine* Music = irrklang::createIrrKlangDevice();

GLFWwindow* window;                         // ����һ��GLFW���ڶ���ָ��
vec2 windowSize;                            // ����һ��vec2���͵Ĵ��ڴ�С����

int main() {
    const double TARGET_FRAME = 0.016667;   // Ŀ��֡��Ϊ1/60�룬ʵ��60֡
    const double FRAME_ALPHA = 0.25;        // ����ϵ��
    double currentFrame;                     // ��ǰ֡ʱ��
    double deltaTime;                        // ʱ���
    double lastFrame;                         // ��һ֡ʱ��
    double frameTime = 0.0;                   // ֡ʱ��
    double renderAccum = 0.0;                 // ��Ⱦ�ۻ�ʱ��
    double smoothFrameTime = TARGET_FRAME;    // ƽ����֡ʱ��

    srand(time(0));                          // ��������ӳ�ʼ��

    Maze maze;                             //����Maze��������maze.obj  
    maze.GenerateMazeOBJ("maze.obj");

    GLuint gameModel = 1;                     // ����һ���޷�����������gameModel����ʼ��Ϊ1
    cout << "------------Game Start------------\n";

    //cin >> gameModel;                         // �ӱ�׼�����ȡ��Ϸģʽ
    cout << "\n";

    OpenWindow();                             // ����OpenWindow������������
    PrepareOpenGL();                          // ����PrepareOpenGL����׼��OpenGL

    World world(window, windowSize);          // ����World���󣬴��봰�ںʹ��ڴ�С



    currentFrame = glfwGetTime();             // ��ȡ��ǰʱ��
    lastFrame = currentFrame;                  // ��ʼ����һ֡ʱ��

    // world.SetGameModel(gameModel);            // ����World�������Ϸģʽ
    world.SetGameModel(1);
    float gameTime = 0;                       // ��ʼ����Ϸʱ��Ϊ0

    Music->play2D("res/sounds/background.wav", GL_FALSE);	//����irrKlang�����Ƶ������

    while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        currentFrame = glfwGetTime();          // ��ȡ��ǰʱ��
        deltaTime = currentFrame - lastFrame;  // ����ʱ���
        lastFrame = currentFrame;               // ������һ֡ʱ��
        renderAccum += deltaTime;               // �ۻ���Ⱦʱ��

        if (renderAccum >= TARGET_FRAME) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // �����ɫ����������Ȼ�����

            renderAccum -= TARGET_FRAME;                         // ��ȥĿ��֡ʱ��

            world.Update(deltaTime);                             // ����World����
            if (world.IsOver())                                   // �����Ϸ�������˳�ѭ��
                break;
            world.Render();                                      // ��ȾWorld����
        }
        glfwSwapBuffers(window);                                 // ����ǰ�󻺳���
        glfwPollEvents();                                        // �������¼�
    }
    glfwTerminate();                                             // ��ֹGLFW

    cout << "----------------------------���ĵ÷�Ϊ��" << world.GetScore() << " ----------------------------" << endl;
    return 0;
}

void OpenWindow() {
    const char* TITLE = "Maze Game";                            // ���ڱ���
    int WIDTH = 1960;                                            // ���ڿ��
    //WIDTH = 960; 
    int HEIGHT = 1080;                                           // ���ڸ߶�

    if (!glfwInit()) {                                           // ��ʼ��GLFW
        cout << "Could not initialize GLFW" << endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                // ����OpenGL�����ĵ����汾��Ϊ3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                // ����OpenGL�����ĵĴΰ汾��Ϊ3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);// ����OpenGL����Ϊ����ģʽ
    glfwWindowHint(GLFW_REFRESH_RATE, 60);                        // ����ˢ����Ϊ60Hz

    window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);   // ��������
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);                               // �����ڵ�����������Ϊ��ǰ������
    glfwSwapInterval(1);                                          // ���ô�ֱͬ��Ϊ1

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);   // �������ģʽΪ����״̬��������겢��׽
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {   // ��ʼ��GLAD
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glGetError();                                               // ���֮ǰ��OpenGL����

    windowSize = vec2(WIDTH, HEIGHT);                           // ���ô��ڴ�СΪvec2���͵ı���

}

void PrepareOpenGL() {
    glEnable(GL_DEPTH_TEST);                                    // ������Ȳ���
    glEnable(GL_BLEND);                                         // ���û��
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);          // ���û�Ϻ���

    glClearColor(0.529f, 0.808f, 0.922f, 0.0f);                 // ����������ɫΪ����ɫ
}
