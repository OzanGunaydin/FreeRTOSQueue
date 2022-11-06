# FreeRTOSQueue EXAMPLE

DESCRIPTION

This is a FreeRTOS queue demo applciation which creates:
- 2 Tasks
- 2 Queues

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
 
 
 
ENVIRONMENT

Developed and Tested using following configuration:

-Intel 5820k
-Ubuntu 20.04.4 LTS 5.15.0-52 Kernel
-gcc 9.4.0
-FreeRTOS Linux Simulator
 
BUILD INSTRUCTIONS

1) Download and install FreeRTOS as described in FreeRTOS Linux Simulator instructions
2) Clone the repository
3) Update Makefile variable FREERTOS_DIR_REL to point to local FreeRTOS root directory (Where Source folder is located).
4) Inside repository root directory run make.
5) To run the application :

$REPO_ROOT/cd build

$REPO_ROOT/build/queue_demo








