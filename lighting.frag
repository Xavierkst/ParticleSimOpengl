#version 450 core
out vec4 FragColor;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;

uniform Material mat;
uniform Light light;
uniform vec3 viewPos;

uniform mat4 proj;
uniform mat4 view;

void main()
{
	float ambientScale = 0.10f;
	vec3 ambient = light.ambient * mat.ambient;
	vec3 lightPos = vec3(view * vec4(light.position, 1.0f));
	vec3 lightDir = normalize(FragPos - lightPos);

	// Calculate dot prod of surface normal and reverse of lightDir
	vec3 norm = normalize(Normal);
	float diff = max(dot(-lightDir, norm), 0.0);
	vec3 diffuse = light.diffuse * (diff * mat.diffuse);

	vec3 viewDir = normalize(-FragPos);
	// reflect fn expect light dir to point from light src to frag position
	vec3 reflectDir = reflect(lightDir, norm);
	float spec = pow(max(0.0, dot(viewDir, reflectDir)), mat.shininess);
	vec3 specular = light.specular * (spec * mat.specular);
	
	vec3 res = (diffuse + ambient + specular);
	FragColor = vec4(res, 1.0f);
}
