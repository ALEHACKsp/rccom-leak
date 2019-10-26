#pragma once;
#include <ntifs.h>
#include <ntimage.h>
#include <ntstrsafe.h>

#include "module.h"
#include "..\util\defines.h"

#define INRANGE( x, a, b )	( x >= a && x <= b )
#define GETBITS( x )		( INRANGE( x, '0', '9' ) ? ( x - '0' ) : ( ( x & ( ~0x20 ) ) - 'A' + 0xa ) )
#define GETBYTE( x )		( GETBITS( x[0] ) << 4 | GETBITS( x[1] ) )

typedef unsigned char BYTE;
typedef BYTE* PBYTE;

typedef unsigned short WORD;
typedef WORD* PWORD;

PVOID find_pattern( LPCWSTR module, LPCSTR pattern );
uintptr_t match_pattern( uintptr_t base, uintptr_t size, const char* pattern );
uintptr_t parse_offset( PVOID address, ULONG skip_bytes );