#ifndef __COMMON_RDTSC_H_
#define __COMMON_RDTSC_H_

#define		KARVMM_SYS				L"karvmm.sys"
#define		KARVMM_DEVICENAME_DRV	L"\\Device\\KarVMMDev"
#define		KARVMM_LINKNAME_DRV 	L"\\DosDevices\\"KARVMM_SYS
#define		KARVMM_LINKNAME_GUI 	L"\\\\.\\"KARVMM_SYS
#define		KARVMM_SERVICENAME_GUI	KARVMM_SYS

extern const wchar_t *g_MethodDetectionName[];

//Type of a method of detection
enum MethodDetectingType 
{
	BlockOfCPUID  = 0, // подсчёт числа членов в вариационном ряду, трасса из 10 инструкций CPUID
	CPUIDwithDisabledCache, // сравнение границ варирования времени выполнения  некешированной трассы, трасса из 10 инструкций CPUID
	BlockOfVMXON, // подсчёт числа членов в вариационном ряду, трасса из инструкций VMXON
	VMXONwithDisabledCache, // сравнение границ варирования времени выполнения  некешированной трассы, трасса из инструкций VMXON	
	WalkThroughMemory, // измерение времени отлика при обращении к памяти
};

#define FILE_DEVICE_IOCTL  0x00008301
#define IOCTL_BLOCK_OF_CPUID	CTL_CODE(FILE_DEVICE_IOCTL, 0x800 + 1, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_BLOCK_OF_CPUID_WITH_DISABLED_CACHE		CTL_CODE(FILE_DEVICE_IOCTL, 0x800 + 2, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_BLOCK_OF_VMXON	CTL_CODE(FILE_DEVICE_IOCTL, 0x800 + 11, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_BLOCK_OF_VMXON_WITH_DISABLED_CACHE		CTL_CODE(FILE_DEVICE_IOCTL, 0x800 + 12, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_WALK_MEM_NEITHER	CTL_CODE(FILE_DEVICE_IOCTL, 0x800 + 21, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_ONECPUID_CACHE		CTL_CODE(FILE_DEVICE_IOCTL, 0x800 + 31, METHOD_NEITHER, FILE_ANY_ACCESS)



///////////////////////////////////////////////////////////////
// GUI --> DRIVER
/* Структура задания для драйвера */
typedef struct _TaskForMeasurement
{
	// поля используются всеми методами
	MethodDetectingType method_type; // метод обработки
	unsigned long ctrl_code; // контрольный код для обработки

	// поля используются методами, производящими измерение времени трасс
	unsigned int affinity; // маска ядра, на котором производится обнаружение монитора
	unsigned int trace_length; // длина трассы (const = 10) // <- длина трассы (по умолчанию 10 инструкций)
	unsigned int num_measurements; // число измерений (1000) // < - число измерений (по умолчанию 1000 измерений)
	unsigned int num_repetitions; // число повторов измерений (10)
	unsigned int delay_between_rep; // задержка между повторами измерений
	unsigned int num_experiment; // число опытов (1-2-3)
	unsigned int delay_between_exp; // // задержка между опытами
	
	// поля используются только при прохождении по памяти
	__int64 start_low_phys_addr; //= 0 начальный адрес младших физ памяти
	__int64 start_high_phys_addr; //= PAGE_SIZE  начальный адрес старших физ памяти
	__int64 phys_mem_offset; //= PAGE_SIZE шаг прохода по физ памяти
	
	// для функции MmAllocatePagesForMdl
	__int64 skipbytes; //= {PAGE_SIZE} параметр функции MmAllocatePagesForMdl 	
	unsigned long totalbytes; //= PAGE_SIZE параметр функции MmAllocatePagesForMdl

	// для функции ZwMapViewOfSection
	size_t viewsize; //= 0x20 размер отображаемой памяти

}TaskForMeasurement, *PTaskForMeasurement;


#define MAX_PHYS_MEM_ADDR 0xFFFFFFFF

// Define the page size for the Intel 386 as 4096 (0x1000).
#define PHYSMEM_PAGE_SIZE 0x1000

//////////////////////////////////////////////////////////////////////////
//  GUI <-- DRIVER

/* Структура результата для управляющей программы */
typedef struct _TimeInterval
{	
	// поля используются всеми методами для хранения измерения времени
	unsigned int cycles_high_start; 
	unsigned int cycles_low_start;
	unsigned int cycles_high_finish;
	unsigned int cycles_low_finish;

	// поля используются только при прохождении по памяти
	__int64 low_phys_addr; // адрес младших адресов физ памяти
	__int64 high_phys_addr; // адрес старших адресов физ памяти
	unsigned int valid_address; // определяет валидность адреса физ памяти
	long nt_status; // статус функции при отработке



}TimeInterval, *PTimeInterval;

/* Структура результата для управляющей программы с учётом обработанного времени */
typedef struct _TimeIntervalRes
{	
	// поле используется всеми методами для хранения измерения времени
	unsigned __int64 trace_duration;

	// поля используются только при прохождении по памяти
	__int64 low_phys_addr; // адрес младших адресов физ памяти
	__int64 high_phys_addr; // адрес старших адресов физ памяти
	unsigned int valid_address; // определяет валидность адреса физ памяти
	long nt_status; // статус функции при отработке

}TimeIntervalRes, *PTimeIntervalRes;

#endif // __COMMON_RDTSC_H_