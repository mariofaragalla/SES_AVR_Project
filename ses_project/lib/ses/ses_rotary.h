#ifndef ROTARY_H_
#define ROTARY_H_

typedef void (*pTypeRotaryCallback)();  //type definition for rotary button callbacks

/** 
  * initialize rotary button terminals A,B (left and right)
 */
void rotary_init();

/** 
  * set the callback for the rotary button for clockwise direction
  *      param : pTypeRotaryCallback is a pointer to the function to be called back
 */
void rotary_setClockwiseCallback(pTypeRotaryCallback);

/** 
  * set the callback for the rotary button for counter clockwise direction
  *      param : pTypeRotaryCallback is a pointer to the function to be called back
 */
void rotary_setCounterClockwiseCallback(pTypeRotaryCallback);

/**
 * Callback-procedure to plot the samples of the rotary pins on the LCD after first change
*/
void check_rotary();

/**
 * the callback for rotary_setClockwiseCallback function  
 * this function increments the counter in case of clockwise direction
*/
void rotaryIncrement();

/**
 * the callback for rotary_setCounterClockwiseCallback function 
 * this function decrements the counter in case of counter clockwise direction
*/
void rotaryDecrement(); 

/**
 *  returns the value of the signal of terminal (A-C)
*/
bool rotaryA_isPressed(void);

/**
 *  returns the value of the signal of terminal (B-C)
*/
bool rotaryB_isPressed(void);

/**
 *  checks the state of the rotary button
*/
void rotary_checkState(void);

#endif /* ROTARY_H_ */