#include "timing_rdtsc.h"

extern "C" namespace timing_rdtsc
{	
	bool inline check_support_rdtsc()
	{
		EDX_FEATURE_FLAGS edx_flags;
		__asm
		{
			PUSHAD
			
			mov EAX,1
			CPUID
			mov edx_flags,EDX
			
			POPAD
		}
		return edx_flags.TSC;
	}

	void get_time_cache_trace_cpuid( PVOID pTraceFeatures, PVOID pTimeInterval )
	{
		KdPrint((TEXT("> get_time_cache_trace_cpuid\n")));
		if (check_support_rdtsc())
		{
			PTaskForMeasurement p_trace_features = (PTaskForMeasurement)pTraceFeatures;
			PTimeInterval p_time_interval = (PTimeInterval)pTimeInterval;

			KdPrint((TEXT("Trace len = %d \r\n"), p_trace_features->trace_length));

			switch (p_trace_features->trace_length)
			{
			case (10):
				get_time_cache_trace_cpuid_ten(p_trace_features, p_time_interval);
				break;
			default:
				KdPrint((TEXT("Unknown parameter \n")));
			}	
		}
		else
		{
			KdPrint((TEXT("TSC is not supported \r\n")));
		}

		KdPrint((TEXT("< get_time_cache_trace_cpuid\n")));
	}

	void get_time_cache_trace_cpuid_ten( PTaskForMeasurement pTraceFeatures, PTimeInterval pTimeInterval )
	{
		KdPrint((TEXT(">get_time_trace_cpuid_ten\n")));
		unsigned int high_start = 0, low_start = 0, high_finish = 0, low_finish = 0;
		KAFFINITY affinity = pTraceFeatures->affinity;
		KeSetSystemAffinityThread( affinity ); // http://www.msuiche.net/2009/01/05/multi-processors-and-kdversionblock
		__debugbreak();
		disable_cache();
		{
			raiseirql :: RaiseIrql raise_irql(HIGH_LEVEL);
			for (unsigned int i = 0 ; i < pTraceFeatures->num_measurements ; i++)
			{
// 				if (i ==0)
// 					{   wbinvd();   }
				wbinvd();
				__asm 
				{
					PUSHAD	
					MY_CPUID
					RDTSC	//////////////////////////////////////////////////////////////////////////
					MOV high_start, EDX
					MOV low_start, EAX
					CPUID // 1
					CPUID // 2
					CPUID // 3
					CPUID // 4
					CPUID // 5
					CPUID // 6
					CPUID // 7 
					CPUID // 8
					CPUID // 9
					CPUID // 10
					MY_CPUID
					RDTSC	//////////////////////////////////////////////////////////////////////////
					MOV high_finish, EDX
					MOV low_finish, EAX
					POPAD
				}
				pTimeInterval[i].cycles_high_start = high_start;
				pTimeInterval[i].cycles_high_finish = high_finish;
				pTimeInterval[i].cycles_low_start = low_start;
				pTimeInterval[i].cycles_low_finish = low_finish;
			}			
		}	
		enable_cache();
		KeRevertToUserAffinityThread();
		KdPrint((TEXT("<get_time_trace_cpuid_ten\n")));
	}

	//////////////////////////////////////////////////////////////////////////

	void get_time_trace_cpuid( PVOID pTraceFeatures, PVOID pTimeInterval )
	{
		KdPrint((TEXT("> get_time_trace_cpuid\n")));
		
		if (check_support_rdtsc())
		{
			PTaskForMeasurement p_trace_features = (PTaskForMeasurement)pTraceFeatures;
			PTimeInterval p_time_interval = (PTimeInterval)pTimeInterval;
			
			KdPrint((TEXT("Trace len = %d \r\n"), p_trace_features->trace_length));
			
			switch (p_trace_features->trace_length)
			{
			case (10):
				get_time_trace_cpuid_ten(p_trace_features, p_time_interval);
				break;
			case (1):
				get_time_one_cpuid(p_trace_features, p_time_interval);
				break;
			default:
				KdPrint((TEXT("Unknown parameter \n")));
			}
		}
		else   {   KdPrint((TEXT("TSC is not supported \r\n")));   }

		KdPrint((TEXT("< get_time_trace_cpuid\n")));
	}

	/* измерение времени трассы из 10 инструкций CPUID */
	void get_time_trace_cpuid_ten( PTaskForMeasurement pTraceFeatures, PTimeInterval pTimeInterval )
	{
		KdPrint((TEXT(">get_time_trace_cpuid_ten\n")));
		unsigned int high_start = 0;
		unsigned int low_start = 0;
		unsigned int high_finish = 0;
		unsigned int low_finish = 0;		
		KAFFINITY affinity = pTraceFeatures->affinity;
		KeSetSystemAffinityThread( affinity /*0x00000001*/ ); // http://www.msuiche.net/2009/01/05/multi-processors-and-kdversionblock		
		{
			raiseirql :: RaiseIrql raise_irql(HIGH_LEVEL);
			for (unsigned int i = 0 ; i < pTraceFeatures->num_measurements ; i++)
			{
				__asm 
				{
					PUSHAD	
					MY_CPUID
					RDTSC	//////////////////////////////////////////////////////////////////////////
					MOV high_start, EDX
					MOV low_start, EAX
					CPUID // 1
					CPUID // 2
					CPUID // 3
					CPUID // 4
					CPUID // 5
					CPUID // 6
					CPUID // 7 
					CPUID // 8
					CPUID // 9
					CPUID // 10
					MY_CPUID
					RDTSC	//////////////////////////////////////////////////////////////////////////
					MOV high_finish, EDX
					MOV low_finish, EAX
					POPAD
				}
				pTimeInterval[i].cycles_high_start = high_start;
				pTimeInterval[i].cycles_high_finish = high_finish;
				pTimeInterval[i].cycles_low_start = low_start;
				pTimeInterval[i].cycles_low_finish = low_finish;
			}	
		}	
		KeRevertToUserAffinityThread();
		KdPrint((TEXT("<get_time_trace_cpuid_ten\n")));
	}

	/* измерение времени трассы из 1 инструкции CPUID */
	void get_time_one_cpuid( PTaskForMeasurement pTraceFeatures, PTimeInterval pTimeInterval )
	{
		KdPrint((TEXT(">get_time_trace_cpuid_ten\n")));
		unsigned int high_start = 0;
		unsigned int low_start = 0;
		unsigned int high_finish = 0;
		unsigned int low_finish = 0;		
		KAFFINITY affinity = pTraceFeatures->affinity;
		KeSetSystemAffinityThread( affinity /*0x00000001*/ ); // http://www.msuiche.net/2009/01/05/multi-processors-and-kdversionblock	
		{
			raiseirql :: RaiseIrql raise_irql(HIGH_LEVEL);
			for (unsigned int i = 0 ; i < pTraceFeatures->num_measurements ; i++)
			{
				wbinvd();
				__asm 
				{
					PUSHAD	
						MY_CPUID
						RDTSC	//////////////////////////////////////////////////////////////////////////
						MOV high_start, EDX
						MOV low_start, EAX
						CPUID // 1
						MY_CPUID
						RDTSC	//////////////////////////////////////////////////////////////////////////
						MOV high_finish, EDX
						MOV low_finish, EAX
						POPAD
				}
				pTimeInterval[i].cycles_high_start = high_start;
				pTimeInterval[i].cycles_high_finish = high_finish;
				pTimeInterval[i].cycles_low_start = low_start;
				pTimeInterval[i].cycles_low_finish = low_finish;
			}	
		}	
		KeRevertToUserAffinityThread();
		KdPrint((TEXT("<get_time_trace_cpuid_ten\n")));
	}

	//////////////////////////////////////////////////////////////////////////

	void get_time_trace_vmxon( PVOID pTraceFeatures, PVOID pTimeInterval )
	{
		KdPrint((TEXT("> get_time_trace_cpuid\n")));
		if (check_support_rdtsc())
		{
			PTaskForMeasurement p_trace_features = (PTaskForMeasurement)pTraceFeatures;
			PTimeInterval p_time_interval = (PTimeInterval)pTimeInterval;

			KdPrint((TEXT("Trace len = %d \r\n"), p_trace_features->trace_length));

			switch (p_trace_features->trace_length)
			{
			case (10):
				get_time_trace_vmxon_ten(p_trace_features, p_time_interval);
				break;
			default:
				KdPrint((TEXT("Unknown parameter \n")));
			}	
		}
		else
		{
			KdPrint((TEXT("TSC is not supported \r\n")));
		}

		KdPrint((TEXT("< get_time_trace_cpuid\n")));
	}

	void get_time_trace_vmxon_ten( PTaskForMeasurement pTraceFeatures, PTimeInterval pTimeInterval )
	{
		KdPrint((TEXT(">get_time_trace_cpuid_ten\n")));
		unsigned int high_start = 0;
		unsigned int low_start = 0;
		unsigned int high_finish = 0;
		unsigned int low_finish = 0;		
		KAFFINITY affinity = pTraceFeatures->affinity;
		KeSetSystemAffinityThread( affinity /*0x00000001*/ ); // http://www.msuiche.net/2009/01/05/multi-processors-and-kdversionblock	
		{
			KIRQL start_irql = KfRaiseIrql(HIGH_LEVEL);
			for (unsigned int i = 0 ; i < pTraceFeatures->num_measurements ; i++)
			{
				__asm 
				{
					MY_CPUID
					RDTSC
					MOV high_start, EDX
					MOV low_start, EAX
				}

				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON

				__asm
				{
					MY_CPUID
					RDTSC
					MOV high_finish, EDX
					MOV low_finish, EAX
				}

				pTimeInterval[i].cycles_high_start = high_start;
				pTimeInterval[i].cycles_high_finish = high_finish;
				pTimeInterval[i].cycles_low_start = low_start;
				pTimeInterval[i].cycles_low_finish = low_finish;
			}	
			KeLowerIrql(start_irql);
		}	
		KeRevertToUserAffinityThread();
		KdPrint((TEXT("<get_time_trace_cpuid_ten\n")));
	}

	void get_time_cache_trace_vmxon( PVOID pTraceFeatures, PVOID pTimeInterval )
	{
		KdPrint(("> get_time_cache_trace_vmxon\n"));
		if (check_support_rdtsc())
		{
			PTaskForMeasurement p_trace_features = (PTaskForMeasurement)pTraceFeatures;
			PTimeInterval p_time_interval = (PTimeInterval)pTimeInterval;

			KdPrint((TEXT("Trace len = %d \r\n"), p_trace_features->trace_length));

			switch (p_trace_features->trace_length)
			{
			case (10):
				get_time_cache_trace_vmxon_ten(p_trace_features, p_time_interval);
				break;
			default:
				KdPrint(("Unknown parameter \n"));
			}	
		}
		else
		{
			KdPrint(("TSC is not supported \r\n"));
		}

		KdPrint(("< get_time_cache_trace_vmxon\n"));
	}

	void get_time_cache_trace_vmxon_ten( PTaskForMeasurement pTraceFeatures, PTimeInterval pTimeInterval )
	{
		KdPrint((">get_time_cache_trace_vmxon_ten\n"));
		unsigned int high_start = 0, low_start = 0, high_finish = 0, low_finish = 0;		
		KAFFINITY affinity = pTraceFeatures->affinity;
		KeSetSystemAffinityThread( affinity /*0x00000001*/ ); // http://www.msuiche.net/2009/01/05/multi-processors-and-kdversionblock					
		{
			KIRQL start_irql = KfRaiseIrql(HIGH_LEVEL);
			for (unsigned int i = 0 ; i < pTraceFeatures->num_measurements ; i++)
			{				
				wbinvd();
				__asm 
				{					
					MY_CPUID
					RDTSC
					MOV high_start, EDX
					MOV low_start, EAX
				}

				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON
				CALL_VMXON

				__asm
				{
					MY_CPUID
					RDTSC
					MOV high_finish, EDX
					MOV low_finish, EAX					
				}

				pTimeInterval[i].cycles_high_start = high_start;
				pTimeInterval[i].cycles_high_finish = high_finish;
				pTimeInterval[i].cycles_low_start = low_start;
				pTimeInterval[i].cycles_low_finish = low_finish;				
			}	
			KeLowerIrql(start_irql);
		}	
		enable_cache();
		KeRevertToUserAffinityThread();
		KdPrint(("<get_time_cache_trace_vmxon_ten\n"));
	}
}