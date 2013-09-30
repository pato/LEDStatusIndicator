/* LED Pulsate with PWM
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

int main()
{
    // Turn on LED output pin
    DDRB   |= _BV(DDB0);

    // Clear OC0A on match
    TCCR0A |= _BV(COM0A1);

    // Set timer to count with F_CPU / 64
    TCCR0B |= _BV(CS10); // F_CPU
    TCCR0B |= _BV(CS01) | _BV(CS00);  // /64 prescale

    // Use Fast PWM, OCRA TOP
    TCCR0A |= _BV(WGM00);
    TCCR0A |= _BV(WGM01);
    //TCCR0B |= _BV(WGM02);

    // Initial value for our pulse width is 0
    OCR0A = 0x00;

    uint8_t dir = 1;           // Direction 
    uint16_t div = 500;        // # of clocks per inc/decrement
    uint16_t stall = 0;        // Initial stall counter
    uint16_t stall_time = 500; // # of clocks to stall at top/bottom
    for(;;) {
        // We only want to update every div counts
        if(TCNT0 % div != 0) continue;

        // Stall at the top and bottom, and change direction
        if(OCR0A == 255 || OCR0A == 0) {
            // Switch directon at top
            if(OCR0A == 255) dir = -1;
            else 
            {   // Disable LED at bottom
                // And switch direction
                DDRB  &= ~(_BV(DDB0));
                dir = 1; 
            }
            while(stall < stall_time) 
            { 
                ++stall;
                _delay_ms(10); 
            }
            // Turn output back on if necessary
            if(OCR0A == 0) DDRB  |= _BV(DDB0);
            stall = 0;
        }
        // General case - increment direction either way
        OCR0A += dir;
    }
    return 0;
}
