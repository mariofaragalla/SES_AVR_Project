#ifndef MOTORFREQUENCY_H_
#define MOTORFREQUENCY_H_

#include <avr/io.h>
#include "ses_common.h"

#define MOTOR_CHECKS_NUM 6
#define HERTZ_TO_RPM 60

// Define callback type to INT0 interrupt
typedef void (*pTimer5callback)(void);
typedef void (*pMotorCallback)(void);        

void timer5_set_callback (pTimer5callback cb);

void motorFrequency_init();
uint16_t motorFrequency_getRecent();
uint16_t motorFrequency_getMedian();
uint16_t motorFrequency_update(void);
void calc_time_one_rev (void);
void stop_motor_detection (void);

#endif /* MOTORFREQUENCY_H_ */


 