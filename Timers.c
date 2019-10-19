
#include "Timers.h"
#include"tmu.h"

uint8_t over_flow=0;



/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;
static volatile void (*g_callBackPtr2)(void) = NULL_PTR;

#define NUMBER_OF_OVERFLOWS_PER_HALF_SECOND 2

uint8_t duty_percentage;


volatile uint8_t  g_tick_off=0;

volatile uint64_t g_time_MS;

volatile uint8_t  g_tick = 0,g_tick2=0;



/*********************************************************************************************************
 * *******************************************************************************************************
 * ****************************************************************************************************
 * *******************************************************************************************************/
/*TIMER0________________Interrupt*/

ISR(TIMER0_OVF_vect)
{

	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)();
		/* another method to call the function using pointer to function g_callBackPtr(); */
	}
}



ISR(TIMER0_COMP_vect)
{

	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)();
		/* another method to call the function using pointer to function g_callBackPtr(); */
	}

}


/*********************************************************************************************************
 * *******************************************************************************************************
 * ****************************************************************************************************
 * *******************************************************************************************************/

/*Timer2_______Interrupt*/

void ISR_TIMER2_OVF (void)
{
	//if( !( GIFR & (1<<INTF2) ))
	   over_flow++;

	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}



void ISR_TIMER2_COMP (void)
{

	if(g_callBackPtr2 != NULL_PTR)
	{ 		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr2)();
		/* another method to call the function using pointer to function g_callBackPtr(); */

	}

}


/*********************************************************************************************************
 * *******************************************************************************************************
 * ****************************************************************************************************
 * *******************************************************************************************************/

void set_interrupt_call_back(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}



void Timer_setCallBack2(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr2 = a_ptr;
}



/*********************************************************************************************************
 * *******************************************************************************************************
 * ****************************************************************************************************
 * *******************************************************************************************************/


void timer_init(timer_type type )
{




		switch(type)
		{
		case Timer0:

			TCNT0=0; //timer initial value

			/*******************TIMER0 EDIT SETTINGS************/

			TCCR0 = (1<<FOC0) //| (Timers_setup[type].T_clk)
			    	|((Timers_setup[type].T_WGMode&0x02)<<2)
				    |((Timers_setup[type].T_WGMode&0x01)<<6)
					|(Timers_setup[type].T_Compare_out_mode<<4);

			/*************CHOOSE TIMER0 MODES*************************/

			switch(Timers_setup[type].T_WGMode)   //enable overflow interrupt
			{
			case WG_Normal:
				if(Timers_setup[type].T_INT)
					TIMSK |= (1<<TOIE0);				 //En_TOF_interrupt of timer0

				break;
				/***********************************************/

			case WG_CTC :
				if(Timers_setup[type].T_INT)
					TIMSK |= (1<<OCIE0);				//En_TCOM_interrupt of timer0

				OCR0 = Timers_setup[type].ComVal;

				/***********************************************/

				break;
				/*****************************************************/

			case WG_FPWM :
				OCR0 = Timers_setup[type].ComVal;

				break;
				/******************************************************/

			default:
				break;
			}
			break;



			case Timer1:
				TCNT1 = 0; // clear T1
				TCCR1A=0;//added
				TCCR1B=0;//added
				TCCR1A=(TCCR1A &0XFC)|((Timers_setup[type].T_WGMode) &0X03);//MODE SELECT //adjusted
				TCCR1B=(TCCR1B &0XE7)|(((Timers_setup[type].T_WGMode)<<1)&0X18);//MODE SELECT BITS //adjusted
				TCCR1B |= (TCCR1B & 0xF8);// | Timers_setup[type].T_clk;//CLK SET //adjusted


				switch(Timers_setup[type].T_WGMode)   //enable overflow interrupt
				{
				case WG_Normal:
					//ADDED
					if(Timers_setup[type].T_ICU == EN_ICU)
					{
						DDRD &= ~(1<<PD6);
						ICR1H =0;
						ICR1L =0;
						TCNT1H=0;
						TCNT1L=0;
						TIMSK |= (1<<TICIE1);
						if(Timers_setup[type].CLK_EDGE == Rising)
						{
							TCCR1B = (TCCR1B & 0xBF) | ((Rising)<<6);
						}
						else if (Timers_setup[type].CLK_EDGE == Falling)
						{
							TCCR1B = (TCCR1B & 0xBF) | ((Falling)<<6);
						}

					}

					// END OF ADJUSTING

					if(Timers_setup[type].T_INT)
						TIMSK |= (1<<TOIE1);
					break;
				case WG_T1CTC :
					if(Timers_setup[type].T_INT)
					{
						//unit A
						TIMSK |= (1<<OCIE1A);
						//unit B
						//TIMSK |= (1<<OCIE1B);
					}

					//unit A
					OCR1AH=(char)(0x1324>>8);
					OCR1AL=(char)(0x1324);
					//unit B
					//OCR1BH=(64<<8);
					//OCR1BL=(64>>8);

					break;
				case PWM_PHASE_8://added

					TCCR1A &=~(1<<FOC1A);
					TCCR1A &=~(1<<FOC1B);
					 if (Timers_setup[type].OUTPUT_PINS==one_pin)
					 {DDRD|=(1<<PD5);
					 if (Timers_setup[type].T_Compare_out_mode <MAX_COMPARE)
					 {
						 TCCR1A|=(TCCR1A &0X3F)| ((Timers_setup[type].T_Compare_out_mode) <<6);// COMPARE MODE SIT
						 if (Timers_setup[type].ComVal <=T1_MAX )
						 {
							 OCR1AH=(uint8_t)(Timers_setup[type].ComVal>>8); /*COMPARE VALUE IN OCR1A*/
							 OCR1AL=(uint8_t)(Timers_setup[type].ComVal);
						 }

					 }

					 }
					 else if (Timers_setup[type].OUTPUT_PINS==two_pins)
					 {DDRD|=(1<<PD5)|(1<<PD4);
					 if (Timers_setup[type].T_Compare_out_mode <MAX_COMPARE)
					 {
						 TCCR1A|=(TCCR1A &0X3F)| ((Timers_setup[type].T_Compare_out_mode) <<6);//COMPARE MODE  SET
						 TCCR1A|=(TCCR1A &0XCF)| (((Timers_setup[type].T_Compare_out_mode) <<4)&0X30);
						 if (Timers_setup[type].ComVal <=T1_MAX )
						 {

							 OCR1AH=(uint8_t) (Timers_setup[type].ComVal>>8); //COMPARE VALUE IN OCR1A
							 OCR1AL=(uint8_t)(Timers_setup[type].ComVal);
							 OCR1BH=(uint8_t)Timers_setup[type].ComVal>>8; //COMPARE VALUE IN OCR1B
							 OCR1BL=(uint8_t)(Timers_setup[type].ComVal);
						 }

					 }
					 }
					 break;
					 /*case WG_FPWM :
									//unit A
									OCR1AH=(64<<8);
									OCR1AL=(64>>8);
									//unit B
									OCR1BH=(64<<8);
									OCR1BL=(64>>8);
									break;*/

				default:
					break;
				}
				break;








				case Timer2:

					TCNT2=0; //timer initial value

					/*EDIT_________ TIMER02*/

					TCCR2 = (1<<FOC2) //| (Timers_setup[type].T_clk)
																						|((Timers_setup[type].T_WGMode&0x02)<<2)
																						|((Timers_setup[type].T_WGMode&0x01)<<6)
																						|(Timers_setup[type].T_Compare_out_mode<<4);

					/*************CHOOSE TIMER0 MODES*************************/

					switch(Timers_setup[type].T_WGMode)   //enable overflow interrupt
					{
					case WG_Normal:

						if(Timers_setup[type].T_INT)
							TIMSK |= (1<<TOIE2);              //En_TOF_interrupt of timer0
						break;

					case WG_CTC :
						if(Timers_setup[type].T_INT)
							TIMSK |= (1<<OCIE2); //En_TCOM_interrupt of timer2

						OCR2 = Timers_setup[type].ComVal;

						break;
					case WG_FPWM :
						OCR2 = Timers_setup[type].ComVal;

						break;

					default:
						break;
					}
					break;



					default:
						break;



	}
}
void timer_setup(timer_type type )
{
	switch(type)
	{
	case Timer0:
		    TCNT0=0; //timer initial value

			TCCR0 = (1<<FOC0);

			TIMSK |= (1<<TOIE0);
			break;
	default:
		break;

	}
}

void timer_resolution(timer_type type,uint8_t resolution_ms)
{
	//uint32_t prescaler;
	if( (resolution_ms==2)  )
	{
	   //prescaler=( (resolution_ms) / 256 )*( freq_MHZ * 1000);

		TCCR0 =  F_cpu_8;

		


	}
    else if((resolution_ms=1))
	{
		TCCR0 =  F_cpu_8;
		/* go to configration edit it to work on compare value equal to 128*/
		
	}
	
	

}


/*********************************************************************************************************
 * *******************************************************************************************************
 * ****************************************************************************************************
 * *******************************************************************************************************/



/*********************************************************************************************************
 * *******************************************************************************************************
 * ****************************************************************************************************
 * *******************************************************************************************************/

void Timer_Square_Wave_Init(void)
{

	uint8_t type=0;
	for(type=0;type<MAX_num_of_timers;type++)
	{

		if(Timers_setup[type].T_type==Timer0)
		{
			DDRB = DDRB | (1<<PB3);                       	//enable OC0 as output PIN
		}
		if(Timers_setup[type].T_type==Timer2)
		{
			DDRD = DDRD | (1<<PD7);                         //enable OC2 as output PIN
		}
		if(Timers_setup[type].T_type==Timer1)
		{
			DDRD = DDRD | (1<<PD5);             //enable OC1A as output PIN
			//DDRD = DDRD | (1<<PD4);            //enable OC1B as output PIN
		}
	}



}



/*********************************************************************************************************
 * *******************************************************************************************************
 * ****************************************************************************************************
 * *******************************************************************************************************/

/*DELAY______BY_______POLLING*/

void Delay_ms(uint64_t n_milleSec,t_type timer)
{


	static uint8_t count31=0;

	if(timer==T0)
	{

		while(n_milleSec)
		{

			while( !(TIFR & (1<<TOV0) ) );        //go out of loop if there is interrupt happens
			count31++;
			if (31 == count31)                    //if timer flag be one by 31 time as 1ms "interrupt happens
				//by o.f
			{

				count31 = 0;
				n_milleSec--;

			}
			TIFR |= (1<<TOV0);                    //clear T0 interrupt flag
		}

	}

	if(timer==T2)
	{

		while(n_milleSec)
		{

			while( !(TIFR & (1<<TOV2) ) );
			count31++;
			if (31 == count31)
			{

				count31 = 0;
				n_milleSec--;

			}
			TIFR |= (1<<TOV2);



		}
	}

}



/*********************************************************************************************************
 * *******************************************************************************************************
 * ****************************************************************************************************
 * *******************************************************************************************************/

/*pointer to call the function when the interrupt happens*/

void Wait_ms(void)
{

	static uint64_t count=0;                           //the number you wanted to delay as EX delay(3000ms)
	static uint8_t count31=0;                          //the number that will reach to make a 1ms
	count31++;

	if (31 == count31)
	{
		count++;
		count31 = 0;

	}

	if(count == g_time_MS)                            //if the number reached the number you want call the func
	{

		if(g_callBackPtr2 != NULL_PTR)
		{
			/* Call the Call Back function in the application after the edge is detected */
			(*g_callBackPtr2)();
			/* another method to call the function using pointer to function g_callBackPtr(); */
		}

		count=0;

	}

}



void Dely_ms_service(t_type timer,uint64_t time_MS,void(*ptr_display)(void))
{


	g_time_MS = time_MS;
	g_callBackPtr = Wait_ms;                //interrupt call

	g_callBackPtr2=ptr_display;             //func call



}


void SPEED(void){


	//g_tick_on++;
	g_tick_off++;
	if(g_tick_off == 100- duty_percentage)
	{
		PORTC|= (1<<PC0); //toggle led every 0.5 second

		//clear the tick counter again to count a new 0.5 second



	}
	if(g_tick_off ==  100 )
	{
		PORTC  &=~ (1<<PC0); //toggle led every 0.5 second
		//DIO_write(PORT_C,PIN0,Toggle);
		g_tick_off = 0; //clear the tick counter again to count a new 0.5 second
	}


}


void TIMER_PWM (uint8_t timer, uint8_t duty)//added
{
	uint8_t OCR_VALUE;
if (timer==Timer0)
{
	OCR_VALUE=(duty*T_MAX)/100;

	if (OCR_VALUE <=T_MAX )
	{ /*  Set the compare value IN  OCR0*/
		OCR0=OCR_VALUE; /*COMPARE VALUE IN OCR0*/
	}


}
else if (timer==Timer2)
{
	OCR_VALUE=(duty*T_MAX)/100;

	if (OCR_VALUE <=T_MAX )
	{ /*  Set the compare value IN  OCR2*/
		OCR2=OCR_VALUE; /*COMPARE VALUE IN OCR0*/
	}

}
else if (timer==Timer1)
{
	OCR_VALUE=(duty*T_MAX)/100;

	if (OCR_VALUE <=T1_MAX )
	{ /*  Set the compare value IN  OCR1A*/
		OCR1B=OCR_VALUE;
		OCR1A=OCR_VALUE; /*COMPARE VALUE IN OCR0*/
	}


}

}


