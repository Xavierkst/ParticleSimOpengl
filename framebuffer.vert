#version 450 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 offset;

out vec3 color;


void main() {
	vec2 position = aPos * (gl_InstanceID / 100.0);
	gl_Position = vec4(position + offset, 0.0f, 1.0f);
	color = aColor;
}