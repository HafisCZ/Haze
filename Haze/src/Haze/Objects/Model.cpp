#include "hzpch.h"
#include "Model.h"

namespace Haze 
{

	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<Triangle>& triangles, const VertexFormat& format) 
	{
		VAO = new VertexArray();
		VBO = new VertexBuffer(vertices.data(), (int) vertices.size(), format);
		IBO = new IndexBuffer(triangles.data(), (int) triangles.size() * 3);

		VAO->BindBuffer(*VBO, format);

		Vertices = std::move(vertices);
		Triangles = std::move(triangles);
	}
}