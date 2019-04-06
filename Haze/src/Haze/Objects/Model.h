#pragma once

#include "Haze/Core.h"

#include <vector>

namespace Haze 
{

	class HAZE_API Mesh
	{
		public:
			Mesh(unsigned int vertices, unsigned int indices) : Vertices(vertices), Indices(indices) {}

		public:
			unsigned int Vertices, Indices;
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