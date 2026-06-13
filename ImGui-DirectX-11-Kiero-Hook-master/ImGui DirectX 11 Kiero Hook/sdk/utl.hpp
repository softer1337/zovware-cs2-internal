#pragma once
#include <Windows.h>

class CUtlString
{
public:
	CUtlString() {}

	CUtlString(const char* szString)
	{
		Set(szString);
	}

	const char* Get() const
	{
		return m_szString;
	}

	void Set(const char* szValue)
	{
		using FnUtlStringSet_t = void(__fastcall*)(CUtlString*, const char*);

		static auto fnUtlStringSet =
			reinterpret_cast<FnUtlStringSet_t>(
				GetProcAddress(
					GetModuleHandleA("tier0.dll"),
					"?Set@CUtlString@@QEAAXPEBD@Z"
				)
				);

		if (!fnUtlStringSet)
			return;

		fnUtlStringSet(this, szValue);
	}

private:
	const char* m_szString;
	//CUtlBinaryBlock storage;
};

//template <typename T>
//class CUtlVector
//{
//public:
//	auto begin() const { return m_pData; }
//	auto end() const { return m_pData + m_nSize; }
//
//	auto At(int i) { return m_pData[i]; }
//	auto AtPtr(int i) { return m_pData + i; }
//
//	bool Exists(T val) const
//	{
//		for (const auto& it : *this)
//			if (it == val) return true;
//		return false;
//	}
//
//	bool Empty() const { return m_nSize == 0; }
//
//	std::span<T> it() { return std::span(m_pData, m_nSize); }
//
//	// Reordered to match cs2::CUtlVector
//	int m_nSize;        // size
//	T* m_pData;         // memory
//	int allocationCount;
//	int growSize;
//};

