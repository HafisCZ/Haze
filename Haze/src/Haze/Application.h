#pragma once

#include "Core.h"

namespace Haze {

	class HAZE_API Application {

		public:
			Application();
			virtual ~Application();

			void Run();

	};

	Application* CreateApplication();

}