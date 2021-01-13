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
#include "registers.h"
#include "arm_constants.h"
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include <stdio.h>

#define MODE_READ 1
#define MODE_WRITE 0
struct registers_data {
    uint32_t regs[37];
};

int is_correct_registers(uint8_t reg) {
    return reg <= 15 && reg >= 0; 
}

registers registers_create() {
    registers r = malloc(sizeof(struct registers_data));
    return r;
}

void registers_destroy(registers r) {
    free(r);
}

uint8_t get_mode(registers r) {
    //5 premiers bits du CPSR
    //0b11111
    int32_t cpsr = read_cpsr(r);
	return get_bits(cpsr, 4, 0);
} 

int current_mode_has_spsr(registers r) {
    uint8_t mode = get_mode(r);
    return mode == FIQ || mode == IRQ || mode == SVC || mode == ABT || mode == UND;
}

int in_a_privileged_mode(registers r) {
    uint8_t mode = get_mode(r);
    return mode == FIQ || mode == IRQ || mode == SVC || mode == ABT || mode == UND || mode == SYS;
}

uint32_t read_register(registers r, uint8_t reg) {
    if (!is_correct_registers(reg)) return 0;
    return read_register_with_mode(r, reg);
}

uint32_t read_usr_register(registers r, uint8_t reg) {
    //read_register_with_mode simplifié pour usr
    if (!is_correct_registers(reg)) return 0;
    if (reg <= R12) {
        return r->regs[reg];
    } else {
        switch (reg) {
            case R13_USR_SYS:
                return r->regs[R13_USR_SYS];
                break;
            case R14_USR_SYS:
                return r->regs[R14_USR_SYS];
                break;
            case PC:
                return r->regs[PC];
                break;
            default:
                return 0;
                break;
        }
    }
}

uint32_t read_cpsr(registers r) {
    return r->regs[CPSR];
}

uint32_t read_spsr(registers r) {
    uint32_t value = 0;
    if (current_mode_has_spsr(r)) {
        read_register_with_mode(r, SPSR);
    }
    return value;
}

void write_register(registers r, uint8_t reg, uint32_t value) {
    if (is_correct_registers(reg)) {
        write_register_with_mode(r, reg, value);
    }
}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
    if (is_correct_registers(reg)) {
        write_register(r, reg, value);
    }
}

void write_cpsr(registers r, uint32_t value) {
    r->regs[CPSR] = value;
}

void write_spsr(registers r, uint32_t value) {
    if (current_mode_has_spsr(r)) {
        write_register_with_mode(r, SPSR, value);
    }
}

/**
 * is_read: read, value ignored
 * !is_read: write, value used
 * Concerne tout les registres concerné par le mode (all hors CPSR)
 */
uint32_t action_register_mode(registers r, uint8_t reg, uint32_t value, uint8_t is_read) {
    if (reg <= 7 || (get_mode(r) != FIQ && reg <= 12)) { //Registres généraux ou differents des privées de FIQ
        return read_write_register_worker(r, reg, value, is_read);
    } else if (reg <= 12 && get_mode(r) == FIQ) { //Registres généraux > 7 de FIQ
        return read_write_register_worker(r, (reg % 8 + R8_FIQ), value, is_read);
    } else if (reg == R13 || reg == R14 || reg == SPSR) { //SP, LR, SPSR
        //i = 0: R13, i = 1: R14, i = 2: SPSR
        //Pas la plus optimisé mais factorise le code
        uint8_t i = reg == 13 ? 0 : 
                    reg == 14 ? 1 : 2;
        switch(get_mode(r)) {
			case USR:
			case SYS: read_write_register_worker(r, R13_SVC + i, value, is_read);
                if (reg != 17) {
                    return read_write_register_worker(r, R13_USR_SYS + i, value, is_read);
                } else {
                    return 0;
                }
			case FIQ: return read_write_register_worker(r, R13_FIQ + i, value, is_read);    break;
			case IRQ: return read_write_register_worker(r, R13_IRQ + i, value, is_read);    break;
			case SVC: return read_write_register_worker(r, R13_SVC + i, value, is_read);    break;
			case ABT: return read_write_register_worker(r, R13_ABT + i, value, is_read);    break;
			case UND: return read_write_register_worker(r, R13_UND + i, value, is_read);    break;
			default:  return -1;                                                            break;
		}
    } else if (reg == PC) {
        return read_write_register_worker(r, reg, value, is_read);
    } else {
        //N'est pas censé ce passer
        return 0;
    }
}

void write_register_with_mode(registers r, uint8_t reg, uint32_t value) {
    action_register_mode(r, reg, value, MODE_WRITE); 
}

uint32_t read_register_with_mode(registers r, uint8_t reg) {
    return action_register_mode(r, reg, 0, MODE_READ);
}

uint32_t read_write_register_worker(registers r, uint8_t reg, uint32_t value, uint8_t is_read) {
    if (is_read) {
        return r->regs[reg];       
    } else {
        r->regs[reg] = value;
    }
    return 1;
}