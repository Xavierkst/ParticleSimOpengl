#version 450 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform samplerCube cubeMap;

void main() {
	// get the viewToFragPosRay and pass in the normal. This generates the reflected ray
	float ior = 1.00 / 1.52;
	vec3 incidentRay = normalize(FragPos - viewPos);
	vec3 R1 = reflect(incidentRay, normalize(Normal));

	FragColor = vec4(texture(cubeMap, R1).rgb, 1.0f);
}
