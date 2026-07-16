#include "ColorTechnique.h"

ColorTechnique::ColorTechnique() {
}

void ColorTechnique::setWVP(const glm::mat4x4& VP)
{
	glUniformMatrix4fv(m_WVPLoc, 1, /*Transpose*/GL_FALSE, glm::value_ptr(VP));
}

void ColorTechnique::setColor(const glm::vec4& c)
{
	glUniform4f(m_colorLoc, c.r, c.g, c.b, c.a);
}

bool ColorTechnique::Init() {
	const char* placeHolderVShader = "shaders/computeShader/particleSim.vert";
	const char* placeHolderFShader = "shaders/computeShader/particleSim.frag";

	// Create shader prog + compile, and link vert and frag shader 
	if (!AddShader(GL_VERTEX_SHADER, placeHolderVShader, placeHolderFShader)) {
		std::cout << "Color Technique Shader failed to get added!" << std::endl;
		return false;
	}
	
	m_WVPLoc = GetUniformLocation("WVPMatrix"); // or gWVP in the OG case
	m_colorLoc = GetUniformLocation("Color"); // or gColor in OG case

	return true;
}
