
#include"tmu.h"
/*
*using the mechanism of circular buffer that starts with rested full_flag & head= tail =0
*every tmu start func ,head incremented till head= tail so full flag=0
*every tmu stop or one shot func ,tail incremented till head=tail and the buffer is empty
*
*/


 static volatile uint8_t flag = 0;
 /*using these flag to implement circular buffer*/
 static uint8_t tail = 0;
 static uint8_t head = 0;
 static uint8_t full_flag = 0;

typedef void(*ptrToFunc)(void); 
typedef struct{

	ptrToFunc func_name;
	uint16_t time_delay;
	periodicity_enum periodicity;
	/**status_enum status;**/

}Buff_strct;


static Buff_strct TMU_buffer [MAX_SIZE_OF_TASKS] = {0};

static uint32_t flags[MAX_SIZE_OF_TASKS] = {0};

/**
 * @brief This function initializing the tmu by specific resolution.
 * @param pointer to struct.
 * @return void.
 */
void TMU_Init(TMU_config* ptr_config)
{
	timer_setup( ptr_config->type );

	timer_resolution( ptr_config->type , ptr_config->resolution );     /*type,resolution,Freq*/



	set_interrupt_call_back(interrrupt_call);

}



void TMU_Start (uint16_t delay_time ,ptrToFunc func,periodicity_enum periodicity)
{
	if ( full_flag == 0 )
	{
		PORTC ^= (1<<1);
		TMU_buffer [head].func_name     = func;
		TMU_buffer [head].time_delay	= delay_time;
		TMU_buffer [head].periodicity   = periodicity;
		/* Modulo operator will cause the head value to reset to 0
		 * when the maximum size is reached 
		 */
		head = (head+1) % MAX_SIZE_OF_TASKS; 
		if ( head == tail )
		{
			PORTC ^= (1<<0);
			full_flag = 1; /** The only case that can set the full_flag **/
		}
		/* TMU_buffer [head].status		= Active; */

		/* retvalue = OK; */
	}
	/*
	else
		retvalue = NOK;

	return retvalue;

	 */
}



void TMU_Stop ( ptrToFunc func )
{

	if ( tail < head)
	{
		/*retvalue = NOK;*/
		for ( uint8_t i = tail; i < head; i++)
		{
			if(TMU_buffer[i].func_name == func)
			{
				TMU_buffer[i] = TMU_buffer[tail];  /*** if the stopped function is not the last one, write the last struct over this func ***/
				flags[i] = flags[tail]; //need to edit

				/* Modulo operator will cause the tail value to reset to 0
				 * when the maximum size is reached 
				 */
				tail = (tail + 1) % MAX_SIZE_OF_TASKS;
				/*retvalue = OK;*/
				break;
			}

		}

	}

	else if ( tail > head )
	{	
		/*retvalue = NOK;*/
		uint8_t end_of_loop = MAX_SIZE_OF_TASKS;
		for ( uint8_t i = tail; i < end_of_loop; i++)
		{
			if(TMU_buffer[i].func_name == func)
			{
				TMU_buffer[i] = TMU_buffer[tail];  /***if the stopped function is not the last one, write the last struct over this func ***/
				flags[i] = flags[tail]; //need to edit
				tail = (tail + 1) % MAX_SIZE_OF_TASKS; 
				/*retvalue = OK;*/
				break;
			}

			if( i == MAX_SIZE_OF_TASKS - 1 )
			{

				i = 0;
				end_of_loop = head;				

			}

		}			
	}

	else if ( (tail == head) && (full_flag == 1) )
	{	
		/*retvalue = NOK;*/
		uint8_t end_of_loop = MAX_SIZE_OF_TASKS;
		for ( uint8_t i = tail; i < end_of_loop; i++)
		{
			if(TMU_buffer[i].func_name == func)
			{
				TMU_buffer[i] = TMU_buffer[tail];  /***if the stopped function is not the last one, write the last struct over this func ***/
				flags[i] = flags[tail]; //need to edit
				tail = (tail + 1) % MAX_SIZE_OF_TASKS;
				PORTC ^= (1<<7); 
				full_flag = 0 ; /* reset the full_flag because the buffer decremented by one element */
				/*retvalue = OK;*/
				break;
			}

			if( i == MAX_SIZE_OF_TASKS - 1 )
			{

				i = 0;
				end_of_loop = head;				

			}

		}			
	}

	else 
	{

		/*retvalue = NOK;*/ /** Becuase the buffer is already empty **/

	}

	/*return retvalue;*/

}

int8_t TMU_Dispatch (void)
{
	int8_t retvalue = OK;

	if (flag == 1)
	{
		if ( tail < head )
		{
			for (int i = tail ; i < head; i++ )
			{
				if (flags[i] >= TMU_buffer[i].time_delay  )
				{
					flags[i] = 0;

					(*TMU_buffer[i].func_name)();

					if (TMU_buffer[i].periodicity == one_shot)
					{
						TMU_buffer[i] = TMU_buffer[tail];  /***if the stopped function is not the last one, write the last struct over this func ***/
						flags[i] = flags[tail]; //need to edit
						tail = (tail + 1) % MAX_SIZE_OF_TASKS; 

					}
					if (flags[i] <= 2 * TMU_buffer[i].time_delay)
						retvalue = OK;
					else if (flags[i] > 2 * TMU_buffer[i].time_delay)
						retvalue = TIMEOUT;

				}

			}
		}

		else if ( tail > head )
		{
			uint8_t end_of_loop = MAX_SIZE_OF_TASKS;
			for ( uint8_t i = tail; i < end_of_loop; i++)
			{
				if (flags[i] >= TMU_buffer[i].time_delay)
				{
					flags[i] = 0;

					(*TMU_buffer[i].func_name)();

					if (TMU_buffer[i].periodicity == one_shot)
					{
						TMU_buffer[i] = TMU_buffer[tail];  /***if the stopped function is not the last one, write the last struct over this func ***/
						flags[i] = flags[tail]; //need to edit
						tail = (tail + 1) % MAX_SIZE_OF_TASKS; 

					}
					if (flags[i] <= 2 * TMU_buffer[i].time_delay)
					retvalue = OK;
					else if (flags[i] > 2 * TMU_buffer[i].time_delay)
					retvalue = TIMEOUT;

				}

				if( i == MAX_SIZE_OF_TASKS - 1 )
				{

					i = 0;
					end_of_loop = head;				

				}

			}	


		}

		else if ( (tail == head) && (full_flag == 1))
		{

			uint8_t end_of_loop = MAX_SIZE_OF_TASKS;
			for ( uint8_t i = tail; i < end_of_loop; i++)
			{

				if (flags[i] >= TMU_buffer[i].time_delay)
				{
					PORTC ^= (1<<2);
					flags[i] = 0;

					(*TMU_buffer[i].func_name)();

					if (TMU_buffer[i].periodicity == one_shot)
					{
						TMU_buffer[i] = TMU_buffer[tail];  /***if the stopped function is not the last one, write the last struct over this func ***/
						flags[i] = flags[tail]; //need to edit
						tail = (tail + 1) % MAX_SIZE_OF_TASKS; 
						full_flag = 0;	/* reset the full_flag because the buffer decremented by one element */

					}
										if (flags[i] <= 2 * TMU_buffer[i].time_delay)
										retvalue = OK;
										else if (flags[i] > 2 * TMU_buffer[i].time_delay)
										retvalue = TIMEOUT;

				}

				if( i == MAX_SIZE_OF_TASKS - 1 )
				{

					i = 0;
					end_of_loop = head;				

				}

			}	

			PORTC ^= (1<<2);
		}		

		else
		{	
			retvalue = EMPTY_BUFFER; /** Becuase the buffer is already empty **/
		}


		flag = 0;
	}

	return retvalue;

}


void interrrupt_call (void)
{
	PORTD ^= 0xff;

	flag = 1;
	if ( tail < head)
	{
		for(int i = tail; i < head; i++)
			flags[i]++;
	}
	else if ( tail > head )
	{
		uint8_t end_of_loop = MAX_SIZE_OF_TASKS;

		for(int i = tail; i < end_of_loop; i++)
		{
			flags[i]++;
			if( i == MAX_SIZE_OF_TASKS - 1 )
			{
				i = 0;
				end_of_loop = head;

			}
		}
	}
	else if ( (tail == head) && (full_flag == 1) )
	{
		uint8_t end_of_loop = MAX_SIZE_OF_TASKS;

		for(int i = tail; i < end_of_loop; i++)
		{
			PORTC ^= (1<<3);
			flags[i]++;
			if( i == MAX_SIZE_OF_TASKS - 1 )
			{
				i = 0;
				end_of_loop = head;

			}
		}
		PORTC ^= (1<<3);
	}
	else
	{
		/* Do nothing because the buffer is empty */
	}

}



