#ifndef __PRINT_H_
#define __PRINT_H_

#include <windows.h>

namespace print
{
	void print_mes(char *fmt, ...);
	void print_mes(TCHAR *fmt, ...);
	void print_last_err(char *fmt, ...);
	void print_last_err(TCHAR *fmt, ...);
}

#endif // __PRINT_H_