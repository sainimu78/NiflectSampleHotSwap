#pragma once
#include "RunTimeModule.h"
#include "Niflect/Serialization/RwTree.h"
#include "HotSwap/Nata.h"
#include "HotSwap/TypeSafeInvocationBestPractice.h"

using namespace RwTree;

struct SMethodBinding
{
	Niflect::HashInt m_signatureHash;
	uint32* m_outIndex;
};

class CHotSwap
{
public:
	CHotSwap()
		: m_swappableType(NULL)
		, m_runtimeVersion(INDEX_NONE)
	{
	}
	RUNTIME_API ~CHotSwap();
	void Init(const Niflect::CString& sourceDirPath, const Niflect::CString& moduleName, const Niflect::CString& pluginDirPath)
	{
		m_sourceDirPath = sourceDirPath;
		m_moduleName = moduleName;
		m_pluginDirPath = pluginDirPath;
	}
	bool Swap()
	{
		CRwNode rwOld;
		bool wasValid = this->SaveAndDestroy(&rwOld);
		m_runtimeVersion++;
		if (this->CopyPluginFromSourceDirPath())
		{
			return this->CreateAndMigrate(&rwOld);
		}
		else
		{
			if (wasValid)
				ASSERT(false);
		}
		return false;
	}
	void Bind(Niflect::TArray<SMethodBinding>& vecBinding)
	{
		auto& vecMethod = m_swappableType->m_vecMethodInfo;
		for (auto& it0 : vecBinding)
		{
			bool found = false;
			for (uint32 idx1 = 0; idx1 < vecMethod.size(); ++idx1)
			{
				auto& it1 = vecMethod[idx1];
				if (it1.m_signatureHash == it0.m_signatureHash)
				{
					*it0.m_outIndex = idx1;
					found = true;
					break;
				}
			}
		}
	}
	void InvokeBestPractice(uint32 methodIdx, CInvocationContext& derivedCtx)
	{
		ASSERT(methodIdx != INDEX_NONE);
		auto& Func = m_swappableType->m_vecMethodInfo[methodIdx].m_Func;
		std::array<Niflect::InstanceType*, 1> argArray = { &derivedCtx };
		Func(m_swappableInstance.Get(), argArray.data());
	}

private:
	template <typename ...TArgs>
	void Reserved_InvokeWithoutTypeChecking(uint32 methodIdx, TArgs&& ...args)
	{
		//通过扩展维护困难的模板元编程类型检查, 见备用参考 TypeSafeInvocation_BadAlternative_MethodProxy
		ASSERT(methodIdx != INDEX_NONE);
		std::array<Niflect::InstanceType*, sizeof ...(TArgs)> argArray = { (&args)... };
		auto& Func = m_swappableType->m_vecMethodInfo[methodIdx].m_Func;
		Func(m_swappableInstance.Get(), argArray.data());
	}

private:
	bool CreateAndMigrate(const CRwNode* rwOld)
	{
		Niflect::CNiflectType* foundType = NULL;
		if (auto reg = m_module.Load(m_pluginDirPath, this->GetVersionedModuleName()))
		{
			for (uint32 idx0 = 0; idx0 < reg->GetModulesCount(); ++idx0)
			{
				auto module = reg->GetModuleByIndex(idx0);
				auto table = module->GetTable();
				for (uint32 idx1 = 0; idx1 < table->GetTypesCount(); ++idx1)
				{
					auto type = table->GetTypeByIndex(idx1);
					auto nata = Niflect::CastDerivedNata<CTypeNata>(type->GetNata());
					if (nata->m_isSwappable)
					{
						ASSERT(type->m_vecMethodInfo.size() > 0);
						foundType = type;
						break;
					}
				}
				if (foundType != NULL)
					break;
			}
		}
		else
		{
			ASSERT(false);
		}
		if (foundType != NULL)
		{
			m_swappableType = foundType;
			ASSERT(m_swappableType->m_vecConstructorInfo.size() > 0);//可能未正确定义继承类, 如未 override 所有纯虚函数
			m_swappableInstance = Niflect::NiflectTypeMakeShared<DummyType>(m_swappableType);
			return m_swappableType->LoadInstanceFromRwNode(m_swappableInstance.Get(), rwOld);
		}
		return false;
	}
	bool SaveAndDestroy(CRwNode* rw)
	{
		bool saved = false;
		if (m_swappableInstance != NULL)
		{
			m_swappableType->SaveInstanceToRwNode(m_swappableInstance.Get(), rw);
			saved = true;
		}
		this->DestroyInstance();
		return saved;
	}
	RUNTIME_API void DestroyInstance();
	RUNTIME_API bool CopyPluginFromSourceDirPath() const;
	RUNTIME_API Niflect::CString GetVersionedModuleName() const;
	RUNTIME_API Niflect::CString GetDesiredPluginLibFilePath() const;

private:
	typedef void* DummyType;

private:
	Niflect::CString m_sourceDirPath;
	Niflect::CString m_pluginDirPath;
	Niflect::CString m_moduleName;
	CRunTimeModule m_module;
	Niflect::TSharedPtr<DummyType> m_swappableInstance;
	Niflect::CNiflectType* m_swappableType;
	uint32 m_runtimeVersion;
};