#pragma once

#include "Core.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Bubble {
	class BUBBLE_API Log
	{
	public:	
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define BG_CORE_ERROR(...) ::Bubble::Log::GetCoreLogger()->error(__VA_ARGS__)
#define BG_CORE_WARN(...) ::Bubble::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define BG_CORE_INFO(...) ::Bubble::Log::GetCoreLogger()->info(__VA_ARGS__)
#define BG_CORE_TRACE(...) ::Bubble::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define BG_CORE_FATAL(...) ::Bubble::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define BG_CORE_DEBUG(...) ::Bubble::Log::GetCoreLogger()->debug(__VA_ARGS__) 

// Client log macros
#define BG_ERROR(...) ::Bubble::Log::GetClientLogger()->error(__VA_ARGS__)
#define BG_WARN(...) ::Bubble::Log::GetClientLogger()->warn(__VA_ARGS__)
#define BG_INFO(...) ::Bubble::Log::GetClientLogger()->info(__VA_ARGS__)
#define BG_TRACE(...) ::Bubble::Log::GetClientLogger()->trace(__VA_ARGS__)
#define BG_FATAL(...) ::Bubble::Log::GetClientLogger()->critical(__VA_ARGS__)
#define BG_DEBUG(...) ::Bubble::Log::GetClientLogger()->debug(__VA_ARGS__)