/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

uint32_t temp, current, start;
float duration, frequency;
int delayValues[] = {5000, 2000, 1000, 5, 1, 0};
void delay_ms (uint32_t count);

int main(void)
{
    /* Initialize timer */
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    frequency = MAP_CS_getMCLK();
    printf("%.0fHz\n", frequency);

    /* Loop over delayValues array */
    int kk;
    for (kk = 0; kk < 6; kk++) {
        uint32_t t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        delay_ms (delayValues[kk]);
        uint32_t  t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
        duration = (t0 - t1) * (1000000/frequency);
        printf("Timer Delay: %dus\n", delayValues[kk] * 1000);
        printf("Measured Time: %.0fus\n", duration);
        printf("Timer Value Difference: %.0fus\n", duration - (delayValues[kk] * 1000));
        printf("\n");
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


