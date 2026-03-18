#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec4 FragPos;
out vec4 Normal;
out vec2 textureCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	// Read matrix multiplications from R to L, with R being the first transform applied
	gl_Position = proj*view*model* vec4(aPos, 1.0);
	// The fragment's position in world space coords 
	FragPos = model * vec4(aPos, 1.0f);
	Normal = vec4(mat3(transpose(inverse(model))) * aNormal, 0.0f);
	textureCoords = aTexCoords;
}
