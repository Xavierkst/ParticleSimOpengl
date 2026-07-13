#version 450 core

uniform vec4 Color = vec4(1.0);
layout (location = 0) out vec4 FragColor;
// in vec2 TexCoords;

void main() {
	FragColor = Color;
}