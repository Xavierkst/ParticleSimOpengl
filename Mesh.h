#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int ID;
	std::string type;
	std::string path;
};

class Mesh {
public: 
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	unsigned int VAO;
	
	Mesh(std::vector<Vertex> verts, std::vector<unsigned int> inds, std::vector<Texture> texts);

	void Draw(Shader& shader);

private:
	unsigned int VBO, EBO, IBO;

	void setupMesh();
};

#endif // MESH_H
