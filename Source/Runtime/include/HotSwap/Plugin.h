#pragma once
#include "Niflect/NiflectLoadTimeModuleRegistry.h"

//#ifdef WIN32
//	#ifdef PLUGIN_EXPORTS
//		#define PLUGIN_API __declspec(dllexport)
//	#else
//		#define PLUGIN_API __declspec(dllimport)
//	#endif
//#else
//	#ifdef PLUGIN_EXPORTS
//		#define PLUGIN_API __attribute__((visibility("default")))
//	#else
//		#define PLUGIN_API 
//	#endif
//#endif

#ifdef WIN32
	#ifdef PLUGIN_C_EXPORTS
		#define PLUGIN_C_API extern "C" __declspec(dllexport)
	#else
		#define PLUGIN_C_API extern "C" __declspec(dllimport)
	#endif
#else
	#ifdef PLUGIN_C_EXPORTS
		#define PLUGIN_C_API extern "C" __attribute__((visibility("default")))
	#else
		#define PLUGIN_C_API extern "C" 
	#endif
#endif

#ifdef WIN32
#define CPF_PLATFORM_OS_NAME "Windows"
#define CPF_FINAL_OUTPUT_DIR_NAME "bin"
#else
#define CPF_PLATFORM_OS_NAME "Linux"
#define CPF_FINAL_OUTPUT_DIR_NAME "lib"
#endif
#ifdef NODEBUG
#define CPF_CONFIG_NAME "Release"
#else
#define CPF_CONFIG_NAME "Debug"
#endif

#define DEFAULT_PLUGIN_DIR_PATH "../../../../../Plugin/" CPF_PLATFORM_OS_NAME "/DefaultBuild/" CPF_CONFIG_NAME "/" CPF_FINAL_OUTPUT_DIR_NAME