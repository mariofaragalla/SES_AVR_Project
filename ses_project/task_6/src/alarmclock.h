#ifndef ALARMCLOCK_H_
#define ALARMCLOCK_H_

/*INCLUDES *******************************************************************/
#include "../../lib/ses/ses_common.h"
#include "../../lib/ses/ses_led.h"
#include "../../lib/ses/ses_scheduler.h"

typedef uint32_t systemTime_t;
typedef struct fsm_s Fsm;               //< typedef for alarm clock state machine
typedef struct event_s Event;           //< event type for alarm clock fsm
typedef uint8_t fsmReturnStatus;        //< typedef to be used with above enum

/** typedef for state event handler functions */
typedef fsmReturnStatus (*State)(Fsm *, const Event *);
void setFsm(Fsm *fsm);

struct time_t {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t milli;
};

struct fsm_s {
    State state;            //< current state, pointer to event handler
    bool isAlarmEnabled;    //< flag for the alarm status
    struct time_t timeSet;  //< multi-purpose var for system time and alarm time
};

struct event_s {
    uint8_t signal; //< identifies the type of event
};

/** return values */
enum {
    RET_HANDLED,    //< event was handled
    RET_IGNORED,    //< event was ignored; not used in this implementation
    RET_TRANSITION  //< event was handled and a state transition occurred
};

/** Events used by the FSM for the flow **/
enum {
    ENTRY,                // event that indicates first time entry for the state
    EXIT,                 // event that indicates leaving from the state 
    JOYSTICK_PRESSED,     // event that indicates Joystick was pressed for the state   
    ROTARY_PRESSED,       // event that indicates rotary was pressed for the state 
    ALARM_MATCHED,        // event that indicates alarm matched while alarm is enabled 
    SECONDS_LIMIT_REACH   // event that indicates 5 seconds where reached after alarm was on 
};

/**
 * Function to return the time from the Schedular
**/
systemTime_t scheduler_getTime();

/**
 * Function to set the time from the Schedular
 * param 1 : time in MS of type uint32_t
**/
void scheduler_setTime(systemTime_t time);

/**
 * Function to set the clock timing intially to 0 and Reset state
**/
void clock_init(void);

/**
 * Function to be used anytime needed to generate and dispatch events to the FSM
 * usually used in callbacks 
**/
void generate_joystick_event();
void generate_rotary_event();
void generate_compare_alarm();
void generate_seconds_reach();

/**
 * Function called internally to set the user's FSM to be the main FSM in the clock code
 * param 1: Finite State Machine / to be passed by the initiallization function
**/
void setFsm(Fsm *fsm);

/**
 * Tasks used by the Schedular 
 * param 1: void pointer that can be of any type to be used by the function
**/
void clock_task(void* val);
void compare_alarm_task(void *val);
void toggle4HZ_task(void *val);

/* dispatches events to state machine, called in application*/
inline static void fsm_dispatch(Fsm * fsm, const Event * event) {
    static Event entryEvent = {.signal = ENTRY};
    static Event exitEvent = {.signal = EXIT};
    State s = fsm->state;
    fsmReturnStatus r = fsm->state(fsm, event);
    if (r == RET_TRANSITION) {
        s(fsm, &exitEvent); //< call exit action of last state
        fsm->state(fsm, &entryEvent); //< call entry action of new state
    }
}
/* sets and calls initial state of state machine */
inline static void fsm_init(Fsm * fsm, State init) {
    //... other initialization
    Event entryEvent = {.signal = ENTRY};
    clock_init();                   
    led_redOff();
    led_yellowOff();
    led_greenOff();
    fsm->state = init;
    fsm->state(fsm, &entryEvent);
    setFsm(fsm);
}

/** 
 * Function representing the first state where the user input hours
 * param 1: Finite State Machine in use 
 * param 2: the event to be used and dispatched
**/
fsmReturnStatus inputHours(Fsm * fsm, const Event * event);

/**
 * Function representing the second state where the user input minuites
 * param 1: Finite State Machine in use 
 * param 2: the event to be used and dispatched
**/ 
fsmReturnStatus inputMins(Fsm * fsm, const Event * event);

/**
 * Function to start clock and later allow normal operation
 * param 1: Finite State Machine in use 
 * param 2: the event to be used and dispatched
**/ 
fsmReturnStatus normalClock(Fsm * fsm, const Event * event);

/**
 * Function to enable or disable alarm
 * param 1: Finite State Machine in use 
 * param 2: the event to be used and dispatched
**/ 
fsmReturnStatus setAlarmStatus(Fsm * fsm, const Event * event);

/**
 * Function to allow user to input alarm hours
 * param 1: Finite State Machine in use 
 * param 2: the event to be used and dispatched
**/ 
fsmReturnStatus inputAlarmHours(Fsm * fsm, const Event * event);

/**
 * Function to allow user to input alarm mins
 * param 1: Finite State Machine in use 
 * param 2: the event to be used and dispatched
**/ 
fsmReturnStatus inputAlarmMins(Fsm * fsm, const Event * event);

/**
 * Function to wait for alarm to be reached
 * param 1: Finite State Machine in use 
 * param 2: the event to be used and dispatched
**/ 
fsmReturnStatus waitTimeMatch(Fsm * fsm, const Event * event);

/**
 * Function indicating alarm is on
 * param 1: Finite State Machine in use 
 * param 2: the event to be used and dispatched
**/ 
fsmReturnStatus alarmON(Fsm * fsm, const Event * event);

/**
 * Function reset alarm 
 * param 1: Finite State Machine in use 
 * param 2: the event to be used and dispatched
**/
fsmReturnStatus alarmOFF(Fsm * fsm, const Event * event);

#endif /* ALARMCLOCK_H_ */