#include <msp430.h>
#include <stdint.h>

// Definições dos Pinos dos Botões da Launchpad
#define S1 BIT1 // Botão S1 no P2.1
#define S2 BIT1 // Botão S2 no P1.1

// 1. Definição da lista com 5 frases arbitrárias
char* frases[7] = {
    "1: Ola Mundo!",
    "2: Sist Micro",
    "3: MSP430 UnB",
    "4: UART e I2C",
    "5: Fim da lista"
};
int frase_atual = 0; // Índice da frase sendo exibida (0 a 4)

// =================================================================
// FUNÇÕES DE CONTROLE DO LCD VIA I2C (USCI_B0)
// =================================================================

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

// =================================================================
// FUNÇÃO PRINCIPAL
// =================================================================

void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Interrompe o Watchdog Timer

    // --- Configuração da Interface I2C (USCI_B0) ---
    P3SEL |= BIT0 | BIT1;               // Pinos P3.0 (SDA) e P3.1 (SCL)
    UCB0CTL1 |= UCSWRST;                // Reseta a interface
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC; // Mestre, I2C, Síncrono
    UCB0CTL1 = UCSSEL_2 | UCSWRST;      // Usa SMCLK
    UCB0BR0 = 10;                       // Clock I2C (~100kHz)
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;               // Libera interface

    // --- Configuração dos Botões S1 (P2.1) e S2 (P1.1) ---
    // Configura os pinos como entrada e habilita resistores de pull-up
    P2DIR &= ~S1; P2REN |= S1; P2OUT |= S1;
    P1DIR &= ~S2; P1REN |= S2; P1OUT |= S2;

    // --- Inicialização e Tela Inicial ---
    lcdInit();
    lcdWrite(frases[frase_atual]);

    // Variáveis de estado para detecção de borda
    int s1_estado = 1, s2_estado = 1;
    int s1_ultimo = 1, s2_ultimo = 1;
    int atualiza_lcd = 0;

    while(1) {
        // Lê o estado atual das chaves (1 = Solto, 0 = Pressionado)
        s1_estado = (P2IN & S1) ? 1 : 0;
        s2_estado = (P1IN & S2) ? 1 : 0;

        // 2. Transição S1 (Aberta -> Fechada)
        if (s1_ultimo == 1 && s1_estado == 0) {
            __delay_cycles(20000); // 20ms de Debounce
            if ((P2IN & S1) == 0) { // Confirma que ainda está pressionado
                // Volta uma frase (se estiver no índice 0, vai para o 4)
                frase_atual = (frase_atual == 0) ? 4 : frase_atual - 1;
                atualiza_lcd = 1;
            }
        }

        // 3. Transição S2 (Aberta -> Fechada)
        if (s2_ultimo == 1 && s2_estado == 0) {
            __delay_cycles(20000); // 20ms de Debounce
            if ((P1IN & S2) == 0) { // Confirma que ainda está pressionado
                // Avança uma frase (se estiver no índice 4, vai para o 0)
                frase_atual = (frase_atual == 4) ? 0 : frase_atual + 1;
                atualiza_lcd = 1;
            }
        }

        // Salva os estados para a próxima iteração
        s1_ultimo = s1_estado;
        s2_ultimo = s2_estado;

        // 5. Atualiza o LCD se alguma das chaves disparou a transição
        if (atualiza_lcd) {
            lcdWriteByte(0x01, 0); // Envia o comando para limpar a tela
            __delay_cycles(2000);  // Deve-se aguardar no mínimo 1,53ms 
            lcdWrite(frases[frase_atual]);
            atualiza_lcd = 0;
        }
    }
}