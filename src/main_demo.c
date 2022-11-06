/*
===============================================================================
* FreeRTOS Queue demo
* 
* Author: Ozan Gunaydin
* Oct 2022
================================================================================
*
 * main_demo() creates two queues and two tasks. It then
 * starts the scheduler.
 *
 * Task A:
 * Task A is implemented by the doInputOutputTask() function in
 * this file. It reads the console input and echoes every character back. Meanwhile
 * monitors if "delay X\n" received in the console input where X must be a digit. 
 * When detected extracts the number and sends that number to Queue 1 (delay Queue).
 * If delay command received without a number following it is not submitted to queue
 * and an ERROR is printed. Also monitors Queue 2 (Message Queue) and prints everything 
 * received to console output.
 *
 * Task B:
 * The queue receive task is implemented by the updateDelayTask() function
 * in this file.  updateDelayTask() waits for data to arrive on the queue 1 (delay
 * queue). It keeps a local task delay rate variable and when data is received, 
 * extracts the number from the queue and updates its local task delay rate. This
 * delay rates adds a wait to the task in milliseconds. In addition sends 
 * "Delayed by: X" String back to TASK A using Queue 2(Message Queue).
 * 
 * ============================================================================
 */

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
  * Task function declarations
  * The tasks as described in the comments at the top of this file.
  *
  */
static void doInputOutputTask(void* pvParameters);
static void updateDelayTask(void* pvParameters);


/* Delay Queue (Queue 1) from TASK A to TASK B */
static QueueHandle_t delayQUEUE = NULL;

/* Message Queue (Queue 2) from TASK B to TASK A*/
static QueueHandle_t messageQUEUE = NULL;


/* Global Variables                                         */
/*----------------------------------------------------------*/

// Data Structure used in Message Queue
// A Struct is used here to add flexibility for expansion
typedef struct Message {
    char body[20];
} Message;

static const char command[] = "delay ";  // Command to detect in receive buffer
static const int buf_length = 255;       // Receiving buffer length

/*-----------------------------------------------------------*/

void main(void)
{

     /* Create the queues. */
    messageQUEUE = xQueueCreate(messageQUEUE_LENGTH, sizeof(Message));
    delayQUEUE = xQueueCreate(delayQUEUE_LENGTH, sizeof(uint32_t));

    //Check if the queues are created successfully
    if ((messageQUEUE != NULL) && (delayQUEUE != NULL))
    {
    
        xTaskCreate(doInputOutputTask,              /* The function that implements the task. */
            "SerialComms",                           /* The text name assigned to the task - for debug only as it is not used by the kernel. */
            configMINIMAL_STACK_SIZE,               /* The size of the stack to allocate to the task. */
            NULL,                                    /* The parameter passed to the task - not used in this simple case. */
            mainQUEUE_RECEIVE_TASK_PRIORITY,        /* The priority assigned to the task. */
            NULL);                                   /* The task handle is not required, so NULL is passed. */

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

/*
 * Function:  doInputOutputTask
 * --------------------
 *  Implements the Task A behaviour as described at the top of the file.
 * 
 * parameters:
 *  void* pvParameters
 * 
 * returns:
 */
static void doInputOutputTask(void* pvParameters)
{
    /*Local Task Variables                                  */
    /*-------------------------------------------------------*/
    Message received_msg;
    uint8_t idx = 0;
    uint8_t cmd_length = strlen(command);
    char c;
    char buf[255];
   
    int delay;

    /*--------------------------------------------------------*/

    // Clear buffer
    memset(buf, 0, buf_length);

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

/*
 * Function:  updateDelayTask
 * --------------------
 *  Implements the Task B behaviour as described at the top of the file.
 * 
 * parameters:
 *  void* pvParameters
 * 
 * returns:
 */
static void updateDelayTask(void* pvParameters)
{
    /* Prevent the compiler warning about the unused parameter. */
    (void)pvParameters;

    /*Local Task Variables                                  */
    /*-------------------------------------------------------*/
    Message msg;
    int delay = 0;

    //Task delay rate in milliseconds
    int taskDelayRateInMs = 0;

    /*-------------------------------------------------------*/

    for (; ; )
    {
        //Convert milliseconds to tick periods for VTaskDelay
        const TickType_t xDelay = taskDelayRateInMs / portTICK_PERIOD_MS;
        
        vTaskDelay(xDelay);

        // Check if anything received without blocking, store in int
        if (xQueueReceive(delayQUEUE, (void*)&delay, 0) == pdTRUE) {

            //Update task delay rate for next execution
            taskDelayRateInMs = delay;

            //Construct the message body to be sent back to Task A
            snprintf(msg.body, sizeof(msg.body), "Delayed by: %d", delay);
            
            //send to Queue
            xQueueSend(messageQUEUE, (void*)&msg.body, 10);

        }

    }
}