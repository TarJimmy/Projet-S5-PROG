/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).

Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
	 B�timent IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'H�res
*/
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"
#include "decode.h"

/* Calcul du resultat des operations */
uint32_t calcul (arm_core p, uint32_t ins, uint32_t value, uint32_t* alu_out){
	uint8_t Rn = get_bits(ins, 19, 16);
	uint32_t val_cpsr = arm_read_cpsr(p);
	int C_flag = (val_cpsr >> C) & 1;
	switch (get_bits(ins, 24, 21))
	{
		case 0b0000: //AND
			return (arm_read_register(p, Rn) & value);
			break;
		
		case 0b0001: //EOR
			return (arm_read_register(p, Rn) ^ value);
			break;
			
		case 0b0010: //SUB
			return (arm_read_register(p, Rn) - value);
			break;
		case 0b0011: //RSB
			return (value - arm_read_register(p, Rn));
			break;
			
		case 0b0100: //ADD
			return (arm_read_register(p, Rn) + value);
			break;
			
		case 0b0101: //ADC
			return (arm_read_register(p, Rn) + value + C_flag);
			break;
			
		case 0b0110: //SBC
			return (arm_read_register(p, Rn) - value - !C_flag);
			break;
			
		case 0b0111: //RSC
			return (value - arm_read_register(p, Rn) - !C_flag);
			break;
				
		case 0b1000: //TST
			*alu_out = arm_read_register(p, Rn) & value;
			return UNDEFINED_INSTRUCTION;
			break;
			
		case 0b1001: //TEQ
			*alu_out = arm_read_register(p, Rn) ^ value;
			return UNDEFINED_INSTRUCTION;
			break;
			
		case 0b1010: //CMP
			*alu_out = arm_read_register(p, Rn) - value;
			return UNDEFINED_INSTRUCTION;
			break;
			
		case 0b1011: //CMN
			*alu_out = arm_read_register(p, Rn) + value;
			return UNDEFINED_INSTRUCTION;
			break;
			
		case 0b1100: //ORR
			return (arm_read_register(p, Rn) | value);
			break;
			
		case 0b1101: //MOV
			return value;
			break;
			
		case 0b1110: //BIC
			return (arm_read_register(p, Rn) & !value);
			break;
			
		case 0b1111: //MVN
			return !value;
			break;
			
		default:
			return UNDEFINED_INSTRUCTION;
	}
}

int CarryFrom(uint32_t a, uint32_t b){
	if ((a > (2^32)-1 - b) || (b > (2^32)-1 - a)) {
		return 1;
	} else {
		return 0;
	}
}

int BorrowFrom(uint32_t a, uint32_t b){
	if (a < b) {
		return 1;
	} else {
		return 0;
	}
}

int OverflowFrom(uint32_t a, uint32_t b, char op){
	if (op=='+') {
		uint32_t c = a+b;
		if ((get_bit(a,31) == get_bit(b,31)) && (get_bit(a,31) != get_bit(c,31))) {
			return 1;
		} else {
			return 0;
		}
	} else if (op=='-') {
		uint32_t c = a-b;
		if ((get_bit(a,31) != get_bit(b,31)) && (get_bit(a,31) != get_bit(c,31))) {
			return 1;
		} else {
			return 0;
		}
	} else {
		return -1;
	}
}

/* Mise a jour des flags apres operations */
void maj_flags(arm_core p, uint32_t ins, uint32_t alu_out, uint32_t shifter_carry_out){
	uint8_t Rn = get_bits(ins, 19, 16);
	uint32_t shifter_operand = get_bits(ins, 11, 0);
	uint32_t val_cpsr = arm_read_cpsr(p);
    int V_flag = (val_cpsr >> V) & 1;
    int C_flag = (val_cpsr >> C) & 1;
    int Z_flag = (val_cpsr >> Z) & 1;
    int N_flag = (val_cpsr >> N) & 1;
	uint32_t S = get_bit(ins,20);
	uint32_t Rd = get_bits(ins, 15, 12);
	switch (get_bits(ins, 24, 21)){
		case 0b1000: //TST
		case 0b1001: //TEQ
			N_flag = get_bit(alu_out,31);
			if (alu_out==0) { Z_flag = 1; } else { Z_flag = 0;}
			C_flag = shifter_carry_out;
			break;
			
		case 0b1010: //CMP
			N_flag = get_bit(alu_out,31);;
			if (alu_out==0) { Z_flag = 1; } else { Z_flag = 0;}
			C_flag = !BorrowFrom(Rn,shifter_operand);
			V_flag = OverflowFrom(Rn,shifter_operand,'-');
			break;
			
		case 0b1011: //CMN
			N_flag = get_bit(alu_out,31);;
			if (alu_out==0) { Z_flag = 1; } else { Z_flag = 0;}
			C_flag = CarryFrom(Rn,shifter_operand);
			V_flag = OverflowFrom(Rn,shifter_operand,'+');
			break;
			
		default: //autres op�rations
			if (S && Rd==15){
				if ( arm_current_mode_has_spsr(p) ){ arm_write_cpsr(p,arm_read_spsr(p)); }
			} else if (S) {
				N_flag = get_bit(arm_read_register(p,Rd),31);
				if (Rd==0) { Z_flag = 1; }
				else { Z_flag = 0; }
				switch (get_bits(ins, 24, 21))
				{
					case 0b0000: //AND
					case 0b0001: //EOR
					case 0b1100: //ORR
					case 0b1101: //MOV
					case 0b1110: //BIC
					case 0b1111: //MVN
						C_flag = shifter_carry_out;
						break;
						
					case 0b0010: //SUB
						C_flag = !BorrowFrom(Rn,shifter_operand);
						V_flag = OverflowFrom(Rn,shifter_operand,'-');
						break;
						
					case 0b0011: //RSB
						C_flag = !BorrowFrom(shifter_operand,Rn);
						V_flag = OverflowFrom(shifter_operand,Rn,'-');
						break;
						
					case 0b0100: //ADD
						C_flag = CarryFrom(Rn,shifter_operand);
						V_flag = OverflowFrom(Rn,shifter_operand,'+');
						break;
						
					case 0b0101: //ADC
						if (CarryFrom(Rn,shifter_operand)) {
							C_flag = CarryFrom(Rn,shifter_operand);
						} else { 
							C_flag = CarryFrom(Rn+shifter_operand,C_flag);
						}
						if (OverflowFrom(Rn,shifter_operand,'+')) {
							V_flag = OverflowFrom(Rn,shifter_operand,'+');
						} else { 
							V_flag = OverflowFrom(Rn+shifter_operand,C_flag,'+');
						}
						break;
						
					case 0b0110: //SBC
						if (!BorrowFrom(Rn,shifter_operand)) {
							C_flag = !BorrowFrom(Rn,shifter_operand);
						} else {
							C_flag = !BorrowFrom(Rn-shifter_operand,!C_flag);
						}
						if (OverflowFrom(Rn,shifter_operand,'-')) {
							V_flag = OverflowFrom(Rn,shifter_operand,'-');
						} else {
							V_flag = OverflowFrom(Rn-shifter_operand,!C_flag,'-');
						}
						break;
						
					case 0b0111: //RSC
						if (!BorrowFrom(shifter_operand,Rn)) {
							C_flag = !BorrowFrom(shifter_operand,Rn);
						} else {
							C_flag = !BorrowFrom(shifter_operand-Rn,!C_flag);
						}
						if (OverflowFrom(shifter_operand,Rn,'-')) {
							V_flag = OverflowFrom(shifter_operand,Rn,'-');
						} else {
							V_flag = OverflowFrom(shifter_operand-Rn,!C_flag,'-');
						}
						break;
						
					default:
						break;
				}
			}
			break;
	}
	//maj flag dans p
    if (N_flag) { val_cpsr = set_bit(val_cpsr,N); } else { val_cpsr = clr_bit(val_cpsr,N); }
    if (Z_flag) { val_cpsr = set_bit(val_cpsr,Z); } else { val_cpsr = clr_bit(val_cpsr,Z); }
    if (C_flag) { val_cpsr = set_bit(val_cpsr,C); } else { val_cpsr = clr_bit(val_cpsr,C); }
    if (V_flag) { val_cpsr = set_bit(val_cpsr,V); } else { val_cpsr = clr_bit(val_cpsr,V); }
	arm_write_cpsr(p, val_cpsr);
}

/* Decoding functions for different classes of instructions */
int arm_data_processing(arm_core p, uint32_t ins) {
	uint8_t Rd = get_bits(ins, 15, 12);
	uint8_t I = get_bit(ins, 25); //0=registre 1=valeur_immediate
	uint8_t rotate_imm = get_bits(ins, 11, 8);
	uint32_t value;
	uint32_t shifter_carry_out = 0;
	uint32_t alu_out;
	uint8_t error = condition(arm_read_cpsr(p),ins>>28);
	
	if (error) {
		if (I){
			uint32_t imm = get_bits(ins, 7, 0); //valeur immediate
			value = calcul(p, ins, imm, &alu_out);
			if (rotate_imm == 0) { shifter_carry_out = (arm_read_cpsr(p) >> C) & 1; }
			else {
				uint32_t shifter_operand = get_bits(ins, 11, 0);
				shifter_carry_out = get_bit(shifter_operand,31);
			}
		} //p446
		else { value = calcul(p, ins, shift(p,ins,&shifter_carry_out), &alu_out);}
		if (Rd!=0) { arm_write_register(p, Rd, value); }
		maj_flags(p, ins, alu_out, shifter_carry_out);
		return 0; //tout c'est bien passe
	}
	return UNDEFINED_INSTRUCTION;
}


