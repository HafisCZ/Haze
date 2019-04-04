#pragma once

#ifdef HZ_PLATFORM_WINDOWS
	#ifdef HZ_BUILD_DLL
		#define HAZE_API __declspec(dllexport)
	#else
		#define HAZE_API __declspec(dllimport)
	#endif
#else
	#error ERROR
#endif

#define BIT(x) (1 << x)