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
    Timer0A_Init(SwitchHandler, 800000, 3);
}

/*
 * Read switch inputs
 */
static void SwitchIn() {
}

static void SwitchHandler() {
    SwitchIn();
    AudioCommandsOut();
}



