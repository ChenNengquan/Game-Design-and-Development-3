#include <glad/glad.h>                     // 包含OpenGL函数声明
#include <GLFW/glfw3.h>                     // 包含GLFW库的函数声明
#include "world.h"                          // 包含自定义的world.h头文件
#include "setmaze.h"                        //生成maze.obj
#include <irrKlang/irrKlang.h>			//irrKlang：一个跨平台的音频库

#pragma comment(lib, "library/lib/irrKlang.lib")	//irrKlang：一个跨平台的音频库

void OpenWindow();                          // 声明OpenWindow函数
void PrepareOpenGL();                       // 声明PrepareOpenGL函数

irrklang::ISoundEngine* Music = irrklang::createIrrKlangDevice();

GLFWwindow* window;                         // 定义一个GLFW窗口对象指针
vec2 windowSize;                            // 定义一个vec2类型的窗口大小变量

int main() {
    const double TARGET_FRAME = 0.016667;   // 目标帧率为1/60秒，实现60帧
    const double FRAME_ALPHA = 0.25;        // 计算系数
    double currentFrame;                     // 当前帧时间
    double deltaTime;                        // 时间差
    double lastFrame;                         // 上一帧时间
    double frameTime = 0.0;                   // 帧时间
    double renderAccum = 0.0;                 // 渲染累积时间
    double smoothFrameTime = TARGET_FRAME;    // 平滑的帧时间

    srand(time(0));                          // 随机数种子初始化

    Maze maze;                             //创建Maze对象，生成maze.obj  
    maze.GenerateMazeOBJ("maze.obj");

    GLuint gameModel = 1;                     // 定义一个无符号整数变量gameModel并初始化为1
    cout << "------------Game Start------------\n";

    //cin >> gameModel;                         // 从标准输入读取游戏模式
    cout << "\n";

    OpenWindow();                             // 调用OpenWindow函数创建窗口
    PrepareOpenGL();                          // 调用PrepareOpenGL函数准备OpenGL

    World world(window, windowSize);          // 创建World对象，传入窗口和窗口大小



    currentFrame = glfwGetTime();             // 获取当前时间
    lastFrame = currentFrame;                  // 初始化上一帧时间

    // world.SetGameModel(gameModel);            // 设置World对象的游戏模式
    world.SetGameModel(1);
    float gameTime = 0;                       // 初始化游戏时间为0

    Music->play2D("res/sounds/background.wav", GL_FALSE);	//调用irrKlang库的音频处理功能

    while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        currentFrame = glfwGetTime();          // 获取当前时间
        deltaTime = currentFrame - lastFrame;  // 计算时间差
        lastFrame = currentFrame;               // 更新上一帧时间
        renderAccum += deltaTime;               // 累积渲染时间

        if (renderAccum >= TARGET_FRAME) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // 清除颜色缓冲区和深度缓冲区

            renderAccum -= TARGET_FRAME;                         // 减去目标帧时间

            world.Update(deltaTime);                             // 更新World对象
            if (world.IsOver())                                   // 如果游戏结束则退出循环
                break;
            world.Render();                                      // 渲染World对象
        }
        glfwSwapBuffers(window);                                 // 交换前后缓冲区
        glfwPollEvents();                                        // 处理窗口事件
    }
    glfwTerminate();                                             // 终止GLFW

    cout << "----------------------------您的得分为：" << world.GetScore() << " ----------------------------" << endl;
    return 0;
}

void OpenWindow() {
    const char* TITLE = "Maze Game";                            // 窗口标题
    int WIDTH = 1960;                                            // 窗口宽度
    //WIDTH = 960; 
    int HEIGHT = 1080;                                           // 窗口高度

    if (!glfwInit()) {                                           // 初始化GLFW
        cout << "Could not initialize GLFW" << endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                // 设置OpenGL上下文的主版本号为3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                // 设置OpenGL上下文的次版本号为3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);// 设置OpenGL配置为核心模式
    glfwWindowHint(GLFW_REFRESH_RATE, 60);                        // 设置刷新率为60Hz

    window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);   // 创建窗口
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);                               // 将窗口的上下文设置为当前上下文
    glfwSwapInterval(1);                                          // 设置垂直同步为1

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);   // 设置鼠标模式为禁用状态（隐藏鼠标并捕捉
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {   // 初始化GLAD
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glGetError();                                               // 清除之前的OpenGL错误

    windowSize = vec2(WIDTH, HEIGHT);                           // 设置窗口大小为vec2类型的变量

}

void PrepareOpenGL() {
    glEnable(GL_DEPTH_TEST);                                    // 启用深度测试
    glEnable(GL_BLEND);                                         // 启用混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);          // 设置混合函数

    glClearColor(0.529f, 0.808f, 0.922f, 0.0f);                 // 设置清屏颜色为天蓝色
}
