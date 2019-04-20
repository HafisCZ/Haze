#pragma once

#include "Haze/Scene/Scene.h"
#include "Haze/Scene/Camera.h"

namespace Haze 
{

	class OBBRenderer {
		public:
			static void Draw(Scene* scene, Camera* camera, Object* so = nullptr, Mesh* sm = nullptr);
	};

}