
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
#ifndef __ARM_LOAD_STORE_H__
#define __ARM_LOAD_STORE_H__
#include <stdint.h>
#include "arm_core.h"

int arm_load_store(arm_core p, uint32_t ins);
int arm_load_store_multiple(arm_core p, uint32_t ins);
int arm_coprocessor_load_store(arm_core p, uint32_t ins);

//Function Utile

//Calculate adress associate at U_bit
uint32_t handling_offset(uint32_t address,int offset,uint8_t U_bit);

//Call function associate at W_bit
//(W_bit == 0) normal memory acces
//(W_bit == 1) unprivileged (User mode) memory access 
void write_register_mode(arm_core p, uint8_t reg, uint32_t value, uint8_t W_bit);
uint32_t read_register_mode(arm_core p, uint8_t reg, uint8_t W_bit);

int word_byte_load_store(arm_core p, uint8_t I_bit, uint8_t P_bit, uint8_t U_bit, uint8_t B_bit, uint8_t W_bit, uint8_t L_bit, uint8_t rn, uint8_t rd, uint16_t data, uint8_t cond);

int miscellaneous_load_store(arm_core p, uint8_t I_bit, uint8_t P_bit, uint8_t U_bit, uint8_t B_bit, uint8_t W_bit, uint8_t L_bit, uint8_t rn, uint8_t rd, uint16_t data, uint8_t cond);

int load_store_half_double_byte (arm_core p, uint8_t rd, uint32_t address, uint8_t L_bit, uint8_t S_bit, uint8_t H_bit, uint8_t cond);
#endif
