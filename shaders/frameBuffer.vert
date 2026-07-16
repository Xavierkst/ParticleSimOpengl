#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceModel;

out vec2 texCoords;

layout (std140) uniform Matrices {
	mat4 view;
	mat4 proj;
};

void main() {
	texCoords = aTexCoords;
	gl_Position = proj*view*instanceModel*vec4(aPos, 1.0f);
}