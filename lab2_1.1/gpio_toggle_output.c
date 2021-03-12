/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
//![Simple GPIO Config]
int main(void)
{
    volatile uint32_t ii;
    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();
    /* Configuring P1.0 as output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    /* Configuring P2.0, P2.1, P2.2 as outputs */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0); // red
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1); // green
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2); // blue
    /* Bring LEDs to low */
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0); // red
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1); // green
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2); // blue
    while (1)
    {
        /* Delay Loop */
        for(ii=0;ii<50000;ii++)
        {
        }
        // blinks LEDs
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }
}
//![Simple GPIO Config]
