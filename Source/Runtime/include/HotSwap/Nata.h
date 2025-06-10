#pragma once
#include "Niflect/NiflectNata.h"
#include "RuntimeMethodHash.h"

class CTypeNata : public Niflect::CNata
{
	typedef CTypeNata CThis;
public:
	CThis& SetSwappable() { m_isSwappable = true; return *this; }

public:
	bool m_isSwappable = false;
};

class CMethodNata : public Niflect::CNata
{
	typedef CMethodNata CThis;
public:
	template <typename TMethodAddr>
	CThis& SetHash(TMethodAddr&& methodAddr) { m_methodHash = CRuntimeMethodHash(methodAddr); return *this; }

public:
	CRuntimeMethodHash m_methodHash;
};