/*
 * stop_watch.c
 *
 *  Created on: Sep 15, 2023
 *      Author: shredan abdullah
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned char SEC1 =0;
unsigned char SEC2 =0;
unsigned char MIN1 =0;
unsigned char MIN2 =0;
unsigned char HOUR1 =0;
unsigned char HOUR2 =0;
unsigned char flag =0;

void initSevenSegment(void){
	// determine the direction of the first 4 bits in port c as output
	DDRC |= (1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3);
	// initialize it with zero
	PORTC &=~ (1<<PC0)&~(1<<PC1)&~(1<<PC2)&~(1<<PC3);
	//initialize the direction of the 6 bits in port A as output
	DDRA |= (1<<PA0)|(1<<PA1)|(1<<PA2)|(1<<PA3)|(1<<PA4)|(1<<PA5);
	//initialize it with zero
	PORTA &=~ (1<<PA0)&~(1<<PA1)&~(1<<PA2)&~(1<<PA3)&~(1<<PA4)&~(1<<PA5);
}
void initTimer1_compareMode(void){
	//Set timer1 initial count to zero
		/* Set FOC1A for non-PWM mode
		 * Set WGM12 for CTC mode
		 * Choosing prescaler = 1024 by setting CS10 & CS12
		 * Set compare value to 1000 (1 second) in OCR1A
		 * Match interrupt enable
		 */
	TCCR1A = (1<<FOC1A);
	TCCR1B = (1<<CS10)|(1<<CS12)|(1<<WGM12);
	TCNT1 = 0;
	OCR1A = 1000;
	TIMSK |= (1<<OCIE1A);
	SREG  |= (1<<7);
}
void initInterrupt0_1_2(void){
	//make the bins in the ports as inputs
	DDRD &=~(1<<PD2)&~(1<<PD3);
	DDRB &=~(1<<PB2);
	//make the interrupt 0 and 2 internal pull up
	PORTB |= (1<<PB2);
	PORTD |= (1<<PD2);
	//make interrupt 1 raising edge and interrupt 0 falling edge
	MCUCR |=(1<<ISC01)|(1<<ISC11)|(1<<ISC10);
	//make interrupt 2 falling edge
	MCUCSR &=~(1<<ISC2);
	//enable the three interrupts
	GICR  |=(1<<7)|(1<<6)|(1<<5);
	//enable the Global interrupt bit
	SREG  |= (1<<7);
}
ISR(INT0_vect){
	//reset the stop watch
	 SEC1 =0;SEC2 =0;MIN1 =0;MIN2 =0;HOUR1 =0;HOUR2 =0;
	 //reset the timer
	 TCNT1 = 0;
}
ISR(INT1_vect){
	//Pause stop watch
	TCCR1B &=~(1<<CS10) &~(1<<CS12)	; //Counter is stopped
}
ISR(INT2_vect){
	//resume counting
	TCCR1B |= (1<<CS10) |(1<<CS12);  //selecting clock source with prescaler = 1024
}
ISR(TIMER1_COMPA_vect){
	flag=1;
}


int main(void){
	    //initializations
		//seven segment
		initSevenSegment();
		//timer1
		initTimer1_compareMode();
		//interrupts
		initInterrupt0_1_2();

	while(1){
		/*if the flag = 1 this means that the counter counted 1 second */
				if(flag==1)
				{
					//increment 7-seg devices
								SEC1++;
								if(SEC1 ==10)
								{
									SEC1=0;
									SEC2++;
									_delay_ms(5);
								}
								if(SEC2 ==6)
								{
									SEC2=0;
									MIN1++;
									_delay_ms(5);
								}
								if(MIN1 == 10)
								{
									MIN1=0;
									MIN2++;
									_delay_ms(5);
								}
								if(MIN2 == 6)
								{
									MIN2=0;
									HOUR1++;
									_delay_ms(5);
								}
								if(HOUR1 ==10)
								{
									HOUR1=0;
									HOUR2++;
									_delay_ms(5);
								}
								if(HOUR2 ==10)
								{
									HOUR2=0;
									_delay_ms(5);
								}
								flag=0;
				}
				else
				{
					PORTA =0x01;
					PORTC = (PORTC & 0xF0) | (SEC1 & 0x0F);
					_delay_ms(1);
					PORTA =0x02;
					PORTC = (PORTC & 0xF0) | (SEC2 & 0x0F);
					_delay_ms(1);
					PORTA =0x04;
					PORTC = (PORTC & 0xF0) | (MIN1 & 0x0F);
					_delay_ms(1);
					PORTA =0x08;
					PORTC = (PORTC & 0xF0) | (MIN2 & 0x0F);
					_delay_ms(1);
					PORTA =0x10;
					PORTC = (PORTC & 0xF0) | (HOUR1 & 0x0F);
					_delay_ms(1);
					PORTA =0x20;
					PORTC = (PORTC & 0xF0) | (HOUR2 & 0x0F);
					_delay_ms(1);
				}
			}
	return 0;
}
