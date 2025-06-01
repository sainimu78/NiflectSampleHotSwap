#pragma once
#include "Component.h"
#include "Niflect/Base/Array.h"

class CActor
{
public:
	Niflect::TArray<CSharedComponent> m_vecComponent;
};
using CSharedActor = Niflect::TSharedPtr<CActor>;