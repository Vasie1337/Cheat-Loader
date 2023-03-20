#pragma once
#include <ntifs.h>
#include <windef.h>

#define to_lower(c_char) ((c_char >= 'A' && c_char <= 'Z') ? (c_char + 32) : c_char)

class util {
public:
	static void print(PCSTR str);

	static BOOLEAN DataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);
	static UINT64 FindPattern(UINT64 dwAddress, UINT64 dwLen, BYTE* bMask, char* szMask);

	template <typename str_type, typename str_type_2>
	static bool crt_strcmp(str_type str, str_type_2 in_str, bool two);
};

template<typename str_type, typename str_type_2>
inline bool util::crt_strcmp(str_type str, str_type_2 in_str, bool two)
{
	if (!str || !in_str)
		return false;

	wchar_t c1, c2;

	do
	{
		c1 = *str++; c2 = *in_str++;
		c1 = to_lower(c1); c2 = to_lower(c2);

		if (!c1 && (two ? !c2 : 1))
			return true;

	} while (c1 == c2);

	return false;
}
