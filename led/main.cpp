extern "C"{
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <avr/interrupt.h>
}



//uint8_t sin_curve[] = { 0, 2, 4, 6, 8, 10, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 30, 32, 34, 35, 37, 39, 40, 42, 43, 45, 46, 48, 49, 50, 51, 52, 54, 55, 56, 57, 57, 58, 59, 60, 60, 61, 61, 62, 62, 63, 63, 63, 63, 63, 64, 63, 63, 63, 63, 63, 62, 62, 61, 61, 60, 60, 59, 58, 57, 57, 56, 55, 54, 52, 51, 50, 49, 48, 46, 45, 43, 42, 40, 39, 37, 35, 34, 32, 30, 29, 27, 25, 23, 21, 19, 17, 15, 13, 11, 10, 8, 6, 4, 2, 0};
uint8_t sin_curve[] = { 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 6, 7, 8, 10, 11, 13, 14, 16, 18, 20, 22, 24, 26, 27, 29, 32, 34, 36, 37, 39, 41, 43, 45, 47, 49, 50, 52, 53, 55, 56, 57, 59, 60, 60, 61, 62, 62, 63, 63, 63, 64, 63, 63, 63, 62, 62, 61, 60, 60, 59, 57, 56, 55, 53, 52, 50, 49, 47, 45, 43, 41, 39, 37, 36, 34, 32, 29, 27, 26, 24, 22, 20, 18, 16, 14, 13, 11, 10, 8, 7, 6, 4, 3, 3, 2, 1, 1, 0, 0, 0, 0};
uint8_t led_colors[] = {0, (1<<0), (1<<1), (1<<4), (1<<1) | (1<<4)};
volatile uint8_t current_color;//Changed by interrupt
volatile bool interrupt_has_occurred;


void softPWM(uint8_t duty, uint8_t mask){
    for (int i = 0; i < duty; ++i)
    {
        PORTB |= mask;
        _delay_us(150);
    }
    for (int i = duty; i < 64; ++i)
    {
        PORTB &= ~mask;
        _delay_us(150);
    }
}

void pwmRamp(uint8_t mask){

    for (uint8_t i = 0; i < sizeof(sin_curve); ++i)
    {
        if(interrupt_has_occurred) return;
        softPWM(sin_curve[i], mask);
    }
    
}

SIGNAL(SIG_PIN_CHANGE){
    bool button_state = PINB & (1<<3); //State of button
    if (button_state == false)//Button has been pushed
    {
        current_color++;
        current_color = current_color % 5;//We only have 5 colors
        interrupt_has_occurred = true;
        _delay_ms(50);
    }
}

int main(void)
{
    current_color = 0;

    DDRB = (1<<0) | (1<<1) | (1<<4);//For LEDs
    PORTB = (1<<3);//Pullup resistor for button
    _delay_ms(1);//Wait for pullup to work

    GIMSK |= (1<<PCIE);//Enable pin interrupts
    PCMSK |= (1<<PCINT3);//Allow an interrupt from the button
    sei();//Enable interrupts
    


    for(;;){
        interrupt_has_occurred = false;
        pwmRamp(led_colors[current_color]);
        PORTB &= ~((1<<0)|(1<<1)|(1<<4));//Turn off all LEDs, just in case
        for (int i = 5000; i != 0; i--)
        {
            _delay_ms(1);
            if (interrupt_has_occurred) break;
        }

    }

    return 0;
}