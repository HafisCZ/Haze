#pragma once

#include "Core.h"

#include "Window.h"

#include "Haze/LayerStack.h"
#include "Haze/Events/Event.h"
#include "Haze/Events/ApplicationEvent.h"

#include "Haze/ImGui/ImGuiLayer.h"

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

			inline static Application& Get() { return *_Instance; }
			inline Window& GetWindow() { return *_Window; }

		private:
			bool OnWindowClose(WindowCloseEvent);
			bool Application::OnWindowResize(WindowResizeEvent& event);

			std::unique_ptr<Window> _Window;
			ImGuiLayer* _ImGuiLayer;

			bool _Running = true;

			LayerStack _LayerStack;

			static Application* _Instance;
	};

	Application* CreateApplication();

}