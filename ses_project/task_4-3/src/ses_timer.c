/* INCLUDES ******************************************************************/
#include "ses_timer.h"
#include "defenitions.h"
#include "ses_led.h"
#include "ses_button.h"
#include "ses_scheduler.h"

/* DEFINES & MACROS **********************************************************/
#define TIMER1_CYC_FOR_5MILLISEC    250	
#define TIMER2_CYC_FOR_1MILLISEC	250	

uint16_t timer2 = 0;
uint16_t timer1 = 0;
uint16_t timer3 = 0;
uint16_t total_time = 0;
uint16_t millis = 0;
pTimerCallback timer1_callback;
volatile pTimerCallback timer2_callback;

// extern taskDescriptor task_1;

// extern void (*update_ptr)(void);

/*FUNCTION DEFINITION ********************************************************/
void timer2_setCallback(pTimerCallback cb) {
	(*cb) ();
}

void timer2_start() {
	TCNT2 = 0;					// initialize timer counts to 0
	SET_BIT(TCCR2A, WGM21);		// Choose CTC Mode using 0x02 Wmg20 and Wmg21
	CLEAR_BIT(TCCR2A, WGM20);

	SET_BIT(TCCR2B, CS22);		// Choose 64 PSC Mode
	CLEAR_TWO_BITS(TCCR2B, CS21, CS20);

	SET_BIT(TIMSK2, OCIE2A);	// Compare Match with A

	SET_BIT(TIFR2, OCF2A);		// Flag set initially 

	OCR2A = 250;				// 250 delay inside OCR2A (equation ??)			
}

void timer2_stop() {
    // TODO
}

void timer1_setCallback(pTimerCallback cb) {
	millis++ ;
	if(millis == 5)		// 5 ms delay since timer ticks every 1 ms
	{
		(*cb) ();		// execute method in pointer to function
		millis = 0;		// reset timer for 5 ms delay
	}	
}

void timer1_start() {
	// timer1_callback = &button_checkState;

	TCNT1 = 0;					// initialize timer counts to 0
	CLEAR_BIT(TCCR1B, WGM13);	// Choose CTC Mode using 0x02 Wmg10 and Wmg11
	SET_BIT(TCCR1B, WGM12);
	CLEAR_BIT(TCCR1A, WGM11);		
	CLEAR_BIT(TCCR1A, WGM10);

	CLEAR_BIT(TCCR1B, CS12);	// Choose 64 PSC Mode
	SET_TWO_BITS(TCCR1B, CS11, CS10);

	SET_BIT(TIMSK1, OCIE1A);	// Compare Match with A

	SET_BIT(TIFR1, OCF1A);		// Flag set initially 

	OCR1A = 250;				// 250 delay inside OCR1A (equation ??)		(prescalar/frequency)* OCR1A 
}

void timer1_stop() {
	// TODO
}

void softwareTimer2 ()
{
	timer2++;
}

ISR(TIMER1_COMPA_vect) {
	timer1_setCallback(timer1_callback);
}

ISR(TIMER2_COMPA_vect) {
	// timer2_callback = &softwareTimer2;
	timer2_setCallback(timer2_callback);
}
