#include <iostream>
#include "Niflect/NiflectLoadTimeModuleRegistry.h"
#include "HotSwap/RuntimeMethodHash.h"
#include "HotSwap/Plugin.h"
#include "HotSwap/HotSwap.h"
#include "AntiCheat.h"
#include "App_private.h"

#define KEY_EXIT 'q'

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
static void PrintTips()
{
	printf("Tips:\n");
	uint32 tipsCount = 0;
#ifdef WIN32
	printf(
R"(%u. (For VS users) Must launch without debugger(Ctrl+F5) - Debug mode(F5) locks PDB files, preventing debug DLL rebuild.
%u. (For VS users) Use "Debug > Attach to Process" for debugging the plugin DLL.
)", tipsCount + 1, tipsCount + 2);
	tipsCount += 2;
#endif
	printf(
R"(%u. Press [Enter] to hot-swap.
%u. Press [%c] then [Enter] to quit.
)", tipsCount + 1, tipsCount + 2, KEY_EXIT);
	printf("------------------------------------------\n");
}

int main()
{
	PrintTips();

	Niflect::CNiflectModuleRegistry2 reg;
	reg.InitLoadTimeModules();

	{
		const char* pszPluginName = "AntiCheat";
		CHotSwap swapper;
		swapper.Init(DEFAULT_PLUGIN_DIR_PATH, pszPluginName, "Swappable");
		while (true)
		{
			if (swapper.Swap())
			{
				uint32 methodIdx_Report = INDEX_NONE;
				uint32 methodIdx_Detect = INDEX_NONE;
				Niflect::TArray<SMethodBinding> vecBinding;
				auto type = Niflect::StaticGetType<CAntiCheat>();
				vecBinding.push_back({ FindMethodSignatureHash(type, &CAntiCheat::Detect), &methodIdx_Detect });
				vecBinding.push_back({ FindMethodSignatureHash(type, &CAntiCheat::Report), &methodIdx_Report });
				swapper.Bind(vecBinding);
				CDetectingContext detectingCtx;
				if (methodIdx_Detect != INDEX_NONE)
					swapper.InvokeBestPractice(methodIdx_Detect, detectingCtx);
				else
					printf("Method Detect not found\n");
				CReportingContext reportingCtx;
				if (methodIdx_Report != INDEX_NONE)
					swapper.InvokeBestPractice(methodIdx_Report, reportingCtx);
				else
					printf("Method Report not found\n");
			}
			else
			{
				printf("Fail to load the plugin, please build the %s project then try again\n", pszPluginName);
			}
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