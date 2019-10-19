/*
 * Timer_config.h
 *
 *  Created on: ??þ/??þ/????
 *      Author: Mohamed ELameer
 */

#ifndef TIMER_CONFIG_H_
#define TIMER_CONFIG_H_


#include "MICRO_setting.h"
#define T1_MAX 0XFFFF//added
#define T_MAX 0XFF//added
#define NA 0xff
#define one_pin        0//added
#define two_pins       1//added
typedef enum{
	Timer0,Timer1,Timer2,MAX_num_of_timers
}timer_type;

typedef enum{
	Dis_INT,EN_INT
}InT_timer;



typedef enum{
	NO_clk,F_cpu_clk,F_cpu_8,F_cpu_64,F_cpu_256,F_cpu_1024,Exter_F_clk,Exter_R_clk
}timer_clk;

typedef enum{
	WG_Normal,WG_PWM_PHC,WG_CTC,WG_FPWM,WG_T1CTC=4
}timer_WGMode;

typedef enum
{
	T1_NORMAL,PWM_PHASE_8,PWM_PHASE_9,PWM_PHASE_10,CTC_OCR1A,FAST_PWM_8,FAST_PWM_9,FAST_PWM_10,
	PWM_PHASE_FREQ_ICR1,PWM_PHASE_FREQ_OCR1A,PWM_PHASE_ICR1,PWM_PHASE_OCR1A,CTC_ICR1,RESERVED,FAST_PWM_ICR1,FAST_PWM_OCR1A,MAX_T1_MODES
}timer1_WGMode;//added



typedef enum{
	NPWM_Normal=0,
	NPWM_Toggle=1,
	NPWM_Clear=2,
	NPWM_Set=3,
	PHC_PWM_Clear_up_set_down=2,
	PHC_PWM_Set_up_clear_down=3, 
    FPWM_No_inverting_mode=2,
    FPWM_Inverting_mode=3,
	MAX_COMPARE,

}timer_COM;




typedef enum{
	Falling,Rising
}ICU_edges;//ADDED

typedef enum{
	EN_ICU,DIS_ICU
}ICU_COND;//ADDED
typedef struct{
	timer_clk clock;
	ICU_edges edge;
}ICU_config;

typedef struct{
	uint8_t     OUTPUT_PINS;//ADDED
	ICU_COND     T_ICU;// ADDED
	ICU_edges  CLK_EDGE;// ADDED
    timer_type   T_type;
    InT_timer    T_INT;
	//timer_clk    T_clk;
	timer_WGMode T_WGMode;
	uint16_t      ComVal;
	timer_COM    T_Compare_out_mode;
	
	//ICU_config  ICU_conf;

}Timer_config;




void Clear_Timers(void);


extern Timer_config Timers_setup[MAX_num_of_timers];






#endif /* TIMER_CONFIG_H_ */
