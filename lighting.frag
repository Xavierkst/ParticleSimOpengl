#version 450 core
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};
struct Light {
	float constant;
	float linear;
	float quadratic;

	vec4 direction;
	vec4 position;
	float cutOff; // cosine of the inner cone
	float outerCutOff; // cosine of outer cone

	vec4 lightVector;
	vec4 lightVector2;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 textureCoords;
in vec4 FragPos;
in vec4 Normal;

uniform Material mat;
uniform Light light;
uniform float time;
uniform vec4 viewPos;

uniform mat4 proj;
uniform mat4 view;

vec4 calculateLight(vec4 lightVec);

void main()
{
	// contribution from pointLight + dirLight
	vec4 lightColor = calculateLight(light.lightVector);
	vec4 lightColor2 = calculateLight(light.lightVector2);
	// contribution from spotLight
	vec4 spotLightColor = calculateLight(light.position);

	// Spotlight soft edges
	vec4 slightDir = normalize(light.position - FragPos);
	float cosTheta = dot(normalize(-light.direction), slightDir);
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((cosTheta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
	spotLightColor *= intensity;

	// Emission map
	vec3 specMapComponent = texture(mat.specular, textureCoords).rgb;
	vec3 emissionColor = texture(mat.emission, textureCoords + vec2(0.0f, time)).rgb;
	vec3 emissionMask = step(vec3(1.0f), vec3(1.0f) - specMapComponent);
	emissionColor *= emissionMask;

	vec3 res = spotLightColor.rgb + lightColor.rgb + lightColor2.rgb;

	FragColor = vec4(res, 1.0f);
}

vec4 calculateLight(vec4 lightVec) {
	// determine if its directional or spot light
	vec4 lightPos;
	vec4 lightDir;
	if (lightVec.w == 1.0f) {
		lightPos = lightVec;
		lightDir = normalize(lightPos - FragPos);
	} else if (lightVec.w == 0.0f) {
		lightDir = normalize(lightVec);
	}
	vec4 norm = normalize(Normal);
	// obtain the lightDir and compute ambient, diff, spec
	vec3 ambient = light.ambient * texture(mat.diffuse, textureCoords).rgb;

	float diff = max(dot(lightDir, Normal), 0.0f);
	vec3 diffuse = light.diffuse * diff * texture(mat.diffuse, textureCoords).rgb;
	
	// obtain incoming lightDir and normal, and pass into reflect
	// Use reflected ray vec and dot with viewDir vec, clamp to 0, and raise to shininess
	// 
	vec4 viewDir = normalize(viewPos - FragPos);
	vec4 reflectedLight = reflect(-lightDir, norm);
	float spec = pow(max(dot(reflectedLight, viewDir), 0.0f), mat.shininess);
	vec3 specular = spec * light.specular * texture(mat.specular, textureCoords).rgb;

	// Computer light attenuation (only when light source is NOT directional)
	if (lightVec.w == 1.0f) {
		float dist = length(lightPos - FragPos);
		float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
	}

	vec3 result = ambient + diffuse + specular; // + emissionColor;

	return vec4(result, 1.0f);
}
