#version 450 core

out vec4 FragColor;

in vec3 vertColor;
in vec2 textureCoord;
in vec3 FragPos;

uniform sampler2D myTexture;
uniform sampler2D myTexture2;
uniform float mixValue;

void main() {
	FragColor = mix(texture(myTexture, textureCoord), texture(myTexture2, textureCoord), mixValue);
}
