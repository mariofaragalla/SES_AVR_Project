#include "../../lib/ses/pscheduler.h"
#include "../../lib/ses/ses_led.h"
#include "avr/interrupt.h"

void taskA (void) {

while (1) {  
 }
}
void taskB (void) {

while (1) { 
 }
}
void taskC (void) {

while (1) { 
 }
}

task_t taskList[] = {taskA, taskB, taskC};
uint8_t numTasks = 3;

int main(void) {

led_redInit();
led_yellowInit();
led_greenInit();

pscheduler_init();
sei();

pscheduler_run(taskList, numTasks);

return 0;
}