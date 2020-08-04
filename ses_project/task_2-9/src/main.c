#include <avr/io.h>
#include "../../lib/ses/defenitions.h"
#include "../../lib/ses/ses_lcd.h"
#include "../../lib/ses/ses_led.h"
#include "../../lib/ses/ses_button.h"
#include "../../lib/ses/ses_adc.h"
#include "../../lib/ses/ses_uart.h"
#include <util/delay.h>
#include <stdint.h>

#define BAUDRATE 57600
/**
* Print the time since boot on the LCD
* @timeBoot Time since boot in seconds
*/
void printTime(uint16_t timeBoot) {
lcd_clear();
// first row
lcd_setCursor(0,0);
fprintf(lcdout,"Time since reset:");
// second row
lcd_setCursor(0,1);
fprintf(lcdout,"%4u seconds",timeBoot);
}
/**
* Write the time since boot to the UART
* @timeBoot Time since boot in seconds
*/
void sendTime(uint16_t timeBoot) {
fprintf(uartout,"%4u seconds since reset.\n",timeBoot);
}
/**
* main function
*/
int main (void) {
led_redInit();
led_greenInit();
led_yellowInit();
lcd_init();
uart_init(BAUDRATE);
uint16_t timeBoot = 0; // time since boot in seconds
// set all LED outputs to a low level
led_redOn(); // pin PG1
led_yellowOn(); // pin PF7
led_greenOn(); // pin PF6
// superloop
while (1) {
// 1 second delay
led_redOff();
_delay_ms(1000);
timeBoot++;
led_redOn();
// print on LCD
led_yellowOff();
printTime(timeBoot);
led_yellowOn();
// write to UART
led_greenOff();
sendTime(timeBoot);
led_greenOn();
}
return 0;
}