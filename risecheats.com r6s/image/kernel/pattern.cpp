#include "pattern.h"

// Usage:
// find_pattern( L"ntoskrnl.exe", "89 5C 24 20 48 8B 0D" );

PVOID find_pattern( LPCWSTR module, LPCSTR pattern )
{
	PVOID image_base_address = get_system_module_base( module );

	if ( !image_base_address )
		return nullptr;

	PIMAGE_NT_HEADERS64 nt_header = RtlImageNtHeader( image_base_address );

	if ( !nt_header )
		return nullptr;

	PIMAGE_SECTION_HEADER first_section = reinterpret_cast<PIMAGE_SECTION_HEADER>( nt_header + 1 );

	for ( PIMAGE_SECTION_HEADER section = first_section; section < first_section + nt_header->FileHeader.NumberOfSections; section++ )
	{
		if ( section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE )
			continue;

		if ( section->Characteristics & IMAGE_SCN_CNT_CODE &&
			 section->Characteristics & IMAGE_SCN_MEM_EXECUTE )
		{
			// DbgPrint( "[+] scanning section: %llx size: %llx\n", section->VirtualAddress, section->SizeOfRawData );

			uintptr_t section_base_address = reinterpret_cast<uintptr_t>( image_base_address ) + section->VirtualAddress;
			uintptr_t section_size = section->SizeOfRawData;

			__try
			{
				uintptr_t instance = match_pattern( section_base_address, section_size, pattern );

				if ( instance )
				{
					return reinterpret_cast<PVOID>( instance );
				}

			} __except ( EXCEPTION_EXECUTE_HANDLER )
			{

			}

		}
	}

	return nullptr;
}

uintptr_t match_pattern( uintptr_t base, uintptr_t size, const char* pattern )
{
	const unsigned char* pattern_offset = reinterpret_cast<const unsigned char*>( pattern );

	uintptr_t first_match = 0;
	uintptr_t range = base + size;

	for ( uintptr_t current_offset = base; current_offset < range; ++current_offset )
	{
		if ( *const_cast<PBYTE>( pattern_offset ) == static_cast<BYTE>( '\?' ) || *reinterpret_cast<BYTE*>( current_offset ) == GETBYTE( pattern_offset ) )
		{
			if ( !first_match )
				first_match = current_offset;

			if ( !pattern_offset[2] )
				return first_match;

			// add: skip ?, ?? wild cards

			pattern_offset += ( *( PWORD )pattern_offset == ( WORD )'\?\?' || *( PBYTE )pattern_offset != ( BYTE )'\?' ) ? 3 : 2;

			if ( !*pattern_offset )
				return first_match;

		} else if ( first_match ) {

			current_offset = first_match;
			pattern_offset = reinterpret_cast<const unsigned char*>( pattern );
			first_match = 0;

		}
	}

	return NULL;
}

// copies a 32-bit offset value from an instruction

// from this:     48 8B 0D[01 8D 35 00]		mov rcx, cs : lpUnknown;
//   to this:              01 8D 35 00
// 
// adds original address

uintptr_t parse_offset( PVOID address, ULONG skip_bytes )
{
	uintptr_t source_operand
		 = reinterpret_cast<uintptr_t>( address ) + skip_bytes;

	DbgPrint( "> offset: %llx\n", *reinterpret_cast<ULONG*>( source_operand ) );

	return reinterpret_cast<uintptr_t>( address ) + *reinterpret_cast<ULONG*>( source_operand );
}
