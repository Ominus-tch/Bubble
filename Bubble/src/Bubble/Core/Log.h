#pragma once

#include "Bubble/Core/Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Bubble {

	class  Log
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

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

// Core log macros
#define BG_CORE_ERROR(...)	 ::Bubble::Log::GetCoreLogger()->error(__VA_ARGS__)
#define BG_CORE_WARN(...)	 ::Bubble::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define BG_CORE_INFO(...)	 ::Bubble::Log::GetCoreLogger()->info(__VA_ARGS__)
#define BG_CORE_TRACE(...)	 ::Bubble::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define BG_CORE_DEBUG(...)	 ::Bubble::Log::GetCoreLogger()->debug(__VA_ARGS__) 
#define BG_CORE_CRITICAL(...)::Bubble::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define BG_ERROR(...)		 ::Bubble::Log::GetClientLogger()->error(__VA_ARGS__)
#define BG_WARN(...)		 ::Bubble::Log::GetClientLogger()->warn(__VA_ARGS__)
#define BG_INFO(...)		 ::Bubble::Log::GetClientLogger()->info(__VA_ARGS__)
#define BG_TRACE(...)		 ::Bubble::Log::GetClientLogger()->trace(__VA_ARGS__)
#define BG_CRITICAL(...)	 ::Bubble::Log::GetClientLogger()->critical(__VA_ARGS__)
#define BG_DEBUG(...)		 ::Bubble::Log::GetClientLogger()->debug(__VA_ARGS__)