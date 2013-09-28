#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/wdt.h>


#define BODS    6
#define BODSE   5


// int main(void){
// 	DDRD  = 0b11110111;
// 	DDRC  = 0b00000000;
// 	PINC  = 0b11111111;

// 	//				Green		Blue		Purple		Red		
// 	int colors[] = {0b00000010, 0b00000001, 0b00000101, 0b00000100};
// 	int i = 0;

// 	PORTD = colors[0];
// 	while(1){
// 		if (!(PINC & 0x04)){
// 			PORTD = colors[i];
// 			_delay_ms(200);
// 			if (i<4)
// 				i++;
// 			else
// 				i=0;
// 		}
// 	}
//     return(0);
// }

// watchdog interrupt
ISR (WDT_vect) {
   wdt_disable();
}

void loop(){
	
	// disable ADC
	ADCSRA = 0;  

	// clear various "reset" flags
	MCUSR = 0;     
	// allow changes, disable reset
	WDTCSR = _BV (WDCE) | _BV (WDE);
	// set interrupt mode and an interval 
	WDTCSR = _BV (WDIE) | _BV (WDP3) | _BV (WDP0);    // set WDIE, and 8 seconds delay
	wdt_reset();  // pat the dog
	
	set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
	sleep_enable();
	
	// turn off brown-out enable in software
	MCUCR = _BV (BODS) | _BV (BODSE);
	MCUCR = _BV (BODS); 
	sleep_cpu ();  
	
	// cancel sleep as a precaution
	sleep_disable();
}