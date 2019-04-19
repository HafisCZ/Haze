#include "hzpch.h"
#include "Mesh.h"

namespace Haze
{

	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<Triangle>& triangles, const VertexFormat& format) {
		VAO = new VertexArray();
		VBO = new VertexBuffer(vertices.data(), (int)vertices.size(), format);
		IBO = new IndexBuffer(triangles.data(), (int)triangles.size() * 3);

		VAO->BindBuffer(*VBO, format);

		AABB_MIN = { 0.0f, 0.0f, 0.0f };
		AABB_MAX = { 0.0f, 0.0f, 0.0f };

		std::for_each(vertices.begin(), vertices.end(), [this](Vertex& v) {
			if (v.Position.x < AABB_MIN.x) AABB_MIN.x = v.Position.x;
			else if (v.Position.x > AABB_MAX.x) AABB_MAX.x = v.Position.x;
			if (v.Position.y < AABB_MIN.y) AABB_MIN.y = v.Position.y;
			else if (v.Position.y > AABB_MAX.y) AABB_MAX.y = v.Position.y;
			if (v.Position.z < AABB_MIN.z) AABB_MIN.z = v.Position.z;
			else if (v.Position.z > AABB_MAX.z) AABB_MAX.z = v.Position.z;
		});

		Vertices = std::move(vertices);
		Triangles = std::move(triangles);
	}

	Mesh* Mesh::GetCUBE() {
		static Mesh* cube = new Mesh(std::vector<Vertex>{
			{ { 1.0f, 1.0f, 1.0f } },
			{ {  1.0f,  1.0f, -1.0f } },
			{ {  1.0f, -1.0f,  1.0f } },
			{ {  1.0f, -1.0f, -1.0f } },
			{ { -1.0f,  1.0f,  1.0f } },
			{ { -1.0f,  1.0f, -1.0f } },
			{ { -1.0f, -1.0f,  1.0f } },
			{ { -1.0f, -1.0f, -1.0f } }
		}, std::vector<Triangle>{
			{ 5, 7, 3 }, { 3, 1, 5 },
			{ 6, 7, 5 }, { 5, 4, 6 },
			{ 3, 2, 0 }, { 0, 1, 3 },
			{ 6, 4, 0 }, { 0, 2, 6 },
			{ 5, 1, 0 }, { 0, 4, 5 },
			{ 7, 6, 3 }, { 3, 6, 2 }
		});

		return cube;
	}

	Mesh* Mesh::GetPLANE() {
		static Mesh* plane = new Mesh(std::vector<Vertex>{
			{ { -1.0f, 1.0f, 0.0f }, {}, { 0.0f, 1.0f } },
			{ { -1.0f, -1.0f, 0.0f }, {}, { 0.0f, 0.0f } },
			{ { 1.0f, 1.0f, 0.0f }, {}, { 1.0f, 1.0f } },
			{ { 1.0f, -1.0f, 0.0f }, {}, { 1.0f, 0.0f } }
		}, std::vector<Triangle>{
			{ 0, 1, 2 }, { 1, 3, 2 }
		});

		return plane;
	}


}