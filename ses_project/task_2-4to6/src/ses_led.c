/* INCLUDES ******************************************************************/

#include "../../lib/ses/ses_common.h"
#include "../../lib/ses/ses_led.h"
#include "../../lib/ses/defenitions.h"

/* DEFINES & MACROS **********************************************************/

// LED wiring on SES board
#define LED_RED_PORT       	PORTG
#define LED_RED_PIN         	1

#define LED_YELLOW_PORT 	PORTF
#define LED_YELLOW_PIN      	7

#define LED_GREEN_PORT 		PORTF
#define LED_GREEN_PIN       	6

/* FUNCTION DEFINITION *******************************************************/

void led_redInit(void) {
    SET_BIT(DDR_REGISTER(LED_RED_PORT), LED_RED_PIN);     // 0b00000010 (set high to pin 1 - output)
    SET_BIT(PIN_REGISTER(LED_RED_PORT), LED_RED_PIN);     // Red starts high (turned off)
}

void led_redToggle(void) {
    TOGGLE_BIT(LED_RED_PORT, LED_RED_PIN);      
}

void led_redOn(void) {
    CLEAR_BIT(LED_RED_PORT, LED_RED_PIN);      
}

void led_redOff(void) {
    SET_BIT(LED_RED_PORT, LED_RED_PIN);       
}

void led_yellowInit(void) {
    SET_BIT(DDR_REGISTER(LED_YELLOW_PORT), LED_YELLOW_PIN);     // 0b10000000 (set high to pin 7 - output)
    SET_BIT(PIN_REGISTER(LED_YELLOW_PORT), LED_YELLOW_PIN);     // yellow starts high (turned off)
}

void led_yellowToggle(void) {
    TOGGLE_BIT(LED_YELLOW_PORT, LED_YELLOW_PIN);
}

void led_yellowOn(void) {
     CLEAR_BIT(LED_YELLOW_PORT, LED_YELLOW_PIN);   
}
void led_yellowOff(void) {
   SET_BIT(LED_YELLOW_PORT, LED_YELLOW_PIN);            
}

void led_greenInit(void) {
    SET_BIT(DDR_REGISTER(LED_GREEN_PORT), LED_GREEN_PIN);     // 0b01000000 (set high to pin 6 - output)
    SET_BIT(PIN_REGISTER(LED_GREEN_PORT), LED_GREEN_PIN);     // green starts high (turned off)
}

void led_greenToggle(void) {
    TOGGLE_BIT(LED_GREEN_PORT, LED_GREEN_PIN);
}

void led_greenOn(void) {
    CLEAR_BIT(LED_GREEN_PORT, LED_GREEN_PIN);
}

void led_greenOff(void) {
    SET_BIT(LED_GREEN_PORT, LED_GREEN_PIN);
    
}