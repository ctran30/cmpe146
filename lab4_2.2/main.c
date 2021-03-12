/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

uint32_t temp, current, start;
float duration, frequency;
//int delayValues[] = {5000, 2000, 1000, 5, 1, 0};
void delay_ms (uint32_t count);

int main(void)
{
    /* Initialize timer */
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    frequency = MAP_CS_getMCLK();
    printf("%.0fHz\n", frequency);

    /* Red LED */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    while (1)
    {
        delay_ms(1000);
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
}

// param: count is the number of ms to delay
void delay_ms (uint32_t count) {
    temp = count * (frequency/1000);
    current = MAP_Timer32_getValue(TIMER32_0_BASE);
    start = current - temp;

    while (current > start) {
        current = MAP_Timer32_getValue(TIMER32_0_BASE);
    }
}
