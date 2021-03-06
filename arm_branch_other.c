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
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include <debug.h>
#include <stdlib.h>

int32_t signed_extend30(uint32_t ins) {
    int32_t mod = get_bits(ins, 23, 0);
    if (get_bit(ins,23)){
        int32_t extension = 0xb111111; 
        return (extension << 23) | mod;
    } else {
        return mod;
    }
}
 
int arm_branch(arm_core p, uint32_t ins) {
    if(get_bits(ins, 31, 28) != 0xF){
    // B, BL
        if(get_bit(ins, 24)){
            arm_write_register(p, 14, arm_read_register(p, 15) - 4);
        }
        arm_write_register(p, 15, arm_read_register(p, 15) + (signed_extend30(ins) << 2));
        return 0;
    }
    return -1;
}
 
int arm_coprocessor_others_swi(arm_core p, uint32_t ins) {
    if (get_bit(ins, 24)) {
        /* Here we implement the end of the simulation as swi 0x123456 */
        if ((ins & 0xFFFFFF) == 0x123456)
            exit(0);
        return SOFTWARE_INTERRUPT;
    } 
    return UNDEFINED_INSTRUCTION;
}
 
int arm_miscellaneous(arm_core p, uint32_t ins) {
    // MRS
    if(get_bits(ins, 21, 20) == 0b00) {
        int rd = get_bits(ins, 15, 12);
        if (get_bit(ins,22)) {
            arm_write_register(p, rd, arm_read_spsr(p));
        } else {
            arm_write_register(p, rd, arm_read_cpsr(p));
        }
    }
    return 0;
}
