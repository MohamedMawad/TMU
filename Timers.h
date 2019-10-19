/*
 * Timers.h
 *
 *  Created on: ??þ/??þ/????
 *      Author: Mohamed ELameer
 */

#ifndef TIMERS_H_
#define TIMERS_H_


#include "Timer_config.h"
#include "MICRO_setting.h"

#define NULL_PTR    ((void*)0)

extern uint8_t over_flow;

typedef enum{
	T0,T2
}t_type;


void Delay_ms(uint64_t n_milleSec,t_type timer);

void Dely_ms_service(t_type timer,uint64_t time_MS,void(*ptr_display)(void));



void timer_init(timer_type type );

void Timer_Square_Wave_Init(void);

void Timer_setCallBack(void(*a_ptr)(void));

void Toggle_half_sec(void);


void Toggle_Sec(void);

extern uint8_t duty_percentage;

void SPEED(void);

void timer_resolution(timer_type type,uint8_t resolution_ms);


void TIMER_PWM (uint8_t timer, uint8_t duty);
void interrrupt_call(void);

void timer_setup( timer_type type);





#endif /* TIMERS_H_ */
