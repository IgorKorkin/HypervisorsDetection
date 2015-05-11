#include "control_hypervisor.h"


/****************************************************************************/
/*	R	U	N		S	T	O	P		V	M	M		S	T	A	R	T	*/
/****************************************************************************/
NTSTATUS RunVMMCPU0()
{
	NTSTATUS	ntStatus = STATUS_UNSUCCESSFUL;

	ULONG		EntryEFlags = 0;
	ULONG		cr4 = 0;

	ULONG		EntryEAX = 0;
	ULONG		EntryECX = 0;
	ULONG		EntryEDX = 0;
	ULONG		EntryEBX = 0;
	ULONG		EntryESP = 0;
	ULONG		EntryEBP = 0;
	ULONG		EntryESI = 0;
	ULONG		EntryEDI = 0;	


	Log( "Driver Routines" , 0 );
	Log( "---------------" , 0 );
	Log( "   Driver Entry", DriverEntry );
	Log( "   Driver Unload", DriverUnload );
	Log( "   StartVMX", StartVMX );
	Log( "   VMMEntryPoint", VMMEntryPoint );

	//	Check if PAE is enabled.
	//
	__asm
	{
		PUSH	EAX

			_emit	0x0F	// MOV	EAX, CR4
			_emit	0x20
			_emit	0xE0

			MOV		cr4, EAX

			POP		EAX
	}
	if( cr4 & 0x00000020 )
	{
		Log( "******************************" , 0 );
		Log( "Error : PAE must be disabled." , 0 );
		Log( "Add the following to boot.ini:" , 0 );
		Log( "  /noexecute=alwaysoff /nopae" , 0 );
		Log( "******************************" , 0 );
		return STATUS_UNSUCCESSFUL;
	}

	//	Allocate the VMXON region memory.
	//
	pVMXONRegion = (ULONG *)MmAllocateNonCachedMemory( 4096 );
	if( pVMXONRegion == NULL )
	{	
		Log( "ERROR : Allocating VMXON Region memory." , 0 );
		return STATUS_UNSUCCESSFUL;
	}
	Log( "VMXONRegion virtual address" , pVMXONRegion );
	RtlZeroMemory( pVMXONRegion, 4096 );
	PhysicalVMXONRegionPtr = MmGetPhysicalAddress( pVMXONRegion );
	Log( "VMXONRegion physical address" , PhysicalVMXONRegionPtr.LowPart );

	//	Allocate the VMCS region memory.
	//
	pVMCSRegion = (ULONG *)MmAllocateNonCachedMemory( 4096 );
	if( pVMCSRegion == NULL )
	{
		Log( "ERROR : Allocating VMCS Region memory." , 0 );
		MmFreeNonCachedMemory( pVMXONRegion , 4096 );
		return STATUS_UNSUCCESSFUL;
	}
	Log( "VMCSRegion virtual address" , pVMCSRegion );
	RtlZeroMemory( pVMCSRegion, 4096 );
	PhysicalVMCSRegionPtr = MmGetPhysicalAddress( pVMCSRegion );
	Log( "VMCSRegion physical address" , PhysicalVMCSRegionPtr.LowPart );

	//	Allocate stack for the VM Exit Handler.
	//
	FakeStack = ExAllocatePoolWithTag( NonPagedPool , 0x2000, 'kSkF' );
	if( FakeStack == NULL )
	{
		Log( "ERROR : Allocating VM Exit Handler stack memory." , 0 );
		MmFreeNonCachedMemory( pVMXONRegion , 4096 );
		MmFreeNonCachedMemory( pVMCSRegion , 4096 );
		return STATUS_UNSUCCESSFUL;
	}
	Log( "FakeStack" , FakeStack );

	__asm
	{
		CLI 
			MOV		GuestStack, ESP
	}

	Log( "GuestStack" , GuestStack );

	//	Save the state of the architecture.
	//

	ULONG ñEntryEDI = EntryEDI;
	ULONG ñEntryESI = EntryESI;
	ULONG ñEntryEBP = EntryEBP;
	ULONG ñEntryESP = EntryESP;
	ULONG ñEntryEBX = EntryEBX;
	ULONG ñEntryEDX = EntryEDX;
	ULONG ñEntryECX = EntryECX;
	ULONG ñEntryEAX = EntryEAX;

	ULONG ñEntryEFlags = EntryEFlags;

	__asm
	{
		PUSHAD
			POP		EntryEDI
			POP		EntryESI
			POP		EntryEBP
			POP		EntryESP
			POP		EntryEBX
			POP		EntryEDX
			POP		EntryECX
			POP		EntryEAX
			PUSHFD
			POP		EntryEFlags
	}

	Log( "StartVMX>>" , 0 );

	StartVMX( );

	Log( "StartVMX<<" , 0 );

	//	Restore the state of the architecture.

	__asm
	{
		PUSH	EntryEFlags
			POPFD
			PUSH	EntryEAX
			PUSH	EntryECX
			PUSH	EntryEDX
			PUSH	EntryEBX
			PUSH	EntryESP
			PUSH	EntryEBP
			PUSH	EntryESI
			PUSH	EntryEDI
			POPAD
	}

	__asm
	{
		STI 
			MOV		ESP, GuestStack
	}

	Log( "Running on Processor" , KeGetCurrentProcessorNumber() );

	if( ScrubTheLaunch == 1 )
	{
		Log( "ERROR : Launch aborted." , 0 );
		MmFreeNonCachedMemory( pVMXONRegion , 4096 );
		MmFreeNonCachedMemory( pVMCSRegion , 4096 );
		ExFreePoolWithTag( FakeStack, 'kSkF' );
		return STATUS_UNSUCCESSFUL;
	}

	Log( "VM is now executing." , 0 );

	return STATUS_SUCCESS;


}
NTSTATUS StopVMMCPU0()
{
	Log("StopVMMCPU0>", 0);
	ULONG		ExitEFlags = 0;

	ULONG		ExitEAX = 0;
	ULONG		ExitECX = 0;
	ULONG		ExitEDX = 0;
	ULONG		ExitEBX = 0;
	ULONG		ExitESP = 0;
	ULONG		ExitEBP = 0;
	ULONG		ExitESI = 0;
	ULONG		ExitEDI = 0;

	Log( "[vmm-StopVMMCPU0] Active Processor Bitmap  \n", (ULONG)KeQueryActiveProcessors( ) );

	Log( "[vmm-StopVMMCPU0] Disabling VMX mode on CPU 0.\n", 0 );
	KeSetSystemAffinityThread( (KAFFINITY) 0x00000001 );

	if( VMXIsActive )
	{
		__asm
		{
			PUSHAD
				MOV		EAX, 0x12345678

				_emit 0x0F		// VMCALL
				_emit 0x01
				_emit 0xC1

				POPAD
		}
	}

	Log( "[vmm-StopVMMCPU0] Freeing memory regions.\n", 0 );

	MmFreeNonCachedMemory( pVMXONRegion , 4096 );
	MmFreeNonCachedMemory( pVMCSRegion , 4096 );
	ExFreePoolWithTag( FakeStack, 'kSkF' );

	Log( "[vmm-StopVMMCPU0] Driver StopVMMCPU0.\n", 0);
	return STATUS_SUCCESS;
}
/********************************************************************************/
/*	R	U	N		S	T	O	P		V	M	M		F	I	N	I	S	H	*/
/********************************************************************************/