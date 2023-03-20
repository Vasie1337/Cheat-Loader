#include "process.h"

#pragma warning(disable : 4996)

PEPROCESS process::findbypid(uintptr_t pid) {
	PEPROCESS targetProcess = NULL;
	PsLookupProcessByProcessId((HANDLE)pid, &targetProcess);
	
	return targetProcess;
}

PEPROCESS process::findbyname(wchar_t* name) {
	CHAR image_name[15]{};
	PEPROCESS sys_process = PsInitialSystemProcess;
	PEPROCESS cur_entry = sys_process;

	do
	{
		RtlCopyMemory((PVOID)(&image_name), (PVOID)((uintptr_t)cur_entry + 0x5A8), sizeof(image_name));

		if (util::crt_strcmp(image_name, name, true))
		{
			DWORD active_threads;
			RtlCopyMemory((PVOID)&active_threads, (PVOID)((uintptr_t)cur_entry + 0x5F0), sizeof(active_threads));

			if (active_threads)
				return cur_entry;
		}

		PLIST_ENTRY list = (PLIST_ENTRY)((uintptr_t)(cur_entry)+0x448);
		cur_entry = (PEPROCESS)((uintptr_t)list->Flink - 0x448);

	} while (cur_entry != sys_process);

	return 0;
}

uintptr_t process::getimage(wchar_t* name)
{
	ULONG bytes = 0;
	NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);

	if (!bytes) return 0;

	PRTL_PROCESS_MODULES modules = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(NonPagedPool, bytes, 0x504D5448);

	status = ZwQuerySystemInformation(SystemModuleInformation, modules, bytes, &bytes);

	if (!NT_SUCCESS(status)) return 0;

	PRTL_PROCESS_MODULE_INFORMATION module = modules->Modules;
	PVOID module_base = 0, module_size = 0;

	for (ULONG i = 0; i < modules->NumberOfModules; i++)
	{
		if (util::crt_strcmp(module[i].FullPathName, name, true))
		{
			module_base = module[i].ImageBase;
			module_size = (PVOID)module[i].ImageSize;
			break;
		}
	}

	if (modules) ExFreePoolWithTag(modules, 0);
	if (module_base <= 0) return 0;
	return (uintptr_t)module_base;
}

VOID process::getbase(PEPROCESS src, PEPROCESS dest, uintptr_t buf) {
	SIZE_T returnSize = 0;
	uintptr_t uBase = (uintptr_t)PsGetProcessSectionBaseAddress(src);
	MmCopyVirtualMemory(IoGetCurrentProcess(), &uBase, dest, (PVOID)buf, sizeof(uBase), 0, &returnSize);
}

SIZE_T process::read(PEPROCESS src, uintptr_t adr, PEPROCESS dest, uintptr_t buf, SIZE_T size) {
	SIZE_T returnSize = 0;
	MmCopyVirtualMemory(src, (PVOID)adr, dest, (PVOID)buf, size, NULL, &returnSize);

	return returnSize;
}