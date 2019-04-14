#include "hzpch.h"
#include "AssimpLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

namespace Haze 
{

	ModelLoader* ModelLoader::_Instance = new AssimpLoader();

	Model* AssimpLoader::LoadImpl(const std::string& path, ModelLoaderFlags flags)
	{
		Model* model = new Model();

		Assimp::Importer importer;
		
		unsigned int aiFlags = 0;
		if (flags & Triangulate) aiFlags |= aiProcess_Triangulate;
		if (flags & CalculateTangents) aiFlags |= aiProcess_CalcTangentSpace;
		if (flags & SmoothNormals) aiFlags |= aiProcess_GenSmoothNormals;
		if (flags & FlipUVCoords) aiFlags |= aiProcess_FlipUVs;
	
		const aiScene* scene = importer.ReadFile(path.c_str(), aiFlags);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			return nullptr;
		}

		std::string pathFS = path;
		std::replace(pathFS.begin(), pathFS.end(), '\\', '/');

		std::string directory = pathFS.substr(0, pathFS.find_last_of('/'));

		const static std::function<glm::vec3(const aiVector3D&)> vec3(
			[](const aiVector3D& vec) -> glm::vec3 { return glm::vec3(vec.x, vec.y, vec.z); }
		);

		const static std::function<glm::vec2(const aiVector3D&)> vec2(
			[](const aiVector3D& vec) -> glm::vec2 { return glm::vec2(vec.x, vec.y); }
		);

		const std::function<void(const aiScene*, aiMesh*)> processMesh(
			[&, this](const aiScene* scene, aiMesh* mesh) {
				std::vector<Vertex> vertices(mesh->mNumVertices);
				
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

				aiMaterial* materials = scene->mMaterials[mesh->mMaterialIndex];

				const auto loadMaterial = [&](aiTextureType type) -> Texture* {
					if (materials->GetTextureCount(type) > 0) 
					{
						aiString filename;
						materials->GetTexture(type, 0, &filename);
	
						return TextureLoader::Load(directory + '/' + std::string(filename.C_Str()));
					}
					return nullptr;
				};

				if (indices.size() % 3 != 0) 
				{
					HZ_CORE_ERROR("Incompatible model! Model has to be composed only of triangles!");
				}

				std::vector<Triangle> triangles(indices.size() / 3);
				for (unsigned int i = 0; i < triangles.size(); i++) {
					triangles[i] = { indices[i * 3 + 0], indices[i * 3 + 1], indices[i * 3 + 2] };
				}

				Texture* textures[4];
				textures[0] = loadMaterial(aiTextureType_DIFFUSE);
				textures[1] = loadMaterial(aiTextureType_HEIGHT);
				textures[2] = loadMaterial(aiTextureType_SPECULAR);
				textures[3] = loadMaterial(aiTextureType_AMBIENT);

				Mesh* modelMesh = new Mesh(vertices, triangles);
				for (unsigned int i = 0; i < 3; i++) 
				{
					modelMesh->Textures[i] = textures[i];
				}

				model->Meshes.push_back(modelMesh);
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