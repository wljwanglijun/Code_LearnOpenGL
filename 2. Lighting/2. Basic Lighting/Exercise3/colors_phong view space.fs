#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    //ambient light
    float ambientStrength = 0.1; //环境光常量
    vec3 ambient = ambientStrength * lightColor; //环境光颜色 = 环境光常量 * 灯光颜色

    //diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //specular light
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-FragPos); //viewDir = viewPos - FragPos,但在视图坐标系下，viewPos永远为000，因此此处为-FragPos
    vec3 reflectDir = reflect(-lightDir, norm); //reflect函数第一个参数是光源位置指向片段位置的向量
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); //pow(x,y) x的y次幂
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor; //物体最终显示颜色 = 环境光颜色 * 物体颜色
    FragColor = vec4(result, 1.0);
}