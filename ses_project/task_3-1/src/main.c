#include <avr/io.h>
#include <util/delay.h>
#include "../../lib/ses/ses_timer.h"
#include "../../lib/ses/ses_button.h"
#include "../../lib/ses/ses_led.h"
#include "../../lib/ses/ses_uart.h"
#include <avr/interrupt.h>

int main (void)
{   
    uart_init(BAUD_RATE);
    led_redInit();
    led_yellowInit();
    led_greenInit();
    button_init(1);
    sei();

    while(1)
    {

    }
    return 0;
}