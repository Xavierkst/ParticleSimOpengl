#version 450 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTex;

void main() {
	const float offset = 1.0f / 300.0;
	vec2 offsets[9] = {
		vec2(-offset, offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right  
	};
	vec3 sampleTex[9];
	float kernel[9] = {
		-1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
	};
	float kernel2[9] = float[](
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16  
	);

	FragColor = texture(screenTex, vec2(1.0f - texCoords.x, texCoords.y));
}