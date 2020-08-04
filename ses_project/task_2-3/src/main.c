#include "ses_lcd.h"
#include "ses_uart.h"
#include <avr/delay.h>
#include <avr/io.h>

int main (void)
{
    uart_init(57600);           // initialise UART
    lcd_init();                 // initialise LCD

    for (int i=0; i<10; i++)
    {
        fprintf(uartout, "START %d \n", i);     // transmit and print to UART
        fprintf(lcdout, "START %d", i);         // print to LCD
        _delay_ms(500);                         // print every 0.5 seconds
    }
    return 0;
}