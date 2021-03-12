/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void delay_ms (uint32_t count);

int main(void)
{
    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();

    /* Set up Timer32 */
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    /* Create oscillator at Port 4, Pin 1 */
    CAPTIO0CTL = 0;
    CAPTIO0CTL |= (1 << 8);     // Enable CAPTIO
    CAPTIO0CTL |= 0b0100 << 4;  // Choose Port 4
    CAPTIO0CTL |= 0b0001 << 1;  // Choose Pin 1

    /* Read state of oscillator output */
    while(1) {
        bool state = CAPTIO0CTL & 0x200;
        printf("%d", state);
        fflush(stdout);
        delay_ms(10);
    }
}

void delay_ms (uint32_t count) {
    uint32_t t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t clock_rate = MAP_CS_getMCLK();
    uint32_t t_final = t0 - (uint32_t)((unsigned long long)clock_rate * count/1000);
    while(1) {
        if (MAP_Timer32_getValue(TIMER32_0_BASE) < t_final) {
            break;
        }
    }
}
