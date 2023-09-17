#pragma once

#ifdef BG_PLATFORM_WINDOWS
	#ifdef BG_BUILD_DLL
		#define BUBBLE_API __declspec(dllexport)
	#else
		#define BUBBLE_API __declspec(dllimport)
	#endif
#else
	#error Bubble only supports Windows!
#endif

#ifdef BG_ENABLE_ASSERTS
	#define BG_ASSERT(x, ...) { if(!(x)) { BG_ERROR("Assertion Failed {0}", __VA_ARGS__); __debugbreak(); } }
	#define BG_CORE_ASSERT(x, ...) { if(!(x)) { BG_CORE_ERROR("Assertion Failed {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define BG_ASSERT(x, ...)
	#define BG_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)