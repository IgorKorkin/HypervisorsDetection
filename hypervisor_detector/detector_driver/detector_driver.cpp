
#include "detector_driver.h"


void del_symbol_link(PWCHAR linkName)
{
	UNICODE_STRING UnLinkName; 
	RtlInitUnicodeString(&UnLinkName, linkName);
	IoDeleteSymbolicLink(&UnLinkName);
}

void Unload(IN PDRIVER_OBJECT driverObject)
{
	KdPrint(("> Unload \n"));
	del_symbol_link(KARVMM_LINKNAME_GUI);

	PDEVICE_OBJECT p_next_device_obj = driverObject->DeviceObject;	
	while(p_next_device_obj != NULL)
	{
		IoDeleteDevice(p_next_device_obj);
		p_next_device_obj = p_next_device_obj->NextDevice;
	}
	KdPrint(("< Unload \n"));
}

NTSTATUS create_device(IN PDRIVER_OBJECT pDrv,  ULONG uFlags, PWCHAR devName, PWCHAR linkName)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	UNICODE_STRING DevName, LinkName;
	PDEVICE_OBJECT pDev;
	RtlInitUnicodeString(&DevName, devName);	
	RtlInitUnicodeString(&LinkName, linkName);

	if ((status=IoCreateDevice(pDrv, 0 /*sizeof(DEVICE_EXTENSION)*/, &DevName, 65500, 0, 0, &pDev)) == STATUS_SUCCESS)
	{
		pDev->Flags |= uFlags;
		IoDeleteSymbolicLink(&LinkName);
		status = IoCreateSymbolicLink(&LinkName, &DevName);  // Создаём символьную ссылку  			
	}
	if (status != STATUS_SUCCESS)
		{   IoDeleteDevice(pDev);   }
	return status;
}

NTSTATUS OnCreateClose (IN PDEVICE_OBJECT  pDrv, IN PIRP  Irp)
{	
	KdPrint(("> OnCreateClose \n"));
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	switch (stack->MajorFunction) 
	{
		case IRP_MJ_CREATE:
			KdPrint((" create \n"));
			break;
		case IRP_MJ_CLOSE:
			KdPrint((" close \n"));
			break;
	}
	Irp->IoStatus.Status = STATUS_SUCCESS;  
	Irp->IoStatus.Information = 0; 	
	IoCompleteRequest(Irp, 0);	 // завершения обработки пакета IRP без изменения приоритета
	KdPrint(("< OnCreateClose \n"));
	return STATUS_SUCCESS;
}

NTSTATUS OnWrite(IN PDEVICE_OBJECT  pDeviceObject, IN PIRP  Irp)
{
	KdPrint(("> OnWrite \n"));
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	if (stack->MajorFunction == IRP_MJ_WRITE) 
	{
		PVOID buf = NULL;
		ULONG buf_size = stack->Parameters.Write.Length;
		if (pDeviceObject->Flags & DO_BUFFERED_IO)
			{   buf = Irp->AssociatedIrp.SystemBuffer;   }
		else if (pDeviceObject->Flags & DO_DIRECT_IO)
			{   buf = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority );   }
		else
			{   buf = Irp->UserBuffer;   }
		Irp->IoStatus.Information = 0;
	}	
	Irp->IoStatus.Status = STATUS_SUCCESS; 
	IoCompleteRequest( Irp, IO_NO_INCREMENT); 
	KdPrint(("< OnWrite \n"));
	return STATUS_SUCCESS;
}

NTSTATUS OnRead(IN PDEVICE_OBJECT  pDeviceObject, IN PIRP  Irp)
{
	KdPrint(("> OnRead \n"));
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	if (stack->MajorFunction == IRP_MJ_READ) 
	{
		PVOID buf = NULL;
		ULONG buf_size = stack->Parameters.Read.Length;
		if (pDeviceObject->Flags & DO_BUFFERED_IO)
			{   buf = Irp->AssociatedIrp.SystemBuffer;   }
		else if (pDeviceObject->Flags & DO_DIRECT_IO)
			{   buf = MmGetSystemAddressForMdlSafe( Irp->MdlAddress, NormalPagePriority );   }
		else
			{   buf = Irp->UserBuffer;   }
		Irp->IoStatus.Information = 0;
	}
	Irp->IoStatus.Status = STATUS_SUCCESS; 
	IoCompleteRequest( Irp, IO_NO_INCREMENT); 
	KdPrint(("< OnRead \n"));
	return STATUS_SUCCESS;
}

void read_input_param(IN PIRP pIrp, OUT PVOID &inBuf, OUT DWORD &inBufSize, OUT LPVOID &outBuf, OUT DWORD &outBufSize)
{	
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	inBufSize = stack->Parameters.DeviceIoControl.InputBufferLength;
	outBufSize = stack->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG method = stack->Parameters.DeviceIoControl.IoControlCode & 0x03L;		
	switch(method)
	{
	case METHOD_BUFFERED:
		inBuf = pIrp->AssociatedIrp.SystemBuffer;		
		outBuf = pIrp->AssociatedIrp.SystemBuffer;
		break;
	case METHOD_IN_DIRECT:
		inBuf = pIrp->AssociatedIrp.SystemBuffer;						
		outBuf = MmGetSystemAddressForMdlSafe( pIrp->MdlAddress, NormalPagePriority );
		break;		
	case METHOD_OUT_DIRECT:
		inBuf = pIrp->AssociatedIrp.SystemBuffer;				
		outBuf = MmGetSystemAddressForMdlSafe( pIrp->MdlAddress, NormalPagePriority );	
		break;		
	case METHOD_NEITHER:			
		inBuf = stack->Parameters.DeviceIoControl.Type3InputBuffer;
		outBuf = pIrp->UserBuffer;
		break;
	}
}

NTSTATUS OnDispath(IN PDEVICE_OBJECT  pDeviceObject, IN PIRP  pIrp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);	
	PVOID in_buf = NULL, out_buf = NULL;
	ULONG in_buf_sz = 0, out_buf_sz = 0;
	NTSTATUS status = STATUS_INVALID_PARAMETER  ;
	ULONG_PTR info = 0 ;
	read_input_param(pIrp, in_buf, in_buf_sz, out_buf, out_buf_sz);
	if  (in_buf_sz == sizeof(TaskForMeasurement))
	{
		switch(stack->Parameters.DeviceIoControl.IoControlCode)
		{
		case IOCTL_BLOCK_OF_CPUID:
			timing_rdtsc :: get_time_trace_cpuid(in_buf, out_buf);
			status = STATUS_SUCCESS;
			info = in_buf_sz;		
			break;
		case IOCTL_BLOCK_OF_CPUID_WITH_DISABLED_CACHE:
			timing_rdtsc :: get_time_cache_trace_cpuid(in_buf, out_buf);
			status = STATUS_SUCCESS;
			info = in_buf_sz;
			break;	
		case IOCTL_WALK_MEM_NEITHER:
			timing_walk_mem :: get_walk_mem_time(in_buf, out_buf);
			status = STATUS_SUCCESS;
			info = in_buf_sz;
			break;
		case IOCTL_BLOCK_OF_VMXON:
			timing_rdtsc :: get_time_trace_vmxon(in_buf, out_buf);
			status = STATUS_SUCCESS;
			info = in_buf_sz;
			break;	
		case IOCTL_BLOCK_OF_VMXON_WITH_DISABLED_CACHE:
			timing_rdtsc :: get_time_cache_trace_vmxon(in_buf, out_buf);
			status = STATUS_SUCCESS;
			info = in_buf_sz;
			break;
		default: {}
		}
	}
	else
		{   status = STATUS_INVALID_DEVICE_REQUEST;   }
	pIrp->IoStatus.Information = info;
	pIrp->IoStatus.Status = status; 
	IoCompleteRequest( pIrp, IO_NO_INCREMENT); 
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry( IN PDRIVER_OBJECT pDrv, IN PUNICODE_STRING pRegistryPath)
{
	KdPrint(("> DriverEntry \n"));	

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	pDrv->DriverUnload=Unload;  	
	pDrv->MajorFunction[IRP_MJ_CREATE] = 
	pDrv->MajorFunction[IRP_MJ_CLOSE] = OnCreateClose;

 	pDrv->MajorFunction[IRP_MJ_READ] = OnRead;
 	pDrv->MajorFunction[IRP_MJ_WRITE] = OnWrite;
	pDrv->MajorFunction[IRP_MJ_DEVICE_CONTROL]= OnDispath;
	
	status = create_device(pDrv, NULL, KARVMM_DEVICENAME_DRV, KARVMM_LINKNAME_DRV);
	KdPrint(("create_neith_device= %08x \r\n",  status));

	KdPrint(("< DriverEntry \n"));
	return STATUS_SUCCESS;
} 