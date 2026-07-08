#include <msp430.h>
#include "ir.h"

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    P1DIR |= BIT0; // Set P1.0 as output for LED
    P1OUT &= ~BIT0; // Turn off LED initially

    IR_Init();

    while(1)
    {
        if(dadoPronto)
        {
            dadoPronto = 0;

            switch(comando)
            {

            case 0x18:

                // cima
                P1OUT ^= BIT0;
                // lcdWrite("cima");

                break;

            case 0x52:

                // lcdWrite("baixo");
                // baixo

                break;

            case 0x08:

                // esquerda

                break;

            case 0x5A:

                // direita

                break;

            case 0x1C:

                // OK

                break;
            }

        }

    }
}