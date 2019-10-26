#include "functions.h"

bool r6manager::verify_game( )
{
	RECT DesktopRect;
	GetWindowRect(GetDesktopWindow(), &DesktopRect);

	this->horizontal = (float)(DesktopRect.right - DesktopRect.left);
	this->vertical = (float)(DesktopRect.bottom - DesktopRect.top);

	// get the hooked function

	m_driver_control = kernel_control_function( );

	if (!m_driver_control)
	{
		return false;
	}
		

	// get process id

	m_pid = GetProcId( "RainbowSix.exe" );

	if (!m_pid)
	{

		return false;
	}
	// get base address

	base = call_driver_control(
		m_driver_control, ID_GET_PROCESS_BASE, m_pid );

	if ( !base )
		return false;

	// get r6 game manager

	m_game_manager = read<UINT_PTR>( base + OFFSET_GAME_MANAGER );

	if (!m_game_manager)
	{
		return false;
	}
		

	return true;
}