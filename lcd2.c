#include <msp430f5529.h>
#include "intrinsics.h"
#include "lcd.h"

#define LCD_I2C_ADDR  0x27

#define LCD_RS        0x01
#define LCD_RW        0x02
#define LCD_EN        0x04
#define LCD_BL        0x08

void initI2C_Master(void);
uint8_t i2cSend(uint8_t slaveAddr, uint8_t data);

uint8_t lcdWriteNibble(uint8_t nibble, uint8_t isChar);
uint8_t lcdWriteByte(uint8_t byte, uint8_t isChar);

void initI2C_Master(void)
{
    P3SEL |= BIT0 | BIT1;

    UCB0CTL1 |= UCSWRST;

    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;

    UCB0CTL1 = UCSSEL_2 | UCSWRST;

    UCB0BR0 = 10;
    UCB0BR1 = 0;

    UCB0CTL1 &= ~UCSWRST;
}

uint8_t i2cSend(uint8_t slaveAddr, uint8_t data)
{
    UCB0I2CSA = slaveAddr;

    UCB0CTL1 |= UCTR | UCTXSTT;

    while(!(UCB0IFG & UCTXIFG));

    UCB0TXBUF = data;

    while(UCB0CTL1 & UCTXSTT);

    UCB0CTL1 |= UCTXSTP;

    while(UCB0CTL1 & UCTXSTP);

    return 0;
}

uint8_t lcdWriteNibble(uint8_t nibble, uint8_t isChar)
{
    uint8_t ctrl;

    ctrl = LCD_BL;

    if(isChar)
        ctrl |= LCD_RS;

    i2cSend(LCD_I2C_ADDR,nibble | ctrl);

    i2cSend(LCD_I2C_ADDR,nibble | ctrl | LCD_EN);

    i2cSend(LCD_I2C_ADDR,nibble | ctrl);

    return 0;
}

uint8_t lcdWriteByte(uint8_t byte, uint8_t isChar)
{
    lcdWriteNibble(byte & 0xF0,isChar);

    lcdWriteNibble((byte << 4) & 0xF0,isChar);

    return 0;
}

void lcdInit(void)
{
    initI2C_Master();

    __delay_cycles(50000);

    lcdWriteNibble(0x30,0);
    __delay_cycles(20000);

    lcdWriteNibble(0x30,0);
    __delay_cycles(20000);

    lcdWriteNibble(0x30,0);
    __delay_cycles(20000);

    lcdWriteNibble(0x20,0);
    __delay_cycles(20000);

    lcdWriteByte(0x28,0);
    lcdWriteByte(0x0C,0);
    lcdWriteByte(0x06,0);
    lcdWriteByte(0x01,0);

    __delay_cycles(50000);
}

void lcdClear(void)
{
    lcdWriteByte(0x01,0);
    __delay_cycles(50000);
}

void lcdSetCursor(uint8_t linha, uint8_t coluna)
{
    uint8_t addr;

    if(linha == 0)
        addr = 0x80 + coluna;
    else
        addr = 0xC0 + coluna;

    lcdWriteByte(addr,0);
}

void lcdWrite(char *str)
{
    while(*str)
    {
        lcdWriteByte(*str,1);
        str++;
    }
}
