#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
// layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out mat3 normMatrix;
out vec3 LightPos;

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
}
