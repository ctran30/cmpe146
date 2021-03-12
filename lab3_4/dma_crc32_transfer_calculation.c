/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/* Statics */
static volatile uint32_t crcSignature;

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

uint8_t data_array[10240];

#define CRC32_SEED 0xFFFFFFFF

volatile int dma_done;

int size_array[] = {512, 1024, 1030, 1824, 2048, 2049, 2303, 10240};
int size;
int num_blocks;


int main(void)
{
    /* Halting Watchdog */
    MAP_WDT_A_holdTimer();

    /* Initialize timer */
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u\n", MAP_CS_getMCLK());

    /* Configuring DMA module */
    MAP_DMA_enableModule();

    int i;
    for (i = 0; i < sizeof(size_array)/sizeof(size_array[0]); i++)
    {
        size = size_array[i];
        num_blocks = 1;
        uint32_t hwCRC = 0;
        crcSignature = 0;

        while (size > 1024) {
            num_blocks++;
            size -= 1024;
        }
        size = size_array[i];

        uint32_t time_hw = 0;
        uint32_t time_dma = 0;
        int j;
        for (j = 0; j < num_blocks; j++) {
            uint32_t t0_hw = MAP_Timer32_getValue(TIMER32_0_BASE);
            MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);

            int current_index = j * 1024;
            int current_size = (j + 1) * 1024;
            if (current_size > size)
                current_size = size;

            int ii;
            for (ii = current_index; ii < current_size; ii++)
                MAP_CRC32_set8BitData(data_array[ii], CRC32_MODE);
            hwCRC += MAP_CRC32_getResult(CRC32_MODE);
            uint32_t t1_hw = MAP_Timer32_getValue(TIMER32_0_BASE);
            time_hw += (t0_hw - t1_hw);

            MAP_DMA_setControlBase(controlTable);

            /* Setting Control Indexes. In this case we will set the source of the
             * DMA transfer to our random data array and the destination to the
             * CRC32 data in register address*/
            MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
                    UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
            MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
                    UDMA_MODE_AUTO, data_array,
                    (void*) (&CRC32->DI32), current_size);

            /* Assigning/Enabling Interrupts */
            MAP_DMA_assignInterrupt(DMA_INT1, 0);
            MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
            MAP_Interrupt_enableMaster();

            /* Enabling DMA Channel 0 */
            dma_done = 0;
            uint32_t t0_dma = MAP_Timer32_getValue(TIMER32_0_BASE);
            MAP_DMA_enableChannel(0);

            /* Forcing a software transfer on DMA Channel 0 */
            MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
            MAP_DMA_requestSoftwareTransfer(0);

            while(1)
            {
                if (dma_done) break;
            }
            uint32_t t1_dma = MAP_Timer32_getValue(TIMER32_0_BASE);
            time_dma += (t0_dma - t1_dma);
        }

        printf("size: %u\n", size_array[i]);
        printf("hwCRC=%08x\n", hwCRC);
        time_hw /= 3;
        printf("hw time= %uus\n", time_hw);
        printf("dmaCRC=%08x\n", crcSignature);
        time_dma /= 3;
        printf("dma time= %uus\n", time_dma);
        printf("speedup: %ux faster\n", time_hw/time_dma);
        printf("-------------------------\n");
    }
}

/* Completion interrupt for DMA */
void DMA_INT1_IRQHandler(void)
{
    MAP_DMA_disableChannel(0);
    crcSignature += MAP_CRC32_getResult(CRC32_MODE);
    dma_done = 1;
}

///* DriverLib Includes */
//#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
//
///* Standard Includes */
//#include <stdint.h>
//#include <stdio.h>
//#include <string.h>
//#include <stdbool.h>
//
//#define CRC32_SEED 0xFFFFFFFF
//
///* Statics */
//static volatile uint32_t crcSignature, hwCRC;
//volatile int dma_done;
//int size_array[] = {512, 1024, 1030, 1824, 2048, 2049, 2303, 10240};
//int size;
//int num_blocks;
//
///* DMA Control Table */
//#if defined(__TI_COMPILER_VERSION__)
//#pragma DATA_ALIGN(controlTable, 1024)
//#elif defined(__IAR_SYSTEMS_ICC__)
//#pragma data_alignment=1024
//#elif defined(__GNUC__)
//__attribute__ ((aligned (1024)))
//#elif defined(__CC_ARM)
//__align(1024)
//#endif
//
//uint8_t controlTable[1024];
//uint8_t data_array[10240];
//
//int main(void)
//{
//    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
//    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
//    printf("%u \n", MAP_CS_getMCLK());
//
//    /* Halting Watchdog */
//    MAP_WDT_A_holdTimer();
//
//    int jj;
//    for (jj = 0; jj < 8; jj++) {
//        size = size_array[jj];
//        num_blocks = 1;
//        hwCRC = 0;
//        crcSignature = 0;
//
//        while (size > 1024) {
//            num_blocks++;
//            size -= 1024;
//        }
//        size = size_array[jj];
//        uint32_t timerHW = 0;
//        uint32_t timerDMA = 0;
//
//        int kk;
//        for (kk = 0; kk < num_blocks; kk++) {
//            uint32_t t0_hw = MAP_Timer32_getValue(TIMER32_0_BASE);
//            MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
//
//            int index = kk * 1024;
//            int current_size = (kk + 1) * 1024;
//            if (current_size > size) {
//                current_size = size;
//            }
//
//            int ii;
//            for (ii = index; ii < current_size; ii++)
//                MAP_CRC32_set8BitData(data_array[ii], CRC32_MODE);
//            hwCRC += MAP_CRC32_getResult(CRC32_MODE);
//            uint32_t t1_hw = MAP_Timer32_getValue(TIMER32_0_BASE);
//            timerHW += (t0_hw - t1_hw);
//
//            /* Configuring DMA module */
//            MAP_DMA_enableModule();
//            MAP_DMA_setControlBase(controlTable);
//
//            /* Setting Control Indexes. In this case we will set the source of the
//             * DMA transfer to our random data array and the destination to the
//             * CRC32 data in register address*/
//            MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
//                                      UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
//            MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
//                                       UDMA_MODE_AUTO, data_array,
//                                       (void*) (&CRC32->DI32), current_size);
//
//            /* Assigning/Enabling Interrupts */
//            MAP_DMA_assignInterrupt(DMA_INT1, 0);
//            MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
//            MAP_Interrupt_enableMaster();
//
//            dma_done = 0;
//
//            /* Enabling DMA Channel 0 */
//            MAP_DMA_enableChannel(0);
//
//            uint32_t t0_dma = MAP_Timer32_getValue(TIMER32_0_BASE);
//            MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
//
//            /* Forcing a software transfer on DMA Channel 0 */
//            MAP_DMA_requestSoftwareTransfer(0);
//
//            while(dma_done != 1)
//            {
//            }
//
//            uint32_t t1_dma = MAP_Timer32_getValue(TIMER32_0_BASE);
//            timerDMA += (t0_dma - t1_dma);
//        }
//        printf("Hardware Checksum for %d bytes: %08x \n", size_array[jj], hwCRC);
//        printf("Time to compute: %u microseconds \n", timerHW/3);
//        printf("DMA Checksum for %d bytes: %08x \n", size_array[jj], crcSignature);
//        printf("Time to compute: %u microseconds \n", timerDMA/3);
//
//        double speedup = timerHW / timerDMA;
//        printf("DMA method is %0.2f times faster than the hardware method for %d bytes.\n", speedup, size_array[jj]);
//        printf("\n");
//    }
//}
//
//
///* Completion interrupt for DMA */
//void DMA_INT1_IRQHandler(void)
//{
//    //    if (size <= 1024) {
//        //            MAP_DMA_disableChannel(0);
//        //            crcSignature = MAP_CRC32_getResult(CRC32_MODE);
//        //            dma_done = 1;
//        //        } else {
//        //            uint32_t transferRequests = (size / 1024) + ((size % 1024) != 0);
//        //            size = (size % 1024);
//        //            int kk;
//        //            for (kk = 1; kk < transferRequests; kk++) {
//        //                if (kk = transferRequests - 1) {
//        //                    MAP_DMA_enableModule();
//        //                    MAP_DMA_setControlBase(controlTable);
//        //                    MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
//        //                                              UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
//        //                    MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
//        //                                               UDMA_MODE_AUTO, data_array,
//        //                                               (void*) (&CRC32->DI32), size);
//        //                    MAP_DMA_assignInterrupt(DMA_INT1, 0);
//        //                    MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
//        //                    MAP_Interrupt_enableMaster();
//        //                    MAP_DMA_enableChannel(0);
//        //                    MAP_DMA_requestSoftwareTransfer(0);
//        //                } else {
//        //                    MAP_DMA_enableModule();
//        //                    MAP_DMA_setControlBase(controlTable);
//        //                    MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
//        //                                              UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
//        //                    MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
//        //                                               UDMA_MODE_AUTO, data_array,
//        //                                               (void*) (&CRC32->DI32), 1024);
//        //                    MAP_DMA_assignInterrupt(DMA_INT1, 0);
//        //                    MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
//        //                    MAP_Interrupt_enableMaster();
//        //                    MAP_DMA_enableChannel(0);
//        //                    MAP_DMA_requestSoftwareTransfer(0);
//        //                }
//        //            }
//    MAP_DMA_disableChannel(0);
//    crcSignature += MAP_CRC32_getResult(CRC32_MODE);
//    dma_done = 1;
//}
