// TLV5616.c
// Runs on TM4C123
// Use SSI1 to send a 16-bit code to the TLV5616 and return the reply.
// Daniel Valvano
// EE445L Fall 2015
//    Jonathan W. Valvano 9/22/15

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// SSIClk (SCLK) connected to PD0
// SSIFss (FS)   connected to PD1
// SSITx (DIN)   connected to PD3

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

//----------------   DAC_Init     -------------------------------------------
// Initialize TLV5616 12-bit DAC
// assumes bus clock is 80 MHz
// inputs: initial voltage output (0 to 4095)
// outputs:none
void DAC_Init(void){
    SYSCTL_RCGCGPIO_R |= 0x08;       // Port D
    while((SYSCTL_PRGPIO_R & 0x08) == 0){}; // wait for port D ready
    SYSCTL_RCGCSSI_R |= 0x02;        // SSI1

    //PD0 = SSI1Clk, PD1 = SSI1Fss, PD3 = SSI1Tx
    GPIO_PORTD_AFSEL_R |= 0x0B;   // enable alt funct on PD0,1,3
    GPIO_PORTD_DEN_R   |= 0x0B;   // configure PD0,1,3 as SSI
    GPIO_PORTD_AMSEL_R &= ~0x0B; // disable analog functionality on PD0, 1, 3

    // Clear PCTL fields for PD0,13, then set PD0,1,3 = 2 (SSI)
    GPIO_PORTD_PCTL_R &= ~0xFFFF;
    GPIO_PORTD_PCTL_R |= 0x2022;

    SSI1_CR1_R = 0x00000000;          // disable ssi during config
    SSI1_CPSR_R = 0x06;                  //~10 MHz with SCR=0
    SSI1_CR0_R = 0x008F;              // SCR=0, SPH=1, SPO=0, DSS=16-bit
    SSI1_CR1_R |= 0x02;               // enable SSI
}

void DAC_Out(uint16_t code){
    while((SSI1_SR_R & 0x02) == 0){}; // // Read SSI Status register until FIFO is not full (bit 1 is set)
    SSI1_DR_R = code;
    while((SSI1_SR_R & 0x10) != 0){}; // wait until BSY==0 (transfer finished)
}

// --------------     DAC_OutNonBlocking   ------------------------------------
// Send data to TLV5616 12-bit DAC without checking for room in the FIFO
// inputs:  voltage output (0 to 4095)
// 
void DAC_Out_NB(uint16_t code){
    // Consider writing this (If it is what your heart desires)
    // Consider the following registers:
	  // SSI1_SR_R, SSI1_DR_R
}
