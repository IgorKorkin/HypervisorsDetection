/*#pragma once*/

#include "driver_entry.h"
#include "load_hypervisor.h"


/************************************************************************/
/*					CONFIG                                              */
/************************************************************************/

#define CPUID_ATTACK	1

//#define RESEARCH_TEST

//#define NII_702_futjutsi_E6600 // E6600 2400MHz
//#define NII_702_i7_950 // i7 950 3066MHz
//#define NII_ryadom_502 //E8600
//#define HOME_ALEX_E6300 //E6300
#define HOME_IGOR_E8200 //E8200

#ifdef RESEARCH_TEST
	#define DELTA 0
#endif

#ifdef NII_702_futjutsi_E6600
	#define DELTA  2209 - 212 + 1
#endif

#ifdef NII_702_i7_950
	#define DELTA  1447 - 278 - 1  
#endif

#ifdef NII_ryadom_502
	#define DELTA 1630 - 285 + 10 + 1 
#endif

#ifdef HOME_ALEX_E6300
	#define DELTA 2280 - 290 + 10 - 1 - 1 
#endif

#ifdef HOME_IGOR_E8200
	#define DELTA 2238 - 292
#endif

/************************************************************************/
/*				CPUID_ATTACK                                            */
/************************************************************************/
extern __int64 g_TscNext;
extern __int64 g_Delta;
void read_tsc();
void patch_tsc();

#if CPUID_ATTACK
	void read_tsc();
	void patch_tsc();
	void set_delta(__int64 delta);

	#define SPYRDTSÑ()  read_tsc()
	#define PATCHRDTSC() patch_tsc()
#else
	#define SPYRDTSÑ()  
	#define PATCHRDTSC(_x_)
#endif

extern __int64 T1RDTSC;


