#version 450 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	float ambientScale = 0.10f;
	vec3 ambient = lightColor * ambientScale;
	vec3 lightDir = normalize(FragPos - lightPos);
	// Calculate dot prod of surface normal and reverse of lightDir
	vec3 norm = normalize(Normal);
	float diff = max(dot(-lightDir, norm), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 eyeDir = normalize(viewPos - FragPos);
	// reflect fn expect light dir to point from light src to frag position
	vec3 reflectDir = reflect(lightDir, norm);
	float specularStr = 0.5f;
	float spec = pow(max(0.0, dot(eyeDir, reflectDir)), 64);
	vec3 specular = spec * lightColor * specularStr;

	vec3 res = (diffuse + ambient + specular)*objectColor;
	FragColor = vec4(res, 1.0f);
}
