#include "hook.h"

INT64(__fastcall* NtUserCreateDesktopEx)(PVOID, PVOID, PVOID, PVOID, PVOID, PVOID);
INT64 __fastcall Handler(PVOID a1, PVOID a2, _data* a3, PVOID a4, PVOID a5, PVOID a6) {
    if ((!a3) || (ExGetPreviousMode() != UserMode || a3->code != 123)) return NtUserCreateDesktopEx(a1, a2, a3, a4, a5, a6);
    
    PEPROCESS src = process::findbypid(a3->src);
    PEPROCESS dest = process::findbypid(a3->dest);

    if (!src || !dest)
        return NULL;

    if (a3->base)
        process::getbase(src, dest, a3->buff);
    else
        process::read(src, a3->addr, dest, a3->buff, a3->size);

    return NULL;
}

NTSTATUS hook::init() {

    PEPROCESS pwinlogon = process::findbyname(L"winlogon.exe");
    if (!pwinlogon) return STATUS_UNSUCCESSFUL;
    
    uintptr_t uImage = process::getimage(L"\\SystemRoot\\System32\\win32kbase.sys");
    if (!uImage) return STATUS_UNSUCCESSFUL;
    
    uintptr_t uDataPtr = util::FindPattern(
        (UINT64)uImage,
        (UINT64)0xFFFFFFFFFF,
        (BYTE*)"\x74\x2E\x8B\x84\x24\x00\x00\x00\x00",
        "xxxxx????"
    );
    if (!uDataPtr) return STATUS_UNSUCCESSFUL;
    
    UINT64 qword_ptr_derf = (UINT64)(uDataPtr)-0xA;
    qword_ptr_derf = (UINT64)qword_ptr_derf + *(PINT)((PBYTE)qword_ptr_derf + 3) + 7;

    KeAttachProcess(pwinlogon);
    
    *(PVOID*)&NtUserCreateDesktopEx = InterlockedExchangePointer((PVOID*)qword_ptr_derf, (PVOID)Handler);

    KeDetachProcess();

    return 0x000000123L;
}