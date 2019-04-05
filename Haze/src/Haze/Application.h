#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Haze/Events/ApplicationEvent.h"

#include "Window.h"

namespace Haze {

	class HAZE_API Application
	{
		public:
			Application();
			virtual ~Application();

			void Run();

			void OnEvent(Event& e);

		private:
			bool OnWindowClose(WindowCloseEvent);

			std::unique_ptr<Window> _Window;
			bool _Running = true;
	};

	Application* CreateApplication();

}