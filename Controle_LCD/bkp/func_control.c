#include <msp430.h>
#include <stdint.h>
// #include "lcd.c"

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
// #define IR_LEADER      1
#define IR_RECEBENDO   1

void IR_Init(void)
{
    // P1.2 -> TA0.1
    P1DIR &= ~BIT2;
    P1SEL |= BIT2;

    TA0CTL = TASSEL__SMCLK |
             MC__CONTINUOUS |
             TACLR;

    TA0CCTL1 =
            CM_2 |         // captura subida e descida
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
    P1OUT &= ~BIT0;
    P1DIR |= BIT0;

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
        uint16_t tempo;
        tempo = TA0CCR1;

        largura = tempo - ultimoTempo;

        ultimoTempo = tempo;

        if(indice < 100)
        {
            pulsos[indice++] = largura;
        }

        switch(estado)
        {

            //--------------------------------------------------
            // Espera Leader
            //--------------------------------------------------

            case IR_IDLE:
                if(largura > 12500 && largura < 14500)
                {
                    indice = 0;

                    pulsos[indice++] = largura;

                    bit = 0;
                    frame = 0;

                    estado = IR_RECEBENDO;
                }
                break;

            //--------------------------------------------------
            // Recebendo dados
            //--------------------------------------------------

            case IR_RECEBENDO:
                if(indice < 100)
                {
                    pulsos[indice++] = largura;
                }
                if((largura > 1800) && (largura < 2800))
                    {
                        frame |= (1UL << bit);
                    }
                else if ((largura > 900) && (largura < 1500)) {
                    // bit 0
                }
                else {
                    estado = IR_IDLE;
                    break;
                }

                bit++;
                // break;

                if(bit >= 32)
                {
                    uint8_t endereco;
                    uint8_t enderecoInv;
                    uint8_t cmd;
                    uint8_t cmdInv;

                    endereco        = (frame >> 0) & 0xFF;
                    enderecoInv     = (frame >> 8) & 0xFF;
                    cmd             = (frame >> 16) & 0xFF;
                    cmdInv          = (frame >> 24) & 0xFF;

                    if(((endereco ^ enderecoInv) == 0xFF) &&
                      ((cmd ^ cmdInv) == 0xFF))
                      {
                        comando = cmd;
                        dadoPronto = 1;
                      }
                      estado = IR_IDLE;
                }
                break;
            }
            break;
        }
        default:
            break;
    }
}

