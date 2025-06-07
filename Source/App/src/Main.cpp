#include <iostream>
#include "Niflect/NiflectLoadTimeModuleRegistry.h"
#include "HotSwap/RuntimeMethodHash.h"
#include "HotSwap/RunTimeModule.h"
#include "HotSwap/Plugin.h"
#include "HotSwap/HotSwap.h"
#include "AntiCheat.h"
#include "App_private.h"

#define KEY_EXIT 'q'

static void DebugPrintRegistryTypes(Niflect::CNiflectModuleRegistry2* reg)
{
	for (uint32 idx0 = 0; idx0 < reg->GetModulesCount(); ++idx0)
	{
		auto module = reg->GetModuleByIndex(idx0);
		printf("Module: %s\n", module->GetName().c_str());
		auto table = module->GetTable();
		for (uint32 idx1 = 0; idx1 < table->GetTypesCount(); ++idx1)
		{
			auto type = table->GetTypeByIndex(idx1);
			printf("%s\n", type->GetTypeName().c_str());
		}
	}
}
template <typename TMethodAddr>
static Niflect::HashInt FindMethodSignatureHash(Niflect::CNiflectType* type, TMethodAddr&& methodAddr)
{
	CRuntimeMethodHash hashToFind(methodAddr);
	for (uint32 idx = 0; idx < type->m_vecMethodInfo.size(); ++idx)
	{
		auto& method = type->m_vecMethodInfo[idx];
		if (auto nata = Niflect::CastDerivedNata<CPluginMethodNata>(method.m_nata.Get()))
		{
			if (nata->m_methodHash == hashToFind)
				return method.m_signatureHash;
		}
	}
	return INVALID_HASH;
}

int main()
{
	Niflect::CNiflectModuleRegistry2 reg;
	reg.InitLoadTimeModules();

	{
		CHotSwap swapper;
		swapper.Init(DEFAULT_PLUGIN_DIR_PATH, "AntiCheat", PluginInterfaceName_InitPlugin, "Swappable");
		while (true)
		{
			bool ok = swapper.Reload();
			ASSERT(ok);
			uint32 methodIdx_Report = INDEX_NONE;
			uint32 methodIdx_Detect = INDEX_NONE;
			Niflect::TArray<SMethodBinding2> vecBinding;
			auto type = Niflect::StaticGetType<CAntiCheat>();
			vecBinding.push_back({ FindMethodSignatureHash(type, &CAntiCheat::Detect), &methodIdx_Detect });
			vecBinding.push_back({ FindMethodSignatureHash(type, &CAntiCheat::Report), &methodIdx_Report });
			swapper.Bind(vecBinding);
			if (methodIdx_Detect != INDEX_NONE)
				swapper.Invoke(methodIdx_Detect);
			else
				printf("methodIdx_Detect is invalid\n");
			if (methodIdx_Report != INDEX_NONE)
				swapper.Invoke(methodIdx_Report);
			else
				printf("methodIdx_Report is invalid\n");

			bool quit = false;
			auto key = std::cin.get();
			switch (key)
			{
			case KEY_EXIT:
				quit = true;
				break;
			default:
				break;
			}
			if (quit)
				break;
		}
	}

	return 0;
}