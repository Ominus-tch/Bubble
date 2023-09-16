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