#include <Haze.h>

#include "imgui/imgui.h"

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

		void OnImGuiRender() override 
		{
			ImGui::Begin("fea");
			ImGui::Text("HELLO");
			ImGui::End();
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