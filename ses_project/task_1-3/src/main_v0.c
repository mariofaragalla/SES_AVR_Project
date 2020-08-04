#include <avr/io.h>
#include <util/delay.h>

//Defining constants:
#define SET_DDG1    0x02        // Set bit 1 to output in DDRG
#define SET_PORTG1  0x02        // Set bit 1 to output in PORTG
#define FREQUENCY   16000000    // Microprocessor frequency 16MHz
#define CYCLES      7           // Number of cycles corresponding to one loop instructions(nop,sbiw,sbc(x2),brne,in)
#define CONV_TO_SEC 1000        // Used to convert from milliseconds to seconds
#define TRUE        1

void wait(uint16_t);        // Prototype for the wait function

int main(void)
{
    uint16_t milis = 1000;  // Milli seconds (1000 milis = 1 second)
    DDRG |= SET_DDG1;       // Configure PORTG1 as an output

    while (TRUE)
    {
        wait(milis);         // Delay for milis seconds
        PORTG ^= SET_PORTG1; // Toggle PORTG1 which is connected to the red LED
    }

    return 0;
}

/*  wait for 16000000 cycles (1 second) multiplied by the required seconds.
    i.e. This loop consumes 7 cycles when executed , and 16000000 cycles are needed to pass one second, 
    so 16000000/7 will give the number of times this loop needs to execute to make one second pass, and 
    since we take milli seconds as an input we have to converted to seconds by diving by 1000. */
void wait(uint16_t millis)
{
    for (uint32_t i=(FREQUENCY/CYCLES*CONV_TO_SEC)*millis; i>0; i--)
    {
        asm volatile("nop"); // one cycle with no operation
    }
}
