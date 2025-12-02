#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/SysTickInts.h"
#include "../inc/TLV5616.h"
#include "AudioCommands.h"
#include "Timers.h"
#include "../inc/UART.h"
#include "Switch.h"
#include "Pot.h"

//Send audio commands through UART
/*
 * PB0 - U1RX
 * PB1 - U1TX
 */

/*
 * Initialize UART
 */


int index = 0;
uint32_t data[10];
uint32_t HEADER = 0xFFFF;

void AudioCommandInit() {
    SYSCTL_RCGCUART_R |= 0x02;            // activate UART1
    SYSCTL_RCGCGPIO_R |= 0x02;            // activate port B
    UART1_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
    UART1_IBRD_R = 43;                    // IBRD = int(80,000,000 / (16 * 115,200)) = int(43.403)
    UART1_FBRD_R = 26;                    // FBRD = round(0.4028 * 64 ) = 26
                                          // 8 bit word length (no parity bits, one stop bit, FIFOs)
    UART1_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
    UART1_CTL_R |= UART_CTL_UARTEN;       // enable UART
    GPIO_PORTB_AFSEL_R |= 0x03;           // enable alt funct on PB1-0
    GPIO_PORTB_DEN_R |= 0x03;             // enable digital I/O on PB1-0
                                          // configure PA1-0 as UART
    GPIO_PORTB_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
    GPIO_PORTB_AMSEL_R &= ~0x03;          // disable analog functionality on PB

    PotInit();
    SwitchInit();
    Timer0A_Init(AudioCommandsHandler, 800000, 3);
}

/*
 * Send signals to UART
 */


void AudioCommandsHandler() {
    data[9] = SwitchIn();
    PotIn(&data[0]);
    AudioCommandOut();
}

void UART_Out(uint32_t n){
    uint32_t transmit = n & 0xFF; // Lower 8 bits of data
    while((UART1_FR_R&UART_FR_TXFF) != 0);
    UART1_DR_R = transmit;
    transmit = n >> 8; // Upper 8 bits of data if needed
    while((UART1_FR_R&UART_FR_TXFF) != 0);
    UART1_DR_R = transmit;
}

void AudioCommandOut() {
    UART_Out(HEADER);
    for (index = 9; index >= 0; index--) {
        UART_Out(data[index]);
    }
}




