#include <Haze.h>

#include "imgui/imgui.h"

class Sandbox : public Haze::Application
{
	public:
		Sandbox() 
		{
			PushLayer(new Haze::RendererLayer());
		}

		~Sandbox() 
		{

		}
};

Haze::Application* Haze::CreateApplication()
{
	return new Sandbox();
}