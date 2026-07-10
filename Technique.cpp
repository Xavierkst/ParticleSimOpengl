#include "Technique.h"

Technique::Technique() {
	m_shaderProg = 0;
}

Technique::~Technique() {
	// delete shaders already handled in Shader class 
	if (m_shaderProg != 0) {
		glDeleteProgram(m_shaderProg);
		m_shaderProg = 0;
	}
}

void Technique::Enable() {
	if (m_shaderProg == 0) {
		return;
	}
	// using the first program 
	// (we don't anticipate there to be more than 1 shader prog for now)
	// m_shaderList.front()->use();
	glUseProgram(m_shaderProg);
}

GLuint Technique::GetProgram()
{
	return m_shaderProg;
}

bool Technique::AddShader(GLenum shaderType, const char* shaderPath1, const char* shaderPath2, const char* shaderPath3) {
	std::shared_ptr<Shader> shader;

	if (shaderType == GL_COMPUTE_SHADER) {
		if (!shaderPath1) {
			std::cout << "Shader doesn't exist!" << std::endl;
			return false;
		}
		const char* compShaderPath = shaderPath1;
		shader = std::make_shared<Shader>(compShaderPath);
		if (shader->ID == 0) {
			std::cout << "Error creating shader." << std::endl;
			return false;
		}
		m_shaderList.push_back(shader);
		m_shaderProg = shader->ID;
		return true;
	}

	if (!shaderPath1 || !shaderPath2) {
		return false;
	}

	// vert + frag + (maybe) geom shader:
	const char* vShaderPath = shaderPath1;
	const char* fShaderPath = shaderPath1;
	const char* gShaderPath = shaderPath1;
	shader = std::make_shared<Shader>(vShaderPath, fShaderPath, gShaderPath);

	if (!shader) {
		std::cout << "Shader doesn't exist!" << std::endl;
		return false;
	}
	if (shader->ID == 0) {
		std::cout << "Error creating shader." << std::endl;
		return false;
	}

	m_shaderList.push_back(shader);
	m_shaderProg = shader->ID;
	return true;
}

GLint Technique::GetUniformLocation(const char* pUniformName) {
	return glGetUniformLocation(m_shaderProg, pUniformName);
}
