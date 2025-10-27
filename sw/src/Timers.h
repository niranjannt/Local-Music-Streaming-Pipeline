/*
 * Timers.h
 *
 *  Created on: Oct 5, 2025
 *      Author: nathan
 */

#ifndef TIMERS_H_
#define TIMERS_H_

void Timer0A_Init(void(*task)(void), uint32_t period, uint32_t priority);
void Timer0A_Stop(void);
void Timer0A_Start(void);

void Timer1A_Init(void(*task)(void), uint32_t period, uint32_t priority);
void Timer1A_Stop(void);
void Timer1A_Start(void);

void Timer2A_Init(void(*task)(void), uint32_t period, uint32_t priority);
void Timer2A_Stop(void);
void Timer2A_Start(void);

void Timer3A_Init(void(*task)(void), uint32_t period, uint32_t priority);
void Timer3A_Stop(void);

#endif /* TIMERS_H_ */
