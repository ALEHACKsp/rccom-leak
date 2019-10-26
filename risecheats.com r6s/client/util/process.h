#pragma once

#include <windows.h>
#include <string>
#include <tlhelp32.h>
#include <cstdint>

DWORD GetProcId( LPCSTR lpExeName );
HWND GetProcessWindow( DWORD pid );