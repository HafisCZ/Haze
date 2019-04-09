#pragma once

#ifdef HZ_PLATFORM_WINDOWS

extern Haze::Application* Haze::CreateApplication();

int main(int argc, char** argv)
{
	Haze::Log::Init();
	HZ_CORE_WARN("Initialized!");

	auto application = Haze::CreateApplication();
	application->Run();

	delete application;
}

#endif
