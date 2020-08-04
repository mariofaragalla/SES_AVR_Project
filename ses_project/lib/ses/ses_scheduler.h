#ifndef SCHEDULER_H_
#define SCHEDULER_H_

/*INCLUDES *******************************************************************/
#include "ses_common.h"


/* TYPES ********************************************************************/

/**type of function pointer for tasks */
typedef void (*task_t)(void*);

/** Task structure to schedule tasks
 */
typedef struct taskDescriptor_s {
	task_t task;          ///< function pointer to call
	void *  param;        ///< pointer, which is passed to task when executed
	uint16_t expire;      ///< time offset in ms, after which to call the task
	uint16_t period;      ///< period of the timer after firing; 0 means exec once
	uint8_t execute:1;    ///< for internal use
	uint8_t reserved:7;   ///< reserved
	struct taskDescriptor_s * next; ///< pointer to next task, internal use
} taskDescriptor;



// Enum identification
enum led_Option     // Enum to choose the led for toggling at 0.5Hz
{
    GREEN, 
    RED,
    YELLOW
}; 

/* PREDEFINED VALUES *******************************************************/
#define ENABLE_EXE	1	
#define DISABLE_EXE	0
#define LED_PERIOD	2000
#define DEBOUNCE_PERIOD	5
#define JOYSTICK_PERIOD 800
#define STOPWATCH_PERIOD 100
#define EXPIRE_INIT	0

/* FUNCTION PROTOTYPES *******************************************************/
void led_Task (void* val); 
void debounce_task (void* val);
void yellow_toggle_task (void* val);
void stop_watch_task(void* val);
void equate_taskDescriptors (taskDescriptor * taskDescriptor1, taskDescriptor * taskDescriptor2);
void task_init(taskDescriptor* newTask, task_t impTask, void* passParam, uint16_t exp, uint16_t per, uint8_t exe, taskDescriptor* nxt);

/**
 * Initializes the task scheduler. Uses hardware timer2 of the AVR.
 */
void scheduler_init();

/**
 * Runs scheduler in an infinite loop.
 */
void scheduler_run();

/**
 * Adds a new task to the scheduler.
 * May be called from any context (interrupt or main program)
 *
 * @param td   Pointer to taskDescriptor structure. The scheduler takes
 *             possesion of the memory pointed at by td until the task
 *             is removed by scheduler_remove or -- in case of a 
 *             non-periodic task -- the task is executed. td->expire 
 *             and td->execute are changed to by the task scheduler.
 *
 * @return     false, if task is already scheduled or invalid (NULL)
 *             true, if task was successfully added to scheduler and 
 *             will be executed after td->expire ms.
 */
bool scheduler_add(taskDescriptor * td);

/**
 * Removes a task from the scheduler.
 * May be called from any context (interrupt or main program)
 *
 * @param td	pointer to task descriptor to remove
 * */
void scheduler_remove(taskDescriptor * td);



#endif /* SCHEDULER_H_ */
