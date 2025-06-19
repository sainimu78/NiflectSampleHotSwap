#pragma once
#include "Niflect/Component/DefaultCommon.h"

#ifdef WIN32
	#ifdef RUNTIME_EXPORTS
		#define RUNTIME_API __declspec(dllexport)
	#else
		#define RUNTIME_API __declspec(dllimport)
	#endif
#else
	#ifdef RUNTIME_EXPORTS
		#define RUNTIME_API __attribute__((visibility("default")))
	#else
		#define RUNTIME_API 
	#endif
#endif