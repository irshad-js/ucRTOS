/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/******************************************************************************
 * NOTE: Windows will not be running the FreeRTOS demo threads continuously, so
 * do not expect to get real time behaviour from the FreeRTOS Windows port, or
 * this demo application.  Also, the timing information in the FreeRTOS+Trace
 * logs have no meaningful units.  See the documentation page for the Windows
 * port for further information:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 *
 * NOTE 2:  This project provides two demo applications.  A simple blinky style
 * project, and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
 * between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
 * in main.c.  This file implements the simply blinky version.  Console output
 * is used in place of the normal LED toggling.
 *
 * NOTE 3:  This file only contains the source code that is specific to the
 * basic demo.  Generic functions, such FreeRTOS hook functions, are defined
 * in main.c.
 ******************************************************************************
 *
 * main_blinky() creates one queue, one software timer, and two tasks.  It then
 * starts the scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  It uses vTaskDelayUntil() to create a periodic task that sends
 * the value 100 to the queue every 200 milliseconds (please read the notes
 * above regarding the accuracy of timing under Windows).
 *
 * The Queue Send Software Timer:
 * The timer is a one-shot timer that is reset by a key press.  The timer's
 * period is set to two seconds - if the timer expires then its callback
 * function writes the value 200 to the queue.  The callback function is
 * implemented by prvQueueSendTimerCallback() within this file.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() waits for data to arrive on the queue.
 * When data is received, the task checks the value of the data, then outputs a
 * message to indicate if the data came from the queue send task or the queue
 * send software timer.
 *
 * Expected Behaviour:
 * - The queue send task writes to the queue every 200ms, so every 200ms the
 *   queue receive task will output a message indicating that data was received
 *   on the queue from the queue send task.
 * - The queue send software timer has a period of two seconds, and is reset
 *   each time a key is pressed.  So if two seconds expire without a key being
 *   pressed then the queue receive task will output a message indicating that
 *   data was received on the queue from the queue send software timer.
 *
 * NOTE:  Console input and output relies on Windows system calls, which can
 * interfere with the execution of the FreeRTOS Windows port.  This demo only
 * uses Windows system call occasionally.  Heavier use of Windows system calls
 * can crash the port.
 */

#include <stdio.h>
#include <conio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#define mainQUEUE_RECEIVE_TASK_PRIORITY	(tskIDLE_PRIORITY + 2)
#define	mainQUEUE_SEND_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)
#define mainTASK_SEND_FREQUENCY_MS			pdMS_TO_TICKS(200UL)
#define mainTIMER_SEND_FREQUENCY_MS			pdMS_TO_TICKS(2000UL)
#define mainQUEUE_LENGTH					      (10)

static void _prvQueueReceiveTask(void *pParameters);
static void _prvQueueSendTask(void *pParameters);
static void _prvQueueSendTimerCallback(TimerHandle_t xTimerHandle);

static QueueHandle_t _xQueue = NULL;
static TimerHandle_t _xTimer = NULL;
static SemaphoreHandle_t _xTaskMutex = NULL;

void main_blinky(void) {
  const TickType_t xTimerPeriod = mainTIMER_SEND_FREQUENCY_MS;
  _xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(uint32_t));

  if(_xQueue != NULL) {
    xTaskCreate(_prvQueueReceiveTask, "Rx", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL);

    uint32_t vt1[] = {1, 2, 3};
    uint32_t vt2[] = {4, 5, 6};

    _xTaskMutex = xSemaphoreCreateMutex();
    xTaskCreate(_prvQueueSendTask, "Tx1", configMINIMAL_STACK_SIZE, vt1, mainQUEUE_SEND_TASK_PRIORITY, NULL);
    xTaskCreate(_prvQueueSendTask, "Tx2", configMINIMAL_STACK_SIZE, vt2, mainQUEUE_SEND_TASK_PRIORITY, NULL);

    _xTimer = xTimerCreate("Timer", xTimerPeriod, pdFALSE, NULL, _prvQueueSendTimerCallback);

    vTaskStartScheduler(); // Start the tasks and timer running.
  }

  for(;;);
}

static void _prvQueueSendTask(void *pParameters) {
  uint32_t pValuesToSend[3];

  for (int i = 0; i < 3; ++i)
    pValuesToSend[i] = ((uint32_t*)pParameters)[i];

  TickType_t xNextWakeTime = xTaskGetTickCount();
  const TickType_t xBlockTime = mainTASK_SEND_FREQUENCY_MS;

  for(;;) {
    vTaskDelayUntil(&xNextWakeTime, xBlockTime);
    xSemaphoreTake(_xTaskMutex, portMAX_DELAY);

    int numElems = sizeof(pValuesToSend) / sizeof(uint32_t);

    for(int i = 0; i < numElems; ++i)
      xQueueSend(_xQueue, &pValuesToSend[i], 0U);

    xSemaphoreGive(_xTaskMutex);
  }
}

static void _prvQueueSendTimerCallback(TimerHandle_t xTimerHandle) {
  const uint32_t ulValueToSend = 3;
  (void) xTimerHandle;

  xQueueSend(_xQueue, &ulValueToSend, 0U);
}

static void _prvQueueReceiveTask(void *pParameters) {
  uint32_t ulReceivedValue;
  (void) pParameters;

  for(;;) {
    xQueueReceive(_xQueue, &ulReceivedValue, portMAX_DELAY);
    printf("Value: %d\n", ulReceivedValue);

    if(_kbhit() != 0) {
      (void) _getch();

      xTimerReset(_xTimer, portMAX_DELAY);
    }
  }
}
