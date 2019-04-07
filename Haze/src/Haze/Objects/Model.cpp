#include "hzpch.h"
#include "Model.h"

namespace Haze 
{

	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<Triangle>& triangles) 
	{
		VAO = new VertexArray();
		VBO = new VertexBuffer(vertices.data(), vertices.size());
		IBO = new IndexBuffer(triangles.data(), triangles.size() * 3);

		VAO->BindBuffer(*VBO);

		Vertices = std::move(vertices);
		Triangles = std::move(triangles);
	}

}