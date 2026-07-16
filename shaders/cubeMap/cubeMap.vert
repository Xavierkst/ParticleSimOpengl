#version 450 core

layout (location = 0) in vec3 aPos;

out vec3 texCoord;

layout (std140) uniform Matrices {
	mat4 view;
	mat4 proj;
};

uniform mat4 model;

void main() {
	vec4 pos = proj*view*model*vec4(aPos, 1.0f);
	gl_Position = pos.xyww;
	texCoord = aPos;
}
