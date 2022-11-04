# FreeRTOSQueue Example:

- 2 Tasks
- 2 Queues

-Task A should print any message it receives from Queue 2. Plus, it should read any keyboard input from the user and echo back this input to the console. If the user enters "delay" followed by a space, a number, and a new line, it should send that number to Queue 1.

-Task B should read any messages from Queue 1. If the message containts a number, it should update its delay rate to that number (milliseconds). It should also send the string "Delayed by x" to Queue 2, where x is the number entered by user.
