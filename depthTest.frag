#version 450 core

out vec4 FragColor;

in VS_OUT {
	in vec4 FragPos;
	in vec2 texCoords;
} fs_in;

uniform sampler2D tex1;

float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth) {
	float z = depth * 2.0f - 1.0f; // back to NDC
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
	// Since we added blending, no need to discard fragments that are nearly transparent
	vec4 texColor = texture(tex1, fs_in.texCoords);
	if (texColor.a < 0.1) 
		discard;
	FragColor = texColor;
}