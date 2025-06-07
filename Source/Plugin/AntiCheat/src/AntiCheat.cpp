#include "HotSwap/Plugin.h"
#include <stdio.h>
#include "AntiCheat_private.h"

Niflect::CNiflectModuleRegistry2* InitPlugin()
{
	static CPluginInstance s_singleton;
	return &s_singleton.m_reg;
}