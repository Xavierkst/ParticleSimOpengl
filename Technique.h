#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include <list>
#include <memory>

#include <glad/glad.h>
#include "Shader.h"

class Technique {
public:
	Technique();
	virtual ~Technique();
	virtual bool Init();
	bool AddShader(GLenum ShaderType, const char* vShaderPath = nullptr, const char* fShaderPath = nullptr, const char* shaderPath3 = nullptr);
	bool Finalize();
	void Enable();
	GLint GetUniformLocation(const char* pUniformName);

	GLuint m_shaderProg = 0;

private:
	using ShaderObjList = std::list<std::shared_ptr<Shader>>;
	ShaderObjList m_shaderList;

	void PrintUniformList();
};

#endif // TECHNIQUE_H