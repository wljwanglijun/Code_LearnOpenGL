#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0)); //经过模型变换在世界坐标系下的顶点位置
    Normal = mat3(transpose(inverse(model))) * aNormal; //经过模型变换在世界坐标系下的法线向量（先取逆，再转置）
    TexCoords = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);//mvp变换：模型变换 视角变换 投影变化
}