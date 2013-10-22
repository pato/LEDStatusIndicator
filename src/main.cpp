extern "C"{
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
}


//Sine wave
//uint8_t led_pulse_curve[] = { 0, 2, 4, 6, 8, 10, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 30, 32, 34, 35, 37, 39, 40, 42, 43, 45, 46, 48, 49, 50, 51, 52, 54, 55, 56, 57, 57, 58, 59, 60, 60, 61, 61, 62, 62, 63, 63, 63, 63, 63, 64, 63, 63, 63, 63, 63, 62, 62, 61, 61, 60, 60, 59, 58, 57, 57, 56, 55, 54, 52, 51, 50, 49, 48, 46, 45, 43, 42, 40, 39, 37, 35, 34, 32, 30, 29, 27, 25, 23, 21, 19, 17, 15, 13, 11, 10, 8, 6, 4, 2, 0};
//Modified sine wave
uint8_t led_pulse_curve[] = { 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 6, 7, 8, 10, 11, 13, 14, 16, 18, 20, 22, 24, 26, 27, 29, 32, 34, 36, 37, 39, 41, 43, 45, 47, 49, 50, 52, 53, 55, 56, 57, 59, 60, 60, 61, 62, 62, 63, 63, 63, 64, 63, 63, 63, 62, 62, 61, 60, 60, 59, 57, 56, 55, 53, 52, 50, 49, 47, 45, 43, 41, 39, 37, 36, 34, 32, 29, 27, 26, 24, 22, 20, 18, 16, 14, 13, 11, 10, 8, 7, 6, 4, 3, 3, 2, 1, 1, 0, 0, 0, 0};

//On breadboard:
//Green is PB0
//Blue is PB1
//Red is PB4
//On PCB:
//Blue is PB0
//Green is PB1
//Red is PB4
//uint8_t led_colors[] = {0, (1<<0), (1<<1), (1<<0) | (1<<4), (1<<4)};//Breadboard
uint8_t led_colors[] = {0, (1<<1), (1<<0), (1<<0) | (1<<4), (1<<4)};//PCB

volatile uint8_t current_color;//Changed by interrupt
volatile bool interrupt_has_occurred;

//Does a software PWM. The LEDs you want to blink should be set to 1 in mask
//And the duty cycle is out of 64
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

//Cycle through the brightnesses specified in led_pulse_curve
void pwm_ramp(uint8_t mask){
    if(mask == 0) return;
    for (uint8_t i = 0; i < sizeof(led_pulse_curve); ++i)
    {
        if(interrupt_has_occurred) return;
        softPWM(led_pulse_curve[i], mask);
    } 
}

SIGNAL(SIG_PIN_CHANGE){
    // //Normal clock mode. Clock /= 8
    // CLKPR = (1<<CLKPCE);
    // CLKPR = (1<<CLKPS0)||(1<<CLKPS1);
    bool button_state = PINB & (1<<3); //State of button
    if (button_state == false)//Button has been pushed
    {
        _delay_ms(30);
        if(button_state == false){ //And it wasn't just bouncing
            current_color++;
            if(current_color == 5) current_color = 0;
            interrupt_has_occurred = true;
        }
    }
}

ISR(WDT_vect){
	interrupt_has_occurred = true;
}

//Triggers the WDT_vect interrupt every 4 seconds. Used to wake from sleep mode.
void enable_watchdog_interrupt(){
	//Set WDIE //Enable watchdog timer interrupt
	//Set WDP3 //Prescaler 3. 4 second time-out.

    WDTCR = (1<<WDIE) | (1<<WDP3);

}

//Sets watchdog timer back to 0
void reset_watchdog(){
	asm volatile("wdr");
}

//Sleeps the microcontroller. Wakes up on interrupt (button or watchdog overflow)
void go_to_sleep(){
    //MCUCR:
        //Set SE //Enable sleep mode
        //Set SM[1:0] to 0b10 //Choose deep sleep mode
    //Call SLEEP instruction
    MCUCR |= (1<<SE) | (1<<SM1);
    asm volatile("sleep");
}

int main(void)
{
    current_color = 0;

    DDRB = (1<<0) | (1<<1) | (1<<4); //Set LEDs to output

    PORTB = (1<<3);//Pullup resistor for button

    _delay_ms(1);//Wait for pullup to work

    GIMSK |= (1<<PCIE);//Enable pin interrupts
    PCMSK |= (1<<PCINT3);//Allow an interrupt from the button
    sei();//Enable interrupts
    
    //Super low power time
    power_all_disable(); //Disable all unused peripherals (including ADC)
    ACSR &= ~(1<<ACD);//Turn off analog comparator (not ADC)


    //Set clock frequency to watchdog/8
    CLKPR = (1<<CLKPCE);
    CLKPR = (1<<CLKPS0)||(1<<CLKPS1);

    //Every 4 seconds, the watchdog timer will overflow and trigger an interupt
    enable_watchdog_interrupt();

    for(;;){
        interrupt_has_occurred = false;
        pwm_ramp(led_colors[current_color]);
        PORTB &= ~((1<<0)|(1<<1)|(1<<4));//Turn off all LEDs, just in case

        
        reset_watchdog(); //Watchdog interrupt will be triggered in 4 seconds *from now*
        go_to_sleep();
    }

    return 0;
}