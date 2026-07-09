#include "ParticlesTechnique.h"

ParticlesTechnique::ParticlesTechnique() {
}

bool ParticlesTechnique::Init() {
	if (!Technique::Init()) {
		return false;
	}

	if (!AddShader(GL_COMPUTE_SHADER, "particles.comp")) {
		return false;
	}

	// retrieve Uniform for both black holes and check the uniform values
	return true;
}

bool ParticlesTechnique::SetBlackHoles(const glm::vec3& pos1, const glm::vec3& pos2) {
	glUniform3f(m_blackHoleLoc1, pos1.x, pos1.y, pos1.z);
	glUniform3f(m_blackHoleLoc2, pos2.x, pos2.y, pos2.z);
}
