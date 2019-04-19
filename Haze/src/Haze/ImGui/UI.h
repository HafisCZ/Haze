#pragma once

#include "Haze/Core.h"

namespace Haze 
{

	class Scene;
	class Camera;
	class Object;

	struct UI 
	{

		static void ShowUI(Scene* scene, Camera* camera, Object* selected = nullptr);

	};

}