#include "../../lib/ses/ses_button.h"
#include "../../lib/ses/ses_led.h"
#include "../../lib/ses/ses_lcd.h"
#include "../../lib/ses/ses_uart.h"
#include "../../lib/ses/ses_pwm.h"
#include "../../lib/ses/ses_timer.h"
#include "../../lib/ses/ses_scheduler.h"
#include "../../lib/ses/ses_motorFrequency.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "util/atomic.h"

#define INVERTED_170_SPEED  85          // Inverted PWM
#define TASK_EXPIRE_TIME    0           
#define TASK_PERIOD_TIME    1000        // Display frequency every 1 SEC

// Scheduler Task and Toggling Method Prototype
void freqDisplay_task(void* val);
void dutyCycle_toggle(uint8_t min, uint8_t max);

uint8_t parameter = 0;                  
void* param_ptr_0 = &parameter;

pButtonCallback_2 cb_button = &dutyCycle_toggle;
pTimer5callback cb_one_rev = &calc_time_one_rev;

int main (void){

    // Setting callbacks before initialization
    timer5_set_callback(cb_one_rev); 
    button_setJoystickButtonCallback_2(cb_button, STOP, MAXIMUM_SPEED_PWM);

    // Libraries Initialization 
    uart_init(BAUD_RATE);   
    button_init(true);         
    scheduler_init();    
    led_yellowInit();
    led_greenInit();
    led_redInit();
    pwm_init();
    lcd_init();
    motorFrequency_init();
    pwm_setDutyCycle(STOP);
    sei();                      // Enable Global Interrupt

    // Task 5-2 Schedular
    taskDescriptor task_1;              

    // Initialize the task with its individual desired values
    task_init(&task_1, &freqDisplay_task, param_ptr_0, TASK_EXPIRE_TIME, TASK_PERIOD_TIME, DISABLE_EXE, NULL);
    scheduler_add(&task_1);      // Add task to Schedular task array

    while(1){
        scheduler_run();        // Run Schedular
    }
    return 0;
}

// Function is called by the Callback when the button is pressed
void dutyCycle_toggle(uint8_t min, uint8_t max){
    static bool toggle_flag = false;    // Flag used to toggle motor ON/OFF
    
    if(toggle_flag == false){
        pwm_setDutyCycle(max);
        toggle_flag = true;
    } 
    else {
        pwm_setDutyCycle(min);
        toggle_flag = false;
    }
}

// Task implementation to Display frequency every 1 SEC
void freqDisplay_task(void* val){
    uint16_t freq_recent = motorFrequency_getRecent();          // Store the frequency values to be displayed
    uint16_t freq_median = motorFrequency_getMedian();
    uint16_t rpm_recent = freq_recent * HERTZ_TO_RPM;           // Conversion from hertz to rpm
    uint16_t rpm_median = freq_median * HERTZ_TO_RPM;

    lcd_clear();                                                 // Clear and display
    lcd_setCursor(ZERO_POSITION ,ZERO_POSITION);
    fprintf(lcdout, "R: %hu, M: %hu", rpm_recent, rpm_median);
    fprintf(uartout, "R: %hu, M: %hu", rpm_recent, rpm_median);
    // Uncomment to display Hertz
    // fprintf(lcdout, "R: %hu, M: %hu \n", freq_recent, freq_median);
}