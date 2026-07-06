#ifndef IR_H_
#define IR_H_

#include <stdint.h>

void IR_Init(void);

uint8_t IR_Available(void);

uint8_t IR_GetCommand(void);

#endif
