#pragma once

#include <ntifs.h>
#include <cstddef>
#include <cstdint>

#include "..\util\defines.h"

typedef struct _MEMORY_STRUCT
{
	UINT_PTR	process_id;
	PVOID		address;
	SIZE_T		size;
	SIZE_T		size_copied;
	PVOID		buffer;
} MEMORY_STRUCT, *PMEMORY_STRUCT;

NTSTATUS read_virtual_memory( PMEMORY_STRUCT memory_struct );
NTSTATUS write_virtual_memory( PMEMORY_STRUCT memory_struct );
NTSTATUS read_kernel_memory( PVOID address, PVOID buffer, SIZE_T size );