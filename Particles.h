#ifndef PARTICLES_H
#define PARTICLES_H

#include <vector>

#include "ParticlesTechnique.h"
#include "ColorTechnique.h"
#include "glm/glm.hpp"

class Particles {
public:
	Particles();
	void Init();
	void Update(float t);
	void Render(const glm::mat4& viewProj);

private:
	void InitBuffers();
	// Give the initial positions to be sent over as SSBOs in the GPU
	void ComputePositions(std::vector<glm::vec4>& positions);
	// Draw the particles and black hole cores
	void RenderParticles(const glm::vec3& blkHolePos1, const glm::vec3& blkHolePos2, const glm::mat4& viewProj);
	void ExecuteComputeShader(const glm::vec3& blkHolePos1, const glm::vec3& blkHolePos2);
	
	ParticlesTechnique m_pTech;
	int m_numParticlesX = 0;
	int m_numParticlesY = 0;
	int m_numParticlesZ = 0;
	float m_speed = 0.0, m_angle = 0.0;
	int m_totNumParticles = 0;
	
	// Don't need a pos or velo buffer since they live in GPU--just need to initialize it
	GLuint m_VAO = 0, m_posBuf = 0, m_veloBuf = 0;
	GLuint m_bhVAO = 0, m_bhVBO = 0;
	glm::vec4 m_bhPos1, m_bhPos2;

	ColorTechnique m_colorTech;
	ParticlesTechnique m_particleTech;
};

#endif // PARTICLES_H
