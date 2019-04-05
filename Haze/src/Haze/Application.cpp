#include "hzpch.h"
#include "Application.h"

#include "Haze/Log.h"

#include <glad/glad.h>

namespace Haze {

	Application* Application::_Instance = nullptr;

	Application::Application() 
	{
		HZ_CORE_ASSERT(!_Instance, "Application already exists!");
		_Instance = this;

		_Window = std::unique_ptr<Window>(Window::Create());
		_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));
	}

	Application::~Application() 
	{

	}

	void Application::PushLayer(Layer* layer) 
	{
		_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{ 
		_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e) 
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(Application::OnWindowClose));

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
