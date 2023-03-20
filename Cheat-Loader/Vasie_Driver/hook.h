#pragma once
#include <ntifs.h>
#include <windef.h>

#include "util.h"
#include "process.h"

struct _data {
	INT code;

	uintptr_t addr;
	uintptr_t buff;
	SIZE_T size;

	uintptr_t src;
	uintptr_t dest;

	BOOLEAN base;
};

class hook {
public:
	static NTSTATUS init();
};

