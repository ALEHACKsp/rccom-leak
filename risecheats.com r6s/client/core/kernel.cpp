#include "kernel.h"
#include <iostream>

void* kernel_control_function( )
{
	HMODULE hModule = LoadLibrary( "win32u.dll" );

	if (!hModule)
	{
		return nullptr;
		std::cout << "\n Driver Not Installed! Error! ";
	}
	

	return reinterpret_cast<void*>( GetProcAddress( hModule, "NtGdiDdDDINetDispQueryMiracastDisplayDeviceStatus" ) );
}

uint64_t read_kernel( void* control_function, uint64_t address, void* buffer, std::size_t size )
{
	return call_driver_control( control_function, ID_READ_KERNEL_MEMORY, address, buffer, size );
}
