#pragma once

#include "Haze/Core.h"
#include "Haze/Objects/Model.h"

#include <glm/glm.hpp>

namespace Haze
{

	class HAZE_API Scene 
	{
		public:
			std::vector<Object*> Objects;
	};

	struct HAZE_API ModelMatrix 
	{
		glm::vec3 Scale;
		glm::vec3 Position;
		glm::vec3 Rotation;
	};

	class HAZE_API Object 
	{
		public:
			Model* Model;
			ModelMatrix Matrix;
	};

}