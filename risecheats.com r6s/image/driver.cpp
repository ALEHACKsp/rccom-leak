#include "driver.h"

extern "C" __declspec( dllexport ) UINT_PTR DriverControl( ULONG uCode, UINT_PTR uParam1, UINT_PTR uParam2, UINT_PTR uParam3 )
{
	UNREFERENCED_PARAMETER( uParam1 );
	UNREFERENCED_PARAMETER( uParam2 );
	UNREFERENCED_PARAMETER( uParam3 );

	DbgPrint( "control code: %d\n", uCode );

	// Main switch struct

	switch ( uCode )
	{
	case ID_READ_PROCESS_MEMORY:
		return read_virtual_memory(
			reinterpret_cast<PMEMORY_STRUCT>( uParam1 ) ); 

	case ID_READ_KERNEL_MEMORY:	
		return read_kernel_memory(
			reinterpret_cast<PVOID>( uParam1 ), // address
			reinterpret_cast<PVOID>( uParam2 ), // buffer
			uParam3 ); // size

	case ID_WRITE_PROCESS_MEMORY:
		return write_virtual_memory(
			reinterpret_cast<PMEMORY_STRUCT>( uParam1 ) );

	case ID_GET_PROCESS:
		return reinterpret_cast<UINT_PTR>( get_process(
			reinterpret_cast<HANDLE>( uParam1 ) ) ); // process id
		 
	case ID_GET_PROCESS_BASE:
		return reinterpret_cast<UINT_PTR>( get_process_base(
			reinterpret_cast<HANDLE>( uParam1 ) ) ); // process id

	case ID_GET_PROCESS_MODULE:
		return reinterpret_cast<UINT_PTR>(
			get_process_module_base(
				reinterpret_cast<HANDLE>( uParam1 ), // process id
				L"kernel32.dll" // module name
			) ); 

	default:
		break;
	}

	return 0;
}

NTSTATUS DriverEntry( struct _DRIVER_OBJECT* DriverObject, PUNICODE_STRING RegistryPath )
{
	UNREFERENCED_PARAMETER( DriverObject );
	UNREFERENCED_PARAMETER( RegistryPath );

	// this function will be hooked in kernel to point to the control routine above

	UINT_PTR hook_function
		= reinterpret_cast<UINT_PTR>( get_system_module_export( L"win32kbase.sys", "NtGdiDdDDINetDispQueryMiracastDisplayDeviceStatus" ) );

	DbgPrint( "> hook_function %llx\n", hook_function );

	// parse offset 

	// .text : 00000001C00DA5B0 48 83 EC 28				sub     rsp, 28h
	// .text : 00000001C00DA5B4 48 8B 05 AD 8E 0D 00	mov     rax, cs : qword_1C01B3468;

	UINT_PTR hook_pointer = *( DWORD *)( hook_function + 7 );
	hook_pointer += 0xB;
	hook_pointer += hook_function;

	DbgPrint( "> hook_pointer offset %llx\n", hook_pointer );
	//
	//memcpy( reinterpret_cast<PVOID>( hook_pointer ), &DriverControl, sizeof( PVOID ) );

	//DbgPrint( "> hooked\n" );

	return STATUS_SUCCESS;
}
