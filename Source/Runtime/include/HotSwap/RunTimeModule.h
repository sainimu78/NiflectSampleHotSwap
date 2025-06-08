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
			return reinterpret_cast<TFuncPtr>(found);
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
	Niflect::CNiflectModuleRegistry2* Load(const Niflect::CString& dirPath, const Niflect::CString& libName)
	{
		if (m_lib.Load(dirPath, libName))
		{
			m_reg = Niflect::MakeShared<Niflect::CNiflectModuleRegistry2>();
			if (m_reg->InitLoadTimeModules())
				return m_reg.Get();
		}
		return NULL;
	}
	void Unload()
	{
		m_reg = NULL;
		m_lib.Unload();
	}

private:
	CRunTimeLinkingLibrary m_lib;
	Niflect::CSharedModuleRegistry2 m_reg;
};