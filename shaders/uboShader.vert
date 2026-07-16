#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT { 
	out vec4 FragPos;
	out vec3 Normal;
} vs_out;

uniform mat4 model;
layout (std140) uniform Matrices {
	mat4 view;
	mat4 proj;
};

void main() {
	vec4 pos = proj * view * model * vec4(aPos, 1.0); 
	gl_Position = pos;
	vs_out.FragPos = pos;
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
}