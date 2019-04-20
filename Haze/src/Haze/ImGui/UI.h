#pragma once

#include "Haze/Core.h"

namespace Haze 
{

	class Scene;
	class Camera;
	class Object;
	class Mesh;

	struct UI 
	{

		static void ShowUI(Scene* scene, Camera* camera, Object* so = nullptr, Mesh* sm = nullptr);

	};

}