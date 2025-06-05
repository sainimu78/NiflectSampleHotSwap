#include "Game.h"
#include <iostream>
#include "Niflect/NiflectLoadTimeModuleRegistry.h"
#include "AntiCheatInterface.h"
#include "RuntimeMethodHash.h"
#include "HotSwap/RunTimeModule.h"
#include "HotSwap/Plugin.h"

#define EXIT_KEY 'q'

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

int main()
{
	Niflect::CNiflectModuleRegistry2 reg;
	reg.InitLoadTimeModules();

	{
		CRunTimeModule rtm;
		if (auto reg = rtm.Load(DEFAULT_PLUGIN_DIR_PATH, "AntiCheat", PluginInterfaceName_InitPlugin))
		{
			DebugPrintRegistryTypes(reg);
			printf("");
		}
	}

	CRuntimeMethodHash a(&CAntiCheatInterface::Detect);
	CRuntimeMethodHash b(&CAntiCheatInterface::Detect);
	ASSERT(a == b);
	

	auto type = Niflect::StaticGetType<CAntiCheatInterface>();
	for (auto& it0 : type->m_vecMethodInfo)
	{
		//it0.m_signatureHash
		//it0.m_vecInput
	}

	auto game = CreateGame();
	auto actor = Niflect::MakeShared<CActor>();
	game->AddActor(actor);

	printf(
R"(--------------------------------------
1. Hot-Swap:	Press [Enter]
2. Exit:	Press [%c] then [Enter]
--------------------------------------
)", EXIT_KEY);

	game->Start();
	printf("Running ...\n");

	while (true)
	{
		auto key = std::cin.get();
		if (key == EXIT_KEY)
			break;
	}
	game->Stop();
	return 0;
}