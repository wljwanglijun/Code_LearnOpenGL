#version 330 core
in vec3 ourColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    //为使第二个贴图在模型上显示时左右反转，只需要在贴该贴图时改变模型的纹理坐标，将对应的纹理坐标左右反转
    FragColor = mix(texture(texture1, texCoord), texture(texture2, vec2(1-texCoord.x, texCoord.y)), 0.2f);
}