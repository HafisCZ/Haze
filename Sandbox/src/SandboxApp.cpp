#include <Haze.h>

#include "imgui/imgui.h"

class Sandbox : public Haze::Application
{
	public:
		Sandbox() 
		{

		}

		~Sandbox() 
		{

		}
};

Haze::Application* Haze::CreateApplication()
{
	return new Sandbox();
}