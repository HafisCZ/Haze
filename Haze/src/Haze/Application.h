#pragma once

#include "Core.h"

#include "Window.h"

#include "Haze/LayerStack.h"
#include "Haze/Events/Event.h"
#include "Haze/Events/ApplicationEvent.h"

namespace Haze {

	class HAZE_API Application
	{
		public:
			Application();
			virtual ~Application();

			void Run();

			void OnEvent(Event& e);

			void PushLayer(Layer* layer);
			void PushOverlay(Layer* layer);

		private:
			bool OnWindowClose(WindowCloseEvent);

			std::unique_ptr<Window> _Window;
			bool _Running = true;

			LayerStack _LayerStack;
	};

	Application* CreateApplication();

}