#pragma once

#include <windows.h>
#include <stdio.h>

#include <vector>
#include <fstream>
#include <iostream>

#include <fcntl.h>
#include <io.h>

#include "game\functions.h"

#ifdef _DEBUG
	#pragma comment( lib, "kelib/debug/kelib.lib" )
#else
	#pragma comment( lib, "kelib/release/kelib.lib" )
#endif

#pragma comment( linker, "\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"" )

void create_console( );