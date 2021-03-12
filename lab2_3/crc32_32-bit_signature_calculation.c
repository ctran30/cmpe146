#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <stdlib.h>

#define CRC32_POLY              0xEDB88320
#define CRC32_INIT              0xFFFFFFFF

uint8_t myData[10240];
static uint32_t calculateCRC32(uint8_t* data, uint32_t length);
static uint32_t compute_simple_checksum(uint8_t* data, uint32_t length);

volatile uint32_t hwCalculatedCRC, swCalculatedCRC, simpleCRC;

//![Simple CRC32 Example] 
int main(void)
{
    uint32_t ii;
    uint32_t n = 0;
    uint32_t timerHW, timerSW, timerC;

    /* Stop WDT */
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u \n", MAP_CS_getMCLK());

    for (ii = 0; ii < 10240; ii++) {
        myData[ii] = (rand() % 50 + 1);
        myData[ii] = (myData[ii]/2)*2;
        if (n == 1) {
            myData[ii]++;
        }
        n ^= 1;

    }

    MAP_WDT_A_holdTimer();

    /* Data Point 20 with LSB Reversed */
    myData[20] ^= 0x01;

    /* Hardware Calculation */
    uint32_t t0_hw = MAP_Timer32_getValue(TIMER32_0_BASE);
    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);
    for (ii = 0; ii < 10240; ii++) {
        MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);
    }
    hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
    uint32_t t1_hw = MAP_Timer32_getValue(TIMER32_0_BASE);
    timerHW = (t0_hw - t1_hw) * 0.000333 * 1000;
    printf("Hardware Checksum for myData[20]: %u \n", hwCalculatedCRC);
    printf("Time to calculate: %u microseconds \n", timerHW);

    /* Simple Calculation */
    uint32_t t0_s = MAP_Timer32_getValue(TIMER32_0_BASE);
    simpleCRC = compute_simple_checksum((uint8_t*) myData, 10240);
    uint32_t t1_s = MAP_Timer32_getValue(TIMER32_0_BASE);
    timerC = (t0_s - t1_s) * 0.000333 * 1000;
    printf("Simple Checksum for myData[20]: %u \n", simpleCRC);
    printf("Time to calculate: %u microseconds\n", timerC);
    printf("\n");

    /* Data Point 21 with LSB Reversed */
    myData[21] ^= 0x01;

    /* Hardware Calculation */
    t0_hw = MAP_Timer32_getValue(TIMER32_0_BASE);
    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);
    for (ii = 0; ii < 10240; ii++) {
        MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);
    }
    hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
    t1_hw = MAP_Timer32_getValue(TIMER32_0_BASE);
    timerHW = (t0_hw - t1_hw) * 0.000333 * 1000;
    printf("Hardware Checksum for myData[21]: %u \n", hwCalculatedCRC);
    printf("Time to calculate: %u microseconds \n", timerHW);

    /* Simple Calculation */
    t0_s = MAP_Timer32_getValue(TIMER32_0_BASE);
    simpleCRC = compute_simple_checksum((uint8_t*) myData, 10240);
    t1_s = MAP_Timer32_getValue(TIMER32_0_BASE);
    timerC = (t0_s - t1_s) * 0.000333 * 1000;
    printf("Simple Checksum for myData[21]: %u \n", simpleCRC);
    printf("Time to calculate: %u microseconds\n", timerC);
    printf("\n");

    /* Pause for the debugger */
    __no_operation();
}
//![Simple CRC32 Example] 

/* Standard software calculation of CRC32 */
static uint32_t calculateCRC32(uint8_t* data, uint32_t length)
{
    uint32_t ii, jj, byte, crc, mask;;

    crc = 0xFFFFFFFF;

    for(ii=0;ii<length;ii++)
    {
        byte = data[ii];
        crc = crc ^ byte;

        for (jj = 0; jj < 8; jj++)
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (CRC32_POLY & mask);
        }

    }

    return ~crc;
}

uint32_t compute_simple_checksum(uint8_t* data, uint32_t length) {
    uint32_t sum = 0;
    uint32_t i;

    for (i = 0; i < length; i++) {
        sum += data[i];
    }

    return ~sum;
}
