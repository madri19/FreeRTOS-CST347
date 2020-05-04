#ifndef FREERTOSCONFIG_H_
#define FREERTOSCONFIG_H_

#include "system_sam4e.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/
#define configUSE_PREEMPTION                                  1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION               1
#define configUSE_QUEUE_SETS                                  1
#define configUSE_IDLE_HOOK                                   0
#define configUSE_TICK_HOOK                                   0
#define configCPU_CLOCK_HZ                                    ( SystemCoreClock )
#define configTICK_RATE_HZ                                    ( 1000 )
#define configMAX_PRIORITIES                                  8
#define configMINIMAL_STACK_SIZE                              ( ( unsigned short ) 130 )
#define configTOTAL_HEAP_SIZE                                 ((size_t)0x13000) /* Not used as heap_5.c is being built, see http://www.freertos.org/a00111.html. */
#define configMAX_TASK_NAME_LEN                               20
#define configUSE_TRACE_FACILITY                              0
#define configUSE_16_BIT_TICKS                                0
#define configIDLE_SHOULD_YIELD                               1
#define configUSE_MUTEXES                                     1
#define configQUEUE_REGISTRY_SIZE                             8
#define configCHECK_FOR_STACK_OVERFLOW                        2
#define configUSE_RECURSIVE_MUTEXES                           1
#define configUSE_MALLOC_FAILED_HOOK                          1
#define configUSE_APPLICATION_TASK_TAG                        0
#define configUSE_COUNTING_SEMAPHORES                         1
#define configUSE_TASK_NOTIFICATIONS                          1
#define configUSE_TICKLESS_IDLE                               0            
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS               3 /* FreeRTOS+FAT requires 3 pointers ifa CWD is supported. */
#define configUSE_STATS_FORMATTING_FUNCTIONS                  1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                                 0

/* Software timer definitions. */
#define configUSE_TIMERS                                      1
#define configTIMER_TASK_PRIORITY                             ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH                              5
#define configTIMER_TASK_STACK_DEPTH                          ( configMINIMAL_STACK_SIZE * 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet                       0
#define INCLUDE_uxTaskPriorityGet                      0
#define INCLUDE_vTaskDelete                                   0
#define INCLUDE_vTaskCleanUpResources                  0
#define INCLUDE_vTaskSuspend                                  0
#define INCLUDE_vTaskDelayUntil                               0
#define INCLUDE_vTaskDelay                                    1
#define INCLUDE_xTaskGetCurrentTaskHandle       0
#define INCLUDE_xTaskGetSchedulerState                 0
#define INCLUDE_pcTaskGetTaskName                      1
#define INCLUDE_xTimerPendFunctionCall                 1

/* Set configINCLUDE_QUERY_HEAP_COMMAND to 1 to include the "query-heap" command
in the CLI. */
#define configINCLUDE_QUERY_HEAP_COMMAND        1

/* Cortex-M specific definitions.  http://www.freertos.org/RTOS-Cortex-M3-M4.html */
#ifdef __NVIC_PRIO_BITS
 /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
 #define configPRIO_BITS          __NVIC_PRIO_BITS
#else
 #define configPRIO_BITS          4      /* 15 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY        15

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
void vAssertCalled( const char *pcFile, uint32_t ulLine );
#define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ );

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler           SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

/*
If not defined, the local broadcast address will be used, e.g. 192.168.0.255
*/

#endif /* FREERTOSCONFIG_H_ */