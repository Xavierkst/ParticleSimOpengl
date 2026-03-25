#version 450 core

out vec4 FragColor;

in vec4 FragPos;
in vec2 texCoords;

uniform sampler2D tex1;

float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth) {
	float z = depth * 2.0f - 1.0f; // back to NDC
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
	float depth = linearizeDepth(gl_FragCoord.z) / far;
	FragColor = vec4(vec3(depth), 1.0f);
	// FragColor = texture(tex1, texCoords);
}