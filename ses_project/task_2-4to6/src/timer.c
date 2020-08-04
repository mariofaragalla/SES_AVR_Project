#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../../lib/ses/defenitions.h"

#define High 1
#define Low  0
#define SET_DDG1    0x02        // Set bit 1 to output in DDRG
#define SET_DDF7    0x80        // Set bit 7 to output in DDRF
#define SET_PORTG1  0x02        // Set bit 1 in PORTG
#define SET_PORTF7  0x80        // Set bit 7 in PORTF
#define clear       0x00        // Empty

extern uint16_t my_timer;
extern uint8_t my_timer_flag;

uint16_t Tdesired = Low;              // Global place holder for the time to be delayed
volatile int extra_timer = Low;       // Counter to indicate that the Tdesired time is reached // volatile to be used by ISR Routine
volatile uint8_t End_Flag = Low;      // Flag that indicates that the delay is accomplished

void timer_init(void) 
{
    TIMSK0 = (High<<OCIE0A);        // Output Compare Match Interrupt Enable (to indicate when the value is reached by the timer)
    sei();                          // Interrupt Enable    
    TCCR0B = (High<<CS00) | (High<<CS01) | (Low<<CS02);     // Choose prescaler 011 (64)
    TCNT0 = clear;                  // Set the counter/Timer0 to 0x00
}

void DEL (uint16_t milli)
{
    Tdesired = milli ;          // Assign milli to global variable holder Tdesired to be accessed by the ISR

    TIMSK0 = (High<<OCIE0A);    // Output Compare Match Interrupt Enable (to indicate when the value is reached by the timer)
    sei();                      // Interrupt Enable    
    TCCR0B = (High<<CS00) | (High<<CS01) | (Low<<CS02);     // Choose prescaler 011 (64)
    TCNT0 = clear;              // Set the counter/Timer0 to 0x00
    End_Flag = Low;             // Initialize the Flag
    while(End_Flag == Low);     // Stays in the loop until the Flag is toggled by the ISR indicating that the Delay is accomplished
}

ISR (TIMER0_COMPA_vect)             // Interrupt Service Routine
{
    extra_timer ++;                 // Timer increment every 1 ms
    if (my_timer_flag == 1)
    {
        my_timer++;
    }
    if ( extra_timer == Tdesired )  // When the timer reaches the desired delay the flag is toggled and counter is reset
    {
        End_Flag = High;
        extra_timer = 0;            // Reset counter
    }
}