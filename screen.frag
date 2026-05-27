#version 450 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D quadTex;
uniform float near_plane;
uniform float far_plane;

const float offset = 1.0 / 300.0;

float linearize_depth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane)); 
}

void main() { 
    // vec2 offsets[9] = vec2[](
    //         vec2(-offset,  offset), // top-left
    //         vec2( 0.0f,    offset), // top-center
    //         vec2( offset,  offset), // top-right
    //         vec2(-offset,  0.0f),   // center-left
    //         vec2( 0.0f,    0.0f),   // center-center
    //         vec2( offset,  0.0f),   // center-right
    //         vec2(-offset, -offset), // bottom-left
    //         vec2( 0.0f,   -offset), // bottom-center
    //         vec2( offset, -offset)  // bottom-right    
    // );

    // float kernel[9] = float[](
    //     -1, -1, -1,
    //     -1,  9, -1,
    //     -1, -1, -1
    // );

    // vec3 sampleTex[9];
    // for (int i = 0; i < 9; ++i) {
    //     sampleTex[i] = vec3(texture(quadTex, texCoords+offsets[i]));
    // }
    // vec3 res;
    // for (int i = 0; i < 9; ++i) {
    //     res += sampleTex[i] * kernel[i];
    // }

    // // FragColor = vec4(res, 1.0);
    float fragDepth = texture(quadTex, texCoords).r;
	FragColor = vec4(vec3(fragDepth), 1.0f);
}