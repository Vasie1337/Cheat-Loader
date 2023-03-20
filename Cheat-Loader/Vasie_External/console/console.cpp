#include "console.h"
#include "../antidebug/strings/xorstr.hpp"

void Console::PrintLogo()
{
	system(_s("cls"));
	std::cout << _s(R"(
 _  _  __    ___  ____  ____    ____  _  _  ____  ____  ____  _  _    __    __   
( \/ )/__\  / __)(_  _)( ___)  ( ___)( \/ )(_  _)( ___)(  _ \( \( )  /__\  (  )  
 \  //(__)\ \__ \ _)(_  )__)    )__)  )  (   )(   )__)  )   / )  (  /(__)\  )(__ 
  \/(__)(__)(___/(____)(____)  (____)(_/\_) (__) (____)(_)\_)(_)\_)(__)(__)(____)       
)") << "\n\n";
}

void Console::TitleThread()
{
	//while (true)
	//{
	//	std::string str = Util::gen_random(20);
	//	SetConsoleTitleA(str.c_str());
	//	Sleep(10);
	//}

	std::string str = " Vasie External - 1.0.1";
	SetConsoleTitleA(str.c_str());
}

void Console::Init()
{
	system(_s("color 8"));
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)TitleThread, 0, 0, 0);
}