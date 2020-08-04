/* INCLUDES ******************************************************************/
#include "ses_common.h"
#include "ses_button.h"
#include "defenitions.h"
#include "ses_pwm.h"
#include "ses_led.h"
#include "ses_uart.h"
#include "ses_timer.h"
#include "../../task_6/src/alarmclock.h "

// BUTTON wiring on SES board
#define BUTTON_PORT PINB  // PINB register to read the status of register PORTB

#define BUTTON_ROTARY_ENCODER_PIN   6
#define BUTTON_JOYSTICK_PIN         7

#define BUTTON_NUM_DEBOUNCE_CHECKS      5           // around 30 Ms delay
#define BUTTON_DEBOUNCE_PERIOD          500
#define BUTTON_DEBOUNCE_POS_JOYSTICK    0x01
#define BUTTON_DEBOUNCE_POS_ROTARY      0x02

bool flagDebounceOption = false;           // flag used by the ISR to check if the debouncing option is chosen by the user
volatile bool joyStickPress = false;

uint8_t parameter1 = 0;
uint8_t parameter2 = 0;

pButtonCallback callback_rot = &generate_rotary_event;
pButtonCallback callback_joy  = &generate_joystick_event; 

// pButtonCallback_2 pwm_callback;

uint16_t ctr = 0;

void button_init(bool debouncing_option){
    CLEAR_TWO_BITS  (DDR_REGISTER(BUTTON_PORT), BUTTON_ROTARY_ENCODER_PIN, BUTTON_JOYSTICK_PIN);    // set buttons to input
    SET_TWO_BITS    (PIN_REGISTER(BUTTON_PORT), BUTTON_ROTARY_ENCODER_PIN, BUTTON_JOYSTICK_PIN);    // activate pull up option
    SET_TWO_BITS    (PCMSK0, PCINT7, PCINT6);               // enable interrupt masking for the flag bits 7 and 6
    SET_BIT         (PCICR, PCIE0);

    if(debouncing_option) {
        fprintf(uartout, "init \n");
        flagDebounceOption = debouncing_option;           // enable global debouncing flag for ISR usage
        timer1_start();                                     // timer1 used for debouncing
    }
    else {
        SET_TWO_BITS    (PCMSK0, PCINT7, PCINT6);               // enable interrupt masking for the flag bits 7 and 6
        SET_BIT         (PCICR, PCIE0);                         // enable interrupt on pins PORTB6 & PORTB7
    }
}

bool button_isJoystickPressed(void)
{   
    return !(GET_BIT(BUTTON_PORT,BUTTON_JOYSTICK_PIN)); 
}

bool button_isRotaryPressed(void)
{
    return !(GET_BIT(BUTTON_PORT,BUTTON_ROTARY_ENCODER_PIN)); 
}

// void button_setJoystickButtonCallback_2(pButtonCallback_2 callback_local, uint8_t par1, uint8_t par2)
// {
//     pwm_callback = callback_local;     // executes the method in pointer to function passed by the ISR
//     parameter1 = par1;
//     parameter2 = par2;
// }

void button_setRotaryButtonCallback(pButtonCallback callback_local)
{
    callback_rot = callback_local;     // executes the method in pointer to function passed by the ISR
}

void button_setJoystickButtonCallback(pButtonCallback callback_local)
{
    callback_joy = callback_local;     // executes the method in pointer to function passed by the ISR
}

void button_checkState(void) 
{
    static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = {};  // array of unsigned char (0, 255) each element
    static uint8_t index = 0;
    static uint8_t debouncedState = 0;                      // initial debouncing state
    ctr += 1;

    state[index] = 0;                                       // each bit in every state byte represents one button
    if(button_isJoystickPressed()) 
    {
        state[index] |= BUTTON_DEBOUNCE_POS_JOYSTICK;       // example 00000000 |= 0x01
    }
    if(button_isRotaryPressed()) 
    {
        state[index] |= BUTTON_DEBOUNCE_POS_ROTARY;         // example 00000000 |= 0x02
    }
    index++;
    if (index == BUTTON_NUM_DEBOUNCE_CHECKS)                // performs the check on the button several times according to choice                   
    {
        index = 0;
    }
    uint8_t j = 0xFF;
    for(uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++) 
    {
        j = j & state[i];                   // example: 11111111 & 00000011 = 00000011 & 0x01 = 00000001 
    }
    debouncedState = j;                     // outputs only 1 for the button pressed full num debounce check times
    
    
    if(GET_BIT(debouncedState, 0) && ctr >= BUTTON_DEBOUNCE_PERIOD){
        ctr = 0;
        (*callback_joy)();
        
    }
    if(GET_BIT(debouncedState, 1) && ctr >= BUTTON_DEBOUNCE_PERIOD){
        ctr = 0;
        (*callback_rot)();   
    }
}

ISR(PCINT0_vect)
{
    // if (flagDebounceOption && ctr >= BUTTON_DEBOUNCE_PERIOD) {  // checks the debouncing flag chosen by user
    //     if(callback_joy != NULL){
    //         (*callback_joy) ();
    //     }
    //     if(callback_rot != NULL) { 
    //         (*callback_rot) ();
    //     }
    //     ctr = 0;
    // }
    // else if(GET_BIT(PCMSK0, BUTTON_ROTARY_ENCODER_PIN) && GET_BIT(PCMSK0, BUTTON_JOYSTICK_PIN)) {     // check interrupt masking bits
    //     if(GET_BIT(BUTTON_PORT, PCINT6) == LOW)              
    //     {
    //         SET_BIT(PORTB, PCINT6);                     // Reset flags for interrupt manually
    //         button_setRotaryButtonCallback(callback);   // callback method called with pointer to function LED_Toggle
    //     }
    //     if(GET_BIT(BUTTON_PORT, PCINT7) == LOW)             
    //     {
    //         SET_BIT(PORTB, PCINT7);                     // Reset flags for interrupt manually
    //         button_setJoystickButtonCallback(callback); // callback method called with pointer to function LED_Toggle    
    //     } 
    // }                   
}