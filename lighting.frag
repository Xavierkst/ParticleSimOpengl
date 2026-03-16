#version 450 core
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};
struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 textureCoords;
in vec3 FragPos;
in vec3 Normal;

uniform Material mat;
uniform Light light;
uniform vec3 viewPos;
uniform float time;

uniform mat4 proj;
uniform mat4 view;

void main()
{
	vec3 ambient = light.ambient * vec3(texture(mat.diffuse, textureCoords));
	vec3 lightPos = vec3(view * vec4(light.position, 1.0f));
	vec3 lightDir = normalize(FragPos - lightPos);

	// Calculate dot prod of surface normal and reverse of lightDir
	vec3 norm = normalize(Normal);
	float diff = max(dot(-lightDir, norm), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(mat.diffuse, textureCoords));

	vec3 viewDir = normalize(-FragPos);
	// reflect fn expect light dir to point from light src to frag position
	vec3 reflectDir = reflect(lightDir, norm);
	float spec = pow(max(0.0, dot(viewDir, reflectDir)), mat.shininess);
	vec3 specMapComponent = vec3(texture(mat.specular, textureCoords));
	vec3 specular = light.specular * spec * specMapComponent;
	vec3 emissionColor = texture(mat.emission, textureCoords + vec2(0.0f, time)).rgb;
	vec3 emissionMask = step(vec3(1.0f), vec3(1.0f) - specMapComponent);
	emissionColor *= emissionMask;

	vec3 res = (diffuse + ambient + specular + emissionColor); 
	
	FragColor = vec4(res, 1.0f);
}
