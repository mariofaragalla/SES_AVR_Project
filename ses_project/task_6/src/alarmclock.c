#include "alarmclock.h"
#include "../../lib/ses/ses_uart.h"
#include "../../lib/ses/ses_lcd.h"
#include "../../lib/ses/ses_led.h"
#include "../../lib/ses/ses_scheduler.h"

// Definitions used by Alarm to adjust clocks and signalling
#define TRANSITION(newState) (fsm->state = newState, RET_TRANSITION)
#define TASK_EXPIRE_TIME    0
#define TASK_PERIOD_TIME    1
#define CHECK_PERIOD_ALARM  5 
#define TOGGLE_WITH_4HZ     100

// millisec and clock used for task 6_1 only
volatile static systemTime_t millisecs = 0;     
struct time_t clock; 

// Flags handling special cases during clock and alarm adjustment
bool returnFromAlarm = false;           // Flag indicating adjustment of normal operation after alarm is turned off
bool firstAlarm = true;                 // Flag indicating that's the first time to adjust the alarm / handles empty variables 
bool currentlyAdjustingTime = false;    // Flag to stop alarm and clock matching during adjusting the time 
bool alarmAlreadyAdjusted = false;      // Flag indicating alarm was adjusted while being disabled / user can then enable
extern bool isSchedulerEmpty;           

// Parameter for Task scheduler
uint8_t parameter = 0;                  
void* param_ptr_0 = &parameter; 

// Definition for Schedular tasks used by the clock
taskDescriptor task1;
taskDescriptor task2;
taskDescriptor task3;

// Define the State Machine
Fsm theFsm;

// time structures to be adjusted continuously by schedular
struct time_t alarmTime;    
struct time_t normalTime;

/*
    In the following Dispatch functions
    1. Create an event that an input was active
    2. Announce to the FSM states that the event occured
    3. Events are found in ENUM in the alarmclock.h file
*/
static void joystickPressedDispatch(void * param);
static void rotaryPressedDispatch(void * param);
static void alarmMatchedDispatch(void * param);
static void secondsReachDispatch(void * param);

void scheduler_setTime(systemTime_t time){
    millisecs = time;

    if(millisecs % 1000 == 0){
        clock.second ++;
        if(clock.second %60 == 0){
            clock.second = 0;
            clock.minute ++;
            if(clock.minute % 60 == 0){
                clock.minute = 0;
                clock.hour ++;
                if(clock.hour % 24 == 0){
                    clock.hour = 0;
                }
            }
        }
    }
}

systemTime_t scheduler_getTime(){
	return millisecs;
}

void setFsm(Fsm *fsm){
    theFsm = *fsm;              // Assign the user's FSM to the Clock FSM

    // Task to be used by the clock initiallizations 
    task_init(&task1, &clock_task, param_ptr_0, TASK_EXPIRE_TIME, TASK_PERIOD_TIME, DISABLE_EXE, &task2);
    task_init(&task2, &compare_alarm_task, param_ptr_0, TASK_EXPIRE_TIME, CHECK_PERIOD_ALARM, DISABLE_EXE, &task3);
    task_init(&task3, &toggle4HZ_task, param_ptr_0, TASK_EXPIRE_TIME, TOGGLE_WITH_4HZ, DISABLE_EXE, NULL);
}

static void joystickPressedDispatch(void * param) {
    Event e = {.signal = JOYSTICK_PRESSED};         // Create an event that joystick button was pressed
    fsm_dispatch(&theFsm, &e);                      // Announce to the FSM states that the event occured                             
}

static void rotaryPressedDispatch(void * param) {
    Event e = {.signal = ROTARY_PRESSED};           // Create an event that rotary button was pressed
    fsm_dispatch(&theFsm, &e);                      
}
 
static void alarmMatchedDispatch(void * param) {
    Event e = {.signal = ALARM_MATCHED};            // Create an event that alarm reached the time desired            
    fsm_dispatch(&theFsm, &e);                      
}

static void secondsReachDispatch(void * param) {
    Event e = {.signal = SECONDS_LIMIT_REACH};      // Create an event that alarm was on for 5 seconds already
    fsm_dispatch(&theFsm, &e);
}

void generate_joystick_event(){                     // used by Joystick call back to dispatch joystick pressed event
    uint8_t param = 0;
    joystickPressedDispatch((uint8_t *)&param);
}

void generate_rotary_event(){                       // used by rotary call back to dispatch rotary pressed event
    uint8_t param = 0;
    rotaryPressedDispatch((uint8_t *)&param);
}

void generate_seconds_reach(){
    uint8_t param = 0;
    secondsReachDispatch((uint8_t *)&param);
}

void generate_compare_alarm(){
    if(theFsm.isAlarmEnabled){              // Alarm matching only occurs if the alarm was enabled by the user
        if(alarmTime.hour == normalTime.hour && alarmTime.minute == normalTime.minute){     // if alarm matched Event is dispatched
            uint8_t param = 0;
            alarmMatchedDispatch((uint8_t *)&param);
        } 
    }
}

// State ONE
fsmReturnStatus inputHours(Fsm * fsm, const Event * event){
    struct time_t *tempclk = &(fsm->timeSet);   // Temporary pointer to adjust the clock without changing the actual clock changing contiuously

    switch(event->signal){ 
        case ENTRY:                                     
            lcd_clear();                                    // Clear LCD upon Entry
            lcd_setCursor(0, 1);
            fprintf(lcdout, "please enter hour: \n");
            fprintf(lcdout, "Clock: HH:MM");                // Show user desired Clock format
            fprintf(uartout, "please enter hour: \n");
            fprintf(uartout, "Clock: HH:MM \n");
            return RET_HANDLED;

        case JOYSTICK_PRESSED: 
            return TRANSITION(inputMins);       // Joystick press causes a transition to the next state -> input actual clock minuites

        case ROTARY_PRESSED:                    // Rotary press to adjust Hours of the normal clock
            tempclk->hour ++;
            if(tempclk->hour == 24){            // Reset after 24 hours
                tempclk->hour = 0;
            }

            lcd_clear();
            lcd_setCursor(0, 1);
            fprintf(lcdout, "Clock: %02d:MM", tempclk->hour);
            fprintf(uartout, "Clock: %02d:MM \n", tempclk->hour);
            return RET_HANDLED;

        case EXIT:
            normalTime.hour = tempclk->hour;    // update actual clock hours upon exiting this state
            tempclk->hour = 0;                  // reset time setter hours
            return RET_HANDLED;
            
        default:
            return RET_IGNORED;
    }
}

// State TWO
fsmReturnStatus inputMins(Fsm * fsm, const Event * event){
    struct time_t *tempclk = &(fsm->timeSet);   // Temporary pointer to adjust the clock without changing the actual clock changing contiuously

    switch(event->signal){ 
        case ENTRY: 
            lcd_clear();
            lcd_setCursor(0, 1);        
            fprintf(lcdout, "please enter mins: ");                                     // Ask user to enter required minuites
            fprintf(lcdout, "Clock: %02d:%02d \n", normalTime.hour, tempclk->minute);
            fprintf(uartout, "please enter mins: \n");
            fprintf(uartout, "Clock: %02d:%02d \n", normalTime.hour, tempclk->minute);
            return RET_HANDLED;

        case JOYSTICK_PRESSED: 
            return TRANSITION(normalClock);     // Joystick press leads to tranistion to next state -> Normal operation mode

        case ROTARY_PRESSED:                    // Rotary press to increment minuites every press
            tempclk->minute ++;                 
            if(tempclk->minute == 60){          // Reset when reaching 60 minuites
                tempclk->minute = 0;
            }

            lcd_clear();
            lcd_setCursor(0, 1);
            fprintf(lcdout, "Clock: %02d:%02d \n", normalTime.hour, tempclk->minute);
            fprintf(uartout, "Clock: %02d:%02d \n", normalTime.hour, tempclk->minute);
            return RET_HANDLED;

        case EXIT:
            normalTime.minute = tempclk->minute;    // update actual clock minuites upon exiting this state  
            tempclk->minute = 0;                    // reset time setter minuites      
            return RET_HANDLED;
            
        default:
            return RET_IGNORED;                     // Ignore any other events
    }   
}

// State THREE
fsmReturnStatus normalClock(Fsm * fsm, const Event * event){
    switch(event->signal){ 
        case ENTRY: 
            lcd_clear();            // Clear LCD upon entry and start printing the clock every one second
            lcd_setCursor(0, 1);    // printing screen in line 1 as desired 
            fprintf(lcdout, "Clock: %02d:%02d:%02d", normalTime.hour, normalTime.minute, normalTime.second);
            fprintf(uartout, "Clock: %02d:%02d:%02d \n", normalTime.hour, normalTime.minute, normalTime.second);
             
            if(!returnFromAlarm){   // Case first entry -> add the task for starting the clock countings
                scheduler_add(&task1);
                isSchedulerEmpty = false;          // indicate schedular is not empty    
                fsm->isAlarmEnabled = false;       // initially disable alarm
            }
            else{ 
                led_yellowOff();                   // turn yellow led off if alarm is not enabled
                returnFromAlarm = false;
            }

            lcd_setCursor(0, 2);
            fprintf(lcdout, "To enable alarm please press rotary");         // User guide
            fprintf(uartout, "To enable alarm please press rotary \n");
            return RET_HANDLED;

        case JOYSTICK_PRESSED: 
            return TRANSITION(inputAlarmHours);     // Joystick press leads to transition to next state -> input alarm hours

        case ROTARY_PRESSED: 
             if(fsm->isAlarmEnabled){           // Rotary press toggles alarm enable/disable states 
                fsm->isAlarmEnabled = false;    // Alarm disabled
                led_yellowOff();                // Yellow LED turned on only if alarm is enabled
                lcd_clear();
                lcd_setCursor(0,2);
                fprintf(lcdout, "Alarm Disabled - To enable press rotary\n");  // print alarm state
                fprintf(uartout, "Alarm Disabled - To enable press rotary\n");
            } 
            else{
                fsm->isAlarmEnabled = true;     // Alarm enabled
                led_yellowOn();
                if(alarmAlreadyAdjusted){       // Case where the alarm hours and minuites where adjusted while the alarm was disabled -> only enable the alarm and wait for matching
                    fprintf(uartout, "%02d, %02d, %02d, %02d \n", normalTime.hour, alarmTime.hour, normalTime.minute, alarmTime.minute);
                    if((normalTime.hour > alarmTime.hour) || (normalTime.minute > alarmTime.minute) || (normalTime.second > alarmTime.second)){ // Alarm passed and then the usesr enabled the alarm
                        lcd_clear();
                        lcd_setCursor(0,2);
                        fprintf(lcdout, "Alarm Missed");                         
                        fprintf(uartout, "Alarm Missed \n");
                        return TRANSITION(alarmOFF);    // Transition to final state for Reset then back to normal operation from final state
                    }
                    lcd_clear();
                    lcd_setCursor(0,2);
                    fprintf(lcdout, "Alarm Enabled - Wait for time match");                                             
                    fprintf(uartout, "Alarm Enabled - Wait for time match\n");
                    return TRANSITION(waitTimeMatch);   // If alarm is enabled + not missed -> wait for the matching
                }
                lcd_clear();
                lcd_setCursor(0,2);
                fprintf(lcdout, "Alarm Enabled - To adjust alarm hours press joystick");                                             
                fprintf(uartout, "Alarm Enabled - To adjust alarm hours press joystick\n");
            }
            return RET_HANDLED;
        
        case EXIT:                  // Ignore any other inputs -> do not affect current state 
            return RET_IGNORED;

        case ALARM_MATCHED:
            return RET_IGNORED;

        case SECONDS_LIMIT_REACH:
            return RET_IGNORED;
            
        default:
            lcd_clear();
            lcd_setCursor(0, 1);
            fprintf(lcdout, "Clock: %02d:%02d:%02d", normalTime.hour, normalTime.minute, normalTime.second);        
            fprintf(uartout, "Clock: %02d:%02d:%02d \n", normalTime.hour, normalTime.minute, normalTime.second);        
            return RET_HANDLED;
    }
}

// State FOUR
fsmReturnStatus inputAlarmHours(Fsm * fsm, const Event * event){
    struct time_t *tempAlarmClk = &(fsm->timeSet);  // Temp storing of alarm adjustment to avoid changing actual alarm times used 

    switch(event->signal){ 
        case ENTRY: 
            lcd_clear();
            lcd_setCursor(0, 1);
            fprintf(lcdout, "please enter alarm hour: \n");     // Ask user for alarm time adjustments
            fprintf(lcdout, "Clock: HH:MM");
            fprintf(uartout, "please enter alarm hour: \n");
            fprintf(uartout, "Clock: HH:MM \n");
            currentlyAdjustingTime = true;          // avoid alarm and actual time matching during alarm time adjustment
            return RET_HANDLED;

        case JOYSTICK_PRESSED: 
            return TRANSITION(inputAlarmMins);      // Joystick press leads to state transition -> input alarm minuites 

        case ROTARY_PRESSED: 
            tempAlarmClk->hour ++;                  // Rotary press to adjust Hours of the Alarm time
            if(tempAlarmClk->hour == 24){           // Reset after 24 hours        
                tempAlarmClk->hour = 0;                 
            }

            lcd_clear();
            lcd_setCursor(0, 1);
            fprintf(lcdout, "Clock: %02d:MM \n", tempAlarmClk->hour);
            fprintf(uartout, "Clock: %02d:MM \n", tempAlarmClk->hour);
            return RET_HANDLED;

        case EXIT:
            alarmTime.hour = tempAlarmClk->hour;    // update alarm clock hours upon exiting this state                    
            tempAlarmClk->hour = 0;                 // reset alarm time setter hours  
            return RET_HANDLED;
            
        default:
            return RET_IGNORED;
    }
}

// State FIVE
fsmReturnStatus inputAlarmMins(Fsm * fsm, const Event * event){
    struct time_t *tempAlarmClk = &(fsm->timeSet);  // Temp storing of alarm adjustment to avoid changing actual alarm minuites used
    
    switch(event->signal){ 
        case ENTRY: 
            lcd_clear();                            // LCD clear upon entry
            lcd_setCursor(0, 1);
            fprintf(lcdout, "please enter alarm mins: \n");         // Ask user to enter minuites for alarm
            fprintf(lcdout, "Clock: %02d:%02d", alarmTime.hour, alarmTime.minute);
            fprintf(uartout, "please enter alarm mins: \n");
            fprintf(uartout, "Clock: %02d:%02d \n", alarmTime.hour, alarmTime.minute);
            return RET_HANDLED;

        case JOYSTICK_PRESSED:          // Joystick press leads to state transition according to alarm enabled state
            if(fsm->isAlarmEnabled){    // if alarm enabled state -> Transition to Time wait match state
                // Case where the alarm time already passed / ajusted falsly by user
                if((normalTime.hour > alarmTime.hour) || (normalTime.minute > tempAlarmClk->minute)  || (normalTime.second > alarmTime.second)){
                    lcd_clear();
                    lcd_setCursor(0,2);                 
                    fprintf(lcdout, "Alarm Missed !");                                             
                    fprintf(uartout, "Alarm Missed ! \n");
                    return TRANSITION(alarmOFF);    // If the alarm is falsly -> transtion to final state and then to normal clock is internal
                }
                return TRANSITION(waitTimeMatch);   // If alarm is not skipped already -> wait for alarm and actual time match
            }
            else{
                alarmAlreadyAdjusted = true;        
                return TRANSITION(normalClock);     // If alarm is not enabled but adjusted -> wait for the user to enable the alarm in normal clock operations
            }

        case ROTARY_PRESSED:            // Rotary press to adjust alarm minutes 
            tempAlarmClk->minute ++;    // increment alarm minutes every button
            if(tempAlarmClk->minute == 60){     // Reset after 60 seconds
                tempAlarmClk->minute = 0;
            }
 
            lcd_clear();
            lcd_setCursor(0, 1);
            fprintf(lcdout, "Clock: %02d:%02d", alarmTime.hour ,tempAlarmClk->minute);
            fprintf(uartout, "Clock: %02d:%02d \n", alarmTime.hour, tempAlarmClk->minute);
            return RET_HANDLED;

        case EXIT:
            alarmTime.minute = tempAlarmClk->minute;    // update alarm clock minutes upon exiting this state   
            tempAlarmClk->minute = 0;                   // reset alarm time setter minutes    
            currentlyAdjustingTime = false;             // Time adjusting is finished -> alarm matching is allowed
            return RET_HANDLED;
            
        default:
            return RET_IGNORED;
    }
}

// State SIX
fsmReturnStatus waitTimeMatch(Fsm * fsm, const Event * event){

    switch(event->signal){ 
        case ENTRY:             
            lcd_clear();            
            lcd_setCursor(0, 0);
            fprintf(lcdout, "Alarm Clock: %02d:%02d:%02d", alarmTime.hour, alarmTime.minute, alarmTime.second);
            fprintf(uartout, "Alarm Clock: %02d:%02d:%02d \n", alarmTime.hour, alarmTime.minute, alarmTime.second);
            scheduler_add(&task2); // Add to schedular the task to compare clock and alarm values
            return RET_HANDLED;

        case JOYSTICK_PRESSED: 
            return RET_IGNORED;

        case ROTARY_PRESSED: 
            return RET_IGNORED;

        case EXIT:
            return RET_IGNORED;

        case ALARM_MATCHED:
            if(fsm->isAlarmEnabled && !currentlyAdjustingTime){     // Only if Alarm matched while alarm enabled and user not adjusting time
                return TRANSITION(alarmON); 
            } 
            else {
                return RET_HANDLED;
            } 
            
        default:
            lcd_clear();
            lcd_setCursor(0, 0);
            fprintf(lcdout, "Alarm Clock: %02d:%02d:%02d \n", alarmTime.hour, alarmTime.minute, alarmTime.second);
            fprintf(uartout, "Alarm Clock: %02d:%02d:%02d \n", alarmTime.hour, alarmTime.minute, alarmTime.second);
            return RET_HANDLED;
    }
}

// State SEVEN
fsmReturnStatus alarmON(Fsm * fsm, const Event * event){

    switch(event->signal){ 
        case ENTRY: 
            if(firstAlarm){
                scheduler_add(&task3);  // Add schedular task to terminate alarm after 5 seconds
                firstAlarm = false;     // Only first time -> adds the task
            }
            return RET_HANDLED;

        case JOYSTICK_PRESSED:          // Any button press or 5seconds terminates alarm
            return TRANSITION(alarmOFF);

        case ROTARY_PRESSED: 
            return TRANSITION(alarmOFF);

        case SECONDS_LIMIT_REACH:
            return TRANSITION(alarmOFF);
            
        default:
            return RET_IGNORED;
    }
}

// State EIGHT
fsmReturnStatus alarmOFF(Fsm * fsm, const Event * event){

    switch(event->signal){ 
        case ENTRY: 
            alarmAlreadyAdjusted = false;       // Reset all flags and indications
            returnFromAlarm = true;
            fsm->isAlarmEnabled = false;
            led_yellowOff();
            led_redOff();
            alarmTime.hour = 0;
            alarmTime.minute = 0;
            alarmTime.second = 0;
            return TRANSITION(normalClock);     // Transition state back to normal clock operation

        default:
            return RET_IGNORED;
    }
}

void clock_init(){
    theFsm.timeSet.hour = 0;        // Reset and Initialize all clock values 
    theFsm.timeSet.second = 0;
    theFsm.timeSet.minute = 0;
    theFsm.timeSet.milli = 0;
}

void clock_task(void* val){
    normalTime.milli++;
	if(normalTime.milli == 1000){
		normalTime.milli = 0; 
		normalTime.second++;
        led_greenToggle();
        
        if(theFsm.state == alarmON){        // 5 seconds terminate for the alarm if it is currently in the on State
            static uint8_t start = 0; 
            if(start == 5){
                generate_seconds_reach();   // generate event to dispatch in FSM that 5 seconds are reached
                start = 0;
            } else{
                start++;
            }
        }

		if(normalTime.second == 60){
			normalTime.second = 0;
			normalTime.minute++;
			
			if(normalTime.minute == 60){
				normalTime.minute = 0;
				normalTime.hour++;

				if(normalTime.hour == 24){
					normalTime.hour = 0;
				}
			}
		}
        // Print the clock only if in specific states 
        if((theFsm.state == normalClock && !theFsm.isAlarmEnabled) || theFsm.state == waitTimeMatch){   
	        fprintf(lcdout, "%02d:%02d:%02d \n" ,normalTime.hour,normalTime.minute,normalTime.second);
	        fprintf(uartout, "%02d:%02d:%02d \n" ,normalTime.hour,normalTime.minute,normalTime.second);
        }
    } 
}

void compare_alarm_task(void *val){
    generate_compare_alarm();
}
void toggle4HZ_task(void *val){
    if(theFsm.state == alarmON){
        led_redToggle();
    }else{
        led_redOff();
    }
}