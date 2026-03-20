#pragma once
#ifndef MODEL_H_
#define MODEL_H_

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

class Model {
public: 
	Model(const char* path) {
		loadModel(path);
	}
	
	void Draw(Shader& shader) {
		// recursively call draw for each node:
		for (auto& mesh : meshes) {
			mesh.Draw(shader);
		}
	}
private:
	std::vector<Mesh> meshes;
	std::string directory;

	void processNode(aiNode* node, const aiScene* scene) {

		// aiMesh** meshes = scene->mMeshes;
		// for (int i = 0; i < scene->mNumMeshes; ++i) {
		// 	processMesh(meshes[i], scene);
		// }
	}
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	// recurisvely draw the models and its children
	void loadModel(std::string path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		} 

		directory = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene);

	}
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};


#endif MODEL_H_

