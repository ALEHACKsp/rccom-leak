#include <ntddk.h>
#include <windef.h>
#include <ntifs.h>
#include <ntstrsafe.h>
#define IO_CHECKDEBUG              CTL_CODE(FILE_DEVICE_UNKNOWN,0x809,METHOD_BUFFERED,FILE_SPECIAL_ACCESS)//? From user mod to kernel mod
#define DebugPort                  0x420 //: Ptr64 Void
#define BeingDebugged              0x002 //: UChar
#define PEB_Offset                 0x3F8

#define IO_READ              0x1
#define IO_WRITE             0x2

NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);

NTSTATUS KeReadMemory(PEPROCESS CurrentProcess, PEPROCESS TargetProcess, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
{
	PSIZE_T Bytes = 0;
	if (NT_SUCCESS(MmCopyVirtualMemory(TargetProcess, SourceAddress, CurrentProcess, TargetAddress, Size, KernelMode, &Bytes)))
	{
		return STATUS_SUCCESS;
	}
	else
	{
		return STATUS_ACCESS_DENIED;
	}
}
NTSTATUS KeWriteMemory(PEPROCESS CurrentProcess, PEPROCESS TargetProcess, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
{
	PSIZE_T Bytes = 0;
	if (NT_SUCCESS(MmCopyVirtualMemory(CurrentProcess, SourceAddress, TargetProcess, TargetAddress, Size, KernelMode, &Bytes)))
	{
		return STATUS_SUCCESS;
	}
	else
	{
		return STATUS_ACCESS_DENIED;
	}
}

UNICODE_STRING DeviceName;
UNICODE_STRING DosDevice;
PDEVICE_OBJECT DeviceObject;

typedef struct _KERNEL_REQUEST
{
	PVOID BeingDebuggedFlag;
	PVOID DebugPortValue;
}*PKERNEL_REQUEST;

typedef struct _KERNEL_REQUEST_TEST
{
	USHORT              CtlCode;
	HANDLE              CurrentPID;
	HANDLE              TargetPID;
	PVOID               Address;
	PVOID               Buffer;
	PVOID               Response;//? Using only for IO_GETMODULEINFO
	ULONG               Size;
}*PKERNEL_REQUEST_TEST;

NTSTATUS NTAPI IoCreateDriver(IN PUNICODE_STRING DriverName 	OPTIONAL, IN PDRIVER_INITIALIZE 	InitializationFunction);
NTSTATUS DefaultRequest(PDEVICE_OBJECT DeviceObj, PIRP Irp)
{
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IofCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS DispatchHandler(PDEVICE_OBJECT device_object, PIRP irp)
{
	NTSTATUS         status = STATUS_SUCCESS;
	PEPROCESS        CurrentProc = NULL;
	PEPROCESS        TargetProc = NULL;
	PRKAPC_STATE     state = NULL;
	PKERNEL_REQUEST_TEST Request = irp->AssociatedIrp.SystemBuffer;
	PsLookupProcessByProcessId(Request->CurrentPID, &CurrentProc);
	PsLookupProcessByProcessId(Request->TargetPID, &TargetProc);
	switch (Request->CtlCode)
	{
	case IO_READ://? Read
		status = KeReadMemory(CurrentProc, TargetProc, Request->Address, Request->Buffer, Request->Size);
		if (status == STATUS_ACCESS_DENIED)
		{
			BYTE Error = 0x5;
			memcpy(Request->Buffer, &Error, sizeof(BYTE));
		}
		break;
	case IO_WRITE://? Write
		status = KeWriteMemory(CurrentProc, TargetProc, Request->Buffer, Request->Address, Request->Size);
		if (status == STATUS_ACCESS_DENIED)
		{
			BYTE Error = 0x5;
			memcpy(Request->Buffer, &Error, sizeof(BYTE));
		}
		break;
	}
	irp->IoStatus.Information = 0;
	irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS MainEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	RtlInitUnicodeString(&DeviceName, L"\\Device\\Qdeycn");
	RtlInitUnicodeString(&DosDevice, L"\\DosDevices\\Qdeycn");
	status = IoCreateDevice(DriverObject, NULL, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
	if (NT_SUCCESS(status))
	{
#ifdef DEBUGPRINT
		DbgPrint("\r\n Successfully create Device Object \r\n");
#endif
	}
	else
	{
#ifdef DEBUGPRINT
		DbgPrint("\r\n Error create Device Object \r\n");
#endif
		return status;
	}
	DeviceObject->Flags |= DO_BUFFERED_IO;
	DeviceObject->Flags &= (~DO_DEVICE_INITIALIZING);
	status = IoCreateUnprotectedSymbolicLink(&DosDevice, &DeviceName);
	if (status == STATUS_SUCCESS)
	{
#ifdef DEBUGPRINT
		DbgPrint("\r\n Succesfully create symbolic link \r\n");
#endif
	}
	else
	{
#ifdef DEBUGPRINT
		DbgPrint("\r\n Error create symbolic link \r\n");
#endif
		return status;
	}
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
		DriverObject->MajorFunction[i] = DefaultRequest;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchHandler;
	RtlZeroMemory(&DriverObject->DriverName.Buffer, sizeof(DriverObject->DriverName.Length));
	return STATUS_SUCCESS;
}
char ADictionary[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
wchar_t WDictionary[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
VOID GenerateRandomSymbolsW(wchar_t* Buff, SIZE_T Length)
{
	for (SIZE_T i = 0; i < Length; i++)
	{
		ULONG seed = KeQueryTimeIncrement();
		unsigned int r = RtlRandomEx(&seed) % 27;
		Buff[i] = WDictionary[r];
	}
}
VOID GenerateRandomSymbolsA(char* Buff, SIZE_T Length)
{
	for (SIZE_T i = 0; i < Length; i++)
	{
		ULONG seed = KeQueryTimeIncrement();
		unsigned int r = RtlRandomEx(&seed) % (sizeof(ADictionary) - 2);
		Buff[i] = ADictionary[r];
	}
}
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	WCHAR Buff[20];
	WCHAR DrvName[256];
	memset(&Buff, 0, 19);
	memset(&DrvName, 0, 255);
	GenerateRandomSymbolsW(&Buff, 6);
	RtlStringCchPrintfW(&DrvName, 255, L"\\Driver\\%s", Buff);
	//DrvName[14] = '\0';
	UNICODE_STRING drvName;
	RtlInitUnicodeString(&drvName, &DrvName);
	return IoCreateDriver(&drvName, &MainEntry);
}