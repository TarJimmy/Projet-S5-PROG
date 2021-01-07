#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include "util.h"
#include "arm_core.h"
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "debug.h"
			


int shift(arm_core p, uint32_t instruction){
	uint8_t shift = get_bits(instruction, 6, 5);
    uint8_t offset = get_bits(instruction, 3, 0);
    uint8_t bit_4 = get_bit(instruction, 4); //(shift) 0=valeur_immédiate 1=registre
    uint8_t valeur = 0;
    if (bit_4) {  
    	uint8_t Rs = get_bits(instruction, 11, 8);
    	valeur = arm_read_register(p, Rs);
    } else { valeur = get_bits(instruction, 11, 7); }
    switch (shift) {
        case 0: //LSL
            offset = offset << valeur;
            break;
        case 1: //LSR
            if (valeur == 0) {
                offset = 0;
            } else {
                offset = offset >> valeur;
            }
            break;
        case 2: //ASR
            if (valeur == 0) {
                asr(offset, 32);
            } else {
                offset = offset >> valeur;
            }
            break;
        case 3: 
            if (valeur ==0) {
                offset = (arm_read_cpsr(p) << 31) | (offset >> 1) ;
            } else {
                offset = ror(offset, valeur);
            }
        default:
            break;
    }
    return offset;
}






