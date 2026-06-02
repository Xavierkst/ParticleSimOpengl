#version 450 core
out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	vec4 lightSpaceFragPos;
} fs_in;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

// Calculates a given fragment position with the closest depth stored in the shadowMap 
// from an earlier render pass. Returns 1.0 if fragment is in shadow, and 0.0 if isn't.
float calcShadow(vec4 lightSpaceFragPos) {
	// the fragment position wrt light is in clip space. Convert it to NDC:
	vec3 fragPosNDC = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
	// now its bet. [-1, 1], convert it to [0,1] so we can: 
	// 1. compare with depthMap values
	// 2. sample texture which requires values in range [0.0, 1.0]
	fragPosNDC = (fragPosNDC * 0.5) + 0.5;
	// sample the closest depth value (of some fragment from light's POV)
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	// float bias = max(0.005, 0.05* (1.0 - dot(lightDir, fs_in.Normal)));
	float shadow = 0.0;
	float currentDepth = fragPosNDC.z;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			float percentageCloserFilterDepth = texture(shadowMap, fragPosNDC.xy + texelSize * vec2(x,y)).r;
			shadow += (currentDepth - 0.006) > percentageCloserFilterDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	// Allow fragments whose z values exceed the far plane of orthographic frustum to not be in shadow
	if (currentDepth > 1.0) {
		shadow = 0.0;
	}

	return shadow;
}

void main() {
	vec3 normal = normalize(fs_in.Normal);
	vec3 color = texture(diffuseTexture, fs_in.TexCoord).rgb;
	vec3 lightColor = vec3(0.3);
	// ambient
	vec3 ambient = 0.15 * lightColor;
	// diffuse
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;
	// specular
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 halfVec = normalize(viewDir + lightDir);
	float spec = pow(max(dot(halfVec, normal), 0.0), 128.0);
	vec3 specular = spec * lightColor;
	// vec3 specular = vec3(0.0f);

	float shadow = calcShadow(fs_in.lightSpaceFragPos);
	vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	FragColor = vec4(result, 1.0);
}





