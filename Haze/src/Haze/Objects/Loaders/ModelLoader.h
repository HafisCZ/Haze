#pragma once

#include "Haze/Objects/Model.h"

#include "Haze/Utils/Lambda.h"

namespace Haze 
{

	class ModelLoader 
	{
		private:
			static ModelLoader* _Instance;

		public:
			inline static Model* Load(const std::string& path, ModelLoaderFlags flags = All) { return static_cast<Model*>(Repository::Load(path, Lambda([&]() -> void* { return _Instance->LoadImpl(path, flags); }))); }

			virtual Model* LoadImpl(const std::string& path, ModelLoaderFlags flags) = 0;
	};

}