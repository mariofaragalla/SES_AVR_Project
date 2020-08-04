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

// Parameter for Task scheduler
uint8_t parameter = 0;                  
void* param_ptr_0 = &parameter; 

#define TASK_EXPIRE_TIME    0
#define TASK_PERIOD_TIME    5       // period for PID calculations every 5MSEC
#define FIRST_ACTIVE_PWM    120
#define YLIMIT              20
#define XLIMIT              3
#define RESET               0
#define NEG                 -1

int getMax(int num1, int num2);     // Function to get Maximum of 2 numbers
int getMin(int num1, int num2);     // Function to get Minimum of 2 numbers
void check_pid_task(void* val);     // PID calculations
void plot (void);                   // plot PID

int error = 0;
int error_integral = 0;
int aw_limit = 10; 
int f_target = 100;
int f_current = 0;
uint8_t kp = 1;
uint8_t ki = 0;
uint8_t kd = 0;
int u = 0;
int error_last = 0;

// pTimer5callback cb_int0 = &calc_time_one_rev;

int main (void){
    
    // timer5_set_callback(cb_int0); 

    // Libraries Initialization 
    uart_init(BAUD_RATE);
    led_redInit();
    led_yellowInit();
    led_greenInit();
    scheduler_init();
    pwm_init();
    lcd_init();
    motorFrequency_init();
    pwm_setDutyCycle(STOP);
    sei(); 

    // Define Tasks for scheduler
    taskDescriptor task_1;               
    
    // Initialize task with its individual values
    task_init(&task_1, &check_pid_task, param_ptr_0, TASK_EXPIRE_TIME, TASK_PERIOD_TIME, DISABLE_EXE, NULL);
    scheduler_add(&task_1);      // Add task to scheduler tasks array

    while (1){   
        scheduler_run();        // Run Scheduler
    }
    return 0;
}

void check_pid_task(void* val){
    f_current = motorFrequency_getMedian();
    error = abs(f_target - f_current);
    error_integral = getMax(getMin(error_integral + error , aw_limit), NEG*aw_limit);
    u = abs(FIRST_ACTIVE_PWM - (kp*error) + (ki * error_integral) + (kd * (error_last - error)));
    error_last = error;

    // Motor operates only between 120 and 0 pwm values due to hardware limitation
    if(u > FIRST_ACTIVE_PWM){       
        u = FIRST_ACTIVE_PWM;
    } 
    else if (u < MAXIMUM_SPEED_PWM){
        u = MAXIMUM_SPEED_PWM;
    } 

    pwm_setDutyCycle((uint8_t)u);       // Set PID adjusted pwm 

    // plot();
}

int getMax(int num1, int num2){ 
    if(num1 >= num2){               // Function returns Maximum
        return num1;
    } else {
        return num2;
    }
}

int getMin(int num1, int num2){
    if(num1 <= num2){               // Function returns Minimum
        return num1;
    } else {
        return num2;
    }
}

void plot (void){
    static uint32_t x = RESET;
    static uint32_t y = RESET;

    lcd_setPixel(x,y,true);
    x++; 
    y++;
    if(y == YLIMIT){
        y = RESET;
    }
    if(x == XLIMIT){
        x = RESET;
    }
    lcd_setPixel(x,y,true);
    x++; 
    y++;
    if(y == YLIMIT){
        y = RESET;
    }
    if(x == XLIMIT){
        x = RESET;
    }
    lcd_setPixel(x,y,true);
    x++;
    y++;
    if(y == YLIMIT){
        y = RESET;
    }
    if(x == XLIMIT){
        x = RESET;
    }
}