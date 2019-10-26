#include "process.h"

PEPROCESS get_process( HANDLE process_id )
{
	PEPROCESS process;

	if ( !NT_SUCCESS( PsLookupProcessByProcessId( process_id, &process ) ) )
		return NULL;

	return process;
}

PVOID get_process_base( HANDLE process_id )
{
	PEPROCESS process = get_process( process_id );

	if ( !process )
		return NULL;

	return PsGetProcessSectionBaseAddress( process );
}

PVOID get_process_module_base( HANDLE pid, LPCWSTR module_name )
{
	UNREFERENCED_PARAMETER( module_name );

	PEPROCESS process = nullptr;
	PVOID result = nullptr;

	if ( !NT_SUCCESS( PsLookupProcessByProcessId( pid, &process ) ) )
		return result;

	PPEB64 peb = reinterpret_cast<PPEB64>( PsGetProcessPeb( process ) );

	if ( !peb )
		return result;

	KAPC_STATE state;
	KeStackAttachProcess( process, &state );

	// InLoadOrderLinks will have main executable first, ntdll.dll second, kernel32.dll

	for ( PLIST_ENTRY pListEntry = peb->Ldr->InLoadOrderLinks.Flink; pListEntry != &peb->Ldr->InLoadOrderLinks; pListEntry = pListEntry->Flink )
	{
		if ( !pListEntry )
			continue;

		PLDR_DATA_TABLE_ENTRY module_entry = CONTAINING_RECORD( pListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks );

		UNICODE_STRING unicode_name;
		RtlUnicodeStringInit( &unicode_name, module_name );

		if ( RtlCompareUnicodeString( &module_entry->BaseDllName, &unicode_name, TRUE ) == 0 )
			result = module_entry->DllBase;
	}

	KeUnstackDetachProcess( &state );

	return result;
}