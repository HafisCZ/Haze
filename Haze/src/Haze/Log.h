#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Haze {

	class Log
	{
		public:
			static void Init();

			inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return _CoreLogger; }
			inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return _ClientLogger; }

		private:
			static std::shared_ptr<spdlog::logger> _CoreLogger;
			static std::shared_ptr<spdlog::logger> _ClientLogger;
	};
}

#define HZ_CORE_CRITICAL(...)   ::Haze::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define HZ_CORE_ERROR(...)   ::Haze::Log::GetCoreLogger()->error(__VA_ARGS__)
#define HZ_CORE_WARN(...)    ::Haze::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define HZ_CORE_INFO(...)    ::Haze::Log::GetCoreLogger()->info(__VA_ARGS__)
#define HZ_CORE_TRACE(...)   ::Haze::Log::GetCoreLogger()->trace(__VA_ARGS__)

#define HZ_CRITICAL(...)        ::Haze::Log::GetClientLogger()->critical(__VA_ARGS__)
#define HZ_ERROR(...)        ::Haze::Log::GetClientLogger()->error(__VA_ARGS__)
#define HZ_WARN(...)         ::Haze::Log::GetClientLogger()->warn(__VA_ARGS__)
#define HZ_INFO(...)         ::Haze::Log::GetClientLogger()->info(__VA_ARGS__)
#define HZ_TRACE(...)        ::Haze::Log::GetClientLogger()->trace(__VA_ARGS__)