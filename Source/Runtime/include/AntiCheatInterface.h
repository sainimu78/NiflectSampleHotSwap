#pragma once
#include "AntiCheatInterface_gen.h"

NIF_T()
class CAntiCheatInterface
{
public:
	NIF_M()
	virtual void Detect() = 0;
};