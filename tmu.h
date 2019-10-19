
#ifndef TMU_H_
#define TMU_H_


#include "tmu_config.h"
#include "Timers.h"

#define TIMEOUT -5
#define FULL_BUFFER -4
#define EMPTY_BUFFER -3
//#define 
#define OK 0

typedef enum{

	one_shot,
	periodic

}periodicity_enum;

typedef struct{

	uint8_t resolution;
	timer_type type;

}TMU_config;


extern void TMU_Start (uint16_t delay_time ,void (*func) (void),periodicity_enum periodicity);
extern void TMU_Stop ( void (*func)(void) );
extern int8_t TMU_Dispatch (void);
extern void TMU_Init(TMU_config* ptr_config);


#endif /* TMU_H_ */
