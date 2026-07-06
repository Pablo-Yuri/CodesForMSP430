#include <msp430.h>
#include <stdint.h>

volatile uint16_t ultimoTempo = 0;
volatile uint16_t largura = 0;
volatile uint16_t capturaCompleta = 0;

volatile uint8_t estado = 0;
volatile uint8_t bit = 0;

volatile uint32_t frame = 0;

volatile uint8_t comando = 0;
volatile uint8_t dadoPronto = 0;

volatile uint16_t pulsos[100];
volatile uint8_t indice = 0;

#define IR_IDLE        0
#define IR_LEADER      1
#define IR_RECEBENDO   2

void IR_Init(void)
{
    // P1.2 -> TA0.1
    P1DIR &= ~BIT2;
    P1SEL |= BIT2;

    TA0CTL = TASSEL__SMCLK |
             MC__CONTINUOUS |
             TACLR;

    TA0CCTL1 =
            CM_3 |         // captura subida e descida
            CCIS_0 |       // CCI1A
            CAP |
            SCS |
            CCIE;

    __enable_interrupt();
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    IR_Init();

    P1DIR &= ~BIT0;
    P1OUT |= BIT0;

    while(1)
    {
        if(dadoPronto)
        {
            dadoPronto = 0;

            switch(comando)
            {

            case 0x18:

                P1OUT ^= BIT0;

                break;

            case 0x52:

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

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    switch(__even_in_range(TA0IV,14))
    {

    case TA0IV_TACCR1:

        {
            uint16_t tempo = TA0CCR1;

            largura = tempo - ultimoTempo;

            ultimoTempo = tempo;

            switch(estado)
            {

            //--------------------------------------------------
            // Espera Leader
            //--------------------------------------------------

            case IR_IDLE:

                if(largura > 8500 && largura < 9500)
                {
                    estado = IR_LEADER;
                }

                break;

            //--------------------------------------------------
            // Espera espaço de 4,5 ms
            //--------------------------------------------------

            case IR_LEADER:

                if(largura > 4000 && largura < 5000)
                {
                    estado = IR_RECEBENDO;

                    bit = 0;

                    frame = 0;
                }
                else
                {
                    estado = IR_IDLE;
                }

                break;

            //--------------------------------------------------
            // Recebendo dados
            //--------------------------------------------------

            case IR_RECEBENDO:

                /*
                 * O protocolo NEC alterna:
                 *
                 * 560 LOW
                 * 560 HIGH -> 0
                 *
                 * ou
                 *
                 * 560 LOW
                 *1690 HIGH -> 1
                 */

                if(largura > 1300)
                {
                    frame |= (1UL<<bit);
                }

                bit++;

                if(bit>=32)
                {
                    comando = (frame>>16)&0xFF;

                    dadoPronto = 1;

                    estado = IR_IDLE;
                    __no_operation();
                }
                if(indice >= 68)
                {
                    capturaCompleta = 1;

                    __no_operation();      // <-- Breakpoint aqui
                }

                break;
            }

        }

        break;
    }
    pulsos[indice++] = largura;
}
