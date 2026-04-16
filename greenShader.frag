#version 450 core
out vec4 FragColor;

in VS_OUT {
	in vec4 FragPos;
	in vec3 Normal;
} vs_in;

void main() {
	FragColor = vec4(.0, 1.0, .0, 1.0);
}