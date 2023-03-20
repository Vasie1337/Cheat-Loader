#include "Auth.h"

std::string Auth::GetHwid() {
    ATL::CAccessToken accessToken;
    ATL::CSid currentUserSid;
    if (accessToken.GetProcessToken(TOKEN_READ | TOKEN_QUERY) &&
        accessToken.GetUser(&currentUserSid))
        return std::string(CT2A(currentUserSid.Sid()));
    return _s("none");
}

#include <winsock.h>
#include <ws2tcpip.h>

AuthError Auth::Login(std::string license) {
    WSADATA wsa_data{};
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return AuthError::AUTH_CONNECTION_ERROR;
    }

    sockaddr_in sAddr{};
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(iPort);
    inet_pton(AF_INET, sIp.c_str(), &sAddr.sin_addr);

    SOCKET sSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connect(sSock, (sockaddr*)&sAddr, sizeof(sAddr)) != 0) {
        closesocket(sSock);
        return AuthError::AUTH_CONNECTION_ERROR;
    }

    std::string sHwid = Auth::GetHwid();
    std::string sVersion = _s("1.0.1");
    std::string sMessage = license + _s(":") + sHwid + _s(";") + sVersion;

    if (send(sSock, sMessage.c_str(), static_cast<int>(strlen(sMessage.c_str())), 0) != static_cast<int>(sMessage.size())) {
        closesocket(sSock);
        WSACleanup();
        return AuthError::AUTH_CONNECTION_ERROR;
    }

    int iResponseSize = recv(sSock, (char*)(void*)(cResponse), sizeof(cResponse), 0);
    if (iResponseSize <= 0) {
        closesocket(sSock);
        WSACleanup();
        return AuthError::AUTH_INVALID_LICENSE;
    }

    if (iResponseSize == sizeof(cResponse)) {
        closesocket(sSock);
        WSACleanup();
        return AuthError::AUTH_NO_ERROR;
    }

    closesocket(sSock);
    WSACleanup();

    return AuthError::AUTH_INVALID_LICENSE;
}
