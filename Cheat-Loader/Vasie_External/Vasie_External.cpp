
#include <Windows.h>
#include <iostream>

#include "render/render.h"
#include "cheat/esp/esp.h"
#include "antidebug/antidebug.h"
#include "auth/auth.h"
#include "console/console.h"

int main() {

    
    Console::Init();
    Console::PrintLogo();

    //Util::KillProc(_s("FortniteClient-Win64-Shipping.exe"));

    AntidebugError AntidebugCode = Antidebug::Check();
    if (Antidebug::Check() != AntidebugError::ANTI_DEBUG_NO_ERROR) {
        Console::PrintLogo();
        std::cout << _s("   Debugger is running") << std::endl;
        Sleep(3000);
        return 1;
    }

    std::cout << _s("  License") << _s(">");
    std::string in; std::cin >> in;
    AuthError AuthCode = AuthError::AUTH_NO_ERROR;

    if (AuthCode == AuthError::AUTH_INVALID_LICENSE) {
        Console::PrintLogo();
        std::cout << _s("   Invalid license or outdated loader") << std::endl;
        Sleep(3000);
        return 1;
    }
    else if (AuthCode == AuthError::AUTH_CONNECTION_ERROR)
    {
        Console::PrintLogo();
        std::cout << _s("   Couldn't connect to server") << std::endl;
        Sleep(3000);
        return 1;
    }

    Console::PrintLogo();

    std::cout << _s("  Welcome ") << Util::GetUser() << std::endl;
    Sleep(3000);

    AntidebugCode = Antidebug::Check();
    if (Antidebug::Check() != AntidebugError::ANTI_DEBUG_NO_ERROR) {
        Console::PrintLogo();
        std::cout << _s("   Debugger is running")  << std::endl;
        Sleep(3000);
        return 1;
    }

    if (!Driver::Load()) {
        Console::PrintLogo();
        std::cout << _s("   Couldn't load driver") << std::endl;
        Sleep(3000);
        return 1;
    }

	Driver::Init();
	Driver::find_process(_s(L"FortniteClient-Win64-Shipping.exe"));
	Driver::getbase();

    Console::PrintLogo();
    std::cout << _s("   Waiting for Fortnite") << std::endl;
    while (!base || !process_id) {
        Driver::find_process(_s(L"FortniteClient-Win64-Shipping.exe"));
        Driver::getbase();

        Sleep(200);
    }
    Console::PrintLogo();
    std::cout << _s("   Fortnite has been found") << std::endl;
    Sleep(3000);
    Console::PrintLogo();
    std::cout << _s("   Enjoy :)") << std::endl;
    

    Driver::Init();
    Driver::find_process(_s(L"FortniteClient-Win64-Shipping.exe"));
    Driver::getbase();

	Render::CreateOverlay();
	Render::DirectXInit();
	Render::MainLoop();
}