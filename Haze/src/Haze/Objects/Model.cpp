#include "hzpch.h"
#include "Model.h"

namespace Haze 
{

	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<Triangle>& triangles) 
	{
		VBO = new VertexBuffer(vertices.data(), vertices.size());
		VAO = new VertexArray(*VBO);
		IBO = new IndexBuffer(triangles.data(), triangles.size() * 3);

		Vertices = std::move(vertices);
		Triangles = std::move(triangles);
	}

}