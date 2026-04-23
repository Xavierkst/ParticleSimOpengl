#version 450 core

out vec4 FragColor;

in vec2 texCoords;

struct Material {
	sampler2D texture_diffuse1;
};

uniform Material mat;

void main() {
	FragColor = texture(mat.texture_diffuse1, texCoords);
}