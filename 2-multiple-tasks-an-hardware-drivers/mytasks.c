/*
 * mytasks.c
 *
 * Created: 1/9/2020 6:17:02 PM
 *  Author: admin
 */ 

#include <asf.h>
#include <FreeRTOS.h>
#include "task.h"
#include "myDefines.h"
#include "mytasks.h"
#include "ledDriver.h"
#include "buttonDriver.h"




void taskSystemControl(void * pvParamaters){
	
	// Variable to control which LED to act on
	int LEDController = 0;
	
	// To know taskSuspended status
	int suspended = 0;
	// Counters for suspend tracking
	int counter, counter2;
	
	// Task Creation Handling
	TaskHandle_t xHandle[3] = {NULL, NULL, NULL};
	
	// End of Polling, do a 100ms delay
	const TickType_t pollingDelay = 100 / portTICK_PERIOD_MS;
	
	// Create a Task to toggle the On Board LED(heartBeat)
	xTaskCreate(taskHeartbeat, "My Heartbeat Task", configMINIMAL_STACK_SIZE, 0, 1, NULL);
	
	while(true)
	{
		// Is EXT_SW1 Pressed?*******************************************************************************************************
		if (readButton(1) == _ACTIVE_)
		{
			if (LEDController == 0 || LEDController == 1 || LEDController == 2)
			{
				LEDController = LEDController + 1;
				// Create a Task to turn on LED
				xTaskCreate(taskHeartbeat, "My LED Task", configMINIMAL_STACK_SIZE, (void *) LEDController, 1, &xHandle[LEDController-1]);
			}
		}
		// Is EXT_SW2 Pressed?*******************************************************************************************************
		if (readButton(2) == _ACTIVE_)
		{
			// We don't want to delete tasks if the tasks are suspended, this also helps control: LEDController
			if(suspended == 0)
			{
				if (LEDController == 1 || LEDController == 2 || LEDController == 3)
				{
					// Delete the LED Task!
					vTaskDelete(xHandle[LEDController-1]);
					LEDController = LEDController - 1;
				}
			}
		}
		//Is SW0 (Onboard Switch Button) Pressed?************************************************************************************
		if (readButton(0) == BUTTON_0_ACTIVE)
		{
			if(suspended == 0)
			{
				counter = LEDController;
				counter2 = counter;
				
				while(counter)
				{
					// Suspend the LED Tasks!
					vTaskSuspend(xHandle[counter-1]);
					counter--;
				}
				suspended = 1;
		}
		else
			{
				while(counter2)
				{
					// Resume the suspended tasks
					vTaskResume(xHandle[counter2-1]);
					counter2--;
				}
				suspended = 0;
			}
		}
		vTaskDelay(pollingDelay);
	}
}


void taskHeartbeat(void * pvParamaters){	   
	
	/* Block for 1 second. */
	const TickType_t heartbeatDelay = 1000 / portTICK_PERIOD_MS;
	/* Block for 500ms. */
	const TickType_t ledDelay = 500 / portTICK_PERIOD_MS;

	while(true)
	{
		/* Simply toggle the LED every 1 second, blocking between each toggle. */
		toggleLED(pvParamaters);
		if (pvParamaters == 0)
		{
			vTaskDelay(heartbeatDelay);
		}
		else
		{
			vTaskDelay(ledDelay);
		}
	}
}