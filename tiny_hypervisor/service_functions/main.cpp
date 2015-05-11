
#include "service_functions.h"
#include "stdio.h"

TCHAR driver_name[] = TEXT("test_drv.sys");
TCHAR driver_bin_path[] = TEXT("C:\\123\\test_drv.sys");

WCHAR link_name_direct[] = L"\\\\.\\LinkName_DIRECT";
WCHAR link_name_buff[] = L"\\\\.\\LinkName_BUFFERED";
WCHAR link_name_neith[] = L"\\\\.\\LinkName_NEITHER";

#define FILE_DEVICE_IOCTL 0x00008301
#define IOCTL_MY_NEITHER   CTL_CODE(FILE_DEVICE_IOCTL, 0x800, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define IOCTL_MY_BUFFERED  CTL_CODE(FILE_DEVICE_IOCTL, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MY_INDIRECT  CTL_CODE(FILE_DEVICE_IOCTL, 0x802, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_MY_OUTDIRECT CTL_CODE(FILE_DEVICE_IOCTL, 0x803, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)


void test_read_write(service_functions :: ServiceManager &csmgr, HANDLE h)
{
	int i_rdbuf = 10;
	char rdbuf[10] = {0};
	int rdbytes = csmgr.read(h, rdbuf, i_rdbuf);

	int i_wrbuf = 10;
	char wrbuf[10] = "123456789";
	int wrbytes = csmgr.write(h, wrbuf, i_wrbuf);

	i_rdbuf = 10;
	rdbuf[0] = 0;
	rdbytes = csmgr.read(h, rdbuf, i_rdbuf);
}

void test_send_ctl(service_functions :: ServiceManager &csmgr, HANDLE h, DWORD ctlCode)
{
	int sz_in_buf = 10;
	char in_buf[10] = "123456789";
	int sz_out_buf = 10;
	char out_buf[10] = { 0 };
	int i_res = csmgr.send_ctrl_code(h, ctlCode, in_buf , sz_in_buf, out_buf, sz_out_buf, NULL);
}



void __TEST()
{
	HANDLE h_direct = NULL, h_buffered = NULL, h_neither = NULL;
	service_functions :: ServiceManager csmgr;

	if (csmgr.init(driver_name, driver_bin_path) && csmgr.add_driver() && csmgr.start_driver())
	{
		if (csmgr.chk_service_active())
		{
			if ((h_direct = csmgr.open_device(link_name_direct)) != (HANDLE)INVALID_HANDLE_VALUE )
			{
				test_read_write(csmgr, h_direct);
				test_send_ctl(csmgr, h_direct, IOCTL_MY_INDIRECT);
				test_send_ctl(csmgr, h_direct, IOCTL_MY_OUTDIRECT);
				csmgr.close_device(h_direct);
			}
			
			if ((h_buffered = csmgr.open_device(link_name_buff)) != (HANDLE)INVALID_HANDLE_VALUE )
			{
				test_read_write(csmgr, h_buffered);
				test_send_ctl(csmgr, h_buffered, IOCTL_MY_BUFFERED);
				csmgr.close_device(h_buffered);
			}
			
			if ((h_neither = csmgr.open_device(link_name_neith)) != (HANDLE)INVALID_HANDLE_VALUE )
			{
				test_read_write(csmgr, h_neither);
				test_send_ctl(csmgr, h_neither, IOCTL_MY_NEITHER);
				csmgr.close_device(h_neither);
			}
		}
	}
	if (csmgr.stop_driver() && csmgr.remove_driver())
		{   printf("+");   }
}

int main()
{
	__TEST();
	return 0;
}