extern "C"{
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/power.h>
}



//uint8_t sin_curve[] = { 0, 2, 4, 6, 8, 10, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 30, 32, 34, 35, 37, 39, 40, 42, 43, 45, 46, 48, 49, 50, 51, 52, 54, 55, 56, 57, 57, 58, 59, 60, 60, 61, 61, 62, 62, 63, 63, 63, 63, 63, 64, 63, 63, 63, 63, 63, 62, 62, 61, 61, 60, 60, 59, 58, 57, 57, 56, 55, 54, 52, 51, 50, 49, 48, 46, 45, 43, 42, 40, 39, 37, 35, 34, 32, 30, 29, 27, 25, 23, 21, 19, 17, 15, 13, 11, 10, 8, 6, 4, 2, 0};
uint8_t sin_curve[] = { 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 6, 7, 8, 10, 11, 13, 14, 16, 18, 20, 22, 24, 26, 27, 29, 32, 34, 36, 37, 39, 41, 43, 45, 47, 49, 50, 52, 53, 55, 56, 57, 59, 60, 60, 61, 62, 62, 63, 63, 63, 64, 63, 63, 63, 62, 62, 61, 60, 60, 59, 57, 56, 55, 53, 52, 50, 49, 47, 45, 43, 41, 39, 37, 36, 34, 32, 29, 27, 26, 24, 22, 20, 18, 16, 14, 13, 11, 10, 8, 7, 6, 4, 3, 3, 2, 1, 1, 0, 0, 0, 0};
//uint8_t sin_curve[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 7, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 16, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 14, 13, 13, 13, 12, 12, 11, 11, 10, 10, 9, 9, 9, 8, 8, 7, 6, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0};
uint8_t led_colors[] = {0, (1<<0), (1<<1), (1<<4), (1<<1) | (1<<4)};
volatile uint8_t current_color;//Changed by interrupt
volatile bool interrupt_has_occurred;


void softPWM(uint8_t duty, uint8_t mask){
    PORTB |= mask;
    for (int i = 0; i < duty; ++i)
    {
        asm volatile("nop");
        //_delay_us(150);
    }
    PORTB &= ~mask;
    for (int i = duty; i < 64; ++i)
    {
        asm volatile("nop");
        //_delay_us(150);
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
    //Normal clock mode. Clock /= 8
    CLKPR = (1<<CLKPCE);
    CLKPR = (1<<CLKPS0)||(1<<CLKPS1);
    bool button_state = PINB & (1<<3); //State of button
    if (button_state == false)//Button has been pushed
    {
        current_color++;
        if(current_color == 5) current_color = 0;
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
    
    //Super low power time
    power_adc_disable();
    power_timer1_disable();
    power_usi_disable();


    for(;;){
        interrupt_has_occurred = false;
        pwmRamp(led_colors[current_color]);
        PORTB &= ~((1<<0)|(1<<1)|(1<<4));//Turn off all LEDs, just in case
        //Super slow mode. Clock /= 256
        CLKPR = (1<<CLKPCE);
        CLKPR = (1<<CLKPS3);//Super slow mode
        for (int i = 5000/32; i != 0; i--)
        {
            _delay_ms(1);
            if (interrupt_has_occurred) break;
        }
        //Normal clock mode. Clock /= 8
        CLKPR = (1<<CLKPCE);
        CLKPR = (1<<CLKPS0)||(1<<CLKPS1);

    }

    return 0;
}