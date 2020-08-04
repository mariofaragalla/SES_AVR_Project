#include "../../lib/ses/ses_timer.h"
#include "../../lib/ses/ses_scheduler.h"
#include "../../lib/ses/ses_led.h"
#include "../../lib/ses/ses_uart.h"
#include "../../lib/ses/ses_button.h"
#include "../../lib/ses/ses_lcd.h"
#include "util/atomic.h"

extern bool flagJoystickDebounce;     // flag used by the Task to check debounce state of the Joystick button
extern bool flagRotaryDebounce;       // flag used by the Task to check debounce state of the Rotary button

uint8_t tenth_of_seconds = 0;           // to be used in the stopwatch (100 ms) -> for counting 0.1Sec
uint8_t seconds = 0;                    // Seconds counter

bool flagStopWatch = false;             // flag used for toggling stopwatch ON/OFF
bool repeated = false;
uint8_t repeated_1 = 0;

uint8_t parameter = GREEN;                  
void* param_ptr_0 = &parameter; 

int main (void)
{
    uart_init(BAUD_RATE);               // initialize uart with defualt 57600 baud rate                 
    lcd_init();                         // initialize LCD 
    button_init(true);                  // initialize Buttons 
    led_greenInit();                    // initialize LEDs
    led_yellowInit();                    
    led_redInit();      
    scheduler_init();                
    
    // initialize tasks 1 to 4 using pointer of task descriptor type (to be used for determining tasks)
    // Book memory location of size task descriptor for task_1 to task_4
    taskDescriptor *task_1;            
    task_1 = (taskDescriptor*)malloc(sizeof(taskDescriptor));   
    taskDescriptor *task_2;             
    task_2 = (taskDescriptor*)malloc(sizeof(taskDescriptor));   
    taskDescriptor *task_3;             
    task_3 = (taskDescriptor*)malloc(sizeof(taskDescriptor));   
    taskDescriptor *task_4;             
    task_4 = (taskDescriptor*)malloc(sizeof(taskDescriptor));

    // Tasks initialization =====================================
    task_init(task_1, &led_Task, param_ptr_0, EXPIRE_INIT, LED_PERIOD, ENABLE_EXE, task_2);            
    task_init(task_2, &debounce_task, param_ptr_0, EXPIRE_INIT, DEBOUNCE_PERIOD, DISABLE_EXE, task_3);
    task_init(task_3, &yellow_toggle_task, param_ptr_0, EXPIRE_INIT, JOYSTICK_PERIOD, DISABLE_EXE, task_4);
    task_init(task_4, &stop_watch_task, param_ptr_0, EXPIRE_INIT, STOPWATCH_PERIOD, DISABLE_EXE, NULL);

    // add task 1 to 4 to the scheduler taskList ================
    scheduler_add(task_1);          
    scheduler_add(task_2);     
    scheduler_add(task_3);       
    scheduler_add(task_4);      

    // allow global interrupts
    sei();       

    // run scheduler in an infinite loop
    while (1) {
        scheduler_run();       
    }
    return 0;
}



/* Task that uses the parameter "parameter" to toggle an LED of user's choice 
 * 0.5 Hz speed 
*/
void led_Task (void* val) {
    switch(parameter)           // Depending on the user's choice the LED is chosen
    {
        case GREEN:  led_greenToggle();  break;   
        case RED:    led_redToggle();    break;
        case YELLOW: led_yellowToggle(); break;
        default:  break;
    }
    return;
}

// Task responsible for checking debounce states of the buttons every 5Ms
void debounce_task(void* val) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        button_checkState();                // changes global flags whether the buttons are pressed or not after debouncing
    }
}

// Task responsible for toggling yellow LED upon user's button press
void yellow_toggle_task(void* val) {  	
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(!repeated)                       // flag for checking that the same press is not repeated twice  
        {
            if(flagJoystickDebounce) {    // check whether Joystick is actually pressed or not -> after debounce checks
                repeated = true;
                led_yellowToggle();
            }
        } 
        else {
            repeated = false;
        }    
    }  
}

// Task responsible for stopwatch calculations and display every 100 Ms
void stop_watch_task(void* val) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(flagRotaryDebounce) {
            if(repeated_1 == 0) {
                if(!flagStopWatch) {        // check if the rotary button is pressed while the stopwatch is off
                    flagStopWatch = true;     
                }
                else {  // check if the rotary button is pressed while the stopwatch is on
                    flagStopWatch = false;
                }
                repeated_1++;
            }
            else {
                if(repeated_1 < 3){
                    repeated_1++;
                }
                else {
                    repeated_1 = 0;
                } 
            }
        }

        if(flagStopWatch) {            // if the stopwatch flag is enabled -> proceed with stopwatch calculations and display
            tenth_of_seconds++;

            if(tenth_of_seconds == 10){     // 200 Ms -> 5 times is 1 Second
                seconds++;
                tenth_of_seconds = 0;
            }
            lcd_clear();                           
            lcd_setCursor(0,0);             // print at the same position on the LCD display
            fprintf(lcdout, "StopWatch: %d.%d \n", seconds, tenth_of_seconds);      
        }
    }
}