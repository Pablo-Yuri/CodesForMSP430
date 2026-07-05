#include <msp430.h>
#include <stdint.h>
#include "libs/gpio.h"

#define btex  2,2
#define ledex 3,1

void main(){
    WDTCTL = WDTPW | WDTHOLD;

    // Config LED
    pinWrite(ledex, 0);
    pinMode(ledex, output);

    // Config Botao
    pinMode(btex, inPullUp);
    // pinMode(2,0 , inPullUp);

    while (1) 
    {
        // if (!(pinRead(2, 1)))
        //     pinWrite(1, 0, 1);
        // else
        //     pinWrite(1, 0, 0);
        if(!(pinRead(btex)))
        {
            pinToggle(ledex);
            while (!(pinRead(btex)));
            debounce();
        }
    }
}