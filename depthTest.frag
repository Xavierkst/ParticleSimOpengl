#version 450 core

out vec4 FragColor;

in GS_OUT {
	in vec2 texCoords;
} fs_in;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
};
// uniform sampler2D tex1;
uniform Material mat;

float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth) {
	float z = depth * 2.0f - 1.0f; // back to NDC
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
	// Since we added blending, no need to discard fragments that are nearly transparent
	vec4 texColor = texture(mat.texture_diffuse1, fs_in.texCoords);
	// if (texColor.a < 0.1) 
	// 	discard;
	FragColor = texColor;
}