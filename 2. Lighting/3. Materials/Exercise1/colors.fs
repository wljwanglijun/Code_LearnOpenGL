#version 330 core

out vec4 FragColor;

in vec3 Position;
in vec3 Normal;

struct Material {
    vec3 ambient; //对于Light.ambient的反射率
    vec3 diffuse; //对于Light.diffuse的反射率
    vec3 specular; //对于Light.pecular的反射率
    float shininess; //镜面反射形成的光点的聚焦程度，越大越聚焦，越小越涣散
};

struct Light {
    vec3 position; //灯光位置属性
    vec3 ambient; //灯光环境光颜色属性 一般与diffuse相同，或者RGB同比率减小
    vec3 diffuse; //灯光漫反射颜色属性 本身的颜色
    vec3 specular; //灯光镜面反射光颜色属性 一般为白光 1 1 1
};

uniform Material material;
uniform Light light;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
        //物体表面显示的最终颜色是灯光和材质共同作用的结果
        // ambient
        //ambient是灯光的环境光作用，不管该灯光位置在哪里，在渲染场景中对每个物体都有均匀的光照影响，与灯光位置、物体位置、片段法线均无关
        //ambient是灯光的ambient与材质的ambient相乘的结果，可以理解为材质的ambient对灯光的RGB三个分量的反射率，某个通道
        //越接近与1，对灯光在该通道下的反射率越大，最终在该通道下物体表面的颜色更接近于灯光在该通道下的颜色
        vec3 ambient = light.ambient * material.ambient;

        // diffuse
        //首先依据当前片段的法线与入射光线夹角，夹角越小diff越大，点乘，diff表示当前片段对灯光diffuse分量的反射率的比例
        //物体diffuse的分量为反射率，diff与material.diffuse相乘，diff的范围为0-1，如果diff为0，按照material.diffuse进行灯光反射，
        //否则需要乘0。几，那就相当于弱化对灯光的反射率，体现在物体上，物体面向光的地方看起来亮，背光的地方看起来暗
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - Position);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * (diff * material.diffuse);


        // specular
        //首先依据当前的反射光线与视线的夹角，确定spec，同样是夹角越小，spec越大，spec的范围同样是0-1
        //material.specular表示对light.specular的反射率，具体反射程度由spec决定
        //当当前片段的夹角越小，spec趋向于1，对light.specular的反射完全按照material.specular计算，否则会乘上spec，进行弱化
        vec3 viewDir = normalize(viewPos - Position);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * (spec * material.specular);

        //ambient diffuse specular三者相加，得到最后的颜色
        vec3 result = ambient + diffuse + specular;

        FragColor = vec4(result, 1.0);
}