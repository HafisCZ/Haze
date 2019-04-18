#pragma once

#include "Haze/Core.h"

namespace Haze 
{

	class Scene;
	class Camera;

	struct HAZE_API UI 
	{

		static void ShowUI(Scene* scene, Camera* camera, std::pair<float, unsigned int> raw);

	};

}