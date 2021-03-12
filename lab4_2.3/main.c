/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#define TIMER_MAX 0x000fffff

uint32_t delay, current, timer, previous, t0, t1;
float duration, frequency;
int delayValues[] = {10000, 5000, 2000, 1000, 10, 5, 2, 1, 0, 10000};
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
    for (kk = 0; kk < 10; kk++) {
        t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        delay_ms (delayValues[kk]);
        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
        duration = (t0 - t1) * (1000000/frequency);
        printf("Timer Delay: %dus\n", delayValues[kk] * 1000);
        printf("Measured Time: %.0fus\n", duration);
        printf("Timer Value Difference: %.0fus\n", duration - (delayValues[kk] * 1000));
        printf("\n");
    }
}

// param: count is the number of ms to delay
void delay_ms (uint32_t count) {
    timer = t0;
    delay = count * (frequency/1000);
    current = 0;
    previous = 0;

    while (!(delay <= (t0-timer))) {
        current = MAP_Timer32_getValue(TIMER32_0_BASE) & TIMER_MAX;
        if (current < previous) {
            timer = timer - (previous - current);
        } else {
            timer = timer - previous - (TIMER_MAX - current);
        }
        previous = current;
    }

    if (count <= 10) {
        delay = count * (frequency/1000);
        current = MAP_Timer32_getValue(TIMER32_0_BASE);
        timer = current - delay;
        while (current > timer) {
            current = MAP_Timer32_getValue(TIMER32_0_BASE);
        }
    }
}


