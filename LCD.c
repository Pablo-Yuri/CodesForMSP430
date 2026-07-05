#include <msp430f5529.h> 

// Biblioteca para usar __delay_cycles
#include "intrinsics.h"

// Esta biblioteca permite usar o tipo uint8_t
#include <stdint.h>

// Endereço I2C do módulo LCD
#define LCD_I2C_ADDR 0x27 // talvez o seu seja 0x3F


// Definição dos pinos no PCF8574
// P7-P4: D7-D4 (dados)
// P3: BL (backlight)
// P2: EN (enable)
// P1: RW (read/write)
// P0: RS (register select)
#define LCD_RS      0x01    // 0: Comando, 1: Dados
#define LCD_RW      0x02    // 0: Escrita, 1: Leitura
#define LCD_EN      0x04    // Pulso de habilitação
#define LCD_BL      0x08    // Controle da luz de fundo
#define LCD_D4      0x10    // Bit de dados 4
#define LCD_D5      0x20    // Bit de dados 5
#define LCD_D6      0x40    // Bit de dados 6
#define LCD_D7      0x80    // Bit de dados 



// Protótipos das funções
void initI2C_Master(void);
uint8_t i2cSend(uint8_t slaveAddr, uint8_t data);
uint8_t lcdWriteNibble (uint8_t nibble, uint8_t isChar);
uint8_t lcdWriteByte (uint8_t byte, uint8_t isChar);
void lcdInit(void);
void lcdWrite(char* str);


void main(void){
	WDTCTL = WDTPW | WDTHOLD;	
	
	// Configura os pinos GPIO para I2C
    // UCB0: P3.0 = SDA, P3.1 = SCL
    P3SEL |= BIT0 | BIT1;                   // Atribui os pinos ao módulo I2C UCB0

    // Inicializa o módulo I2C
    initI2C_Master();

    // Esperar um pouco antes de realizar outra ação
    __delay_cycles(20000);

    // Inicializar LCD
    lcdInit();

    // Enviar string para ser escrita
    lcdWrite("Pablo Yuri");

    // Loop infinito
	for(;;){

	}
}

void initI2C_Master(void) {
    UCB0CTL1 |= UCSWRST;                    // Reseta para iniciar a configuração
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;   // Mestre, I2C (MODE = 3), síncrono
    UCB0CTL1 = UCSSEL_2 | UCSWRST;          // Usa SMCLK (1 MHz), mantém reset
    UCB0BR0 = 10;                           // SMCLK/10 = 100 kHz
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;                   // Limpa reset para terminar a configuração
}

uint8_t i2cSend(uint8_t slaveAddr, uint8_t data) {
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
    if (!(UCB0IFG & UCNACKIFG)) {
        // a. Espere o byte data ser transmitido (TXIFG=1) [cite: 167]
        while (!(UCB0IFG & UCTXIFG));
    }
    
    // 6. Por fim, envie um stop fazendo STP=1 [cite: 168]
    UCB0CTL1 |= UCTXSTP;
    
    // 7. Aguarde STP voltar para zero para garantir que o stop foi enviado [cite: 169]
    while (UCB0CTL1 & UCTXSTP);
    
    // 8. Retorne a variável que sinaliza se houve um ACK ou NACK (NACKIFG) [cite: 170]
    if (UCB0IFG & UCNACKIFG) {
        return 1; // 1 sinaliza NACK (falha) [cite: 160]
    } else {
        return 0; // 0 sinaliza ACK (sucesso) [cite: 160]
    }
}

uint8_t lcdWriteNibble (uint8_t nibble, uint8_t isChar) {
    
    // Prepara os bits de controle:
    // isChar define se é comando (RS=0) ou caractere (RS=1)[cite: 325].
    // Mantemos o Backlight (LCD_BL) ligado e o R/W como 0 (já é 0 implicitamente na lógica).
    uint8_t controlBits = (isChar ? LCD_RS : 0) | LCD_BL;
    
    // 1. Prepare RS (isChar) e D[7:4] com EN=0 e R/W=0 (write) [cite: 322]
    uint8_t step1 = nibble | controlBits;
    i2cSend(LCD_I2C_ADDR, step1);
    
    // 2. Em seguida mantenha R/W, RS e D[7:4] constantes e faça EN=1 [cite: 323]
    uint8_t step2 = nibble | controlBits | LCD_EN;
    i2cSend(LCD_I2C_ADDR, step2);
    
    // 3. Por fim, faça EN voltar a 0 mantendo os outros campos constantes [cite: 324]
    uint8_t step3 = nibble | controlBits; 
    i2cSend(LCD_I2C_ADDR, step3);
    
    return 0; 
}

uint8_t lcdWriteByte (uint8_t byte, uint8_t isChar){
    
    // 1. Separa os 4 bits mais significativos (High Nibble)
    // Usamos a máscara 0xF0 (11110000) para isolar apenas os 4 bits superiores.
    uint8_t msn = (byte & 0xF0); 
    
    // 2. Separa os 4 bits menos significativos (Low Nibble)
    // Deslocamos os 4 bits inferiores para a esquerda para que eles assumam 
    // as posições D4-D7 e zeramos o resto com a máscara 0xF0.
    uint8_t lsn = ((byte << 4) & 0xF0);

    // 3. Envia o primeiro nibble (MSB)
    lcdWriteNibble(msn, isChar);
    
    // 4. Envia o segundo nibble (LSB)
    lcdWriteNibble(lsn, isChar);

    return 0; // Retorna 0 indicando que a operação foi concluída
}

void lcdInit (void){
    // Enviar esse nibble três vezes para garantir que chegamos no modo de 8 bits 
    // Correção: Enviar 0x30 em vez de 3 para alinhar com os pinos D4-D7
    uint8_t nibble = 0x30;

    lcdWriteNibble(nibble, 0);
    __delay_cycles(20000);
    lcdWriteNibble(nibble, 0);
    __delay_cycles(20000);
    lcdWriteNibble(nibble, 0);
    __delay_cycles(20000);

    // Na quarta vez, enviar 2 para colocar no modo 4 bits [cite: 330]
    // Correção: Enviar 0x20 em vez de 2 para alinhar com os pinos D4-D7
    nibble = 0x20;
    lcdWriteNibble(nibble, 0);
    __delay_cycles(20000);

    // Configurar display (display ligado, cursor piscando)
    uint8_t byte = 0x0F;
    lcdWriteByte(byte, 0);
    __delay_cycles(20000);

    // Apagar tudo e levar o cursor para o início
    byte = 0x01;
    lcdWriteByte(byte, 0);
    __delay_cycles(20000);

}

void lcdWrite(char* str){
    uint8_t count = 0; // Contador de caracteres na linha atual

    // O laço continua até encontrar o byte 0x00 (terminador de string)
    while(*str != '\0') {
        
        // Verifica se há o caractere de nova linha ou se a linha 1 encheu (16 chars)
        if(*str == '\n' || count == 16) {
            
            // Envia o comando 0xC0 para mover o cursor para o início da 2ª linha.
            // O parâmetro isChar é 0 pois se trata de uma instrução de controle.
            lcdWriteByte(0xC0, 0); 
            
            // Zera o contador de colunas
            count = 0; 

            // Se a quebra de linha foi causada por um '\n', pulamos este 
            // caractere para não tentar imprimi-lo no display.
            if(*str == '\n') {
                str++;
                continue;
            }
        }

        // Escreve o caractere atual no LCD. 
        // O parâmetro isChar é 1 pois estamos enviando dados visuais.
        lcdWriteByte(*str, 1);
        
        // Avança para o próximo caractere da string e incrementa o contador da linha
        str++;
        count++;
    }
}



