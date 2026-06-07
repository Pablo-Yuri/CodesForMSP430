#include <msp430.h>

// Paradigma do super loop (laço infinito)

void main()
{
  // Botão S1
  P2DIR &= ~BIT1;         // Configura o pino como entrada
  P2REN |=  BIT1;         // Ativa o resistor 
  P2OUT |=  BIT1;         // de pull-up

  // LED está no pino P1.0
  P1OUT &= ~BIT0;         // Começa em 0
  P1DIR |=  BIT0;         // P1.0 => Saída
  
  while(1)
  {
    if(P2IN & BIT1)       // Lê o valor do pino P2.1
    {
      // Botão solto      
      P1OUT &= ~BIT0;     // Apaga o LED
    }
    else 
    {
      // Botão pressionado
      P1OUT |=  BIT0;     // Acende o LED
    }

  }

}
