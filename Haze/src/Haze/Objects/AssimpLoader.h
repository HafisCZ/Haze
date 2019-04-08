#pragma once

#include "Model.h"

namespace Haze 
{

	class HAZE_API AssimpLoader : public ModelLoader
	{
		public:
			virtual Model* LoadImpl(const std::string& path, ModelLoaderFlags flags) override;
	};

}