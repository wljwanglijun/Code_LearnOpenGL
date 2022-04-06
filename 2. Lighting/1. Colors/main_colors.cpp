#include <glad/glad.h> //glad头文件添加成功 写在前面 不然会报错
#include <GLFW/glfw3.h> //GLFW头文件添加成功
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_s.h"
#include "camera.h"
#include <iostream>


//函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x_mouse, double y_mouse);
void scroll_callback(GLFWwindow* window, double x_mouse, double y_mouse);

//常量定义
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//帧渲染时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//相机初始化
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH/2;
float lastY = SCR_HEIGHT/2; //设置初始鼠标位置为窗口中心
bool firstMouse = true;

//灯光位置
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


int main(int argc, const char * argv[]) {

    glfwInit(); //glfw库初始化，在使用glfw函数前，必须先初始化
    //以下均为glfw配置，第一个参数为配置的选项，以GLFW_为前缀，第二个参数为配置的z值
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //opengl版本设置
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //opengl版本设置
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE); //使用opengl core profile配置
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //苹果系统需要特别加的一句

    //创建窗口
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Filed to create GLFW window" << std::endl; //有了std::，就不需要在最前面写using namespace std
        glfwTerminate();
        return -1;
    }
    //使创建的window上下文为当前线程的主上下文 意思是之后所有的绘制都是在window这个窗口下
    glfwMakeContextCurrent(window);
    //当window大小发生变化时，viewport大小也跟着同步变化 设置视口大小
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //当鼠标位置移动后，调用mouse_callback函数
    glfwSetCursorPosCallback(window, mouse_callback);
    //当鼠标滚轮移动后，调用scroll_callback函数
    glfwSetScrollCallback(window, scroll_callback);

    //capture the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    //GLAD初始化 GLAD加载OpenGL函数的指针
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Filed to initialize GLAD" << std::endl;
        return -1;
    }

    //启用深度检测
    glEnable(GL_DEPTH_TEST);

    //build & compile shader & link shader program
    Shader lightingShader("../Shader/colors.vs", "../Shader/colors.fs"); //返回上一级菜单../
    Shader lightCubeShader("../Shader/light_cube.vs", "../Shader/light_cube.fs");

    //vertex data
    float vertices[]={
            //顶点坐标 cube
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
    };

    //vertex buffer
    //cubeVAO设置
    unsigned int VBO;//opengl object:vertex buffer object 将顶点数据一次性从gpu发送至图卡内存
    unsigned int cubeVAO; //opengl object:vertex array object 将顶点属性设置、顶点属性状态及使用哪个VBO画储存起来
    glGenVertexArrays(1, &cubeVAO);//生成VAO
    glGenBuffers(1, &VBO);//生成VBO
    //绑定VBO，顶点数据发送至显存
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //绑定VAO
    glBindVertexArray(cubeVAO);
    //顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //lightVAO设置
    unsigned int lightVAO;
    glGenVertexArrays(1, &cubeVAO);//生成VAO
    glBindVertexArray(cubeVAO); //绑定VAO
    glBindBuffer(GL_ARRAY_BUFFER,VBO); //绑定VBO 此VBO中绑定了立方体顶点信息
    //顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //render loop 一直不停的绘制直至结束
    while(!glfwWindowShouldClose(window)) //每一次循环开始时都检查是否需要关闭窗口
    {
        //计算前后帧间隔时间
        float currentFrame = 0.0;
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //按键输入监测
        processInput(window);

        //绘制指令
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //确定用什么颜色来清理屏幕
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //确定清理哪一个缓冲区，这里是用上面的颜色来清理颜色缓冲区

        //设置立方体颜色绘制着色器（本身颜色*灯光颜色）
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

        //投影变换矩阵
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(camera.Zoom),(float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        //投影变换传递给顶点着色器
        lightingShader.use();
        lightingShader.setMat4("projection", projection);

        //视角变换矩阵
        glm::mat4 view = glm::mat4(1.0f);
        //lookat的第二个参数表示相机始终看向当前位置的向Z轴减1的位置（无论相机位置怎么变，危机始终看向当前z轴-1）
        view = camera.GetViewMatrix();
        lightingShader.setMat4("view", view);

        //模型变换
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        //绘制带颜色的立方体
        glBindVertexArray(cubeVAO); //绑定VAO
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //绘制灯光立方体
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightCubeShader.setMat4("model", model);

        //绘制灯光立方体
        glBindVertexArray(lightVAO); //绑定VAO
        glDrawArrays(GL_TRIANGLES, 0, 36);


        //事件响应并交换前后缓冲区
        glfwSwapBuffers(window);//交换前后缓冲区，在屏幕上显示图像
        glfwPollEvents(); //检查是否有事件触发 更新window状态 响应回调函数中的自定义功能
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    //规范的退出应用 释放glfw占用的内存资源
    glfwTerminate();

    return 0;
}


//回调函数：当window大小发生变化时，viewport大小也跟着同步变化
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0,0,width,height);//前两个参数是左下角坐标，后两个参数是宽/高，数值代表像素
}

//键盘输入相应函数
void processInput(GLFWwindow* window)
{
    //如果按下escape键，将window设置为关闭属性，glfwWindowShouldClose(window)返回true，render loop结束，窗口关闭
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    //键盘控制相机移动
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboardInput(FORWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboardInput(BACKWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboardInput(LEFT, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboardInput(RIGHT, deltaTime);
    }
}


void mouse_callback(GLFWwindow* window, double x_mouse, double y_mouse)
{
    float xPos = static_cast<float>(x_mouse);
    float yPos = static_cast<float>(y_mouse);

    //避免初始获取鼠标位置时的相机瞬间跳转
    if(firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    //计算前后帧鼠标的位移量
    float xOffset = xPos - lastX;
    float yOffset = yPos - lastY;
    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseInput(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double x_mouse, double y_mouse)
{
    camera.ProcessScrollInput(static_cast<float>(y_mouse));
}