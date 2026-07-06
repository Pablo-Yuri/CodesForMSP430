#include "lcd.h"
#include <msp430f5529.h>
#include <stdint.h>
#include "intrinsics.h"

#define LCD_I2C_ADDR   0x27

#define LCD_RS         0x01
#define LCD_EN         0x04
#define LCD_BL         0x08

static void I2C_Init(void);
static uint8_t I2C_Send(uint8_t addr, uint8_t data);

static void LCD_WriteNibble(uint8_t nibble, uint8_t isChar);
static void LCD_WriteByte(uint8_t byte, uint8_t isChar);

static void I2C_Init(void)
{
    P3SEL |= BIT0 | BIT1;

    UCB0CTL1 |= UCSWRST;

    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;
    UCB0CTL1 = UCSSEL_2 | UCSWRST;

    UCB0BR0 = 10;
    UCB0BR1 = 0;

    UCB0CTL1 &= ~UCSWRST;
}

static uint8_t I2C_Send(uint8_t addr, uint8_t data)
{
    UCB0I2CSA = addr;

    UCB0CTL1 |= UCTR | UCTXSTT;

    while(!(UCB0IFG & UCTXIFG));

    UCB0TXBUF = data;

    while(UCB0CTL1 & UCTXSTT);

    UCB0CTL1 |= UCTXSTP;

    while(UCB0CTL1 & UCTXSTP);

    return 0;
}

static void LCD_WriteNibble(uint8_t nibble, uint8_t isChar)
{
    uint8_t ctrl;

    ctrl = (isChar ? LCD_RS : 0) | LCD_BL;

    I2C_Send(LCD_I2C_ADDR, nibble | ctrl);
    I2C_Send(LCD_I2C_ADDR, nibble | ctrl | LCD_EN);
    I2C_Send(LCD_I2C_ADDR, nibble | ctrl);
}

static void LCD_WriteByte(uint8_t byte, uint8_t isChar)
{
    LCD_WriteNibble(byte & 0xF0, isChar);
    LCD_WriteNibble((byte << 4) & 0xF0, isChar);

    __delay_cycles(1000);
}

void LCD_Init(void)
{
    I2C_Init();

    __delay_cycles(50000);

    LCD_WriteNibble(0x30,0);
    __delay_cycles(20000);

    LCD_WriteNibble(0x30,0);
    __delay_cycles(20000);

    LCD_WriteNibble(0x30,0);
    __delay_cycles(20000);

    LCD_WriteNibble(0x20,0);
    __delay_cycles(20000);

    LCD_WriteByte(0x28,0);
    LCD_WriteByte(0x0C,0);
    LCD_WriteByte(0x06,0);
    LCD_WriteByte(0x01,0);

    __delay_cycles(20000);
}

void LCD_Clear(void)
{
    LCD_WriteByte(0x01,0);
    __delay_cycles(20000);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t addr;

    addr = (row == 0) ? 0x80 : 0xC0;
    addr += col;

    LCD_WriteByte(addr,0);
}

void LCD_WriteChar(char c)
{
    LCD_WriteByte((uint8_t)c,1);
}

void LCD_WriteString(char *str)
{
    while(*str)
    {
        LCD_WriteChar(*str++);
    }
}
``
