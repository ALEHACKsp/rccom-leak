#pragma once

#include "virtual.h"

NTSTATUS read_virtual_memory( PMEMORY_STRUCT memory_struct )
{
	if ( memory_struct == NULL || memory_struct->buffer == NULL )
	{
		return STATUS_INVALID_PARAMETER_1;
	}

	PEPROCESS source_process = nullptr;
	PEPROCESS target_process = PsGetCurrentProcess( );

	if ( !NT_SUCCESS( PsLookupProcessByProcessId( reinterpret_cast<HANDLE>( memory_struct->process_id ), &source_process ) ) )
	{
		return STATUS_NOT_FOUND;
	}

	SIZE_T size_copied = 0;

	NTSTATUS status =
		MmCopyVirtualMemory( source_process, memory_struct->address, target_process, memory_struct->buffer, memory_struct->size, KernelMode, &size_copied );

	memory_struct->size_copied = size_copied;

	ObDereferenceObject( source_process );
	return status;
}

NTSTATUS write_virtual_memory( PMEMORY_STRUCT memory_struct )
{
	PEPROCESS target_process = nullptr;
	PEPROCESS curent_process = PsGetCurrentProcess( );

	if ( !NT_SUCCESS( PsLookupProcessByProcessId( reinterpret_cast<HANDLE>( memory_struct->process_id ), &target_process ) ) )
	{
		return STATUS_NOT_FOUND;
	}

	SIZE_T size_copied = 0;

	NTSTATUS status = STATUS_SUCCESS;

	__try {
	
		// ProbeForWrite( memory_struct->address, memory_struct->size, sizeof( PVOID ) );
		status = MmCopyVirtualMemory( curent_process, memory_struct->buffer, target_process, memory_struct->address, memory_struct->size, KernelMode, &size_copied );
	
	} __except ( EXCEPTION_EXECUTE_HANDLER ) {
	
		status = STATUS_UNSUCCESSFUL;
	
	}

	memory_struct->size_copied = size_copied;

	ObDereferenceObject( target_process );
	return status;
}

NTSTATUS read_kernel_memory( PVOID address, PVOID buffer, SIZE_T size )
{
	PVOID non_paged_buffer = ExAllocatePool( NonPagedPool, size );

	if ( !non_paged_buffer )
	{
		return STATUS_MEMORY_NOT_ALLOCATED;
	}

	SIZE_T size_copied = 0;

	MM_COPY_ADDRESS mm_address = {0};
	mm_address.VirtualAddress = address;

	NTSTATUS status =
		MmCopyMemory( non_paged_buffer, mm_address, size, MM_COPY_MEMORY_VIRTUAL, &size_copied );

	if ( NT_SUCCESS( status ) )
	{
		RtlCopyMemory( buffer, non_paged_buffer, size_copied );
	}

	ExFreePool( non_paged_buffer );

	return status;
}