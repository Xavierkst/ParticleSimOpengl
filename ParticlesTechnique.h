#ifndef PARTICLES_TECHNIQUE_H
#define PARTICLES_TECHNIQUE_H

#include "Technique.h"

class ParticlesTechnique : public Technique {
public:
	ParticlesTechnique();
	bool SetBlackHoles(const glm::vec3& pos1, const glm::vec3& pos2);
	virtual bool Init();

private:
	GLuint m_blackHoleLoc1;
	GLuint m_blackHoleLoc2;
};

#endif // PARTICLES_TECHNIQUE_H
