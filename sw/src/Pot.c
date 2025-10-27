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
