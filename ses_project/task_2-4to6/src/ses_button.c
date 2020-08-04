/* INCLUDES ******************************************************************/
#include "../../lib/ses/ses_common.h"
#include "../../lib/ses/ses_button.h"
#include "../../lib/ses/defenitions.h"

// BUTTON wiring on SES board
#define BUTTON_PORT       	        PINB            // PINB register to read the status of register PORTB

#define BUTTON_ROTARY_ENCODER_PIN   6
#define BUTTON_JOYSTICK_PIN         7

pButtonCallback Rotary_Callback = &;
pButtonCallback Joystick_Callback;

void button_init(void){
    CLEAR_TWO_BITS  (DDR_REGISTER(BUTTON_PORT), BUTTON_ROTARY_ENCODER_PIN, BUTTON_JOYSTICK_PIN);    // set buttons to input
    SET_TWO_BITS    (PIN_REGISTER(BUTTON_PORT), BUTTON_ROTARY_ENCODER_PIN, BUTTON_JOYSTICK_PIN);    // activate pull up option
    SET_BIT(PCICR, PCIE0);                          // enable interrupt on pins PORTB6 & PORTB7
    SET_TWO_BITS(PCMSK0, PCINT7, PCINT6);           // enable interrupt masking for the flag bits 7 and 6
}

bool button_isJoystickPressed(void){
    if(GET_BIT(BUTTON_PORT,BUTTON_JOYSTICK_PIN))    // Check if Joystick button is pressed 
    {
        return false;
    } 
    return true;
}

bool button_isRotaryPressed(void)
{
    if(GET_BIT(BUTTON_PORT,BUTTON_ROTARY_ENCODER_PIN))  // Check if Rotary Encoder button is pressed
    {
        return false;
    } 
    return true;
}

ISR(PCINT0_vect){
// execute callbacks here
    if(GET_BIT(PCMSK0, BUTTON_ROTARY_ENCODER_PIN))
    {
        button_setRotaryButtonCallback( Rotary_Callback );
    }
    if(GET_BIT(PCMSK0, BUTTON_JOYSTICK_PIN))
    {

    }
}