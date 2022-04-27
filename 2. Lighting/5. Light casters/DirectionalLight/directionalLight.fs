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

struct Light {
    //vec3 position; //灯光位置属性
    vec3 direction; //directional light 平行光方向
    vec3 ambient; //灯光环境光颜色属性 一般与diffuse相同，或者RGB同比率减小
    vec3 diffuse; //灯光漫反射颜色属性 本身的颜色
    vec3 specular; //灯光镜面反射光颜色属性 一般为白光 1 1 1
};


//uniform变量允许在应用程序中修改进行修改
uniform Material material; //材质属性
uniform Light light; //灯光属性
uniform vec3 viewPos; //视角（相机）位置

void main()
{
        //物体表面显示的最终颜色是灯光和材质共同作用的结果
        // ambient
        vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));

        // diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(-light.direction);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));


        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * (vec3(texture(material.specular, TexCoords)));

        //emission
        //vec3 emission = vec3(texture(material.emission, TexCoords));

        //ambient diffuse specular三者相加，得到最后的颜色
        //vec3 result = ambient + diffuse + specular + emission;

        vec3 result = ambient + diffuse + specular;

        FragColor = vec4(result, 1.0);
}