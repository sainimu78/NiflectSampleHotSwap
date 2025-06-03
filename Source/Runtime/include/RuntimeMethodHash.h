#pragma once
#include <array>

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