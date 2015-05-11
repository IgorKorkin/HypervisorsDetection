#ifndef DRV_H
#define DRV_H

#ifdef WIN64
#define _AMD64_
#define _M_AMD64_
#else 
#define i386 1
#define _X86_ 1
#define CONDITION_HANDLING 1
#define _NT1X_ 100
#define _WIN32_WINNT 0x0600 
#define  WINVER  0x0600 
#endif 

extern "C" 
{
	#pragma pack(push, 8)
	#ifdef _AMD64_
	  #pragma warning(push)
	  #pragma warning (disable : 4430)
	#endif // _AMD64_
		#include <ntddk.h>
		#include "tiny_hyp_shared\define.h"
		#include "WinDef.h"
	#ifdef _AMD64_
	  #pragma warning(pop)
	#endif // _AMD64_
	#pragma pack(pop)
};

#include "control_hypervisor.h"

#endif // DRV_H