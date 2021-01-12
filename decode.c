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
			


uint32_t shift(arm_core p, uint32_t instruction, uint32_t* shifter_carry_out){
	uint32_t shift = get_bits(instruction, 6, 5);
    uint32_t offset = get_bits(instruction, 3, 0); //shifter_operand = Rm
    uint32_t bit_4 = get_bit(instruction, 4); //(shift) 0=valeur_immédiate 1=registre
    uint32_t valeur = 0;
    if (bit_4) {  
    	uint8_t Rs = get_bits(instruction, 11, 8);
    	valeur = arm_read_register(p, Rs);
    	uint32_t Rs_7_0 = get_bits(valeur,7,0);
    	uint32_t Rs_4_0 = get_bits(valeur,4,0);
    	switch (shift) {
		    case 0: //LSL p449
		    	if (Rs_7_0 == 0) {
				  *shifter_carry_out = (arm_read_cpsr(p) >> C) & 1;
				} else if (Rs_7_0 < 32) {
				  offset = offset << Rs_7_0;
				  *shifter_carry_out = get_bit(arm_read_register(p, offset), (32 - Rs_7_0) );
				} else if (Rs_7_0 == 32) {
				  offset = 0;
				  *shifter_carry_out = get_bit(arm_read_register(p, offset), 0);
				} else {
				  offset = 0;
				  *shifter_carry_out = 0;
				}
		        break;
		    case 1: //LSR
		        if (Rs_7_0 == 0) {
				  *shifter_carry_out = (arm_read_cpsr(p) >> C) & 1;
				} else if (Rs_7_0 < 32) {
				  offset = offset >> Rs_7_0;
				  *shifter_carry_out = get_bit(arm_read_register(p, offset), (Rs_7_0 - 1) );
				} else if (Rs_7_0 == 32) {
				  offset = 0;
				  *shifter_carry_out = get_bit(arm_read_register(p, offset), 31);
				} else {
				  offset = 0;
				  *shifter_carry_out = 0;
				}
		        break;
		    case 2: //ASR
		        if (Rs_7_0 == 0) {
		        	*shifter_carry_out = (arm_read_cpsr(p) >> C) & 1;
		        } else if (Rs_7_0 < 32) {
		        	offset = asr(offset, Rs_7_0);
		        	*shifter_carry_out = get_bit(offset, Rs_7_0-1);
		        } else {
		        	if (!get_bit(arm_read_register(p, offset), 31)) {
		        		offset = 0;
		        	} else {
		        		offset = 0xFFFFFFFF;
		        	}
		        	*shifter_carry_out = get_bit(arm_read_register(p, offset),31);
		        }
		        break;
		    case 3: //ROR
		        if (Rs_7_0 == 0) {
		        	*shifter_carry_out = (arm_read_cpsr(p) >> C) & 1;
		        } else if (Rs_4_0==0) {
		        	*shifter_carry_out = get_bit(arm_read_register(p, offset),31);
		        } else {
		        	offset = ror(offset, Rs_4_0);
		        	*shifter_carry_out = get_bit(arm_read_register(p, offset),Rs_4_0-1);
		        }
		        break;
		    default:
		        break;
    	}
    } else { 
    	valeur = get_bits(instruction, 11, 7);
    	switch (shift) {
		    case 0: //LSL p449
		    	*shifter_carry_out = (arm_read_cpsr(p) >> C) & 1; // = C Flag
		        if (valeur!=0) {
		        	offset = offset << valeur;
		        	*shifter_carry_out = get_bit(arm_read_register(p, offset),32-valeur);
		        }
		        break;
		    case 1: //LSR
		        if (valeur == 0) {
		            offset = 0;
		            *shifter_carry_out = get_bit(arm_read_register(p, offset),31);
		        } else {
		            offset = offset >> valeur;
		            *shifter_carry_out = get_bit(arm_read_register(p, offset),valeur-1);
		        }
		        break;
		    case 2: //ASR
		        if (valeur == 0) {
		            if (!get_bit(arm_read_register(p, offset),31)) {
		            	offset = 0;
		            } else {
		            	offset = 0xFFFFFFFF;
		            }
		            *shifter_carry_out = get_bit(arm_read_register(p, offset),31);
		        } else {
		            offset = asr(offset, valeur);
		            *shifter_carry_out = get_bit(arm_read_register(p, offset),valeur-1);
		        }
		        break;
		    case 3: //ROR
		        if (valeur ==0) {
		            offset = (((arm_read_cpsr(p) >> C) & 1) << 31) | (offset >> 1) ;
		            *shifter_carry_out = get_bit(arm_read_register(p, offset),0);
		        } else {
		            offset = ror(offset, valeur);
		            *shifter_carry_out = get_bit(arm_read_register(p, offset),valeur-1);
		        }
		    default:
		        break;
    	} 
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
