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

//函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//顶点着色器 \0表示字符串结束
const char *vertexShaderSource = "#version 330 core\n" //GLSL版本 对应opengl3.3 core profile
    "layout (location = 0) in vec3 aPos;\n" //顶点位置数据输入
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n" //将顶点给gl_Position，顶点着色器将gl_Position输出至下一环节
    "}\0";

//片段着色器
const char *fragmentShaderSource_1 = "#version 330 core\n" //GLSL版本 对应opengl3.3 core profile
    "out vec4 FragColor;\n" //像素颜色输出
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n" //像素颜色赋值 分别对应RGBA 最后为透明度 1为不透明
    "}\n\0";

const char *fragmentShaderSource_2 = "#version 330 core\n" //GLSL版本 对应opengl3.3 core profile
    "out vec4 FragColor;\n" //像素颜色输出
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n" //像素颜色赋值 分别对应RGBA 最后为透明度 1为不透明
    "}\n\0";

int main(int argc, const char * argv[]) {
    // insert code here...
    //std::cout << "Hello, World!\n";
    
    glfwInit(); //glfw库初始化，在使用glfw函数前，必须先初始化
    //以下均为glfw配置，第一个参数为配置的选项，以GLFW_为前缀，第二个参数为配置的z值
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //opengl版本设置
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //opengl版本设置
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE); //使用opengl core profile配置
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    //创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Filed to create GLFW window" << std::endl; //有了std::，就不需要在最前面写using namespace std
        glfwTerminate();
        return -1;
    }
    //使创建的window上下文为当前线程的主上下文
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
    
    //fragment shader 1
    unsigned int fragmentShader_1;
    fragmentShader_1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader_1, 1, &fragmentShaderSource_1, NULL);
    glCompileShader(fragmentShader_1);//编译片段着色器，着色器需要实时在线编译
    //检查fragment shader是否编译成功
    glGetShaderiv(fragmentShader_1, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader_1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    //fragment shader 2
    unsigned int fragmentShader_2;
    fragmentShader_2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader_2, 1, &fragmentShaderSource_2, NULL);
    glCompileShader(fragmentShader_2);//编译片段着色器，着色器需要实时在线编译
    //检查fragment shader是否编译成功
    glGetShaderiv(fragmentShader_2, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader_2, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    //Link shaders
    unsigned int shaderProgram_1;
    shaderProgram_1 = glCreateProgram();//创建shader program object 并进行id引用
    glAttachShader(shaderProgram_1, vertexShader); //将顶点着色器附加到shader program object
    glAttachShader(shaderProgram_1, fragmentShader_1); //将片段着色器附加到shader program object
    glLinkProgram(shaderProgram_1); //链接shader program object
    //检查shaderProgram是否link成功
    glGetProgramiv(shaderProgram_1, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram_1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    unsigned int shaderProgram_2;
    shaderProgram_2 = glCreateProgram();//创建shader program object 并进行id引用
    glAttachShader(shaderProgram_2, vertexShader); //将顶点着色器附加到shader program object
    glAttachShader(shaderProgram_2, fragmentShader_2); //将片段着色器附加到shader program object
    glLinkProgram(shaderProgram_2); //链接shader program object
    //检查shaderProgram是否link成功
    glGetProgramiv(shaderProgram_2, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram_2, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    
    
    //删除shader object (一旦shader program object链接成功，删除shader program object附加的着色器，后面不在需要了)
    glDeleteShader(vertexShader);//删除顶点着色器
    glDeleteShader(fragmentShader_1); //删除片段着色器
    glDeleteShader(fragmentShader_2); //删除片段着色器
    
    
    
    
    
    //vertex data
    float firstTriangle[]={
        //first triangle
        -0.9f, -0.5f, 0.0f,
        0.0f, -0.5f, 0.0f,
        -0.45f, 0.5f, 0.0f
    };
    float secondTriangle[]={
        //second triangle
        0.0f, -0.5f, 0.0f,
        0.9f, -0.5f, 0.0f,
        0.45f, 0.5f, 0.0f
    };
    
    //index data
//    unsigned int indices[] = {
//        0, 1, 3,
//        1, 2, 3
//    };
    
    
    //vertex buffer
    unsigned int VBOs[2];//opengl object:vertex buffer object 将顶点数据一次性从gpu发送至图卡内存
    unsigned int VAOs[2]; //opengl object:vertex array object 将顶点属性设置、顶点属性状态及使用哪个VBO画储存起来
    //unsigned int EBO; //opengl object:element buffer object 将顶点索引储存起来 避免相同顶点在不同三角形中的重复绘制
    glGenVertexArrays(2, VAOs);//生成VAO
    glGenBuffers(2, VBOs);//生成VBO
    //glGenBuffers(1, &EBO);//生成EBO
    
    //first tirangle
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER,VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    //second triangle
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER,VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    
//    //1. 绑定VAO
//    glBindVertexArray(VAO);//绑定VAO
//
//    //2. 绑定VBO，并将顶点数据传输给图卡
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);//VBO buffer绑定：一旦绑定，后续所有的调用和配置都在绑定的VBO上
//    //将定义的顶点数据复制到当前绑定的buffer（已经将顶点数据发送到了显存上，GPU可以快速取用）
//    //参数1buffer类型、参数2想要复制进buffer中的数据大小（以字节为单位）、参数3想要发送的数据、参数4图卡如何管理这些数据
//    //参数4 3种参数：GL_STREAM_DRAW设置1次最多使用几次、GL_STATIC_DRAW设置1次使用多次、GL_DYNAMIC_DRAW数据变化使用多次
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    //3. 绑定EBO
////    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
////    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//    //4. 配置顶点属性，确定顶点数据读入方法（结合顶点数据的组成+顶点着色器的顶点属性设置）
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);//位置为0的顶点属性生效
//
//    //VBO解绑
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//    //VAO解绑
//    glBindVertexArray(0);
    
    //绘制线形三角形
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    //render loop 一直不停的绘制直至结束
    while(!glfwWindowShouldClose(window)) //每一次循环开始时都检查是否需要关闭窗口
    {
        //按键输入监测
        processInput(window);
        
        //绘制指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //确定用什么颜色来清理屏幕
        glClear(GL_COLOR_BUFFER_BIT); //确定清理哪一个缓冲区，这里是用上面的颜色来清理颜色缓冲区
        
        //画三角形
        glUseProgram(shaderProgram_1); //激活shaderProgram，应用shaderProgram附加的着色器进行绘制
        glBindVertexArray(VAOs[0]); //绑定VBO，间接定义顶点VBO及顶点属性指针
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 3); //绘制 第一个参数是基本体类型，第二个参数是从顶点数组的哪里开始q读取顶点数据，第三个参数是想要绘制的顶点数量
        
        glUseProgram(shaderProgram_2);
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3); //second triangle
        
        
        //事件响应并交换前后缓冲区
        glfwSwapBuffers(window);//交换前后缓冲区，在屏幕上显示图像
        glfwPollEvents(); //检查是否有事件触发 更新window状态 响应回调函数中的自定义功能
    }
    
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    //glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram_1);
    glDeleteProgram(shaderProgram_2);

    
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
