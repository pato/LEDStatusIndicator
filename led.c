#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>


int main(void){
	DDRD  = 0b11110111;
	DDRC  = 0b00000000;
	PINC  = 0b11111111;

	//				Green		Blue		Purple		Red		
	int colors[] = {0b00000010, 0b00000001, 0b00000101, 0b00000100};
	int i = 0;

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
    return(0);
}
