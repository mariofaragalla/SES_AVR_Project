#include "ses_common.h"
#include "defenitions.h"
#include "ses_adc.h"
#include <avr/io.h>
#include <util/delay.h>

#define ADC_VREF_SRC    1.6     // Max Voltage Reference
#define ADC_PRESCALE    8       // Prescaler chosen for 2MHz ADC

#define ADC_TEMP_MAX        4040     // 40 celusis = 313.15 kelvin then multiply by ADC_TEMP_FACTOR
#define ADC_TEMP_MIN        2020     // 20 celusis = 293.15 kelvin then multiply by ADC_TEMP_FACTOR
#define ADC_TEMP_RAW_MAX    256      // adc values corresponding to 40 celusis
#define ADC_TEMP_RAW_MIN    480      // adc values corresponding to 20 celusis
#define ADC_TEMP_FACTOR     101 
#define ADC_MUX_MASK        0x1F 

// ADC wiring on SES board
#define ADC_PIN         PINF
#define ADC_PORT        PORTF
#define ADC_DDR      	DDRF

extern int16_t myADC ; 


void adc_init(void)    //Intializes Temperature sensor, light sensor, joystick and Microphone 
{
    // Configure Temperature sensor , light sensor, joystick and Microphone pins to be inputs
    CLEAR_BIT(DDR_REGISTER(ADC_DDR), ADC_TEMP_CH);
    CLEAR_BIT(DDR_REGISTER(ADC_DDR), ADC_LIGHT_CH);
    CLEAR_BIT(DDR_REGISTER(ADC_DDR), ADC_JOYSTICK_CH);
    CLEAR_BIT(DDR_REGISTER(ADC_DDR), ADC_MIC_NEG_CH);
    CLEAR_BIT(DDR_REGISTER(ADC_DDR), ADC_MIC_POS_CH);

    // Deactivate pull up resistors for Temperature sensor , light sensor, joystick and Microphone pins to be inputs
    CLEAR_BIT(PIN_REGISTER(ADC_PORT), ADC_TEMP_CH);
    CLEAR_BIT(PIN_REGISTER(ADC_PORT), ADC_LIGHT_CH);
    CLEAR_BIT(PIN_REGISTER(ADC_PORT), ADC_JOYSTICK_CH);
    CLEAR_BIT(PIN_REGISTER(ADC_PORT), ADC_MIC_NEG_CH);
    CLEAR_BIT(PIN_REGISTER(ADC_PORT), ADC_MIC_POS_CH);

    CLEAR_BIT(PRR0, PRADC);                     // Disable power reduction mode
    SET_TWO_BITS(ADMUX, REFS0, REFS1);          // Set internal Voltage Ref to 1.6V
    CLEAR_BIT(ADMUX, ADLAR);                    // Right adjustment for the ADLAR Bit

    //set the prescalar to 8 by making ADPS2,ADPS1,ADPS0 equal to 100
    //(100 -> PSC = 8 for 16MHz/8 = 2MHz)
    CLEAR_BIT(ADCSRA, ADPS0);                  
    CLEAR_BIT(ADCSRA, ADPS1);                  
    SET_BIT(ADCSRA, ADPS2);                    

    CLEAR_BIT(ADCSRA, ADATE);                   // AutoTrigger is disabled
    SET_BIT(ADCSRA, ADEN);                      // Enable ADC
}

int16_t adc_read(uint8_t adc_channel)
{   
    if(adc_channel < ADC_MIC_NEG_CH || adc_channel >= ADC_NUM){
        return ADC_INVALID_CHANNEL;
    }

    ADMUX = (ADMUX & ~ADC_MUX_MASK) | (adc_channel & ADC_MUX_MASK); // select ADC channel

    SET_BIT(ADCSRA, ADSC);                  // ADC Start Conversion until ADC finishes reading   
    while(GET_BIT(ADCSRA, ADSC)){}     // Keep reading until the ADC stops conversion 
                                            // (ADSC is cleared automatically)
    return (ADC);                           // reads the whole ADC 16 bits ADCL and ADCH
}

uint8_t adc_getJoystickDirection()
{
return 0;
}

int16_t adc_getTemperature()    
{
    int16_t adc = adc_read(ADC_TEMP_CH);                    
    int16_t slope = (ADC_TEMP_MAX - ADC_TEMP_MIN) / (ADC_TEMP_RAW_MAX - ADC_TEMP_RAW_MIN);  
    int16_t offset = ADC_TEMP_MAX - (ADC_TEMP_RAW_MAX * slope);
    return ((adc * slope) + offset) / ADC_TEMP_FACTOR;
}

int16_t adc_getLight()
{
    int16_t adc = adc_read(ADC_LIGHT_CH);
    return adc;
}