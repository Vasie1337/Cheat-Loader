#include "../../mapper/kdmapper.hpp"
#include "../../antidebug/strings/xorstr.hpp"

#include "driver.h"

bool Driver::Init()
{
	LoadLibraryA(_s("user32.dll"));
	LoadLibraryA(_s("win32u.dll"));
	LoadLibraryA(_s("ntdll.dll"));

	*(PVOID*)&NtUserCreateDesktopEx = GetProcAddress(
		GetModuleHandleA(_s("win32u.dll")),
		_s("NtUserCreateDesktopEx")
	);

	return true;
}

bool Driver::callbackExample(ULONG64* param1, ULONG64* param2, ULONG64 allocationPtr, ULONG64 allocationSize, ULONG64 mdlptr) {
	UNREFERENCED_PARAMETER(param1);
	UNREFERENCED_PARAMETER(param2);
	UNREFERENCED_PARAMETER(allocationPtr);
	UNREFERENCED_PARAMETER(allocationSize);
	UNREFERENCED_PARAMETER(mdlptr);
	return true;
}

bool Driver::Load()
{
	HANDLE iqvw64e_device_handle = intel_driver::Load();
	
	if (iqvw64e_device_handle == INVALID_HANDLE_VALUE)
		return false;
	
	if (!kdmapper::MapDriver(iqvw64e_device_handle, DriverData, 0, 0, false, true, false, false, callbackExample, &driver_exit_code)) {
		intel_driver::Unload(iqvw64e_device_handle);
		return false;
	}
	
	if (!intel_driver::Unload(iqvw64e_device_handle)) {
		return false;
	}

	if (driver_exit_code != 291) {
		return false;
	}

	return true;
}

uintptr_t Driver::getbase()
{
	uintptr_t Buffer{};

	_data r{};
	r.code = 123;
	r.buff = (uintptr_t)&Buffer;
	r.base = TRUE;
	r.src = process_id;
	r.dest = GetCurrentProcessId();
	NtUserCreateDesktopEx(NULL, NULL, &r, NULL, NULL, NULL);

	base = Buffer;

	return Buffer;
}

INT32 Driver::find_process(LPCTSTR process_name)
{
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) {
		do {
			if (!lstrcmpi(pt.szExeFile, process_name)) {
				CloseHandle(hsnap);
				process_id = pt.th32ProcessID;
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap);

	return { NULL };
}
