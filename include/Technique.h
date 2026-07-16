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
	void Enable();
	GLuint GetProgram();

protected:
	bool AddShader(GLenum ShaderType, const char* shaderPath1, const char* shaderPath2 = nullptr, const char* shaderPath3 = nullptr);
	GLint GetUniformLocation(const char* pUniformName);
	GLuint m_shaderProg = 0;

private:
	using ShaderObjList = std::list<std::shared_ptr<Shader>>;
	// holds ptr to all Shader programs -- At this pt, should expect 1 per Technique
	ShaderObjList m_shaderList; 
	void PrintUniformList();
};

#endif // TECHNIQUE_H