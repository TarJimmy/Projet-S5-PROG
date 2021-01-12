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

int condition(uint32_t cpsr, uint8_t cond) {
    int V_flag = (cpsr >> V) & 1;
    int C_flag = (cpsr >> C) & 1;
    int Z_flag = (cpsr >> Z) & 1;
    int N_flag = (cpsr >> N) & 1;
    switch (cond) {
        case 0:  return Z_flag;                         break;   // EQ       
        case 1:  return !Z_flag;                        break;   // NE    
        case 2:  return C_flag;                         break;   // CS/HS
        case 3:  return !C_flag;                        break;   // CC/LO
        case 4:  return N_flag;                         break;   // MI
        case 5:  return !N_flag;                        break;   // PL
        case 6:  return V_flag;                         break;   // VS
        case 7:  return !V_flag;                        break;   // VC
        case 8:  return C_flag || Z_flag;               break;   // HI
        case 9:  return !C_flag || Z_flag;              break;   // LS
        case 10: return N_flag == V_flag;               break;   // GE
        case 11: return N_flag != V_flag;               break;   // LT
        case 12: return !Z_flag && (N_flag == V_flag);  break;   // GT
        case 13: return Z_flag || (N_flag != V_flag);   break;   // LE
        case 14: return 1;                              break;   // AL
        default: return 0;                              break;
    }
}