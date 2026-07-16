#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Model::Model(const char* path) {
	loadModel(path);
}

void Model::Draw(Shader& shader) {
	// recursively call draw for each node:
	for (auto& mesh : meshes) {
		mesh.Draw(shader);
	}
}

// recurisvely draw the models and its children
void Model::loadModel(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	stbi_set_flip_vertically_on_load(true);
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// Process current node's mesh
	for (int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		// vertex position
		aiVector3D position = mesh->mVertices[i];
		vertex.Position = glm::vec3(position.x, position.y, position.z);

		// vertex normals
		if (mesh->HasNormals()) {
			aiVector3D normal = mesh->mNormals[i];
			vertex.Normal = glm::vec3(normal.x, normal.y, normal.z);
		}

		// texture coords
		if (mesh->mTextureCoords[0]) {
			aiVector3D textureCoords = mesh->mTextureCoords[0][i];
			vertex.TexCoords = glm::vec2(textureCoords.x, textureCoords.y);
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	// indices for EBO
	for (int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process materials (if the mesh has any)
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = loadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture> normalMaps = loadMaterialTextures(mat, aiTextureType_NORMALS, "texture_normals");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	aiColor3D color;
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	std::vector<Texture> textures;
	for (int i = 0; i < mat->GetTextureCount(type); ++i) {
		aiString fileName;
		Texture texture;
		mat->GetTexture(type, i, &fileName);
		// Check loaded_textures to see if we've loaded this texture file before, and retrieve it from there
		// to prevent re-loeading the texture from the file path more than once
		auto it = textures_loaded.begin();
		bool skip = false;
		for (int j = 0; j < textures_loaded.size(); ++j) {
			if (std::strcmp(textures_loaded[j].path.data(), fileName.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			texture.ID = TextureFromFile(fileName.C_Str(), directory.c_str());
			texture.type = typeName;
			// This path is relative to the location of the model object
			texture.path = fileName.C_Str();
			textures_loaded.push_back(texture);
			textures.push_back(texture);
		}
	}

	return textures;
}

unsigned int Model::TextureFromFile(const char* fName, const char* fPath) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	std::string fileDir(fPath);
	std::string combined = fileDir + "/" + fName;
	int width, ht, numColorChannels;
	unsigned char* data = stbi_load(combined.c_str(), &width, &ht, &numColorChannels, 0);

	if (data) {
		GLenum format;
		if (numColorChannels == 1) {
			format = GL_RED;
		}
		else if (numColorChannels == 3) {
			format = GL_RGB;
		}
		else if (numColorChannels == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		// send the texture/lighting map data to the GPU + gen mipmap for it
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, ht, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// sort what happens if tex coords go beyond range [0, 1.0]
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set how to choose + sample mipmaps to render objs that are far away
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << combined.c_str() << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
