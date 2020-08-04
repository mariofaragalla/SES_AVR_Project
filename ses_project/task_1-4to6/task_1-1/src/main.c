#include <stdio.h>

#define GET_BIT(var, bit)  ((var>>bit) & 1)
#define SET_BIT(var, bit) var |= (1<<bit)
#define SET_TWO_BITS(var, bit1, bit2) var |= (1<<bit1) | (1<<bit2) 
#define CLEAR_BIT(var, bit) var &=~ (1<<bit)
#define CLEAR_TWO_BITS(var, bit1, bit2) var &=~ (1<<bit1) &~ (1<<bit2)
#define TOGGLE_BIT(var, bit) var ^= (1<<bit)
#define TOGGLE_TWO_BITS(var, bit1, bit2) var ^= (1<<bit1) ^ (1<<bit2)

int main(void) {

    // Set Bit 3 Manually without shifting (OR= with 1)
    uint8_t var_0 = 0b0000000 ;         // 0 
    var_0 |= 0b00001000;                // 8
    printf("%d \n", var_0);
    
    // Set Bit 4 and 6 with shifting
    var_0 = 0b00000000;                 // 0
    SET_TWO_BITS(var_0, 4, 6);          // 80
    printf("%d \n", var_0);         

    // Set Bit 4 and 6 without shifting
    var_0 = 0b00000000;
    var_0 |= 0b01010000;

    // Clear Bit 2 
    var_0 = 0b00000100;                 // 4
    CLEAR_BIT(var_0, 2);                // 0
    printf("%d \n", var_0);

    // Clear Bit 2 and 7
    var_0 = 0b10000100;                 // 132
    CLEAR_TWO_BITS(var_0, 2, 7);        // 0
    printf("%d \n",var_0);

    //Toggle bit 3
    var_0 = 0b00000000;                 // 0
    TOGGLE_BIT(var_0, 3);               // 8
    printf("%d \n", var_0);

    //Set bit 2 and clear bits 5 and 7 at the same time
    var_0 = 0b10100000;                         
    var_0 |= (1<<2);
    var_0 &=~ (1<<5) &~ (1<<7);
    printf("%d \n", var_0);             // 4

    //swap bits 3-5 and bits 0-2 
    var_0 = 0b10000101;             
    if(GET_BIT(var_0, 3) != GET_BIT(var_0, 5)) { TOGGLE_TWO_BITS(var_0, 3, 5); }
    if(GET_BIT(var_0, 0) != GET_BIT(var_0, 2)) { TOGGLE_TWO_BITS(var_0, 0, 2); }
    printf("%d \n", var_0);             // 133
}

