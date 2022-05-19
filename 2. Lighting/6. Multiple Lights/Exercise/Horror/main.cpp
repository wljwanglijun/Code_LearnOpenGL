#include <glad/glad.h> //glad头文件添加成功 写在前面 不然会报错
#include <GLFW/glfw3.h> //GLFW头文件添加成功
#include "stb_image.h"
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
unsigned int loadTexture(const char *path);

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
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

//纹理贴图路径
//diffuse map
const char diffuseMapPath[] = "../Texture/container_diffuseMap.png";
const char *path_diffuseMap = diffuseMapPath;
//specular map
const char specularMapPath[] = "../Texture/container_specularMap.png";
const char *path_specularMap = specularMapPath;
//emission map
const char emissionMapPath[] = "../Texture/container_emissionMap.jpg";
const char *path_emissionMap = emissionMapPath;


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
    Shader lightingShader("../Shader/multipleLights.vs", "../Shader/multipleLights.fs"); //返回上一级菜单../
    Shader lightCubeShader("../Shader/light_cube.vs", "../Shader/light_cube.fs");

    //vertex data
    float vertices[]={
            //顶点坐标+法向量+纹理坐标 cube
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f
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
    //顶点位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //顶点法线
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //顶点纹理坐标
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    //load diffuse map
    unsigned int diffuseMap = loadTexture(path_diffuseMap);
    //load specular map
    unsigned int specularMap = loadTexture(path_specularMap);
    //load emission map
    unsigned int emissionMap = loadTexture(path_emissionMap);
    //设置片段着色器diffuseMap变量值
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0); //只有一个纹理单元的时候，该纹理单元的索引默认为0
    lightingShader.setInt("material.specular", 1);
    lightingShader.setInt("material.emission", 2);

    //10个立方体在空间中的位置
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

    //4个点光源位置
    glm::vec3 pointLightPositions[] = {
            glm::vec3(0.7f,  0.2f,  2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3(0.0f,  0.0f, -3.0f),
    };
    glm::vec3 pointLightColors[] = {
            glm::vec3(0.1f, 0.1f, 0.1f),
            glm::vec3(0.1f, 0.1f, 0.1f),
            glm::vec3(0.1f, 0.1f, 0.1f),
            glm::vec3(0.3f, 0.1f, 0.1f)
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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //确定用什么颜色来清理屏幕
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //确定清理哪一个缓冲区，这里是用上面的颜色来清理颜色缓冲区

        //设置立方体颜色绘制着色器（本身颜色*灯光颜色）
        lightingShader.use();
        //设置视点位置输入 计算specular
        lightingShader.setVec3("viewPos", camera.Position);
        //设置立方体材质属性
        lightingShader.setFloat("material.shininess", 64.0f);
        //灯光属性设置
// Directional light
        glUniform3f(glGetUniformLocation(lightingShader.ID, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "dirLight.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "dirLight.diffuse"), 0.05f, 0.05f, 0.05);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "dirLight.specular"), 0.2f, 0.2f, 0.2f);
// Point light 1
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[0].ambient"), pointLightColors[0].x * 0.1,  pointLightColors[0].y * 0.1,  pointLightColors[0].z * 0.1);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[0].diffuse"), pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[0].specular"), pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "pointLights[0].linear"), 0.14);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "pointLights[0].quadratic"), 0.07);
// Point light 2
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[1].ambient"), pointLightColors[1].x * 0.1,  pointLightColors[1].y * 0.1,  pointLightColors[1].z * 0.1);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[1].diffuse"), pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[1].specular"), pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "pointLights[1].linear"), 0.14);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "pointLights[1].quadratic"), 0.07);
// Point light 3
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[2].ambient"), pointLightColors[2].x * 0.1,  pointLightColors[2].y * 0.1,  pointLightColors[2].z * 0.1);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[2].diffuse"), pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[2].specular") ,pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "pointLights[2].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "pointLights[2].linear"), 0.22);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "pointLights[2].quadratic"), 0.20);
// Point light 4
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[3].ambient"), pointLightColors[3].x * 0.1,  pointLightColors[3].y * 0.1,  pointLightColors[3].z * 0.1);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[3].diffuse"), pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "pointLights[3].specular"), pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "pointLights[3].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "pointLights[3].linear"), 0.14);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "pointLights[3].quadratic"), 0.07);
// SpotLight
        glUniform3f(glGetUniformLocation(lightingShader.ID, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "spotLight.linear"), 0.09);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "spotLight.quadratic"), 0.032);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "spotLight.cutOff"), glm::cos(glm::radians(10.0f)));
        glUniform1f(glGetUniformLocation(lightingShader.ID, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

//        //平行光
//        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
//        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
//        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
//        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
//        //点光源
//        //点光源1
//        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
//        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
//        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
//        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
//        lightingShader.setFloat("pointLights[0].constant", 1.0f);
//        lightingShader.setFloat("pointLights[0].linear", 0.09f);
//        lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
//        //点光源2
//        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
//        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
//        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
//        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
//        lightingShader.setFloat("pointLights[1].constant", 1.0f);
//        lightingShader.setFloat("pointLights[1].linear", 0.09f);
//        lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
//        //点光源3
//        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
//        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
//        lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
//        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
//        lightingShader.setFloat("pointLights[2].constant", 1.0f);
//        lightingShader.setFloat("pointLights[2].linear", 0.09f);
//        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
//        //点光源4
//        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
//        lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
//        lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
//        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
//        lightingShader.setFloat("pointLights[3].constant", 1.0f);
//        lightingShader.setFloat("pointLights[3].linear", 0.09f);
//        lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
//        //聚光灯
//        lightingShader.setVec3("spotLight.position", camera.Position);
//        lightingShader.setVec3("spotLight.direction", camera.Front);
//        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
//        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
//        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
//        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
//        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
//        lightingShader.setFloat("spotLight.constant", 1.0f);
//        lightingShader.setFloat("spotLight.linear", 0.09f);
//        lightingShader.setFloat("spotLight.quadratic", 0.032f);

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

        //激活纹理单元
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emissionMap);

        //绑定立方体VAO
        glBindVertexArray(cubeVAO);

        //绘制10个立方体
        for(unsigned int i = 0; i < 10; ++i)
        {
            //模型变换
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMat4("model", model);
            //立方体绘制
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //设置灯光立方体的mvp变换
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glBindVertexArray(lightCubeVAO); //绑定VAO
        for(unsigned int i = 0; i < 4; ++i)
        {
            glm::mat4 model_lightCube = glm::mat4(1.0f);
            model_lightCube = glm::translate(model_lightCube, pointLightPositions[i]);
            model_lightCube = glm::scale(model_lightCube, glm::vec3(0.2f));
            lightCubeShader.setMat4("model", model_lightCube);
            lightCubeShader.setVec3("color", pointLightColors[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //事件响应并交换前后缓冲区
        glfwSwapBuffers(window);//交换前后缓冲区，在屏幕上显示图像
        glfwPollEvents(); //检查是否有事件触发 更新window状态 响应回调函数中的自定义功能
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
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

unsigned int loadTexture(const char *path)
{
    //定义纹理对象
    unsigned int textureID;
    glGenTextures(1, &textureID);

    //texture图像导入
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

    if(data)
    {
        //确定texture的颜色通道数
        GLenum format;
        if(nrComponents == 1)
        {
            format = GL_RED;
        }
        else if(nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if(nrComponents == 4)
        {
            format = GL_RGBA;
        }

        //纹理对象绑定
        glBindTexture(GL_TEXTURE_2D, textureID);
        //纹理图片绑定
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        //生成mipmap贴图
        //欲解决问题：当物体贴图分辨率较高，且距离视点位置较远时，此时该物体占据的像素单元较少，一个像素单元会对应多个顶点，每个
        //顶点对应的颜色不同，opengl无法确定该使用哪一个颜色
        //目的：物体距离视点位置的变化，表面贴图分辨率的自适应调整
        //实现：一组基于原有贴图生成的分辨率逐渐减小的贴图序列，根据距离变化，使用最近邻/线性选择对应的贴图，并进行纹理采样
        //这一系列图片之间分辨率4倍4倍的下降，实际操作过程是上一级图片上下左右四个像素合并为下一级图片的4个像素
        glGenerateMipmap(GL_TEXTURE_2D);

        //设置纹理环绕
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //设置纹理过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //图片内存释放
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}