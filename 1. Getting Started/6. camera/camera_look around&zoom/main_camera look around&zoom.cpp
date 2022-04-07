#include <glad/glad.h> //glad头文件添加成功 写在前面 不然会报错
#include <GLFW/glfw3.h> //GLFW头文件添加成功
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_s.h"
#include "stb_image.h"
#include <iostream>


//函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
float mixFactorChange(float& factor, GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x_mouse, double y_mouse);
void scroll_callback(GLFWwindow* window, double x_mouse, double y_mouse);

//常量定义
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//混合度
float mixFactor = 0.0f;

//帧渲染时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;


//鼠标控制相机pitch&yaw
float lastX = SCR_WIDTH/2;
float lastY = SCR_HEIGHT/2; //设置初始鼠标位置为窗口中心
float yaw = -90.0f; //初始状态相机面向-z轴（如果yaw为0，相机面向+x，为了是相机面向-z，需顺时针旋转90度，就是-90，顺时针为-，逆时针为+）
float pitch =  0.0f;
bool firstMouse = true;

//鼠标滚轮控制zoom in&out
float fov = 45.0f;


//视角变换初始定义
glm::vec3 cameraPos =  glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); //此处设置为-1，相机的朝向始终向-z，即指向屏幕的一侧，如果是1，则相机朝向+z，指向人了
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


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
    Shader ourShader("../Shader/shader_camera.vs", "../Shader/shader_camera.fs"); //返回上一级菜单../

    //vertex data
    float vertices[]={
            //顶点+纹理坐标
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    //vertex buffer
    unsigned int VBO;//opengl object:vertex buffer object 将顶点数据一次性从gpu发送至图卡内存
    unsigned int VAO; //opengl object:vertex array object 将顶点属性设置、顶点属性状态及使用哪个VBO画储存起来
    glGenVertexArrays(1, &VAO);//生成VAO
    glGenBuffers(1, &VBO);//生成VBO

    //绑定VAO
    glBindVertexArray(VAO);
    //绑定VBO，顶点数据发送至显存
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //纹理属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    //load & create a texture
    unsigned int texture1, texture2;
    //create texture object 1
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);//绑定
    //设置纹理环绕方式,x/s y/t方向均为重复环绕
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    float color[] = {1.0f, 1.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);//GL_CLAMP_TO_BORDER模式下边界颜色设置
    //设置纹理过滤方式,缩小时mipmap方法，mipmap:linear/filtering:linear,放大时采用linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //load texture(stb库)
    int width, height, nrChannels;
    unsigned char* data = stbi_load("../Texture/container.jpg", &width, &height, &nrChannels, 0);
    //基于load image生成纹理
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);//当前texture object已包含之前load的贴图信息
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture!" << std::endl;
    }
    //释放image内存
    stbi_image_free(data);

    //create texture object 2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);//绑定
    //设置纹理环绕方式,x/s y/t方向均为重复环绕
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //设置纹理过滤方式,缩小时mipmap方法，mipmap:linear/filtering:linear,放大时采用linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //load texture(stb库)
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("../Texture/awesomeface.png", &width, &height, &nrChannels, 0);
    //基于load image生成纹理
    if(data)
    {
        //png图片有alpha通道，需要将贴图信息存储形式设置为GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);//当前texture object已包含之前load的贴图信息
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture!" << std::endl;
    }
    //释放image内存
    stbi_image_free(data);

    //设置uniform sampler2d的值
    ourShader.use();
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture2"), 1);//传统设置uniform变量的方法
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);//使用写好的shader类进行设置

    //10个cube在世界中的位置
    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
    };

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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //确定用什么颜色来清理屏幕
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //确定清理哪一个缓冲区，这里是用上面的颜色来清理颜色缓冲区

        //绑定纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        //画container
        ourShader.use(); //激活shaderProgram，应用shaderProgram附加的着色器进行绘制

        //视角变换矩阵
        glm::mat4 view = glm::mat4(1.0f);
        //lookat的第二个参数表示相机始终看向当前位置的向Z轴减1的位置（无论相机位置怎么变，危机始终看向当前z轴-1）
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);

        //投影变换矩阵
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov),(float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        //投影变换传递给顶点着色器
        ourShader.use();
        ourShader.setMat4("projection", projection);

        //绘制10个立方体
        for(int i = 0; i < 10; ++i)
        {
            glm::mat4 model = glm::mat4(1.0f); //基础单位矩阵
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f,0.3f,0.5f));
            ourShader.setMat4("model", model);
            glBindVertexArray(VAO); //绑定VAO
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //事件响应并交换前后缓冲区
        glfwSwapBuffers(window);//交换前后缓冲区，在屏幕上显示图像
        glfwPollEvents(); //检查是否有事件触发 更新window状态 响应回调函数中的自定义功能
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &EBO);

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
    float cameraSpeed = 2.5f * deltaTime;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos = cameraPos + cameraFront * cameraSpeed;//当前位置z轴向前走（cameraFront * cameraSpeed是负的，cameraPos越加越小，越小就是越往前）
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos = cameraPos - cameraFront * cameraSpeed; //当前位置z轴向后走
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos = cameraPos - glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; //当前位置x轴向左走
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos = cameraPos + glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;//当前位置x轴向右走
    }


}

//通过按键改变两张贴图的混合度
float mixFactorChange(float& factor, GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        factor -= 0.001;
        if(factor <= 0.0f)
        {
            factor = 0.0f;
        }
    }
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        factor += 0.001;
        if(factor >= 1.0f)
        {
            factor = 1.0f;
        }
    }

    return factor;
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

    //鼠标控制相机pitch&yaw的灵敏度
    float sensitivity = 0.1f; //灵敏度参数
    xOffset = xOffset * sensitivity;
    yOffset = yOffset * sensitivity;

    //更新pitch&yaw
    yaw = yaw + xOffset; //x控制偏航
    pitch = pitch - yOffset; //y控制俯仰

    //设置pitch角度限制，向上不超过90度，向下不超过-90度
    if(pitch > 89.0f)
    {
        pitch = 89.0f;
    }
    if(pitch < -89.0f)
    {
        pitch = -89.0f;
    }

    //pitch&yaw 实时设置cameraFront(direction)
    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double x_mouse, double y_mouse)
{
    fov = fov + y_mouse;

    if(fov < 1.0f)
    {
        fov = 1.0f;
    }
    if(fov > 45.0f)
    {
        fov = 45.0f;
    }
}