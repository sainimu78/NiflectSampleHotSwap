#pragma once
#include "Niflect/NiflectNata.h"
#include "RuntimeMethodHash.h"

class CPluginTypeNata : public Niflect::CNata
{
	typedef CPluginTypeNata CThis;
public:
	CThis& SetHotSwappable() { m_isHotSwappable = true; return *this; }

public:
	bool m_isHotSwappable = false;
};

class CPluginMethodNata : public Niflect::CNata
{
	typedef CPluginMethodNata CThis;
public:
	template <typename TMethodAddr>
	CThis& SetMethodHash(TMethodAddr&& methodAddr) { m_methodHash = CRuntimeMethodHash(methodAddr); return *this; }

public:
	CRuntimeMethodHash m_methodHash;
};