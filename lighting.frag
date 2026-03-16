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
	float cutOff; // this is the cosine of the cutoff angle

	vec4 lightVector;
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

uniform mat4 proj;
uniform mat4 view;

void main()
{
	vec3 ambient = light.ambient * texture(mat.diffuse, textureCoords).rgb;

	vec4 lightPos = view * light.lightVector;
	vec4 lightDir  = normalize(lightPos - FragPos);
	// if (light.lightVector.w == 1.0f) {
	// 	lightDir  = vec4(vec3(normalize(FragPos - light.lightVector)), 0.0f);
	// 	dist = length(light.lightVector - FragPos);
	// } else if (light.lightVector.w == 0.0f) {
	// 	lightDir = normalize(light.lightVector);
	// }

	// Calculate dot prod of surface normal and reverse of lightDir
	vec4 norm = normalize(Normal);
	float diff = max(dot(lightDir, norm), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(mat.diffuse, textureCoords).rgb;

	vec4 viewDir = normalize(vec4(-FragPos.xyz, 0.0f));
	// reflect fn expect light dir to point from light src to frag position
	vec4 reflectDir = vec4(reflect(-lightDir, norm).xyz, .0f);
	float spec = pow(max(0.0, dot(viewDir, reflectDir)), mat.shininess);
	vec3 specMapComponent = texture(mat.specular, textureCoords).rgb;
	vec3 specular = light.specular * spec * specMapComponent;

	// Emission map
	vec3 emissionColor = texture(mat.emission, textureCoords + vec2(0.0f, time)).rgb;
	vec3 emissionMask = step(vec3(1.0f), vec3(1.0f) - specMapComponent);
	emissionColor *= emissionMask;

	// Attenuation
	float dist = length(lightPos - FragPos);
	float attenuation = 1.0f;
	if (light.lightVector.w == 1.0f) {
		attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);
	}

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// calculate the angle made by fragment and spotDir
	// if the angle is <= phi (the largest radius for spot light), 
	// the resulting FragColor should get the contribution from this spot light
	vec4 color;
	vec4 slightDir = normalize(view*light.position - FragPos);
	float cosTheta = max(dot(normalize(view*-light.direction), slightDir), 0.0f);

	if (cosTheta > light.cutOff) {
		// do lighting calculations
		vec3 spotAmbient = light.ambient * texture(mat.diffuse, textureCoords).rgb;

		float spotDiff = max(dot(slightDir, norm), .0f);
		vec3 spotDiffuse = spotDiff * light.diffuse * texture(mat.diffuse, textureCoords).rgb;

		vec4 spotReflectDir = vec4(normalize(reflect(-slightDir, norm).xyz), 0.0f);
		float spotSpec = pow(max(dot(spotReflectDir, viewDir), .0f), mat.shininess);
		vec3 spotSpecular = spotSpec * light.specular * texture(mat.specular, textureCoords).rgb;
		float spotDist = length(light.position - FragPos);
		float spotAttenuation = 1.0f/ (light.constant + light.linear * spotDist + light.quadratic * spotDist * spotDist);
		color = vec4(spotAmbient + spotDiffuse + spotSpecular, 1.0f);
	} else {
		// just add ambient light contribution from your spotlight
		color = vec4(light.ambient * texture(mat.diffuse, textureCoords).rgb, 1.0f);
	}


	vec3 res = diffuse + ambient + specular + color.rgb;// + emissionColor; 
	
	FragColor = vec4(res, 1.0f);
}
