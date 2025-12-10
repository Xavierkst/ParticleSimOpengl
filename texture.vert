#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 texCoord;
uniform mat4 transf;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	// NOTE: we read the matrix multiplications from R to L, with R being the earliest applied transform
	gl_Position = proj*view*model* vec4(aPos, 1.0);
	// gl_Position = vec4(aPos, 1.0);
	vertexColor = vec4(aColor, 1.0);
	
	texCoord = aTexCoord;
}
