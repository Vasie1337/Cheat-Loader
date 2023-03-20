#include "util.h"

void util::print(PCSTR str) {
	DbgPrintEx(0, 0, str);
}
BOOLEAN util::DataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask) {
	for (; *szMask; ++szMask, ++pData, ++bMask) if (*szMask == 'x' && *pData != *bMask) return 0;
	return (*szMask) == 0;
}
UINT64 util::FindPattern(UINT64 dwAddress, UINT64 dwLen, BYTE* bMask, char* szMask) {
	for (UINT64 i = 0; i < dwLen; i++) if (DataCompare((BYTE*)(dwAddress + i), bMask, szMask)) return (UINT64)(dwAddress + i);
	return 0;
}