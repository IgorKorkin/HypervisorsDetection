#ifndef __TIMING_RDTSC_H_
#define __TIMING_RDTSC_H_

#include "shared/common_vmmkar.h"
#include "raiseirql.h"
#include "cache.h"



//#define SERIALIZATION  //  control this define
		
#ifdef SERIALIZATION //  look up
	#define MY_CPUID CPUID 
	#pragma message("* * * * * SERIALIZATION ON * * * * * ") 
#else
	#define MY_CPUID
	#pragma message("* * * * * SERIALIZATION OFF * * * * * ") 	
#endif
	
extern "C" namespace timing_rdtsc
{
		// EDX:       Feature Flags
	typedef struct  _EDX_FEATURE_FLAGS
	{
		unsigned int FPU		:1;		//   [0:0]   FPU on chip
		unsigned int VME		:1;		//   [1:1]   VME: Virtual-8086 Mode enhancements
		unsigned int DE			:1;		//   [2:2]   DE: Debug Extensions (I/O breakpoints)
		unsigned int PSE		:1;		//   [3:3]   PSE: Page Size Extensions
		unsigned int TSC		:1;		//   [4:4]   TSC: Time Stamp Counter
		unsigned int MSR		:1;		//   [5:5]   MSR: RDMSR and WRMSR support
		unsigned int PAE		:1;		//   [6:6]   PAE: Physical Address Extensions
		unsigned int MCE		:1;		//   [7:7]   MCE: Machine Check Exception
		unsigned int CXS		:1;		//   [8:8]   CXS: CMPXCHG8B instruction
		unsigned int APIC		:1;		//   [9:9]   APIC: APIC on Chip
		unsigned int Reserved	:22;	//   [10:10] Reserved

		//   [10:10] Reserved
		//   [11:11] SYSENTER/SYSEXIT support
		//   [12:12] MTRR: Memory Type Range Reg
		//   [13:13] PGE/PTE Global Bit
		//   [14:14] MCA: Machine Check Architecture
		//   [15:15] CMOV: Cond Mov/Cmp Instructions
		//   [16:16] PAT: Page Attribute Table
		//   [17:17] PSE-36: Physical Address Extensions
		//   [18:18] Processor Serial Number
		//   [19:19] CLFLUSH: CLFLUSH Instruction support
		//   [20:20] Reserved
		//   [21:21] DS: Debug Store
		//   [22:22] ACPI: Thermal Monitor and Software Controlled Clock Facilities
		//   [23:23] MMX Technology
		//   [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
		//   [25:25] SSE: SSE Extensions
		//   [26:26] SSE2: SSE2 Extensions
		//   [27:27] Reserved
		//   [28:28] Hyper Threading Technology
		//   [29:29] TM: Thermal Monitor
		//   [30:30] Reserved
		//   [31:31] PBE: Pending Break Enable
	}EDX_FEATURE_FLAGS, *PEDX_FEATURE_FLAGS;

	/* функция диспетчеризации, измерение времени трассы из инструкций CPUID*/
	void get_time_trace_cpuid(PVOID pTraceFeatures, PVOID pTimeInterval);
	/* измерение времени трассы из 10 инструкций CPUID */
	void get_time_trace_cpuid_ten(PTaskForMeasurement pTraceFeatures, PTimeInterval pTimeInterval);
	/* измерение времени трассы из 1 инструкции CPUID */
	void get_time_one_cpuid( PTaskForMeasurement pTraceFeatures, PTimeInterval pTimeInterval );

	void get_time_cache_trace_cpuid( PVOID pTraceFeatures, PVOID pTimeInterval );	
	void get_time_cache_trace_cpuid_ten( PTaskForMeasurement pTraceFeatures, PTimeInterval pTimeInterval );
	
							 // VMXON [ESP]
	#define CALL_VMXON_ESP  \
		__asm _emit	0xF3 \
		__asm _emit	0x0F \
		__asm _emit	0xC7 \
		__asm _emit	0x34 \
		__asm _emit	0x24 

		/*
		or
		__asm _emit	0xF3 \     // VMXON (only)
		__asm _emit	0x0F \
		__asm _emit	0xC7 \
		*/



	#define CALL_VMXON \
		__try {   CALL_VMXON_ESP   }   __except(EXCEPTION_EXECUTE_HANDLER) {   }

	void get_time_trace_vmxon( PVOID pTraceFeatures, PVOID pTimeInterval );	
	 
	void get_time_trace_vmxon_ten( PTaskForMeasurement pTraceFeatures, PTimeInterval pTimeInterval );
	
	void get_time_cache_trace_vmxon( PVOID pTraceFeatures, PVOID pTimeInterval );

	void get_time_cache_trace_vmxon_ten( PTaskForMeasurement pTraceFeatures, PTimeInterval pTimeInterval );	
}


#endif // __TIMING_RDTSC_H_