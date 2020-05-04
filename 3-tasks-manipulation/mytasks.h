/*
 * mytasks.h
 *
 * Created: 1/9/2020 6:16:21 PM
 *  Author: admin
 */ 
#ifndef MYTASKS_H_
#define MYTASKS_H_


// Struct definitions
struct controlTaskStruct{
	QueueHandle_t ledQ;
	QueueHandle_t uartQ;
	TaskHandle_t * nextTask;
	uint8_t led;
};

struct ledTaskstruct{
	QueueHandle_t ledQ;
	QueueHandle_t uartQ;
	uint8_t led;
};

typedef enum Control_t {DECREMENT, INCREMENT} Control_t;

void taskMainControl(void * pvParamaters);
void taskLED(void * pvParamaters);
void taskUART(void * pvParamaters);
void taskHeartbeat(void * pvParamaters);


#endif /* MYTASKS_H_ */