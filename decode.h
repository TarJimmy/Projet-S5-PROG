#ifndef __DECODE_H__
#define __DECODE_H__
#include <stdint.h>

int shift(arm_core p, uint32_t instruction);

int condition(uint32_t cpsr, uint8_t cond);

#endif
