#pragma once
#include "HotSwap/Nata.h"
#include "HotSwap/TypeSafeInvocationBestPractice.h"
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
		printf("Detecting ...\n");
	}
	NIF_M(CMethodNata().SetHash(&CAntiCheat::Report))
	void Report(CReportingContext& ctx)
	{
		printf("Reporting ...\n");
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
		printf("Detecting %d\n", m_detectingCount++);
	}
	NIF_M(CMethodNata().SetHash(&CAntiCheat::Report))
	void Report(CReportingContext& ctx)
	{
		printf("Reporting %d\n", m_reportingCount++);
	}

public:
	NIF_F()
	int m_detectingCount = 0;
	NIF_F()
	int m_reportingCount = 0;
};
#elif RUNTIME_VERSION == 2
NIF_T(CTypeNata().SetSwappable())
class CAntiCheat
{
public:
	NIF_M(CMethodNata().SetHash(&CAntiCheat::Detect))
	void Detect(CDetectingContext& ctx)
	{
		printf("Detecting ...\n");
	}
	NIF_M(CMethodNata().SetHash(&CAntiCheat::Report))
	void Report(CReportingContext& ctx)
	{
		printf("Reporting ...\n");
	}

public:
	NIF_F()
	int m_detectingCount = 0;
	NIF_F()
	int m_reportingCount = 0;
};
#endif