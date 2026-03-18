#version 450 core
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct DirLight {
	vec4 direction;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct PointLight {
	float constant;
	float linear;
	float quadratic;

	vec4 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	float constant;
	float linear;
	float quadratic;

	float cutOff; // cosine of the inner cone
	float outerCutOff; // cosine of outer cone 
	
	vec4 position;
	vec4 direction;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 textureCoords;
in vec4 FragPos;
in vec4 Normal;

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform Material mat;
uniform float time;
uniform vec4 viewPos;

uniform mat4 proj;
uniform mat4 view;

vec3 calculateDirLight(DirLight light);
vec3 calculatePointLight(PointLight light);
vec3 calculateSpotLight(SpotLight light);
float calcAttenuation(vec4 lightPos, float constant, float linear, float quadratic);
vec3 phongShading(vec3 lightAmbient, vec3 lightDiffuse, vec3 lightSpecular, vec4 lightDir, vec4 norm);

void main()
{
	// contribution from pointLight + dirLight
	vec3 dirLightColor = calculateDirLight(dirLight);

	vec3 pointLightColor;
	for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
		pointLightColor += calculatePointLight(pointLights[i]);
	}

	// contribution from spotLight
	vec3 spotLightColor = calculateSpotLight(spotLight);

	// Emission map
	vec3 specMapComponent = texture(mat.specular, textureCoords).rgb;
	vec3 emissionColor = texture(mat.emission, textureCoords + vec2(0.0f, time)).rgb;
	vec3 emissionMask = step(vec3(1.0f), vec3(1.0f) - specMapComponent);
	emissionColor *= emissionMask;

	vec3 res = dirLightColor + pointLightColor + spotLightColor;
	FragColor = vec4(res, 1.0f);
}

vec3 calculatePointLight(PointLight light) {
	vec4 lightDir = normalize(light.position - FragPos);
	vec4 norm = normalize(Normal);
	vec3 result = phongShading(light.ambient, light.diffuse, light.specular, lightDir, norm);

	// Computer light attenuation (only when light source is NOT directional)
	float attenuation = calcAttenuation(light.position, light.constant, light.linear, light.quadratic);
	result *= attenuation;

	return result;
}

vec3 calculateSpotLight(SpotLight light) {
	vec4 lightDir = normalize(light.position - FragPos);
	vec4 norm = normalize(Normal);
	vec3 result = phongShading(light.ambient, light.diffuse, light.specular, lightDir, norm);

	float attenuation = calcAttenuation(light.position, light.constant, light.linear, light.quadratic);
	result *= attenuation;

	// Spotlight soft edges
	float cosTheta = dot(normalize(-light.direction), lightDir);
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((cosTheta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

	result *= intensity;
	
	return result;
}

vec3 calculateDirLight(DirLight light) {
	vec4 lightDir = normalize(-light.direction);
	vec4 norm = normalize(Normal);
	vec3 result = phongShading(light.ambient, light.diffuse, light.specular, lightDir, norm);
	// no atten calc

	return result;
}

float calcAttenuation(vec4 lightPos, float constant, float linear, float quadratic) {
	float dist = length(lightPos - FragPos);
	float attenuation = 1.0f / (constant + linear * dist + quadratic * dist * dist);
	return attenuation;
}

vec3 phongShading(vec3 lightAmbient, vec3 lightDiffuse, vec3 lightSpecular, vec4 lightDir, vec4 norm) {
	// obtain the lightDir and compute ambient, diff, spec
	vec3 ambient = lightAmbient * texture(mat.diffuse, textureCoords).rgb;

	float diff = max(dot(lightDir, norm), 0.0f);
	vec3 diffuse = lightDiffuse * diff * texture(mat.diffuse, textureCoords).rgb;
	
	// obtain incoming lightDir and normal, and pass into reflect
	// Use reflected ray vec and dot with viewDir vec, clamp to 0, and raise to shininess
	vec4 viewDir = normalize(viewPos - FragPos);
	vec4 reflectedLight = reflect(-lightDir, norm);
	float spec = pow(max(dot(reflectedLight, viewDir), 0.0f), mat.shininess);
	vec3 specular = spec * lightSpecular * texture(mat.specular, textureCoords).rgb;

	return ambient + diffuse + specular;
}
