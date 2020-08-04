#include "../../lib/ses/ses_button.h"
#include "../../lib/ses/ses_led.h"
#include "../../lib/ses/ses_lcd.h"
#include "../../lib/ses/ses_uart.h"
#include "../../lib/ses/ses_timer.h"
#include "../../lib/ses/ses_scheduler.h"
#include "../../lib/ses/ses_rotary.h"
#include <avr/io.h>
#include "alarmclock.h"

volatile bool isSchedulerEmpty = true;

// For the Challenge task please uncomment the main method 2 and comment main method 1
// Main method 1
int main (void){
    
    Fsm ourFsm;
    uart_init(BAUD_RATE);  
    lcd_init();
    button_setJoystickButtonCallback(&generate_joystick_event);
    button_setRotaryButtonCallback(&generate_rotary_event);
    button_init(true);
    clock_init();
    led_yellowInit();
    led_redInit();
    led_greenInit();
    scheduler_init();
    fsm_init(&ourFsm,&inputHours);
    sei();

    while (1){   
        if(!isSchedulerEmpty){
            scheduler_run();        // Run Scheduler
        }
    }
    
    return 0;
}


/*
// Main method 2
    int main (void){
        uart_init(BAUD_RATE);  
        lcd_init();
        rotary_setClockwiseCallback(&rotaryIncrement);
        rotary_setCounterClockwiseCallback(&rotaryDecrement);
        rotary_init();
        sei();
        while (1)
        {
        }
        
        return 0;
}
*/


