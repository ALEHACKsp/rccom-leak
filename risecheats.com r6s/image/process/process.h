#include <ntifs.h>
#include "../util/defines.h"
#include "../kernel/module.h"

PEPROCESS get_process( HANDLE process_id );
PVOID get_process_base( HANDLE process_id );
PVOID get_process_module_base( HANDLE pid, LPCWSTR module_name );
