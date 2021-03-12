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

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // red
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0); // red
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1); // green
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2); // blue

    /* Configuring P1.0, P2.0, P2.1, P2.2 as low output pins */
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0); // red
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0); // red
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1); // green
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2); // blue

    uint8_t *alias_addr;
    unsigned int port_addr = 0x40004C03;
    unsigned int peripheral_region_addr = 0x40000000;
    unsigned int bit_pos = 0x2;
    unsigned int alias_region_addr = 0x42000000;

    alias_addr = (port_addr - peripheral_region_addr) * 32 + bit_pos * 4 + alias_region_addr;
    printf("%x \n", alias_addr);

    int counter = 0;

    while (1)
    {
        /* Delay Loop */
        for(ii=0;ii<50000;ii++)
        {
        }

        if (counter %2 == 0) {
            *alias_addr = 0x0; // LED off
        }
        else {
            *alias_addr = 0x1; // LED on
        }
        counter++;
    }
}
//![Simple GPIO Config]


