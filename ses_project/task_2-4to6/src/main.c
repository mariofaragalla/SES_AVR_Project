#include "../../lib/ses/ses_led.h"
#include "../../lib/ses/ses_button.h"
#include "../../lib/ses/ses_lcd.h"
#include "../../lib/ses/ses_uart.h"
#include "../../lib/ses/defenitions.h"
#include <util/delay.h>
#include <avr/io.h>

volatile uint16_t my_timer;             // Time interval counter - incremented in timer.c
volatile uint8_t my_timer_flag = 1;     // Global flag to start calculating time interval 
uint16_t output = 0;                    // variable to store timer value before reseting 

int main (void)
{   
    uint8_t print_flag = 0;

    uart_init(57600);       // initialise UART
    led_redInit();          // initialise LEDs
    led_yellowInit();
    led_greenInit();
    button_init();          // initialise Buttons
    timer_init();           // initialise Timer for measuring time intervals

    while (1)
    {   
        while(button_isJoystickPressed())
        {   
            my_timer_flag = 1;                  // if button is pressed counter starts counting to calculate time interval
            led_greenOn();
            print_flag = 1;                     // Flag to print only after the button is pressed and not every loop
        }
        led_greenOff();        
        output = my_timer/1000;                 // print time interval in seconds
        reset_Timer();                          // set timer to 0
        
        while(button_isRotaryPressed())
        {
            my_timer_flag = 1;                  // if button is pressed counter starts counting to calculate time interval          
            led_redOn();                        
            print_flag = 1;                     // Flag to print only after the button is pressed and not every loop
        }                   
        led_redOff();                   
        output = my_timer/1000;                 // print time interval in seconds   
        reset_Timer();                          // set timer to 0

        if(print_flag == 1)
        {
            print();
            print_flag = 0;
        }
    } 
}

void reset_Timer(void)
{
    my_timer = 0;
    my_timer_flag = 0;
}

void print(void)
{
    fprintf(uartout, "Timer is %d \n", output);
    fprintf(lcdout, "Timer is %d \n", output);
}