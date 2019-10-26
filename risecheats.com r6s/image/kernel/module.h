#include <ntifs.h>
#include <ntstrsafe.h>

#include "..\util\defines.h"

PVOID get_system_routine_address( LPCWSTR routine_name );
PVOID get_system_module_export( LPCWSTR module_name, LPCSTR routine_name );
PVOID get_system_module_base( LPCWSTR module_name );
PVOID get_ntoskrnl_base( );