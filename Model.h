#pragma once
#ifndef MODEL_H_
#define MODEL_H_

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>

#include "Mesh.h"

class Model {
public:
	Model(const char* path);

	void Draw(Shader& shader);
private:
	std::unordered_map<std::string, Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	// recurisvely draw the models and its children
	void loadModel(std::string path);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* fName, const char* fPath = ".");
};


#endif // MODEL_H_

