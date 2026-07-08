#include "lcd.h"
#include <msp430.h>
#include <stdint.h>
#include "intrinsics.h"

#define LCD_I2C_ADDR 0x27 // talvez o seu seja 0x3F

#define LCD_RS      0x01    // 0: Comando, 1: Dados
#define LCD_EN      0x04    // Pulso de habilitação
#define LCD_BL      0x08    // Controle da luz de fundo

static void I2C_Init(void);
static uint8_t I2C_Send(uint8_t slaveAddr, uint8_t data);

static uint8_t LCD_WriteNibble(uint8_t nibble, uint8_t isChar);
static uint8_t LCD_WriteByte(uint8_t byte, uint8_t isChar);

static void I2C_Init(void)
{
    // Configura os pinos GPIO para I2C
    // UCB0: P3.0 = SDA, P3.1 = SCL
    P3SEL |= BIT0 | BIT1;                   // Atribui os pinos ao módulo I2C UCB0

    // UCB0CTL1 |= UCSWRST;                    // Reseta para iniciar a configuração
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;   // Mestre, I2C (MODE = 3), síncrono
    UCB0CTL1 = UCSSEL_2 | UCSWRST;          // Usa SMCLK (1 MHz), mantém reset
    
    UCB0BR0 = 10;                           // SMCLK/10 = 100 kHz
    UCB0BR1 = 0;
    
    UCB0CTL1 &= ~UCSWRST;                   // Limpa reset para terminar a configuração
}

static uint8_t I2C_Send(uint8_t slaveAddr, uint8_t data) {
    // 1. Escreva o endereço do escravo em I2CSA [cite: 161]
    UCB0I2CSA = slaveAddr;
    
    // 2. Faça TR=1 e STT=1 para requisitar um start como transmissor [cite: 162]
    UCB0CTL1 |= UCTR | UCTXSTT;
    
    // 3. Escreva o byte data em TXBUF quando o buffer de transmissão ficar vazio (TXIFG=1) para destravar o ciclo de ACK [cite: 163, 164]
    while (!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = data;
    
    // 4. Espere o ciclo de acknowledge observando quando STT volta para zero [cite: 165]
    while (UCB0CTL1 & UCTXSTT);
    
    // 5. Se o escravo respondeu (NACKIFG=0) [cite: 166]
    // if (!(UCB0IFG & UCNACKIFG)) {
    //     // a. Espere o byte data ser transmitido (TXIFG=1) [cite: 167]
    //     while (!(UCB0IFG & UCTXIFG));
    // }
    
    // 6. Por fim, envie um stop fazendo STP=1 [cite: 168]
    UCB0CTL1 |= UCTXSTP;
    
    // 7. Aguarde STP voltar para zero para garantir que o stop foi enviado [cite: 169]
    while (UCB0CTL1 & UCTXSTP);
    
    // 8. Retorne a variável que sinaliza se houve um ACK ou NACK (NACKIFG) [cite: 170]
    // if (UCB0IFG & UCNACKIFG) {
    //     return 1; // 1 sinaliza NACK (falha) [cite: 160]
    // } else {
    //     return 0; // 0 sinaliza ACK (sucesso) [cite: 160]
    // }
    return 0;
}

static void LCD_WriteNibble(uint8_t nibble, uint8_t isChar) {
    // Prepara os bits de controle:
    // isChar define se é comando (RS=0) ou caractere (RS=1)[cite: 325].
    // Mantemos o Backlight (LCD_BL) ligado e o R/W como 0 (já é 0 implicitamente na lógica).
    uint8_t ctrl = (isChar ? LCD_RS : 0) | LCD_BL;
    
    I2C_Send(LCD_I2C_ADDR, nibble | ctrl); 
    I2C_Send(LCD_I2C_ADDR, nibble | ctrl | LCD_EN); // Pulso de habilitação
    I2C_Send(LCD_I2C_ADDR, nibble | ctrl); 
}

static void LCD_WriteByte(uint8_t byte, uint8_t isChar) {
    // Envia o nibble mais significativo (D7-D4)
    LCD_WriteNibble(byte & 0xF0, isChar);
    
    // Envia o nibble menos significativo (D3-D0)
    LCD_WriteNibble((byte << 4) & 0xF0, isChar);

    __delay_cycles(1000); // Pequena pausa para garantir que o LCD processe o byte
}

void LCD_Init(void) {
    I2C_Init();
    
    // Enviar esse nibble três vezes para garantir que chegamos no modo de 8 bits 
    // Correção: Enviar 0x30 em vez de 3 para alinhar com os pinos D4-D7
    uint8_t nibble = 0x30;
    __delay_cycles(50000);

    LCD_WriteNibble(nibble, 0);
    __delay_cycles(20000);
    LCD_WriteNibble(nibble, 0);
    __delay_cycles(20000);
    LCD_WriteNibble(nibble, 0);
    __delay_cycles(20000);

    // Na quarta vez, enviar 2 para colocar no modo 4 bits [cite: 330]
    // Correção: Enviar 0x20 em vez de 2 para alinhar com os pinos D4-D7
    nibble = 0x20;
    LCD_WriteNibble(nibble, 0);
    __delay_cycles(20000);

    // Configurar display (display ligado, cursor piscando)
    uint8_t byte = 0x0F;
    LCD_WriteByte(byte, 0);
    __delay_cycles(20000);

    // Apagar tudo e levar o cursor para o início
    byte = 0x01;
    LCD_WriteByte(byte, 0);
    __delay_cycles(20000);
}

void LCD_Clear(void) {
    uint8_t byte = 0x01; // Comando para limpar o display
    LCD_WriteByte(byte, 0);
    __delay_cycles(20000); // Aguarda o comando ser processado
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address;

    address= (row == 0) ? 0x00 : 0xC0;
    address += col;

    LCD_WriteByte(address, 0);
}

void LCD_WriteChar(char c) {
    LCD_WriteByte((uint8_t)c, 1); // Envia o caractere como dado (RS=1)
}

void LCD_WriteString(const char* str) {
    while(*str != '\0') {
        LCD_WriteChar(*str++);
    }
}