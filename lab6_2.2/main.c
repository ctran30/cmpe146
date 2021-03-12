// DriverLib Includes
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// Standard Includes
#include <stdint.h>
#include <stdio.h>
#include <string.h>

volatile bool adc_done;
void delay_ms (uint32_t count);

int main(void)
{
    // Halt WDT
    WDT_A_holdTimer();

    /* Set up Timer32 */
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("MCLK: %u\n", MAP_CS_getMCLK());

    // Set reference voltage to 2.5 V and enable temperature sensor
    REF_A_enableReferenceVoltage();
    REF_A_enableTempSensor();
    REF_A_setReferenceVoltage(REF_A_VREF2_5V);

    // Initializing ADC (MCLK/1/1) with temperature sensor routed
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, ADC_TEMPSENSEMAP);

    // Configure ADC Memory for temperature sensor data
    ADC14_configureSingleSampleMode(ADC_MEM0, false);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A22, false);

    // Configure the sample/hold time
    ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_192, ADC_PULSE_WIDTH_192);

    // Enable sample timer in manual iteration mode and interrupts
    ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);

    // Enable conversion
    ADC14_enableConversion();

    // Enabling Interrupts
    ADC14_enableInterrupt(ADC_INT0);
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();

    // Calibration Points
    uint32_t *calTLV = 0x002010EC;
    float cal30 = *calTLV; // referenced from TLV
    calTLV = 0x002010F0;
    float cal85 = *calTLV; // referenced from TLV

    float temp0 = (cal85 - cal30) / 55;
    float temp1 = cal30 - (temp0 * 30);

    while(1) {
        // Trigger conversion with software
        adc_done = false;
        ADC14_toggleConversionTrigger();

        while (!adc_done)
        {
        }
        uint32_t adc_value = ADC14_getResult(ADC_MEM0);

        // calculate temperature of chip
        float tempC = (adc_value - temp1) / temp0;
        float tempF = (tempC * 9/5) + 32;

        printf("Temperature Readings: %0.2fC // %0.2fF\n", tempC, tempF);
        delay_ms(500);
    }
}

// This interrupt happens every time a conversion has completed
void ADC14_IRQHandler(void)
{
    uint64_t status;
    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    if(status & ADC_INT0)
    {
        adc_done = true;
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
