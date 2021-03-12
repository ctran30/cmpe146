/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define CRC32_SEED 0xFFFFFFFF

/* Statics */
static volatile uint32_t crcSignature;
volatile int dma_done;
int size_array[] = {2, 4, 16, 32, 64, 128, 256, 786, 1024};

/* DMA Control Table */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(controlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#elif defined(__CC_ARM)
__align(1024)
#endif

uint8_t controlTable[1024];
uint8_t data_array[1024];

int main(void)
{

    uint32_t timerHW, timerDMA;

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u \n", MAP_CS_getMCLK());

    /* Halting Watchdog */
    MAP_WDT_A_holdTimer();

    int jj;
    for (jj = 0; jj < 9; jj++) {
        uint32_t t0_hw = MAP_Timer32_getValue(TIMER32_0_BASE);
        MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
        int ii;
        for (ii = 0; ii < size_array[jj]; ii++)
            MAP_CRC32_set8BitData(data_array[ii], CRC32_MODE);
        uint32_t hwCRC = MAP_CRC32_getResult (CRC32_MODE);
        uint32_t t1_hw = MAP_Timer32_getValue(TIMER32_0_BASE);
        timerHW = (t0_hw - t1_hw);

        /* Configuring DMA module */
        MAP_DMA_enableModule();
        MAP_DMA_setControlBase(controlTable);

        /* Setting Control Indexes. In this case we will set the source of the
         * DMA transfer to our random data array and the destination to the
         * CRC32 data in register address*/
        MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
                                  UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
        MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
                                   UDMA_MODE_AUTO, data_array,
                                   (void*) (&CRC32->DI32), size_array[jj]);

        /* Assigning/Enabling Interrupts */
        MAP_DMA_assignInterrupt(DMA_INT1, 0);
        MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
        MAP_Interrupt_enableMaster();

        /* Enabling DMA Channel 0 */
        MAP_DMA_enableChannel(0);

        dma_done = 0;
        uint32_t t0_dma = MAP_Timer32_getValue(TIMER32_0_BASE);
        MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);

        /* Forcing a software transfer on DMA Channel 0 */
        MAP_DMA_requestSoftwareTransfer(0);

        while(dma_done != 1)
        {
        }

        uint32_t t1_dma = MAP_Timer32_getValue(TIMER32_0_BASE);
        timerDMA = (t0_dma - t1_dma);

        printf("Hardware Checksum for %d bytes: %08x \n", size_array[jj], hwCRC);
        printf("Time to compute: %u microseconds \n", timerHW/3);
        printf("DMA Checksum for %d bytes: %08x \n", size_array[jj], crcSignature);
        printf("Time to compute: %u microseconds \n", timerDMA/3);

        double speedup = timerHW / timerDMA;
        printf("DMA method is %0.2f times faster than the hardware method for %d bytes.\n", speedup, size_array[jj]);
        printf("\n");

        timerHW = 0;
        timerDMA = 0;
    }
}

/* Completion interrupt for DMA */
void DMA_INT1_IRQHandler(void)
{
    MAP_DMA_disableChannel(0);
    crcSignature = MAP_CRC32_getResult(CRC32_MODE);
    dma_done = 1;
}
