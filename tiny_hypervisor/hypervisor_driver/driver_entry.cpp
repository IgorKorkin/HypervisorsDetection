/************************************************************************/
/*                                                                      */
/************************************************************************/

#include "driver_entry.h"

NTSTATUS OnDispatch(IN PDEVICE_OBJECT  pDev, IN PIRP  pIrp)
{	
	ULONG io_control_code = 0;
	PVOID in_buf = NULL, out_buf = NULL;
	ULONG in_buf_sz = 0, out_buf_sz = 0;

	PIO_STACK_LOCATION p_io_stack = IoGetCurrentIrpStackLocation(pIrp);
	io_control_code = p_io_stack->Parameters.DeviceIoControl.IoControlCode;

	{
		switch (io_control_code)
		{
		case RUN_VMM_CPU0:
			{
				RunVMMCPU0();
				break;
			}			

		case STOP_VMM_CPU0:
			{
				StopVMMCPU0();
				break;
			}			

		case GET_CPUID_0:		
			{
				out_buf = 
					MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);

				ULONG A = 0, B = 0, C = 0, D = 0;	
				KeSetSystemAffinityThread( (KAFFINITY) 0x00000001 /*k_afinity*/ );
				__asm
				{
					PUSHAD
					MOV		EAX, 0
					CPUID
					MOV		A, EAX
					MOV		B, EBX
					MOV		C, ECX
					MOV		D, EDX
					POPAD
				}

				__try
				{
					((OUT_CPUID_0_1*)out_buf)->EAX = A;
					((OUT_CPUID_0_1*)out_buf)->EBX = B;
					((OUT_CPUID_0_1*)out_buf)->ECX = C;
					((OUT_CPUID_0_1*)out_buf)->EDX = D;
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					KdPrint(("EXCEPTION_EXECUTE_HANDLER (%08x) outBuf= ", io_control_code, out_buf ));					
				}
				break;
			}

		case SET_DELTA:
			{
				in_buf = 
					pIrp->AssociatedIrp.SystemBuffer;
				__int64 delta = 0;
				__try
				{
					delta = *((__int64 *)in_buf); 
					set_delta(delta);
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					KdPrint(("EXCEPTION_EXECUTE_HANDLER (%08x) in_buf=0x%08x", io_control_code, in_buf ));
				}		

				break;
			}

		default:
			{   Log( "Unknown stack->Parameters.DeviceIoControl.IoControlCode", io_control_code);   }		
		}
	}
	pIrp->IoStatus.Information = out_buf_sz;
	pIrp->IoStatus.Status = STATUS_SUCCESS; 
	IoCompleteRequest( pIrp, IO_NO_INCREMENT );
	return STATUS_SUCCESS;
}

void del_symbol_link(PWCHAR linkName)
{
	UNICODE_STRING UnLinkName; 
	RtlInitUnicodeString(&UnLinkName, linkName);
	IoDeleteSymbolicLink(&UnLinkName);
}

void DriverUnload(IN PDRIVER_OBJECT driverObject)
{
	KdPrint(("> DriverUnload \n"));
	del_symbol_link(VMM_DRV_NAME_LinkNameToDRV);

	PDEVICE_OBJECT p_next_device_obj = driverObject->DeviceObject;	
	while(p_next_device_obj != NULL)
	{
		IoDeleteDevice(p_next_device_obj);
		p_next_device_obj = p_next_device_obj->NextDevice;
	}
	KdPrint(("< DriverUnload \n"));
}

NTSTATUS OnCreateClose(IN struct _DEVICE_OBJECT *DeviceObject,IN struct _IRP *pIrp)
{
	KdPrint(( ">>OnCreateClose\r\n"));
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	switch (stack->MajorFunction) 
	{
	case IRP_MJ_CLOSE:
		KdPrint(("Close\r\n"));
		break;
	case IRP_MJ_CREATE:
		KdPrint(("Create\r\n"));
		break;
	}
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;	
	IoCompleteRequest( pIrp, IO_NO_INCREMENT );

	KdPrint(("<<OnCreateClose\r\n"));
	return STATUS_SUCCESS; 
}

NTSTATUS create_device(IN PDRIVER_OBJECT pDrv,  ULONG uFlags, PWCHAR devName, PWCHAR linkName)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	UNICODE_STRING DevName, LinkName;
	PDEVICE_OBJECT pDev;
	RtlInitUnicodeString(&DevName, devName);	
	RtlInitUnicodeString(&LinkName, linkName);

	if ((status=IoCreateDevice(pDrv, 0, &DevName, 65500, 0, 0, &pDev)) == STATUS_SUCCESS)
	{
		pDev->Flags |= uFlags;
		IoDeleteSymbolicLink(&LinkName);
		status = IoCreateSymbolicLink(&LinkName, &DevName);  
		KdPrint(("Create %ws [%08x]", linkName, status));
	}
	if (status != STATUS_SUCCESS)
	{   IoDeleteDevice(pDev);   }
	return status;
}

NTSTATUS DriverEntry( IN PDRIVER_OBJECT pDrv, IN PUNICODE_STRING pRegistryPath)
{
	KdPrint(("> DriverEntry \n"));

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	pDrv->DriverUnload = DriverUnload;
	pDrv->MajorFunction[IRP_MJ_CREATE] = 
	pDrv->MajorFunction[IRP_MJ_CLOSE] = OnCreateClose;
	pDrv->MajorFunction[IRP_MJ_DEVICE_CONTROL]= OnDispatch;

	status = create_device(pDrv, NULL, VMM_DRV_NAME_DeviceNameToDRV, VMM_DRV_NAME_LinkNameToDRV);

	KdPrint(("create_device= %08x \r\n",  status));

	KdPrint(("< DriverEntry \n"));
	return STATUS_SUCCESS;
} 
