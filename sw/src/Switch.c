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
 */

static void SwitchHandler();

void SwitchInit() {
    SYSCTL_RCGCGPIO_R |= 0x20;       //activate clock for port F
    for(int i = 0; i < 100; i++){}; //delay
    GPIO_PORTF_DIR_R &= ~0x1F;      //set input for pins 0-4
    GPIO_PORTF_DEN_R |= 0x1F;       //set digital enable for pins 0-4
    Timer0A_Init(SwitchHandler, 800000, 3);
}

/*
 * Read switch inputs as a bit vector
 */
static uint8_t SwitchIn() {
    return GPIO_PORTF_DATA_R & 0x1F;
}

static void SwitchHandler() {
    uint8_t switches = SwitchIn();
    AudioCommandsOut();
}



