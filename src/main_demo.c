
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

static const char command[] = "delay ";  // Command to detect in receive buffer
static const int buf_length = 255;       // Receiving buffer

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

    Message received_msg;
    uint8_t idx = 0;
    uint8_t cmd_length = strlen(command);
    char c;
    char buf[255];
   
    int delay;

    // Clear buffer
    memset(buf, 0, buf_length);


    //printf("command length:%u", cmd_len);

    /* Prevent the compiler warning about the unused parameter. */
    (void)pvParameters;


    for (; ; )
    {
        //Add delay here to allow queue processing of Task B's messages 
        //so it can be received here before being blocked by console input
        vTaskDelay(1);

        
        // Check if anything received without blocking, store in Message.body
        // Print if back to console
        if (xQueueReceive(messageQUEUE, (void*)&received_msg.body, 0) == pdTRUE) {
            printf(received_msg.body);
         
        }

        //Read characters from console
        c = getchar();
       
        // Store the received character in buffer until buffer is full
        if ((c!=EOF) && (idx < buf_length - 1)) {
            buf[idx] = c;
            idx++;
        }
      
        // If a newline character is received print it and process the buffer
        // If delay command is detected send it via queue
        // Reset buffer
        if ((c == '\n') || (c == '\r')) {

            putchar('\r');
            putchar('\n');

            // Check if the delay plus space char is found
            if (memcmp(buf, command, cmd_length) == 0) {
        
                printf("delay detected\n");
                // Capture the char after the space 
                char* tail = buf + cmd_length;

                //Check if the character is a digit and store it as delay value
                //If not a digit, the command is not valid
                if (isdigit(tail[0]))
                {
                    delay = atoi(tail);
                    delay = abs(delay);

                    // Send the integer value to Task B
                    xQueueSend(delayQUEUE, (void*)&delay, 10);                     
                }
                else
                {
                    printf("ERROR:Parameter not a number\n");

                }
               
            }

            // Reset buffer and counter
            memset(buf, 0, buf_length);
            idx = 0;

        }
        else if (c!=EOF)
        {
                // If not newline, echo character back to console
        
            putchar(c);
        }
    }

    
}


static void updateDelayTask(void* pvParameters)
{
    /* Prevent the compiler warning about the unused parameter. */
    (void)pvParameters;

    Message msg;
    int delay = 0;


    for (; ; )
    {
        
        // Check if anything received without blocking, store in int
        if (xQueueReceive(delayQUEUE, (void*)&delay, 0) == pdTRUE) {

            //Construct the message body to be sent back to Task A
            snprintf(msg.body, sizeof(msg.body), "Delayed by: %d", delay);
            
            //send to Queue
            xQueueSend(messageQUEUE, (void*)&msg.body, 10);

        }

    }
}