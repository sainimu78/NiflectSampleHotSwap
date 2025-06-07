#pragma once
#include "Niflect/Base/String.h"
#include "Niflect/NiflectLoadTimeModuleRegistry.h"

RUNTIME_API Niflect::CString ConvertToLibFilePath(const Niflect::CString& dirPath, const Niflect::CString& libName);

class CRunTimeLinkingLibrary
{
public:
	CRunTimeLinkingLibrary()
		: m_handle(NULL)
	{
	}
	~CRunTimeLinkingLibrary()
	{
		this->Unload();
	}

public:
	RUNTIME_API bool Load(const Niflect::CString& dirPath, const Niflect::CString& libName);
	RUNTIME_API void Unload();
	template <typename TFuncPtr>
	TFuncPtr FindSymbol(const Niflect::CString& symbolName) const
	{
		if (auto found = this->InternalFindSymbol(symbolName))
			return static_cast<TFuncPtr>(found);
		return NULL;
	}

private:
	RUNTIME_API void* InternalFindSymbol(const Niflect::CString& symbolName) const;

private:
	typedef void* CNativeHandle;

private:
	CNativeHandle m_handle;
};

class CRunTimeModule
{
public:
	Niflect::CNiflectModuleRegistry2* Load(const Niflect::CString& dirPath, const Niflect::CString& libName, const Niflect::CString& GetLoadTimeModuleRegistryFuncName)
	{
		if (m_lib.Load(dirPath, libName))
		{
			if (auto Func = m_lib.FindSymbol<GetLoadTimeModuleRegistryFunc>(GetLoadTimeModuleRegistryFuncName))
				return Func();
		}
		return NULL;
	}
	void Unload()
	{
		m_lib.Unload();
	}

private:
	CRunTimeLinkingLibrary m_lib;
	typedef Niflect::CNiflectModuleRegistry2* (*GetLoadTimeModuleRegistryFunc)();
};