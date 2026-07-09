#include "Technique.h"

Technique::Technique() {
	m_shaderProg = 0;
}

Technique::~Technique() {
	// for (ShaderObjList::iterator it = m_shaderList.begin(); it != m_shaderList.end(); ++it) {
	// 	glDeleteShader((*it)->ID);
	// }
	if (m_shaderProg != 0) {
		glDeleteProgram(m_shaderProg);
		m_shaderProg = 0;
	}
}

bool Technique::Init() {
	// if (m_shaderProg) {
	// 	glDeleteProgram(m_shaderProg);
	// }
	return true;
}

void Technique::Enable() {
	// using the first program 
	// (we don't anticipate there to be more than 1 shader prog for now)
	m_shaderList.front()->use();
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
		m_shaderList.push_back(shader);
		return true;
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


bool Technique::Finalize() {
}

GLint Technique::GetUniformLocation(const char* pUniformName) {
	std::shared_ptr<Shader> sh = std::make_shared<Shader>((*m_shaderList.begin()));
	return glGetUniformLocation(sh->ID, pUniformName);
}
