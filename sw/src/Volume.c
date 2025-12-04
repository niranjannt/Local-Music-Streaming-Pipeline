/*
 * Volume.c
 *
 *  Created on: Dec 2, 2025
 *      Author: Ashton
 */
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Volume.h"

//const uint32_t bassSettings = {0x00, 0x08, 0x04, 0x0C};
//const uint32_t middSettings = {0x00, 0x20, 0x10, 0x30};
//const uint32_t trebSettings = {0x00, 0x80, 0x40, 0xC0};
//
//#define PB0   (*((volatile uint32_t *)0x40005004))
//#define PB1   (*((volatile uint32_t *)0x40005008))
//#define PB2   (*((volatile uint32_t *)0x4000500C))
//#define PB3   (*((volatile uint32_t *)0x40005010))
//#define PB4   (*((volatile uint32_t *)0x40005014))
//#define PB5   (*((volatile uint32_t *)0x40005018))
//#define PB6   (*((volatile uint32_t *)0x4000501C))
//#define PB7   (*((volatile uint32_t *)0x40005020))

void Clock_Delay1ms(uint32_t n);
void Clock_Delay(uint32_t ulCount);

void initVolume() {
    SYSCTL_RCGCGPIO_R |= 0x06;  // activate clock for Port B and C
    Clock_Delay1ms(1);

    GPIO_PORTB_DIR_R |= 0x00FC;         // PB2, PB3, PB4, PB5, PB6, PB7 out
    GPIO_PORTB_DEN_R |= 0x00FC;         // enable digital I/O on PB2, PB3, PB4, PB5, PB6, PB7
    GPIO_PORTB_DATA_R &= ~(0x00FC);          // Initialize the Pins to Off


    GPIO_PORTC_DIR_R |= 0x70;         // PC4, PC5, PC6
    GPIO_PORTC_DEN_R |= 0x70;
    GPIO_PORTC_DATA_R &= ~(0x30);
    GPIO_PORTC_DATA_R |= (0x40);
}




void setBass(unsigned input){
    switch (input) {
    case 0 :
        GPIO_PORTB_DATA_R &= ~(0x0C);
        break;
    case 1 :
        GPIO_PORTB_DATA_R &= ~(0x08);
        GPIO_PORTB_DATA_R |= (0x04);
        break;
    case 2 :
        GPIO_PORTB_DATA_R &= ~(0x04);
        GPIO_PORTB_DATA_R |= (0x08);
        break;
    case 3 :
        GPIO_PORTB_DATA_R |= (0x0C);
        break;
    default :
        GPIO_PORTB_DATA_R &= ~(0x0C);
    }

}

void setMid(unsigned input){
    switch (input) {
    case 0 : // 0 - 1023
        GPIO_PORTB_DATA_R &= ~(0x30);
        break;
    case 1 : // 1024 - 2046
        GPIO_PORTB_DATA_R &= ~(0x20);
        GPIO_PORTB_DATA_R |= (0x10);
        break;
    case 2 : // 2047 - 3069
        GPIO_PORTB_DATA_R &= ~(0x10);
        GPIO_PORTB_DATA_R |= (0x20);
        break;
    case 3 : // 3070 - 4095
        GPIO_PORTB_DATA_R |= (0x30);
        break;
    default :
        GPIO_PORTB_DATA_R &= ~(0x30);
    }

}

void setTreb(unsigned input){
    switch (input) {
    case 0 :
        GPIO_PORTB_DATA_R &= ~(0xC0);
        break;
    case 1 :
        GPIO_PORTB_DATA_R &= ~(0x80);
        GPIO_PORTB_DATA_R |= (0x40);
        break;
    case 2 :
        GPIO_PORTB_DATA_R &= ~(0x40);
        GPIO_PORTB_DATA_R |= (0x80);
        break;
    case 3 :
        GPIO_PORTB_DATA_R |= (0xC0);
        break;
    default :
        GPIO_PORTB_DATA_R &= ~(0xC0);
    }

}

void setVolume(unsigned input){
    switch (input) {
    case 0 : // 0 - 585
        GPIO_PORTC_DATA_R &= ~(0x30);
        GPIO_PORTC_DATA_R |= (0x40);
        break;
    case 1 : // 586 - 1170
        GPIO_PORTC_DATA_R &= ~(0x50);
        GPIO_PORTC_DATA_R |= (0x20);
        break;
    case 2 : // 1171 - 1755
        GPIO_PORTC_DATA_R &= ~(0x60);
        GPIO_PORTC_DATA_R |= (0x10);
        break;
    case 3 : // 1756 - 2340
        GPIO_PORTC_DATA_R &= ~(0x10);
        GPIO_PORTC_DATA_R |= (0x60);
        break;
    case 4 : // 2341 - 2925
        GPIO_PORTC_DATA_R &= ~(0x20);
        GPIO_PORTC_DATA_R |= (0x50);
        break;
    case 5 : // 2926 - 3510
        GPIO_PORTC_DATA_R &= ~(0x40);
        GPIO_PORTC_DATA_R |= (0x30);
        break;
    case 6 : // 3511 - 4095
        GPIO_PORTC_DATA_R |= (0x70);
        break;
    default :
        GPIO_PORTC_DATA_R |= (0x70);
    }

}




void Clock_Delay(uint32_t ulCount){
  while(ulCount){
    ulCount--;
  }
}

// ------------Clock_Delay1ms------------
// Simple delay function which delays about n milliseconds.
// Inputs: n, number of msec to wait
// Outputs: none
void Clock_Delay1ms(uint32_t n){
  while(n){
    Clock_Delay(23746);  // 1 msec, tuned at 80 MHz, originally part of LCD module
    n--;
  }
}
