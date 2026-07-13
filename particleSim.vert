#version 450 core

layout (location = 0) in vec4 aPos;

uniform mat4 WVPMatrix = mat4(1.0f);

void main() {
	gl_Position = WVPMatrix * aPos;
}