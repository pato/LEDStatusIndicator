extern "C"{
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
}

//LED 1:PB1 OC1A
//LED 2:PB0 0C0A
//LED 3:PB4 0C1B
void configureHardwareForPWM(){
	//Use full clock speed for OC0x
	TCCR0B = (1<<CS00);
	//Fast PWM on OC0A/B, top = OCRA
	TCCR0A = (1 << WGM00) | (1 << WGM01);
	TCCR0B |=  (1 << WGM02); //For using OCR0A as top
	//for PWM mode, clear OC0A and OC0B on compare match
	TCCR0A |=    (1<<COM0B1); // (1<<COM0A1)   |

	//Set TOP to 63
	OCR0A = 64;//TOP
	OCR0B = 0;//Green (led 2) (0C0B) pulse width
}

void setBlue(uint8_t brightness){
	if (brightness != 0)
	{
		PORTB |= 1<<0;
	}else{
		PORTB &= ~(1<<0);
	}
}

void setGreen(uint8_t brightness){
	if (brightness != 0)
	{
		PORTB |= 1<<1;
	}else{
		PORTB &= ~(1<<1);
	}
}

void setRed(uint8_t brightness){
	if (brightness != 0)
	{
		PORTB |= 1<<4;
	}else{
		PORTB &= ~(1<<4);
	}
}

void setAll(uint8_t led_counter){
	setRed(led_counter & (1<<0));
	setGreen(led_counter & (1<<1));
	setBlue(led_counter & (1<<2));
}

void waitForButton(){
	while(PINB & (1<<3)){}
	_delay_ms(10);
	while(!(PINB & (1<<3))){}
	_delay_ms(10);
}

int main(void)
{
	const uint8_t led_mask = (1<<0) | (1<<1) | (1<<4);
	DDRB = led_mask;
	PORTB = 0;
	/*
	PORTB |= (1<<3);//Button pullup
	
	uint8_t led_counter = 0;
	uint8_t led_colors[] = {0, 0x2, 0x4, 0x5, 0x1};
	for(;;){
		setAll(led_colors[led_counter]) ;
		led_counter++;
		led_counter = led_counter % 5;
		waitForButton();
	}
	*/
	configureHardwareForPWM();
	for(;;){
		OCR0B+= 55;
		_delay_ms(500);
	}

    return 0;
}