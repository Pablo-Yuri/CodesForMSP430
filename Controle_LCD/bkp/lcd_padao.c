#include <msp430.h>
#include <stdint.h>

// Definições dos Pinos dos Botões da Launchpad
#define S1 BIT1 // Botão S1 no P2.1
#define S2 BIT1 // Botão S2 no P1.1

void I2Cinit();
void lcdWriteNibble(uint8_t nibble, uint8_t isChar);
void lcdWriteByte(uint8_t byte, uint8_t isChar);
void lcdWrite(char *str);
void lcdInit();
void lcdClear();


void I2Cinit(){
    // --- Configuração da Interface I2C (USCI_B0) ---
    P3SEL |= BIT0 | BIT1;               // Pinos P3.0 (SDA) e P3.1 (SCL)
    UCB0CTL1 |= UCSWRST;                // Reseta a interface
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC; // Mestre, I2C, Síncrono
    UCB0CTL1 = UCSSEL_2 | UCSWRST;      // Usa SMCLK
    UCB0BR0 = 10;                       // Clock I2C (~100kHz)
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;               // Libera interface
}

// Envia um byte pela interface I2C para o endereço do PCF8574
uint8_t i2cSend(uint8_t addr, uint8_t data) {
    UCB0I2CSA = addr;                 
    UCB0CTL1 |= UCTR | UCTXSTT;       
    while(!(UCB0IFG & UCTXIFG));      
    UCB0TXBUF = data;                 
    while(UCB0CTL1 & UCTXSTT);        
    
    if(UCB0IFG & UCNACKIFG) {         
        UCB0CTL1 |= UCTXSTP;          
        while(UCB0CTL1 & UCTXSTP);
        return 1;                     
    }
    
    while(!(UCB0IFG & UCTXIFG));      
    UCB0CTL1 |= UCTXSTP;              
    while(UCB0CTL1 & UCTXSTP);        
    return 0;                         
}

// Envia um nibble (4 bits) para o LCD
void lcdWriteNibble(uint8_t nibble, uint8_t isChar) {
    uint8_t byte = (nibble << 4) | 0x08 | (isChar ? 1 : 0); 
    i2cSend(0x27, byte);         // Prepara os dados (EN=0)
    byte |= 0x04;                // Seta EN=1
    i2cSend(0x27, byte);         // Pulso EN Alto
    byte &= ~0x04;               // Zera EN=0
    i2cSend(0x27, byte);         // Pulso EN Baixo
}

// Envia um byte completo dividindo-o em dois nibbles
void lcdWriteByte(uint8_t byte, uint8_t isChar) {
    lcdWriteNibble(byte >> 4, isChar);   
    lcdWriteNibble(byte & 0x0F, isChar); 
}

// Escreve uma string no LCD
void lcdWrite(char *str) {
    while(*str != '\0') {        
        lcdWriteByte(*str++, 1); 
    }
}

// Rotina de Inicialização do LCD em modo 4 bits
void lcdInit() {
    __delay_cycles(15000);       
    lcdWriteNibble(0x03, 0); __delay_cycles(5000);
    lcdWriteNibble(0x03, 0); __delay_cycles(100);
    lcdWriteNibble(0x03, 0);
    lcdWriteNibble(0x02, 0);     // Altera para modo 4 bits
    
    lcdWriteByte(0x28, 0);       // Configura 2 linhas, fonte 5x8
    lcdWriteByte(0x0C, 0);       // Liga o display, desliga o cursor
    lcdWriteByte(0x01, 0);       // Limpa o display
    __delay_cycles(2000);        // Aguarda execução do comando (1,53ms)
    lcdWriteByte(0x06, 0);       // Incremento de cursor
}

void lcdClear() {
    lcdWriteByte(0x01, 0);       // Limpa o display
    __delay_cycles(2000);        // Aguarda execução do comando (1,53ms)
}

// =================================================================
// FUNÇÃO PRINCIPAL
// =================================================================

void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Interrompe o Watchdog Timer


    // --- Inicialização e Tela Inicial ---
    I2Cinit();
    lcdInit();
    lcdClear();
    lcdWrite("PABLO YURI");

}