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

#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

#define TASKSTACKSIZE   2048

Void task1(UArg arg0, UArg arg1);
Void task2(UArg arg0, UArg arg1);
Void task3(UArg arg0, UArg arg1);
UART_Handle uart;
UART_Params uartParams;

Task_Struct task1Struct, task2Struct, task3Struct;
Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE], task3Stack[TASKSTACKSIZE];

int main(){
    /* Construct BIOS objects */
    Task_Params taskParams;

    /* Call driver init functions */
    Board_init();

    /* Construct task threads */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)task1, &taskParams, NULL);

    taskParams.stack = &task2Stack;
    Task_construct(&task2Struct, (Task_FuncPtr)task2, &taskParams, NULL);

    taskParams.stack = &task3Stack;
    Task_construct(&task3Struct, (Task_FuncPtr)task3, &taskParams, NULL);

    BIOS_start();    /* Does not return */
    return(0);
}

Void task1(UArg arg0, UArg arg1)
{
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1); // P1.1 as input
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    float freq = MAP_CS_getMCLK();

    printf("Task1\n");
    UART_init();    // Driver init

    // Set up communication parameters and open the device
    UART_Params_init(&uartParams);
    uartParams.readEcho = UART_ECHO_OFF;
    uart = UART_open(CONFIG_UART_0, &uartParams);

    if (uart == NULL) {
        printf("Failed to open UART.\n");
        while (1);
    }

    char buffer[10];
    uint8_t currentState;
    uint8_t oldState;
    uint32_t t0, t1, duration;
    float time;

    while (1) {
        currentState = MAP_GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);
        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
        duration = t0 - t1;
        time = duration * (1000/freq); // calculates ms

        if (oldState != currentState) {
            if (currentState == GPIO_INPUT_PIN_LOW) {
                sprintf(buffer, "S10\n");
                UART_write(uart, buffer, strlen(buffer));
                t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
            } else if (currentState == GPIO_INPUT_PIN_HIGH) {
                sprintf(buffer, "S11\n");
                UART_write(uart, buffer, strlen(buffer));
            }
        } else if (time > 5000 && currentState == GPIO_INPUT_PIN_LOW) {
            sprintf(buffer, "S10 - timer > 5s\n");
            UART_write(uart, buffer, strlen(buffer));
            t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        } else if (time > 5000 && currentState == GPIO_INPUT_PIN_HIGH) {
            sprintf(buffer, "S11 - timer > 5s\n");
            UART_write(uart, buffer, strlen(buffer));
            t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        } else {
            continue;
        }

        oldState = currentState;

        Task_sleep(100);
    }
}

Void task2(UArg arg0, UArg arg1)
{
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4); // P1.1 as input
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    float freq = MAP_CS_getMCLK();

    printf("Task2\n");
    while (1) {
        if (uart == NULL) {
            printf("Failed to open UART.\n");
            while (1);
        } else {
            char buffer[10];
            uint8_t currentState;
            uint8_t oldState;
            uint32_t t0, t1, duration;
            float time;

            while (1) {
                currentState = MAP_GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4);
                t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
                duration = t0 - t1;
                time = duration * (1000/freq); // calculates ms

                if (oldState != currentState) {
                    if (currentState == GPIO_INPUT_PIN_LOW) {
                        sprintf(buffer, "S20\n");
                        UART_write(uart, buffer, strlen(buffer));
                        t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
                    } else if (currentState == GPIO_INPUT_PIN_HIGH) {
                        sprintf(buffer, "S21\n");
                        UART_write(uart, buffer, strlen(buffer));
                    }
                } else if (time > 5000 && currentState == GPIO_INPUT_PIN_LOW) {
                    sprintf(buffer, "S20 - timer > 5s\n");
                    UART_write(uart, buffer, strlen(buffer));
                    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
                } else if (time > 5000 && currentState == GPIO_INPUT_PIN_HIGH) {
                    sprintf(buffer, "S21 - timer > 5s\n");
                    UART_write(uart, buffer, strlen(buffer));
                    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
                } else {
                    continue;
                }

                oldState = currentState;

                Task_sleep(100);
            }
        }

    }
}

Void task3(UArg arg0, UArg arg1)
{
    printf("Task3\n");
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    uint8_t UARTvalue = 0;

    while(1) {
        UART_read(uart, &UARTvalue, 1);
        if (UARTvalue == 48) {
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
        } else if (UARTvalue == 49) {
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
        }
        Task_sleep(100);
    }
}
