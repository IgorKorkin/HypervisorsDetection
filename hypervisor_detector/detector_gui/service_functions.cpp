#include "stdafx.h"

#include "service_functions.h"

namespace service_functions
{
	ServiceManager :: ServiceManager()
	{
// 		service_name[0] = 0;
// 		display_name[0] = 0;
		driver_name[0] = 0;
		driver_bin_path[0] = 0;
		symbol_link[0] = 0;
		handle_device = INVALID_HANDLE_VALUE;
		handle_scmanager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	}

	
	ServiceManager :: ~ServiceManager()
	{
		close_device(handle_device);
		stop_driver();
		remove_driver();
		CloseServiceHandle(handle_scmanager);
		handle_scmanager = NULL;
	}

	
	bool ServiceManager :: init(LPCTSTR driverName, LPCTSTR driverBinPath)
	{
		return handle_scmanager && driverName && !_tcscpy_s(driver_name, driverName) && 
// 					!_tcscpy_s(service_name, driverName) && !_tcscat_s(service_name, TEXT("_service_name")) &&
// 					!_tcscpy_s(display_name, driverName) && !_tcscat_s(display_name, TEXT("_display_name")) &&
					driverBinPath && !_tcscpy_s(driver_bin_path, driverBinPath);
	}

	
	bool ServiceManager :: add_driver()
	{
		bool b_res = false;
		// SC_HANDLE service = CreateService (handle_scmanager, service_name, display_name, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, driver_bin_path, NULL, NULL, NULL, NULL, NULL);
		
		SC_HANDLE service = CreateService (handle_scmanager, driver_name, driver_name, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, driver_bin_path, NULL, NULL, NULL, NULL, NULL);

 		if ( service )
 			{   b_res = true;   }

		CloseServiceHandle(service);
		return b_res;
	}


	SC_HANDLE ServiceManager :: open_service()
	{
		SC_HANDLE handle_service = NULL;
		if (handle_scmanager)
		{
			handle_service = OpenService(handle_scmanager, driver_name, SERVICE_ALL_ACCESS);
		}
		return handle_service;
	}


	bool ServiceManager :: remove_driver()
	{
		bool b_res = false;
		SC_HANDLE service = open_service();
		if (service)
		{
			if (DeleteService(service))
				{   b_res = true;   }
		}
		CloseServiceHandle(service);
		return b_res;
	}

	void ServiceManager :: delete_binfile()
	{
		DeleteFile(driver_bin_path);
	}


	bool ServiceManager :: start_driver()
	{
		bool b_res = false;
		SC_HANDLE service = open_service();
		if (service)
		{	
			BOOLEAN bEnabled = true; // we have to load driver with SeLoadDriverPrivileges
 			if(RtlAdjustPrivilege(SE_LOAD_DRIVER_PRIVILEGE, TRUE, FALSE, &bEnabled) == 0) 
			{
				if (StartService(service, 0, NULL)) 
					{   b_res = true;   }
			}
		}
		CloseServiceHandle(service);
		return b_res;
	}

	
	bool ServiceManager :: stop_driver()
	{
		bool b_res = false;
		SC_HANDLE service = open_service();
		if (service)
		{
			SERVICE_STATUS service_status = {0};
			if (ControlService(service, SERVICE_CONTROL_STOP, &service_status))
				{   b_res = true;   }
		}
		CloseServiceHandle(service);
		return b_res;
	}


	HANDLE ServiceManager :: open_device(PCTCH symbolLink)
	{
		handle_device = CreateFile(symbolLink, FILE_ALL_ACCESS, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
		return handle_device;
	}


	void ServiceManager :: close_device(HANDLE &hDevice)
	{
		if (hDevice != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hDevice);
			hDevice = INVALID_HANDLE_VALUE;
		}
	}

	
	int ServiceManager :: read(HANDLE hDevice, LPVOID lpBuffer, DWORD nNumberOfBytesToRead)
	{
		DWORD number_of_bytes_read = -1;
		if (!ReadFile(hDevice, lpBuffer, nNumberOfBytesToRead, &number_of_bytes_read, NULL))
			{   number_of_bytes_read = -1;   }
		return number_of_bytes_read;
	}

	
	int ServiceManager :: write(HANDLE hDevice, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
	{	
		DWORD number_of_bytes_written = -1;
		if (!WriteFile(hDevice, lpBuffer, nNumberOfBytesToWrite, &number_of_bytes_written, NULL))
			{   number_of_bytes_written = -1;   }
		return number_of_bytes_written;
	}

	
	bool ServiceManager::send_ctrl_code( HANDLE hDevice, DWORD ctrlCode, LPVOID inBuf, DWORD inBufSize, LPVOID outBuf, DWORD outBufSize, LPOVERLAPPED lpOverlapped )
	{
		DWORD number_of_bytes_returned = -1;
		bool b_res = false;
		if (DeviceIoControl(hDevice, ctrlCode, inBuf, inBufSize, outBuf, outBufSize, &number_of_bytes_returned, lpOverlapped))
			{   b_res = true;   }
		return b_res;
	}

	/* check the presence of the driver in the list of active drivers */
	bool ServiceManager :: chk_service_active()
	{
		bool b_res = false;
		DWORD bytes_needed = 0;
		DWORD resume_handle = 0;
		DWORD num_service_entries = 0;
		EnumServicesStatus(handle_scmanager, SERVICE_TYPE_ALL, SERVICE_STATE_ALL,  NULL, 0, &bytes_needed, &num_service_entries, &resume_handle);
		
		LPENUM_SERVICE_STATUS m_pStatus = (LPENUM_SERVICE_STATUS)_aligned_malloc( bytes_needed , 4096);
		if (m_pStatus)
		{
			memset( m_pStatus, 0, bytes_needed );

			if (EnumServicesStatus( handle_scmanager, SERVICE_DRIVER, SERVICE_ACTIVE, m_pStatus, bytes_needed, &bytes_needed, &num_service_entries, &resume_handle))
			{	
				for (unsigned int i = 0 ; i < num_service_entries ; i++ )
				{
					if (wcsstr( m_pStatus[i].lpServiceName, driver_name /*service_name*/) && 
						wcsstr( m_pStatus[i].lpDisplayName, driver_name /*display_name*/))
					{   b_res = true;   break;   }
				}
			}
		}
		if (m_pStatus)   {   _aligned_free(m_pStatus);   }
		
		return b_res;
	}
}
