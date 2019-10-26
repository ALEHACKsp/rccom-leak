#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include "client.h"
#include "./vars.h"
#include "./inireader.h"
#include "./iniwriter.h"
#pragma warning(disable:4996)


#define BUFFER 8192


DWORD WINAPI main_thread( LPVOID )
{
			AllocConsole();
			freopen("conin$", "r", stdin);
			freopen("conout$", "w", stdout);
			freopen("conout$", "w", stderr);


		r6manager game;
		while (TRUE)
		{




			if (GetAsyncKeyState(VK_F1))
			{
				if (!game.verify_game())
					continue;
				game.glow();
				printf("\n [+] GLOW = ON ");
				Sleep(100);
			}

			return 0;
		}
};
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
			CreateThread(NULL, 0, main_thread, NULL, NULL, NULL);
			while ((GetAsyncKeyState(VK_NUMLOCK) & 1) == 0)
				Sleep(100);
			return 0;
			
}
void create_console()
{
	if ( !AllocConsole( ) )
	{
		char buffer[1024] = {0};
		sprintf_s( buffer, "Failed to AllocConsole( ), GetLastError( ) = %d", GetLastError( ) );
		MessageBox( HWND_DESKTOP, buffer, "Error", MB_OK );

		return;
	}
	auto lStdHandle = GetStdHandle( STD_OUTPUT_HANDLE );
	auto hConHandle = _open_osfhandle( PtrToUlong( lStdHandle ), _O_TEXT );
	auto fp = _fdopen( hConHandle, "w" );
	freopen_s( &fp, "CONOUT$", "w", stdout );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
}