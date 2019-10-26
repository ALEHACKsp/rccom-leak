#pragma once

#include <ntstatus.h>

#define NTSTRSAFE_LIB
#include <ntstrsafe.h>

#include <cstddef>
#include <cstdint>

#include "memory/virtual.h"

#include "process/process.h"

#include "kernel/module.h"
#include "kernel/pattern.h"

#include "util/defines.h"

enum E_COMMAND_CODE
{
	ID_NULL					= 0,	//
	ID_READ_PROCESS_MEMORY	= 5,	// 
	ID_READ_KERNEL_MEMORY	= 6,	// 
	ID_WRITE_PROCESS_MEMORY = 7,	//
	ID_GET_PROCESS			= 10,	//
	ID_GET_PROCESS_BASE		= 11,	//
	ID_GET_PROCESS_MODULE	= 12,	//
};