#define F_CPU 1000000UL



#include <avr/io.h>
#include <util/delay.h>

#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT)) 
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT)) 
#define FLIPBIT(ADDRESS,BIT) (ADDRESS ^= (1<<BIT)) 
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT)) 


int main(void){
	DDRD  = 0b11110111;
	DDRC  = 0b00000000;
	PINC  = 0b11111111;

	while(0){
		_delay_ms(500);
		SETBIT(PORTD,2);
		_delay_ms(500);
		SETBIT(PORTD,3);
		_delay_ms(500);
	}

	int i = 0;
	int colors[] = {0b00000010, 0b00000001, 0b00000101, 0b00000100};
	PORTD = colors[0];
	while(1){
		if (!(PINC & 0x04)){
			PORTD = colors[i];
			_delay_ms(200);
			if (i<4)
				i++;
			else
				i=0;
		}
	}

	while (0){
		//blue
		PORTD = 0b00000001;

		_delay_ms(500);
		//green
		PORTD = 0b00000010;

		_delay_ms(500);
		//red
		PORTD = 0b00000100;

		_delay_ms(500);

		PORTD = 0b00000101;
		
		_delay_ms(500);

	}



    return(0);

}
