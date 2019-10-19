/*
 * TMU_V2.c
 *
 * Created: 10/17/2019 12:27:38 PM
 * Author : M_Moawad
 */ 

#include <avr/io.h>
#include "tmu.h"
#include "Timers.h"

void Toggle_first_led (void);
void Toggle_second_led (void);
void Toggle_third_led (void);



int main(void)
{
	DDRD = 0xff;
	DDRC = 0xff;
	DDRB = 0;
	TMU_config* config;
	config->resolution=2;
	config->type=Timer0;

	TMU_Init(config);

	sei();
	
	
	
	TMU_Start (1000,Toggle_first_led,periodic);
	TMU_Start (700,Toggle_second_led,periodic);
	TMU_Start (400,Toggle_third_led,one_shot);
	
	
	
	
	/* Replace with your application code */
	while (1)
	{
		
		TMU_Dispatch();

		if(PINB & (1<<PINB0))
		{
			TMU_Stop(Toggle_second_led);
			//TMU_Start(500,Toggle_third_led,periodic);
			while(PINB & (1<<PINB0));
		}
		if(PINB & (1<<PINB1))
		{
			//TMU_Stop(Toggle_second_led);
			//TMU_Stop(Toggle_first_led);
			TMU_Start(500,Toggle_third_led,periodic);
			while(PINB & (1<<PINB1));
		}
		
		
		
	}
}

void Toggle_first_led (void)
{

	PORTC ^= ( 1<< PINC4);

}

void Toggle_second_led (void)
{

	PORTC ^= ( 1<< PINC5);

}

void Toggle_third_led (void)
{

	PORTC ^= ( 1<< PINC6);

}
