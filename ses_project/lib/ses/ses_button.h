#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_

/* INCLUDES ******************************************************************/

#include "ses_common.h"

typedef void (*pButtonCallback_2)(uint8_t, uint8_t);
typedef void (*pButtonCallback)(void);                                          // pointer to function

void button_setRotaryButtonCallback(pButtonCallback callback);              // callback method for rotary button
void button_setJoystickButtonCallback(pButtonCallback callback);            // callback method for joystick button
void button_setJoystickButtonCallback_2(pButtonCallback_2 callback_local, uint8_t par1, uint8_t par2);

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes rotary encoder and joystick button
 */
void button_init(bool debouncing_option);

/** 
 * Get the state of the joystick button.
 */
bool button_isJoystickPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryPressed(void);

void button_checkState(void);

#endif /* SES_BUTTON_H_ */
