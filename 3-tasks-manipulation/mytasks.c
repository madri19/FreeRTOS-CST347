/*
 * mytasks.c
 *
 * Created: 1/9/2020 6:17:02 PM
 *  Author: admin
 */ 
#include <asf.h>
#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include "myDefines.h"
#include "mytasks.h"
#include "ledDriver.h"
#include "buttonDriver.h"
#include "Uartdrv.h"


void taskMainControl(void * pvParamaters){
	
	// Process the pvParameter
	struct controlTaskStruct * controlParams =(struct controlTaskStruct*) pvParamaters;
	QueueHandle_t ledQueueParam = controlParams->ledQ;
	QueueHandle_t uartQueueParam = controlParams->uartQ;
	TaskHandle_t nextTaskHandleParam = *controlParams->nextTask;
	uint8_t ledParam = controlParams->led;
	
	// End of Polling, do a 100ms delay
	const TickType_t pollingDelay = 100 / portTICK_PERIOD_MS;
	const TickType_t buttonDelay = 10 / portTICK_PERIOD_MS;
	
	const char *newLEDstr1 = "LED 1 IS NOW ACTIVE\r\n";
	const char *newLEDstr2 = "LED 2 IS NOW ACTIVE\r\n";
	const char *newLEDstr3 = "LED 3 IS NOW ACTIVE\r\n";
	const char *fail1 = "FAILED TO ADD DECREMENT TO THE QUEUE\r\n";
	const char *fail2 = "FAILED TO ADD INCREMENT TO THE QUEUE\r\n";
	const char *fail3 = "FAILED TO SWITCH LEDS\r\n";
	
	int lockout = 0;
	Control_t message;
	
	
	while(true)
	{
		vTaskDelay(pollingDelay);
		/*
			EXT_SW1: This Switch will decrease the delay time of the corresponding LED task. It will accomplish this by sending a message to this
			task via the queue setup for this pair using the API function vQueueSendToBack() function.  The message will tell the LED
			task to decrease its delay time.
		*/
		if (readButton(1) == _ACTIVE_ && lockout == 0){
			vTaskDelay(buttonDelay);
			if(readButton(1) == _ACTIVE_ && lockout == 0){
				lockout = 1;
				message = DECREMENT;
				if(xQueueSendToBack(ledQueueParam, (void *)&message, (TickType_t) 10) != pdPASS)
				{
					/* Failed to post the message, even after 10 ticks. */
					xQueueSendToBack(uartQueueParam, fail1, (TickType_t)0);
				}
			}
		}
		/*
			EXT_SW2: This Switch will increase the delay time of the corresponding LED task. It will accomplish this by sending a message to
			this task via the queue setup for this pair. It will accomplish this by calling vQueueSendToBack() function.
		*/
		if (readButton(2) == _ACTIVE_ && lockout == 0){
			vTaskDelay(buttonDelay);
			if (readButton(2) == _ACTIVE_ && lockout == 0){
				lockout = 1;
				message = INCREMENT;
				if(xQueueSendToBack(ledQueueParam, (void *)&message, (TickType_t) 10) != pdPASS)
				{
					/* Failed to post the message, even after 10 ticks. */
					xQueueSendToBack(uartQueueParam, fail2, (TickType_t)0);
				}
			}
		}
		/*
			SW0: This Switch will cause the next LED to be selected. It will accomplish this by resuming the next MainControl (vTaskResume)
			Task and then suspending (vTaskSuspend) itself. So if LED1 is the current MainControl Task then after the SUSPEND/RESUME
			LED2 will be the active MainControl Task. During this operation a message will also be sent to the UART task to identify
			the new active LED. This message will be in the form of a char * and contain a message stating “LED N IS NOW ACTIVE”. Make
			sure that it includes the appropriate Line feeds to move cursor to the next line.

			The polling of the switches will happen in sequence and actions will be taken accordingly. After all three switches have 
			been polled, the task will perform a vTaskDelay() for 100ms. Error checking must be implemented to assure that the Message
			was sent to the appropriate queue with the vQueueSendtoBack().

			Suggestion: create an enum for the message to say either increment or decrement the delay time.
		*/
		if (readButton(0) == BUTTON_0_ACTIVE && lockout == 0){
			vTaskDelay(buttonDelay);
			if (readButton(0) == BUTTON_0_ACTIVE && lockout == 0){
				lockout = 1;
				if(ledParam == 3){
					xQueueSendToBack(uartQueueParam, newLEDstr1, (TickType_t)0);
				}
				else if (ledParam == 1){
					xQueueSendToBack(uartQueueParam, newLEDstr2, (TickType_t)0);
				}
				else if(ledParam == 2){
					xQueueSendToBack(uartQueueParam, newLEDstr3, (TickType_t)0);
				}
				else{
					xQueueSendToBack(uartQueueParam, fail3, (TickType_t)0);
				}
				vTaskResume(nextTaskHandleParam);
				vTaskSuspend(NULL);
			}
		}
		lockout = 0;
		vTaskDelay(pollingDelay);
	}
}


void taskLED(void * pvParamaters){
	
	// Process the pvParameter
	struct ledTaskstruct * ledParams =(struct ledTaskstruct*) pvParamaters;
	QueueHandle_t ledQueueParam = ledParams->ledQ;
	QueueHandle_t uartQueueParam = ledParams->uartQ;
	uint8_t ledParam = ledParams->led;
		
	int MSdefault = 500;
	TickType_t ledDelay = MSdefault / portTICK_PERIOD_MS;
	Control_t controlGetter;
	
	const char *fail4 = "UNKNOWN VALUE RECIEVED FROM LED QUEUE\r\n";
	const char *fail5 = "UNKNOWN VALUE OF LED\r\n";
	const char *confirmLED1d = "LED 1 HAS BEEN DECREMENTED\r\n";
	const char *confirmLED1i = "LED 1 HAS BEEN INCREMENTED\r\n";
	const char *confirmLED2d = "LED 2 HAS BEEN DECREMENTED\r\n";
	const char *confirmLED2i = "LED 2 HAS BEEN INCREMENTED\r\n";
	const char *confirmLED3d = "LED 3 HAS BEEN DECREMENTED\r\n";
	const char *confirmLED3i = "LED 3 HAS BEEN INCREMENTED\r\n";

	while(true)
	{
		if(uxQueueMessagesWaiting(ledQueueParam)){
			if(xQueueReceive(ledQueueParam, &controlGetter, (TickType_t)0) == pdTRUE){
				if(ledParam == 1)
					if(controlGetter == DECREMENT){
						//LED delay minus 50ms but min should be 200ms
						MSdefault -= 50;
						if(MSdefault < 200){
							MSdefault = 200;
						}
						ledDelay = MSdefault / portTICK_PERIOD_MS;
						xQueueSendToBack(uartQueueParam, confirmLED1d, (TickType_t)0);
					}
					else if(controlGetter == INCREMENT){
						//LED delay add 50ms but max should be 1000ms
						MSdefault  += 50;
						if(MSdefault < 1000){
							MSdefault = 1000;
						}
						ledDelay = MSdefault / portTICK_PERIOD_MS;
						xQueueSendToBack(uartQueueParam, confirmLED1i, (TickType_t)0);
					}
					else{
						// UNKNOWN VALUE CAME FROM LED QUEUE
						xQueueSendToBack(uartQueueParam, fail4, (TickType_t)0);
					}
				else if (ledParam == 2)
				{
					if(controlGetter == DECREMENT){
						//LED delay minus 50ms but min should be 200ms
						MSdefault  -= 50;
						if(MSdefault < 200){
							MSdefault = 200;
						}
						ledDelay = MSdefault / portTICK_PERIOD_MS;
						xQueueSendToBack(uartQueueParam, confirmLED2d, (TickType_t)0);
					}
					else if(controlGetter == INCREMENT){
						//LED delay add 50ms but max should be 1000ms
						MSdefault  += 50;
						if(MSdefault < 1000){
							MSdefault = 1000;
						}
						ledDelay = MSdefault / portTICK_PERIOD_MS;
						xQueueSendToBack(uartQueueParam, confirmLED2i, (TickType_t)0);
					}
					else{
						// UNKNOWN VALUE CAME FROM LED QUEUE
						xQueueSendToBack(uartQueueParam, fail4, (TickType_t)0);
					}
				}
				else if (ledParam == 3)
				{
					if(controlGetter == DECREMENT){
						//LED delay minus 50ms but min should be 200ms
						MSdefault  -= 50;
						if(MSdefault < 200){
							MSdefault = 200;
						}
						ledDelay = MSdefault / portTICK_PERIOD_MS;
						xQueueSendToBack(uartQueueParam, confirmLED3d, (TickType_t)0);
					}
					else if(controlGetter == INCREMENT){
						//LED delay add 50ms but max should be 1000ms
						MSdefault  += 50;
						if(MSdefault < 1000){
							MSdefault = 1000;
						}						
						ledDelay = MSdefault / portTICK_PERIOD_MS;
						xQueueSendToBack(uartQueueParam, confirmLED3i, (TickType_t)0);
					}
					else{
						// UNKNOWN VALUE CAME FROM LED QUEUE
						xQueueSendToBack(uartQueueParam, fail4, (TickType_t)0);
					}
				}
				else{
					// UNKNOWN VALUE CAME FROM LED QUEUE
					xQueueSendToBack(uartQueueParam, fail5, (TickType_t)0);
				}
			}
		}
		/* Simply toggle the LED, blocking between each toggle. */
		toggleLED(ledParam);
		vTaskDelay(ledDelay);
	}
	
}


void taskUART(void * pvParamaters){
	
	const char *helloTask = "UART Task is Enabled!\r\n";
	UARTPutStr(EDBG_UART, helloTask, 0);
	
	QueueHandle_t tempQueue = (QueueHandle_t ) pvParamaters;
	char tempSTR[50];
	
	while (true)
	{
		if(uxQueueMessagesWaiting(tempQueue)){
			if(xQueueReceive(tempQueue, &tempSTR, portMAX_DELAY) == pdTRUE){
				UARTPutStr(EDBG_UART, tempSTR, 0);
			}
		}
	}
}


void taskHeartbeat(void * pvParamaters){	   
	
	/* Block for 1 second. */
	const TickType_t heartbeatDelay = 1000 / portTICK_PERIOD_MS;

	while(true)
	{
		/* Simply toggle the LED every 1 second, blocking between each toggle. */
		toggleLED((int)pvParamaters);
		vTaskDelay(heartbeatDelay);
	}
}