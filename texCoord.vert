#version 450 core


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 texCoord;

out vec2 textureCoord;
out vec3 FragPos;
out vec3 vertColor;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	gl_Position = proj*view*model*vec4(aPos, 1.0f);
	FragPos = vec3(gl_Position);
	textureCoord = vec2(texCoord.x, texCoord.y);
	vertColor = aColor;
}
