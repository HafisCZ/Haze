#pragma once

#ifdef HZ_PLATFORM_WINDOWS

extern Haze::Application* Haze::CreateApplication();

int main(int argc, char** argv)
{
	auto application = Haze::CreateApplication();
	application->Run();

	delete application;
}

#endif
