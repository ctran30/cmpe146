/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//TLV starts at address 0x0020_1000 and ends at 0x0020_1040 (for this program) in ROM

int main(void)
{
    //     int address;
    //     address = 0x00201000;

    const char *descriptions[] = {"TLV Checksum", "Device Info Tag", "Device Info Length", "Device ID",
                                  "HW Revision", "Boot Code Revision", "ROM Driver Library Revision",
                                  "Die Record Tag", "Die Record Length", "Die X-Position", "Die Y-Position",
                                  "Wafer ID", "Lot ID", "Reserved", "Reserved", "Reserved", "Test Results"};

    unsigned int* table;
    table = *(unsigned int *)0x00201000;
    int i;

    int addresses[17];
    addresses[0] = 0x00201000;
    for (i = 1; i < 17; i++) {
        addresses[i] = addresses[i-1] + 0x00000004;
    }


    for (i = 0; i < 17; i++) {
        printf("%s  /  0x00%x  /   %x\n", descriptions[i], addresses[i], table);
        table = *(unsigned int *)addresses[i+1];
    }



}
