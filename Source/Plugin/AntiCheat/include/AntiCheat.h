#pragma once
#include "HotSwap/Nata.h"
#include "AntiCheat_gen.h"

#define RUNTIME_VERSION 1

#if RUNTIME_VERSION == 0
NIF_T(CPluginTypeNata().SetHotSwappable())
class CAntiCheat
{
public:
	NIF_M(CPluginMethodNata().SetMethodHash(&CAntiCheat::Detect))
	void Detect()
	{
		printf("detecting ...\n");
	}
	NIF_M(CPluginMethodNata().SetMethodHash(&CAntiCheat::Report))
	void Report()
	{
		printf("reporting ...\n");
	}
};
#elif RUNTIME_VERSION == 1
NIF_T(CPluginTypeNata().SetHotSwappable())
class CAntiCheat
{
public:
	NIF_M(CPluginMethodNata().SetMethodHash(&CAntiCheat::Detect))
	void Detect()
	{
		printf("detecting %d\n", m_detectingCount++);
	}
	NIF_M(CPluginMethodNata().SetMethodHash(&CAntiCheat::Report))
	void Report()
	{
		printf("reporting %d\n", m_reportingCount++);
	}

public:
	NIF_F()
		int m_detectingCount = 0;
	NIF_F()
		int m_reportingCount = 0;
};
#elif RUNTIME_VERSION == 2
NIF_T(CPluginTypeNata().SetHotSwappable())
class CAntiCheat
{
public:
	NIF_M(CPluginMethodNata().SetMethodHash(&CAntiCheat::Detect))
	void Detect()
	{
		printf("detecting ...\n");
	}
	NIF_M(CPluginMethodNata().SetMethodHash(&CAntiCheat::Report))
	void Report()
	{
		printf("reporting ...\n");
	}

public:
	NIF_F()
	int m_detectingCount = 0;
	NIF_F()
	int m_reportingCount = 0;
};
#endif