#ifndef DEFINES_H
#define DEFINES_H

#include <ntifs.h>

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	// ...
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _PEB_LDR_DATA
{
	unsigned int Length;
	int Initialized;
	void* SSHandle;
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	// ...
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB64
{
	unsigned char InheritedAddressSpace;	// 0x0000 
	unsigned char ReadImageFileExecOptions;	// 0x0001 
	unsigned char BeingDebugged;			// 0x0002 
	unsigned char BitField;					// 0x0003 
	unsigned char pad_0x0004[0x4];			// 0x0004
	PVOID Mutant;							// 0x0008 
	PVOID ImageBaseAddress;					// 0x0010 
	PPEB_LDR_DATA Ldr;						// 0x0018
	// ...
} PEB64, *PPEB64;

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR  FullPathName[MAXIMUM_FILENAME_LENGTH];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemModuleInformation = 0xb,
} SYSTEM_INFORMATION_CLASS;

extern "C" __declspec( dllimport ) POBJECT_TYPE* IoDriverObjectType;
extern "C" __declspec( dllimport ) PLIST_ENTRY	 PsLoadedModuleList;

extern "C" __declspec( dllimport )
NTSTATUS NTAPI ObReferenceObjectByName(
	PUNICODE_STRING ObjectName,
	ULONG Attributes,
	PACCESS_STATE AccessState,
	ACCESS_MASK DesiredAccess,
	POBJECT_TYPE ObjectType,
	KPROCESSOR_MODE AccessMode,
	PVOID ParseContext OPTIONAL,
	PVOID* Object
);

extern "C" __declspec( dllimport )
NTSTATUS NTAPI MmCopyVirtualMemory(
	PEPROCESS FromProcess,
	PVOID FromAddress,
	PEPROCESS ToProcess,
	PVOID ToAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T NumberOfBytesCopied
);

extern "C" __declspec( dllimport )
PVOID NTAPI PsGetProcessSectionBaseAddress(
	PEPROCESS Process
);

extern "C" __declspec( dllimport )
PIMAGE_NT_HEADERS NTAPI RtlImageNtHeader(
	PVOID Base
);

extern "C" __declspec( dllimport )
PVOID NTAPI RtlFindExportedRoutineByName(
	PVOID ImageBase,
	PCCH RoutineName
);

extern "C" __declspec( dllimport )
ULONG RtlRandomEx(
	PULONG Seed
);

extern "C" __declspec( dllimport )
PPEB NTAPI PsGetProcessPeb(
	PEPROCESS Process
);

extern "C" __declspec( dllimport )
PEPROCESS NTAPI PsGetCurrentProcessByThread(
	PETHREAD Thread
);

extern "C" __declspec( dllimport )
NTSTATUS NTAPI ZwQuerySystemInformation(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
);

#endif