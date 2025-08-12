#pragma once
#include "HotSwap/Nata.h"
#include "HotSwap/TypeSafeInvocationBestPractice.h"
#include <map>
#include "AntiCheat_gen.h"

NIF_T()
class CDetectingContext : public CInvocationContext
{
};

NIF_T()
class CReportingContext : public CInvocationContext
{
};

#define RUNTIME_VERSION 1

#if RUNTIME_VERSION == 0
NIF_T(CTypeNata().SetSwappable())
class CAntiCheat
{
public:
	NIF_M(CMethodNata().SetHash(&CAntiCheat::Detect))
	void Detect(CDetectingContext& ctx)
	{
		printf("No suspicious\n");
	}
	NIF_M(CMethodNata().SetHash(&CAntiCheat::Report))
	void Report(CReportingContext& ctx)
	{
		printf("Nothing to report\n");
	}
};
#elif RUNTIME_VERSION == 1
NIF_T(CTypeNata().SetSwappable())
class CAntiCheat
{
public:
	NIF_M(CMethodNata().SetHash(&CAntiCheat::Detect))
	void Detect(CDetectingContext& ctx)
	{
		printf("Traditional anti-cheat, %d\n", m_detectingCount++);
	}
	NIF_M(CMethodNata().SetHash(&CAntiCheat::Report))
	void Report(CReportingContext& ctx)
	{
		printf("Nothing to report\n");
	}
	NIF_F()
	int m_detectingCount = 0;
};
#elif RUNTIME_VERSION == 2
static void PrintCheaters(const std::map<std::string, int>& map)
{
	printf("Cheaters: %u\n", static_cast<uint32>(map.size()));
	uint32 idx = 0;
	for (auto& it : map)
		printf("%u: %s, %d\n", idx++, it.first.c_str(), it.second);
}

NIF_T(CTypeNata().SetSwappable())
class CAntiCheat
{
public:
	NIF_M(CMethodNata().SetHash(&CAntiCheat::Detect))
	void Detect(CDetectingContext& ctx)
	{
		printf("Modern anti-cheat, %d\n", m_detectingCount++);
		m_map["A"] = 123;
		m_map["B"] = 456;
	}
	NIF_M(CMethodNata().SetHash(&CAntiCheat::Report))
	void Report(CReportingContext& ctx)
	{
		PrintCheaters(m_map);
	}
	NIF_F()
	std::map<std::string, int> m_map;
	NIF_F()
	int m_detectingCount = 0;
};
#elif RUNTIME_VERSION == 3
NIF_T(CTypeNata().SetSwappable())
class CAntiCheat
{
public:
	NIF_M(CMethodNata().SetHash(&CAntiCheat::Detect))
	void Detect(CDetectingContext& ctx)
	{
		printf("Traditional anti-cheat, %d\n", m_detectingCount);
	}
	NIF_M(CMethodNata().SetHash(&CAntiCheat::Report))
	void Report(CReportingContext& ctx)
	{
		printf("Finished\n");
	}
	NIF_F()
	int m_detectingCount = 0;
};
#endif