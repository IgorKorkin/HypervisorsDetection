/************************************************************************/
/*         

*/
/************************************************************************/
#ifndef __RAISEIRQL__H__
#define __RAISEIRQL__H__

#include "ntddk.h"

namespace raiseirql
{
	class RaiseIrql
	{
		/* started IRQL level*/
		KIRQL start_irql;

	public:
		/* get started IRQL level set specific IRQL level*/
		RaiseIrql(KIRQL kiRaiseIrql);

		/* low to started IRQL level*/
		~RaiseIrql();
	};
}

#endif //__RAISEIRQL__H__