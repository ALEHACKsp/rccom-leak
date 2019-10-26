#include <windows.h>
#include <vector>
#include <cstdint>
#include "offsets.h"
#include <iostream>
#define QWORD unsigned long long
#define PI 3.14159265358979323846f
constexpr float d2r = 0.01745329251f;
constexpr float r2d = 57.2957795131f;
#include "..\vector.h"//probably we calling vectors twice
#include "..\core\kernel.h"
#include "..\util\process.h"
//its better now cuz it was inside the vector.h
class r6manager
{
private:
	void* m_driver_control;			// driver control function Where ist the menu I mean keybinds
	unsigned long m_pid;			// process id
	uintptr_t m_game_manager;		// address of R6GameManager* (as integer for arithmetic)
public:
	uintptr_t base;				// game base (as integer for arithmetic)
	float vertical = 0;
	float horizontal = 0;
	int looking = 0;

	r6manager( ) { }
	bool verify_game( ); 

	template<typename T>
	class TArray
	{
	public:
		T* Buffer;
		WORD Size;
	};
	template<typename T>
	T read(uint64_t address)
	{
		T buffer = NULL;

		if (!this->m_pid)
			return buffer;

		MEMORY_STRUCT memory_struct = { 0 };
		memory_struct.process_id = this->m_pid;
		memory_struct.address = reinterpret_cast<void*>(address);
		memory_struct.size = sizeof(T);
		memory_struct.buffer = &buffer;

		NTSTATUS result
			= (NTSTATUS)(call_driver_control(this->m_driver_control, ID_READ_PROCESS_MEMORY, &memory_struct));

		if (result != 0)
			return NULL;

		return buffer;
	}
	template<typename T>
	bool write( uint64_t address, T buffer )
	{
		MEMORY_STRUCT memory_struct	= {0};
		memory_struct.process_id	= this->m_pid;
		memory_struct.address		= reinterpret_cast<void*>( address );
		memory_struct.size			= sizeof( T );
		memory_struct.buffer		= &buffer;

		NTSTATUS result
			= ( NTSTATUS )( call_driver_control( this->m_driver_control, ID_WRITE_PROCESS_MEMORY, &memory_struct ) );

		if ( result != 0 )
			return false;

		return true;
	}
	WORD get_player_count( )
	{
		uint64_t game_manager = read<uint64_t>(base + OFFSET_GAME_MANAGER);
		//std::cout << "\n Game Manager:" << game_manager;
		if (!game_manager)
		{
			return FALSE;
		}
		WORD count = read<WORD>(game_manager + 0x1C8 + 0x8);
		//std::cout << "\n Max Entities:" << max_entities;

		if (!count) {
			
			return FALSE;
		}
		return count;
	}
	uint64_t get_player_by_id( unsigned int i )
	{
		uint64_t game_manager = read<uint64_t>(base + OFFSET_GAME_MANAGER);
		uint64_t entity_list = read<uint64_t>(game_manager + 0x1C8);
		uint64_t entity = read<uint64_t>(entity_list + i * sizeof(uint64_t));
		//std::cout << "\n Entity:" << entity;
		uint64_t entity_info = read<uint64_t>(entity + 0x28);
		//std::cout << "\n Entity info:" << entity_info;
			
		return entity_info;
	} 
	WORD get_player_team(uint64_t player)
	{
		if (!player)			
			return 0xFF;


		uint64_t replication = read<uint64_t>(player + 0x28);
		replication = read<uint64_t>(player + 0xA8);

		if (!replication)
			return 0xFF;

		WORD online_team_id = read<uint64_t>(replication + 0x19E);

		return online_team_id;
	}
	uint64_t get_local_player()
	{
		uint64_t status_manager = read<uint64_t>( base + OFFSET_PLAYER_MANAGER);

		if ( !status_manager )
			return NULL;

		uint64_t entity_container = read<uint64_t>( status_manager + 0x68);

		if ( !entity_container )
			return NULL;

		entity_container = read<uint64_t>( entity_container + 0x0);

		if ( !entity_container )
			return NULL;

		return read<uint64_t>( entity_container + 0x28);
	}
	bool get_enemy_players( std::vector<uint64_t>& players )
	{
		uint64_t local_player = get_local_player( );
		if ( !_VALID( local_player ) )
			return FALSE;

		unsigned short local_team = get_player_team(local_player);

		unsigned int count = get_player_count();
		

		if ( count > 261 )
			return false;

		for ( unsigned int i = 0; i < count; i++ )
		{
			uint64_t target_player = get_player_by_id( i );

			if ( !_VALID( target_player ) )
				continue;

			if ( target_player == local_player )
				continue;

			if ( get_player_team( target_player ) == local_team )
				continue;
			std::cout << target_player << "\n";

			players.push_back( target_player );
		}

		return true;
	}
	bool glow() //glow
	{
		float g = 1;
		

		float R, G, B;
		R = 255;
		G = 255;
		B = 255;

		uint64_t glowbase = read<uint64_t>(base + OFFSET_ENVIRONMENT_AREA);
		if (!glowbase)
			return false;

		uint64_t glowbase1 = read<uint64_t>(glowbase + 0xB8);
		write<float>(glowbase1 + 0x118, R);
		write<float>(glowbase1 + 0x11C, G);
		write<float>(glowbase1 + 0x120, B);
		
		write<float>(glowbase1 + 0x138, g);
		write<float>(glowbase1 + 0x130, g);
		
		write<float>(glowbase1 + 0x130 + 0x4, g);


		return true;
	}
	
};
