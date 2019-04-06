#include "hzpch.h"
#include "AssimpLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

namespace Haze 
{

	ModelLoader* ModelLoader::_Instance = new AssimpLoader();

	Model* AssimpLoader::LoadImpl(const std::string& path)
	{
		Model* model = new Model();

		Assimp::Importer importer;
		
		const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			return nullptr;
		}

		std::string directory = path.substr(0, path.find_last_of('/'));

		const static std::function<glm::vec3(const aiVector3D&)> vec3(
			[](const aiVector3D& vec) -> glm::vec3 { return glm::vec3(vec.x, vec.y, vec.z); }
		);

		const static std::function<glm::vec2(const aiVector3D&)> vec2(
			[](const aiVector3D& vec) -> glm::vec2 { return glm::vec2(vec.x, vec.y); }
		);

		const std::function<void(const aiScene*, aiMesh*)> processMesh(
			[&, this](const aiScene* scene, aiMesh* mesh) {
				using vertex_t = struct {
					glm::vec3 pos, nor;
					glm::vec2 tex;
					glm::vec3 tan, bta;
				};

				std::vector<vertex_t> vertices(mesh->mNumVertices);
				std::vector<unsigned int> indices(
					[&mesh]() {
						unsigned int count = 0;
						for (unsigned int i = 0; i < mesh->mNumFaces; i++) count += mesh->mFaces[i].mNumIndices;
						return count;
					}()			
				);

				for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
					vertices[i] = {
						vec3(mesh->mVertices[i]),
						mesh->mNormals ? vec3(mesh->mNormals[i]) : glm::vec3(0.0f),
						mesh->mTextureCoords[0] ? vec2(mesh->mTextureCoords[0][i]) : glm::vec2(0.0f),
						mesh->mTangents ? vec3(mesh->mTangents[i]) : glm::vec3(0.0f),
						mesh->mBitangents ? vec3(mesh->mBitangents[i]) : glm::vec3(0.0f)
					};
				}

				for (unsigned int i = 0, indice = 0; i < mesh->mNumFaces; i++) {
					aiFace face = mesh->mFaces[i];
					for (unsigned int j = 0; j < face.mNumIndices; j++) {
						indices[indice++] = face.mIndices[j];
					}
				}

				model->Meshes.push_back(new Mesh(vertices.size(), indices.size()));
			}
		);

		const std::function<void(const aiScene*, aiNode*)> processNode = [&, this](const aiScene* scene, aiNode* node) {
			for (unsigned int i = 0; i < node->mNumMeshes; i++) {
				processMesh(scene, scene->mMeshes[node->mMeshes[i]]);
			}
			for (unsigned int i = 0; i < node->mNumChildren; i++) {
				processNode(scene, node->mChildren[i]);
			}
		};

		processNode(scene, scene->mRootNode);

		return model;
	}

}