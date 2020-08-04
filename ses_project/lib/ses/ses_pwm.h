#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>
#include "ses_common.h"

// Inverting Mode Values
#define MAXIMUM_SPEED_PWM   0       
#define STOP            255     

/** 
 * Initialize timer 0 in (Fast PWM/compare match mode).
 */
void pwm_init(void);

/** 
 * Set the PWM for the motor.
 */
void pwm_setDutyCycle(uint8_t dutyCycle);

#endif /* PWM_H_ */