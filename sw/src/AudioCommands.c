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

static void AudioCommandsHandler();

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
uint8_t index = 0;
uint32_t data[10];

void AudioCommandsHandler() {
    data[9] = SwitchIn();
    PotIn(data);
    AudioCommandOut();
}

void AudioCommandOut() {
    for (index = 0; index < 10; index++) {
        UART_OutUDec(data[index]);
    }
}

//-----------------------UART_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void UART_OutUDec(uint32_t n){
// This function uses recursion to convert decimal number
//   of unspecified length as an ASCII string
  if(n >= 10){
    UART_OutUDec(n/10);
    n = n%10;
  }
  UART_OutChar(n+'0'); /* n is between 0 and 9 */
}

//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART_OutChar(char data){
  while((UART1_FR_R&UART_FR_TXFF) != 0);
  UART1_DR_R = data;
}
