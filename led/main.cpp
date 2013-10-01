extern "C"{
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
}



uint8_t sin_curve[] = { 0, 2, 4, 6, 8, 10, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 30, 32, 34, 35, 37, 39, 40, 42, 43, 45, 46, 48, 49, 50, 51, 52, 54, 55, 56, 57, 57, 58, 59, 60, 60, 61, 61, 62, 62, 63, 63, 63, 63, 63, 64, 63, 63, 63, 63, 63, 62, 62, 61, 61, 60, 60, 59, 58, 57, 57, 56, 55, 54, 52, 51, 50, 49, 48, 46, 45, 43, 42, 40, 39, 37, 35, 34, 32, 30, 29, 27, 25, 23, 21, 19, 17, 15, 13, 11, 10, 8, 6, 4, 2, 0};

//LED 1:PB1 OC0B
//LED 2:PB0 !0C1A
//LED 3:PB4 0C1B
void configureHardwareForPWM(){
    //Use full clock speed for OC0x
    TCCR0B = (1<<CS00);
    //Fast PWM on OC0A/B, top = OCRA
    TCCR0A = (1 << WGM00) | (1 << WGM01);
    TCCR0B |=  (1 << WGM02); //For using OCR0A as top
    //for PWM mode, clear OC0A and OC0B on compare match
    TCCR0A |=    (1<<COM0B1); // (1<<COM0A1)   |

    //Set TOP for timer0 to 63
    OCR0A = 64;//TOP
    OCR0B = 32;//Green (led 2) (0C0B) pulse width

    //Now for OC1A and OC1B
    // TCCR1:
    // Set CTC1. This resets Timer/Counter1 to 0 upon a match with OCR1C
    // Set PWM1A. This enables PWM on outputs 1A, with width OCR1A.
    // Set COM1A1. This enables OC1A, but not !OC1A.
    // Set CS10. This makes the clock source undivided CK (system clock)
    TCCR1 = (1<<CTC1) | (1<<PWM1A) | (1<<COM1A1) | (1<<CS10);

    // GTCCR
    // Set PWM1B. This enables PWM on outputs 1B, with width OCR1B.
    // Set COM1B1. This enables OC1B, but not !OC1B.
    GTCCR = (1<<PWM1B) | (1<<COM1B1);

    // OCR1A
    // Set to the duty cycle of OC1A
    OCR1A = 32;

    // OCR1B
    // Set the duty cycle of OC1B
    OCR1B = 32;

    // OCR1C
    // Set to TOP. Only works when CTC1 is set.
    OCR1C = 64;



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
        softPWM(sin_curve[i], mask);
    }
    
}

int main(void)
{
    const uint8_t led_mask = (1<<0) | (1<<1) | (1<<4);
    uint8_t led_colors[] = {0, (1<<0), (1<<1), (1<<4), (1<<1) | (1<<4)};
    DDRB = led_mask;
    PORTB = 0;
    /*
    PORTB |= (1<<3);//Button pullup
    
    uint8_t led_counter = 0;
    
    for(;;){
        setAll(led_colors[led_counter]) ;
        led_counter++;
        led_counter = led_counter % 5;
        waitForButton();
    }
    */
    // configureHardwareForPWM();
    int counter = 0;
    for(;;){
        pwmRamp(led_colors[counter % 5]);
        counter++;
        
        // OCR1B = 0;//Red
        // OCR0B = 0;//Green
        // OCR1A = 0;//Blue
        // for(int i=0; i<64; i++){
        //  OCR1A += 1;
        //  //OCR1A += 2;
        //  //OCR1B += 3;
        //  _delay_ms(20);
        // }
        // OCR1A = 0;
        // for(int i=0; i<64; i++){
        //  OCR0B += 1;
        //  //OCR1A += 2;
        //  //OCR1B += 3;
        //  _delay_ms(20);
        // }
        // OCR0B = 0;
        // for(int i=0; i<64; i++){
        //  OCR1B += 1;
        //  //OCR1A += 2;
        //  //OCR1B += 3;
        //  _delay_ms(20);
        // }
        // OCR1B = 0;

    }

    return 0;
}