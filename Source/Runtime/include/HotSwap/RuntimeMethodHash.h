#pragma once
#include <array>

class CInvocationStaticChecker
{
public:
	virtual bool Check(void* base) const = 0;
};

template <typename TOwner, typename TMethod>
class TInvocationStaticChecker : public CInvocationStaticChecker
{
public:
	virtual bool Check(void* base) const override
	{
		auto owner = static_cast<TOwner*>(base);
		(owner->*m_Setter)(val);
	}
};

class CRuntimeMethodHash
{
public:
	CRuntimeMethodHash()
		: m_addr{}
	{
	}
	template<typename TOwner, typename TRet, typename... TArgs>
	CRuntimeMethodHash(TRet(TOwner::* method)(TArgs...))
	{
		this->Init(method);
	}
	template<typename TOwner, typename TRet, typename... TArgs>
	CRuntimeMethodHash(TRet(TOwner::* method)(TArgs...) const)
	{
		this->Init(method);
	}
	bool operator==(const CRuntimeMethodHash& rhs) const
	{
		return m_addr == rhs.m_addr;
	}
	bool operator<(const CRuntimeMethodHash& rhs) const
	{
		return std::memcmp(m_addr.data(), rhs.m_addr.data(), MethodPointerSize) < 0;
	}

private:
	template <typename TMethod>
	void Init(TMethod method)
	{
		static_assert(MethodPointerSize == sizeof(method), "This implementation assumes single inheritance with consistent pointer representation");
		std::memcpy(m_addr.data(), &method, MethodPointerSize);
	}

private:
	struct SizeProbe { void stub() {}; };
	static constexpr auto MethodPointerSize = sizeof(&SizeProbe::stub);

private:
	std::array<uint8, MethodPointerSize> m_addr;
};

static void _ExpandVariadicArgsRecurs(Niflect::TArray<size_t>& vecArgRttiTypeHash)
{
}
template <typename TFirst, typename... TRest>
static void _ExpandVariadicArgsRecurs(Niflect::TArray<size_t>& vecArgRttiTypeHash, TFirst&&, TRest&&...rest)
{
	vecArgRttiTypeHash.push_back(typeid(std::remove_reference_t<TFirst>).hash_code());
	_ExpandVariadicArgsRecurs(vecArgRttiTypeHash, std::forward<TRest>(rest)...);
}
template <typename... TArgs>
static void ExpandVariadicArgs(Niflect::TArray<size_t>& vecArgRttiTypeHash, TArgs&&... args)
{
	_ExpandVariadicArgsRecurs(vecArgRttiTypeHash, std::forward<TArgs>(args)...);
}

class CMethodIndex
{
public:
	CMethodIndex()
		: m_methodIdx(INDEX_NONE)
	{
	}

public:
	uint32 m_methodIdx;
	Niflect::TArray<size_t> m_vecArgRttiTypeHash;
};