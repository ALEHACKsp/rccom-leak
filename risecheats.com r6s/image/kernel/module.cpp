#include "module.h"

PVOID get_system_routine_address( LPCWSTR routine_name )
{
	UNICODE_STRING name;
	RtlInitUnicodeString( &name, routine_name );
	return MmGetSystemRoutineAddress( &name );
}

PVOID get_system_module_export( LPCWSTR module_name, LPCSTR routine_name )
{
	PVOID lpModule = get_system_module_base( module_name );

	if ( !lpModule )
		return NULL;

	return RtlFindExportedRoutineByName( lpModule, routine_name );
}

PVOID get_system_module_base( LPCWSTR module_name )
{
	//lkd > dt nt!_LDR_DATA_TABLE_ENTRY - l 0xffff8f8a`0f25f110
	//	at 0xffff8f8a`0f25f110
	//	-------------------------------------------- -
	//	+ 0x000 InLoadOrderlinks : _LIST_ENTRY[0xffff8f8a`0cee8c90 - 0xffff8f8a`0f25b010]
	//	+ 0x010 InMemoryOrderlinks : _LIST_ENTRY[0xfffff3ae`f4708000 - 0x00000000`00017034]
	//	+ 0x020 InInitializationOrderlinks : _LIST_ENTRY[0x00000000`00000000 - 0xffff8f8a`0f25f290]
	//	+ 0x030 DllBase          : 0xfffff3ae`f4520000 Void
	//	+ 0x038 EntryPoint       : 0xfffff3ae`f4751010 Void
	//	+ 0x040 SizeOfImage      : 0x26d000
	//	+ 0x048 FullDllName : _UNICODE_STRING "\SystemRoot\System32\win32kbase.sys"
	//	+ 0x058 BaseDllName : _UNICODE_STRING "win32kbase.sys"
	//	+ 0x068 FlagGroup : [4]  ""

	PVOID module_base = NULL;

	__try {

		PLIST_ENTRY module_list = reinterpret_cast<PLIST_ENTRY>( get_system_routine_address( L"PsLoadedModuleList" ) );

		if ( !module_list )
			return NULL;

		UNICODE_STRING name;
		RtlInitUnicodeString( &name, module_name );

		//  InLoadOrderlinks.Flink at 0xffff8f8a`0f25f110
		//	-------------------------------------------- -
		//	+ 0x000 InLoadOrderlinks :  [0xffff8f8a`0cee8c90 - 0xffff8f8a`0f25b010]
		//	+ 0x048 FullDllName : _UNICODE_STRING "\SystemRoot\System32\win32kbase.sys"

		for ( PLIST_ENTRY link = module_list; link != module_list->Blink; link = link->Flink )
		{
			LDR_DATA_TABLE_ENTRY* entry = CONTAINING_RECORD( link, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks );

			// DbgPrint( "driver: %ws\n", entry->FullDllName.Buffer );

			if ( RtlEqualUnicodeString( &entry->BaseDllName, &name, TRUE ) )
			{
				module_base = entry->DllBase;
				break;
			}
		}

	} __except ( EXCEPTION_EXECUTE_HANDLER ) {

		module_base = NULL;
	}

	return module_base;
}

PVOID get_ntoskrnl_base( )
{
	ULONG bytes = 0;
	ZwQuerySystemInformation( SystemModuleInformation, 0, bytes, &bytes );

	PRTL_PROCESS_MODULES modules = reinterpret_cast<PRTL_PROCESS_MODULES>( ExAllocatePool( NonPagedPool, bytes ) );
	RtlSecureZeroMemory( modules, bytes );

	// DbgPrint( "> allocate %lu bytes for modules\n", bytes );

	if ( !NT_SUCCESS( ZwQuerySystemInformation( SystemModuleInformation, modules, bytes, &bytes ) ) )
	{
		ExFreePool( modules );
		return nullptr;
	}

	// DbgPrint( "> ntoskrnl: %llx\n", modules->Modules[0].ImageBase );
	// DbgPrint( "> ntoskrnl: %s\n", modules->Modules[0].FullPathName );

	ExFreePool( modules );
	return modules->Modules[0].ImageBase;
}