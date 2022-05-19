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

//directional light 平行光
struct DirLight {
    vec3 direction; //灯光方向

    vec3 ambient; //灯光环境光颜色属性 一般与diffuse相同，或者RGB同比率减小
    vec3 diffuse; //灯光漫反射颜色属性 本身的颜色
    vec3 specular; //灯光镜面反射光颜色属性 一般为白光 1 1 1
};

//point light 点光源
struct PointLight {
    vec3 position; //灯光位置

    float constant; //灯光衰减常数项
    float linear; //灯光衰减一次项系数
    float quadratic; //灯光衰减二次项系数

    vec3 ambient; //环境光
    vec3 diffuse; //漫反射
    vec3 specular; //镜面反射
};

//spot light 聚光灯
struct SpotLight {
    vec3 position; //灯光位置
    vec3 direction; //灯光方向
    float cutOff; //灯光照射范围 内圆锥
    float outerCutOff; //灯光照射范围 外圆锥

    float constant; //灯光衰减常数项
    float linear; //灯光衰减一次项系数
    float quadratic; //灯光衰减二次项系数

    vec3 ambient; //环境光
    vec3 diffuse; //漫反射
    vec3 specular; //镜面反射
};


//点光源数量 宏定义
#define NR_POINT_LIGHTS 4

//uniform变量允许在应用程序中修改进行修改
uniform Material material; //材质属性
uniform DirLight dirLight; //平行光变量
uniform PointLight pointLights[NR_POINT_LIGHTS]; //点光源变量
uniform SpotLight spotLight; //聚光灯变量
uniform vec3 viewPos; //视角（相机）位置

//平行光效果计算函数
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
//点光源效果计算函数
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//聚光灯效果计算函数
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    //计算法线
    vec3 norm = normalize(Normal);
    //计算视角方向
    vec3 viewDir = normalize(viewPos - FragPos);

    //平行光
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    //点光源
    for(int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        result = result + CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    //聚光灯
    result = result + CalcSpotLight(spotLight, norm, FragPos, viewDir);

    //最终颜色输出
    FragColor = vec4(result, 1.0);
}

//平行光渲染颜色计算
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    //计算灯光入射方向（由物体指向光源）
    vec3 lightDir = normalize(-light.direction);
    //diffuse shading 光线方向与物体表面法线的夹角，夹角越小，diffuse效果越强
    float diff = max(dot(lightDir, normal), 0.0);
    //specular shading 入射光线的反射光线与视线（由物体表面指向眼睛）的夹角，夹角越小，specular效果越强
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),material.shininess);
    //combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    return (ambient + diffuse + specular);
}

//点光源渲染颜色计算
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    //计算灯光入射方向（由物体指向光源）
    vec3 lightDir = normalize(light.position-fragPos);
    //diffuse shading
    float diff = max(dot(lightDir, normal), 0.0);
    //specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //attenuation 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //各分量结果
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient = ambient * attenuation;
    diffuse = diffuse * attenuation;
    specular = specular * attenuation;

    return (ambient + diffuse + specular);
}

//聚光灯渲染颜色计算
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    //计算灯光入射方向（由物体指向光源）
    vec3 lightDir = normalize(light.position-fragPos);
    //diffuse shading
    float diff = max(dot(lightDir, normal), 0.0);
    //specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //attenuation 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * (distance * distance));
    //聚光灯照射强度
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff)/epsilon, 0.0, 1.0);

    //各分量结果
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient = ambient * attenuation;
    diffuse = diffuse * attenuation * intensity;
    specular = specular * attenuation * intensity;

    return (ambient + diffuse + specular);
}