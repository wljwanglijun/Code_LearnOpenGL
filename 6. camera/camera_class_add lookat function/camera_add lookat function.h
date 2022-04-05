//
// Created by Lijun Wang on 2022/4/4.
//

//避免源文件包含同一个头文件2次
#ifndef OPENGL_CLION_CAMERA_H
#define OPENGL_CLION_CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

//相机运动方式枚举
enum Camera_Movement{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

//相机参数默认值
const float YAW = -90.0f; //初始指向-z轴
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
    //相机属性
    glm::vec3 Position; //位置
    glm::vec3 Front; //前 gaze
    glm::vec3 Up; //上
    glm::vec3 Right; //右
    glm::vec3 WorldUp; //世界 上
    //欧拉角
    float Yaw; //偏航
    float Pitch; //俯仰
    //相机参数
    float MovementSpeed; //运动速度
    float MouseSensitivity; //鼠标灵敏度
    float Zoom; //缩放

    //相机类构造函数1 基于向量
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f), float yaw = YAW, float pitch = PITCH)
    : Front(glm::vec3(0.0f,0.0f, -1.0f)),
    MovementSpeed(SPEED),
    MouseSensitivity(SENSITIVITY),
    Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    //相机构造函数2 基于标量
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    :Front(glm::vec3(0.0f,0.0f, -1.0f)),
    MovementSpeed(SPEED),
    MouseSensitivity(SENSITIVITY),
    Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    //依据相机位置及方向，通过lookat函数获取视角变换矩阵 view matrix
    glm::mat4 GetViewMatrix()
    {
        return calculate_lookAt_matrix(Position, Position+Front, Up);
    }

    //处理键盘输入
    void ProcessKeyboardInput(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if(direction == FORWARD)
        {
            Position = Position + Front * velocity;
        }
        if(direction == BACKWARD)
        {
            Position = Position - Front * velocity;
        }
        if(direction == LEFT)
        {
            Position = Position - Right * velocity;
        }
        if(direction == RIGHT)
        {
            Position = Position + Right * velocity;
        }
        //Position.y = 0.0f; //使相机永远在xz平面上移动
    }

    //处理鼠标输入
    void ProcessMouseInput(float xOffset, float yOffset, bool constrainPitch = true)
    {
        xOffset = xOffset * MouseSensitivity;
        yOffset = yOffset * MouseSensitivity;

        Yaw = Yaw + xOffset;
        Pitch = Pitch - yOffset;

        if(constrainPitch)
        {
            if(Pitch < -89.0f)
            {
                Pitch = -89.0f;
            }
            if(Pitch > 89.0f)
            {
                Pitch = 89.0f;
            }
        }

        updateCameraVectors(); //改变pitch&yaw后需要及时更新camera vector
    }

    //处理鼠标滚轮输入
    void ProcessScrollInput(float yOffset)
    {
        Zoom = Zoom + yOffset;
        if(Zoom > 45.0f)
        {
            Zoom = 45.0f;
        }
        if(Zoom < 1.0f)
        {
            Zoom = 1.0f;
        }
    }


private:
    void updateCameraVectors() //计算相机Front（-z）&Up（+y）&Right（+x）向量
    {
        //计算gaze方向
        glm::vec3 front;
        front.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
        front.y = sin(glm::radians(Pitch));
        front.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
        Front = glm::normalize(front);
        //计算Right方向
        Right = glm::normalize(glm::cross(Front, WorldUp));
        //计算up方向
        Up = glm::normalize(glm::cross(Right, Front));
    }

    glm::mat4 calculate_lookAt_matrix(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
    {
        //1. 位置已知：position
        //2. 计算相机+z direction
        glm::vec3 zAxis = glm::normalize(position - target);
        //3. 计算相机+x right
        glm::vec3 xAxis = glm::normalize(glm::cross(glm::normalize(worldUp), zAxis));
        //4. 计算相机+y up
        glm::vec3 yAxis = glm::normalize(glm::cross(zAxis, xAxis));

        //构建变换矩阵
        glm::mat4 translation = glm::mat4(1.0f);
        translation[3][0] = -position.x;
        translation[3][1] = -position.y;
        translation[3][2] = -position.z;
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation[0][0] = xAxis.x;
        rotation[1][0] = xAxis.y;
        rotation[2][0] = xAxis.z;
        rotation[0][1] = yAxis.x;
        rotation[1][1] = yAxis.y;
        rotation[2][1] = yAxis.z;
        rotation[0][2] = zAxis.x;
        rotation[1][2] = zAxis.y;
        rotation[2][2] = zAxis.z;

        return rotation * translation; //参考glm::lookup函数，先平移后旋转
    }

};





#endif //OPENGL_CLION_CAMERA_H
