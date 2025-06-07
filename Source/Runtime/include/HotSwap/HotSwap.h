#pragma once
#include "RunTimeModule.h"
#include "Niflect/Serialization/RwTree.h"
#include "HotSwap/Nata.h"
#include "Niflect/Serialization/JsonFormat.h"

using namespace RwTree;

struct SMethodBinding
{
	Niflect::CString m_inKeyword;
	uint32* m_outIndex;
};

struct SMethodBinding2
{
	Niflect::HashInt m_signatureHash;
	uint32* m_outIndex;
};

RUNTIME_API bool CopyFile(const Niflect::CString& srcFilePath, const Niflect::CString& dstFilePath);
RUNTIME_API bool RenameFile(const Niflect::CString& old_path, const Niflect::CString& new_path);

class CHotSwap
{
public:
	CHotSwap()
		: m_swappableType(NULL)
		, m_runtimeVersion(INDEX_NONE)
	{
	}
	void Init(const Niflect::CString& sourceDirPath, const Niflect::CString& moduleName, const Niflect::CString& loadTimeRegGetterFuncName, const Niflect::CString& pluginDirPath)
	{
		m_sourceDirPath = sourceDirPath;
		m_moduleName = moduleName;
		m_loadTimeRegGetterFuncName = loadTimeRegGetterFuncName;
		m_pluginDirPath = pluginDirPath;
	}
	bool Reload()
	{
		Niflect::CStringStream ss;
		auto canMig = this->SaveAndDestroy(ss);
		m_runtimeVersion++;
		if (this->CopyPluginFromSourceDirPath())
		{
			CRwNode rw;
			if (canMig)
				CJsonFormat::Read(&rw, ss);
			return this->CreateAndMigrate(&rw);
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
				if (it1.m_name.find(it0.m_inKeyword) != std::string::npos)
				{
					*it0.m_outIndex = idx1;
					found = true;
					break;
				}
			}
		}
	}
	void Bind2(Niflect::TArray<SMethodBinding2>& vecBinding)
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
	template <typename ...TArgs>
	void Invoke(uint32 methodIdx, TArgs&& ...args)
	{
		if (methodIdx == INDEX_NONE)
		{
			printf("Invocation failed, invalid method index\n");
			return;
		}
		std::array<Niflect::InstanceType*, sizeof ...(TArgs)> argArray = { (&args)... };
		auto& Func = m_swappableType->m_vecMethodInfo[methodIdx].m_Func;
		Func(m_swappableInstance.Get(), argArray.data());
	}

private:
	bool CreateAndMigrate(const CRwNode* rwOld)
	{
		this->BeforeLoading();
		Niflect::CNiflectType* foundType = NULL;
		if (auto reg = m_module.Load(m_pluginDirPath, this->GetVersionedModuleName(), m_loadTimeRegGetterFuncName))
		{
			for (uint32 idx0 = 0; idx0 < reg->GetModulesCount(); ++idx0)
			{
				auto module = reg->GetModuleByIndex(idx0);
				auto table = module->GetTable();
				for (uint32 idx1 = 0; idx1 < table->GetTypesCount(); ++idx1)
				{
					auto type = table->GetTypeByIndex(idx1);
					auto nata = Niflect::CastDerivedNata<CPluginTypeNata>(type->GetNata());
					if (nata->m_isHotSwappable)
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
		this->AfterLoading();
		if (foundType != NULL)
		{
			m_swappableType = foundType;
			ASSERT(m_swappableType->m_vecConstructorInfo.size() > 0);//可能帏未正确定义继承类, 如未 override 所有纯虚函数
			m_swappableInstance = Niflect::NiflectTypeMakeShared<DummyType>(m_swappableType);
			return m_swappableType->LoadInstanceFromRwNode(m_swappableInstance.Get(), rwOld);
		}
		return false;
	}
	bool SaveAndDestroy(Niflect::CStringStream& ss)
	{
		if (m_swappableInstance != NULL)
		{
			auto rw = CreateRwNode();
			m_swappableType->SaveInstanceToRwNode(m_swappableInstance.Get(), rw.Get());
			CJsonFormat::Write(rw.Get(), ss);
			rw = NULL;//todo: 检查为何不能在卸载后释放 rw, 现推断可能由于 rw 创建写在头文件中导致 PlacementMakeShared 的函数为被卸载模块中定义
			m_swappableType = NULL;
			m_swappableInstance = NULL;
			m_module.Unload();
			return true;
		}
		return false;
	}
	RUNTIME_API bool CopyPluginFromSourceDirPath() const;
	RUNTIME_API Niflect::CString GetVersionedModuleName() const;
	RUNTIME_API void BeforeLoading();
	RUNTIME_API void AfterLoading();
	Niflect::CString GetSrcLibFilePath() const
	{
		return ConvertToLibFilePath(m_sourceDirPath, m_moduleName);
	}
	Niflect::CString GetDstLibFilePath() const
	{
		return ConvertToLibFilePath(m_pluginDirPath, this->GetVersionedModuleName());
	}

private:
	typedef void* DummyType;

private:
	Niflect::CString m_sourceDirPath;
	Niflect::CString m_pluginDirPath;
	Niflect::CString m_moduleName;
	Niflect::CString m_loadTimeRegGetterFuncName;
	CRunTimeModule m_module;
	Niflect::TSharedPtr<DummyType> m_swappableInstance;
	Niflect::CNiflectType* m_swappableType;
	uint32 m_runtimeVersion;
};