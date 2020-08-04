#include <avr/io.h>
#include <util/delay.h>
#include "../../lib/ses/ses_adc.h"
#include "../../lib/ses/defenitions.h"
#include "../../lib/ses/ses_lcd.h"
#include "../../lib/ses/ses_uart.h"

#define ZERO_KELVIN 273.15      // convert to Kelvin degree

int main (void)
{
    uart_init(57600);           // Initialise UART-Baud rate
    adc_init();                 // Initialise ADC

    while(1)
    {   
        fprintf(uartout, "Temperature in celusis is %d - ADC Value is %d \n", adc_getTemperature(), ADC);
        fprintf(uartout, "light adc value is %d \n", adc_getLight());
        _delay_ms(2500);        // Print readings every 2.5 seconds
    }
    return 0;
}