#include "ses_pwm.h"
#include "ses_timer.h"
#include "defenitions.h" 
#include "ses_uart.h"
#include <avr/io.h>

#define MOTOR_PORT      PORTG
#define MOTOR_PIN       5 

void pwm_init(void){
    
    // Set Motor pin as output
    SET_BIT(DDR_REGISTER(MOTOR_PORT), MOTOR_PIN);   

    // Enable Timer0
    CLEAR_BIT(PRR0,PRTIM0); 
    TCNT0 = STOP;       

    // Select Fast PWM mode with TOP = 0xFF (WGM02:00 = 0x03)
    SET_BIT(TCCR0A,WGM00);      
    SET_BIT(TCCR0A,WGM01);
    CLEAR_BIT(TCCR0B,WGM02);        

    // Disable timer0 prescaler
    SET_BIT(TCCR0B,CS00);
    CLEAR_BIT(TCCR0B,CS01);
    CLEAR_BIT(TCCR0B,CS02);

    // Set the OC0B pin when the counter reaches the value of OCR0B / Compare Match (Inverting Mode)
    SET_BIT(TCCR0A,COM0B0);
    SET_BIT(TCCR0A,COM0B1);

    // Initially stop the motor
    pwm_setDutyCycle(STOP);
}

void pwm_setDutyCycle(uint8_t dutyCycle){
    OCR0B = dutyCycle;                      // Set Motor PWM 
}