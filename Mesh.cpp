#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> inds, std::vector<Texture> texts) : vertices(verts), indices(inds), textures(texts) {
	setupMesh();
}

void Mesh::Draw(Shader& shader) {
	unsigned int diffuseNum = 1;
	unsigned int specularNum = 1;
	unsigned int normalsNum = 1;
	// set the textures using a for loop. We have N number of diffuse and M specular textures
	for (int i = 0; i < textures.size(); ++i) {
		std::string name = textures[i].type;
		std::string num;
		glActiveTexture(GL_TEXTURE0 + i);
		if (name == "texture_diffuse") {
			num = diffuseNum++;
		} else if (name == "texture_specular") {
			num = specularNum++;
		} else if (name == "texture_normal") {
			num = normalsNum++;
		} // height map??

		shader.setInt(("mat." + name + num).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].ID);
	}

	// draw the mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Set active texture back to 0 by default as good practice
	glActiveTexture(0);
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	// pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);
	// tex coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, TexCoords)));
	glEnableVertexAttribArray(2);
	// Tangent 
	// Bittangent
	// bones
	// weight

	// unbind the VAO
	glBindVertexArray(0);
}
