#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
// layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out mat3 normMatrix;
out vec3 LightPos;
out vec4 FColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform vec3 lightPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	// Read matrix multiplications from R to L, with R being the first transform applied
	gl_Position = proj*view*model* vec4(aPos, 1.0);
	// The fragment's position in world space coords 
	FragPos = vec3(view*model * vec4(aPos, 1.0f));
	Normal = mat3(transpose(inverse(view*model))) * aNormal;
	LightPos = vec3(view*vec4(lightPos, 1.0f));

	vec4 vertPos = gl_Position;
	// calc ambient
	float ambStr = 0.1f;
	vec3 ambient = ambStr * lightColor;
	// calc diff
	// find the light dir and surface normal
	vec3 lightDir = normalize(FragPos - LightPos);
	vec3 norm = normalize(Normal);
	float diff = max(dot(-lightDir, norm), 0.0f);
	vec3 diffuse = diff * lightColor;

	// calc specular
	// reflect the inc light dir on the fragment to get reflectlight dir
	vec3 viewDir = normalize(-FragPos);
	vec3 reflectedDir = reflect(lightDir, norm);
	float specularStr = 1.0f;
	float spec = pow(max(dot(reflectedDir, viewDir), 0.0f), 32);
	vec3 specular = specularStr * spec * lightColor;
	FColor = vec4((ambient + diffuse + specular) * objectColor, 1.0f);
}
