#include <stdint.h>

void I2Cinit();
void lcdWriteNibble(uint8_t nibble, uint8_t isChar);
void lcdWriteByte(uint8_t byte, uint8_t isChar);
void lcdWrite(char *str);
void lcdInit();
void lcdClear();