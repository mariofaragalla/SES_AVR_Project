#include "ses_common.h" 
#include "defenitions.h"
#include "ses_rotary.h"
#include "ses_button.h"
#include <avr/io.h>
#include "ses_uart.h"
#include "ses_timer.h"

#define A_ROTARY_PORT PORTB 
#define B_ROTARY_PORT PORTG
#define A_ROTARY_PIN 5
#define B_ROTARY_PIN 2
#define ROTARY_NUM_DEBOUNCE_CHECKS      2           // around 30 Ms delay
#define ROTARY_DEBOUNCE_PERIOD          420
#define ROTARY_DEBOUNCE_POS_A_ROTARY    0x01
#define ROTARY_DEBOUNCE_POS_B_ROTARY    0x02

// Not Used during the code
// Just for testing
pTypeRotaryCallback cwCallback = &rotaryIncrement;
pTypeRotaryCallback ccwCallback = &rotaryDecrement;

int16_t counter = 0;        // to check direction: increment if clockwise or decrement if counterclockwise
uint16_t rotary_ctr = 0;    // to check the debounce period

void rotary_init() {

    // set buttons to input
    DDR_REGISTER(A_ROTARY_PORT) &= ~(1<<A_ROTARY_PIN);  // set pin A as input
    DDR_REGISTER(B_ROTARY_PORT) &= ~(1<<B_ROTARY_PIN);  // set pin B as input
    // activate pull up option
    PIN_REGISTER(A_ROTARY_PORT) |= (1<<A_ROTARY_PIN);
    PIN_REGISTER(B_ROTARY_PORT) |= (1<<B_ROTARY_PIN);

    timer1_start();                                     // timer1 used for debouncing / calls rotary_checkstate every 5ms
    timer1_setCallback(&rotary_checkState);             // set the callback of the timer to be rotary_checkState
}

void rotary_setClockwiseCallback(pTypeRotaryCallback local_callback){ 
    cwCallback = local_callback;    // set the incoming callback to the clockwise callback
}

void rotary_setCounterClockwiseCallback(pTypeRotaryCallback local_callback){
    ccwCallback = local_callback;   // set the incoming callback to the clockwise callback
}

// Uncomment printing for testing in rotary increment and decrement
void rotaryIncrement(){
    counter++;
    // fprintf(uartout, "counter: %d\n", counter);
}

void rotaryDecrement(){
    counter--;
    // fprintf(uartout, "counter: %d\n", counter);
}

void rotary_checkState(void) {
    static bool previous_read_A = true; // initially high -> OFF state

    if((rotaryA_isPressed()) != previous_read_A ){  
        // Case 1 -> if (A-B) signal is low while (B-C) signal is high -> CW direction indicated   
        if(rotaryB_isPressed() && ccwCallback != NULL && rotary_ctr >= ROTARY_DEBOUNCE_PERIOD){
            (*ccwCallback)();
            rotary_ctr = 0;
        } 
        // Case 2 -> if (A-B) signal is low while (B-C) signal is low -> CCW direction indicated
        else if(!rotaryB_isPressed() && cwCallback != NULL && rotary_ctr >= ROTARY_DEBOUNCE_PERIOD){
            (*cwCallback)();
            rotary_ctr = 0;
        }
    }
    previous_read_A = rotaryA_isPressed();
}

bool rotaryA_isPressed(void)    // return read from A pin bit of rotary 
{   
    return !(GET_BIT(PIN_REGISTER(A_ROTARY_PORT),A_ROTARY_PIN)); 
}
bool rotaryB_isPressed(void)    // return read from B pin bit of rotary 
{   
    return !(GET_BIT(PIN_REGISTER(B_ROTARY_PORT),B_ROTARY_PIN)); 
}


void check_rotary() {
static uint8_t p = 0;
static bool sampling = false;
    bool a = PIN_REGISTER(A_ROTARY_PORT) & (1 << A_ROTARY_PIN);
    bool b = PIN_REGISTER(B_ROTARY_PORT) & (1 << B_ROTARY_PIN);
    if (a != b) {
        sampling = true;
    }

    if (sampling && p < 122) {
        lcd_setPixel((a) ? 0 : 1, p, true);
        lcd_setPixel((b) ? 4 : 5, p, true);
        p++;
    }
}
