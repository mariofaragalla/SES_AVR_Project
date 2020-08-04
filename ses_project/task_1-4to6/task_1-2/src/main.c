#include <stdio.h>

#define GET_BIT(var, bit)  ((var>>bit) & 1)
#define TOGGLE_TWO_BITS(var, bit1, bit2) var ^= (1<<bit1) ^ (1<<bit2)

int main(void)
{
    uint8_t var_0 = 0b11010001;

    for(int i=0; i<4; i++)
    {
        if(GET_BIT(var_0, i) != GET_BIT(var_0, (7-i)))
        {
            TOGGLE_TWO_BITS(var_0, i, 7-i);
        }
    }

    printf("%d",var_0);
    return 0;
}