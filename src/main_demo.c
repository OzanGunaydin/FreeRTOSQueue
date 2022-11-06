
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"


/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY    ( tskIDLE_PRIORITY + 1)
#define mainQUEUE_SEND_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1)


 /* The number of items the queue can hold at once. */
#define messageQUEUE_LENGTH                   ( 5 )
#define delayQUEUE_LENGTH                     ( 5 )



/*-----------------------------------------------------------*/

 /*
  * The tasks as described in the comments at the top of this file.
  */
static void doInputOutputTask(void* pvParameters);
static void updateDelayTask(void* pvParameters);


/* Message Queue (Queue 2) from TASK B to TASK A*/
static QueueHandle_t messageQUEUE = NULL;


/* Delay Queue (Queue 1) from TASK A to TASK B */
static QueueHandle_t delayQUEUE = NULL;


// Data Structure used in Message Queue
typedef struct Message {
    char body[20];
} Message;


static const int buf_len = 255;          // Receiving buffer
static const char command[] = "delay ";  // Command to detect in receive buffer

/*-----------------------------------------------------------*/

void main(void)
{

     /* Create the queues. */
    messageQUEUE = xQueueCreate(messageQUEUE_LENGTH, sizeof(Message));
    delayQUEUE = xQueueCreate(delayQUEUE_LENGTH, sizeof(uint32_t));

    //Check if the queues are created successfully
    if ((messageQUEUE != NULL) && (delayQUEUE != NULL))
    {
    
        xTaskCreate(doInputOutputTask,             /* The function that implements the task. */
            "SerialComms",                            /* The text name assigned to the task - for debug only as it is not used by the kernel. */
            configMINIMAL_STACK_SIZE,              /* The size of the stack to allocate to the task. */
            NULL,                                     /* The parameter passed to the task - not used in this simple case. */
            mainQUEUE_RECEIVE_TASK_PRIORITY,        /* The priority assigned to the task. */
            NULL);                               /* The task handle is not required, so NULL is passed. */

        xTaskCreate(updateDelayTask,
            "DelayControl",
            configMINIMAL_STACK_SIZE,
            NULL,
            mainQUEUE_SEND_TASK_PRIORITY,
            NULL);


            /* Start the tasks running. */
        vTaskStartScheduler();
    }

    /* If all is well, the scheduler will now be running, and the following
     * line will never be reached.  If the following line does execute, then
     * there was insufficient FreeRTOS heap memory available for the idle and/or
     * timer tasks  to be created.  See the memory management section on the
     * FreeRTOS web site for more details. */
    for (; ; )
    {
    }
}
/*-----------------------------------------------------------*/


static void doInputOutputTask(void* pvParameters)
{
}


static void updateDelayTask(void* pvParameters)
{
    
}