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

		_ImGuiLayer = new ImGuiLayer();

		PushOverlay(_ImGuiLayer);
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
		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it  = _LayerStack.end(); it != _LayerStack.begin(); ) 
		{ 
			(*--it)->OnEvent(e);
			if (e.Handled) 
			{
				break;
			}
		}
	}

	bool Application::OnWindowResize(WindowResizeEvent& event) {
		glViewport(0, 0, event.GetWidth(), event.GetHeight());

		return false;
	}

	void Application::Run()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearStencil(0);

		while (_Running) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			for (Layer* layer : _LayerStack)
			{ 
				layer->OnUpdate();
			}

			_ImGuiLayer->Begin();
			for (Layer* layer : _LayerStack)
			{
				layer->OnImGuiRender();
			}
			_ImGuiLayer->End();

			_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent) {
		_Running = false;
		
		return true;
	}

}
