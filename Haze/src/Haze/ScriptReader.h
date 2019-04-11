#pragma once

#include "Haze/Core.h"

namespace Haze 
{

	class Scene;
	class Camera;

	struct ScriptReader 
	{
		static void Execute(char* script, Scene*& scene, Camera*& camera);
	};

}