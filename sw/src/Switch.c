// File **********Switch.c***********
// Spring 2025
// define your hardware interface
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timers.h"
#include "Switch.h"
#include "../inc/SysTickInts.h"
#include "../inc/TLV5616.h"
#include "AudioCommands.h"
/*
 * PF0 - Pause
 * PF1 - LCDMode
 * PF2 - Flanger
 * PF3 - Distortion
 * PF4 - Reverb
 * PC7 - Delay
 * PE5 - Tempo
 */

void SwitchInit() {
    SYSCTL_RCGCGPIO_R |= 0x34;       //activate clock for port F, C, and E
    while((SYSCTL_PRGPIO_R&0x34) == 0){}; // wait
    GPIO_PORTF_DIR_R &= ~0x1F;      //set input for PF0-4
    GPIO_PORTF_DEN_R |= 0x1F;       //set digital enable for PF0-4

    GPIO_PORTC_DIR_R &= ~0x80; // set input for PC7
    GPIO_PORTC_DEN_R |= 0x80; // digital enable PC7

    GPIO_PORTE_DIR_R &= ~0x20; // input PE5
    GPIO_PORTE_DEN_R |= 0x20; // digital enable PE5
}
/*
 * Read switch inputs as a bit vector
 */
static uint32_t SwitchIn() {
    return (GPIO_PORTF_DATA_R & 0x1F) & (GPIO_PORTC_DATA_R & 0x80) & (GPIO_PORTE_DATA_R & 0x20);
}




