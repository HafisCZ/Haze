#pragma once

#include "Haze/Core.h"

#include "Haze/Buffers/IndexBuffer.h"
#include "Haze/Buffers/VertexArray.h"
#include "Haze/Textures/Texture.h"

#include <vector>

#include <glm/glm.hpp>

namespace Haze 
{

	struct HAZE_API Vertex 
	{
		glm::vec3 Position, Normal;
		glm::vec2 TextureUV;
		glm::vec3 Tangent, Bitangent;
	};

	struct HAZE_API Triangle 
	{
		unsigned int A, B, C;
	};

	class HAZE_API Mesh
	{
		public:
			Mesh() {}
			Mesh(std::vector<Vertex>& vertices, std::vector<Triangle>& triangles);

		public:
			std::vector<Vertex> Vertices;
			std::vector<Triangle> Triangles;

			VertexArray* VAO;
			IndexBuffer* IBO;
			VertexBuffer* VBO;

			Texture* Textures[3];
	};

	enum ModelLoaderFlags 
	{
		All = 15,
		Triangulate			= BIT(0),
		CalculateTangents	= BIT(1),
		SmoothNormals		= BIT(2),
		FlipUVCoords		= BIT(3)
	};

	class HAZE_API Model 
	{
		public:
			Model() {}
			~Model() { for (auto m : Meshes) delete m; }

			std::vector<Mesh*>::iterator begin() { return Meshes.begin(); }
			std::vector<Mesh*>::iterator end() { return Meshes.end(); }

			std::vector<Mesh*> Meshes;
	};

	class HAZE_API ModelLoader
	{
		public:
			static Model* Load(const std::string& path, ModelLoaderFlags flags = All) { return _Instance->LoadImpl(path, flags); }

			virtual Model* LoadImpl(const std::string& path, ModelLoaderFlags flags) = 0;

		private:
			static ModelLoader* _Instance;
	};

}