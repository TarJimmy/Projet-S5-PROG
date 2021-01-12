#ifndef __DECODE_H__
#define __DECODE_H__
#include <stdint.h>

uint32_t shift(arm_core p, uint32_t instruction, uint32_t* shifter_carry_out);

int condition(uint32_t cpsr, uint8_t cond);

#endif
