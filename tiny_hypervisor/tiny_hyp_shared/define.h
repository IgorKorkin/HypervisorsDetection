/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef	DEFINE_H
#define	DEFINE_H


#define FILE_DEVICE_IOCTL 0x00008301
#define VMM_DRV_CTL_CODE(_x_) CTL_CODE(FILE_DEVICE_IOCTL, _x_,  METHOD_IN_DIRECT, FILE_ANY_ACCESS)

// DETECT CPUID
#define GET_CPUID_0			VMM_DRV_CTL_CODE(0x806+10)

// VMM CPU0
#define RUN_VMM_CPU0		VMM_DRV_CTL_CODE(0x806+20)
#define STOP_VMM_CPU0		VMM_DRV_CTL_CODE(0x806+21)

// SET DELTA
#define SET_DELTA			VMM_DRV_CTL_CODE(0x806+30)

//////////////////////////////////////////////////////////////////////////
//Detect
#define		VMM_DRV_NAME	L"hypervisor_driver"
#define		VMM_DRV_NAME_DeviceNameToDRV	L"\\Device\\HypControlDevice"
#define		VMM_DRV_NAME_LinkNameToDRV		L"\\DosDevices\\"VMM_DRV_NAME
#define		VMM_DRV_NAME_LinkNametoGUI		L"\\\\.\\"VMM_DRV_NAME
#define		VMM_DRV_NAME_ServiseNametoGUI	VMM_DRV_NAME

/************************************************************************/
/*                            CHECK CPUID                               */
/************************************************************************/

#define CPU01_EBX	0x53464441  // ADFS
#define CPU01_EDX	0x3123204C  // L #1
#define CPU01_ECX	0x35312730  // 0'15

typedef struct _OUT_CPUID_0_1
{
	unsigned long EAX;
	unsigned long ECX;
	unsigned long EDX;
	unsigned long EBX;
}OUT_CPUID_0_1,*POUT_CPUID_0_1;	


enum un_status
{
	unknown = 0, // в случае некорректно заданного статуса
	init_status = 1,

	all_success, //< удалить потом, не информтивное сообщение

	pae_enable,
	err_alloc_vmxon_region,		
	err_alloc_vmcs_region,
	err_alloc_vmexit_stack,
	vmx_support_not_present,
	unsupported_memory_type_VMXON_region,			
	unknown_vmxon_region_memory_type,
	protectedmode_not_enabled,
	paging_not_enabled,
	bios_support_not_present,
	vmxon_operation_failed,		
	vmclear_operation_failed,		
	vmlaunch_failure,

	launch_abort //< удалить потом, не информтивное сообщение
};	

typedef struct _CONFIG_VMM
{
	/*KAFFINITY*/ __w64 unsigned long kAfinity; // < номер ядра процессора, на котором запускается монитор (по умолчанию =1)
	__int64 i64DeltaTime; // < поправка для скручивания счётчика
}CONFIG_VMM, *PCONFIG_VMM;



#endif	DEFINE_H