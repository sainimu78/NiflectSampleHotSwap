#include "Game.h"
#include <iostream>
#include "Niflect/NiflectLoadTimeModuleRegistry.h"
#include "RuntimeMethodHash.h"
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


			uint32 methodIdx_Report = INDEX_NONE;
			uint32 methodIdx_Detect = INDEX_NONE;
			Niflect::TArray<SMethodBinding2> vecBinding;
			auto type = Niflect::StaticGetType<CAntiCheat>();
			vecBinding.push_back({ FindMethodSignatureHash(type, &CAntiCheat::Detect), &methodIdx_Detect });
			vecBinding.push_back({ FindMethodSignatureHash(type, &CAntiCheat::Report), &methodIdx_Report });
			swapper.Bind2(vecBinding);
			swapper.Invoke(methodIdx_Detect);
			swapper.Invoke(methodIdx_Report);
			ASSERT(ok);


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


			//uint32 methodIdx_Report = INDEX_NONE;
			//uint32 methodIdx_Detect = INDEX_NONE;
			//Niflect::TArray<SMethodBinding> vecBinding;
			//vecBinding.push_back({ "Rep", &methodIdx_Report });
			//vecBinding.push_back({ "Det", &methodIdx_Detect });
			//swapper.Bind(vecBinding);
			//swapper.Invoke(methodIdx_Detect);
			//swapper.Invoke(methodIdx_Report);
			//ASSERT(ok);
		}
	}

	return 0;

	//{
	//	CHotSwap swapper;
	//	swapper.Init(Niflect::StaticGetType<CAntiCheatInterface>(), DEFAULT_PLUGIN_DIR_PATH, "AntiCheat", PluginInterfaceName_InitPlugin);
	//	while (true)
	//	{
	//		bool ok = swapper.Reload();
	//		ASSERT(ok);
	//	}
	//	//CRunTimeModule rtm;
	//	//if (auto reg = rtm.Load(DEFAULT_PLUGIN_DIR_PATH, "AntiCheat", PluginInterfaceName_InitPlugin))
	//	//{
	//	//	DebugPrintRegistryTypes(reg);
	//	//	printf("");
	//	//}
	//}

	//CRuntimeMethodHash a(&CAntiCheatInterface::Detect);
	//CRuntimeMethodHash b(&CAntiCheatInterface::Detect);
	//ASSERT(a == b);
	

	//auto type = Niflect::StaticGetType<CAntiCheatInterface>();
	//for (auto& it0 : type->m_vecMethodInfo)
	//{
	//	//it0.m_signatureHash
	//	//it0.m_vecInput
	//}

	auto game = CreateGame();
	auto actor = Niflect::MakeShared<CActor>();
	game->AddActor(actor);

	printf(
R"(--------------------------------------
1. Hot-Swap:	Press [Enter]
2. Exit:	Press [%c] then [Enter]
--------------------------------------
)", KEY_EXIT);

	game->Start();
	printf("Running ...\n");

	while (true)
	{
		auto key = std::cin.get();
		if (key == KEY_EXIT)
			break;
	}
	game->Stop();
	return 0;
}