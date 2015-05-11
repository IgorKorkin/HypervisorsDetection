#include "phymem.h"

namespace timing_walk_mem
{
	NTSTATUS open_phys_mem(HANDLE &sectionHandle)
	{
		UNICODE_STRING usUnicodeString;
		OBJECT_ATTRIBUTES oaAttributes;

		RtlInitUnicodeString(&usUnicodeString, L"\\Device\\PhysicalMemory");
		InitializeObjectAttributes(&oaAttributes, &usUnicodeString, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, (HANDLE) NULL, (PSECURITY_DESCRIPTOR) NULL);

		return ZwOpenSection(&sectionHandle, SECTION_ALL_ACCESS, &oaAttributes);
	}

	// Maps a view of a section
	NTSTATUS map_phys_mem( HANDLE hPhysMem, LONGLONG paPhysAddr, DWORD & dwSize, PVOID & pVirtAddr )
	{
		PHYSICAL_ADDRESS pa_phys_addr; RtlZeroMemory(&pa_phys_addr, sizeof(PHYSICAL_ADDRESS));
		
		pa_phys_addr.QuadPart = paPhysAddr;

		unsigned int high_start = 0, low_start = 0, high_finish = 0, low_finish = 0;
		__asm
		{
			RDTSC	//////////////////////////////////////////////////////////////////////////
			MOV high_start, EDX
			MOV low_start, EAX
		}
		
		NTSTATUS nt_status = ZwMapViewOfSection( hPhysMem, NtCurrentProcess(), &pVirtAddr, 0L, 
			dwSize, &pa_phys_addr, (PSIZE_T)&dwSize, ViewShare, 0, PAGE_READONLY);
		
		__asm
		{
			RDTSC	//////////////////////////////////////////////////////////////////////////
			MOV high_finish, EDX
			MOV low_finish, EAX
		}

		unsigned __int64 elapsed_cycles = 
			(((unsigned __int64)high_finish << 32) | low_finish)
			- 
			(((unsigned __int64)high_start << 32) | low_start);	

		KdPrint(("%016I64d \t mdl=%08x \t[%016I64x-%016I64x]\n", elapsed_cycles, nt_status, pa_phys_addr.QuadPart, dwSize));   

		return nt_status;
	}

	void unmap_phys_mem( PVOID Address )
	{	
		ZwUnmapViewOfSection( (HANDLE) -1, (PVOID) Address );
	}

	char get_addr(DWORD pAddr)
	{
		char val = '?';
		__try
		{
			val = *(PUCHAR) ( pAddr );
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
		return val;
	}

	void mem_walker_map_section()
	{
		DbgPrint(">> mem_walker \n");

		HANDLE h_physmem = NULL;
		
		DbgPrint("OpenPhysicalMemory %08X", h_physmem);
		if(NT_SUCCESS(open_phys_mem(h_physmem)))
		{				
			LONGLONG paddress = 0x770000;			
			PVOID vaddress = NULL;
			DWORD len = 0x20;

			for (int i = 0; i < 10; i++)
			{
				paddress = paddress + 64*1024*1024 * i;
				DbgPrint("paddress = %08x \n", paddress);
				if(NT_SUCCESS(map_phys_mem( h_physmem, paddress, len, vaddress)))
				{	
					unmap_phys_mem( vaddress );
				}	
				else
				{   DbgPrint("err map_phys_mem \n");   }
			}
			
		}
		else
			{   DbgPrint("err open_phys_mem \n");   }

		DbgPrint("<< mem_walker \n");
	}

	void mem_walker_map_section(PTaskForMeasurement pTraceFeatures, PTimeInterval pTimeInterval)
	{
		KdPrint((">> mem_walker \n"));
		HANDLE h_physmem = NULL;
		if(NT_SUCCESS(open_phys_mem(h_physmem)))
		{		
			__int64 phys_mem_offset = pTraceFeatures->phys_mem_offset;		
			PHYSICAL_ADDRESS low_phys_addr = {0};
			DWORD len = pTraceFeatures->viewsize;
			PVOID virt_addr = NULL;
			NTSTATUS nt_status = STATUS_SUCCESS;
			unsigned int i_max = (unsigned int) ((MAX_PHYS_MEM_ADDR - pTraceFeatures->start_high_phys_addr) / pTraceFeatures->phys_mem_offset);
			KeSetSystemAffinityThread( (KAFFINITY) 0x00000001 );
			{
				unsigned int high_start = 0, low_start = 0, high_finish = 0, low_finish = 0; //raiseirql :: RaiseIrql raise_irql(APC_LEVEL); ///////////////////////////////////////////////////////////////////////////////////////
				for (unsigned int i = 0; i < i_max; i++ )
				{					
					__asm 
					{
						MY_CPUID
						RDTSC	//////////////////////////////////////////////////////////////////////////
						MOV high_start, EDX
						MOV low_start, EAX
					}
					
					nt_status = ZwMapViewOfSection( h_physmem, NtCurrentProcess(), &virt_addr, 0L, len, &low_phys_addr, (PSIZE_T)&len, ViewShare, 0, PAGE_READONLY);
					//__try { *(PBYTE)virt_addr;} 	__except(EXCEPTION_EXECUTE_HANDLER){}		

					__asm
					{
						MY_CPUID
						RDTSC	//////////////////////////////////////////////////////////////////////////
						MOV high_finish, EDX
						MOV low_finish, EAX
					}				

					if(NT_SUCCESS(nt_status)) // STATUS_CONFLICTING_ADDRESSES == 0xC0000018
					{   ZwUnmapViewOfSection( NtCurrentProcess(), virt_addr );   }

					pTimeInterval[i].cycles_high_start = high_start;
					pTimeInterval[i].cycles_high_finish = high_finish;
					pTimeInterval[i].cycles_low_start = low_start;
					pTimeInterval[i].cycles_low_finish = low_finish;
					pTimeInterval[i].low_phys_addr = low_phys_addr.QuadPart;
					pTimeInterval[i].high_phys_addr = low_phys_addr.QuadPart + len;
					pTimeInterval[i].valid_address = (virt_addr == NULL) ? 0 : 1;
					pTimeInterval[i].nt_status = nt_status;

					low_phys_addr.QuadPart += (LONGLONG)(phys_mem_offset);
					virt_addr = NULL;
					len = pTraceFeatures->viewsize;
				}
			}
			KeRevertToUserAffinityThread();
		}		
		KdPrint(("<< mem_walker \n"));
	}

	void mem_walker_alloc_pages(PTaskForMeasurement pTraceFeatures, PTimeInterval pTimeInterval)
	{
		PMDL p_mdl = NULL;	
		PHYSICAL_ADDRESS low_phys_addr = {0}, high_phys_addr = {0}, skipbytes = {0};
		unsigned int i_max = (unsigned int) ((MAX_PHYS_MEM_ADDR - pTraceFeatures->start_high_phys_addr) / pTraceFeatures->phys_mem_offset);
		low_phys_addr.QuadPart = pTraceFeatures->start_low_phys_addr;
		high_phys_addr.QuadPart = pTraceFeatures->start_high_phys_addr;
		skipbytes.QuadPart = pTraceFeatures->skipbytes;
		__int64 phys_mem_offset = pTraceFeatures->phys_mem_offset;		
		unsigned long totalbytes = pTraceFeatures->totalbytes;		
		unsigned __int64 elapsed_cycles = 0;

		KeSetSystemAffinityThread( (KAFFINITY) 0x00000001 );
		{
			raiseirql :: RaiseIrql raise_irql(APC_LEVEL);
			unsigned int high_start = 0, low_start = 0, high_finish = 0, low_finish = 0;
			for (unsigned int i = 0; i < /*1000*/ i_max; i++ )
			{		
				__asm
				{
					MY_CPUID
					RDTSC	//////////////////////////////////////////////////////////////////////////
					MOV high_start, EDX
					MOV low_start, EAX
				}		

				p_mdl = MmAllocatePagesForMdl(low_phys_addr, high_phys_addr, skipbytes, totalbytes);							
				
				__asm
				{
					MY_CPUID
					RDTSC	//////////////////////////////////////////////////////////////////////////
					MOV high_finish, EDX
					MOV low_finish, EAX
				}
				
				pTimeInterval[i].cycles_high_start = high_start;
				pTimeInterval[i].cycles_high_finish = high_finish;
				pTimeInterval[i].cycles_low_start = low_start;
				pTimeInterval[i].cycles_low_finish = low_finish;
				pTimeInterval[i].high_phys_addr = high_phys_addr.QuadPart;
				pTimeInterval[i].low_phys_addr = low_phys_addr.QuadPart;
				pTimeInterval[i].valid_address = (p_mdl == NULL) ? 0 : 1;

				if (p_mdl)
					{   MmFreePagesFromMdl(p_mdl);   }				
				
				if (i%1000 == 0)
				{   
					elapsed_cycles = 
						(((unsigned __int64)high_finish << 32) | low_finish)
						- 
						(((unsigned __int64)high_start << 32) | low_start);
					DbgPrint("%016I64d \t mdl=%08x \t[%016I64x-%016I64x]\n", elapsed_cycles, p_mdl, low_phys_addr.QuadPart, high_phys_addr.QuadPart);   
				} 				
				low_phys_addr.QuadPart += (LONGLONG)(phys_mem_offset); 
				high_phys_addr.QuadPart += (LONGLONG)(phys_mem_offset);
			}		
		}
		KeRevertToUserAffinityThread();		
	}

	void mem_walker_mapiospace(PTaskForMeasurement pTraceFeatures, PTimeInterval pTimeInterval)
	{
		KdPrint((">> mem_walker_mapiospace \n"));
		HANDLE h_physmem = NULL;
		if(NT_SUCCESS(open_phys_mem(h_physmem)))
		{		
			__int64 phys_mem_offset = pTraceFeatures->phys_mem_offset;		
			PHYSICAL_ADDRESS low_phys_addr = {0};
			DWORD len = pTraceFeatures->viewsize;
			PVOID virt_addr = NULL;
			NTSTATUS nt_status = STATUS_SUCCESS;
			unsigned int i_max = (unsigned int) ((MAX_PHYS_MEM_ADDR - pTraceFeatures->start_high_phys_addr) / pTraceFeatures->phys_mem_offset);
			unsigned __int64 elapsed_cycles = 0;
			KeSetSystemAffinityThread( (KAFFINITY) 0x00000001 );
			{
				unsigned int high_start = 0, low_start = 0, high_finish = 0, low_finish = 0; 
				raiseirql :: RaiseIrql raise_irql(DISPATCH_LEVEL); ///////////////////////////////////////////////////////////////////////////////////////
				for (unsigned int i = 0; i < /*1000*/ i_max; i++ )
				{					
					__asm 
					{
						MY_CPUID
						RDTSC	//////////////////////////////////////////////////////////////////////////
						MOV high_start, EDX
						MOV low_start, EAX
					}

					virt_addr = MmMapIoSpace(low_phys_addr, len, MmNonCached);					

					__asm
					{
						MY_CPUID
						RDTSC	//////////////////////////////////////////////////////////////////////////
						MOV high_finish, EDX
						MOV low_finish, EAX
					}					
					pTimeInterval[i].cycles_high_start = high_start;
					pTimeInterval[i].cycles_high_finish = high_finish;
					pTimeInterval[i].cycles_low_start = low_start;
					pTimeInterval[i].cycles_low_finish = low_finish;
					pTimeInterval[i].low_phys_addr = low_phys_addr.QuadPart;
					pTimeInterval[i].high_phys_addr = low_phys_addr.QuadPart + len;
					pTimeInterval[i].valid_address = (virt_addr == NULL) ? 0 : 1;					

					if (i%1000 == 0)
					{   
						elapsed_cycles = 
							(((unsigned __int64)high_finish << 32) | low_finish)
							- 
							(((unsigned __int64)high_start << 32) | low_start);
						DbgPrint("%016I64d \t [%016I64x-%016I64x]\n", elapsed_cycles, low_phys_addr.QuadPart, low_phys_addr.QuadPart + len);   
					}

					low_phys_addr.QuadPart += (LONGLONG)(phys_mem_offset);
					virt_addr = NULL;
					len = pTraceFeatures->viewsize;
				}
			}
			KeRevertToUserAffinityThread();
		}		
		KdPrint(("<< mem_walker_mapiospace \n"));
	}

	void get_walk_mem_time( PVOID pTraceFeatures, PVOID pTimeInterval )
	{
		PTaskForMeasurement p_trace_features = (PTaskForMeasurement)pTraceFeatures;
		PTimeInterval p_time_interval = (PTimeInterval)pTimeInterval;

		mem_walker_map_section(p_trace_features, p_time_interval);
		//mem_walker_mapiospace(p_trace_features, p_time_interval); // T = 5849044; 5824172; 5847865; 5832538; 5842601; 6864589
		//mem_walker_alloc_pages(p_trace_features, p_time_interval); // T = 1540174;  987154;  46228;   47929;   4652933; 1861748;
	}
}

