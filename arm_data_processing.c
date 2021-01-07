/*
Armator - simulateur de jeu d'instruction ARMv5T à but pédagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique Générale GNU publiée par la Free Software
Foundation (version 2 ou bien toute autre version ultérieure choisie par vous).

Ce programme est distribué car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but spécifique. Reportez-vous à la
Licence Publique Générale GNU pour plus de détails.

Vous devez avoir reçu une copie de la Licence Publique Générale GNU en même
temps que ce programme ; si ce n'est pas le cas, écrivez à la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
États-Unis.

Contact: Guillaume.Huard@imag.fr
	 Bâtiment IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'Hères
*/
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"
#include "decode.h"

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
	uint8_t Rn = get_bits(ins, 19, 16);
	uint8_t Rd = get_bits(ins, 15, 12);
	uint32_t val_cpsr = arm_read_cpsr(p);
	int C_flag = (val_cpsr >> C) & 1;
	uint32_t value = 0;
	
	switch (ins >> 20)
	{
		case 0b0000: //AND
			value = arm_read_register(p, Rn) & shift(p,ins);
			arm_write_register(p, Rd, value);
			break;
			
		case 0b0001: //EOR
			value = arm_read_register(p, Rn) ^ shift(p,ins);
			arm_write_register(p, Rd, value);
			break;
			
		case 0b0010: //SUB
			value = arm_read_register(p, Rn) - shift(p,ins);
			arm_write_register(p, Rd, value);
			break;
		case 0b0011: //RSB
			value = shift(p,ins) - arm_read_register(p, Rn);
			arm_write_register(p, Rd, value);
			break;
			
		case 0b0100: //ADD
			value = arm_read_register(p, Rn) + shift(p,ins);
			arm_write_register(p, Rd, value);
			break;
			
		case 0b0101: //ADC
			value = arm_read_register(p, Rn) + shift(p,ins) + C_flag;
			arm_write_register(p, Rd, value);
			break;
			
		case 0b0110: //SBC
			value = arm_read_register(p, Rn) - shift(p,ins) - !C_flag;
			arm_write_register(p, Rd, value);
			break;
			
		case 0b0111: //RSC
			value = shift(p,ins) - arm_read_register(p, Rn) - !C_flag;
			arm_write_register(p, Rd, value);
			break;
			
		case 0b1000: //TST
			break;
			
		case 0b1001: //TEQ
			break;
			
		case 0b1010: //CMP
			break;
			
		case 0b1011: //CMN
			break;
			
		case 0b1100: //ORR
			value = arm_read_register(p, Rn) | shift(p,ins);
			arm_write_register(p, Rd, value);
			break;
			
		case 0b1101: //MOV
			arm_write_register(p, Rd, shift(p,ins));
			break;
			
		case 0b1110: //BIC
			value = arm_read_register(p, Rn) & !(shift(p,ins));
			arm_write_register(p, Rd, value);
			break;
			
		case 0b1111: //MVN
			arm_write_register(p, Rd, !(shift(p,ins)));
			break;
			
		default:
    		return UNDEFINED_INSTRUCTION;
    }
    return 0; //tout c'est bien passé
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
	uint8_t Rn = get_bits(ins, 19, 16);
	uint8_t Rd = get_bits(ins, 15, 12);
	uint8_t imm = get_bits(ins, 7, 0); //valeur immédiate
	uint32_t val_cpsr = arm_read_cpsr(p);
	int C_flag = (val_cpsr >> C) & 1;
	uint32_t value = 0;
	
	switch (ins >> 20)
	{
		case 0b0000: //AND
			value = arm_read_register(p, Rn) & imm;
			arm_write_register(p, Rd, value);
			break;
			
		case 0b0001: //EOR
			value = arm_read_register(p, Rn) ^ imm;
			arm_write_register(p, Rd, value);
			break;
			
		case 0b0010: //SUB
			value = arm_read_register(p, Rn) - imm;
			arm_write_register(p, Rd, value);
			break;
		case 0b0011: //RSB
			value = imm - arm_read_register(p, Rn);
			arm_write_register(p, Rd, value);
			break;
			
		case 0b0100: //ADD
			value = arm_read_register(p, Rn) + imm;
			arm_write_register(p, Rd, value);
			break;
			
		case 0b0101: //ADC
			value = arm_read_register(p, Rn) + imm + C_flag;
			arm_write_register(p, Rd, value);
			break;
			
		case 0b0110: //SBC
			value = arm_read_register(p, Rn) - imm - !C_flag;
			arm_write_register(p, Rd, value);
			break;
			
		case 0b0111: //RSC
			value = imm - arm_read_register(p, Rn) - !C_flag;
			arm_write_register(p, Rd, value);
			break;
			
		case 0b1000: //TST
			break;
			
		case 0b1001: //TEQ
			break;
			
		case 0b1010: //CMP
			break;
			
		case 0b1011: //CMN
			break;
			
		case 0b1100: //ORR
			value = arm_read_register(p, Rn) | imm;
			arm_write_register(p, Rd, value);
			break;
			
		case 0b1101: //MOV
			arm_write_register(p, Rd, imm);
			break;
			
		case 0b1110: //BIC
			value = arm_read_register(p, Rn) & !imm;
			arm_write_register(p, Rd, value);
			break;
			
		case 0b1111: //MVN
			arm_write_register(p, Rd, !imm);
			break;
			
		default:
    		return UNDEFINED_INSTRUCTION;
    }
    return 0; //tout c'est bien passé
}





