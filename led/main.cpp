extern "C"{
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
}

    
void pwm(uint8_t mask, uint8_t val){
	for (uint8_t i = 0; i < val; ++i)
	{
		PORTB |= mask;
	}
	for(uint8_t i = val; i <= 255; i++){
		PORTB &= ~mask;
	}

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
	PORTB |= (1<<3);//Button pullup
	const uint8_t led_mask = (1<<0) | (1<<1) | (1<<4);
	DDRB = led_mask;
	uint8_t led_counter = 0;
	//uint8_t led_colors[] = {0, 0x2, 0x4, 0x5, 0x1};
	const uint8_t bmask = (1<<0);
	const uint8_t gmask = (1<<1);
	const uint8_t rmask = (1<<4);
	const uint8_t led_masks[] = {0, bmask, gmask, bmask | rmask, rmask};
	for(;;){
		uint8_t mask = led_masks[led_counter];
		uint8_t pwm_val = 0;
		while(PINB & (1<<3)){
			pwm(mask, pwm_val);
			pwm_val++;
		}
		led_counter++;
		led_counter = led_counter % 5;
		waitForButton();
	}
    return 0;
}