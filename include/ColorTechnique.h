#ifndef COLOR_TECHNIQUE_H
#define COLOR_TECHNIQUE_H

#include "Technique.h"

class ColorTechnique : public Technique {
public:
	ColorTechnique();
	void setWVP(const glm::mat4x4& VP);
	void setColor(const glm::vec4& c);
	bool Init();
	
private:
	GLuint m_WVPLoc = INVALID_UNIFORM_LOC;
	GLuint m_colorLoc = INVALID_UNIFORM_LOC;
};

#endif // COLOR_TECHNIQUE_H