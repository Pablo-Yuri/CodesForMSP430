#include <msp430f5529.h>
#include "lcd.h"
#include "ir.h"

char hexTable[] = "0123456789ABCDEF";

void byteToHex(uint8_t valor, char *str)
{
    str[0] = hexTable[(valor >> 4) & 0x0F];
    str[1] = hexTable[valor & 0x0F];
    str[2] = 0;
}

int main(void)
{
    uint8_t cmd;

    char texto[3];

    WDTCTL = WDTPW | WDTHOLD;

    P1DIR |= BIT0;

    lcdInit();

    IR_Init();

    lcdClear();

    lcdWrite("IR READY");

    while(1)
    {
        if(IR_Available())
        {
            cmd = IR_GetCommand();

            byteToHex(cmd,texto);

            lcdClear();

            lcdSetCursor(0,0);
            lcdWrite("CMD:");

            lcdSetCursor(1,0);
            lcdWrite(texto);

            P1OUT ^= BIT0;
        }
    }
}
