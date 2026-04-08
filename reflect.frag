#version 450 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform samplerCube cubeMap;
uniform samplerCube normMap;

void main() {
	// get the viewToFragPosRay and pass in the normal. This generates the reflected ray
	float ior = 1.00 / 1.52;
	vec3 incidentRay = normalize(FragPos - viewPos);
	vec3 R1 = refract(incidentRay, normalize(Normal), ior);
	vec3 norm2 = texture(normMap, R1).xyz;
	vec3 R2 = refract(R1, -norm2, 1.0f/ior);

	FragColor = vec4(texture(cubeMap, R2).rgb, 1.0f);
}
