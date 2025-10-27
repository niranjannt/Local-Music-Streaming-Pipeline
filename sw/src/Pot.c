#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timers.h"
#include "Pot.h"
#include "../inc/SysTickInts.h"
#include "../inc/TLV5616.h"
#include "AudioCommands.h"

/*
 * AIN0 - PE3 - Flanger0
 * AIN1 - PE2 - Flanger1
 * AIN2 - PE1 - Distortion
 * AIN3 - PE0 - Bass In
 * AIN4 - PD3 - Bass Volume
 * AIN5 - PD2 - Mid Volume
 * AIN6 - PD1 - Treble Volume
 * AIN7 - PD0 - Volume
 */

static void PotHandler();

void PotInit() {
    //ADC and Clock initializations
    SYSCTL_RCGCADC_R |= 0x00FF;               // 1) activate ADC0-7
    SYSCTL_RCGCGPIO_R |= 0x18;                // 2) activate clock for Port E and Port D
    while((SYSCTL_PRGPIO_R&0x18) != 0x18){};  // 3) for stabilization

    //Port E initializations
    GPIO_PORTE_DIR_R &= ~0x0F;    // 4) make PE0-3 input
    GPIO_PORTE_AFSEL_R |= 0x0F;   // 5) Set the direction of PE0-3 as input
    GPIO_PORTE_DEN_R &= ~0x0F;    // 6) Enable alternative function for PE0-3 (Alternative signal rather than GPIO)
    GPIO_PORTE_AMSEL_R |= 0x0F;   // 7) enable analog functionality on PE0-3

    //Port D initializations
    GPIO_PORTD_DIR_R &= ~0x0F;    // 4) make PD0-3 input
    GPIO_PORTD_AFSEL_R |= 0x0F;   // 5) Set the direction of PD0-3 as input
    GPIO_PORTD_DEN_R &= ~0x0F;    // 6) Enable alternative function for PD0-3 (Alternative signal rather than GPIO)
    GPIO_PORTD_AMSEL_R |= 0x0F;   // 7) enable analog functionality on PD0-3

    ADC0_PC_R &= ~0xF;
    ADC0_PC_R |= 0x1;             // 8) configure for 125K samples/sec
    ADC0_SSPRI_R = 0x0123;        // 9) Sequencer 3 is highest priority
    ADC0_ACTSS_R &= ~0x0008;      // 10) disable sample sequencer 3
    ADC0_EMUX_R &= ~0xF000;       // 11) seq3 is software trigger
    ADC0_SSMUX3_R &= ~0xFFFFFFF;
    ADC0_SSMUX3_R |= 0x7654321;   // 12) Sample 1-8 correlate to Analog input 0-7 respectively
    ADC0_SSCTL3_R = 0x0006;       // 13) no TS0 D0, yes IE0 END0
    ADC0_IM_R &= ~0x0008;         // 14) disable SS3 interrupts
    ADC0_ACTSS_R |= 0x0008;       // 15) enable sample sequencer 3
    Timer1A_Init(PotHandler, 800000, 3);
}

/*
 * Read in values from potentiometers
 */
void PotIn() {

}

static void PotHandler() {
    PotIn();
    AudioCommandOut();
}
