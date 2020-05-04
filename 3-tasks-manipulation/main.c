//system includes
#include <asf.h>

// FreeRTOS Includes
#include <FreeRTOS.h>
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"

// My Includes
#include "ledDriver.h"
#include "buttonDriver.h"
#include "mytasks.h"
#include "Uartdrv.h"


// Defines
#if( BOARD == SAM4E_XPLAINED_PRO )       // Used to place the heap
#define mainHEAP_START            _estack
#define mainRAM_LENGTH            0x00020000    /* 128 KB of internal SRAM. */
#define mainRAM_START             0x20000000    /* at 0x20000000 */
/* The SAM4E Xplained board has 2 banks of external SRAM, each one 512KB. */
#define mainEXTERNAL_SRAM_SIZE           ( 512ul * 1024ul )
#else
#error Define memory regions here.
#endif /* SAM4E_XPLAINED_PRO */

// Function Prototypes
static void prvMiscInitialisation( void );
static void prvInitialiseHeap( void );
void vApplicationMallocFailedHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );


// Used to place the heap
extern char _estack;


// Enum for LED's
typedef enum Led_t {LED0, LED1, LED2, LED3} Led_t;
	
// Task Creation Handling
TaskHandle_t xHandle[3] = {NULL, NULL, NULL}; //only for control tasks
	
// Struct creations
struct ledTaskstruct ledTask1;
struct ledTaskstruct ledTask2;
struct ledTaskstruct ledTask3;
struct controlTaskStruct controlLED1;
struct controlTaskStruct controlLED2;
struct controlTaskStruct controlLED3;

int main (void)
{
	// Initialize The Board
	prvMiscInitialisation();
	
	// Initialize My Drivers
	initializeLEDDriver();
	initializeButtonDriver();
	initUART(EDBG_UART);
	
	const char *welcome = "\r\n\r\n\r\n\r\n\r\n\r\n\r\n_______Welcome To UART0_______\r\n";
	UARTPutStr(EDBG_UART, welcome, 0);
	
	// Create queue handler with queues
	QueueHandle_t QUEUE_HANDLER[4] = {
		xQueueCreate(5, sizeof(Control_t)),
		xQueueCreate(5, sizeof(Control_t)),
		xQueueCreate(5, sizeof(Control_t)),
		xQueueCreate(5, sizeof(char[50]))
	}; // 0 = LED1, 1 = LED2, 2 = LED3, 3 = UART
	
	
	// Structs for pvPARAMETER in tasks
	ledTask1.ledQ = QUEUE_HANDLER[0];
	ledTask1.uartQ = QUEUE_HANDLER[3];
	ledTask1.led = LED1;
	ledTask2.ledQ = QUEUE_HANDLER[1];
	ledTask2.uartQ = QUEUE_HANDLER[3];
	ledTask2.led = LED2;
	ledTask3.ledQ = QUEUE_HANDLER[2];
	ledTask3.uartQ = QUEUE_HANDLER[3];
	ledTask3.led = LED3;
	
	controlLED1.ledQ = QUEUE_HANDLER[0];
	controlLED1.uartQ = QUEUE_HANDLER[3];
	controlLED1.nextTask = &xHandle[1];
	controlLED1.led = LED1;
	controlLED2.ledQ = QUEUE_HANDLER[1];
	controlLED2.uartQ = QUEUE_HANDLER[3];
	controlLED2.nextTask = &xHandle[2];
	controlLED2.led = LED2;
	controlLED3.ledQ = QUEUE_HANDLER[2];
	controlLED3.uartQ = QUEUE_HANDLER[3];
	controlLED3.nextTask = &xHandle[0];
	controlLED3.led = LED3;		
	
	
	// Create a Task to toggle the On Board LED(heartBeat)
	xTaskCreate(taskHeartbeat, "My Heartbeat Task", configMINIMAL_STACK_SIZE, LED0, 1, NULL);
	
	// Create the UART Task
	xTaskCreate(taskUART, "My UART Task", configMINIMAL_STACK_SIZE, (void *) QUEUE_HANDLER[3], 1, NULL);
	
	//// Create a Task to toggle LED1, LED2, LED3
	xTaskCreate(taskLED, "My EXT_LED1 Task", configMINIMAL_STACK_SIZE, &ledTask1, 1, NULL);
	xTaskCreate(taskLED, "My EXT_LED2 Task", configMINIMAL_STACK_SIZE, &ledTask2, 1, NULL);
	xTaskCreate(taskLED, "My EXT_LED3 Task", configMINIMAL_STACK_SIZE, &ledTask3, 1, NULL);
	
	// Create the 3 control tasks and suspend 2 of them before start scheduler
	xTaskCreate(taskMainControl, "My EXT_LED1 Control Task", configMINIMAL_STACK_SIZE, (void *) &controlLED1, 1, &xHandle[0]);
	xTaskCreate(taskMainControl, "My EXT_LED2 Control Task", configMINIMAL_STACK_SIZE, (void *) &controlLED2, 1, &xHandle[1]);
	xTaskCreate(taskMainControl, "My EXT_LED3 Control Task", configMINIMAL_STACK_SIZE, (void *) &controlLED3, 1, &xHandle[2]);
	vTaskSuspend(xHandle[1]);
	vTaskSuspend(xHandle[2]);
	
	// Start The Scheduler
	vTaskStartScheduler();

	while(true)
	{ }
		
	return 0;
}


static void prvInitialiseHeap( )
{
       uint32_t ulHeapSize;
       uint8_t *pucHeapStart, *pucHeapEnd;

       pucHeapStart = ( uint8_t * ) ( ( ( ( uint32_t ) &mainHEAP_START ) + 7 ) & ~0x07ul );
       pucHeapEnd = ( uint8_t * ) ( mainRAM_START + mainRAM_LENGTH );
       ulHeapSize = ( uint32_t ) ( ( uint32_t ) pucHeapEnd - ( uint32_t ) &mainHEAP_START );
       ulHeapSize &= ~0x07ul;
       ulHeapSize -= 1024;
       HeapRegion_t xHeapRegions[] = {
              { ( unsigned char *) pucHeapStart, ulHeapSize },
              { ( unsigned char *) SRAM_BASE_ADDRESS, mainEXTERNAL_SRAM_SIZE },
              { ( unsigned char *) SRAM_BASE_ADDRESS_2ND, mainEXTERNAL_SRAM_SIZE },
              { NULL, 0 }
       };

       vPortDefineHeapRegions( xHeapRegions );
}

static void prvMiscInitialisation( void )
{
       /* Initialize the SAM system */
       sysclk_init();
       board_init();
       prvInitialiseHeap();
       pmc_enable_periph_clk(ID_PIOA);
       pmc_enable_periph_clk(ID_PIOB);
}

void vAssertCalled( const char *pcFile, uint32_t ulLine )
{
       volatile uint32_t ulBlockVariable = 0UL;

       /* Setting ulBlockVariable to a non-zero value in the debugger will allow
       this function to be exited. */
       taskDISABLE_INTERRUPTS();
       {
              while( ulBlockVariable == 0UL )
              {
                     __asm volatile( "NOP" );
              }
       }
       taskENABLE_INTERRUPTS();
}

void vApplicationMallocFailedHook( void )
{
       /* Called if a call to pvPortMalloc() fails because there is insufficient
       free memory available in the FreeRTOS heap.  pvPortMalloc() is called
       internally by FreeRTOS API functions that create tasks, queues, software
       timers, and semaphores.  The size of the FreeRTOS heap is set by the
       configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
       vAssertCalled( __FILE__, __LINE__ );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
       ( void ) pcTaskName;
       ( void ) pxTask;

       /* Run time stack overflow checking is performed if
       configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
       function is called if a stack overflow is detected. */
       taskDISABLE_INTERRUPTS();

       for( ;; );
}