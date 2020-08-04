#define GET_BIT(var, bit)                   ((var>>bit) & 1)
#define SET_BIT(var, bit)                   var |= (1<<bit)
#define SET_TWO_BITS(var, bit1, bit2)       var |= (1<<bit1) | (1<<bit2) 
#define CLEAR_BIT(var, bit)                 var &=~ (1<<bit)
#define CLEAR_TWO_BITS(var, bit1, bit2)     var &=~ (1<<bit1) &~ (1<<bit2)
#define TOGGLE_BIT(var, bit)                var ^= (1<<bit)
#define TOGGLE_TWO_BITS(var, bit1, bit2)    var ^= (1<<bit1) ^ (1<<bit2)

#define HIGH    1
#define LOW     0

void DEL(uint16_t milli);       // Function that will be used instead of predefined delay_ms()
void timer_init(void);