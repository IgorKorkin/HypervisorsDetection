/************************************************************************/
/*                                                                      */
/************************************************************************/

#include "hypervisor_dispatcher.h"

ULONG		ExitReason;
ULONG		ExitQualification;
ULONG		ExitInterruptionInformation;
ULONG		ExitInterruptionErrorCode;
ULONG		IDTVectoringInformationField;
ULONG		IDTVectoringErrorCode;
ULONG		ExitInstructionLength;
ULONG		ExitInstructionInformation;

ULONG		GuestEIP;
ULONG		GuestResumeEIP;
ULONG		GuestESP;
ULONG		GuestCS;
ULONG		GuestCR0;
ULONG		GuestCR3;
ULONG		GuestCR4;
ULONG		GuestEFLAGS;

ULONG		GuestEAX;
ULONG		GuestEBX;
ULONG		GuestECX;
ULONG		GuestEDX;
ULONG		GuestEDI;
ULONG		GuestESI;
ULONG		GuestEBP;

ULONG		movcrControlRegister;
ULONG		movcrAccessType;
ULONG		movcrOperandType;
ULONG		movcrGeneralPurposeRegister;
ULONG		movcrLMSWSourceData;

//////////////////////////////////////////////////////////////////////////

///////////////////////
//                   //
//  VMM Entry Point  //
//                   //
///////////////////////
__declspec( naked ) VOID VMMEntryPoint( )
{

	__asm	CLI	

		
	__asm	PUSHAD

	

	//
	//	Record the General-Purpose registers.
	//
	__asm	MOV GuestEAX, EAX
	__asm	MOV GuestEBX, EBX
	__asm	MOV GuestECX, ECX
	__asm	MOV GuestEDX, EDX
	__asm	MOV GuestEDI, EDI
	__asm	MOV GuestESI, ESI
	__asm	MOV GuestEBP, EBP

	SPYRDTSÑ();
		
		

	///////////////////
	//               //
	//  Exit Reason  //		0x00004400
	//               //
	///////////////////
	__asm
	{
		PUSHAD

			MOV		EAX, 0x00004402

			_emit	0x0F	// VMREAD  EBX, EAX
			_emit	0x78
			_emit	0xC3

			MOV		ExitReason, EBX

			POPAD
	}


	if( ExitReason == 0x0000000A ||	// CPUID
		ExitReason == 0x00000010 || // RDTSC
		ExitReason == 0x00000012 || // VMCALL
		ExitReason == 0x0000001C || // MOV CR
		ExitReason == 0x0000001F || // RDMSR
		ExitReason == 0x00000020 ||	// WRMSR		
		( ExitReason > 0x00000012 && ExitReason < 0x0000001C ) )
	{
		HandlerLogging = 0;
	}
	else
	{
		HandlerLogging = 1;
	}


	if( HandlerLogging )
	{
		Log( "----- VMM Handler CPU0 -----", 0 );
		Log( "Guest EAX" , GuestEAX );
		Log( "Guest EBX" , GuestEBX );
		Log( "Guest ECX" , GuestECX );
		Log( "Guest EDX" , GuestEDX );
		Log( "Guest EDI" , GuestEDI );
		Log( "Guest ESI" , GuestESI );
		Log( "Guest EBP" , GuestEBP );
		Log( "Exit Reason" , ExitReason );
	}

	//////////////////////////
	//                      //
	//  Exit Qualification  //	00006400H
	//                      //
	//////////////////////////
	__asm
	{
		PUSHAD

			MOV		EAX, 0x00006400

			_emit	0x0F	// VMREAD  EBX, EAX
			_emit	0x78
			_emit	0xC3

			MOV		ExitQualification, EBX

			POPAD
	}
	if( HandlerLogging ) Log( "Exit Qualification" , ExitQualification );

	////////////////////////////////////////
	//                                    //
	//  VM-Exit Interruption Information  //	00004404H
	//                                    //
	////////////////////////////////////////
	__asm
	{
		PUSHAD

			MOV		EAX, 0x00004404

			_emit	0x0F	// VMREAD  EBX, EAX
			_emit	0x78
			_emit	0xC3

			MOV		ExitInterruptionInformation, EBX

			POPAD
	}
	if( HandlerLogging ) Log( "Exit Interruption Information" , ExitInterruptionInformation );

	///////////////////////////////////////
	//                                   //
	//  VM-Exit Interruption Error Code  //	00004406H
	//                                   //
	///////////////////////////////////////
	__asm
	{
		PUSHAD

		MOV		EAX, 0x00004406

		_emit	0x0F	// VMREAD  EBX, EAX
		_emit	0x78
		_emit	0xC3

		MOV		ExitInterruptionErrorCode, EBX

		POPAD
	}
	if( HandlerLogging ) Log( "Exit Interruption Error Code" , ExitInterruptionErrorCode );

	///////////////////////////////////////
	//                                   //
	//  IDT-Vectoring Information Field  //	00004408H
	//                                   //
	///////////////////////////////////////
	__asm
	{
		PUSHAD

		MOV		EAX, 0x00004408

		_emit	0x0F	// VMREAD  EBX, EAX
		_emit	0x78
		_emit	0xC3

		MOV		IDTVectoringInformationField, EBX

		POPAD
	}
	if( HandlerLogging ) Log( "IDT-Vectoring Information Field" , IDTVectoringInformationField );

	////////////////////////////////
	//                            //
	//  IDT-Vectoring Error Code  //	0000440AH
	//                            //
	////////////////////////////////
	__asm
	{
		PUSHAD

		MOV		EAX, 0x0000440A

		_emit	0x0F	// VMREAD  EBX, EAX
		_emit	0x78
		_emit	0xC3

		MOV		IDTVectoringErrorCode, EBX

		POPAD
	}
	if( HandlerLogging ) Log( "IDT-Vectoring Error Code" , IDTVectoringErrorCode );

	//////////////////////////////////
	//                              //
	//  VM-Exit Instruction Length  //	0000440CH
	//                              //
	//////////////////////////////////
	__asm
	{
		PUSHAD

		MOV		EAX, 0x0000440C

		_emit	0x0F	// VMREAD  EBX, EAX
		_emit	0x78
		_emit	0xC3

		MOV		ExitInstructionLength, EBX

		POPAD
	}
	if( HandlerLogging ) Log( "VM-Exit Instruction Length" , ExitInstructionLength );

	///////////////////////////////////////
	//                                   //
	//  VM-Exit Instruction Information  //	0000440EH
	//                                   //
	///////////////////////////////////////
	__asm
	{
		PUSHAD

		MOV		EAX, 0x0000440E

		_emit	0x0F	// VMREAD  EBX, EAX
		_emit	0x78
		_emit	0xC3

		MOV		ExitInstructionInformation, EBX

		POPAD
	}
	if( HandlerLogging ) Log( "VM-Exit Instruction Information" , ExitInstructionInformation );

	/////////////////
	//             //
	//  Guest EIP  //
	//             //
	/////////////////
	__asm
	{
		PUSHAD

		MOV		EAX, 0x0000681E

		_emit	0x0F	// VMREAD  EBX, EAX
		_emit	0x78
		_emit	0xC3

		MOV		GuestEIP, EBX

		POPAD
	}
	if( HandlerLogging ) Log( "VM Exit EIP" , GuestEIP );

	//
	//	Writing the Guest VMCS EIP uses general registers.
	//	Must complete this before setting general registers
	//	for guest return state.
	//
	GuestResumeEIP = GuestEIP + ExitInstructionLength;
	WriteVMCS( 0x0000681E, (ULONG)GuestResumeEIP );

	/////////////////
	//             //
	//  Guest ESP  //
	//             //
	/////////////////
	__asm
	{
		PUSHAD

		MOV		EAX, 0x0000681C

		_emit	0x0F	// VMREAD  EBX, EAX
		_emit	0x78
		_emit	0xC3

		MOV		GuestESP, EBX

		POPAD
	}
	if( HandlerLogging ) Log( "VM Exit ESP" , GuestESP );

	////////////////
	//            //
	//  Guest CS  //
	//            //
	////////////////
	__asm
	{
		PUSHAD

		MOV		EAX, 0x00000802

		_emit	0x0F	// VMREAD  EBX, EAX
		_emit	0x78
		_emit	0xC3

		MOV		GuestCS, EBX

		POPAD
	}
	if( HandlerLogging ) Log( "VM Exit CS" , GuestCS );

	/////////////////
	//             //
	//  Guest CR0  //
	//             //
	/////////////////
	__asm
	{
		PUSHAD

		MOV		EAX, 0x00006800

		_emit	0x0F	// VMREAD  EBX, EAX
		_emit	0x78
		_emit	0xC3

		MOV		GuestCR0, EBX

		POPAD
	}
	if( HandlerLogging ) Log( "VM Exit CR0" , GuestCR0 );

	/////////////////
	//             //
	//  Guest CR3  //
	//             //
	/////////////////
	__asm
	{
		PUSHAD

		MOV		EAX, 0x00006802

		_emit	0x0F	// VMREAD  EBX, EAX
		_emit	0x78
		_emit	0xC3

		MOV		GuestCR3, EBX

		POPAD
	}
	if( HandlerLogging ) Log( "VM Exit CR3" , GuestCR3 );

	/////////////////
	//             //
	//  Guest CR4  //
	//             //
	/////////////////
	__asm
	{
		PUSHAD

		MOV		EAX, 0x00006804

		_emit	0x0F	// VMREAD  EBX, EAX
		_emit	0x78
		_emit	0xC3

		MOV		GuestCR4, EBX

		POPAD
	}
	if( HandlerLogging ) Log( "VM Exit CR4" , GuestCR4 );

	////////////////////
	//                //
	//  Guest EFLAGS  //
	//                //
	////////////////////
	__asm
	{
		PUSHAD

		MOV		EAX, 0x00006820

		_emit	0x0F	// VMREAD  EBX, EAX
		_emit	0x78
		_emit	0xC3

		MOV		GuestEFLAGS, EBX

		POPAD
	}
	if( HandlerLogging ) Log( "VM Exit EFLAGS" , GuestEFLAGS );

	/////////////////////////////////////////////
	//                                         //
	//  *** EXIT REASON CHECKS START HERE ***  //
	//                                         //
	/////////////////////////////////////////////


	// goto Exit; 


	/////////////////////////////////////////////////////////////////////////////////////
	//                                                                                 //
	//  VMCLEAR, VMLAUNCH, VMPTRLD, VMPTRST, VMREAD, VMWRITE, VMRESUME, VMXOFF, VMXON  //
	//                                                                                 //
	/////////////////////////////////////////////////////////////////////////////////////
	if( ExitReason > 0x00000012 && ExitReason < 0x0000001C )
	{
		Log( "Request has been denied - CPU0", ExitReason );

		if (ExitReason == 0x0000001B) /* VMXON */
		{			
			PATCHRDTSC();
		}

		__asm
		{
			POPAD
			JMP		Resume
		}
	}

	//////////////
	//          //
	//  RDTSC   //
	//          //
	//////////////
	if( ExitReason == 0x00000010 )
	{
		Log( "RDTSC detected - CPU0" , 0 );
		__asm
		{		
			POPAD
			RDTSC 
			JMP		Resume
		}
	}



	//////////////
	//          //
	//  VMCALL  //
	//          //
	//////////////
	if( ExitReason == 0x00000012 )
	{
		Log( "VMCALL detected - CPU0" , 0 );

		if( GuestEAX == 0x12345678 )
		{
			//	Switch off VMX mode.
			//
			Log( "- Terminating VMX Mode.", 0xDEADDEAD );
			__asm
			{
				_emit	0x0F	// VMXOFF
				_emit	0x01
				_emit	0xC4
			}

			Log( "- Flow Control Return to Address" , GuestResumeEIP );

			__asm
			{
				POPAD
				MOV	ESP, GuestESP
				STI
				JMP	GuestResumeEIP
			}
		}

		Log( "- Request has been denied." , ExitReason );

		__asm
		{
			POPAD
			JMP	Resume
		}
	}

	////////////
	//        //
	//  INVD  //
	//        //
	////////////
	if( ExitReason == 0x0000000D )
	{
		Log( "INVD detected - CPU0" , 0 );

		__asm
		{
			_emit 0x0F  // INVD
			_emit 0x08

			POPAD
			JMP		Resume
		}
	}

	/////////////
	//         //
	//  WBINVD //
	//         //
	/////////////
// 	if( ExitReason == 0x00000036 )
// 	{
// 		Log( "WBINVD detected", 0 );
// 
// 		__asm
// 		{
// 			WBINVD		
// 
// 				POPAD
// 
// 				JMP		Resume
// 		}
// 	}

	/////////////
	//         //
	//  RDMSR  //
	//         //
	/////////////
	if( ExitReason == 0x0000001F )
	{
		Log( "Read MSR - CPU0" , GuestECX );

		__asm
		{
			POPAD

			MOV		ECX, GuestECX				

			_emit	0x0F  // RDMSR
			_emit	0x32

			//////////////////////////////////////////////////////////////////////////

			JMP		Resume
		}
	}

	/////////////
	//         //
	//  WRMSR  //
	//         //
	/////////////
	if( ExitReason == 0x00000020 )
	{
		Log( "Write MSR - CPU0" , GuestECX );
		__asm
		{
			POPAD

			MOV		ECX, GuestECX
			MOV		EAX, GuestEAX
			MOV		EDX, GuestEDX

			_emit	0x0F  // WRMSR
			_emit	0x30

			JMP		Resume
		}
	}

	/////////////
	//         //
	//  CPUID  //
	//         //
	/////////////
	if( ExitReason == 0x0000000A )
	{
		PATCHRDTSC();

		if( HandlerLogging )
		{
			Log( "CPUID detected - CPU0", 0 );
			Log( "- EAX", GuestEAX );
		}

		if( GuestEAX == 0x00000000 )
		{
			__asm
			{
				POPAD

				MOV		EAX, 0x00000000

				CPUID

				MOV		EBX, CPU01_EBX
				MOV		ECX, CPU01_ECX
				MOV		EDX, CPU01_EDX

				JMP		Resume
			}
		}

		__asm
		{
			POPAD

			MOV		EAX, GuestEAX

			CPUID

			JMP		Resume
		}
	}

	///////////////////////////////
	//                           //
	//  Control Register Access  //
	//                           //
	///////////////////////////////
	if( ExitReason == 0x0000001C )
	{
		if( HandlerLogging ) Log( "Control Register Access detected.", 0 );

		movcrControlRegister = ( ExitQualification & 0x0000000F );
		movcrAccessType = ( ( ExitQualification & 0x00000030 ) >> 4 );
		movcrOperandType = ( ( ExitQualification & 0x00000040 ) >> 6 );
		movcrGeneralPurposeRegister = ( ( ExitQualification & 0x00000F00 ) >> 8 );

		if( HandlerLogging )
		{
			Log( "- movcrControlRegister", movcrControlRegister );
			Log( "- movcrAccessType", movcrAccessType );
			Log( "- movcrOperandType", movcrOperandType );
			Log( "- movcrGeneralPurposeRegister", movcrGeneralPurposeRegister );
		}

		//	Control Register Access (CR3 <-- reg32)
		//
		if( movcrControlRegister == 3 && movcrAccessType == 0 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 0 )
		{
			WriteVMCS( 0x00006802, GuestEAX );
			__asm POPAD
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 0 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 1 )
		{
			WriteVMCS( 0x00006802, GuestECX );
			__asm POPAD
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 0 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 2 )
		{
			WriteVMCS( 0x00006802, GuestEDX );
			__asm POPAD
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 0 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 3 )
		{
			WriteVMCS( 0x00006802, GuestEBX );
			__asm POPAD
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 0 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 4 )
		{
			WriteVMCS( 0x00006802, GuestESP );
			__asm POPAD
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 0 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 5 )
		{
			WriteVMCS( 0x00006802, GuestEBP );
			__asm POPAD
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 0 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 6 )
		{
			WriteVMCS( 0x00006802, GuestESI );
			__asm POPAD
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 0 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 7 )
		{
			WriteVMCS( 0x00006802, GuestEDI );
			__asm POPAD
				goto Resume;
		}
		//	Control Register Access (reg32 <-- CR3)
		//
		if( movcrControlRegister == 3 && movcrAccessType == 1 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 0 )
		{
			__asm	POPAD
				__asm	MOV EAX, GuestCR3
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 1 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 1 )
		{
			__asm	POPAD
				__asm	MOV ECX, GuestCR3
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 1 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 2 )
		{
			__asm	POPAD
				__asm	MOV EDX, GuestCR3
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 1 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 3 )
		{
			__asm	POPAD
				__asm	MOV EBX, GuestCR3
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 1 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 4 )
		{
			__asm	POPAD
				__asm	MOV ESP, GuestCR3
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 1 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 5 )
		{
			__asm	POPAD
				__asm	MOV EBP, GuestCR3
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 1 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 6 )
		{
			__asm	POPAD
				__asm	MOV ESI, GuestCR3
				goto Resume;
		}
		if( movcrControlRegister == 3 && movcrAccessType == 1 && movcrOperandType == 0 && movcrGeneralPurposeRegister == 7 )
		{
			__asm	POPAD
				__asm	MOV EDI, GuestCR3
				goto Resume;
		}
	}
	
	goto Exit; //: warning C4102: 'Exit' : unreferenced label
Exit:

	//
	//	Switch off VMX mode.
	//
	Log( "Terminating VMX Mode.", 0xDEADDEAD );
	__asm
	{
		_emit	0x0F	// VMXOFF
			_emit	0x01
			_emit	0xC4
	}

	VMXIsActive = 0;

	Log( "Flow Control Return to Address" , GuestEIP );

	__asm
	{
		POPAD
			MOV		ESP, GuestESP
			STI
			JMP		GuestEIP
	}

Resume:

	//	Need to execute the VMRESUME without having changed
	//	the state of the GPR and ESP et cetera.
	//
	__asm
	{
		STI

			_emit	0x0F	// VMRESUME
			_emit	0x01
			_emit	0xC3
	}
}