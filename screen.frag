#version 450 core
out vec4 FragColor;

in vec2 texCoords;
uniform sampler2D quadTex;

void main() { 
	FragColor = texture(quadTex, texCoords);
}