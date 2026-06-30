#include "Shader.h"
#include <glad/glad.h>

Shader::Shader(const char* cShaderPath)
{
	std::string cShaderCode; 
	std::ifstream cShaderFile;
	cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		cShaderFile.open(cShaderPath);
		std::stringstream cShaderStream;
		cShaderStream << cShaderFile.rdbuf();
		cShaderFile.close();
		cShaderCode = cShaderStream.str();
	} catch (std::ifstream::failure& e) {
		std::cout << "ERROR::COMPUTE_SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	unsigned int compute;
	const char* cShaderCode1 = cShaderCode.c_str();
	int success;
	char infoLog[512];
	compute = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute, 1, &cShaderCode1, NULL);
	glCompileShader(compute);
	glGetShaderiv(compute, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(compute, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	this->ID = glCreateProgram();
	glAttachShader(this->ID, compute);
	glLinkProgram(this->ID);
	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
		std::cout << "ERROR::COMPUTE_SHADER::PROGRAM:: LINKING_FAILED\n" << infoLog << std::endl;
	}
	
	glDeleteShader(compute);
}

Shader::Shader(const char* vShaderPath, const char* fShaderPath, const char* gShaderPath) {
	std::string vShaderCode, fShaderCode, gShaderCode;
	std::ifstream vShaderFile, fShaderFile, gShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vShaderFile.open(vShaderPath);
		fShaderFile.open(fShaderPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vShaderCode = vShaderStream.str();
		fShaderCode = fShaderStream.str();

		if (gShaderPath != nullptr) {
			gShaderFile.open(gShaderPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			gShaderCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode1 = vShaderCode.c_str();
	const char* fShaderCode1 = fShaderCode.c_str();

	// compile the shader files 
	unsigned int vertex, fragment, geom;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode1, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode1, NULL);
	glCompileShader(fragment);
	// checkCompileErrors(fragment, "FRAGMENT");
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	if (gShaderPath != nullptr) {
		geom = glCreateShader(GL_GEOMETRY_SHADER);
		const char* gShaderCode1 = gShaderCode.c_str();
		glShaderSource(geom, 1, &gShaderCode1, NULL);
		glCompileShader(geom);
		glGetShaderiv(geom, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(geom, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}
	
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (gShaderPath != nullptr)
		glAttachShader(ID, geom);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM:: LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (gShaderPath != nullptr)
		glDeleteShader(geom);
}

void Shader::use() const {
	glUseProgram(ID);
}

// ------------------------------------------------------------------------
// Utility uniform function
// ------------------------------------------------------------------------
void Shader::setBool(const std::string& name, bool val) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)val);
}

void Shader::setInt(const std::string& name, int val) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), val);
}

void Shader::setFloat(const std::string& name, float val) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), val);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

// ------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

// ------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

// ------------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4x4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
