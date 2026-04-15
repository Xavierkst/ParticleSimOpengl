#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
	out vec4 FragPos;
	out vec2 texCoords;
} vs_out;

uniform mat4 model;
// 'Matrices' uniform block
layout (std140) uniform Matrices {
	mat4 view;
	mat4 proj;
};

void main() {
	gl_Position = proj*view*model*vec4(aPos, 1.0f);
	vs_out.FragPos = model*vec4(aPos, 1.0f);
	vs_out.texCoords = aTexCoords;
}