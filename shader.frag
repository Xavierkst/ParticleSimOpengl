#version 450 core

out vec4 FragColor;
uniform float greenValue;

void main()
{
   FragColor = vec4(.0f, greenValue, .0f, .0f);
}
