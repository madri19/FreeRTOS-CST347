/*
 * mytasks.c
 *
 * Created: 1/9/2020 6:17:02 PM
 *  Author: admin
 */ 

#include <asf.h>
#include <FreeRTOS.h>
#include "task.h"
#include "mytasks.h"

void taskHeartbeat(void * pvParamaters)
{	   
	/* Block for 500ms. */
	const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

	for( ;; )
	{
		/* Simply toggle the LED every 500ms, blocking between each toggle. */
		/* Is led off? */
		if (ioport_get_pin_level(LED_0_PIN) == LED_0_INACTIVE)
		{
			/* Yes, so turn LED on. */
			ioport_set_pin_level(LED_0_PIN, LED_0_ACTIVE);
		}
		else
		{
			/* No, so turn LED off. */
			ioport_set_pin_level(LED_0_PIN, !LED_0_ACTIVE);
		}
		vTaskDelay( xDelay );
	}
}