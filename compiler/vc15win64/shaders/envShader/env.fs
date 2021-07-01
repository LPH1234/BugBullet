#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
}; 

uniform Material material;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

void main()
{ 
	// ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	/*������*/
	vec3 norm = normalize(Normal);//�ѷ�������׼��
	vec3 lightDir = normalize(light.position - FragPos); //��Դ���򣺹�Դλ�� - Ƭλ��
	float diff = max(dot(norm, lightDir), 0.0);    //��ˣ����������нǷ���Խ������������ͻ�ԽС
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	/*���淴��*/
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0),256);
	vec3 specular = light.specular * (spec * material.specular);

	/*�ϲ�*/
	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0);

   // FragColor = texture(texture_diffuse1, TexCoords);
}