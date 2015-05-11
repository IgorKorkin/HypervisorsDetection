#ifndef VMMCPU0_H 
#define VMMCPU0_H

#include "load_hypervisor.h"
#include "time_cheating.h"
#include "tiny_hyp_shared/define.h"

extern ULONG 		ExitReason;
extern ULONG 		ExitQualification;
extern ULONG 		ExitInterruptionInformation;
extern ULONG 		ExitInterruptionErrorCode;
extern ULONG 		IDTVectoringInformationField;
extern ULONG 		IDTVectoringErrorCode;
extern ULONG 		ExitInstructionLength;
extern ULONG 		ExitInstructionInformation;

extern ULONG 		GuestEIP;
extern ULONG 		GuestResumeEIP;
extern ULONG 		GuestESP;
extern ULONG 		GuestCS;
extern ULONG 		GuestCR0;
extern ULONG 		GuestCR3;
extern ULONG 		GuestCR4;
extern ULONG 		GuestEFLAGS;

extern ULONG 		GuestEAX;
extern ULONG 		GuestEBX;
extern ULONG 		GuestECX;
extern ULONG 		GuestEDX;
extern ULONG 		GuestEDI;
extern ULONG 		GuestESI;
extern ULONG 		GuestEBP;

extern ULONG 		movcrControlRegister;
extern ULONG 		movcrAccessType;
extern ULONG 		movcrOperandType;
extern ULONG 		movcrGeneralPurposeRegister;
extern ULONG		movcrLMSWSourceData;

#endif // VMMCPU0_H