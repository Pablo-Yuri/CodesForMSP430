#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include "func_lcd.h"


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

uint8_t validarLogin(char *user, char *userValid, char *senha1, char *senhaValid) {
    if (strcmp(user, userValid) == 0 && strcmp(senha1, senhaValid) == 0) {
        return 1; // Login válido
    }
    return 0; // Login inválido
}
char arrayLogin[7] = "";
char arraySenha[7] = "";
char array[7] = "";
void lcd_array(uint8_t secret){
    uint8_t indice = 0;
    // uint8_t secret = 1; 
    while(1)
    {
        if(dadoPronto)
        // login--;
        {
            dadoPronto = 0;
            // lcdWrite(comando);

            switch(comando)
            {
            case 0x45:
                // 1
                if(secret)
                {
                    lcdWrite("*");
                    arraySenha[indice++] = '1';

                } else 
                {
                    lcdWrite("1");
                    arrayLogin[indice++] = '1';
                }
                // array[indice++] = '1';
                // indice++;
                break;
            case 0x46:
                // 2
                if(secret)
                {
                    lcdWrite("*");
                    arraySenha[indice++] = '2';
                } else 
                {
                    lcdWrite("2");
                    arrayLogin[indice++] = '2';
                }
                // array[indice++] = '2';
                // indice++;
                break;
            case 0x47:
                // 3
                if(secret)
                {
                    lcdWrite("*");
                    arraySenha[indice++] = '3';
                } else 
                {
                    lcdWrite("3");
                    arrayLogin[indice++] = '3';
                }
                // array[indice++] = '3';
                // indice++;
                break;
            case 0x44:
                // 4
                // lcdClear();
                if(secret)
                {
                    lcdWrite("*");
                    arraySenha[indice++] = '4';
                } else 
                {
                    lcdWrite("4");
                    arrayLogin[indice++] = '4';
                }
                // array[indice++] = '4';
                break;
            case 0x40:
                // 5
                // lcdClear();
                if(secret)
                {
                    lcdWrite("*");
                    arraySenha[indice++] = '5';
                } else 
                {
                    lcdWrite("5");
                    arrayLogin[indice++] = '5';
                }
                // array[indice++] = '5';
                break;
            case 0x43:
                if(secret)
                {
                    lcdWrite("*");
                    arraySenha[indice++] = '6';
                } else 
                {
                    lcdWrite("6");
                    arrayLogin[indice++] = '6';
                }
                // array[indice++] = '6';
                break;
            case 0x07:
                if(secret)
                {
                    lcdWrite("*");
                    arraySenha[indice++] = '7';
                } else 
                {
                    lcdWrite("7");
                    arrayLogin[indice++] = '7';
                }
                // array[indice++] = '7';
                break;
            case 0x15:
                if(secret)
                {
                    lcdWrite("*");
                    arraySenha[indice++] = '8';
                } else 
                {
                    lcdWrite("8");
                    arrayLogin[indice++] = '8';
                }
                // array[indice++] = '8';
                break;
            case 0x09:
                if(secret)
                {
                    lcdWrite("*");
                    arraySenha[indice++] = '9';
                } else 
                {
                    lcdWrite("9");
                    arrayLogin[indice++] = '9';
                }
                // array[indice++] = '9';
                break;
            case 0x19:
                if(secret)
                {
                    lcdWrite("*");
                    arraySenha[indice++] = '0';
                } else 
                {
                    lcdWrite("0");
                    arrayLogin[indice++] = '0';
                }
                // array[indice++] = '0';
                break;
            // case 0x16:
            //     lcdWrite("*");
            //     break;
            // case 0x0D:
            //     lcdWrite("#");
            //     break;

            // case 0x18:

            //     // cima
            //     P1OUT ^= BIT0;
            //     lcdClear();
            //     lcdWrite("cima");

            //     break;

            // case 0x52:
            //     lcdClear();
            //     lcdWrite("baixo");
            //     // baixo

            //     break;

            // case 0x08:

            //     // esquerda
            //     lcdClear();
            //     lcdWrite("esquerda");

            //     break;

            // case 0x5A:

            //     // direita
            //     lcdClear();
            //     lcdWrite("direita");

            //     break;

            // case 0x1C:

            //     // OK
            //     // lcdClear();
            //     // lcdWrite("OK");
            //     lcdSetCursor(1, 7);

            //     break;
            }

        }

        // tamarray = sizeof(login) / sizeof(login[0]);
        if (indice == 6)
        {
            array[indice] = '\0';
            break;
        }
    }
}

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    IR_Init();

    I2Cinit();
    lcdInit();
    lcdClear();
    // lcdWrite("PABLO YURI");

    // while(1);

    P1OUT &= ~BIT0;
    P1DIR |= BIT0;

    P4OUT &= ~BIT7;
    P4DIR |= BIT7;

    P2OUT &= ~BIT2;
    P2DIR |= BIT2;

    lcdWriteByte(0x01,0);               // 
    __delay_cycles(2000);
    
    lcdWriteByte(0x0F,0);

    lcdSetCursor(0,0);                  //  
    lcdWrite("Login:"); 
    arrayLogin[7] = '\0';

    lcdSetCursor(1,0);
    lcdWrite("Senha:");
    arraySenha[7] = '\0';

    lcdSetCursor(0, 7);

    lcd_array(0);

    lcdSetCursor(1, 7);
    lcd_array(1);

    lcdWriteByte(0x01,0);


    lcdSetCursor(0, 0);
    lcdWrite(arrayLogin); // Mostrar senha 
    lcdSetCursor(1, 0);
    lcdWrite(arraySenha); // Mostrar senha 
    char userValido[7] = "123456\0";
    char senhaValida[7] = "123456\0";

    if(validarLogin(arrayLogin, userValido, arraySenha, senhaValida))
    {
        lcdWriteByte(0x01,0);
        lcdSetCursor(0, 0);
        lcdWrite("LOGIN REALIZADO");
        P4OUT ^= BIT7;
        P2OUT ^= BIT2;
    }else 
    {
        lcdWriteByte(0x01,0);
        lcdSetCursor(0, 0);
        lcdWrite("LOGIN FALHOU");
    }
    // int login = 5;

    // if (dadoPronto)
    // {
    //     lcdSetCursor(1, 7);
    //     lcdSetCursor(1, 7);
    //     dadoPronto = 0;
    // }1

    // while(1)
    // {
    //     if(dadoPronto)
    //     // login--;
    //     {
    //         dadoPronto = 0;
    //         // lcdWrite(comando);

    //         switch(comando)
    //         {
    //         case 0x45:
    //             // lcdClear();
    //             // lcdSetCursor(1, 0);
    //             lcdWrite("1");
    //             break;
    //         case 0x46:
    //             // lcdClear();
    //             lcdWrite("2");
    //             break;
    //         case 0x47:
    //             // lcdClear();
    //             lcdWrite("3");
    //             break;
    //         case 0x44:
    //             // lcdClear();
    //             lcdWrite("4");
    //             break;
    //         case 0x40:
    //             // lcdClear();
    //             lcdWrite("5");
    //             break;
    //         case 0x43:
    //             lcdWrite("6");
    //             break;
    //         case 0x07:
    //             lcdWrite("7");
    //             break;
    //         case 0x15:
    //             lcdWrite("8");
    //             break;
    //         case 0x09:
    //             lcdWrite("9");
    //             break;
    //         case 0x19:
    //             lcdWrite("0");
    //             break;
    //         case 0x16:
    //             lcdWrite("*");
    //             break;
    //         case 0x0D:
    //             lcdWrite("#");
    //             break;

    //         case 0x18:

    //             // cima
    //             P1OUT ^= BIT0;
    //             lcdClear();
    //             lcdWrite("cima");

    //             break;

    //         case 0x52:
    //             lcdClear();
    //             lcdWrite("baixo");
    //             // baixo

    //             break;

    //         case 0x08:

    //             // esquerda
    //             lcdClear();
    //             lcdWrite("esquerda");

    //             break;

    //         case 0x5A:

    //             // direita
    //             lcdClear();
    //             lcdWrite("direita");

    //             break;

    //         case 0x1C:

    //             // OK
    //             // lcdClear();
    //             // lcdWrite("OK");
    //             lcdSetCursor(1, 7);

    //             break;
    //         }

    //     }

    //     // break;
    // }
    // lcdWrite("pablo");
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

