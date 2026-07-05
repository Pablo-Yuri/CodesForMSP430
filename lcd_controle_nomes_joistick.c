#include <msp430.h>
#include <stdint.h>

// 1. Definição da lista com 5 frases
char* frases = {
    "1: Ola Mundo!",
    "2: Sist Micro",
    "3: MSP430 UnB",
    "4: UART e I2C",
    "5: Fim da lista"
};
int frase_atual = 0; 

// Constantes de Limiar do Joystick (V1 e V2)
#define V1 1500
#define V2 2500

// =================================================================
// FUNÇÕES DO LCD I2C (Mantidas iguais à Parte 1)
// =================================================================
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

void lcdWriteNibble(uint8_t nibble, uint8_t isChar) {
    uint8_t byte = (nibble << 4) | 0x08 | (isChar ? 1 : 0); 
    i2cSend(0x27, byte);         
    byte |= 0x04;                
    i2cSend(0x27, byte);         
    byte &= ~0x04;               
    i2cSend(0x27, byte);         
}

void lcdWriteByte(uint8_t byte, uint8_t isChar) {
    lcdWriteNibble(byte >> 4, isChar);   
    lcdWriteNibble(byte & 0x0F, isChar); 
}

void lcdWrite(char *str) {
    while(*str != '\0') { lcdWriteByte(*str++, 1); }
}

void lcdInit() {
    __delay_cycles(15000);       
    lcdWriteNibble(0x03, 0); __delay_cycles(5000);
    lcdWriteNibble(0x03, 0); __delay_cycles(100);
    lcdWriteNibble(0x03, 0);
    lcdWriteNibble(0x02, 0);     
    
    lcdWriteByte(0x28, 0);       
    lcdWriteByte(0x0C, 0);       
    lcdWriteByte(0x01, 0);       
    __delay_cycles(2000);        
    lcdWriteByte(0x06, 0);       
}

// =================================================================
// LEITURA DO ADC E PROGRAMA PRINCIPAL
// =================================================================

// Função para ler o canal A0 (P6.0) onde o Eixo X do Joystick está ligado
uint16_t adcRead_A0() {
    ADC12CTL0 &= ~ADC12ENC;              // Desativa ADC
    ADC12MCTL0 = ADC12INCH_0;            // Seleciona Canal A0
    ADC12CTL0 |= ADC12ENC | ADC12SC;     // Inicia conversão
    while((ADC12IFG & ADC12IFG0) == 0);  // Aguarda conversão
    return ADC12MEM0;                    // Retorna valor (0 a 4095)
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD; 

    // --- Configuração I2C (P3.0 e P3.1) ---
    P3SEL |= BIT0 | BIT1;               
    UCB0CTL1 |= UCSWRST;                
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC; 
    UCB0CTL1 = UCSSEL_2 | UCSWRST;      
    UCB0BR0 = 10;                       
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;               

    // --- Configuração ADC12 (P6.0/A0) ---
    P6SEL |= BIT0;                       // Seta P6.0 para função analógica
    ADC12CTL0 = ADC12ON | ADC12SHT0_2;   // Liga o ADC e define janela de amostragem de 16 clocks

    // --- Configuração Timer A0 para 100ms ---
    // Usando ACLK (32.768 Hz): 32768 * 0.1s = ~3276 ciclos
    TA0CCR0 = 3276;
    TA0CTL = TASSEL_1 | MC_1 | TACLR;    // ACLK, Modo Up, Limpa timer

    lcdInit();
    lcdWrite(frases[frase_atual]);

    // Estados lógicos do joystick: 0 = Neutro, 1 = Avança (Direita), -1 = Recua (Esquerda)
    int estado_joystick = 0;     
    int ultimo_estado = 0;       
    int atualiza_lcd = 0;
    uint16_t leitura_adc = 0;

    while(1) {
        // Aguarda a flag do Timer disparar (100ms)
        if (TA0CTL & TAIFG) {
            TA0CTL &= ~TAIFG; // Limpa a flag

            // Lê a posição atual do Joystick
            leitura_adc = adcRead_A0();

            // Identifica a zona em que o joystick está
            if (leitura_adc < V1) {
                estado_joystick = -1; // Esquerda
            } else if (leitura_adc > V2) {
                estado_joystick = 1;  // Direita
            } else {
                estado_joystick = 0;  // Neutro (Entre V1 e V2)
            }

            // Só processa a mudança se sair do Neutro para uma direção nova
            if (estado_joystick == -1 && ultimo_estado == 0) {
                frase_atual = (frase_atual == 0) ? 4 : frase_atual - 1;
                atualiza_lcd = 1;
            } 
            else if (estado_joystick == 1 && ultimo_estado == 0) {
                frase_atual = (frase_atual == 4) ? 0 : frase_atual + 1;
                atualiza_lcd = 1;
            }

            // Atualiza memória do estado
            ultimo_estado = estado_joystick;

            // Atualiza a tela se necessário
            if (atualiza_lcd) {
                lcdWriteByte(0x01, 0); // Limpa tela
                __delay_cycles(2000);  
                lcdWrite(frases[frase_atual]);
                atualiza_lcd = 0;
            }
        }
    }
}