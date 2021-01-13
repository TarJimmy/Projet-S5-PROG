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
#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include "decode.h"

static int arm_execute_instruction(arm_core p) {
    uint32_t instruction;
    //Verification fetch memory
    if (arm_fetch(p, &instruction)) {
        printf("Error: Fetch memory");
    } else {
        //Verification flags
        if(condition(arm_read_cpsr(p), instruction >> 28)) {
            //Distribution de l'instruction à la bonne fonction
            uint8_t typeop = get_bits(instruction, 27, 25);
            switch (typeop) {
                case 0:
                    if (get_bit(instruction, 4) == 0 || get_bit(instruction, 7) == 0) {
                        // Data processing immediate shift or register shift
                        arm_data_processing(p, instruction);
                    } else if (get_bit(instruction, 7) == 1 && get_bit(instruction, 4) == 1) {
                        //Extra load/stores
                        arm_load_store(p, instruction);
                    }  else {
                        arm_miscellaneous(p, instruction);
                    }
                    break;
                case 1:
                    //MSR: Immediate operand
                    arm_data_processing(p, instruction);
                break;
                case 2: 
                    //Load and Store: Immediate offset/index
                    arm_load_store(p, instruction);
                    break;
                case 3 :
                    //Load and Store: Register offset/index
                    arm_load_store(p, instruction);
                    // Media instructions and Architecturally undefined
                    break;
                default:
                case 4:
                    arm_load_store_multiple(p, instruction);
                break;
                case 5:
                    //BLX (1)
                    arm_branch(p, instruction);
                break;
                case 6:
                    //Load and Store Coprocessor
                    arm_coprocessor_load_store(p, instruction);
                case 7:
                    //Exception-generating instructions
                    arm_coprocessor_others_swi(p, instruction);
                break;
            }
        }
    }
    return 0;
}

int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result)
        arm_exception(p, result);
    return result;
}
