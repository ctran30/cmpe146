#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/drivers/Board.h>
#define __MSP432P4XX__
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define TASKSTACKSIZE   2048

Void task_timing(UArg arg0, UArg arg1);
Void task_LEDcontrol(UArg arg0, UArg arg1);

//Task_Struct task1Struct, task2Struct;
//Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE];

#define NTASKS 4
Task_Struct taskStructs[NTASKS];
Char taskStacks[NTASKS][TASKSTACKSIZE];

Semaphore_Struct semaStruct;
Semaphore_Handle semaphoreHandle = NULL;

int main()
{
    // Init drivers
    Board_init();

    srand(time(NULL));      // Set seed for random number generator

    // Construct tasks
    Task_Params timingParams, redLEDParams, greenLEDParams, blueLEDParams;

    Task_Params_init(&timingParams);
    timingParams.stackSize = TASKSTACKSIZE;
    Task_Params_init(&redLEDParams);
    redLEDParams.stackSize = TASKSTACKSIZE;
    Task_Params_init(&greenLEDParams);
    greenLEDParams.stackSize = TASKSTACKSIZE;
    Task_Params_init(&blueLEDParams);
    blueLEDParams.stackSize = TASKSTACKSIZE;

    // Timing Task
    timingParams.stack = &taskStacks[0];
    Task_construct(&taskStructs[0], (Task_FuncPtr)task_timing, &timingParams, NULL);

    // red LED task
    redLEDParams.arg0 = GPIO_PORT_P1;
    redLEDParams.arg1 = GPIO_PIN0;
    redLEDParams.stack = &taskStacks[1];
    Task_construct(&taskStructs[1], (Task_FuncPtr)task_LEDcontrol, &redLEDParams, NULL);

    // green LED task
    greenLEDParams.arg0 = GPIO_PORT_P2;
    greenLEDParams.arg1 = GPIO_PIN1;
    greenLEDParams.stack = &taskStacks[2];
    Task_construct(&taskStructs[2], (Task_FuncPtr)task_LEDcontrol, &greenLEDParams, NULL);

    // blue LED task
    blueLEDParams.arg0 = GPIO_PORT_P2;
    blueLEDParams.arg1 = GPIO_PIN2;
    blueLEDParams.stack = &taskStacks[3];
    Task_construct(&taskStructs[3], (Task_FuncPtr)task_LEDcontrol, &blueLEDParams, NULL);

    // Construct a semaphore object
    Semaphore_Params semaParams;
    Semaphore_Params_init(&semaParams);                 // Initialize structure with default parameters
    Semaphore_construct(&semaStruct, 0, &semaParams);   // Create an instance of semaphore object
    semaphoreHandle = Semaphore_handle(&semaStruct);

    BIOS_start();    // Jump to the OS and won't return
    return(0);
}

Void task_timing(UArg arg0, UArg arg1)
{
    while(1) {
        int ms = (rand() % (5000 + 1 - 1000) + 1000) / 2;
        uint32_t i;
        for (i = 0; i < NTASKS-1; i++){
            Semaphore_post(semaphoreHandle);
        }
        Task_sleep(ms);
    }
}

Void task_LEDcontrol (UArg arg0, UArg arg1) {
    MAP_GPIO_setAsOutputPin(arg0, arg1);
    MAP_GPIO_setOutputLowOnPin(arg0, arg1);
    while(1) {
        Semaphore_pend(semaphoreHandle, BIOS_WAIT_FOREVER);
        MAP_GPIO_toggleOutputOnPin(arg0, arg1);
        Task_yield();
    }
}
