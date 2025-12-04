#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/SysTickInts.h"
#include "../inc/TLV5616.h"
#include "AudioCommands.h"
#include "Timers.h"
#include "../inc/UART.h"
#include "Switch.h"
#include "Pot.h"
#include "Volume.h"

//Send audio commands through UART
/*
 * PB0 - U1RX
 * PB1 - U1TX
 */

/*
 * Initialize UART
 */

_Bool changeVolumes = 0;
unsigned bassVol = 0;
unsigned midVol = 0;
unsigned trebVol = 0;
unsigned mainVol = 0;


uint16_t data[10];
uint16_t HEADER = 0xFFFF;

void AudioCommandInit() {
    SYSCTL_RCGCUART_R |= 0x02;      // enable UART1 clock
    SYSCTL_RCGCGPIO_R |= 0x02;      // enable Port B clock
    volatile int delay = SYSCTL_RCGCGPIO_R;

    UART1_CTL_R &= ~UART_CTL_UARTEN;  // disable UART1

    // configure PB0, PB1 for UART
    GPIO_PORTB_AFSEL_R |= 0x03;
    GPIO_PORTB_DEN_R   |= 0x03;
    GPIO_PORTB_DIR_R   |= 0x02;
    GPIO_PORTB_PCTL_R  = (GPIO_PORTB_PCTL_R & 0xFFFFFF00) | 0x00000011;
    GPIO_PORTB_AMSEL_R &= ~0x03;

    // UART1 settings
    UART1_IBRD_R = 43;
    UART1_FBRD_R = 26;
    UART1_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;

    UART1_CTL_R |= UART_CTL_TXE | UART_CTL_RXE;
    UART1_CTL_R |= UART_CTL_UARTEN;   // ENABLE LAST

    PotInit();
    SwitchInit();
    initVolume();
    Timer0A_Init(AudioCommandsHandler, 800000, 3);
}

/*
 * Send signals to UART
 */


void AudioCommandsHandler() {
    data[9] = SwitchIn();
    PotIn(&data[0]);
    /* Test Code
    data[9] = 800;
    data[8] = 12;
    data[7] = 4095;
    data[6] = 404;
    data[5] = 0;
    data[4] = 2400;
    data[3] = 1206;
    data[2] = 1000;
    data[1] = 505;
    data[0] = 7;
    */
    if (bassVol != (data[0] / 1024)) {
        bassVol = data[0] / 1024;
        setBass(bassVol);
    }
    if (midVol != (data[4] / 1024)) {
        midVol = data[4] / 1024;
        setMid(midVol);
    }
    if (trebVol != (data[5] / 1024)) {
        trebVol = data[5] / 1024;
        setTreb(trebVol);
    }
    if (mainVol != (data[6] / 585)) {
        mainVol = data[6] / 585;
        setVolume(mainVol);
    }

    AudioCommandOut();
}

void UART_Out(uint16_t n){
    uint8_t transmit = (n >> 8) & 0xFF; // Upper 8 bits of data
    while((UART1_FR_R&UART_FR_TXFF) != 0);
    UART1_DR_R = transmit;
    transmit = n & 0xFF; // Lower 8 bits of data
    while((UART1_FR_R&UART_FR_TXFF) != 0);
    UART1_DR_R = transmit;
}

void AudioCommandOut() {
    UART_Out(HEADER);
    for (int index = 0; index < 10; index++) {
        UART_Out(data[index]);
    }
}




