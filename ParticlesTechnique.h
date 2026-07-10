#ifndef PARTICLES_TECHNIQUE_H
#define PARTICLES_TECHNIQUE_H

#include "Technique.h"

class ParticlesTechnique : public Technique {
public:
	ParticlesTechnique();
	bool SetBlackHoles(const glm::vec3& pos1, const glm::vec3& pos2);
	bool Init();

private:
	GLuint m_blackHole1Loc = INVALID_UNIFORM_LOC;
	GLuint m_blackHole2Loc = INVALID_UNIFORM_LOC;
};

#endif // PARTICLES_TECHNIQUE_H
