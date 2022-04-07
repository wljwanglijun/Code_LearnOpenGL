//
//  main.cpp
//  LearnOpengl
//
//  Created by Lijun Wang on 2021/12/12.
//  Copyright © 2021 Lijun Wang. All rights reserved.
//


#include <iostream>
#include <glad/glad.h> //glad头文件添加成功 写在前面 不然会报错
#include <GLFW/glfw3.h> //GLFW头文件添加成功
#include <cmath>

//函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//常量定义
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//顶点着色器 \0表示字符串结束
const char *vertexShaderSource = "#version 330 core\n" //GLSL版本 对应opengl3.3 core profile
    "layout (location = 0) in vec3 aPos;\n"//顶点位置数据输入
    "layout (location = 1) in vec3 aColor;\n"//顶点颜色数据输入
    "out vec3 ourColor;\n" //将颜色输出至片段着色器
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos, 1.0f);\n" //将顶点给gl_Position，顶点着色器将gl_Position输出至下一环节
    "    ourColor = aColor;\n" //顶点颜色赋值
    "}\0";

//片段着色器
const char *fragmentShaderSource = "#version 330 core\n" //GLSL版本 对应opengl3.3 core profile
    "out vec4 FragColor;\n" //像素颜色输出
    "in vec3 ourColor;\n" //顶点着色器顶点颜色输入
    "void main()\n"
    "{\n"
    "    FragColor = vec4(ourColor, 1.0f);\n" //像素颜色赋值 分别对应RGBA 最后为透明度 1为不透明
    "}\n\0";



int main(int argc, const char * argv[]) {
    
    glfwInit(); //glfw库初始化，在使用glfw函数前，必须先初始化
    //以下均为glfw配置，第一个参数为配置的选项，以GLFW_为前缀，第二个参数为配置的z值
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //opengl版本设置
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //opengl版本设置
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE); //使用opengl core profile配置
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
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
    
    //GLAD初始化 GLAD加载OpenGL函数的指针
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Filed to initialize GLAD" << std::endl;
        return -1;
    }
    
    //build & compile shader
    //vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);//创建shader object 通过ID索引
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//将顶点着色器源码附加到vertexShader object上
    glCompileShader(vertexShader);//编译顶点着色器，着色器需要实时在线编译
    //检查vertex shader是否编译成功
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    //fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);//编译片段着色器，着色器需要实时在线编译
    //检查fragment shader是否编译成功
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    //Link shaders
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();//创建shader program object 并进行id引用
    glAttachShader(shaderProgram, vertexShader); //将顶点着色器附加到shader program object
    glAttachShader(shaderProgram, fragmentShader); //将片段着色器附加到shader program object
    glLinkProgram(shaderProgram); //链接shader program object
    //检查shaderProgram是否link成功
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    //删除shader object (一旦shader program object链接成功，删除shader program object附加的着色器，后面不在需要了)
    glDeleteShader(vertexShader);//删除顶点着色器
    glDeleteShader(fragmentShader); //删除片段着色器
  
    //vertex data
    float vertices[]={
        //first triangle
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    
    //vertex buffer
    unsigned int VBO;//opengl object:vertex buffer object 将顶点数据一次性从gpu发送至图卡内存
    unsigned int VAO; //opengl object:vertex array object 将顶点属性设置、顶点属性状态及使用哪个VBO画储存起来
    //unsigned int EBO; //opengl object:element buffer object 将顶点索引储存起来 避免相同顶点在不同三角形中的重复绘制
    glGenVertexArrays(1, &VAO);//生成VAO
    glGenBuffers(1, &VBO);//生成VBO
    
    //first tirangle
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0); //顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));//顶点颜色
    glEnableVertexAttribArray(1);

    
    //render loop 一直不停的绘制直至结束
    while(!glfwWindowShouldClose(window)) //每一次循环开始时都检查是否需要关闭窗口
    {
        //按键输入监测
        processInput(window);
        
        //绘制指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //确定用什么颜色来清理屏幕
        glClear(GL_COLOR_BUFFER_BIT); //确定清理哪一个缓冲区，这里是用上面的颜色来清理颜色缓冲区
        
        //画三角形
        glUseProgram(shaderProgram); //激活shaderProgram，应用shaderProgram附加的着色器进行绘制
        glBindVertexArray(VAO); //绑定VAO
        
        //颜色随时间变 update shader uniform
        double timeValue = glfwGetTime();
        float greenValue = static_cast<float>(sin(timeValue)/2.0) + 0.5f;//0-1范围
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f,1.0f);
        
        glDrawArrays(GL_TRIANGLES, 0, 3); //绘制
        
        //事件响应并交换前后缓冲区
        glfwSwapBuffers(window);//交换前后缓冲区，在屏幕上显示图像
        glfwPollEvents(); //检查是否有事件触发 更新window状态 响应回调函数中的自定义功能
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    
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
}
