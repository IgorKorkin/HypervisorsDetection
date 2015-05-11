#ifndef __PHYMEM_H__
#define __PHYMEM_H__

#include "raiseirql.h"
#include "detector_driver.h"

namespace timing_walk_mem
{
	void get_walk_mem_time( PVOID pTraceFeatures, PVOID pTimeInterval );
}




#endif // __PHYMEM_H__