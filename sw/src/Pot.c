#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timers.h"
#include "Pot.h"
#include "../inc/SysTickInts.h"
#include "../inc/TLV5616.h"
#include "AudioCommands.h"

/*
 * AIN0 - PE3 - Flanger Freq
 * AIN1 - PE2 - Tempo
 * AIN2 - PE1 - Flanger Delay
 * AIN3 - PE0 - Bass
 * AIN4 - PD3 - Mid
 * AIN5 - PD2 - Treble
 * AIN6 - PD1 - Volume
 * AIN7 - PD0 - Bass In
 * AIN9 - PE4 - Delay
 */


void PotInit() {
    //ADC and Clock initializations
    SYSCTL_RCGCADC_R |= 0x0002;               // 1) activate ADC1
    SYSCTL_RCGCGPIO_R |= 0x18;                // 2) activate clock for Port E and Port D
    while((SYSCTL_PRGPIO_R&0x18) != 0x18){};  // 3) for stabilization

    //Port E initializations
    GPIO_PORTE_DIR_R &= ~0x1F;    // 4) make PE0-4 input
    GPIO_PORTE_AFSEL_R |= 0x1F;   // 5) Set the direction of PE0-4 as input
    GPIO_PORTE_DEN_R &= ~0x1F;    // 6) Enable alternative function for PE0-4 (Alternative signal rather than GPIO)
    GPIO_PORTE_AMSEL_R |= 0x1F;   // 7) enable analog functionality on PE0-4

    //Port D initializations
    GPIO_PORTD_DIR_R &= ~0x0F;    // 4) make PD0-3 input
    GPIO_PORTD_AFSEL_R |= 0x0F;   // 5) Set the direction of PD0-3 as input
    GPIO_PORTD_DEN_R &= ~0x0F;    // 6) Enable alternative function for PD0-3 (Alternative signal rather than GPIO)
    GPIO_PORTD_AMSEL_R |= 0x0F;   // 7) enable analog functionality on PD0-3


    ADC1_PC_R &= ~0xF;
    ADC1_PC_R |= 0x1;             // 8) configure for 125K samples/sec
    ADC1_SSPRI_R = 0x0330;        // 9) Sequencer 3 and 0 are highest priority
    ADC1_ACTSS_R &= ~0x9;      // 10) disable sample sequencer 3 and 0
    ADC1_EMUX_R &= ~0xF00F;       // 11) seq3 and 0 are software triggered

    ADC1_SSMUX3_R &= ~0xF;
    ADC1_SSMUX3_R |= 0x7;   // 12) Sample 1 correlates to Analog input 7 respectively

    ADC1_SSMUX0_R &= ~0xFFFFFFFF;
    ADC1_SSMUX0_R &= 0x01234569; // Samples

    ADC1_SSCTL3_R = 0x0006;       // 13) no TS0 D0, yes IE0 END0
    ADC1_IM_R &= ~0x9;         // 14) disable SS3 and SS0 interrupts
    ADC1_ACTSS_R |= 0x9;       // 15) enable sample sequencer 3 and 0
}

/*
 * Read in values from potentiometers
 */
void PotIn(uint32_t* data) {
    ADC1_PSSI_R = 0x0008;            // 1) initiate SS3
    while((ADC1_RIS_R&0x08)==0){};   // 2) wait for conversion done
        // if you have an A0-A3 revision number, you need to add an 8 usec wait here
    data[8] = ADC1_SSFIFO3_R&0xFFF;   // 3) read result
    ADC1_ISC_R = 0x0008;               // 4) acknowledge completion

    ADC1_PSSI_R = 0x0001;            // 1) initiate SS0
    while((ADC1_RIS_R&0x01)==0){};   // 2) wait for conversion done
    data[7] = ADC1_SSFIFO0_R&0xFFF;  // 3) PD0 result
    data[6] = ADC1_SSFIFO0_R&0xFFF;  // 3) PD1 result
    data[5] = ADC1_SSFIFO0_R&0xFFF;  // 3) PD2 result
    data[4] = ADC1_SSFIFO0_R&0xFFF;  // 3) PD3 result
    data[3] = ADC1_SSFIFO0_R&0xFFF;  // 3) PE3 result
    data[2] = ADC1_SSFIFO0_R&0xFFF;  // 3) PE2 result
    data[1] = ADC1_SSFIFO0_R&0xFFF;  // 3) PE1 result
    data[0] = ADC1_SSFIFO0_R&0xFFF;  // 3) PE0 result
    ADC1_ISC_R = 0x0001;
}

