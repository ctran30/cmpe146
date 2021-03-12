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

    /* Configuring P1.0 as low output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Create oscillator at Port 4, Pin 1 */
    CAPTIO0CTL = 0;
    CAPTIO0CTL |= (1 << 8);     // Enable CAPTIO
    CAPTIO0CTL |= 0b0100 << 4;  // Choose Port 4
    CAPTIO0CTL |= 0b0001 << 1;  // Choose Pin 1

    /* Set up TimerA */
    Timer_A_ContinuousModeConfig timer_continuous_obj;
    timer_continuous_obj.clockSource = TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK;
    timer_continuous_obj.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer_continuous_obj.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    timer_continuous_obj.timerClear = TIMER_A_DO_CLEAR;
    MAP_Timer_A_configureContinuousMode(TIMER_A2_BASE, &timer_continuous_obj);
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_CONTINUOUS_MODE);

    /* Read counter and frequency of oscillator */
    while(1) {
        MAP_Timer_A_clearTimer(TIMER_A2_BASE);
        delay_ms(2);
        uint32_t counterValue = MAP_Timer_A_getCounterValue(TIMER_A2_BASE);
        unsigned int frequency = (counterValue/2)*1000;
        if (frequency < 1000000) {
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
            printf("Touched!\n");
        } else {
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
            printf("Not touched!\n");
        }
        printf("Counter value: %u\n", counterValue);
        printf("Frequency: %dHz\n", frequency);
        printf("\n");
        fflush(stdout);
        delay_ms(500);
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
