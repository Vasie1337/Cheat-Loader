#pragma once
#include "../../util/util.h"
#include <Windows.h>
#include <iostream>
#include <winternl.h>
#include <ntstatus.h>
#include <atomic>
#include <mutex>
#include <TlHelp32.h>
#include <d3d9.h>
#include <dwmapi.h>
#include <xmmintrin.h>

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "dwmapi.lib")
