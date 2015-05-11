
#ifndef __VK_DRV_H_
#define __VK_DRV_H_

//-----------------

#include "ntddk.h"          
#include "WinDef.h"


//-----------------

#include "timing_rdtsc.h"
#include "phymem.h"


NTSTATUS DriverEntry( IN PDRIVER_OBJECT pDrv, IN PUNICODE_STRING pRegistryPath);




#endif // __VK_DRV_H_

