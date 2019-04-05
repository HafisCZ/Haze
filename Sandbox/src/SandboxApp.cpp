#include <Haze.h>

class SandboxLayer : public Haze::Layer
{
	public:
		SandboxLayer() : Layer("Sandbox Layer") 
		{
	
		}

		void OnUpdate() override 
		{
			HZ_INFO("Sandbox Layer: Update");
		}

		void OnEvent(Haze::Event& event) override 
		{
			HZ_INFO("{0}", event);
		}
};

class Sandbox : public Haze::Application
{
	public:
		Sandbox() 
		{
			PushLayer(new SandboxLayer());
		}

		~Sandbox() 
		{

		}
};

Haze::Application* Haze::CreateApplication()
{
	return new Sandbox();
}