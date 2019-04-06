#pragma once

#include "Haze/Core.h"

#include "Haze/Buffers/IndexBuffer.h"
#include "Haze/Buffers/VertexArray.h"

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
			Mesh(std::vector<Vertex>& vertices, std::vector<Triangle>& triangles);

		public:
			std::vector<Vertex> Vertices;
			std::vector<Triangle> Triangles;

			VertexArray* VAO;
			IndexBuffer* IBO;
			VertexBuffer* VBO;
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
			static Model* Load(const std::string& path) { return _Instance->LoadImpl(path); }

			virtual Model* LoadImpl(const std::string& path) = 0;

		private:
			static ModelLoader* _Instance;
	};

}