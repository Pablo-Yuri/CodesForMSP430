#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>

void lcdInit(void);
void lcdClear(void);
void lcdWrite(char *str);
void lcdSetCursor(uint8_t linha, uint8_t coluna);

#endif
