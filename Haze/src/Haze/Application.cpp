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

	void Application::PushLayer(Layer* layer) 
	{
		_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{ 
		_LayerStack.PushOverlay(layer);
	}

	void Application::OnEvent(Event& e) 
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it  = _LayerStack.end(); it != _LayerStack.begin(); ) 
		{ 
			(*--it)->OnEvent(e);
			if (e.Handled) 
			{
				break;
			}
		}
	}

	void Application::Run()
	{
		while (_Running) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : _LayerStack)
			{ 
				layer->OnUpdate();
			}

			_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent) {
		_Running = false;
		
		return true;
	}

}
