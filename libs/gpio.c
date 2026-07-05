#include <msp430.h>
#include <stdint.h>
#include "gpio.h"

// #define BIT(n)      (1 << n)
// #define REDLED      1,0
// #define GREENLED    4,7
// #define SW1         1,1 
// #define SW2         2,1 

// Exercicios
// criar libs
// pinMode(port, bit, mode)
// pinRead(port, bit)
// pinWrite(port, bit, value)

// typedef enum {input, output, inPullUp, inPullDown} pinMode_t;
// void    pinMode(uint8_t port, uint8_t bit, pinMode_t mode);
// uint8_t pinRead(uint8_t port, uint8_t bit);
// void    pinWrite(uint8_t port, uint8_t bit, uint8_t value);
// void    pinToggle(uint8_t port, uint8_t bit);
// void    debounce();


volatile uint8_t * PDIR[8] = {&P1DIR, &P1DIR, &P2DIR, &P3DIR, &P4DIR, &P5DIR, &P6DIR, &P7DIR};
volatile uint8_t * PREN[8] = {&P1REN, &P1REN, &P2REN, &P3REN, &P4REN, &P5REN, &P6REN, &P7REN};
volatile uint8_t * POUT[8] = {&P1OUT, &P1OUT, &P2OUT, &P3OUT, &P4OUT, &P5OUT, &P6OUT, &P7OUT};
volatile uint8_t * PIN[8] = {&P1IN, &P1IN, &P2IN, &P3IN, &P4IN, &P5IN, &P6IN, &P7IN};

// void main()
// {
//     WDTCTL = WDTPW | WDTHOLD;

//     pinWrite(GREENLED, 0);
//     pinMode(GREENLED, output);

//     pinMode(SW2, inPullUp);

//     while (1) 
//     {
//         // if (!(pinRead(2, 1)))
//         //     pinWrite(1, 0, 1);
//         // else
//         //     pinWrite(1, 0, 0);
//         if(!(pinRead(SW2)))
//         {
//             while (!(pinRead(SW2)));
//             pinToggle(GREENLED);
//             debounce();
//         }
//     }
// }


void pinMode(uint8_t port, uint8_t bit, pinMode_t mode)
{
    uint8_t mask = 1 << bit;
     
    if (mode == input)
    {
        *(PDIR[port]) &= ~mask;             // pino como entrada
    }

    if(mode == output)
    {
        *(PDIR[port]) |= mask;              // pino como Saída
    }

    if(mode == inPullUp)
    {
        *(PDIR[port]) &= ~mask;
        *(PREN[port]) |= mask;
        *(POUT[port]) |= mask;
    }

    if(mode == inPullDown)
    {
        *(PDIR[port]) &= ~mask;
        *(PREN[port]) |= mask;
        *(POUT[port]) &= ~mask;
    }
}

uint8_t pinRead(uint8_t port, uint8_t bit)
{
    return (*PIN[port] & BIT(bit));
}

void pinWrite(uint8_t port, uint8_t bit, uint8_t value)
{
    if(value)
    {
        *POUT[port] |= BIT(bit);
    } else 
        *POUT[port] &= ~BIT(bit);

}

void pinToggle(uint8_t port, uint8_t bit)
{
    *POUT[port] ^= BIT(bit);
}

void debounce()
{
    volatile int i = 20000;
    while (i--);
}
