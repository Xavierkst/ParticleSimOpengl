#version 450 core
out vec4 FragColor;
in GS_OUT {
 	vec3 gColor;
} gs_in;

void main() {
	FragColor = vec4(gs_in.gColor, 1.0);
}