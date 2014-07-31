#include "ISR.h"
#include "IRQ.h"
#include <stddef.h>

static
void null_ISR (__attribute__ ((unused)) INT_index interrupt)
{
}


// Extern functions

ISR_t ISR_table [INT_LIMIT];

void ISR_table_initialize (ISR_t default_ISR)
{
	if (default_ISR == NULL)
		default_ISR = &null_ISR;
	for (size_t i = 0; i < INT_LIMIT; ++i)
		ISR_table [i] = default_ISR;
}

void ISR_entry (uint32_t interrupt)
{
	if (interrupt == INT_spurious && !IRQ_in_service (INT_LPT1))
		return;

	(*ISR_table [interrupt]) (interrupt);

	if (INT_IRQ_MBASE <= interrupt && interrupt < INT_IRQ_SBASE)
		IRQ_EOI_master ();
	else if (INT_IRQ_SBASE <= interrupt)
		IRQ_EOI_slave ();
}
