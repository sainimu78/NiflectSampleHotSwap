#pragma once

#ifdef WIN32
	#ifdef ANTICHEAT_EXPORTS
		#define ANTICHEAT_API __declspec(dllexport)
	#else
		#define ANTICHEAT_API __declspec(dllimport)
	#endif
#else
	#ifdef ANTICHEAT_EXPORTS
		#define ANTICHEAT_API __attribute__((visibility("default")))
	#else
		#define ANTICHEAT_API 
	#endif
#endif