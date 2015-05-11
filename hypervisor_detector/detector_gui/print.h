#ifndef __PRINT_H_
#define __PRINT_H_

#include "vmmkar_guidlg.h"
#include "vmmkar_gui.h"

namespace print
{
	void print_mes(char *fmt, ...);
	void print_mes(TCHAR *fmt, ...);
	void print_last_err(char *fmt, ...);
	void print_last_err(TCHAR *fmt, ...);

	void init_progressbar();
	void set_progressbar(int iPos);
}

#endif // __PRINT_H_