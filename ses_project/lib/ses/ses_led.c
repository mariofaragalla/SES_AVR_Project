/* INCLUDES ******************************************************************/

#include "ses_common.h"
#include "ses_led.h"
#include "defenitions.h"

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
    SET_BIT(DDR_REGISTER(LED_RED_PORT), LED_RED_PIN);     // set the pin connected to the red led to be output 
    led_redOff();                                         // Red starts high (turned off)
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
    SET_BIT(DDR_REGISTER(LED_YELLOW_PORT), LED_YELLOW_PIN);     // set the pin connected to the yellow led to be output 
    led_yellowOff();                                            // yellow starts high (turned off)
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
    SET_BIT(DDR_REGISTER(LED_GREEN_PORT), LED_GREEN_PIN);     // set the pin connected to the green led to be output 
    led_greenOff();                                           // green starts high (turned off)
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