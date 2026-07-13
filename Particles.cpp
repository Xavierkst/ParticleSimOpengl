#include "Particles.h"

#include "glm/gtc/matrix_transform.hpp"

Particles::Particles() {
	m_numParticlesX = 100;
	m_numParticlesY = 100;
	m_numParticlesZ = 100;
	
	m_speed = 45.0f;
	m_angle = 0.0f;

	m_bhPos1 = glm::vec4(5.0, .0, .0, 1.0);
	m_bhPos2 = glm::vec4(-5.0, .0, .0, 1.0);

	m_totNumParticles = m_numParticlesX * m_numParticlesY * m_numParticlesZ;
}

void Particles::Init() {
	// Create all shader programs and initialize all uniform locations for the rendering shader prog
	// and the compute shader prog
	m_colorTech.Init();
	m_particleTech.Init();

	InitBuffers();
}
// Updates all the current rotation angle in delta T increments
void Particles::Update(float dt)
{	
	if (m_pause) {
		return;
	}

	m_angle += dt * m_speed;
	if (m_angle > 360.0f) {
		m_angle -= 360.0f;
	}
}

void Particles::Render(const glm::mat4& viewProj) {
	glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(m_angle), glm::vec3(.0, .0, .1));
	// apply rot matrix to the black hole initial positions 
	glm::vec3 bh1NewPos(rotMatrix* m_bhPos1);
	glm::vec3 bh2NewPos(rotMatrix* m_bhPos2);
	// compute new positions of all particles 
	ExecuteComputeShader(bh1NewPos, bh2NewPos);
	RenderParticles(bh1NewPos, bh2NewPos, viewProj);
}

void Particles::InitBuffers() {
	std::vector<glm::vec4> positions(m_totNumParticles);
	ComputePositions(positions);
	GLuint bufferSize = positions.size() * sizeof(positions[0]);

	glCreateBuffers(1, &m_posBuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_posBuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, positions.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_posBuf); // positions are binded to binding pt 0
	
	// We're not rendering with this buffer
	std::vector<glm::vec4> velocities(positions.size(), glm::vec4(0.0));
	glCreateBuffers(1, &m_veloBuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_veloBuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, velocities.data(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_veloBuf); // velos are binded to binding pt 1
	
	// VAO for drawing particles
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_posBuf);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// VAO for drawing black hole centers
	glCreateBuffers(1, &m_bhVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_bhVBO);
	GLfloat data[] = { m_bhPos1.x, m_bhPos1.y, m_bhPos1.z, m_bhPos1.w, m_bhPos2.x, m_bhPos2.y, m_bhPos2.z, m_bhPos2.w };
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &m_bhVAO);
	glBindVertexArray(m_bhVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_bhVBO);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void Particles::ComputePositions(std::vector<glm::vec4>& positions)
{	// find the spacing bet each particle:
	float dx = 2.0 / m_numParticlesX;
	float dy = 2.0 / m_numParticlesY;
	float dz = 2.0 / m_numParticlesZ;
	
	// Particles are centered at (0,0,0)
	glm::mat4 transl(1.0f);
	// not sure why we should first translate by -1,-1,-1 tho. Maybe ignore this step.
	glm::translate(transl, glm::vec3(-1.0f)); 

	// start off all particles evenly spaced in a cube shape
	int particleIdx = 0;
	for (int x = 0; x < m_numParticlesX; ++x) {
		for (int y = 0; y < m_numParticlesY; ++y) {
			for (int z = 0; z < m_numParticlesZ; ++z) {
				glm::vec4& particle = positions[particleIdx];
				particle.x = dx * x;
				particle.y = dy * y;
				particle.z = dz * z;
				particle.w = 1.0f;
				
				particle = transl * particle;
				++particleIdx;
			}
		}

	}
}

void Particles::RenderParticles(const glm::vec3& blkHolePos1, const glm::vec3& blkHolePos2, const glm::mat4& viewProj) {
	// enable color shader program
	m_colorTech.Enable();
	// set the view-projection matrix uniform
	m_colorTech.setWVP(viewProj);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// bind the particles VAO and draw particles
	glPointSize(2.0f);
	// The particles are ' quite transparent'
	m_colorTech.setColor(glm::vec4(0.0, 0.0, 0.0, 0.2)); 
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, m_totNumParticles);
	glBindVertexArray(0);

	// large, red black hole core
	glPointSize(15.0f);
	glBindVertexArray(m_bhVAO);
	GLfloat data[] = { blkHolePos1.x, blkHolePos1.y, blkHolePos1.z, 1.0f, blkHolePos2.x, blkHolePos2.y, blkHolePos2.z, 1.0f };
	glBindBuffer(GL_ARRAY_BUFFER, m_bhVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(GLfloat), data);
	m_colorTech.setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
	glDrawArrays(GL_POINTS, 0, 2);
	glBindVertexArray(0);
}

void Particles::ExecuteComputeShader(const glm::vec3& blkHolePos1, const glm::vec3& blkHolePos2) {
	// the newly rotated orientation of both black hole objects per dt are passed in here
	// No VAOs or dt needs to be sent as uniforms to the compute shader here. 
	// All particle positions and velo live in GPU and persist/get updated per dispatch job
	m_particleTech.Enable();
	m_particleTech.SetBlackHoles(blkHolePos1, blkHolePos2);

	glDispatchCompute(m_totNumParticles, 1, 1);
	// do not continue with reading from the buffer until the GPU is done writing(?)
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); 
}

void Particles::togglePause(const bool pause) {
	m_pause = pause;
	m_particleTech.Enable();
	m_particleTech.SetPauseSim(m_pause);
}
