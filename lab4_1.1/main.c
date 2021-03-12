/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

char* string1 = "12345";
char string2[] = "12345";

void main(void)
{
    printf("%x\n", string1);
    printf("%x\n", &string2);

    printf("string 1: %s\n", string1);
    printf("string 2: %s\n", string2);
    string1[0] = '0';
    string2[0] = '0';
    printf("string 1: %s\n", string1);
    printf("string 2: %s\n", string2);
}
