#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;

out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;

void main()
{
    FragPos = vec3(view * model * vec4(aPos, 1.0));//获取视图坐标系下的顶点位置，用于片段着色器计算光线向量
    Normal = mat3(transpose(inverse(view * model))) * aNormal; //获取视图坐标系下的法向量，用于片段着色器计算光照
    LightPos = vec3(view * vec4(lightPos, 1.0)); //LightPos本身是在世界坐标系下
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}