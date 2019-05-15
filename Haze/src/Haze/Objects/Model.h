#pragma once

#include "Haze/Core.h"
#include "Haze/Objects/Mesh.h"



#include <vector>

namespace Haze 
{

	enum ModelLoaderFlags 
	{
		All = 15,
		Triangulate			= BIT(0),
		CalculateTangents	= BIT(1),
		SmoothNormals		= BIT(2),
		FlipUVCoords		= BIT(3)
	};

	class  Model
	{
		public:
			Model() {}
			Model(const std::string& name) : Name(name) {}
			Model(std::vector<Mesh*>& meshes) : Meshes(std::move(meshes)) {}

			~Model() { for (auto m : Meshes) delete m; }

			std::vector<Mesh*> Meshes;
			std::string Name;

			unsigned long Vertices = 0;
			unsigned long Triangles = 0;
	};

}