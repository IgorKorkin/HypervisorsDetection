#ifndef RUN_STOP_VMM_H
#define RUN_STOP_VMM_H

#include "driver_entry.h"
#include "hypervisor_dispatcher.h"
#include "load_hypervisor.h"

NTSTATUS RunVMMCPU0();
NTSTATUS StopVMMCPU0();

#endif // RUN_STOP_VMM_H

