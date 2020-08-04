/*INCLUDES ************************************************************/
#include "ses_timer.h"
#include "pscheduler.h"
#include "util/atomic.h"
#include "ses_uart.h"
#include <util/delay.h>

/* PRIVATE VARIABLES **************************************************/
/** list of scheduled tasks */

taskDescriptor* taskList = NULL;	
extern pTimerCallback timer2_callback;

/*FUNCTION DEFINITION *************************************************/
/* Function used to update the expiry time of all tasks whenever the timer 2 makes an interrupt / 1 Ms
 * controls the execute flag by comparing expiry and period timings 
*/
static void scheduler_update(void) {	

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {	
		taskDescriptor * curr = taskList;		// set current task to the first task in the tasks list
		while (curr != NULL)	{				// while the current task is not empty 	
			curr->expire ++;					// increase exipre time for the current taskDescriptor
			if(curr->expire >= curr->period) {	// if the current task has finished execution
			
				curr->execute = 1;				// then state that it is finished 
				curr->expire = 0;				// and reset its expire time
			}
			curr = curr->next;					// move to the next task in the list
		}
    }
}

void scheduler_init() {
	// Book memory location of size task descriptor for the tasks list
	taskList = (taskDescriptor*)malloc(sizeof(taskDescriptor)); 
	timer2_callback = &scheduler_update; // set the timer callback in order to update the timer each 1 ms.						
	timer2_start();
}

void scheduler_run(task_t taskList_arr, uint8_t numTasks_total) {
	if (taskList != NULL) {						// if the tasks list is not empty 
			
		taskDescriptor * curr = taskList;		// then set the current task to the first task in the tasks list
		do {
			if (curr->execute) {				// if this task is executed 
			
				curr->task(curr->param);		// ?????????????????????????????
				curr->execute = 0;				// reset this task
			}
			curr=curr->next;					// move to the next task in the list 
		} while (curr != NULL);					// check if the current task is not empty for the next loop
	}	
}

bool scheduler_add(taskDescriptor * toAdd) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {	
		if(toAdd -> task == NULL) {				// if the task to be added is not valid (empty)
		
			return false;						// then get out of the function  
		}

		taskDescriptor* curr = taskList;		// set the current task to the first task in the tasks list
		while(curr != NULL) {					// while the current element is not empty
			if(curr->task == toAdd->task) {		// if the current task is the same as the task to be added
				return false;					// then get out of the function (the task was already added)
			}
			curr = curr->next;					// switch to the next task until the end of the Linked list
		}

		curr = taskList;								// if the task was not added then, loop from the beginning
		if(taskList->task==NULL) {						// if the tasks list is empty 
			equate_taskDescriptors(taskList, toAdd);	// make the task to be added the head of the tasks list
		} 	
		else {
			while(curr->next != NULL) {					// else loop until the end of the linked list
				curr = curr->next;						
			}	
			curr = curr->next;							// make the current task the last task in the list
			equate_taskDescriptors(curr, toAdd);		// add the input task to the end of the tasks list 
		}	
	}
	return true;										// the add has been successfully done
}

void scheduler_remove(taskDescriptor * toRemove) {		

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)									 
	{	
		if(toRemove->task == NULL || taskList == NULL) {	// get out of the function if the task to be removed is
			return;											// empty or the tasks list is empty
		}			
																		
		if(taskList != NULL) {							// if the tasks list is not empty
			if(taskList->task == toRemove->task) {		// then if the first task is the task to be removed
				taskList = taskList->next;				// then switch to the next task 
			} 
			else if (taskList->next == NULL) {			// else if the next task is empty
				return;									// get out of the function
			}
			else {
				taskDescriptor * prev = taskList;		// else set the previous task to the first task
				taskDescriptor * curr = taskList->next;	// and set the current task to the second task
				while (curr->task != toRemove->task) {	// while the task to be removed is not the current task 
					if(curr->next != NULL) {			// then if the next task is not empty
						prev = curr;					// set the previous task to the current one
						curr = curr->next;				// set the current task to the next one
					}
					else {								// else get out of the function if you didn't find the task
						return;							// that you want to remove in the tasks list
					}	
				}
				prev->next = curr->next;				// set the tail of the previous task to the next task
				curr = NULL;							// remove the current task 
			}
		}
	}	
	return;
}

// Method to assign an new task descriptor to an old one
void equate_taskDescriptors (taskDescriptor * taskDescriptor1, taskDescriptor * taskDescriptor2){
	taskDescriptor1->task = taskDescriptor2->task;				 
	taskDescriptor1->next = taskDescriptor2->next;
	taskDescriptor1->execute = taskDescriptor2->execute;
	taskDescriptor1->expire	= taskDescriptor2->expire;
	taskDescriptor1->period	= taskDescriptor2->period;
	taskDescriptor1->reserved = taskDescriptor2->reserved;
}

// Method responsible for initializing tasks 
void task_init(taskDescriptor* newTask, task_t impTask, void* passParam, uint16_t exp, uint16_t per, uint8_t exe, taskDescriptor* nxt)
{
    newTask->task = impTask;
    newTask->param = passParam;
    newTask->expire = exp;
    newTask->period = per;
    newTask->execute = exe;
    newTask->next = nxt;
    newTask->reserved = 1;
}