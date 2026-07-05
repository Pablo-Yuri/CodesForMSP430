#include <msp430.h>
#include "libs/types.h"
#include <string.h>

void uartPrint(char * string)
{
  
  while(*string)
  {
    // Espera a flag de TX
    while(!(UCA1IFG & UCTXIFG));
    // Escreve no TXBUF
    UCA1TXBUF = *string++;
  }
}

void uartDMA(char * string)
{
  DMACTL0 = DMA0TSEL__USCIA1TX;
  DMA0SA  = string;
  DMA0DA  = &UCA1TXBUF;
  DMA0SZ  = strlen(string);
  DMA0CTL = DMADT_0        |
            DMASRCINCR_3   |
            DMADSTINCR_0   |
            DMASRCBYTE     |
            DMADSTBYTE     |
            DMALEVEL       |
            DMAEN;

  // Inicia a copia
  //DMA0CTL |= DMAREQ;
}

void main()
{
  WDTCTL = WDTPW | WDTHOLD;
  
  // Configurar para 9600 bps
  // SMCLK @ 2^20
  // Divisor F_CLK / Baudrate = 2^20 / 9600 = 109,23 (BRW=109 | BRS=2)
  // Configuração usando OS: 2^20 / 9600 / 16 = (BRW=6 | BRF=13)

  P4SEL |= BIT4 | BIT5;        // Conecta P4.[4,5] na USCI-A1
  UCA1CTL1  =  UCSWRST;        // Reseta a interface
  UCA1CTL1 |=  UCSSEL__SMCLK;  // Usa SMCLK

  
  // UCA1BRW = 109;
  // UCA1MCTL = UCBRS_2 | UCOS16_1;

  // UCA1BRW = 6;
  // UCA1MCTL = UCOS16 | UCBRF_13;

  // A 19200 bps
  UCA1BR0 = 3;                  // Divisor = 3
  UCA1BR1 = 0;
   UCA1MCTL = UCBRS_1 | UCBRF_6 | UCOS16;

  UCA1CTL1 &= ~UCSWRST;         // CTL0 fica no default
  
  // LSB-first, sem paridade e 1 stop.

  __delay_cycles(10000);

  // uartDMA("Hello World!\n");
  uartPrint("Pablo Yuri\n");


  while(1)                // Loop infinito
  {

  }
}
