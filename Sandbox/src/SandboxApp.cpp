#include <Haze.h>

class SandboxLayer : public Haze::Layer
{
	public:
		SandboxLayer() : Layer("Sandbox Layer") 
		{
	
		}

		void OnUpdate() override 
		{

		}

		void OnEvent(Haze::Event& event) override 
		{

		}
};

class Sandbox : public Haze::Application
{
	public:
		Sandbox() 
		{
			PushLayer(new SandboxLayer());
			PushOverlay(new Haze::ImGuiLayer());
		}

		~Sandbox() 
		{

		}
};

Haze::Application* Haze::CreateApplication()
{
	return new Sandbox();
}