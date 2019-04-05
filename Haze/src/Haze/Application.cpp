#include "hzpch.h"
#include "Application.h"

#include "Haze/Log.h"

#include <GLFW/glfw3.h>

namespace Haze {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application() 
	{
		_Window = std::unique_ptr<Window>(Window::Create());
		_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	Application::~Application() 
	{

	}

	void Application::OnEvent(Event& e) 
	{
		HZ_CORE_TRACE("{0}", e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
	}

	void Application::Run()
	{
		while (_Running) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent) {
		_Running = false;
		
		return true;
	}

}
