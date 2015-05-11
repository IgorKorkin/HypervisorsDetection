/************************************************************************/
/*                                                                      */
/************************************************************************/

#include "raiseirql.h"

namespace raiseirql
{
	/* get started IRQL level set specific IRQL level*/
	RaiseIrql :: RaiseIrql(KIRQL kiRaiseIrql)
	{
		start_irql = PASSIVE_LEVEL;
		start_irql = KfRaiseIrql(kiRaiseIrql);
	}

	/* low to started IRQL level*/
	RaiseIrql :: ~RaiseIrql()
		{   KeLowerIrql(start_irql);   }
}