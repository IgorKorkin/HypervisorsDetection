
#include "time_cheating.h"

__int64 OldTime = 0;

ULONG cycles_high;
ULONG cycles_low;

__int64 g_TscNext = 0;
__int64 g_Delta = 1000;

void set_delta(__int64 delta)
{
	g_Delta = delta;
}

void read_tsc()
{
	ULONG cycles_high = 0, cycles_low = 0;
	__asm
	{
		pushad
		RDTSC
		mov	cycles_high, edx
		mov cycles_low, eax
		popad
	}
	g_TscNext = (((unsigned __int64)cycles_high << 32) | cycles_low);
}

void patch_tsc()
{
	g_TscNext -= g_Delta;
	ULONG cycles_high = (ULONG)(g_TscNext>>32);
	ULONG cycles_low = (ULONG)((g_TscNext<<32)>>32);
	__asm 
	{	
			MOV ECX, 10h		/*IA32_TIME_STAMP_COUNTER*/
			mov	edx,cycles_high
			mov	eax,cycles_low
			WRMSR
	}
}