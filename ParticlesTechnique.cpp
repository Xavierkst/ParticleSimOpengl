#include "ParticlesTechnique.h"

ParticlesTechnique::ParticlesTechnique() {
}

bool ParticlesTechnique::Init() {
	const char* placeHolderShaderPath = "particleSim.comp";
	if (!AddShader(GL_COMPUTE_SHADER, placeHolderShaderPath)) {
		std::cout << "Particle Technique Shader failed to get added!" << std::endl;
		return false;
	}

	// retrieve Uniform for both black holes and check the uniform values
	m_blackHole1Loc = GetUniformLocation("BHPos1");
	m_blackHole2Loc = GetUniformLocation("BHPos2");

	return true;
}

void ParticlesTechnique::SetBlackHoles(const glm::vec3& pos1, const glm::vec3& pos2) {
	glUniform3f(m_blackHole1Loc, pos1.x, pos1.y, pos1.z);
	glUniform3f(m_blackHole2Loc, pos2.x, pos2.y, pos2.z);
}
