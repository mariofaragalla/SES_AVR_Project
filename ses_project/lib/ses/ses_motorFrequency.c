/* INCLUDES ******************************************************************/
#include "ses_common.h"
#include "ses_button.h"
#include "defenitions.h"
#include "ses_led.h"
#include "ses_pwm.h"
#include "ses_uart.h"
#include "ses_timer.h"
#include "ses_motorFrequency.h"
#include <avr/io.h>

#define RESET 0
#define MAXIMUM_PWM 0                   // Inverted PWM Mode       
#define TIMER5_CYC_FOR_10Hz 25000       // set timer5 frequency to be 10Hz using OCR5A
#define TIMER5_CYC_INTERVAL 100         // Interrupt timer 5 every 100 millisecons
#define MSEC_100    100                 // 100 milliseconds
#define MOTOR_DEBOUNCE_PERIOD 5
#define MOTOR_STOP_DETECT_PERIOD 500
#define TIMER_RESET 300000
#define MAX_DATA 20
#define SEC_to_MSEC 1000
#define COMP_REVOLUTION_CNT 6           // six current drops are required for one complete revolution
#define MEDIAN_INDEX  10                // index of the median in the data array MAXDATA/2              

volatile uint32_t stop_check = 0;
volatile uint32_t last_pulse = 0;
volatile uint16_t diff = 0;
volatile uint32_t milli_seconds = 0;
volatile uint8_t current_drops = 0;
volatile uint16_t revolutions = 0;
volatile uint16_t revolutions_diff = 0;
volatile uint16_t end = 0;
volatile uint16_t data [MAX_DATA] = {};
volatile uint8_t Num = 0;
uint16_t hertz = 0;
volatile uint16_t rev_in_100 = 0;

pTimer5callback callback2 = &calc_time_one_rev ;
pMotorCallback callback1 = &stop_motor_detection;       // Set callback1 for timer 5 ;

void motorFrequency_init(){

    // External Interrupt Request Disable
    CLEAR_BIT(EIMSK, INT0); // EIFR -> INTF0 to check flag

    // Every rising edge interrupt
    SET_BIT(EICRA, ISC00);
    SET_BIT(EICRA, ISC01);

    // Write 1 logic in flag before enabling masking
    SET_BIT(EIFR, INTF0); 

    // External Interrupt Request Disable
    SET_BIT(EIMSK, INT0); 

    // Start Timer5:
    TCNT5 = RESET; // initialize timer counter to 0

    // Choose CTC Mode (put 0x4 in WGM53:50)
    CLEAR_BIT(TCCR5A, WGM50);
    CLEAR_BIT(TCCR5A, WGM51);
    SET_BIT(TCCR5B, WGM52);
    CLEAR_BIT(TCCR5B, WGM53);

    // Choose the prescaler to be 64 (put 0x03 in CS52:50 )
    SET_BIT(TCCR5B, CS50);
    SET_BIT(TCCR5B, CS51);
    CLEAR_BIT(TCCR5B, CS52);

    // Compare Match with OCIE5A
    SET_BIT(TIMSK5, OCIE5A); 

    // 0.001 second delay inside OCR5A	(64*1250)/16000000
    OCR5A = TIMER5_CYC_FOR_10Hz; 
}

// Sets callback for timer 5 
void timer5_set_callback (pTimer5callback cb){
    callback2 = cb;
}

// calculate time for one revolution
void calc_time_one_rev (void){
    if(revolutions_diff > RESET){   
        diff = MSEC_100 / revolutions_diff ;    // fetches revolutions in 100MS and calculates time needed for one
        revolutions_diff = RESET;               // Reset counter 
    }
    else{
        diff = RESET;                           // Case for motor not working revolutions are 0
    }
}

void stop_motor_detection (void){

    static uint8_t extra = RESET;

    // compare the times at which the motor interrupt and the timer interrupts were executed
    // if the difference between them exceeds the period for detecting that the motor stopped
    // then increase extra time 
    if((stop_check - last_pulse) > MOTOR_STOP_DETECT_PERIOD){   
        extra ++;
        // if the extra time is equal the period for debouncing, 
        // then the motor is detected to be stopped 
        if(extra == MOTOR_DEBOUNCE_PERIOD){
            led_greenOn();
            extra = RESET;
        }
    } else {    // otherwise the motor is running
        led_greenOff();
    }
    
}

uint16_t motorFrequency_update(void){ 
    hertz = ((revolutions * SEC_to_MSEC) / TIMER5_CYC_INTERVAL);  // Revolutions detected every Timer 5 interval     
    revolutions = RESET;        // Reset revolutions every time the frequency is calculated
    return hertz;
}

uint16_t motorFrequency_getRecent(void){   
    return hertz;
}

// Function sorts the samples and takes their median value
uint16_t motorFrequency_getMedian(void){ 
    uint16_t temp = RESET;
    for(int i=RESET ; i<MAX_DATA ; i++){      
        for(int j=RESET ; j<MAX_DATA-1 ; j++){
            
            if(data[j]>data[j+1]){          // Swapping performed to sort array
                temp       = data[j];
                data[j]    = data[j+1];
                data[j+1]  = temp;
            }
        }
    }
    return (data[MEDIAN_INDEX]) ;           // Return Median Value 
}

ISR(INT0_vect){

     if(GET_BIT(PIND, PD0)){
        last_pulse = milli_seconds;                 // Save the time when was the INT0 was called
        current_drops++;
        led_yellowToggle();
        if(current_drops == COMP_REVOLUTION_CNT){   // After 6 current drops we know a full rotation occured
            revolutions_diff++;                     // for calculating time for one revolution 
            revolutions++;
            current_drops = RESET;
        }
    }
}

ISR(TIMER5_COMPA_vect){  
    milli_seconds += TIMER5_CYC_INTERVAL;   // Counter incremented by 100 ms every timer 5 interrupt.
    stop_check = milli_seconds;             // Save the current time value 

    rev_in_100 = revolutions;               // Save the number of revolution per 100 ms 
    (*callback2)();
    (*callback1)();                          // Always check to detect a stopped motor 

    data[Num] = motorFrequency_update();    // Add data for Median calculations
    Num++;
    if(Num > MAX_DATA){                     // Reset index for Median data array
        Num = RESET;
    }
    if(milli_seconds == TIMER_RESET){       // Reset counter to avoid unwanted overflow
        milli_seconds = RESET;
    }
}