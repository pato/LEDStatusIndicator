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

const uint8_t green = 1 << 0;
const uint8_t blue = 1 << 1;
const uint8_t red = 1 << 2;
const uint8_t sw1 = 1 << 3;
const uint8_t sw2 = 1 << 4;
//All the possible LED colors. 0-3 are accessed with one button, and 4 is accessed with the other button.
const uint8_t led_colors[] = {0, green, blue, red};//PCB

volatile uint8_t current_color; //Changed by interrupt
volatile bool interrupt_has_occurred; //Set to true inside any interrupts

//Does a software PWM. The LEDs you want to blink should be set to 1 in mask
//And the duty cycle is out of 64
void softPWM(const uint8_t duty, const uint8_t mask){
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
void pwm_ramp(const uint8_t mask){
    if(mask == 0) return;
    for (uint8_t i = 0; i < sizeof(led_pulse_curve); ++i)
    {
        if(interrupt_has_occurred) return;
        softPWM(led_pulse_curve[i], mask);
    } 
}

//Triggers the WDT_vect interrupt every 2 seconds. Used to wake from sleep mode.
void enable_watchdog_interrupt(){
	//Set WDIE //Enable watchdog timer interrupt
	//For the prescaler:
    //WDP3 = 4 second timeout
    //WDP0,1,2 = 2 second timeout
    
    WDTCR = (1<<WDIE) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);
    
}

//Sets watchdog timer back to 0
void reset_watchdog(){
	asm volatile("wdr");
}

SIGNAL(SIG_PIN_CHANGE){
    // //Normal clock mode. Clock /= 8
    // CLKPR = (1<<CLKPCE);
    // CLKPR = (1<<CLKPS0)||(1<<CLKPS1);
    bool button1 = PINB & sw1; //State of button
    bool button2 = PINB & sw2;
    if (button1 == false)//Button 1 has been pushed. Goes to red mode
    {
        _delay_ms(20);
        button1 = PINB & sw1;
        if(button1 == false){ //And it wasn't just bouncing
            reset_watchdog();
            current_color = 3; //Turn on the red
            interrupt_has_occurred = true;
        }
    }
    else if (button2 == false)//Button 2 has been pushed. Cycle through non-red modes.
    {
        _delay_ms(20);
        button2 = PINB & sw2;
        if(button2 == false){ //And it wasn't just bouncing
            reset_watchdog();
            current_color++;
            if(current_color >= 3) current_color = 0;
            interrupt_has_occurred = true;
        }
    }
    
}

ISR(WDT_vect){
	interrupt_has_occurred = true;
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

    DDRB = red | green | blue; //Set LEDs to output

    PORTB = sw1 | sw2;//Pullup resistor for buttons

    _delay_ms(1);//Wait for pullup to work

    GIMSK |= (1<<PCIE);//Enable pin interrupts
    PCMSK |= (1<<PCINT3) | (1<<PCINT4);//Allow an interrupt from the buttons (pins 3 and 4)
    sei();//Enable interrupts
    
    //Super low power time
    power_all_disable(); //Disable all unused peripherals (including ADC)
    ACSR &= ~(1<<ACD);//Turn off analog comparator (not ADC)


    //Set clock frequency to watchdog/8
    CLKPR = (1<<CLKPCE);
    CLKPR = (1<<CLKPS0)||(1<<CLKPS1);

    //Every 2 seconds, the watchdog timer will overflow and trigger an interupt
    enable_watchdog_interrupt();

    for(;;){
        interrupt_has_occurred = false;
        pwm_ramp(led_colors[current_color]);
        PORTB &= ~(red|green|blue);//Turn off all LEDs, just in case

        reset_watchdog(); //Watchdog interrupt will be triggered in 2 seconds *from now*
        //If an interrupt occurred since we set this to false, it means someone has pushed
        // a button and we should go to the next color.
        if(!interrupt_has_occurred){
            go_to_sleep();
        }
    }

    return 0;
}