/*
 * main.c
 *
 *      Author: Aya
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

typedef unsigned char uint8;

uint8 i ,Flag , Time[6];

#define SET_BIT(REG,VAL_PIN)    REG |= (1 << VAL_PIN)
#define CLEAR_BIT(REG,VAL_PIN)  REG &= ~(1 << VAL_PIN)

void Timer1_Init(void);
void INT0_Init(void);
void INT1_Init(void);
void INT2_Init(void);
void Stop_Watch(void);

ISR(TIMER1_COMPA_vect){
	Flag = 1;
}

ISR(INT0_vect){
	PORTC &= 0xF0; //7-segment displays 0 as initial value

	for(i = 0; i < 6 ; i++)
		Time[i] = 0;
}

ISR(INT1_vect){
	CLEAR_BIT(TIMSK,OCIE1A);
}

ISR(INT2_vect){
	SET_BIT(TIMSK,OCIE1A);
}

void Timer1_Init(void){
	TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);
	TCNT1 = 0;
	OCR1A = 977;
	SET_BIT(TIMSK,OCIE1A);
	SET_BIT(SREG,7);
	}

void INT0_Init(void){
	CLEAR_BIT(DDRD,2);//D2 --> InPut Pin
	SET_BIT(MCUCR,ISC01);
	SET_BIT(GICR,INT0);
	SET_BIT(SREG,7);
}

void INT1_Init(void){
	CLEAR_BIT(DDRD,3);//D3 --> InPut Pin
	SET_BIT(MCUCR,ISC10);
	SET_BIT(MCUCR,ISC11);
	SET_BIT(GICR,INT1);
	SET_BIT(SREG,7);
}

void INT2_Init(void){
	CLEAR_BIT(DDRB,2); //B2 --> InPut Pin
	CLEAR_BIT(MCUCSR,ISC2);
	SET_BIT(GICR,INT2);
	SET_BIT(SREG,7);
}

void Stop_Watch(void){

	Time[0]++;

	if(Time[0] > 9){
		Time[1]++;
		Time[0] = 0;

		if(Time[1] == 6){
			Time[2]++;
			Time[1] = 0;
		}
	}

	if(Time[2] > 9){
		Time[3]++;
		Time[2] = 0;

		if(Time[3] == 6){
			Time[4]++;
			Time[3] = 0;
		}
	}

	if(Time[4] > 9){
		Time[5]++;
		Time[4] = 0;

		if(Time[5] > 9){
			for(i = 0; i < 6 ; i++)
				Time[i] = 0;
		}
	}
}



int main (void){

		/*Define pins In/Out Pins*/

	DDRA |= 0x3F; //OutPut Pins A0 --> A5
	DDRC |= 0x0F ; //OutPut Pins C0 --> C3

		/*Initialization Values*/

	PORTC &= 0xF0; //7-segment displays 0 as initial value
	PORTA |=0x3F;
	Flag = 0;
	Timer1_Init();
	INT0_Init();
	INT1_Init();
	INT2_Init();

		/*Active Internal Pull up Resistor*/

	SET_BIT(PORTB,2);
	SET_BIT(PORTD,2);

	while(1){


		for(i = 0; i < 6; i++){

			PORTA = (PORTA & 0xC0) | (1<<i);
			PORTC = (PORTC & 0xF0) | (Time[i] & 0x0F);
			_delay_ms(1);

		}

		if(Flag == 1){
			Stop_Watch();
			Flag = 0;
		}

	}

	return 0;
}
