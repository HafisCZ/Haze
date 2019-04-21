#pragma once

#include "Haze/Buffers/IndexBuffer.h"
#include "Haze/Buffers/VertexArray.h"
#include "Haze/Textures/Texture.h"
#include "Haze/Repository.h"

#include <glm/glm.hpp>

namespace Haze 
{

	struct  Vertex {
		glm::vec3 Position, Normal;
		glm::vec2 TextureUV;
		glm::vec3 Tangent, Bitangent;
	};

	struct  Triangle {
		unsigned int A, B, C;
	};

	class  Mesh {
		public:
			Mesh() {}
			Mesh(std::vector<Vertex>& vertices, std::vector<Triangle>& triangles, const VertexFormat& format = VertexFormat::GetDefault());

			virtual ~Mesh() {
				for (auto texture : Textures) {
					Repository::Unload(texture);
				}
			}

			static Mesh* GetCUBE();
			static Mesh* GetPLANE();

		public:
			std::vector<Vertex> Vertices;
			std::vector<Triangle> Triangles;

			VertexArray* VAO;
			IndexBuffer* IBO;
			VertexBuffer* VBO;

			glm::vec3 AABB_MIN;
			glm::vec3 AABB_MAX;

			Texture* Textures[4] = { nullptr, nullptr, nullptr, nullptr };
	};

}