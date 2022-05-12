#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    sampler2D diffuse; //此处材质无ambient属性，是因为通常情况下ambient为diffuse RGB三通道等比例减小的结果，相当于有了diffuse，就有了ambient
    sampler2D specular; //对于Light.pecular的反射率
    sampler2D emission; //自发光贴图
    float shininess; //镜面反射形成的光点的聚焦程度，越大越聚焦，越小越涣散
};

//flash light spot light 聚光灯
struct Light {
    vec3 position; //灯光位置属性
    vec3 direction; //灯光方向
    float cutOff; //灯光照射范围 内圆锥
    float outerCutOff; //灯光照射范围 外圆锥 实现外缘亮度渐变


    vec3 ambient; //灯光环境光颜色属性 一般与diffuse相同，或者RGB同比率减小
    vec3 diffuse; //灯光漫反射颜色属性 本身的颜色
    vec3 specular; //灯光镜面反射光颜色属性 一般为白光 1 1 1

    float constant; //灯光强度衰减 常数项
    float linear; //灯光强度衰减 一次项/线性项
    float quadratic; //灯光强度衰减 二次项
};


//uniform变量允许在应用程序中修改进行修改
uniform Material material; //材质属性
uniform Light light; //灯光属性
uniform vec3 viewPos; //视角（相机）位置

void main()
{
    //物体表面显示的最终颜色是灯光和材质共同作用的结果
    //首先计算当前片段是否在聚光等的照射范围内
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff)/epsilon, 0.0, 1.0);

    //该片段在灯光照射范围内
    //点光源衰减值计算
    float distance = length(light.position - FragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));

    // diffuse
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * (vec3(texture(material.specular, TexCoords)));

    //灯光渐变
    diffuse = diffuse * intensity;
    specular = specular * intensity;

    //衰减应用
    ambient = ambient * attenuation;
    diffuse = diffuse * attenuation;
    specular = specular * attenuation;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}