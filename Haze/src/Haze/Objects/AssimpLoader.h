#pragma once

#include "Model.h"

namespace Haze 
{

	class AssimpLoader : public ModelLoader
	{
		public:
			virtual Model* LoadImpl(const std::string& path, ModelLoaderFlags flags) override;
	};

}