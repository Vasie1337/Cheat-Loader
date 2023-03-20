#pragma once
#include <Windows.h>
#include <winternl.h>
#include <iostream>

enum class AntidebugError {
    ANTI_DEBUG_NO_ERROR,
    ANTI_DEBUG_DEBUGGER_FOUND,
};

class Antidebug
{
public:
	static AntidebugError Check();
};

