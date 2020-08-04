/* INCLUDES ******************************************************************/
#include "ses_timer.h"
#include "defenitions.h"
#include "ses_led.h"
#include "ses_button.h"
#include "ses_scheduler.h"
#include "ses_uart.h"

/* DEFINES & MACROS **********************************************************/
#define TIMER1_CYC_FOR_5MILLISEC    1249	
#define TIMER2_CYC_FOR_1MILLISEC	250	
#define RESET						0
#define FIVE_DELAY					5

uint16_t timer2 = 0;
uint16_t total_time = 0;

volatile uint16_t mydamnctr = 0;
volatile pTimerCallback timer1_callback = &button_checkState;
volatile pTimerCallback timer2_callback = &button_checkState;

/*FUNCTION DEFINITION ********************************************************/
void timer2_setCallback(pTimerCallback cb) {
	timer2_callback = cb;
}

void timer2_start() {
	// timer2_callback = &softwareTimer2;

	TCNT2 = RESET;					// initialize timer counts to 0
	SET_BIT(TCCR2A, WGM21);		// Choose CTC Mode using 0x02 Wmg20 and Wmg21
	CLEAR_BIT(TCCR2A, WGM20);

	SET_BIT(TCCR2B, CS22);		// Choose 64 PSC Mode
	CLEAR_TWO_BITS(TCCR2B, CS21, CS20);

	SET_BIT(TIMSK2, OCIE2A);	// Compare Match with A

	SET_BIT(TIFR2, OCF2A);		// Flag set initially 

	OCR2A = TIMER2_CYC_FOR_1MILLISEC;		// 250 delay inside OCR2A			
}

void timer2_stop() {
	// Choose 0x00 PSC Mode
    CLEAR_BIT(TCCR2B, CS22);
	CLEAR_TWO_BITS(TCCR2B, CS21, CS20);

}

void timer1_setCallback(pTimerCallback cb) {
	timer1_callback = cb;
}

void timer1_start() {

	timer1_setCallback(timer1_callback);

	TCNT1 = RESET;				// initialize timer counts to 0	
	
	CLEAR_BIT(TCCR1B, WGM13);	// Choose CTC Mode using 0x02 WGM10 and WGM11
	SET_BIT(TCCR1B, WGM12);
	CLEAR_BIT(TCCR1A, WGM11);		
	CLEAR_BIT(TCCR1A, WGM10);

	CLEAR_BIT(TCCR1B, CS12);	// Choose 64 PSC Mode
	SET_TWO_BITS(TCCR1B, CS11, CS10);

	SET_BIT(TIMSK1, OCIE1A);	// Compare Match with A
	SET_BIT(TIFR1, OCF1A);		// Flag set initially 
	OCR1A = TIMER1_CYC_FOR_5MILLISEC;	// 250 delay inside OCR1A (prescalar/frequency)* OCR1A 
}

void timer1_stop() {
	// Choose 0x00 PSC Mode
	CLEAR_BIT(TCCR1B, CS12);	
	CLEAR_TWO_BITS(TCCR1B, CS11, CS10);
}

void softwareTimer2 ()
{
	timer2++;
}

ISR(TIMER1_COMPA_vect) {
	(*timer1_callback)();
}

ISR(TIMER2_COMPA_vect) {
	(*timer2_callback)();
	// timer2_setCallback(timer2_callback);
}
