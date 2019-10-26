#include "process.h"


DWORD GetProcId( LPCSTR lpExeName )
{
	
	HANDLE hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	if ( hSnapShot == INVALID_HANDLE_VALUE )
		return NULL;

	PROCESSENTRY32 pe = {0};
	pe.dwSize = sizeof( PROCESSENTRY32 );

	for ( BOOL	success = Process32First( hSnapShot, &pe );
		success == TRUE;
		success = Process32Next( hSnapShot, &pe ) )
	{
		if ( strcmp( lpExeName, pe.szExeFile ) == 0 )
		{
			CloseHandle( hSnapShot );
			return pe.th32ProcessID;
		}
	}

	CloseHandle( hSnapShot );
	return NULL;
}

HWND GetProcessWindow( DWORD pid )
{
	std::pair<HWND, DWORD> params = {0, pid};

	BOOL bResult = EnumWindows( [] ( HWND hwnd, LPARAM lParam ) -> BOOL
	{
		auto pParams = ( std::pair<HWND, DWORD>* )( lParam );

		DWORD processId;

		if ( GetWindowThreadProcessId( hwnd, &processId ) && processId == pParams->second )
		{
			SetLastError( (DWORD)-1 );
			pParams->first = hwnd;
			return FALSE;
		}

		return TRUE;

	}, (LPARAM)&params );

	if ( !bResult && GetLastError( ) == -1 && params.first )
		return params.first;

	return NULL;
}