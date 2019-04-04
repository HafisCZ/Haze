#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Log.h"

namespace Haze {

	Application::Application() 
	{

	}


	Application::~Application() 
	{

	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);

		if (e.IsInCategory(EventCategoryApplication)) 
		{
			HZ_TRACE(e);
		}

		if (e.IsInCategory(EventCategoryInput)) 
		{
			HZ_TRACE(e);
		}

		while (true);
	}
}
