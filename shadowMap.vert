#version 450 core
// layout (std140) uniform Matrices {
// 	mat4 view;
// 	mat4 proj;
// };

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightTransform;


out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	vec3 FragPosLightSpace;
} vs_out;

// rendering in the perspective of the light source. Light proj is orthographic,
// b/c the directional light dir 
void main() {
	gl_Position = lightTransform * model * vec4(aPos, 1.0);
}
