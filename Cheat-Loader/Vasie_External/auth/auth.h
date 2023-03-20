#pragma once

#include "../antidebug/strings/xorstr.hpp"

#include <iostream>
#include <string>
#include <atlsecurity.h>

#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable : 4996)

inline unsigned char cResponse[2] = {};
inline std::string sIp = _s("[YOUR_IP]");
inline int iPort = 1957; // port

inline PVOID g_pFuncAddr;
inline DWORD g_dwFuncSize;
inline DWORD g_dwOriginalChecksum;

enum class AuthError {
    AUTH_NO_ERROR,
    AUTH_CONNECTION_ERROR,
    AUTH_INVALID_LICENSE,
};

class Auth {
public:
    static std::string GetHwid();
    static AuthError Login(std::string license);
};