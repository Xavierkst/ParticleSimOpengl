#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	vec4 lightSpaceFragPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightTransform;

void main() {
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.Normal = inverse(transpose(mat3(model))) * aNormal;
	vs_out.TexCoord = aTexCoord;
	vs_out.lightSpaceFragPos = lightTransform * model * vec4(aPos, 1.0);
}
